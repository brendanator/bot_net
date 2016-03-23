#include <stdio.h>

#define RESET "\033[0m"
#define RED   "\033[31m"
#define GREEN "\033[32m"

int tests_run = 0;
int tests_failed = 0;

#define mu_assert(test) do { if (!(test)) return #test; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; if (message) { printf(RED "  %s\n" RESET, message); tests_failed++; } } while (0)
#define mu_init() do { tests_run = 0; tests_failed = 0; printf(GREEN "Testing "__FILE__ "\n" RESET); } while (0)

int mu_report() {
  if (tests_failed) {
    printf(RED "FAILED: Total %d, Passed %d, Failed %d\n\n" RESET, tests_run, tests_run-tests_failed, tests_failed);
  } else {
    printf(GREEN "PASSED: Total %d, Passed %d, Failed %d\n\n" RESET, tests_run, tests_run-tests_failed, tests_failed);
  }  
  return tests_failed != 0;
}
