#include "recomp_api.h"
#include "reogh_mem.h"
#include "reogh_master.h"

// [Character 2 (CPU) Modifier] (47iscool)  004A78A8 000000??   (8-bit write)
// [Character 3 (CPU) Modifier] (47iscool)  004A8988 000000??   (8-bit write)
// [Go It Alone] (47iscool)                 004A78A8 000000FF / 004A8988 000000FF
// The byte is the character type (+0xB98) of player 2's and player 3's block (player 1's block is 0x004A5C30, stride
// 0x10E0; the blocks lie in submain.bin's memory, which stays loaded). The type values are the game's own, read from
// this disc: the executable's name table 0x00235430 lists KEVIN, MARK, JIM, GEORGE, DAVID, ALYSSA, YOKO, CINDY for
// types 0-7 (game.bin 0x006DFDD8 looks a name up with "lbu a0,0xb98(a0)"), and game.bin 0x0066A8C4 sets a new
// character's HP from the table 0x0071B020 by the same byte: 2300, 3000, 1800, 2100, 2200, 2000, 1300, 1500.
// Verified in the game: with types 4/6, 2/3, 5/4 and 0/0xFF the partners came in with exactly those characters' HP
// and names (David, Yoko, Jim, George, Alyssa, a second Kevin), and 0xFF gave no partner at all.
// 0xFF is what Go It Alone writes: the game then never creates that partner.
//
// The game writes the partners' types when a scenario is set up and creates the partners from them when the scenario
// starts. The mod writes the byte only while that partner has not been created yet (its max HP, +0x546, is still 0),
// so a choice applies from the next scenario on and a partner is never changed in the middle of a scenario.
// Max HP goes back to 0 before every later scenario of a session, not only after boot: game.bin's reset 0x005EAEB0
// clears all player blocks (0x0066A730: 0x004A5C30, 0x8700 bytes) when a scenario is left and again when the next one
// is set up (0x005EB210), right before 0x005EB0D0 writes the selected characters' bytes. Traced in the game: the clear
// at the quit after YOU DIED and at the second setup, and a choice switched on only after the first scenario gave the
// chosen partners in the second (README, "Evidence", 1.2.1).
// A partner that is never created (FF, Go It Alone) keeps max HP 0, so its byte is written all through the scenario,
// as the pnach does; writing a character there in the middle of a scenario creates no partner (run mid_absent).
#define REOGH_P2_TYPE   0x004A78A8u
#define REOGH_P3_TYPE   0x004A8988u
#define REOGH_P2_MAX_HP 0x004A7256u
#define REOGH_P3_MAX_HP 0x004A8336u
#define REOGH_NO_PARTNER 0xFFu

// The choices of the two options, in order: "Off" (no write), the eight characters (types 0-7), "No partner (FF)".
#define REOGH_CHOICE_OFF        0u
#define REOGH_CHOICE_NO_PARTNER 9u

static u32 ReoGH_PartnerType(u32 choice) {
    if (choice >= 1u && choice <= 8u) {
        return choice - 1u;
    }
    if (choice == REOGH_CHOICE_NO_PARTNER) {
        return REOGH_NO_PARTNER;
    }
    return 0x100u; // Off (or an unknown choice): no write
}

static void ReoGH_SetPartner(u32 typeAddr, u32 maxHpAddr, u32 type) {
    if (type > 0xFFu) {
        return;
    }
    if (ReoGH_Read(maxHpAddr, 2) != 0) {
        return; // this partner exists: never changed in the middle of a scenario
    }
    if (ReoGH_Read(typeAddr, 1) != type) {
        ReoGH_Write(typeAddr, 1, type);
    }
}

RECOMP_CALLBACK("*", recomp_on_play_main)
void ReoGH_Partners_OnPlayMain(void) {
    // Every cheat needs the master code (reogh_master.h). Go It Alone writes 0xFF for both partners, whatever the two
    // modifiers are set to. Switched off, nothing is written: the game sets its own partners when it sets up the next
    // scenario (proven for a later scenario of the same session: run s2_off_after_46).
    u32 alone = ReoGH_CheatOn("go_it_alone");
    u32 p2 = ReoGH_PartnerType(ReoGH_Choice("character_2"));
    u32 p3 = ReoGH_PartnerType(ReoGH_Choice("character_3"));

    if (alone) {
        p2 = REOGH_NO_PARTNER;
        p3 = REOGH_NO_PARTNER;
    }
    ReoGH_SetPartner(REOGH_P2_TYPE, REOGH_P2_MAX_HP, p2);
    ReoGH_SetPartner(REOGH_P3_TYPE, REOGH_P3_MAX_HP, p3);
}
