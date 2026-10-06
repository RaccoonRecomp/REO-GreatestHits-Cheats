"""Writes mod.toml, mod.json (the game's mod manifest), manifest.json (Thunderstore) and the cheat table of README.md
(between its "cheat-table" markers) from the list below, so all four show the same cheats with the same keys, defaults
and codes:

    python tools/gen_manifests.py

Only this file defines the settings. The keys the owner's saved settings use (mods/mod_config.json) are kept, so a
saved choice keeps working after an update; every new switch defaults to Off (master_code to On).

The CodeBreaker v7 lines are the owner's list, verbatim; the plain lines are the owner's
"Resident Evil - Outbreak [Greatest Hits] (NTSC-U).pnach", verbatim (address and value of each patch line)."""
import json
import os
import re

OUT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

VERSION = "1.2.1"
NAME = "Resident Evil Outbreak — Greatest Hits Cheats"
SHORT = "All 19 of the Greatest Hits CodeBreaker v7 cheats (+ the pnach's collection codes) for Resident Evil Outbreak " \
        "NTSC-U v2.00"

MASTER_LINES = ["B4336FA9 4DFEFB79", "62838F11 EF8D0773", "3945B15D EB4129EF", "535F17F1 63E46A24"]

CM5 = "Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83"
CHARACTERS = ["Kevin", "Mark", "Jim", "George", "David", "Alyssa", "Yoko", "Cindy"]
PARTNER_CHOICES = ["Off"] + CHARACTERS + ["No partner (FF)"]
PARTNER_IDS = ", ".join(f"{n} {i:02X}" for i, n in enumerate(CHARACTERS))
PARTNER_TIMING = ("Applies from the next scenario on, in every scenario of a session: the game clears the players' "
                  "data when a scenario ends and again when it sets up the next one, then creates the partners from "
                  "this byte when that scenario starts. A partner is never changed in the middle of a scenario: a "
                  "choice made during a scenario applies at the next one.")
GO_ALONE_TIMING = ("Applies from the next scenario on, in every scenario of a session: the game clears the players' "
                   "data when a scenario ends and again when it sets up the next one, and when that scenario starts it "
                   "creates no partner from FF. Partners already in a scenario are never removed in the middle of it: "
                   "switched on during a scenario, it applies at the next one.")
PARTNER_SESSION_EVIDENCE = (
    "s2_partners_46: switched on only after the first scenario (YOU DIED, quit to the title, a second scenario in the "
    "same session), the second scenario had David (2200 HP, DAVID on the item screen) and Yoko (1300 HP). "
    "s2_off_after_46: switched Off after a first scenario with David and Yoko, the second had the game's Mark and "
    "Cindy. s2_clear_trace, s2_setup_trace: game.bin clears the players' data at the quit and again at the next "
    "setup, right before it writes the chosen characters.")
AMMO_RULE = ("Infinite Ammo, Extra Ammo and Max Infinite Ammo change the same instructions, so only one of them works "
             "at a time: if more than one is On, Max Infinite Ammo is used, else Extra Ammo, else Infinite Ammo.")
POINTS_RULE = "Max Reward Points and Infinite Reward Points write the same word: if both are On, Max Reward Points is used."
SAVE_NOTE = ("Writes your save data (kept when the game saves), only once your save data is loaded or created. "
             "Unlocked things stay unlocked after turning this Off, as with a normal unlock.")


def C(key, label, author, cb7, plain, status, evidence, default=False, kind="bool", choices=None, cb7_list=True,
      pnach_note=""):
    """One cheat. cb7: the owner's CodeBreaker v7 lines (None: not in the owner's CodeBreaker list); plain: the address
    and value of each line of the owner's Greatest Hits pnach section."""
    return dict(key=key, label=label, author=author, cb7=cb7, plain=plain, status=status, evidence=evidence,
                default=default, kind=kind, choices=choices, cb7_list=cb7_list, pnach_note=pnach_note)


