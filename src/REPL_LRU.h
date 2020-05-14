#ifndef REPL_LRU_H
#define REPL_LRU_H

#include "replacement_state.h"

u32 lruElemNum;
u32 lruElemLen;

class LRUQueue {
   private:
    u8* queue;

   public:
    LRUQueue() {
        u32 len = (lruElemNum * lruElemLen + 7) / 8;
        if (len == 0)
            len = 1;
        queue = new u8[len];
    }

    ~LRUQueue() { delete[] queue; }

    u8 get_bit(u32 pos) { return (queue[pos / 8] >> (pos % 8)) & 1; }

    u32 get(u32 pos) {
        u32 ret = 0;
        for (i32 i = pos * lruElemLen + lruElemLen - 1;
             i >= i32(pos * lruElemLen); --i) {
            ret <<= 1;
            ret |= get_bit(i);
        }
        return ret;
    }

    void set_bit(u32 pos, bool flag) {
        if (flag)
            queue[pos / 8] |= (((u8)(1)) << (pos % 8));
        else
            queue[pos / 8] &= (~(((u8)(1)) << (pos % 8)));
    }

    void set(u32 pos, u32 data) {
        for (u32 i = pos * lruElemLen; i < pos * lruElemLen + lruElemLen; ++i) {
            if (data & 1) {
                set_bit(i, true);
            } else
                set_bit(i, false);
            data >>= 1;
        }
    }
};

// Replacement State Per Cache Line
// typedef struct {
//     u32 LRUstackposition;
//     // CONTESTANTS: Add extra state per cache line here
// } LINE_REPLACEMENT_STATE;

// The implementation for the cache replacement policy
class REPL_LRU : public CACHE_REPLACEMENT_STATE {
   private:
    LRUQueue* repl;

   public:
    REPL_LRU(u32 _sets, u32 _assoc, u32 _pol)
        : CACHE_REPLACEMENT_STATE(_sets, _assoc, _pol) {
        lruElemNum = _assoc;
        lruElemLen = FloorLog2(_assoc);
        if (lruElemLen == 0)
            lruElemLen = 1;
        InitReplacementState();
    }

    i32 GetVictimInSet(u32 setIndex,
                       const LINE_STATE* vicSet,
                       Addr_t paddr,
                       AccessTypes accessType) {
        // Get pointer to replacement state of current set

        i32 lruWay = 0;
        // Search for victim whose stack position is assoc-1
        for (u32 way = 0; way < assoc; way++) {
            if (repl[setIndex].get(way) == (assoc - 1)) {
                lruWay = way;
                break;
            }
        }
        return lruWay;
    }

    void UpdateReplacementState(u32 setIndex,
                                i32 updateWayID,
                                const LINE_STATE* currLine,
                                AccessTypes accessType,
                                bool cacheHit) {
        // Determine current LRU stack position
        u32 currLRUstackposition = repl[setIndex].get(updateWayID);

        // Update the stack position of all lines before the current line
        // Update implies incremeting their stack positions by one
        for (u32 way = 0; way < assoc; way++) {
            u8 t = repl[setIndex].get(way);
            if (t < currLRUstackposition) {
                ++t;
                repl[setIndex].set(way, t);
            }
        }

        // Set the LRU stack position of new line to be zero
        repl[setIndex].set(updateWayID, 0);
    }

   private:
    void InitReplacementState() {
        // Create the state for sets, then create the state for the ways
        repl = new LRUQueue[numsets];

        // Create the state for the sets
        for (u32 setIndex = 0; setIndex < numsets; setIndex++) {
            // repl[setIndex] = new LINE_REPLACEMENT_STATE[assoc];

            for (u32 way = 0; way < assoc; way++) {
                // initialize stack position (for true LRU)
                repl[setIndex].set(way, way);
            }
        }
    }
};

#endif
