#include "gtest/gtest.h"

// Use gtest now
#if 0
#include "unittest-cpp/src/UnitTest++.h"
#endif

#include "bbi.h"

#include <iostream>
using namespace std;

// TODO
// - boundary tests for +=, +=, ++
// - boundary tests for -=, -=, --
// - are copy tests complete?
// - cs13 - debug properly

TEST(Init, CreateAssign) {
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

    // Remember to delete any new's or gtest will segfault
    // delete b5;
    // delete b5;
    // delete b7;
}

// Conversion to binary string is the basis of all other tests
TEST(Bits, ChunkBits) {
    string chunk_string (BigInt::BITS_PER_CHUNK, '0');
    bbi_chunk_t b = 0;
    EXPECT_EQ(BigInt::chunk_bits(b), chunk_string);

    bbi_chunk_t c = 1;
    chunk_string[ BigInt::BITS_PER_CHUNK - 1 ] = '1';
    EXPECT_EQ(BigInt::chunk_bits(c), chunk_string);

    bbi_chunk_t d = BigInt::HALF_MAX_CHUNK;
    chunk_string[ BigInt::BITS_PER_CHUNK - 1 ] = '0';
    chunk_string[0] = '1';
    EXPECT_EQ(BigInt::chunk_bits(d), chunk_string);

    string chunk_string2 (BigInt::BITS_PER_CHUNK, '1');
    bbi_chunk_t e = BigInt::MAX_CHUNK;
    EXPECT_EQ(BigInt::chunk_bits(e), chunk_string2);
}

// Tests for .all_bits() that don't need operatorX directly or indirectly
TEST(Bits, AllBitsSimple) {
}

// Tests for .bits() that don't need operatorX directly or indirectly
TEST(Bits, BitsSimple) {
    BigInt b = 0;
    EXPECT_EQ(b.bits(), "0");

    BigInt b2 = 1;
    EXPECT_EQ(b2.bits(), "1");

    BigInt b3 = 2;
    EXPECT_EQ(b3.bits(), "10");

    BigInt b4 = 3;
    EXPECT_EQ(b4.bits(), "11");

    BigInt b5 = 4;
    EXPECT_EQ(b5.bits(), "100");

    BigInt b6 = BigInt::HALF_MAX_CHUNK;
    string cs6 (BigInt::BITS_PER_CHUNK, '0');
    cs6[0] = '1';
    EXPECT_EQ(b6.bits(), cs6);

    // This no longer works - BigInt::MAX_CHUNK and above must init with string
    // BigInt b7 = BigInt::MAX_CHUNK;
    // string cs7 (BigInt::BITS_PER_CHUNK, '1');
    // EXPECT_EQ(b7.bits(), cs7);

    // EXPECT_EQ we can store numbers above certain hard limits (2**32, 2**64 etc)

    // Need 14 bits
    BigInt b8 ("12345");
    string cs8 = "11000000111001";
    EXPECT_EQ(b8.bits(), cs8);

    // Need 47 bits
    BigInt b9 ("123451234512345");
    string cs9 = "11100000100011100111010111110101110110111011001";
    EXPECT_EQ(b9.bits(), cs9);

    // Need 81 bits
    BigInt b10 ("1234512345123451234512345");
    string cs10 = "100000101011010110000110001011111001010110000110100110001101111010110110111011001";
    EXPECT_EQ(b10.bits(), cs10);

    // Need 130 bits
    BigInt b11 ("1234512345123451234512345123451234512345");
    string cs11 = "1110100000101111100110110111100001010010100010001100011001011110110110111111100000011100011101010111101011101111010110110111011001";
    EXPECT_EQ(b11.bits(), cs11);

    // Need 130 bits
    BigInt b12 ("1234512345123451234512345123451234512345");
    string cs12 = "1110100000101111100110110111100001010010100010001100011001011110110110111111100000011100011101010111101011101111010110110111011001";
    EXPECT_EQ(b12.bits(), cs12);

    // Need 263 bits
    // (263 bits > number of bits in 4 words for 64-bit architectures)
    // TODO something screwy is happening here
    BigInt b13 ("12345123451234512345123451234512345123451234512345123451234512345123451234512345");
    string cs13 = "11010101001110101010100010000101001101100100101010100110011111110110100011100011111100110100100100011001011000010010010101011100000000011101010011101000010101000111100000001011101110011101111011011010010100000101100101010101101010111101011101111010110110111011001";
    //EXPECT_EQ(b13.bits(), cs13);
}

