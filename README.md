# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore Module: mod-realm-first-titles

[![AzerothCore Module](https://img.shields.io/badge/AzerothCore-Module-red?style=flat-square&logo=github)](https://github.com/azerothcore/azerothcore-wotlk)
[![C++20](https://img.shields.io/badge/Language-C++20-00599C?style=flat-square&logo=c%2B%2B)](https://isocpp.org/)
[![Branch 3.3.5a](https://img.shields.io/badge/Branch-3.3.5a-orange?style=flat-square)](https://github.com/azerothcore/azerothcore-wotlk)
[![License AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue?style=flat-square)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/AlsoNotMehh/mod-realm-first-titles?style=flat-square&color=yellow&logo=github)](https://github.com/AlsoNotMehh/mod-realm-first-titles/stargazers)

An automatic **Realm First Title & Achievement Management System** for **AzerothCore (WotLK 3.3.5a)** that validates, restores, grants, and audits all **43 canonical Realm First achievements and titles** (Level 80 Overall, Classes, Races, 450 Grand Master Professions, and Raids) with GM testing protection, anti-cheat timestamp auditing, and automated offline character repair.

---

### 💡 Why this module?
In retail World of Warcraft 3.3.5a, Blizzard designed iconic Realm First achievements for reaching Level 80 (Overall, Classes, Races) and Grand Master (450) Professions. However, many associated titles—such as `Assassin <Name>`, `Archmage <Name>`, `<Name> the Supreme`, `Iron Chef <Name>`, and `<Name>, Grand Master Blacksmith`—were left disabled, ungranted, or desynchronized in standard server databases.

**`mod-realm-first-titles`** provides a complete, production-ready solution for private servers:
- **Restores 43 Realm First Titles:** Grants legitimate titles for all Level 80 classes, races, professions, and Wrath raid encounters.
- **Timestamp-Based Anti-Cheat & Canonical Winner Guarantee:** Audits achievement completion timestamps in the database to guarantee only the true, earliest first achiever holds and keeps the prestigious title.
- **Staff / GM Testing Protection:** Automatically prevents GameMaster accounts (`Security > 0` or `.gm on`) from accidentally triggering or consuming Realm First achievements while testing or using administrative commands (e.g. `.levelup 80`, `.setskill 450`).
- **Startup Database Audit & Offline Character Repair:** Automatically scans, audits, and fixes missing or out-of-sync title bitmasks for both online and offline characters on worldserver startup.
- **Instant Login Synchronization:** Validates player achievements on login and immediately rewards missing titles.
- **In-Game Administration Suite:** Complete set of `.realmfirst` GM commands for auditing, inspecting, listing, and granting titles in real-time.

---

## 🏆 Supported Realm First Titles (43 Total)

### 🥇 Level 80 Overall
| Achievement | ID | Rewarded Title | Title ID |
| :--- | :---: | :--- | :---: |
| Realm First! Level 80 | `457` | %s the Supreme | `85` |

---

### ⚔️ Level 80 Classes (10 Titles)
| Class | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| **Rogue** | Realm First! Level 80 Rogue | `458` | Assassin %s | `95` |
| **Warrior** | Realm First! Level 80 Warrior | `459` | Warbringer %s | `94` |
| **Mage** | Realm First! Level 80 Mage | `460` | Archmage %s | `93` |
| **Death Knight** | Realm First! Level 80 Death Knight | `461` | %s of the Ebon Blade | `92` |
| **Hunter** | Realm First! Level 80 Hunter | `462` | Stalker %s | `91` |
| **Warlock** | Realm First! Level 80 Warlock | `463` | %s the Malefic | `90` |
| **Priest** | Realm First! Level 80 Priest | `464` | Prophet %s | `89` |
| **Paladin** | Realm First! Level 80 Paladin | `465` | Crusader %s | `156` |
| **Druid** | Realm First! Level 80 Druid | `466` | %s of the Emerald Dream | `87` |
| **Shaman** | Realm First! Level 80 Shaman | `467` | %s of the Ten Storms | `86` |

---

### 🛡️ Level 80 Races (10 Titles)
| Race | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| **Gnome** | Realm First! Level 80 Gnome | `1404` | %s of Gnomeregan | `113` |
| **Blood Elf** | Realm First! Level 80 Blood Elf | `1405` | %s of Quel'Thalas | `110` |
| **Draenei** | Realm First! Level 80 Draenei | `1406` | %s of Argus | `111` |
| **Dwarf** | Realm First! Level 80 Dwarf | `1407` | %s of Khaz Modan | `112` |
| **Human** | Realm First! Level 80 Human | `1408` | %s the Lion Hearted | `114` |
| **Night Elf** | Realm First! Level 80 Night Elf | `1409` | %s, Champion of Elune | `115` |
| **Orc** | Realm First! Level 80 Orc | `1410` | %s, Hero of Orgrimmar | `116` |
| **Tauren** | Realm First! Level 80 Tauren | `1411` | %s the Plainsrunner | `117` |
| **Troll** | Realm First! Level 80 Troll | `1412` | %s of the Darkspear | `118` |
| **Undead** | Realm First! Level 80 Undead | `1413` | %s the Forsaken | `119` |

---

### 🔨 Grand Master (450) Professions (15 Titles)
| Profession | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| **Blacksmithing** | Realm First! Grand Master Blacksmith | `1414` | %s, Grand Master Blacksmith | `97` |
| **Alchemy** | Realm First! Grand Master Alchemist | `1415` | %s, Grand Master Alchemist | `96` |
| **Cooking** | Realm First! Grand Master Chef | `1416` | Iron Chef %s | `98` |
| **Enchanting** | Realm First! Grand Master Enchanter | `1417` | %s, Grand Master Enchanter | `99` |
| **Engineering** | Realm First! Grand Master Engineer | `1418` | %s, Grand Master Engineer | `100` |
| **First Aid** | Realm First! Grand Master Medic | `1419` | Doctor %s | `101` |
| **Fishing** | Realm First! Grand Master Angler | `1420` | %s, Grand Master Angler | `102` |
| **Herbalism** | Realm First! Grand Master Herbalist | `1421` | %s, Grand Master Herbalist | `103` |
| **Inscription** | Realm First! Grand Master Scribe | `1422` | %s, Grand Master Scribe | `104` |
| **Jewelcrafting** | Realm First! Grand Master Jewelcrafter | `1423` | %s, Grand Master Jewelcrafter | `105` |
| **Leatherworking** | Realm First! Grand Master Leatherworker | `1424` | %s, Grand Master Leatherworker | `106` |
| **Mining** | Realm First! Grand Master Miner | `1425` | %s, Grand Master Miner | `107` |
| **Skinning** | Realm First! Grand Master Skinner | `1426` | %s, Grand Master Skinner | `108` |
| **Tailoring** | Realm First! Grand Master Tailor | `1427` | %s, Grand Master Tailor | `109` |
| **All 450s** | Realm First! Northrend Vanguard | `1463` | %s, Hero of Northrend | `123` |

---

### 💀 WotLK Raids (7 Encounters)
| Raid Encounter | Achievement | ID | Rewarded Title | Title ID |
| :--- | :--- | :---: | :--- | :---: |
| **Sartharion (3 Drakes)** | Realm First! Obsidian Slayer | `456` | %s the Obsidian Slayer | `139` |
| **Malygos** | Realm First! The Magic Seeker | `1400` | %s the Magic Seeker | `120` |
| **Naxxramas** | Realm First! Conqueror of Naxxramas | `1402` | %s, Conqueror of Naxxramas | `122` |
| **Yogg-Saron (+0)** | Realm First! Death's Demise | `3117` | %s, Death's Demise | `158` |
| **Algalon the Observer** | Realm First! Celestial Defender | `3259` | %s the Celestial Defender | `159` |
| **Trial of the Grand Crusader** | Realm First! Grand Crusader | `4078` | %s the Grand Crusader | `170` |
| **The Lich King (25H)** | Realm First! Fall of the Lich King | `4576` | %s, The Light of Dawn *(Optional)* | `173` |

---

## 📦 Features

- **Canonical Winner Enforcement:** Compares achievement completion timestamps in the database to guarantee only the legitimate first player keeps each title. Duplicate or invalid title holders are cleanly revoked.
- **GM & Staff Protection:** Prohibits GameMasters (`Security > 0` or `.gm on`) from triggering watched Realm First achievements during administrative tasks or leveling tests.
- **Automatic Offline Character Repair:** Checks character title bitmasks on server startup and repairs missing or desynchronized titles for both online and offline players.
- **Login Synchronization:** Re-validates watched achievements whenever a character enters the world and awards any missing titles immediately.
- **Customizable Title Equipping:** Automatically equips newly awarded Realm First titles upon acquisition or login (configurable).
- **In-Game Audit Suite:** Complete set of `.realmfirst` commands to view winners, list holders, audit missing titles, or perform live bitmask repairs.

---

## 🛡️ GM Commands

| Command | Permission | Description |
| :--- | :---: | :--- |
| `.realmfirst check [<player>]` | GM 2 | Inspects a player's watched Realm First achievements, timestamps, and current title statuses. |
| `.realmfirst refresh [<player>]` | GM 2 | Re-evaluates and grants any missing watched Realm First titles to target player or self. |
| `.realmfirst sync [<player>]` | GM 2 | Alias for `.realmfirst refresh`. |
| `.realmfirst grant <achievementId> [<player>]` | GM 3 | Instantly awards the specified Realm First achievement and equips its title. |
| `.realmfirst audit` | GM 3 | Scans the entire database and repairs all missing/extra titles across online and offline characters. |
| `.realmfirst missing` | GM 2 | Non-destructive audit: lists all characters holding watched achievements without their title. |
| `.realmfirst achievements` | GM 1 | Displays all characters currently holding watched Realm First achievements. |
| `.realmfirst winners` | GM 1 | Alias for `.realmfirst achievements`. |
| `.realmfirst titles` | GM 1 | Displays all characters currently holding watched Realm First titles. |

---

## ⚙️ Configuration Reference (`mod_realm_first_titles.conf`)

| Setting | Default | Description |
| :--- | :---: | :--- |
| `RealmFirstTitles.Enabled` | `1` | Master switch to enable or disable the Realm First Titles module. |
| `RealmFirstTitles.GrantBlizzlikeRaidTitles` | `1` | Enable Blizzlike Wrath realm-first raid titles (Obsidian Slayer, Death's Demise, etc.). |
| `RealmFirstTitles.GrantRestoredBetaTitles` | `1` | Enable restored Level 80 class, race, and profession titles (The Supreme, Assassin, etc.). |
| `RealmFirstTitles.GrantLightOfDawnFallback` | `0` | Enable fallback title grant for Realm First: Fall of the Lich King (Achievement 4576). |
| `RealmFirstTitles.RequireCanonicalFirstHolder` | `0` | `0` = All achievement holders keep title. `1` = Strictly earliest database timestamp winner only. |
| `RealmFirstTitles.PreventGMAchievements` | `1` | Block GM accounts (`Security > 0` or `.gm on`) from earning Realm Firsts via admin commands. |
| `RealmFirstTitles.SyncOnLogin` | `1` | Re-validate and grant missing titles whenever an eligible character logs in. |
| `RealmFirstTitles.RepairOnStartup` | `1` | Automatically audit and repair character title bitmasks on server startup. |
| `RealmFirstTitles.AutoSetCurrentTitle` | `1` | Automatically set newly granted realm-first titles as the player's active title. |
| `RealmFirstTitles.AnnounceGrantedTitles` | `1` | Send a confirmation whisper/notification to the player when their title is granted. |
| `RealmFirstTitles.AuditRepairOfflineCharacters` | `1` | Enable offline character bitmask repair directly in the database during audits. |

---

## 🚀 Installation Guide

1. Clone into your AzerothCore `modules` directory:
   ```bash
   cd azerothcore/modules
   git clone https://github.com/AlsoNotMehh/mod-realm-first-titles.git
   ```
2. Re-run CMake and compile your server:
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
3. Copy `conf/mod_realm_first_titles.conf.dist` to your `worldserver` configs folder as `mod_realm_first_titles.conf`:
   ```bash
   cp modules/mod-realm-first-titles/conf/mod_realm_first_titles.conf.dist configs/mod_realm_first_titles.conf
   ```

---

## ⭐ Show your support

If you find this module helpful for your server, please consider giving it a **star on GitHub**! It helps more developers in the AzerothCore community discover the project.

---

## 👤 Credits

- **Author:** [AlsoNotMehh](https://github.com/AlsoNotMehh) ([Discord](https://discord.com/users/1063304041419001966) / [Email](mailto:itsbrayanrodriguez@gmail.com))
- **Framework:** [AzerothCore](https://www.azerothcore.org)

---

## 📄 License

This project is licensed under the [GNU AGPL v3 License](LICENSE).
