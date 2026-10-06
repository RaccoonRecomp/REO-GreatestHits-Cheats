#include "recomp_api.h"
#include "reogh_mem.h"
#include "reogh_master.h"

// [Infinite Health] (Code Master)  pnach:  E0023881 00660794 / 20660800 2403270F / 20660808 AE030544
//                                  used:   E0024200 006607F4 / 20660800 2403270F / 20660808 A6030544
// Two fixes for this disc, both verified in the game (CHEATS_DESIGN.md section 4):
// - Guard. The pnach moved the writes to the Greatest Hits addresses but kept the regular release's guard address
//   0x00660794, which on this disc is "mfhi v0" (0x1010) in another function, so the code never ran. 0xC before the
//   first write, as in the regular code, is 0x006607F4 "lh v1,0x4200(at)" = 0x84234200: guard halfword 0x4200.
// - Store. 0x00660808 was "sw v1,0x544(s0)", a 32-bit store that also writes 0 into max HP (+0x546); after the cheat
//   is switched off, the next hit leaves the character dying. "sh v1,0x544(s0)" (0xA6030544) stores HP only.
// 0x00660800 = "lh v1,0x544(s0)" becomes "li v1,9999"; 0x00660808 is the nop in the delay slot of the death check's
// branch, and becomes the HP store. The check runs for you and the CPU partners.
static ReoGH_PatchWrite sInfiniteHealthWrites[] = {
    REOGH_WRITE32(0x00660800, 0x2403270F),
    REOGH_WRITE32(0x00660808, 0xA6030544),
};
static ReoGH_Patch sInfiniteHealth = REOGH_PATCH(0x006607F4, 0x4200, sInfiniteHealthWrites);

// The three ammo codes share a guard, D05B1FF8 00001E3C ("dsll32 v1,s2,0x18", the CodeBreaker line 28E323AA DFEA4D9A
// they all begin with), and the same two instructions of the "use one round" path:
//   0x005B2004 bne v1,v0,0x005B2010   (v0 = 1: taken unless a round is used)
//   0x005B2008 addiu v0,a0,-1         (delay slot: the new count, a0 = the count)
//   0x005B200C sh v0,0x24(s0)         (stores the new count)
//   0x005B2010 lh v0,0x24(s0)         (v0 is reloaded here, on both paths)
// [Infinite Ammo] (Code Master)  205B200C 00000000: the store becomes a nop, the count never changes.
static ReoGH_PatchWrite sInfiniteAmmoWrites[] = {
    REOGH_WRITE32(0x005B200C, 0x00000000),
};
static ReoGH_Patch sInfiniteAmmo = REOGH_PATCH(0x005B1FF8, 0x1E3C, sInfiniteAmmoWrites);

// [Extra Ammo]  105B2008 00000001: the low halfword of "addiu v0,a0,-1" (0x2482FFFF) becomes 1, "addiu v0,a0,1": every
// shot adds a round (verified: 7 -> 19 over 12 shots).
static ReoGH_PatchWrite sExtraAmmoWrites[] = {
    REOGH_WRITE16(0x005B2008, 0x0001),
};
static ReoGH_Patch sExtraAmmo = REOGH_PATCH(0x005B1FF8, 0x1E3C, sExtraAmmoWrites);

// [Max Infinite Ammo]  pnach: 205B2008 24030063 ("addiu v1,zero,99") / used: 205B2008 24020063 ("addiu v0,zero,99")
// Fixed for this disc: the store at 0x005B200C writes v0, not v1. As written, v1 gets 99 and v0 keeps the 1 loaded at
// 0x005B1FFC, so the count drops to 1 and stays there (verified, ammo_max). With v0 the store writes 99: the count is
// set to 99 at the first shot and stays 99 (verified, ammo_max_fix). v0 is dead after the store on both paths (it is
// reloaded at 0x005B2010) and v1 is left as the game has it, so nothing else changes.
static ReoGH_PatchWrite sMaxInfiniteAmmoWrites[] = {
    REOGH_WRITE32(0x005B2008, 0x24020063),
};
static ReoGH_Patch sMaxInfiniteAmmo = REOGH_PATCH(0x005B1FF8, 0x1E3C, sMaxInfiniteAmmoWrites);

