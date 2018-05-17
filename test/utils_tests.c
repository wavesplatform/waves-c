#include "utils_tests.h"
#include "utils.h"

void print_amount_test_1() {
    unsigned char expected[] = "0.00000001";
    unsigned char amount_str[100];
    bool result = print_amount(1, 8, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 1 failed\n");
        exit(-1);
    }
}

void print_amount_test_2() {
    unsigned char expected[] = "0.001";
    unsigned char amount_str[100];
    bool result = print_amount(100000, 8, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 2 failed\n");
        exit(-1);
    }
}

void print_amount_test_3() {
    unsigned char expected[] = "100";
    unsigned char amount_str[100];
    bool result = print_amount(10000000000, 8, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 3 failed\n");
        exit(-1);
    }
}

void print_amount_test_4() {
    unsigned char expected[] = "100000000";
    unsigned char amount_str[100];
    bool result = print_amount(10000000000000000, 8, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 4 failed\n");
        exit(-1);
    }
}

void print_amount_test_5() {
    unsigned char expected[] = "10000";
    unsigned char amount_str[100];
    bool result = print_amount(10000, 0, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 5 failed\n");
        exit(-1);
    }
}

void print_amount_test_6() {
    unsigned char expected[] = "1";
    unsigned char amount_str[100];
    bool result = print_amount(1, 0, amount_str, sizeof(amount_str));
    result &= strcmp(expected, amount_str) == 0;
    if (!result) {
        printf("print_amount_test 6 failed\n");
        exit(-1);
    }
}