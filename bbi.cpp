#include <iostream>
#include <limits>
#include <climits>
#include <cassert>
#include <string>
#include <sstream>
#include <ostream>

#include "bbi.h"
#include "masklookup.h"

using std::cout;        using std::endl;
using std::string;      using std::stringstream;
using std::max;         using std::ostream;
using std::min;

/*
const bbi_data::size_type BigInt::BITS_PER_CHUNK = sizeof(bbi_chunk_t) * CHAR_BIT;
const bbi_chunk_t BigInt::MAX_CHUNK = std::numeric_limits<bbi_chunk_t>::max();
const bbi_chunk_t BigInt::HALF_MAX_CHUNK = BigInt::MAX_CHUNK / 2 + 1;
// INITIAL_CHUNKS should not be 1. Otherwise things like a++
// could result in vector realloc -- shouldn't be that expensive.
// Good rule of thumb: choose chunk size that results in default bit-width at
// least 2*CPU word size (e.g. 128 bits for a 64-bit architecture)
// Probably also good if this results in a structure that can be word-aligned,
// so avoid INITIAL_CHUNKS == 3 or so.
const bbi_data::size_type BigInt::INITIAL_CHUNKS = 128 / (BigInt::BITS_PER_CHUNK / 2);
const bbi_data::size_type BigInt::INITIAL_BITS = BigInt::INITIAL_CHUNKS * BigInt::BITS_PER_CHUNK;
*/

// for testing
const bbi_data::size_type BigInt::BITS_PER_CHUNK = 8;
const bbi_chunk_t BigInt::MAX_CHUNK = 255;
const bbi_chunk_t BigInt::HALF_MAX_CHUNK = 128;
const bbi_data::size_type BigInt::INITIAL_CHUNKS = 4;
const bbi_data::size_type BigInt::INITIAL_BITS = 32;

const bbi_chunk_t* BigInt::MASK_LOOKUP_TOP = mask_lookup_8_top;
const bbi_chunk_t* BigInt::MASK_LOOKUP_BOTTOM = mask_lookup_8_bottom;

void BigInt::init(bbi_data::size_type initial_chunks = INITIAL_CHUNKS) {
    negative = false;
    data.reserve(initial_chunks);
    data.resize(initial_chunks, 0);
}

BigInt::BigInt() {
    init();
}

BigInt::~BigInt() {
}

BigInt::BigInt(bbi_sval_t val) {
    init();

    // TODO what if max negative
    if (val < 0) {
        val = -val;
        negative = true;
    }
    data[0] = val;
}

BigInt::BigInt(const BigInt& other) {
    data = other.data;
    negative = other.negative;
}

BigInt::BigInt(string s) {
    init();

    string::size_type len = s.size();
    if (len == 0) {
        return;
    }
    if (len == 1 && s[0] >= '0' && s[0] <= '9') {
        data[0] = s[0] - '0';
        return;
    }

    string::size_type i = len - 1;
    string::size_type j = 0;

    if (s[0] == '-') {
        negative = true;
        j = 1;
    }

    BigInt pos_val = 1;
    BigInt ten = 10;
    for (; j < len; ++j) {
        bbi_chunk_t digit_val = s[i] - '0';
        BigInt pos_add (digit_val);
        pos_add *= pos_val;
        this->operator+=(pos_add);
        pos_val *= ten;
        --i;
    }
}

string BigInt::to_string(unsigned int base=10) const {
    stringstream ss;
    if (base < 2 || base > 36) {
        // TODO exception
        return "bad base value";
    }
    if (base == 2) {
        ss << bits();
        string tmp = ss.str();
        string ret;
        if (negative) {
            ret += '-';
        }
        string::size_type len = tmp.size();
        bool still_leading0 = true;
        for (unsigned long i = 0; i < len; ++i) {
            if (tmp[i] == '1')
                still_leading0 = false;
            if (!still_leading0)
                ret += tmp[i];
        }
        return ret;
    }
    if (base == 10) {
        BigInt tmp = 10;
        while (tmp < *this)
            tmp *= 10;
    }
    return ss.str();
}

ostream& operator<< (ostream& o, const BigInt& instance) {
    string tmp = instance.to_string();
    o << tmp;
    return o;
}

