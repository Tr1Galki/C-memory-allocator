#include "tests.h"
#include <stdio.h>

int main() {
    if (tests_run()) {
        printf("tests completed");
    } else {
        printf("tests failed");
    }

    return 0;
}
