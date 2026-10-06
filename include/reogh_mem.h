#ifndef __REOGH_MEM_H__
#define __REOGH_MEM_H__

#include "recomp_api.h"

// Greatest Hits cheat set for SLUS-20765 disc version 2.00, the disc this recompilation runs. Its game code has the
// Greatest Hits memory layout. Every address used with these helpers comes from the owner's
// "Resident Evil - Outbreak [Greatest Hits] (NTSC-U).pnach" (the same cheats as the owner's CodeBreaker v7 list) and
// was checked against that disc; where a code is wrong for this disc, the fix was read from the disc's own code and
// proven in the game (README, "Codes").

// GH-version EE RAM mapping here: an aligned EE word (0x00000000-0x01FFFFFF) is dereferenced
// directly. If mod code sees EE RAM at a different base, this macro is the only thing that changes.
#define REOGH_EE_U32(addr) (*(volatile u32*)(addr))

#define REOGH_ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// EE RAM is little-endian, but N64Recomp translates a mod's byte and halfword loads/stores with
// big-endian (N64) byte order, so they would land on the wrong bytes of the word. Every access
// goes through the aligned 32-bit word, and bytes/halfwords are shifted in and out of it.
// addr must be naturally aligned for its width.
static inline u32 ReoGH_Read(u32 addr, u32 width) {
    u32 word = REOGH_EE_U32(addr & ~3u);
    u32 shift = (addr & 3u) * 8;

    switch (width) {
        case 1: return (word >> shift) & 0xFFu;
        case 2: return (word >> shift) & 0xFFFFu;
        default: return word;
    }
}

static inline void ReoGH_Write(u32 addr, u32 width, u32 value) {
    u32 shift = (addr & 3u) * 8;
    u32 mask;

    switch (width) {
        case 1: mask = 0xFFu << shift; break;
        case 2: mask = 0xFFFFu << shift; break;
        default: REOGH_EE_U32(addr & ~3u) = value; return;
    }
    REOGH_EE_U32(addr & ~3u) = (REOGH_EE_U32(addr & ~3u) & ~mask) | ((value << shift) & mask);
}

// The buttons of the controller in port 1, as the game keeps them: a halfword, one bit per button, 0 = pressed
// (0xFFFF = nothing pressed). The jokers of the pnach test it (D02B081C / E0nnvvvv 002B081C). Proven on this disc in a
// probe (README, "Evidence"): R2+Up reads 0xFDEF, R2+Down 0xFDBF, L1+Select 0xFBFE, L1 0xFBFF, Select 0xFFFE.
#define REOGH_PAD1_BUTTONS 0x002B081Cu
#define REOGH_PAD_NONE     0xFFFFu

static inline u32 ReoGH_Pad1(void) {
    return ReoGH_Read(REOGH_PAD1_BUTTONS, 2);
}

// One pnach write line: 0aaaaaaa = 8-bit, 1aaaaaaa = 16-bit, 2aaaaaaa = 32-bit.
typedef struct {
    u32 addr;
    u32 value;
    u32 width;
    u32 original; // Runtime: what the game had at addr before we wrote value.
} ReoGH_PatchWrite;

#define REOGH_WRITE32(addr, value) { (addr), (value), 4, 0 }
#define REOGH_WRITE16(addr, value) { (addr), (value), 2, 0 }

// A guarded pnach code: Daaaaaaa 0000vvvv (guards the next line) or E0nnvvvv 0aaaaaaa
// (guards the next nn lines). Both are 16-bit equality tests of the halfword at guardAddr.
// The two joker codes have no module guard in the pnach (only the controller test); for them the mod adds one, a
// 16-bit or 32-bit equality test of an instruction of the same function read from this disc, so that nothing is
// ever written while game.bin is not loaded.
//
// These codes rewrite MIPS instructions inside game code that is loaded at runtime; the
// guard is how the pnach checks that the right module is resident. Applied every frame,
// like PCSX2 applies patch=1 lines. Turning a cheat Off puts the original instructions
// back, which a pnach cannot do.
typedef struct {
    u32 guardAddr;
    u32 guardValue;
    u32 guardWidth; // 2 (a pnach D/E guard) or 4
    ReoGH_PatchWrite* writes;
    u32 count;
    u32 active; // Runtime: 1 while our values are in RAM.
} ReoGH_Patch;

#define REOGH_PATCH(guardAddr, guardValue, writes) \
    { (guardAddr), (guardValue), 2, (writes), REOGH_ARRAY_COUNT(writes), 0 }
#define REOGH_PATCH_GUARD32(guardAddr, guardValue, writes) \
    { (guardAddr), (guardValue), 4, (writes), REOGH_ARRAY_COUNT(writes), 0 }

static inline u32 ReoGH_PatchGuardPasses(const ReoGH_Patch* patch) {
    return ReoGH_Read(patch->guardAddr, patch->guardWidth) == patch->guardValue;
}

static inline void ReoGH_PatchApply(ReoGH_Patch* patch) {
    u32 i;

    if (!ReoGH_PatchGuardPasses(patch)) {
        // Module not loaded. Anything we wrote went away with the previous copy.
        patch->active = 0;
        return;
    }

    for (i = 0; i < patch->count; i++) {
        ReoGH_PatchWrite* w = &patch->writes[i];
        u32 cur = ReoGH_Read(w->addr, w->width);

        if (!patch->active || cur != w->value) {
            // First apply, or the module was reloaded: cur is the game's own value.
            w->original = cur;
        }
        if (cur != w->value) {
            // GH-version code-patch write here: writing the opcode takes effect when this module
            // runs from EE RAM. If the module runs as precompiled native code instead, replace this
            // with a RECOMP_HOOK on the function containing w->addr (name from the syms file).
            ReoGH_Write(w->addr, w->width, w->value);
        }
    }
    patch->active = 1;
}

static inline void ReoGH_PatchRestore(ReoGH_Patch* patch) {
    u32 i;

    if (!patch->active) {
        return;
    }
    patch->active = 0;

    if (!ReoGH_PatchGuardPasses(patch)) {
        return;
    }

    for (i = 0; i < patch->count; i++) {
        ReoGH_PatchWrite* w = &patch->writes[i];

        if (ReoGH_Read(w->addr, w->width) == w->value) {
            ReoGH_Write(w->addr, w->width, w->original);
        }
    }
}

static inline void ReoGH_PatchSet(ReoGH_Patch* patch, u32 enabled) {
    if (enabled) {
        ReoGH_PatchApply(patch);
    } else {
        ReoGH_PatchRestore(patch);
    }
}

// Save data: the save block 0x0031FB60-0x00323D94 is written to the memory card when the game saves. Bit 0 of the
// scenario word 0x00323BD8 (Outbreak) is set by the game when your save data is created or loaded; before that the
// block is not set up, and the mod writes nothing into it.
#define REOGH_SCENARIO_UNLOCK_FLAGS 0x00323BD8u

static inline u32 ReoGH_SaveDataReady(void) {
    return (REOGH_EE_U32(REOGH_SCENARIO_UNLOCK_FLAGS) & 1u) != 0;
}

// Sets the bits of mask in a save word, as the game's own unlock code does (it ORs bits in); writes only when a bit
// is missing, and never clears a bit.
static inline void ReoGH_SaveOr(u32 addr, u32 mask) {
    u32 v = REOGH_EE_U32(addr);

    if ((v & mask) != mask) {
        REOGH_EE_U32(addr) = v | mask;
    }
}

#endif
