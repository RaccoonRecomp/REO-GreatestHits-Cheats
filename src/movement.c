#include "recomp_api.h"
#include "reogh_mem.h"
#include "reogh_master.h"

// [Walk Through Walls & On Air R2 Up On/R2 Down Off] (47iscool), a joker code:
//   E002FDEF 002B081C / 206636D0 03E00008 / 206636D4 00000000   R2+Up: the collision function returns at once
//   E002FDBF 002B081C / 206636D0 27BDFFC0 / 206636D4 FFBF0020   R2+Down: its first two instructions back
// 0x006636D0 "addiu sp,sp,-0x40" / 0x006636D4 "sd ra,0x20(sp)" become "jr ra" / "nop": player 1's collision and floor
// height are no longer applied (walls and objects stop blocking; the height stays where it was, "on air").
// Verified with the real button presses: R2+Up reads 0xFDEF, R2+Down 0xFDBF; while on, Kevin walks 5.4 units past the
// point where the game stops him and his height stays frozen; after R2+Down the game's collision pushes him back and
// every value matches the run without the cheat (walls_joker, walls_joker_base). The author notes it works most of
// the time.
// The pnach has no module guard for it (only the controller test). The mod adds one read from this disc:
// 0x006636E4 "beqz v1,0x00663968" (0x106000A0), an instruction of the same function, so nothing is written while
// game.bin is not loaded.
#define REOGH_PAD_R2_UP   0xFDEFu
#define REOGH_PAD_R2_DOWN 0xFDBFu
static ReoGH_PatchWrite sWalkThroughWallsWrites[] = {
    REOGH_WRITE32(0x006636D0, 0x03E00008),
    REOGH_WRITE32(0x006636D4, 0x00000000),
};
static ReoGH_Patch sWalkThroughWalls = REOGH_PATCH_GUARD32(0x006636E4, 0x106000A0, sWalkThroughWallsWrites);
static u32 sWalkThroughWallsOn;

RECOMP_CALLBACK("*", recomp_on_play_main)
void ReoGH_Movement_OnPlayMain(void) {
    // Every cheat needs the master code (reogh_master.h). With the switch On, R2+Up turns the code on and R2+Down turns
    // it off again, as the joker does; with the switch Off (or the master code Off) the original code is put back and
    // the joker starts from off.
    if (ReoGH_CheatOn("walk_through_walls")) {
        u32 pad = ReoGH_Pad1();

        if (pad == REOGH_PAD_R2_UP) {
            sWalkThroughWallsOn = 1;
        } else if (pad == REOGH_PAD_R2_DOWN) {
            sWalkThroughWallsOn = 0;
        }
    } else {
        sWalkThroughWallsOn = 0;
    }
    ReoGH_PatchSet(&sWalkThroughWalls, sWalkThroughWallsOn);
}