# The owner's 19 CodeBreaker v7 cheats, in the order of the owner's list, then the other sections of the Greatest Hits
# pnach. Status texts: "Works as written" = the pnach's own addresses and values; "Fixed for your disc" = a fix read
# from the disc's own code and proven in the game (README).
CHEATS = [
    C("one_hit_kills", "1-Hit Deaths (Enemies)", CM5,
      ["140BD47B E5A3FA24", "8BC46491 2A3E2920"],
      ["D06407F4 0000882D", "20640800 A6620138"],
      "Works as written: any hit kills an enemy (the damage of the hit is set to 0x7FFF).",
      "one_hit: the three enemies hit die at the first hit (normally -10 HP per hit)."),
    C("zombie_after_death", "Become A Zombie After Death (Offline Mode)", CM5,
      ["1B2D099A BA9248A5", "3A4AC368 C6DECBB8"],
      ["D065FD14 0000202D", "1065FD3A 00001000"],
      "Works as written: when you die you come back as a playable zombie, as in online play, instead of YOU DIED.",
      "zombie_after_death / death_sheet: at 100 % the game goes on with the zombie display (normally YOU DIED)."),
    C("character_2", "Character 2 (CPU) Modifier", "47iscool",
      ["004A78A8 000000??"],
      ["004A78A8 000000FF"],
      "Works: choose the character of your first CPU partner. ?? is the game's character number, read from your "
      f"disc: {PARTNER_IDS}, or FF for no partner (as Go It Alone). Off leaves the game's choice. " + PARTNER_TIMING,
      "partners_46, partners_23, partners_54, partners_0ff: each partner came in with the chosen character's HP from "
      "the game's own table and its name on the item screen (David, Yoko, Jim, George, Alyssa, a second Kevin); FF gave "
      "no partner. " + PARTNER_SESSION_EVIDENCE,
      default="Off", kind="choice", choices=PARTNER_CHOICES,
      pnach_note="the pnach writes FF; your CodeBreaker list leaves the value ?? to choose"),
    C("character_3", "Character 3 (CPU) Modifier", "47iscool",
      ["004A8988 000000??"],
      ["004A8988 000000FF"],
      "Works: choose the character of your second CPU partner. ?? is the game's character number, read from your "
      f"disc: {PARTNER_IDS}, or FF for no partner (as Go It Alone). Off leaves the game's choice. " + PARTNER_TIMING,
      "as Character 2 (the same probes set both partners).",
      default="Off", kind="choice", choices=PARTNER_CHOICES,
      pnach_note="the pnach writes FF; your CodeBreaker list leaves the value ?? to choose"),
    C("no_zombie_over_time", "Don't Become A Zombie Over Time", CM5,
      ["84992637 2E37F659", "A78B1F13 7E057998"],
      ["D0667E54 00009FA4", "20667E5C 00000000"],
      "Works as written: the virus gauge no longer rises with time (you and the partners). Also on while "
      "No Infection / Freeze Infection is On.",
      "no_zombie: all three gauges stay 0 through the zombie phase (normally 11330)."),
    C("no_zombie_when_bitten", "Don't Become A Zombie When Bitten", CM5,
      ["D8E42593 718C544E", "DE34597D 8B407F17"],
      ["D066015C 00009FE4", "20660160 00000000"],
      "Works as written: bites no longer raise the virus gauge (you and the partners). Also on while "
      "No Infection / Freeze Infection is On.",
      "no_zombie: the gauges stay 0 through the bites."),
    C("no_enemy_damage", "Enemies Don't Damage You With Attacks", CM5,
      ["6D1B5F75 4AD28BCF", "AF1D3E83 14A4650D"],
      ["D0660B0C 0000802D", "20660B18 00000000"],
      "Works as written: enemy attacks take no HP from you or the partners. Bites and grabs still raise the virus "
      "gauge.",
      "no_damage: every HP unchanged through the zombie phase."),
    C("extra_ammo", "Extra Ammo", CM5,
      ["28E323AA DFEA4D9A", "F1A7F593 3916B184"],
      ["D05B1FF8 00001E3C", "105B2008 00000001"],
      "Works as written: every shot adds a round instead of using one. " + AMMO_RULE,
      "ammo_extra: 7 rounds become 19 over 12 shots."),
    C("go_it_alone", "Go It Alone", "47iscool",
      ["004A78A8 000000FF", "004A8988 000000FF"],
      ["004A78A8 000000FF", "004A8988 000000FF"],
      "Works as written: no CPU partners (both partners get FF), \"basically the same thing as Lone Wolf Mode\" "
      "(47iscool). Overrides the Character 2 and 3 choices. " + GO_ALONE_TIMING,
      "go_alone, partners_0ff: the partners are never created; Kevin alone. s2_alone: switched on only after the "
      "first scenario of the session, the second scenario had no partners (Kevin alone on the item screen)."),
    C("infinite_ammo", "Infinite Ammo", "Code Master",
      ["28E323AA DFEA4D9A", "C05C0212 C913FE27"],
      ["D05B1FF8 00001E3C", "205B200C 00000000"],
      "Works as written: firing does not use up rounds. " + AMMO_RULE,
      "ammo_inf: 7 stays 7 over 12 shots (normally 7 -> 0); ammo_inf_off: Off resumes normal use."),
    C("infinite_health", "Infinite Health", "Code Master",
      ["F8BD46AB 776D98DB", "199D7A0E D1E356B5", "EB9C4A66 F817FA36"],
      ["E0023881 00660794", "20660800 2403270F", "20660808 AE030544"],
      "Works, fixed for your disc: guard E0024200 006607F4 (the pnach kept the regular release's guard address, "
      "which on your disc is another function) and the HP stored as 16 bits (A6030544 instead of AE030544, which "
      "also zeroed max HP). HP is set to 9999 whenever the game checks for death (you and the CPU partners).",
      "health_fix: all three HP held at 9999; health_sh_off: Off returns to normal damage, no death."),
    C("infinite_items", "Infinite Item Usage", CM5,
      ["AAE56582 2CAA4391", "09E05936 A4941FA4", "CEB1C3CE A106F69E", "2C57061E 2E187B84"],
      ["E0031E3D 005B2178", "205B218C 00000000", "205B21A0 00000000", "205B51B4 00000000"],
      "Works, fixed for your disc: guard E0031E3C 005B2178 (the pnach's 1E3D is one too high: your disc holds "
      "00121E3C there). Using a consumable item does not use it up (you and the partners).",
      "items_fix: Cindy's herb stays 1 after she heals (normally 1 -> 0)."),
    C("infinite_time", "Infinite Time", CM5,
      ["F88D3C42 ED0F0476", "2996B082 931BAA43"],
      ["D068C77C 00001821", "2068C784 00000000"],
      "Only acts in online play; online play is not available yet. The code fits your disc and is applied as "
      "written, but its instruction lies in the game's online event-script interpreter, which offline play never "
      "runs, so offline it changes nothing. Nothing else is done in its place.",
      "t1: in a whole single-play session the instruction at 0x0068C784 never runs; the offline interpreter is "
      "0x007006D0."),
    C("max_infinite_ammo", "Max Infinite Ammo", CM5,
      ["28E323AA DFEA4D9A", "88BD3706 23FC7AA7"],
      ["D05B1FF8 00001E3C", "205B2008 24030063"],
      "Fixed for your disc: 205B2008 24020063 (li v0,99). As written it puts 99 into v1, but the game stores v0, "
      "so the count drops to 1 and stays there. Fixed, the first shot sets the count to 99 and it stays 99. "
      + AMMO_RULE,
      "ammo_max: as written 7 -> 1; ammo_max_fix: 7 -> 99 at the first shot, 99 through all 12 shots."),
    C("infection_100_joker", "P1 Press L1+Select For Infection 100%", CM5,
      ["FD6430A9 14416489", "75FC5FC6 A0A9BE55", "252F9E14 0A358416", "EF5E3E39 CF1DA925"],
      ["D02B081C 0000FBFE", "20667E28 24900000", "D02B081C 0000FFFF", "20667E28 4410B800"],
      "Works as written, and it KILLS: while L1+Select is held, the next virus-gauge update (about once a second) "
      "sets the infection of EVERY character (you and the partners) to 100 %, and 100 % is death. L1+Select also "
      "opens and closes the pause menu, and the gauge is not updated while the game is paused: press Select to "
      "pause, then hold L1+Select. Releasing every button puts the game's code back. It does nothing while Don't "
      "Become A Zombie Over Time or No Infection / Freeze Infection is On.",
      "pad: L1+Select reads FBFE; infect_joker2: all three gauges at 100 % within a quarter second, YOU DIED, "
      "survivors 0/3; infect_joker, infect_joker_hold: pressed from play, the pause menu opens and nothing happens; "
      "infect_joker2_overtime: with Don't Become A Zombie Over Time on, nothing happens."),
    C("unlock_all_npcs", "Unlock All NPC's", CM5,
      ["44124010 EEA76388", "0CF49BDF A4E29C32"],
      ["4031FB6D 00030001", "FFFFFFFF 00000000"],
      "Fixed for your disc: the 40 NPCs of your disc's collection become selectable (character select). As written "
      "the code fills the bytes 0x0031FB6D-0x0031FB78, one byte off the game's NPC words 0x0031FB6C-0x0031FB77: it "
      "misses U.S.S.1, writes a byte outside the NPC words, and turns on dozens of NPC entries no purchase unlocks. "
      "The mod sets exactly the bits the game's own unlock code sets for its 20 NPC items. " + SAVE_NOTE,
      "npc_valid: the character select lists exactly 40 NPCs (Hursh ... Mr. Red, Mr. Blue, Mr. Green, Mr. Gold, "
      "Mr. Black, U.S.S.1); npc_bit6: U.S.S.1; n_npcl_gh: as written, more than 60 entries (Billy, U.S.S.2, ...), "
      "no U.S.S.1."),
    C("unlock_all_scenarios", "Unlock All Scenarios", CM5,
      ["3C36A473 FB276FFB"],
      ["20323BD8 FFFFFFFF"],
      "Works, only valid save data: the five scenario bits (0x0000001F) instead of 0xFFFFFFFF (the game uses bits "
      "0-4 only). All five scenarios can be selected. " + SAVE_NOTE,
      "u1, v_u1: all five scenarios selectable with 0x1F."),
    C("unlock_infinity_mode", "Unlock Infinity Mode", CM5,
      ["93BAE998 45A83A76"],
      ["2031FB7C FFFFFFFF"],
      "Works, only valid save data: sets the two bits the game sets when you buy the collection's INFINITY MODE and "
      "LONE WOLF MODE (byte 0x0031FB7F, 0x04 and 0x01) instead of all 32 bits of 0x0031FB7C. Character select then "
      "offers PARTNER MODE: LONE WOLF MODE, as with the code as written. " + SAVE_NOTE,
      "k4, v_k4: PARTNER MODE: DEFAULT / LONE WOLF MODE at character select."),
    C("walk_through_walls", "Walk Through Walls & On Air R2 Up On/R2 Down Off", "47iscool",
      ["E002FDEF 002B081C", "206636D0 03E00008", "206636D4 00000000", "E002FDBF 002B081C", "206636D0 27BDFFC0",
       "206636D4 FFBF0020"],
      ["E002FDEF 002B081C", "206636D0 03E00008", "206636D4 00000000", "E002FDBF 002B081C", "206636D0 27BDFFC0",
       "206636D4 FFBF0020"],
      "Works as written (a joker): with this switch On, press R2+Up to walk through walls and objects (your height "
      "stays where it is, \"on air\"), R2+Down to turn it off again. \"Works most of the time\" (47iscool). Turning "
      "the switch Off also turns it off.",
      "pad: R2+Up reads FDEF, R2+Down FDBF; walls_joker vs walls_joker_base: on, Kevin walks 5.4 units past where the "
      "game stops him, height frozen; after R2+Down identical to the run without the cheat; walls_47: the same code "
      "held on."),
    # The other sections of the Greatest Hits pnach (not in the owner's CodeBreaker list).
    C("all_costumes", "All Costumes", CM5, None,
      ["2031FB64 FFFFFFFF", "2031FB68 FFFFFFFF"],
      "Works, only valid save data: the costume bits the game's collection sets for its 11 costume items "
      "(0x01010101 and 0x03030301) instead of all 64 bits. Character select offers \"Select A Type\". Kept from your "
      "original Greatest Hits file (your updated copy does not have it). " + SAVE_NOTE,
      "k2, v_k2: \"Select A Type\" at character select.", cb7_list=False,
      pnach_note="section Unlock Costume Codes\\All Costumes"),
    C("collection_available", "Have All Collection Available For Purchase", CM5, None,
      ["40323BF4 00060001", "FFFFFFFF 00000000"],
      "Works, only valid save data: the bits of the 172 collection items instead of 192 bits. Every item can be "
      "bought. " + SAVE_NOTE,
      "c2, v_c2: every item priced.", cb7_list=False, pnach_note="section Collection Codes\\..."),
    C("collection_purchased", "Have All Collection Purchased", CM5, None,
      ["40323C54 00060001", "FFFFFFFF 00000000"],
      "Works, only valid save data: exactly the bits a purchase of every one of the 172 items writes (including the "
      "two other arrays the game's purchase code sets for the first 12 items). Every item shows as owned. " + SAVE_NOTE,
      "c1, v_c1: every item \"You already have this\".", cb7_list=False, pnach_note="section Collection Codes\\..."),
    C("infinite_reward_points", "Infinite Reward Points", CM5, None,
      ["1031FB60 0000FFFF"],
      "Works as written: the low 16 bits of your points are held at FFFF (65,535 points from a lower total), so "
      "buying never runs them out; never above the game's own limit, 999,999,999. " + POINTS_RULE + " " + SAVE_NOTE,
      "c1, v_c1: RESULT 65535 Pts.", cb7_list=False, pnach_note="section Collection Codes\\..."),
    C("max_reward_points", "Max Reward Points", CM5, None,
      ["2031FB60 05F5E0FF"],
      "Works as written: your points are held at 99,999,999 (the game's own limit is 999,999,999). " + POINTS_RULE +
      " " + SAVE_NOTE,
      "m1, v_m1: RESULT 99999999 Pts.", cb7_list=False, pnach_note="section Collection Codes\\..."),
    C("infinite_reward_points_usage", "Infinite Rewards Points Usage", CM5, None,
      ["E0020833 0057ADF4", "2057AE08 00000000", "1057AE16 00001000"],
      "Not active: this code does not fit your disc. Its guard matches no code on it (the collection code, demo.bin, "
      "holds 0x2821 at 0x0057ADF4, and no other overlay has the pattern), and its second write would turn a store "
      "into a branch. This switch does nothing.",
      "check_codes: the guard against every overlay of your disc.", cb7_list=False,
      pnach_note="section Collection Codes\\..."),
]

