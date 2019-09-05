/*
 * algorithm_test.c
 *
 * This program is to test the new way of estimating division results proposed in
 * https://reviews.llvm.org/D66050. It compares four types of estimation with
 * real division instruction:
 * 
 * 1. Multiply raw reciprocal (use native instruction)
 * 2. Multiply reciprocal with 1 iteration
 * 3. Multiply reciprocal with 2 iterations
 * 4. Put multiplication into iteration
 *
 * It's expected that 4 is better than all above in accuracy.
 *
 * Note:
 * 
 * - Please DO NOT turn on -Ofast when compiling this file.
 * - PowerPC target supports partial X86 intrinsics functions, try turning on
 *   "-DNO_WARN_X86_INTRINSICS" if such error arises.
 * - Change the enumeration steps to fit your need.
 *
 * This file originates from https://bugs.llvm.org/show_bug.cgi?id=21385
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__ppc64__) && defined(__linux__)
#define NO_WARN_X86_INTRINSICS
#endif

#include <xmmintrin.h>

// Native division
float div_ieee(float x, float y) { return x / y; }

// Multiply native reciprocal instruction result without iterations
float div_est(float x, float y) {
  float est;
  __m128 vecY = {y, 0, 0, 0};
  __m128 vecRcp = _mm_rcp_ss(vecY);
  _mm_store_ss(&est, vecRcp);
  est *= x;
  return est;
}

// Multiply native reciprocal result with 1 iteration
float div_est_NR1(float x, float y) {
  float est;
  __m128 vecY = {y, 0, 0, 0};
  __m128 vecRcp = _mm_rcp_ss(vecY);
  _mm_store_ss(&est, vecRcp);
  est = est + est * (1 - est * y);
  est *= x;
  return est;
}

// Multiply native reciprocal result with 2 iterations
float div_est_NR2(float x, float y) {
  float est;
  __m128 vecY = {y, 0, 0, 0};
  __m128 vecRcp = _mm_rcp_ss(vecY);
  _mm_store_ss(&est, vecRcp);
  est = est + est * (1 - est * y);
  est = est + est * (1 - est * y);
  est *= x;
  return est;
}

// Multiply first and do iteration for once
float div_est_new(float x, float y) {
  float est;
  __m128 vecY = {y, 0, 0, 0};
  __m128 vecRcp = _mm_rcp_ss(vecY);
  _mm_store_ss(&est, vecRcp);
  est = est + est * (1 - est * y);
  float estMul = est * x;
  est = estMul + est * (x - estMul * y);
  return est;
}

float ulp_s(float y) {
  union {
    float f;
    int i;
  } a, u1, u2;
  a.f = y;
  u1.i = a.i & 0x7F800000;
  u2.i = u1.i | 0x00000001;
  return (u2.f - u1.f);
}

double err(float a, double b) {
  double ulp = (double)ulp_s(a);
  double ad = (double)a;
  if (ad > b) {
    return (ad - b) / ulp;
  } else {
    return (b - ad) / ulp;
  }
}

int get_next() {
  static int group[] = { 1, 4, 7, 10, 100, 1000, 3000, 10000, 20000, 30000 };
  static int n = 0;
  if (n > 8) n = 0;
  return group[n++];
}

int main() {
  int i, j;
  unsigned long long tests = 0;
  unsigned long long not_exact_ORI = 0;
  unsigned long long not_exact_NR1 = 0;
  unsigned long long not_exact_NR2 = 0;
  unsigned long long not_exact_NEW = 0;
  unsigned long long bad_ORI = 0;
  unsigned long long bad_NR1 = 0;
  unsigned long long bad_NR2 = 0;
  unsigned long long bad_NEW = 0;

  // ignore denorm inputs (anything less than 0x00800000)
  // ignore denorm outputs (any input over 0x7e800000)
  // ignore inf (0x7f800000)
  // ignore nans (anything more than 0x7f800000)
  // ignore negative (anything more than 0x80000000)
 
  // These numbers aren't special but prime numbers in a suitable range.
  // Small steps may consume a lot of running time but provides better result.
  const int outer_step = 5471;
  const int inner_step = 3697;

  for (i = 0x00800000; i < 0x7E800000; i += outer_step) {
    for (j = 0x00800000; j < 0x7E800000; j += inner_step) {
      float f, d, ieee, est, est_nr1, est_nr2, est_new;
      int ieee_int, est_int, est_nr1_int, est_nr2_int, est_new_int;

      tests++;
      memcpy(&f, &i, 4);
      memcpy(&d, &j, 4);
      ieee = div_ieee(f, d);
      est = div_est(f, d);
      est_nr1 = div_est_NR1(f, d);
      est_nr2 = div_est_NR2(f, d);
      est_new = div_est_new(f, d);
      memcpy(&ieee_int, &ieee, 4);
      memcpy(&est_int, &est, 4);
      memcpy(&est_nr1_int, &est_nr1, 4);
      memcpy(&est_nr2_int, &est_nr2, 4);
      memcpy(&est_new_int, &est_new, 4);

      if (err(est, ieee) > 0.5)
          ++bad_ORI;

      if (err(est_nr1, ieee) > 0.5)
          ++bad_NR1;

      if (err(est_nr2, ieee) > 0.5)
          ++bad_NR2;

      if (err(est_new, ieee) > 0.5)
          ++bad_NEW;
    }
  }
  printf("Total tests = %llu\n", tests);
  printf("Accurate numbers in OLD way: %llu, accuracy ratio: %.4lf", (tests - bad_NR1), (double)(tests - bad_NR1) / tests);
  printf("Accurate numbers in NEW way: %llu, accuracy ratio: %.4lf", (tests - bad_NEW), (double)(tests - bad_NEW) / tests);
	return 0;
}