TEST(Bits, MethodGetBit) {
    BigInt b = 6;
    EXPECT_EQ(b.get_bit(0), 0);
    EXPECT_EQ(b.get_bit(1), 1);
    EXPECT_EQ(b.get_bit(2), 1);
    EXPECT_EQ(b.get_bit(3), 0);
    EXPECT_EQ(b.get_bit(12345), 0);

    BigInt b2 ("1234512345123451234512345123451234512345");
    string s2 = "1110100000101111100110110111100001010010100010001100011001011110110110111111100000011100011101010111101011101111010110110111011001";
    string::size_type s2_len = s2.size();
    int j = 0;
    for (int i = s2_len - 1; i >= 0; --i) {
        EXPECT_EQ(b2.get_bit(j), s2[i] - '0');
        ++j;
    }
}

TEST(SimpleMethods, IsZero) {
    BigInt b = 0;
    EXPECT_TRUE(b.is_zero());
    BigInt c = 1;
    EXPECT_TRUE(!c.is_zero());

    BigInt b2 (0);
    EXPECT_TRUE(b2.is_zero());
    BigInt c2 (1);
    EXPECT_TRUE(!c2.is_zero());

    BigInt b3 ("0");
    EXPECT_TRUE(b3.is_zero());
    BigInt c3 ("1");
    EXPECT_TRUE(!c3.is_zero());

    BigInt c4 ("12345");
    EXPECT_TRUE(!c4.is_zero());
}

TEST(SimpleMethods, IsNegative) {
    BigInt b ("-0");
    EXPECT_TRUE(b.is_negative());
    EXPECT_TRUE(b.is_zero());
    EXPECT_EQ(b.bits(), "0");

    // TODO init from negative integer literal

    BigInt b2 ("-1");
    EXPECT_TRUE(b2.is_negative());
    EXPECT_TRUE(!b2.is_zero());
    EXPECT_EQ(b2.bits(), "1");

    BigInt b3 ("-12345123451234512345");
    EXPECT_TRUE(b3.is_negative());
    EXPECT_TRUE(!b3.is_zero());
    EXPECT_EQ(b3.bits(), "1010101101010010101100000101111100100011001011010110110111011001");
}

TEST(Operators, Increment) {
    BigInt b = 0;
    ++b;
    EXPECT_EQ(b.bits(), "1");

    BigInt b2 = 1;
    //b2++;
    //EXPECT_EQ(b2.bits(), "10");
}

// TEST(CompareEq) {
//     BigInt b1 = 5;
//     BigInt b2 = 5;
//     EXPECT_EQ(b1, 5);
//     EXPECT_EQ(b1, b2);
//     EXPECT_EQ(b2, b1);
//     BigInt b3 = 6;
//     EXPECT_EQ(b3, 6);
//     EXPECT_EQ(b3 != b2);
//     EXPECT_EQ(b2 != b3);
//     EXPECT_EQ(b3 != 99);
// }

// TEST(CompareOther) {
//     BigInt b1 = 5;
//     BigInt b2 = 5;
//     EXPECT_EQ(b1 <= b2);
//     EXPECT_EQ(b2 <= b1);
//     EXPECT_EQ(b1 >= b2);
//     EXPECT_EQ(b2 >= b1);
//     BigInt b3 = 6;
//     EXPECT_EQ(b1 <= b3);
//     EXPECT_EQ(b1 < b3);
//     EXPECT_EQ(b3 >= b1);
//     EXPECT_EQ(b3 > b1);
// }

TEST(Operators, AddAssign) {
    BigInt b (0);
    ASSERT_EQ(b.bits(), "0");
    b += 1;
    ASSERT_EQ(b.bits(), "1");
    b += 1;
    ASSERT_EQ(b.bits(), "10");
    b += 1;
    ASSERT_EQ(b.bits(), "11");
    b += 1;
    ASSERT_EQ(b.bits(), "100");

    b += 2;
    ASSERT_EQ(b.bits(), "110");

    b += 127;
    ASSERT_EQ(b.bits(), "10000101");

    // EXPECT_EQ various hard limits

    // b += 12345;
    // b += 123451234512345;

    // Now same tests but with BigInt instead of bbi_sval_t
    BigInt c = 0;
    ASSERT_EQ(c.bits(), "0");
    BigInt d = 1;
    c += d;
    ASSERT_EQ(c.bits(), "1");
    c += d;
    ASSERT_EQ(c.bits(), "10");
    c += d;
    ASSERT_EQ(c.bits(), "11");
    c += d;
    ASSERT_EQ(c.bits(), "100");

    BigInt d2 = 2;
    c += d2;
    ASSERT_EQ(c.bits(), "110");

    BigInt d3 = 127;
    c += d3;
    ASSERT_EQ(c.bits(), "10000101");

    // check hard limits TODO
}