OWN = {
    "no_infection": dict(
        label="No Infection / Freeze Infection", default=False,
        desc="Your own switch (not a code of its own): turns on both Don't Become A Zombie Over Time and Don't Become "
             "A Zombie When Bitten, which also have their own switches: the virus gauge no longer rises, for you and "
             "the partners."),
    "infection_freeze_value": dict(
        label="Infection Freeze Value (%)", default=0, min=0, max=100,
        desc="Not active: the Greatest Hits cheat set has no virus gauge code, so No Infection / Freeze Infection "
             "freezes the gauge at its current value whatever this is set to. (100 % would be death.)"),
    "max_inventory": dict(
        label="Max Inventory", default=False,
        desc="Not active: the Greatest Hits cheat set has no inventory code, so this switch does nothing."),
}

MASTER_DESC = ("Must be On for every cheat of this mod, like the master code on a CodeBreaker. Off switches all of them "
               "off and puts the game's original code back.\n\n"
               "Enable Code (Must Be On) by Code Master\nCodebreaker v7+\n" + "\n".join(MASTER_LINES) + "\n\n"
               "There is no cheat device in the recompiled game, so the code's hook is never written into the game: "
               "this switch does its job instead (see README).")


def cheat_desc(c):
    lines = [f"{c['label']} by {c['author']}"]
    if c["cb7"]:
        lines += ["Codebreaker v7+"] + c["cb7"]
    else:
        lines += ["Codebreaker v7+: not in your CodeBreaker list; from your Greatest Hits pnach"]
    note = f" ({c['pnach_note']})" if c["pnach_note"] else ""
    lines += ["", f"Your Greatest Hits pnach{note}: " + " / ".join(c["plain"]), "", "On your disc: " + c["status"]]
    return "\n".join(lines)


