CXX=g++
CXXFLAGS=-Wall -g

bbi: bbi.cpp bbi.h
	$(CXX) $(CXXFLAGS) -o bbi bbi.cpp

test: test_bbi.cpp bbi.h bbi.cpp
	$(CXX) $(CXXFLAGS) -Lunittest-cpp -o test_bbi test_bbi.cpp bbi.cpp -lUnitTest++
	./test_bbi

clean:
	rm -f *.o core bbi test_bbi
