#include "recomp_api.h"
#include "reogh_mem.h"
#include "reogh_master.h"

// Save data. Every word below lies in the save block 0x0031FB60-0x00323D94, which the game writes to the memory card
// when it saves. The mod writes nothing into it before the game has set it up (reogh_mem.h, ReoGH_SaveDataReady), and
// it writes only values the game's own code can produce: the bits the game's unlock and purchase code sets, read from
// this disc. What is unlocked stays unlocked after a switch is turned Off, as with a normal unlock.

// [Unlock All Scenarios]  pnach: 20323BD8 FFFFFFFF / used: 0x00323BD8 = 0x0000001F
// The game uses bits 0-4 of the word, one per File #1 scenario; 0xFFFFFFFF would also save 27 bits the game never
// sets, so only the five scenario bits are written. Verified: with 0x0000001F all five scenarios can be selected
// (CHEATS_DESIGN.md, u1).
#define REOGH_SCENARIO_ALL 0x0000001Fu

// [Unlock All NPC's]  pnach: 4031FB6D 00030001 / FFFFFFFF 00000000 (three 32-bit writes from the odd address 0x0031FB6D,
// i.e. the bytes 0x0031FB6D-0x0031FB78) / used: the three NPC words 0x0031FB6C, 0x0031FB70, 0x0031FB74 with the bits
// the game's own unlock code sets. Fixed for this disc: demo.bin 0x0057C23C-0x0057C2A0 sets NPC bit n in the word
// 0x0031FB6C + 4 * (n / 32); its 20 NPC collection items use bits 6-89, 40 bits in all. The code as written is one
// byte off: it misses byte 0x0031FB6C (bit 6, U.S.S.1, a collection NPC) and writes byte 0x0031FB78, outside the NPC
// words. Verified in the game: with these three words the character select lists exactly 40 NPCs (Hursh ...
// Mr. Red, Mr. Blue, Mr. Green, Mr. Gold, Mr. Black, U.S.S.1), and bit 6 alone gives U.S.S.1, which the code as written
// never sets; the code as written lists more than 60 NPCs, many of which no purchase unlocks (U.S.S.2, Billy, ...).
static const u32 kNpcWords[][2] = {
    { 0x0031FB6Cu, 0x72400C40u },
    { 0x0031FB70u, 0xFF03C200u },
    { 0x0031FB74u, 0x03FFC0FBu },
};

// [Unlock Infinity Mode]  pnach: 2031FB7C FFFFFFFF / used: byte 0x0031FB7F |= 0x05
// The four bytes 0x0031FB7C-0x0031FB7F hold flags: 0x7C is set to 1 when the save is created, 0x7D and 0x7E are only
// read, and 0x7F gets bit 0x04 and bit 0x01 when the collection's "INFINITY MODE" and "LONE WOLF MODE" extras are bought
// (demo.bin 0x0057C364-0x0057C384). Only those two bits are written, not 32 bits. Verified: character select then
// offers PARTNER MODE: DEFAULT / LONE WOLF MODE, as with the code as written.
#define REOGH_MODE_FLAGS      0x0031FB7Cu
#define REOGH_MODE_FLAGS_MASK 0x05000000u // byte 0x0031FB7F, bits 0x01 and 0x04

// [Unlock Costume Codes\All Costumes]  pnach: 2031FB64 FFFFFFFF / 2031FB68 FFFFFFFF / used: the bits the game's
// collection code sets for the 11 costume items (demo.bin 0x0057C1D4-0x0057C21C): 0x01010101 and 0x03030301.
// Verified: character select offers "Select A Type".
static const u32 kCostumeWords[][2] = {
    { 0x0031FB64u, 0x01010101u },
    { 0x0031FB68u, 0x03030301u },
};

// [Collection Codes\Have All Collection Available For Purchase]  pnach: 40323BF4 00060001 / FFFFFFFF 00000000 (six
// words from 0x00323BF4 = 0xFFFFFFFF) / used: the same six words with the bits of the 172 collection items (the item
// ids of demo.bin's collection tables; submain.bin 0x0037EAC4-0x0037EAE8 ORs this array in). Verified: every item priced.
static const u32 kCollectionAvailable[][2] = {
    { 0x00323BF4u, 0xFFFFFFFFu }, { 0x00323BF8u, 0xFFFFFFFFu }, { 0x00323BFCu, 0xFFFFFFFFu },
    { 0x00323C00u, 0xFFFFFFFFu }, { 0x00323C04u, 0xFBFE1FFFu }, { 0x00323C08u, 0x00079FFFu },
};

