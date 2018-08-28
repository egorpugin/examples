#pragma once

#include <primitives/string.h>

struct StringBitset
{
    using bit = bool;

    struct bitref
    {
        bitref(StringBitset *r, int pos) : r(r), pos(pos) {}

        bitref &operator=(bool v)
        {
            r->set(pos, v);
            return *this;
        }

        operator bool() const { return r->operator[](pos); }

    private:
        StringBitset *r;
        int pos;
    };

    StringBitset() = default;
    StringBitset(uint64_t);
    StringBitset(const String &s);

    String toString() const { return rights; }

    bool empty() const { return rights.empty(); }

    bitref operator[](int p);
    bit operator[](int p) const;

    bit get(int p) const;
    void set(int p, bit v);
    void set(int p, std::byte v);

private:
    String rights;
};
