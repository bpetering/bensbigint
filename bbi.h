#ifndef BBI_H
#define BBI_H

#include <vector>
#include <string>

using std::vector;
using std::string;

// typedef unsigned long           bbi_chunk_t;
// typedef signed long             bbi_schunk_t;
typedef unsigned char           bbi_chunk_t;        // testing
typedef signed char             bbi_schunk_t;       // testing

typedef vector<bbi_chunk_t>     bbi_data;

class BigInt {
public:
    // Constructors
    BigInt();
    ~BigInt();
    BigInt(bbi_chunk_t);
    BigInt(const BigInt&);
    BigInt(string);


    // Convert to string using given base.
    string to_string(unsigned int) const;

    // Convert a single chunk to binary string representation
    static string chunk_bits(bbi_chunk_t);

    // Convert all chunks to binary representation with given chunk separator
    string all_bits(string) const;

    // Convert to binary representation with no leading 0s, no separator
    string bits() const;


    // Assign and Compare
    BigInt& operator= (bbi_chunk_t);
    BigInt& operator= (const BigInt&);
    bool operator== (bbi_chunk_t);
    bool operator== (const BigInt&);
    bool operator!= (bbi_chunk_t);
    bool operator!= (const BigInt&);
    bool operator< (bbi_chunk_t);
    bool operator< (const BigInt&);
    bool operator> (bbi_chunk_t);
    bool operator> (const BigInt&);
    bool operator<= (bbi_chunk_t);
    bool operator<= (const BigInt&);
    bool operator>= (bbi_chunk_t);
    bool operator>= (const BigInt&);


    // Basic arithmetic
    BigInt& operator+= (const BigInt&);
    BigInt& operator++ ();
    BigInt& operator-= (const BigInt&);
    BigInt& operator-- ();
    BigInt& operator*= (const BigInt&);
    BigInt& operator/= (const BigInt&);
    BigInt& operator%= (const BigInt&);
    BigInt operator+ (const BigInt&);
    BigInt operator- (const BigInt&);
    BigInt operator* (const BigInt&);
    BigInt operator/ (const BigInt&);
    BigInt operator% (const BigInt&);

    // Bit ops
    BigInt operator~ ();
    BigInt& operator&= (bbi_chunk_t);
    BigInt& operator&= (const BigInt&);
    BigInt& operator|= (bbi_chunk_t);
    BigInt& operator|= (const BigInt&);
    BigInt& operator^= (bbi_chunk_t);
    BigInt& operator^= (const BigInt&);
    // Don't really need BigInt other for these two
    BigInt& operator<<= (bbi_chunk_t);
    BigInt& operator>>= (bbi_chunk_t);
    BigInt operator& (bbi_chunk_t);
    BigInt operator& (const BigInt&);
    BigInt operator| (bbi_chunk_t);
    BigInt operator| (const BigInt&);
    BigInt operator^ (bbi_chunk_t);
    BigInt operator^ (const BigInt&);
    // or these two
    BigInt operator<< (bbi_chunk_t);
    BigInt operator>> (bbi_chunk_t);

    // Other stuff
    bool is_zero() const;
    bool is_negative() const;
    int get_bit(size_t) const;
    // TODO abs?
    operator bool() const;
    bool operator! () const;

    static const size_t BITS_PER_CHUNK;
    static const bbi_chunk_t MAX_CHUNK;         // maximum value storeable by chunk
    static const bbi_chunk_t HALF_MAX_CHUNK;    // half max value + 1
    static const size_t INITIAL_CHUNKS;         // number of chunks to alloc initially
    static const size_t INITIAL_BITS;           // number of bits initial storage
    static const bbi_chunk_t *MASK_LOOKUP_TOP;  // lookup table for bit
    static const bbi_chunk_t *MASK_LOOKUP_BOTTOM;  // lookup table for bit

private:
    bbi_data *data;
    bool negative;

    void init(bbi_data::size_type);
    void expand();
    void clear();
    bbi_data::size_type size() const;
    inline bbi_data::size_type num_free_chunks() const;
    inline bbi_data::size_type freeish_bits() const;
    static inline bool addn_would_overflow(bbi_chunk_t, bbi_chunk_t);

    // Safe bool idiom
    // http://www.artima.com/cppsource/safebool.html
    typedef void (BigInt::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}
};

#endif
