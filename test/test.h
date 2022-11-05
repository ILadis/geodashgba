#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static struct {
  const char *test, *file, *assert;
  int result, line;
} report;

#define suite(...) \
int main() { \
  int total = 0, failures = 0; \
  void (*tests[])() = { __VA_ARGS__, NULL }; \
  printf("\n"); \
  for (int i = 0; tests[i] != NULL; i++) { \
    tests[i](); \
    total++; \
    if (report.result) { \
      printf("[PASS] %s\n", report.test); \
    } else { \
      failures++; \
      printf("[FAIL] %s\n       - %s:%d: %s\n", report.test, report.file, report.line, report.assert); \
    } \
  } \
  printf("\n%d test(s), %d failure(s)\n", total, failures); \
  return 0; \
}

#define test(name) \
static void name()

#define success fail(0)
#define fail(test) \
do { \
  report.result = test == 0; \
  report.te##st = __func__; \
  report.file = __FILE__; \
  report.line = __LINE__; \
  report.assert = test; \
} while (0);

#define assert(test) \
do { \
  if ((test) == 0) { \
    fail(#test); \
    return; \
  } \
  else success; \
} while (0);

#endif