string BigInt::chunk_bits(bbi_chunk_t n) {
    string ret;
    bbi_chunk_t mask = MASK_LOOKUP_TOP[1];
    for (unsigned int i = 0; i < BITS_PER_CHUNK; ++i) {
        int masked = (n & mask);
        if (masked)
            ret += '1';
        else
            ret += '0';
        n <<= 1;
    }
    return ret;
}

string BigInt::all_bits(string sep=" ") const {
    if (is_zero())
        return "0";
    string ret;
    for (bbi_data::size_type i = data.size()-1; i > 0; --i) {
        bbi_chunk_t chunk = data[i];
        ret += chunk_bits(chunk);
        ret += sep;
    }
    bbi_chunk_t chunk = data[0];
    ret += chunk_bits(chunk);
    return ret;
}

string BigInt::bits() const {
    if (is_zero())
        return "0";
    string tmp = all_bits("");
    string::size_type len = tmp.size();
    string::size_type i;
    for (i = 0; i < len; ++i) {
        if (tmp[i] == '1')
            break;
    }
    assert(i < len);
    return tmp.substr(i);
}

//
// Assign and Compare
//

BigInt& BigInt::operator= (bbi_sval_t val) {
    // Variables tend to hold values of similar sizes, so don't realloc
    // Just zero chunks
    clear();
    if (val < 0) {
        val = -val;     // TODO max negative
        negative = true;
    }
    data[0] = val;
    return *this;
}

BigInt& BigInt::operator= (const BigInt& other) {
    bbi_data::size_type other_size = other.data.size();
    while (data.size() < other_size) {
        expand();
    }
    negative = other.negative;
    for (bbi_data::size_type i = 0; i < other_size; ++i) {
        data[i] = other.data[i];
    }
    return *this;
}

bool BigInt::operator== (bbi_sval_t val) {
    if (val < 0) {
        cout << "checking negative" << endl;
        cout << "-val = " << -val << endl;
        return ( (data[0] == -val) && negative );    // TODO max?
    }
    else {
        return ( data[0] == val );
    }
}

bool BigInt::operator== (const BigInt& other) {
    if (negative != other.negative)
        return false;
    bbi_data::size_type min_size = min(data.size(), other.data.size());
    for (bbi_data::size_type i = min_size; i > 0; --i) {
        if (data[i] != other.data[i])
            return false;
    }
    if (data[0] != other.data[0])
        return false;
    return true;
}

bool BigInt::operator!= (bbi_sval_t val) {
    return ! this->operator==(val);
}

bool BigInt::operator!= (const BigInt& other) {
    return ! this->operator==(other);
}

bool BigInt::operator< (bbi_sval_t val) {
    return false;
}

bool BigInt::operator< (const BigInt& other) {
    return false;
}

bool BigInt::operator> (bbi_sval_t val) {
    return false;
}

bool BigInt::operator> (const BigInt& other) {
    return false;
}

bool BigInt::operator<= (bbi_sval_t val) {
    return false;
}

bool BigInt::operator<= (const BigInt& other) {
    return false;
}

bool BigInt::operator>= (bbi_sval_t val) {
    return false;
}

bool BigInt::operator>= (const BigInt& other) {
    return false;
}


//
// Arithmetic
//

BigInt& BigInt::operator+= (const BigInt& other) {
    // Calculate minimum size for result data
    // - start with largest existing size
    //cout << "this = " << this->bits() << endl;
    //cout << "other = " << other.bits() << endl;
    bbi_data::size_type needed_chunks = max(data.size(), other.data.size());
    while (needed_chunks > data.size()) {
        //cout << "data size = " << data.size() << ", expanding..." << endl;
        expand();
    }
    //cout << "AFTER EXPAND" << endl;
    //cout << "this = " << this->bits() << endl;
    //cout << "other = " << other.bits() << endl;
    //cout << endl;

    bool prev_overflow = false;
    unsigned long i;
    for (i = 0; i < data.size(); ++i) {
        bbi_chunk_t a = data[i];
        bbi_chunk_t b = other.data[i];
        //cout << "a bits = " << bits(a) << endl;
        //cout << "b bits = " << bits(b) << endl;
        data[i] = a + b;
        if (prev_overflow) {
            //cout << "prev overflow true" << endl;
            // Check previous iteration
            // This can't overflow, will always have at least one 0 bit
            data[i]++;
            prev_overflow = false;
        }
        // TODO optimization? here and subn, just perform op and check?
        if (addn_would_overflow(a, b)) {
            prev_overflow = true;
            //cout << "set prev overflow" << endl;
        }
    }

    //cout << "doing final, prev overflow = " << prev_overflow << endl;

    bool final_overflow = false;
    if (prev_overflow) {
        if (data[i] == MAX_CHUNK)
            final_overflow = true;
        data[i] += 1;
        if (final_overflow) {
            expand();
            data[i+1] += 1;
        }
    }

    return *this;
}

