#ifndef REPL_STATE_H
#define REPL_STATE_H

#include <cassert>
#include <cstring>

#include "utils.h"

const u32 QUEUE_LENGTH = 256;
// Replacement Policies Supported
typedef enum {
    REPL_LRU_T,
    REPL_RANDOM_T,
    REPL_PLRU_T,
    REPL_PROTECT_LRU_T,
    REPL_CONTESTANT_T
} ReplacemntPolicy;

// typedef struct {
//     i32 queue[QUEUE_LENGTH];
//     u32 head, size;
// } WANDQueue;

// The implementation for the cache replacement policy
class CACHE_REPLACEMENT_STATE {
   protected:
    u32 numsets;
    u32 assoc;
    u32 replPolicy;

    // LINE_REPLACEMENT_STATE** repl;

    COUNTER mytimer;  // tracks # of references to the cache

    // CONTESTANTS:  Add extra state for cache here
    // WANDQueue* wandQueue;
    // u32** wandVis;

   public:
    // The constructor CAN NOT be changed
    CACHE_REPLACEMENT_STATE(u32 _sets, u32 _assoc, u32 _pol) {
        numsets = _sets;
        assoc = _assoc;
        replPolicy = _pol;
        mytimer = 0;
    }

    virtual i32 GetVictimInSet(u32 setIndex,
                               const LINE_STATE* vicSet,
                               Addr_t paddr,
                               AccessTypes accessType) = 0;

    virtual void UpdateReplacementState(u32 setIndex,
                                        i32 updateWayID,
                                        const LINE_STATE* currLine,
                                        AccessTypes accessType,
                                        bool cacheHit) = 0;

    void SetReplacementPolicy(u32 _pol) { replPolicy = _pol; }
    void IncrementTimer() { mytimer++; }

   protected:
    virtual void InitReplacementState() = 0;
};

#endif
