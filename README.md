# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore Module: mod-realm-first-titles

[![AzerothCore Module](https://img.shields.io/badge/AzerothCore-Module-red?style=flat-square&logo=github)](https://github.com/azerothcore/azerothcore-wotlk)
[![C++20](https://img.shields.io/badge/Language-C++20-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![Branch 3.3.5a](https://img.shields.io/badge/Branch-3.3.5a-orange?style=flat-square)](https://github.com/azerothcore/azerothcore-wotlk)
[![License AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue?style=flat-square)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/AlsoNotMehh/mod-realm-first-titles?style=flat-square&color=yellow&logo=github)](https://github.com/AlsoNotMehh/mod-realm-first-titles/stargazers)

An AzerothCore module for Wrath of the Lich King (3.3.5a) that validates, grants, and audits canonical Realm First achievements and titles.

In retail 3.3.5a, Blizzard implemented Realm First achievements for Level 80 characters, classes, races, and Grand Master professions, but left many associated title rewards disabled. This module restores those titles, guarantees only canonical first achievers receive and retain them, blocks GameMaster accounts from accidentally earning them during testing, and automatically repairs title bitmasks on server startup.

---

## Supported Realm First Titles (43 Total)

### Level 80 Overall
| Achievement | ID | Rewarded Title | Title ID |
| :--- | :---: | :--- | :---: |
| Realm First! Level 80 | `457` | %s the Supreme | `85` |

---

### Level 80 Classes
| Class | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| Rogue | Realm First! Level 80 Rogue | `458` | Assassin %s | `95` |
| Warrior | Realm First! Level 80 Warrior | `459` | Warbringer %s | `94` |
| Mage | Realm First! Level 80 Mage | `460` | Archmage %s | `93` |
| Death Knight | Realm First! Level 80 Death Knight | `461` | %s of the Ebon Blade | `92` |
| Hunter | Realm First! Level 80 Hunter | `462` | Stalker %s | `91` |
| Warlock | Realm First! Level 80 Warlock | `463` | %s the Malefic | `90` |
| Priest | Realm First! Level 80 Priest | `464` | Prophet %s | `89` |
| Paladin | Realm First! Level 80 Paladin | `465` | Crusader %s | `156` |
| Druid | Realm First! Level 80 Druid | `466` | %s of the Emerald Dream | `87` |
| Shaman | Realm First! Level 80 Shaman | `467` | %s of the Ten Storms | `86` |

---

### Level 80 Races
| Race | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| Gnome | Realm First! Level 80 Gnome | `1404` | %s of Gnomeregan | `113` |
| Blood Elf | Realm First! Level 80 Blood Elf | `1405` | %s of Quel'Thalas | `110` |
| Draenei | Realm First! Level 80 Draenei | `1406` | %s of Argus | `111` |
| Dwarf | Realm First! Level 80 Dwarf | `1407` | %s of Khaz Modan | `112` |
| Human | Realm First! Level 80 Human | `1408` | %s the Lion Hearted | `114` |
| Night Elf | Realm First! Level 80 Night Elf | `1409` | %s, Champion of Elune | `115` |
| Orc | Realm First! Level 80 Orc | `1410` | %s, Hero of Orgrimmar | `116` |
| Tauren | Realm First! Level 80 Tauren | `1411` | %s the Plainsrunner | `117` |
| Troll | Realm First! Level 80 Troll | `1412` | %s of the Darkspear | `118` |
| Undead | Realm First! Level 80 Undead | `1413` | %s the Forsaken | `119` |

---

### Grand Master (450) Professions
| Profession | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| Blacksmithing | Realm First! Grand Master Blacksmith | `1414` | %s, Grand Master Blacksmith | `97` |
| Alchemy | Realm First! Grand Master Alchemist | `1415` | %s, Grand Master Alchemist | `96` |
| Cooking | Realm First! Grand Master Chef | `1416` | Iron Chef %s | `98` |
| Enchanting | Realm First! Grand Master Enchanter | `1417` | %s, Grand Master Enchanter | `99` |
| Engineering | Realm First! Grand Master Engineer | `1418` | %s, Grand Master Engineer | `100` |
| First Aid | Realm First! Grand Master Medic | `1419` | Doctor %s | `101` |
| Fishing | Realm First! Grand Master Angler | `1420` | %s, Grand Master Angler | `102` |
| Herbalism | Realm First! Grand Master Herbalist | `1421` | %s, Grand Master Herbalist | `103` |
| Inscription | Realm First! Grand Master Scribe | `1422` | %s, Grand Master Scribe | `104` |
| Jewelcrafting | Realm First! Grand Master Jewelcrafter | `1423` | %s, Grand Master Jewelcrafter | `105` |
| Leatherworking | Realm First! Grand Master Leatherworker | `1424` | %s, Grand Master Leatherworker | `106` |
| Mining | Realm First! Grand Master Miner | `1425` | %s, Grand Master Miner | `107` |
| Skinning | Realm First! Grand Master Skinner | `1426` | %s, Grand Master Skinner | `108` |
| Tailoring | Realm First! Grand Master Tailor | `1427` | %s, Grand Master Tailor | `109` |
| All 450s | Realm First! Northrend Vanguard | `1463` | %s, Hero of Northrend | `123` |

---

### WotLK Raids
| Raid Encounter | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| Sartharion (3 Drakes) | Realm First! Obsidian Slayer | `456` | %s the Obsidian Slayer | `139` |
| Malygos | Realm First! The Magic Seeker | `1400` | %s the Magic Seeker | `120` |
| Naxxramas | Realm First! Conqueror of Naxxramas | `1402` | %s, Conqueror of Naxxramas | `122` |
| Yogg-Saron (+0) | Realm First! Death's Demise | `3117` | %s, Death's Demise | `158` |
| Algalon the Observer | Realm First! Celestial Defender | `3259` | %s the Celestial Defender | `159` |
| Trial of the Grand Crusader | Realm First! Grand Crusader | `4078` | %s the Grand Crusader | `170` |
| The Lich King (25H) | Realm First! Fall of the Lich King | `4576` | %s, The Light of Dawn *(Optional)* | `173` |

---

## Features

- **Canonical Holder Detection**: Queries database timestamps to ensure only the true first character to earn each achievement keeps the title. Illegitimate or duplicate title holders are revoked.
- **GM Account Protection**: Blocks GM/staff accounts (`Security > 0` or `.gm on`) from completing Realm First achievements or advancing criteria during administrative testing (e.g. `.levelup 80`, `.setskill 450`).
- **Offline Character Auto-Repair**: Scans and repairs missing or desynchronized title bitmasks on server startup for both online and offline characters.
- **Login Synchronization**: Validates and updates titles immediately upon player login.
- **In-Game GM Commands**: Full command suite for live inspection, synchronization, and database auditing.

---

## GM Commands

| Command | Description |
| :--- | :--- |
| `.realmfirst check [<player>]` | Inspect character watched achievements and title status. |
| `.realmfirst refresh [<player>]` | Sync and grant missing realm first titles for yourself or a target player. |
| `.realmfirst sync [<player>]` | Alias for `.realmfirst refresh`. |
| `.realmfirst grant <achievementId> [<player>]` | Instantly grants the realm first achievement and equips the title on target or self. |
| `.realmfirst audit` | Scan entire database and repair all missing/extra titles for online and offline players. |
| `.realmfirst missing` | Non-destructive scan and list of all missing title holders. |
| `.realmfirst achievements` | List all characters currently holding watched realm first achievements. |
| `.realmfirst winners` | Alias for `.realmfirst achievements`. |
| `.realmfirst titles` | List all characters currently holding watched realm first titles. |

---

## Configuration (`mod_realm_first_titles.conf`)

| Setting | Default | Description |
| :--- | :---: | :--- |
| `RealmFirstTitles.Enabled` | `1` | Enable or disable the Realm First Titles module. |
| `RealmFirstTitles.GrantBlizzlikeRaidTitles` | `1` | Enable Blizzlike Wrath realm-first raid titles (e.g. Obsidian Slayer, Death's Demise). |
| `RealmFirstTitles.GrantRestoredBetaTitles` | `1` | Enable restored Level 80 class, race, and profession titles (The Supreme, Assassin, etc.). |
| `RealmFirstTitles.GrantLightOfDawnFallback` | `0` | Enable fallback title grant for Realm First: Fall of the Lich King (Achievement 4576). |
| `RealmFirstTitles.RequireCanonicalFirstHolder` | `0` | `0` = All achievement holders get the title. `1` = Strictly earliest timestamp winner only. |
| `RealmFirstTitles.PreventGMAchievements` | `1` | Block GM accounts from accidentally earning Realm Firsts through gameplay leveling/testing. |
| `RealmFirstTitles.SyncOnLogin` | `1` | Re-validate and grant missing titles whenever a player logs in. |
| `RealmFirstTitles.RepairOnStartup` | `1` | Automatically audit and repair character title bitmasks on server boot. |
| `RealmFirstTitles.AutoSetCurrentTitle` | `1` | Automatically equip newly granted realm-first titles. |
| `RealmFirstTitles.AnnounceGrantedTitles` | `1` | Send a confirmation message to the player when their title is granted. |
| `RealmFirstTitles.AuditRepairOfflineCharacters` | `1` | Allow offline character bitmask repairs directly in the database. |

---

## Installation

1. Clone into your AzerothCore `modules` directory:
   ```bash
   cd azerothcore/modules
   git clone https://github.com/AlsoNotMehh/mod-realm-first-titles.git
   ```
2. Re-run CMake and compile:
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
3. Copy `conf/mod_realm_first_titles.conf.dist` to your `configs` directory as `mod_realm_first_titles.conf`.

---

## License

This project is licensed under the [GNU AGPL v3 License](LICENSE).