BigInt& BigInt::operator+= (bbi_sval_t n) {
    BigInt tmp (n);
    this->operator+=(tmp);
    return *this;
}

BigInt& BigInt::operator++ () {
    this->operator+=(1);
    return *this;
}


BigInt& BigInt::operator-= (const BigInt& other) {
    // Calculate minimum size for result data
    // - start with largest existing size
    bbi_data::size_type needed_chunks = max(data.size(), other.data.size());
    while (needed_chunks > data.size())
        expand();

    // See if we'll go negative
    if (other > *this)
        negative = true;

    unsigned long i;
    for (i = 0; i < data.size(); ++i) {
        bbi_chunk_t tmp = data[i];
        data[i] -= other.data[i];
        if (data[i] > tmp)
            // TODO underflow
            return *this;
    }
    return *this;
}

BigInt& BigInt::operator-= (bbi_sval_t n) {
    BigInt tmp (n);
    this->operator-=(tmp);
    return *this;
}

// TODO optimize - specialized code for increment/decrement
BigInt& BigInt::operator-- () {
    this->operator-=(1);
    return *this;
}

// TODO other-fix for --, ++


BigInt& BigInt::operator*= (const BigInt& other) {
    BigInt tmp_this = *this;
    BigInt tmp_other = other;

    // Zero self
    clear();

    //cout << "this = " << (*this).bits() << endl;

    while (tmp_other) {
        //cout << "tmp_this = " << tmp_this.bits() << endl;
        if (tmp_other.get_bit(0)) {
            //cout << "adding tmp_this" << endl;
            this->operator+=(tmp_this);
        }
        else {
            //cout << "not adding" << endl;
        }
        tmp_this <<= 1;
        //cout << "tmp_this = " << tmp_this.all_bits() << endl;
        tmp_other >>= 1;
        //cout << "tmp_other = " << tmp_other.bits() << endl << endl;
        //cout << "this = " << (*this).bits() << endl << endl;
    }

    return *this;
}

BigInt& BigInt::operator/= (const BigInt& other) {
    return *this;
}

