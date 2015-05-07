CXX=g++
CXXFLAGS=-Wall -g
GTEST_DIR=gtest-1.7.0
TEST_NAME=test_bbi

bbi: bbi.cpp bbi.h
	$(CXX) $(CXXFLAGS) -o bbi bbi.cpp

run:
	make bbi && ./bbi

buildgtest: ${GTEST_DIR}/src/gtest-all.cc
	$(CXX) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
      -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	rm gtest-all.o

test: ${TEST_NAME}.cpp bbi.h bbi.cpp buildgtest
# Use gtest now
#	$(CXX) $(CXXFLAGS) -Lunittest-cpp -o test_bbi test_bbi.cpp bbi.cpp -lUnitTest++
#	./test_bbi
	$(CXX) -isystem ${GTEST_DIR}/include -pthread ${TEST_NAME}.cpp bbi.cpp \
	  ${GTEST_DIR}/src/gtest_main.cc libgtest.a -o ${TEST_NAME}
	./${TEST_NAME}

clean:
	rm -f *.o core bbi ${TEST_NAME} libgtest.a gtest-all.o

st:
	subl bbi.* ${TEST_NAME}.cpp
