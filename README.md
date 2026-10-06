# Resident Evil Outbreak — Greatest Hits Cheats

A cheat mod for **Resident Evil Outbreak Recompiled**, the unofficial PC port of Resident Evil Outbreak (File #1, USA).
It puts the **Greatest Hits cheat set** on the game's Mods tab: every cheat is its own switch, you turn cheats on and
off while you play, and a cheat you switch off is completely off again.

- Mod id `reo-greatest-hits-cheats`, version **1.2.1**.
- 29 options: the master code, all 19 cheats of the Greatest Hits CodeBreaker v7 list, the other codes of the Greatest
  Hits pnach (All Costumes, the collection and reward-point codes) and three switches kept from older versions.

> Unofficial fan project, not affiliated with or endorsed by Capcom. See [Legal](#legal).

## Which disc it is for

**Resident Evil Outbreak (USA), SLUS-20765, disc version 2.00** - the release Resident Evil Outbreak Recompiled runs.
In the words of the mod's manifest: "SLUS-20765 v2.00, which has the Greatest Hits memory layout". These codes were
made for that layout, so most of them work on this disc as written; where a code as written is wrong for it, the mod
uses a corrected form (marked "fixed for this disc" below).

There are two cheat mods for this disc. **Enable only one of them**: both change the same game code.

| Mod | Cheats |
|---|---|
| **Resident Evil Outbreak — Greatest Hits Cheats** (this one, `reo-greatest-hits-cheats`) | the Greatest Hits cheat set |
| Resident Evil Outbreak — Cheats (`reo-cheats`, repository REO-Cheats) | the regular (original release) cheat set, adapted to this disc |

The two mods declare the conflict: if both are switched on, the Mods tab keeps only the one later in the load order
active and says so.

## Requirements

- **Resident Evil Outbreak Recompiled 1.5.0 or newer.**
- **Your own legal copy** of Resident Evil Outbreak (USA), SLUS-20765, disc version 2.00. This mod contains no disc
  image and no game files.

> **Status.** Resident Evil Outbreak Recompiled 1.5.0 prepares the game's code from your own disc on your PC. In the
> prepared game the cheats marked **game data** or **save data** take effect. The cheats marked **game code** do not:
> they need their code sites compiled into the prepared game, and this download does not carry those sites. The Mods
> tab reports these cheats as changes of the game's code that are not in this game build. Your switches are saved
> either way.

The cheats marked **game code** in the table below never write the game's code directly: the program switches the
matching code sites compiled into the prepared game code, so they act only in a game build that has this mod's sites.
For every switch that is on, the Mods tab says whether it acts in your game (active, waiting for the game, not in this
game build). Cheats marked **game data** or **save data** need nothing more than the mod.

## Install

1. Download `reo-greatest-hits-cheats-1.2.1.zip` from this repository's **Releases** page (it is also in the
   [`release`](release) folder). Do not unpack it.
2. In Resident Evil Outbreak Recompiled, open **Mods**, choose **Install Mods** and pick the zip.
3. Make sure the mod is switched on in the list, then press **Configure** and switch on the cheats you want. Every
   cheat starts Off; the Master Code starts On.

The Mods tab checks the whole archive before it writes anything and installs the mod into its own folder,
`mods\reo-greatest-hits-cheats\`. To install by hand instead, unpack the zip into your mods folder so that
`mods\reo-greatest-hits-cheats\mod.json` exists (the mods folder is `%APPDATA%\Resident Evil Outbreak Recompiled\mods\`,
or `mods\` next to the program in a portable copy).

**Updating:** install the new zip the same way. The Mods tab asks before it replaces the installed version, moves the
old one into `mods\.trash\` (nothing is deleted) and keeps your switches, settings and place in the load order.

## Off means completely off

- Switching a cheat off, switching the Master Code off, or switching the whole mod off takes the cheat out of the game
  at the next game frame: the game's own instructions run again, and data the cheat changed gets the game's own value
  back.
- **Save data too.** What a cheat writes into your save is recorded next to your memory card, in its undo file
  (`MemoryCard_01.ps2.undo.json`, never inside the card), and put back when the cheat is off - also after a restart.
  Before the first change of save data in a session the program copies your memory card once
  (`MemoryCard_01.ps2.before-cheats-<date>`).
- What the game itself changed in the meantime stays: what you earned, or bought with a cheat's points, is not taken
  away.
- Values the game's own code stored while a code cheat was on are the game's own (for example the 9999 HP of Infinite
  Health stays until the next hit).

Some option descriptions inside the mod still say that unlocked things "stay unlocked after turning this Off". They
were written before the program's undo journal existed; the program undoes save-data cheats as described here.

## The cheats

**Count: 19 of 19.** Every cheat of the Greatest Hits CodeBreaker v7 list is an option. Of them, 17 do what their name
says on this disc (4 of them fixed for it), P1 Press L1+Select For Infection 100% does what it says and kills every
character, and Infinite Time acts only in online play, which the program does not have yet. Each option's description
on the Mods tab shows the cheat's author and its CodeBreaker v7 code verbatim next to the plain code.

<!-- cheat-table:begin -->
| Option (key) | Author | On SLUS-20765 v2.00 | Changes |
|---|---|---|---|
| **Master Code (CodeBreaker v7+): Enable Code (Must Be On)** (`master_code`) | Code Master | Must be On for any cheat of this mod to apply (default On). Off switches every cheat of the mod off. See [Master code](#master-code). | - |
| **1-Hit Deaths (Enemies)** (`one_hit_kills`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: any hit kills an enemy. | game code |
| **Become A Zombie After Death (Offline Mode)** (`zombie_after_death`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: when you die you come back as a playable zombie, as in online play, instead of YOU DIED. | game code |
| **Character 2 (CPU) Modifier** (`character_2`) | 47iscool | Works: choose your first CPU partner (Kevin, Mark, Jim, George, David, Alyssa, Yoko, Cindy, or no partner). Off leaves the game's choice. Applies from the next scenario on, in every scenario of a session; a partner is never changed in the middle of a scenario. | game data |
| **Character 3 (CPU) Modifier** (`character_3`) | 47iscool | Works: the same for your second CPU partner. | game data |
| **Don't Become A Zombie Over Time** (`no_zombie_over_time`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: the virus gauge no longer rises with time (you and the partners). | game code |
| **Don't Become A Zombie When Bitten** (`no_zombie_when_bitten`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: bites no longer raise the virus gauge (you and the partners). | game code |
| **No Infection / Freeze Infection** (`no_infection`) | (a switch of this mod) | Turns on both Don't Become A Zombie cheats at once. | game code |
| **Infection Freeze Value (%)** (`infection_freeze_value`) | (a setting of this mod) | Not active: the Greatest Hits set has no virus-gauge code, so this value changes nothing. | - |
| **Enemies Don't Damage You With Attacks** (`no_enemy_damage`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: enemy attacks take no HP from you or the partners. Bites and grabs still raise the virus gauge. | game code |
| **Extra Ammo** (`extra_ammo`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: every shot adds a round instead of using one. Infinite Ammo, Extra Ammo and Max Infinite Ammo change the same instructions, so only one works at a time: Max Infinite Ammo if it is On, else Extra Ammo, else Infinite Ammo. | game code |
| **Go It Alone** (`go_it_alone`) | 47iscool | Works as written: no CPU partners, from the next scenario on ("basically the same thing as Lone Wolf Mode", 47iscool). Overrides the Character 2 and 3 choices. | game data |
| **Infinite Ammo** (`infinite_ammo`) | Code Master | Works as written: firing does not use up rounds. | game code |
| **Infinite Health** (`infinite_health`) | Code Master | Works, fixed for this disc (the code's guard address, and HP stored as 16 bits so the maximum HP is not cleared): HP is set to 9999 whenever the game checks for death (you and the CPU partners). | game code |
| **Infinite Item Usage** (`infinite_items`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works, fixed for this disc (the code's guard value is one too high): using a consumable item does not use it up (you and the partners). | game code |
| **Infinite Time** (`infinite_time`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | **Online play only**, which is not available yet: the code fits this disc, but its instruction is in the game's online event-script code, so offline it changes nothing. | game code |
| **Max Infinite Ammo** (`max_infinite_ammo`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works, fixed for this disc (as written the count drops to 1 and stays there): the first shot sets the loaded rounds to 99 and they stay 99. | game code |
| **P1 Press L1+Select For Infection 100%** (`infection_100_joker`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written, and **it kills**: while L1+Select is held, the next virus-gauge update sets the infection of every character to 100 %, which is death. L1+Select also opens the pause menu, and the gauge does not change while paused: press Select to pause, then hold L1+Select. Does nothing while a Don't Become A Zombie Over Time cheat is on. | game code |
| **Unlock All NPC's** (`unlock_all_npcs`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works, fixed for this disc (as written it is one byte off): the 40 NPCs of the collection become selectable at the character select. | save data |
| **Unlock All Scenarios** (`unlock_all_scenarios`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works: all five scenarios can be selected (only the five scenario bits the game uses are set). | save data |
| **Unlock Infinity Mode** (`unlock_infinity_mode`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works: sets the two bits buying INFINITY MODE and LONE WOLF MODE sets; the character select offers PARTNER MODE: LONE WOLF MODE. | save data |
| **Walk Through Walls & On Air R2 Up On/R2 Down Off** (`walk_through_walls`) | 47iscool | Works as written (a joker): with this switch On, R2+Up lets you walk through walls and objects (your height stays where it is), R2+Down turns it off again. "Works most of the time" (47iscool). | game code |
| **All Costumes** (`all_costumes`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works: the costume bits of the collection's 11 costume items; the character select offers "Select A Type". | save data |
| **Have All Collection Available For Purchase** (`collection_available`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works: every one of the 172 collection items can be bought. | save data |
| **Have All Collection Purchased** (`collection_purchased`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works: every collection item shows as owned, exactly as buying all of them would. | save data |
| **Infinite Reward Points** (`infinite_reward_points`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: the low 16 bits of your points are held at FFFF, so buying never runs them out (never above the game's limit of 999,999,999). If Max Reward Points is also On, it is used instead. | save data |
| **Max Reward Points** (`max_reward_points`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | Works as written: your points are held at 99,999,999. | save data |
| **Infinite Rewards Points Usage** (`infinite_reward_points_usage`) | Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 | **Not active**: this code does not fit this disc, so the switch does nothing. | - |
| **Max Inventory** (`max_inventory`) | (a switch of this mod) | **Not active**: there is no such code in the Greatest Hits set; the switch does nothing. | - |
<!-- cheat-table:end -->

Save-data cheats only ever set values the game itself can write (for example the five scenario bits, not all 32), and
they write only once your save has been loaded or created.

## Master code

    Enable Code (Must Be On) by Code Master
    Codebreaker v7+
    B4336FA9 4DFEFB79
    62838F11 EF8D0773
    3945B15D EB4129EF
    535F17F1 63E46A24

On a CodeBreaker, the master code hooks the device's code engine into the game. The recompiled game has no cheat
device and its code runs as native, precompiled code, so the hook is never written. The code is this mod's master
switch instead: the `master_code` option (On by default) must be On for any cheat of this mod to apply, and Off
switches every cheat off and puts the game's original instructions back.

## Building from source

The release zip holds the built mod; building it yourself is only needed to change it. You need clang with the MIPS
target (LLVM 18.1.8 is the last official Windows release that has it), `ld.lld`, and `RecompModTool` from
[N64Recomp](https://github.com/N64Recomp/N64Recomp). From Git Bash, in this folder:

```bash
python tools/gen_manifests.py   # after changing a cheat or setting: regenerates mod.toml, mod.json, manifest.json
                                # and the table between the cheat-table markers of this README
mingw32-make OS=Linux CC=<path to clang.exe> LD=<path to ld.lld.exe> RECOMP_MOD_TOOL=<path to RecompModTool.exe>
```

The output is `build/REO_GreatestHits_Cheats.nrm`. To try a build, use **Mods → Install Mods** and pick this folder's
`mod.json` (the folder is copied into the mods folder), or pack `mod.json`, `icon.png` and the `.nrm` into a zip as in
the release. `syms/reo.syms.toml` is empty on purpose: the mod calls no game functions.

## Credits

- **Cheat codes:** Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 (GameHacking.org); Character 2/3 (CPU)
  Modifier, Go It Alone and Walk Through Walls by **47iscool**.
- **Master code:** Code Master (CodeBreaker v7+).
- The mod's layout is modeled on ProxyMM's
  [Cheats](https://github.com/garrettjoecox/ProxyMM_RecompMods/tree/main/packages/Cheats) for Zelda 64: Recompiled. No
  code or cheats are taken from it.
- **Capcom** - Resident Evil Outbreak, the original game.
- This mod was built by RaccoonRecomp with AI-assisted programming (Claude, Anthropic).

## Legal

- This is an **unofficial fan project**. It is **not affiliated with, endorsed or sponsored by Capcom**.
- *Resident Evil*, *Resident Evil Outbreak* and all related names, characters and content are trademarks and property
  of Capcom. **All copyright and credit for the game belong to Capcom.** The names are used only to say which game this
  mod works with.
- **You need your own legal copy of the game.** This mod contains **no disc image and no game files**. Like the cheat
  codes it is made from, it holds the game addresses it changes and the few original instruction words it puts back
  when a cheat is switched off.
