#ifndef REPL_PLRU_H
#define REPL_PLRU_H

#include <cstring>

#include "replacement_state.h"

i32 treeSize;
i32 treeHeight;

class PLRUTree {
   private:
    u8* tree;

   public:
    PLRUTree() {
        i32 len = treeSize / 8;
        if (len == 0)
            len = 1;
        tree = new u8[len];
        memset(tree, 0, len * sizeof(u8));
    }

    ~PLRUTree() { delete[] tree; }

    bool get_bit(i32 pos) { return ((tree[pos / 8] >> (pos % 8)) & 1) == 1; }
    void set_bit(i32 pos, bool flag) {
        if (flag) {
            tree[pos / 8] |= (((u8)(1)) << (pos % 8));
        } else {
            tree[pos / 8] &= (~(((u8)(1)) << (pos % 8)));
        }
    }

    i32 getVictim() {
        i32 ind = 0;
        i32 ret = 0;
        for (i32 i = 0; i < treeHeight; ++i) {
            ret <<= 1;
            if (!get_bit(ind)) {
                ind = ind * 2 + 1;
            } else {
                ind = (ind + 1) * 2;
                ret |= 1;
            }
        }
        return ret;
    }

    void update(u32 wayID) {
        bool num[treeHeight];
        for (i32 i = 0; i < treeHeight; ++i) {
            num[i] = ((wayID & 1) == 1);
            wayID >>= 1;
        }
        i32 ind = 0;
        for (i32 i = treeHeight - 1; i >= 0; --i) {
            set_bit(ind, !num[i]);
            if (!num[i]) {
                ind = ind * 2 + 1;
            } else {
                ind = (ind + 1) * 2;
            }
        }
    }
};

class REPL_PLRU : public CACHE_REPLACEMENT_STATE {
   private:
    PLRUTree* repl;

   public:
    REPL_PLRU(u32 _sets, u32 _assoc, u32 _pol)
        : CACHE_REPLACEMENT_STATE(_sets, _assoc, _pol) {
        treeSize = _assoc;
        treeHeight = FloorLog2(_assoc);
        InitReplacementState();
    }

    i32 GetVictimInSet(u32 setIndex,
                       const LINE_STATE* vicSet,
                       Addr_t paddr,
                       AccessTypes accessType) {
        return repl[setIndex].getVictim();
    }

    void UpdateReplacementState(u32 setIndex,
                                i32 updateWayID,
                                const LINE_STATE* currLine,
                                AccessTypes accessType,
                                bool cacheHit) {
        repl[setIndex].update(updateWayID);
    }

   private:
    void InitReplacementState() { repl = new PLRUTree[numsets]; }
};

#endif
