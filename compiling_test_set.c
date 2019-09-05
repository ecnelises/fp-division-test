/*
 * compiling_test_set.c
 *
 * This program is to test accuracy of division estimation between Clang and
 * GCC. It uses a set of random floating point numbers.
 *
 * Thanks to Masoud Ataei Jaliseh for contributing test.
 */

#include <stdio.h>
#include <stdlib.h>

extern float fdiv_clg(float, float);
extern float fdiv_gcc(float, float);

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
  union {
    unsigned int i;
    float f;
  } inputs[] = {
#define DEFINE_INPUTS
#include "./inputs.h"
  };

  double err_gcc, err_clg, z;
  float x, y;
  int i, len, res = 0;
  len = sizeof(inputs) / sizeof(inputs[0]);

  for (i = 0; i < len / 2; i++) {
    z = (double)inputs[2 * i].f / (double)inputs[2 * i + 1].f;
    x = fdiv_gcc(inputs[2 * i].f, inputs[2 * i + 1].f);
    y = fdiv_clg(inputs[2 * i].f, inputs[2 * i + 1].f);

    err_gcc = err(x, z);
    err_clg = err(y, z);
    if (err_clg - err_gcc < 0.5)
      res += 1;
  }

  printf("Accurate rate: %d/%d\n", res, len / 2);

  return 0;
}
