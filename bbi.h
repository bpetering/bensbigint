#ifndef BBI_H
#define BBI_H

#include <vector>
#include <string>

using std::vector;
using std::string;

// typedef unsigned long        bbi_chunk_t;    // type for chunks
// typedef signed long          bbi_sval_t;     // type for signed integer literals
// typedef unsigned long        bbi_uval_t;     // type for unsigned integer literals (e.g. bit-shifting)

typedef unsigned char           bbi_chunk_t;        // testing
typedef signed char             bbi_sval_t;         // testing
typedef unsigned char           bbi_uval_t;         // testing

typedef vector<bbi_chunk_t>     bbi_data;


class BigInt {
public:
    // Constructors
    BigInt();
    ~BigInt();
    BigInt(bbi_sval_t);
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
    BigInt& operator= (bbi_sval_t);
    BigInt& operator= (const BigInt&);

    bool operator== (bbi_sval_t);
    bool operator== (const BigInt&);

    bool operator!= (bbi_sval_t);
    bool operator!= (const BigInt&);

    bool operator< (bbi_sval_t);
    bool operator< (const BigInt&);

    bool operator> (bbi_sval_t);
    bool operator> (const BigInt&);

    bool operator<= (bbi_sval_t);
    bool operator<= (const BigInt&);

    bool operator>= (bbi_sval_t);
    bool operator>= (const BigInt&);


    // Basic arithmetic
    BigInt& operator+= (const BigInt&);
    BigInt& operator+= (bbi_sval_t);
    BigInt& operator++ ();                  // += 1

    BigInt& operator-= (const BigInt&);
    BigInt& operator-= (bbi_sval_t);
    BigInt& operator-- ();                  // -= 1

    BigInt& operator*= (const BigInt&);
    BigInt& operator/= (const BigInt&);
    BigInt& operator%= (const BigInt&);
    BigInt operator+ (const BigInt&);
    BigInt operator- (const BigInt&);
    BigInt operator* (const BigInt&);
    BigInt operator/ (const BigInt&);
    BigInt operator% (const BigInt&);

    //
    // Bit ops
    //
    BigInt operator~ ();

    BigInt& operator&= (const BigInt&);
    BigInt& operator&= (bbi_uval_t);

    BigInt& operator|= (const BigInt&);
    BigInt& operator|= (bbi_uval_t);

    BigInt& operator^= (const BigInt&);
    BigInt& operator^= (bbi_uval_t);

    // Don't need BigInt other for these two (physical memory limits) but
    // include for ease of use
    BigInt& operator<<= (const BigInt&);
    BigInt& operator<<= (bbi_uval_t);

    BigInt& operator>>= (const BigInt&);
    BigInt& operator>>= (bbi_uval_t);

    BigInt operator& (const BigInt&);
    BigInt operator& (bbi_uval_t);

    BigInt operator| (const BigInt&);
    BigInt operator| (bbi_uval_t);

    BigInt operator^ (const BigInt&);
    BigInt operator^ (bbi_uval_t);

    // As above - physical memory limits, but ease of use
    BigInt operator<< (const BigInt&);
    BigInt operator<< (bbi_uval_t);

    BigInt operator>> (const BigInt&);
    BigInt operator>> (bbi_uval_t);

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
    static inline bool subn_would_underflow(bbi_chunk_t, bbi_chunk_t);  // TODO static

    // Safe bool idiom
    // http://www.artima.com/cppsource/safebool.html
    typedef void (BigInt::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}
};

#endif