// [Collection Codes\Have All Collection Purchased]  pnach: 40323C54 00060001 / FFFFFFFF 00000000 / used: as a real
// purchase of every item writes them (demo.bin 0x0057C5A0-0x0057C63C): each item's bit in the array 0x00323C54, and for
// the items 0x00-0x0B (array index -1) also in the arrays 0x00323C74 and 0x00323C94. Verified: every item "You already
// have this".
static const u32 kCollectionPurchased[][2] = {
    { 0x00323C54u, 0xFFFFFFFFu }, { 0x00323C58u, 0xFFFFFFFFu }, { 0x00323C5Cu, 0xFFFFFFFFu },
    { 0x00323C60u, 0xFFFFFFFFu }, { 0x00323C64u, 0xFBFE1FFFu }, { 0x00323C68u, 0x00079FFFu },
    { 0x00323C74u, 0x00000FFFu }, { 0x00323C94u, 0x00000FFFu },
};

// [Collection Codes\Max Reward Points]      2031FB60 05F5E0FF: the points held at 99,999,999.
// [Collection Codes\Infinite Reward Points] 1031FB60 0000FFFF: the low halfword of the points held at 0xFFFF (65,535
// from nothing; higher totals keep their upper part).
// Both never go above the game's own limit, 999,999,999 (submain.bin 0x0037CC28-0x0037CC48 caps the word there).
// They write the same word, so only one is applied: Max Reward Points if both are On.
#define REOGH_POINTS       0x0031FB60u
#define REOGH_POINTS_MAX   99999999u
#define REOGH_POINTS_LIMIT 999999999u

// [Collection Codes\Infinite Rewards Points Usage]  E0020833 0057ADF4 / 2057AE08 00000000 / 1057AE16 00001000:
// not active. Its guard matches nothing on this disc (demo.bin holds 0x2821 at 0x0057ADF4, and no overlay has the
// pattern), and its second write would turn "sw a0,0xc(s0)" into a branch. recomp_get_config_u32
// ("infinite_reward_points_usage") is not read.

static void ReoGH_SaveOrAll(const u32 (*words)[2], u32 count) {
    u32 i;

    for (i = 0; i < count; i++) {
        ReoGH_SaveOr(words[i][0], words[i][1]);
    }
}

RECOMP_CALLBACK("*", recomp_on_play_main)
void ReoGH_Unlocks_OnPlayMain(void) {
    u32 scenarios = ReoGH_CheatOn("unlock_all_scenarios");
    u32 npcs = ReoGH_CheatOn("unlock_all_npcs");
    u32 infinity = ReoGH_CheatOn("unlock_infinity_mode");
    u32 costumes = ReoGH_CheatOn("all_costumes");
    u32 available = ReoGH_CheatOn("collection_available");
    u32 purchased = ReoGH_CheatOn("collection_purchased");
    u32 maxPoints = ReoGH_CheatOn("max_reward_points");
    u32 infinitePoints = ReoGH_CheatOn("infinite_reward_points");

    if (!ReoGH_SaveDataReady()) {
        return; // title screen: the save data is not set up yet
    }
    if (scenarios && REOGH_EE_U32(REOGH_SCENARIO_UNLOCK_FLAGS) != REOGH_SCENARIO_ALL) {
        REOGH_EE_U32(REOGH_SCENARIO_UNLOCK_FLAGS) = REOGH_SCENARIO_ALL;
    }
    if (npcs) {
        ReoGH_SaveOrAll(kNpcWords, REOGH_ARRAY_COUNT(kNpcWords));
    }
    if (infinity) {
        ReoGH_SaveOr(REOGH_MODE_FLAGS, REOGH_MODE_FLAGS_MASK);
    }
    if (costumes) {
        ReoGH_SaveOrAll(kCostumeWords, REOGH_ARRAY_COUNT(kCostumeWords));
    }
    if (available) {
        ReoGH_SaveOrAll(kCollectionAvailable, REOGH_ARRAY_COUNT(kCollectionAvailable));
    }
    if (purchased) {
        ReoGH_SaveOrAll(kCollectionPurchased, REOGH_ARRAY_COUNT(kCollectionPurchased));
    }
    if (maxPoints) {
        if (REOGH_EE_U32(REOGH_POINTS) != REOGH_POINTS_MAX) {
            REOGH_EE_U32(REOGH_POINTS) = REOGH_POINTS_MAX;
        }
    } else if (infinitePoints) {
        u32 v = REOGH_EE_U32(REOGH_POINTS);
        u32 want = (v & 0xFFFF0000u) | 0x0000FFFFu;

        if (want > REOGH_POINTS_LIMIT) {
            want = REOGH_POINTS_LIMIT;
        }
        if (v != want) {
            REOGH_EE_U32(REOGH_POINTS) = want;
        }
    }

    // Max Inventory is not active: the Greatest Hits pnach has no inventory or item-count code, so there is nothing
    // to write for recomp_get_config_u32("max_inventory").
}