def B(key, label, default, desc):
    return dict(key=key, label=label, kind="bool", default=default, desc=desc)


S = [B("master_code", "Master Code (CodeBreaker v7+): Enable Code (Must Be On) by Code Master", True, MASTER_DESC)]
for c in CHEATS:
    S.append(dict(key=c["key"], label=c["label"], kind=c["kind"], default=c["default"], choices=c["choices"],
                  desc=cheat_desc(c)))
    if c["key"] == "no_zombie_when_bitten":
        for k in ("no_infection", "infection_freeze_value"):
            o = OWN[k]
            if "min" in o:
                S.append(dict(key=k, label=o["label"], kind="int", default=o["default"], min=o["min"], max=o["max"],
                              desc=o["desc"]))
            else:
                S.append(B(k, o["label"], o["default"], o["desc"]))
S.append(B("max_inventory", OWN["max_inventory"]["label"], False, OWN["max_inventory"]["desc"]))

N_CB7 = sum(1 for c in CHEATS if c["cb7_list"])

DESCRIPTION = f"""All of the Greatest Hits cheats for Resident Evil Outbreak (NTSC-U), SLUS-20765 disc version 2.00: the disc this recompilation runs. Its game code has the Greatest Hits memory layout, which is what these codes were made for.
Separate from "Resident Evil Outbreak — Cheats" (the regular cheat set): enable only one of the two.

Master Code: Enable Code (Must Be On) by Code Master (CodeBreaker v7+). Must be On for the cheats below.

The {N_CB7} cheats of your CodeBreaker v7 list, each its own option with its code:
""" + "\n".join(f"- {c['label']}" for c in CHEATS if c["cb7_list"]) + """

The other codes of your Greatest Hits pnach:
""" + "\n".join(f"- {c['label']}" for c in CHEATS if not c["cb7_list"]) + """

Fixed for your disc: Infinite Health, Infinite Item Usage, Max Infinite Ammo, Unlock All NPC's. Save data gets only values the game itself can write. Infinite Time acts only in online play (not available yet); Infinite Rewards Points Usage and Max Inventory are not active."""

