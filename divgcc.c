__attribute__((noinline))
float fdiv_gcc(float a, float b) {
  return a / b;
}

__attribute__((noinline))
float fdiv_clg_norecip(float a, float b) {
  return a / b;
}
