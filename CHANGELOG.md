# Changelog

## 1.2.1

First release on GitHub: the mod as a downloadable zip for **Mods → Install Mods** (`release/reo-greatest-hits-cheats-1.2.1.zip`).
The mod's code and manifest are unchanged.

- **Partner cheats proven for every scenario of a session**, not only the first after the game starts. Character 2
  (CPU) Modifier, Character 3 (CPU) Modifier and Go It Alone write a partner's character byte only while that partner
  has not been created yet. The game clears the players' data when a scenario is left and again when the next one is
  set up, then creates the partners from those bytes when that scenario starts, so a choice made during a session
  applies from the next scenario on. Tested in the game: a first scenario with the game's own partners, a second
  scenario in the same session with exactly the chosen partners (and with none for Go It Alone). No code change was
  needed; the option texts now say it.
- A partner that is never created (no partner, Go It Alone) keeps being written all through the scenario, as the
  pnach does; choosing a character there in the middle of a scenario applies at the next one.

## 1.2.0

- **All 19 CodeBreaker v7 cheats and the rest of the Greatest Hits pnach.** New options: Become A Zombie After Death
  (Offline Mode), Character 2 (CPU) Modifier, Character 3 (CPU) Modifier, Don't Become A Zombie Over Time, Don't Become
  A Zombie When Bitten, Enemies Don't Damage You With Attacks, Extra Ammo, Go It Alone, Infinite Time, Max Infinite
  Ammo, P1 Press L1+Select For Infection 100%, Unlock All NPC's, Unlock Infinity Mode, Walk Through Walls & On Air,
  All Costumes, Have All Collection Available For Purchase, Have All Collection Purchased, Infinite Reward Points, Max
  Reward Points, Infinite Rewards Points Usage. Infinite Items and One-Hit Kills are renamed Infinite Item Usage and
  1-Hit Deaths (Enemies); their keys are unchanged.
- **Max Infinite Ammo fixed for the v2.00 disc**: as written the loaded rounds drop to 1; fixed, the first shot sets
  them to 99 and they stay 99.
- **The three ammo cheats are mutually exclusive**: Infinite Ammo, Extra Ammo and Max Infinite Ammo change the same
  instructions, so only one is in the game's code at a time (Max Infinite Ammo, else Extra Ammo, else Infinite Ammo).
- **Character 2 / 3 (CPU) Modifier**: the code's `??` became a choice of character (Kevin, Mark, Jim, George, David,
  Alyssa, Yoko, Cindy, or no partner), with the game's own character numbers.
- **Partner cheats never change a partner in the middle of a scenario.**
- **Don't Become A Zombie Over Time / When Bitten** are options of their own; No Infection / Freeze Infection still
  turns both on.
- **The two jokers work as jokers**: P1 Press L1+Select For Infection 100% and Walk Through Walls read the controller
  and switch their code as the pnach lines do, and write nothing while the code they change is not loaded.
- **Unlock All NPC's fixed for the v2.00 disc**: as written it is one byte off; the mod sets exactly the bits of the
  game's 40 collection NPCs.
- **Save data: only values the game itself can write** (the scenario, mode, costume and collection bits the game's own
  unlock and purchase code sets; points never above the game's limit).
- Infinite Time is included as written (it acts only in online play); Infinite Rewards Points Usage is included but
  not active (it does not fit the v2.00 disc).

## 1.1.0 and earlier

- The first versions: Infinite Health, Infinite Ammo, Infinite Items, Unlock All Scenarios, Max Inventory, No
  Infection / Freeze Infection (with its freeze value) and One-Hit Kills, as switches of one mod for the SLUS-20765
  v2.00 disc. Code cheats are written as patch structures: a guard, the writes, and a restore of the original
  instructions when the cheat is off. The switches of these versions keep their keys and meaning in 1.2.x.
