#include "test_utils.h"


extern inline void run_test_group(const char * name, const test_in_group * tests, size_t amount);
extern inline void base_mmap_checks(void * addr, size_t length, int prot, int flags, int fd, off_t offset);

void print_mmap_call(void * addr, size_t length, int prot, int flags, int fd, off_t offset) {
    fputs("mmap(addr = ", stdout);

    if (addr) {
        printf("%p", addr);
    } else {
        fputs("NULL", stdout);
    }

    printf(", length = %zu, prot = ", length);

    if (prot == PROT_NONE) {
        fputs("PROT_NONE", stdout);
    } else {
        int amount = 0;

        for (int mask = 1; mask < 4; mask <<= 1) {
            if ((prot & mask) != 0) {
                ++amount;
            }
        }

        for (int i = 0, mask = 1; i < amount; ++i, mask <<= 1) {
            if ((prot & mask) == 0) {
                --i;
                continue;
            }

            if (i > 0) {
                fputs(" | ", stdout);
            }

            const char * prot_str = NULL;
            switch (mask) {
                case PROT_READ:
                prot_str = "PROT_READ";
                break;

                case PROT_WRITE:
                prot_str = "PROT_WRITE";
                break;

                case PROT_EXEC:
                prot_str = "PROT_EXEC";
                break;
            }

            printf("%s", prot_str);
        }
    }

    fputs(", flags =", stdout);

    if (flags == 0) {
        fputs("0", stdout);
    } else {
        int amount = 0;

        for (int mask = 1; mask > 0; mask <<= 1) {
            if ((flags & mask) != 0) {
                ++amount;
            }
        }

        for (int i = 0, mask = 1; i < amount; ++i, mask <<= 1) {
            if ((flags & mask) == 0) {
                --i;
                continue;
            }

            if (i > 0) {
                fputs(" |", stdout);
            }

            const char * flags_str = NULL;
            switch (mask) {
                case MAP_SHARED:
                flags_str = "MAP_SHARED";
                break;

                case MAP_PRIVATE:
                flags_str = "MAP_PRIVATE";
                break;

                case MAP_32BIT:
                flags_str = "MAP_32BIT";
                break;

                case MAP_ANONYMOUS:
                flags_str = "MAP_ANONYMOUS";
                break;

                case MAP_DENYWRITE:
                flags_str = "MAP_DENYWRITE";
                break;

                case MAP_EXECUTABLE:
                flags_str = "MAP_EXECUTABLE";
                break;

                case MAP_FILE:
                flags_str = "MAP_FILE";
                break;

                case MAP_FIXED:
                flags_str = "MAP_FIXED";
                break;

                case MAP_FIXED_NOREPLACE:
                flags_str = "MAP_FIXED_NOREPLACE";
                break;

                case MAP_GROWSDOWN:
                flags_str = "MAP_GROWSDOWN";
                break;

                case MAP_HUGETLB:
                flags_str = "MAP_HUGETLB";
                break;

                case MAP_LOCKED:
                flags_str = "MAP_LOCKED";
                break;

                case MAP_NONBLOCK:
                flags_str = "MAP_NONBLOCK";
                break;

                case MAP_NORESERVE:
                flags_str = "MAP_NORESERVE";
                break;

                case MAP_POPULATE:
                flags_str = "MAP_POPULATE";
                break;

                case MAP_STACK:
                flags_str = "MAP_STACK";
                break;

                case MAP_SYNC:
                flags_str = "MAP_SYNC";
                break;
            }

            printf(" %s", flags_str);
        }
    }

    printf(", fd = %d, offset = %zu) -> ...\n", fd, offset);
}

void print_mmap_result(void * retval) {
    if (retval == MAP_FAILED) {
        puts("mmap(...) -> MAP_FAILED");
    } else if (retval) {
        printf("mmap(...) -> %p\n", retval);
    } else {
        puts("mmap(...) -> NULL");
    }
}
