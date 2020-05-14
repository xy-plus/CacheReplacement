#ifndef REPL_RANDOM_H
#define REPL_RANDOM_H

#include "replacement_state.h"

// The implementation for the cache replacement policy
class REPL_RANDOM : public CACHE_REPLACEMENT_STATE {
   public:
    REPL_RANDOM(u32 _sets, u32 _assoc, u32 _pol)
        : CACHE_REPLACEMENT_STATE(_sets, _assoc, _pol) {
        InitReplacementState();
    }

    i32 GetVictimInSet(u32 setIndex,
                       const LINE_STATE* vicSet,
                       Addr_t paddr,
                       AccessTypes accessType) {
        int a = rand();
        return (a % assoc);
    }

    void UpdateReplacementState(u32 setIndex,
                                i32 updateWayID,
                                const LINE_STATE* currLine,
                                AccessTypes accessType,
                                bool cacheHit) {}

   private:
    void InitReplacementState() {}
};

#endif
