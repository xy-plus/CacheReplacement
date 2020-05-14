#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

#define u32 unsigned int
#define u8 uint8_t
#define i32 int
#define ADDRINT unsigned long long
#define COUNTER unsigned long long
#define BITVECTOR unsigned long long
#define Addr_t unsigned long long

// Works by finding position of MSB set.
inline i32 FloorLog2(u32 n) {
    i32 p = 0;

    if (n == 0)
        return -1;

    if (n & 0xffff0000) {
        p += 16;
        n >>= 16;
    }
    if (n & 0x0000ff00) {
        p += 8;
        n >>= 8;
    }
    if (n & 0x000000f0) {
        p += 4;
        n >>= 4;
    }
    if (n & 0x0000000c) {
        p += 2;
        n >>= 2;
    }
    if (n & 0x00000002) {
        p += 1;
    }

    return p;
}

// Works by finding position of MSB set.
// @returns -1 if n == 0.
inline i32 CeilLog2(u32 n) {
    return FloorLog2(n - 1) + 1;
}

extern u32 tagLen;

#define stateLen (8)

class LINE_STATE {
   private:
    u8* src;

   public:
    LINE_STATE() {
        if (tagLen < 63)
            src = new u8[stateLen];
        else {
            src = new u8[stateLen + 1];
        }
    }

    bool isValid() {
        if (tagLen < 63)
            return (*(ADDRINT*)(src)) >> 63;
        else
            return src[8] & 1;
    }
    bool isDirty() {
        if (tagLen < 63)
            return (*(ADDRINT*)(src)) >> 62;
        else
            return (src[8] >> 1) & 1;
    }

    void setValid(bool flag) {
        if (tagLen < 63) {
            if (flag)
                (*(ADDRINT*)(src)) |= (((ADDRINT)(1)) << 63);
            else
                (*(ADDRINT*)(src)) &= (~(((ADDRINT)(1)) << 63));
        } else {
            if (flag)
                src[8] |= 1;
            else
                src[8] &= (~(u8)(1));
        }
    }

    void setDirty(bool flag) {
        if (tagLen < 63) {
            if (flag)
                (*(ADDRINT*)(src)) |= (((ADDRINT)(1)) << 62);
            else
                (*(ADDRINT*)(src)) &= (~(((ADDRINT)(1)) << 62));
        } else {
            if (flag)
                src[8] |= 2;
            else
                src[8] &= (~(u8)(2));
        }
    }

    ADDRINT getTag() {
        if (tagLen < 63) {
            ADDRINT mask = ((ADDRINT)(1) << (tagLen + 1)) - 1;
            return mask & (*(ADDRINT*)(src));
        } else {
            return (*(ADDRINT*)(src));
        }
    }

    void setTag(ADDRINT addr) {
        bool v = isValid();
        bool d = isDirty();
        (*(ADDRINT*)(src)) = addr;
        setValid(v);
        setDirty(d);
    }
};

typedef enum {
    ACCESS_LOAD,
    ACCESS_STORE,
    ACCESS_WRITEBACK,
    ACCESS_UNSUPPORT0,
} AccessTypes;

#define IS_STORE(X) (X == ACCESS_STORE || X == ACCESS_WRITEBACK)

#endif