// [Infinite Item Usage]  pnach: E0031E3D 005B2178 / used: E0031E3C 005B2178; writes 205B218C, 205B21A0, 205B51B4 = 0
// Guard fix: 0x005B2178 holds "dsll32 v1,s2,0x18" = 0x00121E3C, the same instruction the Infinite Ammo guard tests
// 0x180 bytes earlier. 0x1E3D is one too high (it would not even be a valid instruction), so the code never ran.
// 0x005B218C "sh v0,0x24(s0)" (the new count) -> nop; 0x005B21A0 "bne v0,v1" -> nop; 0x005B51B4 "move v0,zero" -> nop.
static ReoGH_PatchWrite sInfiniteItemsWrites[] = {
    REOGH_WRITE32(0x005B218C, 0x00000000),
    REOGH_WRITE32(0x005B21A0, 0x00000000),
    REOGH_WRITE32(0x005B51B4, 0x00000000),
};
static ReoGH_Patch sInfiniteItems = REOGH_PATCH(0x005B2178, 0x1E3C, sInfiniteItemsWrites);

// [Infinite Time]  D068C77C 00001821 / 2068C784 00000000, as written.
// 0x0068C784 "sw v1,(s3)" becomes a nop. It lies in game.bin's ONLINE event-script interpreter: in offline play the
// game hands over to the offline interpreter (0x007006D0) before this point, so the site is never reached and the code
// has no effect offline (CHEATS_DESIGN.md 3.3: no write by 0x0068C784 in a whole single-play session). It acts only in
// online play, which this recompilation does not offer yet. Nothing else is done in its place.
static ReoGH_PatchWrite sInfiniteTimeWrites[] = {
    REOGH_WRITE32(0x0068C784, 0x00000000),
};
static ReoGH_Patch sInfiniteTime = REOGH_PATCH(0x0068C77C, 0x1821, sInfiniteTimeWrites);

// recomp_on_play_main's arguments are not used, so none are declared.
RECOMP_CALLBACK("*", recomp_on_play_main)
void ReoGH_Infinite_OnPlayMain(void) {
    u32 maxAmmo;
    u32 extraAmmo;
    u32 infiniteAmmo;

    // Every cheat needs the master code (reogh_master.h); Off puts the original instructions back.
    ReoGH_PatchSet(&sInfiniteHealth, ReoGH_CheatOn("infinite_health"));

    // The three ammo cheats change the same instructions, so only one of them is ever in the game's code: Max Infinite
    // Ammo, else Extra Ammo, else Infinite Ammo. The ones not chosen are put back first (Extra Ammo and Max Infinite
    // Ammo write the same word), then the chosen one is applied.
    maxAmmo = ReoGH_CheatOn("max_infinite_ammo");
    extraAmmo = ReoGH_CheatOn("extra_ammo") && !maxAmmo;
    infiniteAmmo = ReoGH_CheatOn("infinite_ammo") && !maxAmmo && !extraAmmo;
    if (!maxAmmo) {
        ReoGH_PatchRestore(&sMaxInfiniteAmmo);
    }
    if (!extraAmmo) {
        ReoGH_PatchRestore(&sExtraAmmo);
    }
    if (!infiniteAmmo) {
        ReoGH_PatchRestore(&sInfiniteAmmo);
    }
    if (maxAmmo) {
        ReoGH_PatchApply(&sMaxInfiniteAmmo);
    } else if (extraAmmo) {
        ReoGH_PatchApply(&sExtraAmmo);
    } else if (infiniteAmmo) {
        ReoGH_PatchApply(&sInfiniteAmmo);
    }

    ReoGH_PatchSet(&sInfiniteItems, ReoGH_CheatOn("infinite_items"));
    ReoGH_PatchSet(&sInfiniteTime, ReoGH_CheatOn("infinite_time"));
}
