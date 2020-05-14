#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <cassert>

#include "replacement_state.h"
#include "utils.h"

class CacheManager {
   private:
    // parameters
    u32 numsets;
    u32 assoc;
    u32 linesize;
    u32 replPolicy;
    bool writeAlloc;

    LINE_STATE** cache;
    CACHE_REPLACEMENT_STATE* cacheReplState;

    // Lookup Parameters
    u32 lineShift;
    u32 indexShift;
    u32 indexMask;

    COUNTER mytimer;

   public:
    CacheManager(u32 _cacheSize,
                 u32 _assoc,
                 u32 _linesize,
                 u32 _pol,
                 bool _writeAlloc);

    bool CacheInspect(Addr_t paddr, AccessTypes accessType);
    bool LookupAndFillCache(Addr_t paddr, AccessTypes accessType);

   private:
    Addr_t GetTag(Addr_t addr) { return ((addr >> lineShift) >> indexShift); }
    u32 GetSetIndex(Addr_t addr) { return ((addr >> lineShift) & indexMask); }

    void InitCache();
    void InitCacheReplacementState();

    i32 LookupSet(u32 setIndex, Addr_t tag);
    i32 GetVictimInSet(u32 setIndex, Addr_t paddr, AccessTypes accessType);
};

#endif
