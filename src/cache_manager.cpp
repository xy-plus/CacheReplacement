#include "cache_manager.h"

#include "REPL_LRU.h"
#include "REPL_PLRU.h"
#include "REPL_PROTECT_LRU.h"
#include "REPL_RANDOM.h"

u32 tagLen;

CacheManager::CacheManager(u32 _cacheSize,
                           u32 _assoc,
                           u32 _linesize,
                           u32 _pol,
                           bool _writeAlloc) {
    // Start off with empty cache and replacement state
    cache = NULL;
    cacheReplState = NULL;

    // Initialize parameters to the cache
    numsets = _cacheSize / (_linesize * _assoc);
    assoc = _assoc;
    linesize = _linesize;

    replPolicy = _pol;
    writeAlloc = _writeAlloc;

    // Initialize the cache
    InitCache();

    // Initialize Replacement State
    InitCacheReplacementState();
}

void CacheManager::InitCache() {
    // Initialize the Cache Access Functions
    lineShift = FloorLog2(linesize);
    indexShift = FloorLog2(numsets);
    tagLen = 64 - indexShift;
    indexMask = (1 << indexShift) - 1;

    // Create the cache structure (first create the sets)
    cache = new LINE_STATE*[numsets];

    // ensure that we were able to create cache
    assert(cache);

    // If we were able to create the sets, now create the ways
    for (u32 setIndex = 0; setIndex < numsets; setIndex++) {
        cache[setIndex] = new LINE_STATE[assoc];
    }

    // Initialize cache access timer
    mytimer = 0;
}

i32 CacheManager::GetVictimInSet(u32 setIndex,
                                 Addr_t paddr,
                                 AccessTypes accessType) {
    // Get pointer to replacement state of current set
    LINE_STATE* vicSet = cache[setIndex];

    // First find and fill invalid lines
    for (u32 way = 0; way < assoc; way++) {
        if (vicSet[way].isValid() == false) {
            return way;
        }
    }

    // If no invalid lines, then replace based on replacement policy
    return cacheReplState->GetVictimInSet(setIndex, vicSet, paddr, accessType);
}

// The function looks up the set for the tag
// returns physical way index if the tag was a hit
// returns -1 if it was a miss
i32 CacheManager::LookupSet(u32 setIndex, Addr_t tag) {
    // Get pointer to current set
    LINE_STATE* currSet = cache[setIndex];

    // Find Tag
    for (u32 way = 0; way < assoc; way++) {
        if (currSet[way].isValid() && (currSet[way].getTag() == tag)) {
            return way;
        }
    }

    // If not found, return -1
    return -1;
}

// This function is responsible for looking up and filling a line in the
// cache. If the result is a miss, the function consults the replacement
// policy for a victim candidate, inserts the missing line in the cache
// and consults the replacement policy for replacement state update. If
// the result was a cache hit, the replacement policy again is consulted
// to determine how to update the replacement state.
bool CacheManager::LookupAndFillCache(Addr_t paddr, AccessTypes accessType) {
    LINE_STATE* currLine = NULL;

    // for modeling LRU
    ++mytimer;
    cacheReplState->IncrementTimer();

    // Process request
    bool hit = true;
    u32 setIndex = GetSetIndex(paddr);  // Get the set index
    Addr_t tag = GetTag(paddr);         // Determine Cache Tag

    // Lookup the cache set to determine whether line is already in cache or not
    i32 wayID = LookupSet(setIndex, tag);

    if (wayID == -1) {
        hit = false;
        // get victim line to replace (wayID = -1, then bypass)
        if (IS_STORE(accessType) && !writeAlloc)
            ;
        else {
            wayID = GetVictimInSet(setIndex, paddr, accessType);
            if (wayID != -1) {
                currLine = &cache[setIndex][wayID];

                // Update the line state accordingly
                currLine->setValid(true);
                currLine->setTag(tag);
                // currLine->setDirty(IS_STORE(accessType));

                // Update Replacement State
                cacheReplState->UpdateReplacementState(
                    setIndex, wayID, currLine, accessType, hit);
            }
        }
    } else {
        // get pointer to cache line we hit
        currLine = &cache[setIndex][wayID];

        // Update the line state accordingly
        // currLine->setDirty(IS_STORE(accessType));

        // Update Replacement State
        cacheReplState->UpdateReplacementState(setIndex, wayID, currLine,
                                               accessType, hit);
    }

    return hit;
}

void CacheManager::InitCacheReplacementState() {
    if (replPolicy == REPL_LRU_T)
        cacheReplState = new REPL_LRU(numsets, assoc, replPolicy);
    else if (replPolicy == REPL_RANDOM_T) {
        cacheReplState = new REPL_RANDOM(numsets, assoc, replPolicy);
    } else if (replPolicy == REPL_PLRU_T) {
        cacheReplState = new REPL_PLRU(numsets, assoc, replPolicy);
    } else if (replPolicy == REPL_PROTECT_LRU_T) {
        cacheReplState = new REPL_PROTECT_LRU(numsets, assoc, replPolicy);
    } else {
        cout << "undefine repl" << endl;
        exit(-1);
    }
}