BigInt BigInt::operator+ (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator- (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator* (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator/ (const BigInt& other) {
    BigInt ret;
    return ret;
}

//
// Bit ops
//


BigInt BigInt::operator~ () {
    BigInt ret (*this);
    bbi_data::size_type len = data.size();
    for (bbi_data::size_type i = 0; i < len; ++i) {
        ret.data[i] = ~ ret.data[i];
    }
    return ret;
}

BigInt& BigInt::operator&= (bbi_uval_t val) {
    return *this;
}

BigInt& BigInt::operator&= (const BigInt& other) {
    return *this;
}

BigInt& BigInt::operator|= (bbi_uval_t val) {
    return *this;
}

BigInt& BigInt::operator|= (const BigInt& other) {
    return *this;
}

BigInt& BigInt::operator^= (bbi_uval_t val) {
    return *this;
}

BigInt& BigInt::operator^= (const BigInt& other) {
    return *this;
}

BigInt& BigInt::operator<<= (bbi_uval_t n) {
    if (n == 0)
        return *this;

    //cout << "size before: " << data.size() << endl;
    while (n > freeish_bits())
        expand();
    //cout << "size after: " << data.size() << endl;

    bbi_chunk_t mask = MASK_LOOKUP_TOP[n];           // TODO 32

    for (long i = data.size()-1; i >= 0; --i) {
        // Save top n bits of chunk
        bbi_chunk_t chunk = data[i];
        bbi_chunk_t top_n = chunk & mask;
        top_n >>= BITS_PER_CHUNK - n;
        // Move those bits to next chunk
        bbi_chunk_t tmp_next = data[i+1];
        tmp_next |= top_n;
        data[i+1] = tmp_next;
        // Shift
        chunk <<= n;
        // Store
        data[i] = chunk;
        //cout << bits() << endl << endl;
    }
    return *this;
}

BigInt& BigInt::operator>>= (bbi_uval_t n) {
    if (n == 0)
        return *this;

    bbi_chunk_t mask = MASK_LOOKUP_BOTTOM[n];           // TODO 32

    data[0] >>= n;
    for (unsigned long i = 1; i < data.size(); ++i) {
        bbi_chunk_t chunk = data[i];
        bbi_chunk_t bottom_n = chunk & mask;
        chunk >>= n;
        data[i] = chunk;
        data[i-1] |= (bottom_n << (BITS_PER_CHUNK-n));
    }

    return *this;
}

BigInt BigInt::operator& (bbi_uval_t n) {
    BigInt ret (data[0]);
    ret.data[0] &= n;
    return ret;
}

BigInt BigInt::operator& (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator| (bbi_uval_t val) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator| (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator^ (bbi_uval_t val) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator^ (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator<< (bbi_uval_t val) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator>> (bbi_uval_t val) {
    BigInt ret;
    return ret;
}


//
// Other stuff
//

bool BigInt::is_zero() const {
    bbi_data::size_type sz = data.size();
    // Count up more efficient
    for (bbi_data::size_type i = sz-1; i > 0; --i) {
        if (data[i] != 0) {
            return false;
        }
    }
    if (data[0] != 0)
        return false;
    return true;
}

bool BigInt::is_negative() const {
    return negative;
}

BigInt::operator bool() const {
    return is_zero() ? 0 : &BigInt::this_type_does_not_support_comparisons;
}

bool BigInt::operator! () const {
    return is_zero() ? true : false;
}

// Sometimes want to test a bit without creating a whole new object
int BigInt::get_bit(size_t bit_idx) const {
    size_t max_bit_idx = data.size() * BITS_PER_CHUNK - 1;
    if (bit_idx > max_bit_idx)
        return 0;

    size_t chunk_idx = bit_idx / BITS_PER_CHUNK;
    //cout << "chunk_idx = " << chunk_idx << endl;
    size_t chunk_offset = bit_idx % BITS_PER_CHUNK;
    //cout << "chunk_offset = " << chunk_offset << endl;
    bbi_chunk_t chunk = data[chunk_idx];
    //cout << "chunk = " << bits(chunk) << endl;
    bbi_chunk_t mask = MASK_LOOKUP_BOTTOM[chunk_offset+1];
    //cout << "mask = " << bits(mask) << endl;
    bbi_chunk_t tmp = chunk & mask;
    tmp >>= chunk_offset;
    //cout << "tmp = " << bits(tmp) << endl;
    return tmp ? 1 : 0;
}

//
// Private methods
//

void BigInt::expand() {
    data.resize(data.size() * 2);
}

void BigInt::clear() {
    bbi_data::size_type len = data.size();
    for (bbi_data::size_type i = 0; i < len; ++i) {
        data[i] = 0;
    }
    negative = false;
}

inline bbi_data::size_type BigInt::num_free_chunks() const {
    bbi_data::size_type ret = 0;
    //cout << "num_free_chunks, size = " << data.size() << endl;
    bbi_data::size_type sz = data.size();
    //cout << "size = " << sz << endl;
    for (bbi_data::size_type i = sz-1; i > 0; --i) {
        if (data[i] == 0)
            ++ret;
        else
            break;
    }
    //cout << "num_free_chunks = " << ret << endl;
    return ret;
}

inline bbi_data::size_type BigInt::freeish_bits() const {
    return num_free_chunks() * BITS_PER_CHUNK;
}

inline bool BigInt::addn_would_overflow(bbi_chunk_t a, bbi_chunk_t b) {
    bbi_chunk_t a_compl = ~a;
    if (b <= a_compl)
        return false;
    return true;
}

inline bool BigInt::subn_would_underflow(bbi_chunk_t a, bbi_chunk_t b) {
    return b > a;
}

// int main() {
//     BigInt b = -1;
//     if (b == -1) {
//         cout << "cool" << endl;
//     }
// }

