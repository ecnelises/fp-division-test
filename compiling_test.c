/*
 * compiling_test.c
 *
 * This program is to test accuracy of division estimation in different
 * optimization options of Clang. It enumerates a big range of floating point
 * numbers.
 *
 * Thanks to Masoud Ataei Jaliseh for contributing test.
 */

#include <stdio.h>
#include <stdlib.h>

extern float fdiv_clg(float, float);
extern float fdiv_clg_norecip(float, float);

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

int main() {
  double err_clg_norecip, err_clg, z;
  float x, y;
  int res = 0;
  unsigned long cnt = 0;
  union { unsigned int i; float f; } u1, u2;

  // These numbers aren't special but prime numbers in a suitable range.
  // Small steps may consume a lot of running time but provides better result.
  const unsigned outer_step = 5471;
  const unsigned inner_step = 3697;

  for (unsigned m = 0x00800000; m < 0x7E800000; m += outer_step) {
    for (unsigned n = 0x00800000; n < 0x7E800000; n += inner_step) {
      ++cnt;
      u1.i = m;
      u2.i = n;
      z = (double)u1.f / (double)u2.f;
      x = fdiv_clg_norecip(u1.f, u2.f);
      y = fdiv_clg(u1.f, u2.f);
      err_clg_norecip = err(x, z);
      err_clg = err(y, z);
      if (err_clg - err_clg_norecip < 0.5)
        res += 1;
    }
  }

  printf("Accurate rate: %d/%lu\n", res, cnt);
  return 0;
}
