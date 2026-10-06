#include "recomp_api.h"
#include "reogh_mem.h"
#include "reogh_master.h"

// [Don't Become A Zombie Over Time]  D0667E54 00009FA4 / 20667E5C 00000000
// Guard: 0x00667E54 "jal 0x00667E90". 0x00667E5C "sw s0,0xbac(s1)" stores the virus gauge (+0xBAC) -> nop.
static ReoGH_PatchWrite sNoInfectionOverTimeWrites[] = {
    REOGH_WRITE32(0x00667E5C, 0x00000000),
};
static ReoGH_Patch sNoInfectionOverTime = REOGH_PATCH(0x00667E54, 0x9FA4, sNoInfectionOverTimeWrites);

// [Don't Become A Zombie When Bitten]  D066015C 00009FE4 / 20660160 00000000
// Guard: 0x0066015C "jal 0x00667F90". 0x00660160 (its delay slot) "sw v0,0xbac(s1)" -> nop.
static ReoGH_PatchWrite sNoInfectionWhenBittenWrites[] = {
    REOGH_WRITE32(0x00660160, 0x00000000),
};
static ReoGH_Patch sNoInfectionWhenBitten = REOGH_PATCH(0x0066015C, 0x9FE4, sNoInfectionWhenBittenWrites);

// [1-Hit Deaths (Enemies)]  D06407F4 0000882D / 20640800 A6620138
// Guard: 0x006407F4 "move s1,a2". 0x00640800 "beq v1,v0" (skips enemies at HP 0x7FFF) becomes
// "sh v0,0x138(s3)", a store of the damage 0x7FFF to the halfword at +0x138 of the enemy in s3.
static ReoGH_PatchWrite sOneHitKillsWrites[] = {
    REOGH_WRITE32(0x00640800, 0xA6620138),
};
static ReoGH_Patch sOneHitKills = REOGH_PATCH(0x006407F4, 0x882D, sOneHitKillsWrites);

// [Enemies Don't Damage You With Attacks]  D0660B0C 0000802D / 20660B18 00000000
// Guard: 0x00660B0C "move s0,a0". 0x00660B18 "beqz v1,0x00660B28" (the branch into the HP damage of an enemy attack)
// becomes a nop: never taken, so attacks take no HP from you or the partners. Bites still infect.
static ReoGH_PatchWrite sNoEnemyDamageWrites[] = {
    REOGH_WRITE32(0x00660B18, 0x00000000),
};
static ReoGH_Patch sNoEnemyDamage = REOGH_PATCH(0x00660B0C, 0x802D, sNoEnemyDamageWrites);

// [Become A Zombie After Death (Offline Mode)]  D065FD14 0000202D / 1065FD3A 00001000
// Guard: 0x0065FD14 "move a0,s1". The halfword write puts 0x1000 into the upper half of 0x0065FD38
// "bne v0,v1,0x0065FDB0" (0x1443001D), making it "b 0x0065FDB0": at death the game always takes its online path
// (0x001B02D0 returns 1 offline), so the character comes back as a playable zombie instead of "YOU DIED".
static ReoGH_PatchWrite sZombieAfterDeathWrites[] = {
    REOGH_WRITE16(0x0065FD3A, 0x1000),
};
static ReoGH_Patch sZombieAfterDeath = REOGH_PATCH(0x0065FD14, 0x202D, sZombieAfterDeathWrites);

// [P1 Press L1+Select For Infection 100%], a joker code:
//   D02B081C 0000FBFE / 20667E28 24900000   while the controller reads L1+Select: the replacement
//   D02B081C 0000FFFF / 20667E28 4410B800   when no button is pressed: the original instruction back
// 0x00667E28 "mfc1 s0,f23" (the new gauge value) becomes "addiu s0,a0,0", so the gauge update stores a0 instead;
// the game caps it at the character's maximum: 100 %, which is death. The update runs for every character, so all of
// them are infected to 100 % (verified: all three gauges at their maximum, "YOU DIED", survivors 0/3). L1+Select also
// opens and closes the pause menu, and the gauge is not updated while the game is paused.
// The pnach has no module guard for it (only the controller test). The mod uses the one of
// [Don't Become A Zombie Over Time], D0667E54 00009FA4, the jal 0x2C bytes further in the same function, so nothing is
// written while game.bin is not loaded.
#define REOGH_PAD_L1_SELECT 0xFBFEu
static ReoGH_PatchWrite sInfectionJokerWrites[] = {
    REOGH_WRITE32(0x00667E28, 0x24900000),
};
static ReoGH_Patch sInfectionJoker = REOGH_PATCH(0x00667E54, 0x9FA4, sInfectionJokerWrites);
static u32 sInfectionJokerHeld;

RECOMP_CALLBACK("*", recomp_on_play_main)
void ReoGH_Combat_OnPlayMain(void) {
    // Every cheat needs the master code (reogh_master.h); Off puts the original instructions back.
    // No Infection / Freeze Infection (the owner's saved switch) turns on both zombie codes; each of them also has its
    // own switch.
    u32 noInfection = ReoGH_CheatOn("no_infection");
    u32 overTime = ReoGH_CheatOn("no_zombie_over_time");
    u32 whenBitten = ReoGH_CheatOn("no_zombie_when_bitten");

    ReoGH_PatchSet(&sNoInfectionOverTime, noInfection || overTime);
    ReoGH_PatchSet(&sNoInfectionWhenBitten, noInfection || whenBitten);

    // Infection Freeze Value is not active: the Greatest Hits pnach has no virus gauge code, so the two patches
    // above freeze the gauge at its current value, whatever recomp_get_config_u32("infection_freeze_value") says.
    // (A gauge address exists only in the regular pnach, and this mod takes nothing from that file.)

    ReoGH_PatchSet(&sOneHitKills, ReoGH_CheatOn("one_hit_kills"));
    ReoGH_PatchSet(&sNoEnemyDamage, ReoGH_CheatOn("no_enemy_damage"));
    ReoGH_PatchSet(&sZombieAfterDeath, ReoGH_CheatOn("zombie_after_death"));

    // The joker: L1+Select puts the replacement in, releasing every button puts the original back; any other button
    // leaves it as it is, as the two pnach lines do.
    if (ReoGH_CheatOn("infection_100_joker")) {
        u32 pad = ReoGH_Pad1();

        if (pad == REOGH_PAD_L1_SELECT) {
            sInfectionJokerHeld = 1;
        } else if (pad == REOGH_PAD_NONE) {
            sInfectionJokerHeld = 0;
        }
    } else {
        sInfectionJokerHeld = 0;
    }
    ReoGH_PatchSet(&sInfectionJoker, sInfectionJokerHeld);
}
