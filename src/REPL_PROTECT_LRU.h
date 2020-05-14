#ifndef REPL_PROTECT_LRU_H
#define REPL_PROTECT_LRU_H

#include <vector>

#include "replacement_state.h"

// Constant values
int NUM_MU = 0;            // Number of protected MU lines per set
#define MAX_COUNTER_VAL 8  // Maximum value of the access counter

typedef struct {
    u32 LRUstackposition;

    // CONTESTANTS: Add extra state per cache line here
    COUNTER accesses;  // only uses log_2 MAX_COUNTER_VAL bits
} LINE_REPLACEMENT_STATE;

class REPL_PROTECT_LRU : public CACHE_REPLACEMENT_STATE {
   private:
    LINE_REPLACEMENT_STATE** repl;

   public:
    REPL_PROTECT_LRU(u32 _sets, u32 _assoc, u32 _pol)
        : CACHE_REPLACEMENT_STATE(_sets, _assoc, _pol) {
        NUM_MU = _assoc / 2;
        InitReplacementState();
    }

    i32 GetVictimInSet(u32 setIndex,
                       const LINE_STATE* vicSet,
                       Addr_t paddr,
                       AccessTypes accessType) {
        // Get pointer to replacement state of current set
        LINE_REPLACEMENT_STATE* replSet = repl[setIndex];

        i32 lruWay = 0;

        // Container of all the valid ways
        vector<u32> validWays;

        // Populate array for all way indices
        for (u32 way = 0; way < assoc; ++way)
            validWays.push_back(way);

        // Remove the N most accessed indices
        for (int i = 0; i < NUM_MU; ++i) {
            COUNTER maxVal = 0;
            int maxValIdx = -1;
            // Scan for first entry with greatest number of accesses
            for (size_t j = 0; j < validWays.size(); ++j)
                if (maxValIdx == -1 ||
                    replSet[validWays[j]].accesses > maxVal) {
                    // Save this entry's position and number of accesses
                    maxValIdx = j;
                    maxVal = replSet[validWays[j]].accesses;
                }
            // If a valid entry was found remove it from the list of valid ways
            if (maxValIdx != -1)
                validWays.erase(validWays.begin() + maxValIdx);
        }

        // Find LRU in remaining valid indices
        u32 currStackPos = 0;
        for (size_t i = 0; i < validWays.size(); ++i) {
            if (replSet[validWays[i]].LRUstackposition > currStackPos) {
                currStackPos = replSet[validWays[i]].LRUstackposition;
                lruWay = validWays[i];
            }
        }

        // Reset the number of accesses in the replaced line
        replSet[lruWay].accesses = 0;
        return lruWay;
    }

    void UpdateLRU(u32 setIndex, i32 updateWayID) {
        // Determine current LRU stack position
        u32 currLRUstackposition = repl[setIndex][updateWayID].LRUstackposition;

        // Update the stack position of all lines before the current line
        // Update implies incremeting their stack positions by one
        for (u32 way = 0; way < assoc; way++) {
            if (repl[setIndex][way].LRUstackposition < currLRUstackposition) {
                repl[setIndex][way].LRUstackposition++;
            }
        }

        // Set the LRU stack position of new line to be zero
        repl[setIndex][updateWayID].LRUstackposition = 0;
    }

    void UpdateReplacementState(u32 setIndex,
                                i32 updateWayID,
                                const LINE_STATE* currLine,
                                AccessTypes accessType,
                                bool cacheHit) {
        // Get pointer to replacement state of current set
        LINE_REPLACEMENT_STATE* replSet = repl[setIndex];

        // Increment the number of accesses
        replSet[updateWayID].accesses++;

        // Test accesses against the threshold
        if (replSet[updateWayID].accesses == MAX_COUNTER_VAL)
            for (u32 way = 0; way < assoc; ++way)
                replSet[way].accesses = replSet[way].accesses >> 1;

        // Update LRU stack
        UpdateLRU(setIndex, updateWayID);
    }

   private:
    void InitReplacementState() {
        // Create the state for sets, then create the state for the ways
        repl = new LINE_REPLACEMENT_STATE*[numsets];

        // ensure that we were able to create replacement state
        assert(repl);

        // Create the state for the sets
        for (u32 setIndex = 0; setIndex < numsets; setIndex++) {
            repl[setIndex] = new LINE_REPLACEMENT_STATE[assoc];

            for (u32 way = 0; way < assoc; way++) {
                // initialize stack position (for true LRU)
                repl[setIndex][way].LRUstackposition = way;
                // initialize number of accesses to 0
                repl[setIndex][way].accesses = 0;
            }
        }
    }
};

#endif
