#include <stdio.h>
#include "tests.h"

int main() {
    if (tests_run()) {
        printf("tests completed");
    } else {
        printf("tests failed");
    }

    return 0;
}
