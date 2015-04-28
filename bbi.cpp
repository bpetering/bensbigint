#include "bbi.h"

#include <iostream>
#include <limits>
#include <climits>
#include <cassert>
#include <string>
#include <sstream>
#include <ostream>

#include "masklookup.h"

using std::cout;        using std::endl;
using std::string;      using std::stringstream;
using std::max;         using std::ostream;
using std::min;

/*
const bbi_data::size_type BigInt::BITS_PER_CHUNK = sizeof(bbi_chunk_t) * CHAR_BIT;
const bbi_chunk_t BigInt::MAX_CHUNK = std::numeric_limits<bbi_chunk_t>::max();
const bbi_chunk_t BigInt::HALF_MAX_CHUNK = BigInt::MAX_CHUNK / 2 + 1;
// Default chunk size should not be 1. Otherwise things like a++
// could result in vector realloc -- shouldn't be that expensive
// Probably also good if this results in a value that can be aligned
// TODO experiment with ideal size
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

// TODO experiment with reference counting (like string) vs copying

BigInt::BigInt() {
    data = 0;
    negative = false;
}

BigInt::~BigInt() {
    delete data;
}

bbi_data::size_type BigInt::size() const {
    assert(data);
    return data->size();
}

void BigInt::init(bbi_data::size_type initial_chunks) {
    data = new bbi_data (initial_chunks);
    negative = false;
    for (bbi_data::size_type i = 0; i < initial_chunks; ++i) {
        (*data)[i] = 0;
    }
}

BigInt::BigInt(bbi_chunk_t val) {
    init(INITIAL_CHUNKS);
    // (*data)[i] is quicker (I think) because it avoids the bounds checking
    // of data->at(i) - hence former is used
    (*data)[0] = val;
}

BigInt::BigInt(const BigInt& other) {
    if (other.data == 0)
        return;
    bbi_data::size_type other_size = other.size();
    data = new bbi_data (other_size);
    negative = other.negative;
    for (bbi_data::size_type i = 0; i < other_size; ++i) {
        (*data)[i] = (*other.data)[i];
    }
}

BigInt::BigInt(string s) {
    init(INITIAL_CHUNKS);

    string::size_type len = s.size();
    if (len == 0) {
        return;
    }
    if (len == 1 && s[0] >= '0' && s[0] <= '9') {
        (*data)[0] = s[0] - '0';
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
        //cout << "adding: " << pos_add.bits() << endl;
        this->operator+=(pos_add);
        //cout << "current: " << this->bits() << endl << endl;
        pos_val *= ten;
        --i;
    }
}

string BigInt::to_string(unsigned int base=10) const {
    assert(data);
    stringstream ss;
    if (base < 2 || base > 36) {
        // TODO exception
        return "bad base value";
    }
    if (base == 2) {
        ss << bits("");
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

string BigInt::bits(bbi_chunk_t n) {
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

string BigInt::bits(string sep=" ") const {
    assert(data);
    string ret;
    for (bbi_data::size_type i = data->size()-1; i > 0; --i) {
        bbi_chunk_t n = (*data)[i];
        ret += bits(n);
        ret += sep;
    }
    bbi_chunk_t n = (*data)[0];
    ret += bits(n);
    return ret;
}

inline bool BigInt::addn_would_overflow(bbi_chunk_t a, bbi_chunk_t b) {
    bbi_chunk_t a_compl = ~a;
    if (b <= a_compl)
        return false;
    return true;
}

//
// Assign and Compare
//

BigInt& BigInt::operator= (bbi_chunk_t n) {
    // Variables tend to hold values of similar sizes, so don't realloc
    // Just zero chunks
    if (data)
        clear();
    else
        data = new bbi_data (INITIAL_CHUNKS);
    (*data)[0] = n;
    return *this;
}

BigInt& BigInt::operator= (const BigInt& other) {
    assert(data);

    bbi_data::size_type other_size = other.size();
    while (data->size() < other_size) {
        expand();
    }
    negative = other.negative;
    for (bbi_data::size_type i = 0; i < other_size; ++i) {
        (*data)[i] = (*other.data)[i];
    }
    return *this;
}

bool BigInt::operator== (bbi_chunk_t n) {
    assert(data);
    return (*data)[0] == n;
}

bool BigInt::operator== (const BigInt& other) {
    assert(data);
    bbi_data::size_type min_size = min(data->size(), other.size());
    for (bbi_data::size_type i = 0; i < min_size; ++i) {
        if ((*data)[i] != (*other.data)[i])
            return false;
    }
    return true;
}

bool BigInt::operator!= (bbi_chunk_t n) {
    return ! this->operator==(n);
}

bool BigInt::operator!= (const BigInt& other) {
    return ! this->operator==(other);
}

bool BigInt::operator< (bbi_chunk_t n) {
    return false;
}

bool BigInt::operator< (const BigInt& other) {
    return false;
}

bool BigInt::operator> (bbi_chunk_t n) {
    return false;
}

bool BigInt::operator> (const BigInt& other) {
    return false;
}

bool BigInt::operator<= (bbi_chunk_t n) {
    return false;
}

bool BigInt::operator<= (const BigInt& other) {
    return false;
}

bool BigInt::operator>= (bbi_chunk_t n) {
    return false;
}

bool BigInt::operator>= (const BigInt& other) {
    return false;
}


//
// Arithmetic
//

// BigInt& BigInt::operator+= (bbi_chunk_t n) {
//     assert(data);
//     BigInt tmp (n);
//     this->operator+=(tmp);
//     return *this;
// }

BigInt& BigInt::operator+= (const BigInt& other) {
    assert(data);
    assert(other.data);

    // Calculate minimum size for result data
    // - start with largest existing size
    //cout << "this = " << this->bits() << endl;
    //cout << "other = " << other.bits() << endl;
    bbi_data::size_type needed_chunks = max(data->size(), other.size());
    while (needed_chunks > data->size()) {
        //cout << "data size = " << data->size() << ", expanding..." << endl;
        expand();
    }
    //cout << "AFTER EXPAND" << endl;
    //cout << "this = " << this->bits() << endl;
    //cout << "other = " << other.bits() << endl;
    //cout << endl;

    bool prev_overflow = false;
    unsigned long i;
    for (i = 0; i < data->size(); ++i) {
        bbi_chunk_t a = (*data)[i];
        bbi_chunk_t b = (*other.data)[i];
        //cout << "a bits = " << bits(a) << endl;
        //cout << "b bits = " << bits(b) << endl;
        (*data)[i] = a + b;
        if (prev_overflow) {
            //cout << "prev overflow true" << endl;
            // Check previous iteration
            // This can't overflow, will always have at least one 0 bit
            (*data)[i]++;
            prev_overflow = false;
        }
        if (addn_would_overflow(a, b)) {
            prev_overflow = true;
            //cout << "set prev overflow" << endl;
        }
    }

    //cout << "doing final, prev overflow = " << prev_overflow << endl;

    bool final_overflow = false;
    if (prev_overflow) {
        if ((*data)[i] == MAX_CHUNK)
            final_overflow = true;
        (*data)[i] += 1;
        if (final_overflow) {
            expand();
            (*data)[i+1] += 1;
        }
    }

    return *this;
}

BigInt& BigInt::operator++ () {
    assert(data);
    this->operator+=(1);
    return *this;
}

BigInt& BigInt::operator-= (const BigInt& other) {
    assert(data);
    assert(other.data);
    return *this;
}

BigInt& BigInt::operator-- () {
    assert(data);
    this->operator-=(1);
    return *this;
}

BigInt& BigInt::operator*= (const BigInt& other) {
    assert(data);
    assert(other.data);
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
        tmp_other >>= 1;
        //cout << "this = " << (*this).bits() << endl << endl;
    }

    return *this;
}

BigInt& BigInt::operator/= (const BigInt& other) {
    assert(data);
    assert(other.data);
    return *this;
}

BigInt BigInt::operator+ (const BigInt& other) {
    assert(data);
    assert(other.data);

    BigInt ret;
    return ret;
}

BigInt BigInt::operator- (const BigInt& other) {
    assert(data);
    assert(other.data);
    BigInt ret;
    return ret;
}

BigInt BigInt::operator* (const BigInt& other) {
    assert(data);
    assert(other.data);
    BigInt ret;
    return ret;
}

BigInt BigInt::operator/ (const BigInt& other) {
    assert(data);
    assert(other.data);
    BigInt ret;
    return ret;
}

//
// Bit ops
//


BigInt BigInt::operator~ () {
    assert(data);

    BigInt ret (*this);
    bbi_data::size_type len = data->size();
    for (bbi_data::size_type i = 0; i < len; ++i) {
        (*ret.data)[i] = ~ (*ret.data)[i];
    }
    return ret;
}

BigInt& BigInt::operator&= (bbi_chunk_t n) {
    return *this;
}

BigInt& BigInt::operator&= (const BigInt& other) {
    return *this;
}

BigInt& BigInt::operator|= (bbi_chunk_t n) {
    return *this;
}

BigInt& BigInt::operator|= (const BigInt& other) {
    return *this;
}

BigInt& BigInt::operator^= (bbi_chunk_t n) {
    return *this;
}

BigInt& BigInt::operator^= (const BigInt& other) {
    assert(data);
    assert(other.data);

    return *this;
}

BigInt& BigInt::operator<<= (bbi_chunk_t n) {
    assert(data);
    if (n == 0)
        return *this;

    //cout << "size before: " << data->size() << endl;
    while (n > (num_free_chunks() * BITS_PER_CHUNK))
        expand();
    //cout << "size after: " << data->size() << endl;

    bbi_chunk_t mask = MASK_LOOKUP_TOP[n];           // TODO 32

    for (long i = data->size()-1; i >= 0; --i) {
        // Save top n bits of chunk
        bbi_chunk_t chunk = (*data)[i];
        bbi_chunk_t top_n = chunk & mask;
        top_n >>= BITS_PER_CHUNK - n;
        // Move those bits to next chunk
        bbi_chunk_t tmp_next = (*data)[i+1];
        tmp_next |= top_n;
        (*data)[i+1] = tmp_next;
        // Shift
        chunk <<= n;
        // Store
        (*data)[i] = chunk;
        //cout << bits() << endl << endl;
    }
    return *this;
}

BigInt& BigInt::operator>>= (bbi_chunk_t n) {
    assert(data);
    if (n == 0)
        return *this;

    bbi_chunk_t mask = MASK_LOOKUP_BOTTOM[n];           // TODO 32

    (*data)[0] >>= n;
    for (unsigned long i = 1; i < data->size(); ++i) {
        bbi_chunk_t chunk = (*data)[i];
        bbi_chunk_t bottom_n = chunk & mask;
        chunk >>= n;
        (*data)[i] = chunk;
        (*data)[i-1] |= (bottom_n << (BITS_PER_CHUNK-n));
    }

    return *this;
}

BigInt BigInt::operator& (bbi_chunk_t n) {
    assert(data);

    BigInt ret ((*data)[0]);
    (*ret.data)[0] &= n;
    return ret;
}

BigInt BigInt::operator& (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator| (bbi_chunk_t n) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator| (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator^ (bbi_chunk_t n) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator^ (const BigInt& other) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator<< (bbi_chunk_t) {
    BigInt ret;
    return ret;
}

BigInt BigInt::operator>> (bbi_chunk_t) {
    BigInt ret;
    return ret;
}


//
// Other stuff
//

bool BigInt::is_zero() const {
    if (data == 0)
        return false;

    for (bbi_data::size_type i = 0; i < data->size(); ++i) {
        if ((*data)[i] != 0) {
            return false;
        }
    }
    return true;
}

bool BigInt::is_negative() const {
    if (data == 0)
        return false;
    return negative;
}

BigInt::operator bool() const {
    return is_zero() ? 0 : &BigInt::this_type_does_not_support_comparisons;
}

bool BigInt::operator! () const {
    return is_zero() ? true : false;
}

// Sometimes want to test a bit without creating a whole new object
int BigInt::get_bit(size_t bit_pos) const {
    assert(data);
    size_t chunk_idx = bit_pos / BITS_PER_CHUNK;
    //cout << "chunk_idx = " << chunk_idx << endl;
    size_t chunk_offset = bit_pos % BITS_PER_CHUNK;
    //cout << "chunk_offset = " << chunk_offset << endl;
    bbi_chunk_t chunk = (*data)[chunk_idx];
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
    assert(data);
    data->resize(data->size() * 2);
}

void BigInt::clear() {
    assert(data);
    bbi_data::size_type len = data->size();
    for (bbi_data::size_type i = 0; i < len; ++i) {
        (*data)[i] = 0;
    }
}

inline bbi_data::size_type BigInt::num_free_chunks() const {
    assert(data);
    bbi_data::size_type ret = 0;
    for (bbi_data::size_type i = data->size()-1; i > 0; --i) {
        if (!(*data)[i])
            ++ret;
    }
    return ret;
}

// int main() {
//     BigInt b ("12345");
//     cout << b.bits() << endl;
// }

