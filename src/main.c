#include "tests.h"
#include <stdio.h>

int main() {
    if (tests_run()) {
        printf("tests completed");
        return 0;
    } 
    printf("tests failed");
    return 1;
}
