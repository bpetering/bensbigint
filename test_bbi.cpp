#include "unittest-cpp/src/UnitTest++.h"
#include "bbi.h"

#include <iostream>
using namespace std;

TEST(Sanity)
{
   CHECK_EQUAL(1, 1);
}

TEST(CreateAssign) {
    bool junk;
    BigInt b = 1;
    BigInt b2 (2);
    BigInt b3 ("3");
    BigInt b4;
    BigInt *b5 = new BigInt;            junk = b5->is_zero(); if (junk) {}
    BigInt *b6 = new BigInt (1);        junk = b6->is_zero(); if (junk) {}
    BigInt *b7 = new BigInt ("123");    junk = b7->is_zero(); if (junk) {}

    BigInt b9 = b;
    BigInt b10 = b2;
    BigInt b11 = b3;
    BigInt b12 = b4;
    BigInt b13 = *b5;
    BigInt b14 = *b6;
    BigInt b15 = *b7;

    BigInt b16 (b);
    BigInt b17 (b2);
    BigInt b18 (b3);
    BigInt b19 (b4);
}

TEST(MethodIsZero) {
    BigInt b = 0;
    CHECK(b.is_zero());
    BigInt c = 1;
    CHECK(!c.is_zero());

    BigInt b2 (0);
    CHECK(b2.is_zero());
    BigInt c2 (1);
    CHECK(!c2.is_zero());

    BigInt b3 ("0");
    CHECK(b3.is_zero());
    BigInt c3 ("1");
    CHECK(!c3.is_zero());

    BigInt c4 ("12345");
    CHECK(!c4.is_zero());
}

// TEST(MethodGetBit) {

// }

// TEST(CompareEq) {
//     BigInt b1 = 5;
//     BigInt b2 = 5;
//     CHECK(b1 == 5);
//     CHECK(b1 == b2);
//     CHECK(b2 == b1);
//     BigInt b3 = 6;
//     CHECK(b3 == 6);
//     CHECK(b3 != b2);
//     CHECK(b2 != b3);
//     CHECK(b3 != 99);
// }

// TEST(CompareOther) {
//     BigInt b1 = 5;
//     BigInt b2 = 5;
//     CHECK(b1 <= b2);
//     CHECK(b2 <= b1);
//     CHECK(b1 >= b2);
//     CHECK(b2 >= b1);
//     BigInt b3 = 6;
//     CHECK(b1 <= b3);
//     CHECK(b1 < b3);
//     CHECK(b3 >= b1);
//     CHECK(b3 > b1);
// }

// TEST(AddAssign) {
//     BigInt b = 5;
//     BigInt c = 7;
//     b += c;
//     CHECK(b == 12);
//     BigInt b2 = 100;
//     BigInt c2 = 200;
//     b2 += c2;
//     CHECK(b2 == 300);
//     BigInt b3 = 200;
//     BigInt c3 = 100;
//     b3 += c3;
//     CHECK(b3 == 300);
// }

// TEST(MultAssign) {
//     BigInt b = 5;
//     BigInt c = 7;
//     b *= c;
//     CHECK(b == 35);
//     BigInt b2 = 42;
//     BigInt c2 = 10;
//     b2 *= c2;
//     CHECK(b2 == 420);
// }

// TEST(BitwiseNot) {

// }

// TEST(LogicalNot) {
//     BigInt a = 0;
//     CHECK(!a);
//     BigInt b = 1;
//     CHECK(b);
//     CHECK(!!b);
//     BigInt c = 42;
//     CHECK(c);
//     CHECK(!!c);
// }

// TEST(LeftShiftAssign) {
//     BigInt b = 1;
//     b <<= 1;
//     CHECK(b == 2);
//     b <<= 1;
//     CHECK(b == 4);
//     b <<= 2;
//     CHECK(b == 16);

//     BigInt n = 128;
//     n <<= 1;
//     CHECK(n == 256);

//     BigInt m = 32768;
//     m <<= 1;
//     CHECK(m == 65536);
// }

// TEST(AndAssign) {
//     BigInt b = 1;

// }

// TEST(Copy) {
//     BigInt b = 5;
//     BigInt c = b;
//     b <<= 1;
//     CHECK(b == 10);
//     CHECK(c == 5);
// }

// TEST(General) {
//     {
//         BitInt b ("123451234512345");
//         CHECK(b.to_string(2) == "11100000100011100111010111110101110110111011001");
//     }
//     {
//         BigInt b ("-123123123123123123123");
//         CHECK(b.to_string(2) == "-1101010110010101101001101000010010101101110011011111111001110110011");
//         b++;
//         CHECK(b.to_string(10) == "-123123123123123123122");
//         CHECK(b.to_string(2) == "-1101010110010101101001101000010010101101110011011111111001110110010");
//     }
//     {
//         BigInt b ("-934857938475938475983475");
//         CHECK(b.to_string(10) == "-934857938475938475983475");
//     }
//     {
//         BigInt b ("123456789123456789");
//         BigInt c ("234234");
//         b *= c;
//         CHECK(b.to_string(2) == "110000111111010001011000111110101101110011010110110111101011101010010000010");
//     }
//     {
//         BigInt b ("123456789123456789");
//         CHECK(b.to_string(2) == "110110110100110110100101110101100110100000101111100010101");
//         b >>= 4;
//         CHECK(b.to_string(2) == "11011011010011011010010111010110011010000010111110001");
//     }
//     {
//         BigInt b ("1987239812700033");
//         CHECK(b.to_string(2) == "111000011110110001001010011000111101001011110000001");
//         b <<= 6;
//         CHECK(b.to_string(2) == "111000011110110001001010011000111101001011110000001000000");
//         b <<= 40;
//         cout << b.to_string(2) << endl;
//         CHECK(b.to_string(2) == "1110000111101100010010100110001111010010111100000010000000000000000000000000000000000000000000000");
//     }
// }

int main() {
    return UnitTest::RunAllTests();
}
