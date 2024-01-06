#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#include <stdint.h>

#define MAX_DIGITS 256
#define RADIX 16

typedef struct {
    uint32_t digits[MAX_DIGITS];
    int length;
} BigNumber;

void initializeBigNumberFromHex(BigNumber *num, const char *hexString);
BigNumber* add(BigNumber *a, BigNumber *b);
BigNumber* multiply(BigNumber *a, BigNumber *b);
BigNumber* montgomeryMultiply(BigNumber *a, BigNumber *b, BigNumber *modulus);
BigNumber* squareAndMultiply(BigNumber *base, BigNumber *exponent, BigNumber *modulus);
void printBigNumber(BigNumber *num);

#endif  // BIGNUMBER_H

