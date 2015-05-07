#include "gtest/gtest.h"
#include "bbi.h"

#include <iostream>
using namespace std;

TEST(Init, CreateAssign) {
    BigInt b = 1;
    EXPECT_EQ(b, 1);
}
