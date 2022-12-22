#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static struct {
  const char *test, *file, *assert;
  int result, line;
} report;

#define suite(specs...) \
int main() { \
  int total = 0, failures = 0; \
  void (*tests[])() = { specs, NULL }; \
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
#define fail(assertion) \
do { \
  report.result = assertion == 0; \
  report.test = __func__; \
  report.file = __FILE__; \
  report.line = __LINE__; \
  report.assert = assertion; \
} while (0);

#define assert(assertion) \
do { \
  if ((assertion) == 0) { \
    fail(#assertion); \
    return; \
  } \
  else success; \
} while (0);

#endif