TEST(Operators, MultAssign) {
    BigInt b = 2;
    EXPECT_EQ(b.bits(), "10");
    b *= 1;
    EXPECT_EQ(b.bits(), "10");
    b *= 2;
    EXPECT_EQ(b.bits(), "100");
    b *= 2;
    EXPECT_EQ(b.bits(), "1000");
    b *= 3;
    EXPECT_EQ(b.bits(), "11000");
    b *= 4;
    EXPECT_EQ(b.bits(), "1100000");
    b *= 5;
    EXPECT_EQ(b.bits(), "111100000");
    b *= 6;
    EXPECT_EQ(b.bits(), "101101000000");
    b *= 7;
    EXPECT_EQ(b.bits(), "100111011000000");
    b *= 8;
    EXPECT_EQ(b.bits(), "100111011000000000");
    b *= 9;
    EXPECT_EQ(b.bits(), "101100010011000000000");
    b *= 10;
    EXPECT_EQ(b.bits(), "110111010111110000000000");
    b *= 11;
    EXPECT_EQ(b.bits(), "1001100001000101010000000000");
    b *= 12;
    EXPECT_EQ(b.bits(), "1110010001100111111000000000000");
    b *= 13;
    EXPECT_EQ(b.bits(), "10111001100101000110011000000000000");
    b *= 14;
    EXPECT_EQ(b.bits(), "101000100110000111011001010000000000000");
    b *= 15;
    EXPECT_EQ(b.bits(), "1001100000111011101110111010110000000000000");
    // EXPECT_EQ hard limits TODO

    BigInt c (2);
    BigInt d (1);
    EXPECT_EQ(c.bits(), "10");
    c *= d;
    EXPECT_EQ(c.bits(), "10");

    BigInt d2 = 2;
    c *= d2;
    EXPECT_EQ(c.bits(), "100");

    c *= d2;
    EXPECT_EQ(c.bits(), "1000");

    BigInt d3 = 3;
    c *= d3;
    EXPECT_EQ(c.bits(), "11000");

    BigInt d4 = 4;
    c *= d4;
    EXPECT_EQ(c.bits(), "1100000");

    BigInt d5 = 5;
    c *= d5;
    EXPECT_EQ(c.bits(), "111100000");

    BigInt d6 = 6;
    c *= d6;
    EXPECT_EQ(c.bits(), "101101000000");

    BigInt d7 = 7;
    c *= d7;
    EXPECT_EQ(c.bits(), "100111011000000");

    BigInt d8 = 8;
    c *= d8;
    EXPECT_EQ(c.bits(), "100111011000000000");

    BigInt d9 = 9;
    c *= d9;
    EXPECT_EQ(c.bits(), "101100010011000000000");

    BigInt d10 = 10;
    c *= d10;
    EXPECT_EQ(c.bits(), "110111010111110000000000");

    BigInt d11 = 11;
    c *= d11;
    EXPECT_EQ(c.bits(), "1001100001000101010000000000");

    BigInt d12 = 12;
    c *= d12;
    EXPECT_EQ(c.bits(), "1110010001100111111000000000000");

    BigInt d13 = 13;
    c *= d13;
    EXPECT_EQ(c.bits(), "10111001100101000110011000000000000");

    BigInt d14 = 14;
    c *= d14;
    EXPECT_EQ(c.bits(), "101000100110000111011001010000000000000");

    BigInt d15 = 15;
    c *= d15;
    EXPECT_EQ(c.bits(), "1001100000111011101110111010110000000000000");


    BigInt e = 1;
    BigInt ten = 10;

    e *= ten;
    EXPECT_EQ(e.bits(), "1010");
    e *= ten;
    EXPECT_EQ(e.bits(), "1100100");
    e *= ten;
    EXPECT_EQ(e.bits(), "1111101000");
    e *= ten;
    EXPECT_EQ(e.bits(), "10011100010000");
    e *= ten;
    EXPECT_EQ(e.bits(), "11000011010100000");
    e *= ten;
    EXPECT_EQ(e.bits(), "11110100001001000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "100110001001011010000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "101111101011110000100000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "111011100110101100101000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "1001010100000010111110010000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "1011101001000011101101110100000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "1110100011010100101001010001000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "10010001100001001110011100101010000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "10110101111001100010000011110100100000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "11100011010111111010100100110001101000000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "100011100001101111001001101111110000010000000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "101100011010001010111100001011101100010100000000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "110111100000101101101011001110100111011001000000000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "1000101011000111001000110000010010001001111010000000000000000000");
    e *= ten;
    EXPECT_EQ(e.bits(), "1010110101111000111010111100010110101100011000100000000000000000000");


    // EXPECT_EQ hard limits TODO
}

TEST(Operators, SubAssign) {
    BigInt b = 1;
    EXPECT_EQ(b.bits(), "1");
    b -= 1;
    EXPECT_EQ(b.bits(), "0");

    BigInt b2 = 1;
    b2 -= 2;
    EXPECT_EQ(b2.bits(), "1");
    EXPECT_TRUE(b2.is_negative());
}

// TEST(BitwiseNot) {

// }

// TEST(LogicalNot) {
//     BigInt a = 0;
//     EXPECT_EQ(!a);
//     BigInt b = 1;
//     EXPECT_EQ(b);
//     EXPECT_EQ(!!b);
//     BigInt c = 42;
//     EXPECT_EQ(c);
//     EXPECT_EQ(!!c);
// }

// TEST(LeftShiftAssign) {
//     BigInt b = 1;
//     b <<= 1;
//     EXPECT_EQ(b, 2);
//     b <<= 1;
//     EXPECT_EQ(b, 4);
//     b <<= 2;
//     EXPECT_EQ(b, 16);

//     BigInt n = 128;
//     n <<= 1;
//     EXPECT_EQ(n, 256);

//     BigInt m = 32768;
//     m <<= 1;
//     EXPECT_EQ(m, 65536);
// }

// TEST(AndAssign) {
//     BigInt b = 1;

// }

// TEST(Copy) {
//     BigInt b = 5;
//     BigInt c = b;
//     b <<= 1;
//     EXPECT_EQ(b, 10);
//     EXPECT_EQ(c, 5);
// }

// TEST(General) {
//     {
//         BitInt b ("123451234512345");
//         EXPECT_EQ(b.to_string(2), "11100000100011100111010111110101110110111011001");
//     }
//     {
//         BigInt b ("-123123123123123123123");
//         EXPECT_EQ(b.to_string(2), "-1101010110010101101001101000010010101101110011011111111001110110011");
//         b++;
//         EXPECT_EQ(b.to_string(10), "-123123123123123123122");
//         EXPECT_EQ(b.to_string(2), "-1101010110010101101001101000010010101101110011011111111001110110010");
//     }
//     {
//         BigInt b ("-934857938475938475983475");
//         EXPECT_EQ(b.to_string(10), "-934857938475938475983475");
//     }
//     {
//         BigInt b ("123456789123456789");
//         BigInt c ("234234");
//         b *= c;
//         EXPECT_EQ(b.to_string(2), "110000111111010001011000111110101101110011010110110111101011101010010000010");
//     }
//     {
//         BigInt b ("123456789123456789");
//         EXPECT_EQ(b.to_string(2), "110110110100110110100101110101100110100000101111100010101");
//         b >>= 4;
//         EXPECT_EQ(b.to_string(2), "11011011010011011010010111010110011010000010111110001");
//     }
//     {
//         BigInt b ("1987239812700033");
//         EXPECT_EQ(b.to_string(2), "111000011110110001001010011000111101001011110000001");
//         b <<= 6;
//         EXPECT_EQ(b.to_string(2), "111000011110110001001010011000111101001011110000001000000");
//         b <<= 40;
//         cout << b.to_string(2) << endl;
//         EXPECT_EQ(b.to_string(2), "1110000111101100010010100110001111010010111100000010000000000000000000000000000000000000000000000");
//     }
// }

// Use gtest now
// int main() {
//     return UnitTest::RunAllTests();
// }