HOST_DESCRIPTION = (
    f"All of the Greatest Hits cheats for your disc (SLUS-20765 v2.00, which has the Greatest Hits memory layout): the "
    f"{N_CB7} cheats of your CodeBreaker v7 list, each its own option with its CodeBreaker code, plus the other codes of "
    "your Greatest Hits pnach (All Costumes, the collection and reward-point codes). Fixed for your disc where the "
    "code as written is wrong for it (Infinite Health, Infinite Item Usage, Max Infinite Ammo, Unlock All NPC's); save "
    "data gets only values the game itself can write. Infinite Time acts only in online play, which is not available "
    "yet; Infinite Rewards Points Usage and Max Inventory are not active.\n\n"
    "Master Code: Enable Code (Must Be On) by Code Master (CodeBreaker v7+). It must be On for any of these cheats to "
    "apply; Off switches every cheat of this mod off and puts the game's original code back.\n\n"
    "The switches are saved, but the cheats only take effect once the game runs mod code. Until then this mod changes "
    "nothing. Only one of the two cheat mods can be on."
)


def toml_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n") + '"'


lines = [
    "[manifest]",
    'id = "REO_GreatestHits_Cheats"',
    f'version = "{VERSION}"',
    "display_name = " + toml_str(NAME),
    'description = """\n' + DESCRIPTION + '\n"""',
    "short_description = " + toml_str(SHORT),
    'authors = [ "RaccoonRecomp" ]',
    "# The game id of the disc this recompilation runs (SLUS-20765 v2.00, the Greatest Hits memory layout); the same",
    "# id REO_Cheats and REO_ModernCam use for it.",
    'game_id = "reo"',
    'minimum_recomp_version = "1.0.0"',
    "dependencies = []",
    "native_libraries = []",
    "",
    "[inputs]",
    'elf_path = "build/mod.elf"',
    'mod_filename = "REO_GreatestHits_Cheats"',
    "# Function reference symbols for SLUS-20765 v2.00 (this mod references no game functions).",
    'func_reference_syms_file = "syms/reo.syms.toml"',
    "data_reference_syms_files = []",
    'additional_files = [ "thumb.png" ]',
]
for st in S:
    lines += ["", "[[manifest.config_options]]", "id = " + toml_str(st["key"]), "name = " + toml_str(st["label"]),
              "description = " + toml_str(st["desc"])]
    if st["kind"] == "bool":
        lines += ['type = "Enum"', 'options = [ "Off", "On" ]', "default = " + toml_str("On" if st["default"] else "Off")]
    elif st["kind"] == "choice":
        lines += ['type = "Enum"', "options = [ " + ", ".join(toml_str(x) for x in st["choices"]) + " ]",
                  "default = " + toml_str(st["default"])]
    else:
        lines += ['type = "Number"', f'min = {st["min"]}', f'max = {st["max"]}', "step = 1", "precision = 0",
                  "percent = false", f'default = {st["default"]}']
