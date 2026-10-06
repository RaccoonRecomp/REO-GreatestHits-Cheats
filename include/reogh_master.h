#ifndef __REOGH_MASTER_H__
#define __REOGH_MASTER_H__

#include "recomp_api.h"

// The owner's master code for this cheat set, kept verbatim:
//
//   Enable Code (Must Be On) by Code Master
//   Codebreaker v7+
//   B4336FA9 4DFEFB79
//   62838F11 EF8D0773
//   3945B15D EB4129EF
//   535F17F1 63E46A24
//
// On a CodeBreaker the master code installs the device's hook into the game so that the device's code engine runs
// every frame; without it no cheat works. The recompiled game has no cheat device, and writing a hook into the game's
// code would take the recompiled code out of native execution, so the hook is never written. The code is this mod's
// master enable instead: the "master_code" option (default On) must be On for any cheat of this mod to apply, and
// turning it Off switches every cheat off and puts the game's original code back (see README).
#define REOGH_MASTER_KEY "master_code"

static inline u32 ReoGH_MasterOn(void) {
    return recomp_get_config_u32(REOGH_MASTER_KEY) != 0;
}

// A cheat's switch together with the master code: 1 only while both are On.
static inline u32 ReoGH_CheatOn(const char* key) {
    return ReoGH_MasterOn() && recomp_get_config_u32(key) != 0;
}

// A choice option (an Enum: the index of the selected choice, 0 = the first, "Off") together with the master code:
// 0 while the master code is Off.
static inline u32 ReoGH_Choice(const char* key) {
    return ReoGH_MasterOn() ? recomp_get_config_u32(key) : 0;
}

#endif
