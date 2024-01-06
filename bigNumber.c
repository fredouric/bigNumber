#include "bigNumber.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void initializeBigNumberFromHex(BigNumber *num, const char *hexString) {
    memset(num->digits, 0, sizeof(num->digits));
    num->length = 0;

    int hexLen = strlen(hexString);

    for (int i = hexLen - 1; i >= 0; i--) {
        char hexChar = hexString[i];

        int digit;
        if (hexChar >= '0' && hexChar <= '9') {
            digit = hexChar - '0';
        } else if (hexChar >= 'A' && hexChar <= 'F') {
            digit = hexChar - 'A' + 10;
        } else if (hexChar >= 'a' && hexChar <= 'f') {
            digit = hexChar - 'a' + 10;
        } else {
            fprintf(stderr, "Invalid hex character: %c\n", hexChar);
            exit(EXIT_FAILURE);
        }
        num->digits[num->length++] = digit;
    }

    if (num->length == 0) {
        num->length = 1;
    }
}

BigNumber* add(BigNumber *a, BigNumber *b) {
    BigNumber *result = (BigNumber*)malloc(sizeof(BigNumber));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    int carry = 0;
    for (int i = 0; i < MAX_DIGITS; i++) {
        int sum = a->digits[i] + b->digits[i] + carry;
        result->digits[i] = sum % 16;
        carry = sum / 16;
    }

    result->length = MAX_DIGITS;
    while (result->length > 1 && result->digits[result->length - 1] == 0) {
        result->length--;
    }

    return result;
}

BigNumber* multiply(BigNumber *a, BigNumber *b) {
    BigNumber *result = (BigNumber*)malloc(sizeof(BigNumber));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    memset(result->digits, 0, sizeof(result->digits));
    result->length = 0;

    for (int i = 0; i < a->length; i++) {
        int carry = 0;

        for (int j = 0; j < b->length || carry > 0; j++) {
            int product = result->digits[i + j] + a->digits[i] * (j < b->length ? b->digits[j] : 0) + carry;
            result->digits[i + j] = product % 16;
            carry = product / 16;

            if (i + j >= result->length) {
                result->length = i + j + 1;
            }
        }
    }

    while (result->length > 1 && result->digits[result->length - 1] == 0) {
        result->length--;
    }

    return result;
}

BigNumber* montgomeryMultiply(BigNumber *a, BigNumber *b, BigNumber *modulus) {
    BigNumber *result = (BigNumber*)malloc(sizeof(BigNumber));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    memset(result->digits, 0, sizeof(result->digits));
    result->length = 0;

    int n = modulus->length;

    for (int i = 0; i < n; i++) {
        // Calculate ai
        uint32_t ai = a->digits[i];

        // Calculate term = ai * b mod RADIX
        uint32_t term = 0;
        for (int j = 0; j < n; j++) {
            term += ai * b->digits[j];
            term %= RADIX;
        }

        // Calculate c = result + term * modulus mod RADIX
        uint32_t carry = term;
        for (int j = 0; j < n; j++) {
            uint32_t temp = result->digits[i + j] + carry + term * modulus->digits[j];
            result->digits[i + j] = temp % RADIX;
            carry = temp / RADIX;
        }

        // Propagate the carry
        for (int j = n; carry > 0 && j < n * 2; j++) {
            uint32_t temp = result->digits[i + j] + carry;
            result->digits[i + j] = temp % RADIX;
            carry = temp / RADIX;
        }
    }

    // Reduce result by modulus
    for (int i = n; i < n * 2; i++) {
        uint32_t quotient = result->digits[i] / RADIX;
        uint32_t remainder = result->digits[i] % RADIX;

        // Update result
        result->digits[i] = remainder;

        // Subtract quotient * modulus from the next digit
        if (i + 1 < n * 2) {
            result->digits[i + 1] -= quotient * modulus->digits[i - n + 1];
        }
    }

    // Trim leading zeros
    result->length = n;
    while (result->length > 1 && result->digits[result->length - 1] == 0) {
        result->length--;
    }

    return result;
}

BigNumber* squareAndMultiply(BigNumber *base, BigNumber *exponent, BigNumber *modulus) {
    BigNumber *result = (BigNumber*)malloc(sizeof(BigNumber));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Initialize result to 1
    memset(result->digits, 0, sizeof(result->digits));
    result->digits[0] = 1;
    result->length = 1;

    // Square-and-multiply algorithm
    for (int i = exponent->length - 1; i >= 0; i--) {
        uint32_t currentBit = exponent->digits[i];

        // Square
        result = montgomeryMultiply(result, result, modulus);

        // Multiply if the current bit is 1
        if (currentBit != 0) {
            result = montgomeryMultiply(result, base, modulus);
        }
    }

    return result;
}

void printBigNumber(BigNumber *num) {
    for (int i = num->length - 1; i >= 0; i--) {
        printf("%d ", num->digits[i]);
    }
    printf("\n");
}

int main(void) {
    BigNumber num1, num2, modulus;

    initializeBigNumberFromHex(&num1, "FFFFFFF");
    printf("1st number: ");
    printf("Length: %d\n", num1.length);
    printBigNumber(&num1);

    initializeBigNumberFromHex(&num2, "1");
    printf("2nd number: ");
    printf("Length: %d\n", num2.length);
    printBigNumber(&num2);

    initializeBigNumberFromHex(&modulus, "FFFFFFF");
    printf("Modulus: ");
    printf("Length: %d\n", modulus.length);
    printBigNumber(&modulus);

    // Addition
    BigNumber *sum = add(&num1, &num2);
    printf("Addition: ");
    printf("Length: %d\n", sum->length);
    printBigNumber(sum);

    // Multiplication
    BigNumber *product = multiply(&num1, &num2);
    printf("Multiplication: ");
    printf("Length: %d\n", product->length);
    printBigNumber(product);

    // Montgomery Multiplication
    BigNumber *montgomeryResult = montgomeryMultiply(&num1, &num2, &modulus);
    printf("Montgomery Multiplication: ");
    printf("Length: %d\n", montgomeryResult->length);
    printBigNumber(montgomeryResult);

    BigNumber *exponentiation = squareAndMultiply(&num1, &num2, &modulus);
    printf("Modular Exponentiation: ");
    printf("Length: %d\n", exponentiation->length);
    printBigNumber(exponentiation);

    return 0;
}