open(os.path.join(OUT, "mod.toml"), "w", encoding="utf-8", newline="\n").write("\n".join(lines) + "\n")

settings = []
for st in S:
    d = {"key": st["key"]}
    if st["kind"] == "bool":
        d["type"] = "boolean"
        d["default"] = bool(st["default"])
    elif st["kind"] == "choice":
        d["type"] = "choice"
        d["choices"] = list(st["choices"])
        d["default"] = st["default"]
    else:
        d["type"] = "integer"
        d["default"] = int(st["default"])
        d["min"] = int(st["min"])
        d["max"] = int(st["max"])
    d["label"] = st["label"]
    if st["desc"]:
        d["description"] = st["desc"]
    settings.append(d)

manifest = {
    "id": "reo-greatest-hits-cheats",
    "name": NAME,
    "version": VERSION,
    "author": "RaccoonRecomp",
    "authors": ["Code Master, Lajos Szalay, Jay007, VirusPunk, Jarnold83 (codes, GameHacking.org)",
                "47iscool (Character 2/3 (CPU) Modifier, Go It Alone, Walk Through Walls)"],
    "description": HOST_DESCRIPTION,
    "icon": "icon.png",
    "game": {"id": "resident-evil-outbreak-file-1", "revisions": ["slus-20765-v2.00"]},
    "apiVersion": 1,
    "conflicts": [{"id": "reo-cheats",
                   "reason": "Both cheat mods patch the same game code on your disc; enable one at a time."}],
    "settings": settings,
}
open(os.path.join(OUT, "mod.json"), "w", encoding="utf-8", newline="\n").write(
    json.dumps(manifest, indent=2, ensure_ascii=False) + "\n")

