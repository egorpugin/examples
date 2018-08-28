#include "server_common.h"

StringBitset::StringBitset(uint64_t v)
{
    // consider endianness
#ifdef BIG_ENDIAN
#else
    int bits = sizeof(v) * CHAR_BIT;
    for (int i = 0; i < sizeof(v); i++)
    {
        set(i, (std::byte)v);
        v >>= 8;
    }
#endif
}

StringBitset::StringBitset(const String &s)
    : rights(s)
{
}

StringBitset::bitref StringBitset::operator[](int p)
{
    return bitref(this, p);
}

StringBitset::bit StringBitset::operator[](int p) const
{
    return get(p);
}

StringBitset::bit StringBitset::get(int p) const
{
    int byte = p / CHAR_BIT;
    int byte_bit = p % CHAR_BIT;
    if (rights.size() <= byte)
        return false;
    return rights[byte] & (1 << byte_bit);
}

void StringBitset::set(int p, bit v)
{
    int byte = p / CHAR_BIT;
    int byte_bit = p % CHAR_BIT;
    if (rights.size() <= byte)
        rights.resize(byte + 1);
    if (v)
        rights[byte] |= (1 << byte_bit);
    else
        rights[byte] &= (1 << byte_bit);
}

void StringBitset::set(int p, std::byte v)
{
    if (rights.size() <= p)
        rights.resize(p + 1);
    rights[p] = (char)v;
}