manifest_ts = {
    "name": "REO_GreatestHits_Cheats",
    "version_number": VERSION,
    "website_url": "",
    "description": SHORT,
    "dependencies": [],
}
open(os.path.join(OUT, "manifest.json"), "w", encoding="utf-8", newline="\n").write(
    json.dumps(manifest_ts, indent=4) + "\n")


# README: the cheat table between the markers.
def cell(s):
    return s.replace("|", "\\|").replace("\n", "<br>")


def codes(lst):
    return "<br>".join(f"`{x}`" for x in lst) if lst else "not in your CodeBreaker list"


rows = ["| # | Cheat (option key) | Author | CodeBreaker v7 (your list, verbatim) | Plain code (your GH pnach) | "
        "Status on your disc | Evidence (probe runs) |", "|---|---|---|---|---|---|---|"]
n = 0
for c in CHEATS:
    num = ""
    if c["cb7_list"]:
        n += 1
        num = str(n)
    plain = codes(c["plain"]) + (f"<br>({cell(c['pnach_note'])})" if c["pnach_note"] else "")
    rows.append(f"| {num} | **{cell(c['label'])}** (`{c['key']}`) | {cell(c['author'])} | {codes(c['cb7'])} | {plain} | "
                f"{cell(c['status'])} | {cell(c['evidence'])} |")
table = "\n".join(rows)
readme_path = os.path.join(OUT, "README.md")
readme = open(readme_path, encoding="utf-8").read()
new = re.sub(r"(<!-- cheat-table:begin -->\n).*?(<!-- cheat-table:end -->)",
             lambda m: m.group(1) + table + "\n" + m.group(2), readme, flags=re.S)
assert table in new, "README.md has no cheat-table markers"
open(readme_path, "w", encoding="utf-8", newline="\n").write(new)

# The host refuses a default outside its own bounds, a label over 128 characters, a description over 4096 characters,
# a repeated key or a choice default that is not one of the choices.
for st in S:
    assert len(st["label"]) <= 128, st["key"]
    assert len(st["desc"]) <= 4096, st["key"]
    if st["kind"] == "int":
        assert st["min"] <= st["default"] <= st["max"], st["key"]
    if st["kind"] == "choice":
        assert st["default"] in st["choices"] and len(set(st["choices"])) == len(st["choices"]), st["key"]
keys = [st["key"] for st in S]
assert len(keys) == len(set(keys)), "duplicate keys"
assert len(SHORT) <= 250
assert len(HOST_DESCRIPTION) <= 4096
assert N_CB7 == 19, N_CB7
print(len(S), "settings written;", N_CB7, "CodeBreaker v7 cheats;", len(CHEATS) - N_CB7, "other pnach codes")
