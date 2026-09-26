/*
 * AzerothCore Module: mod-realm-first-titles
 * Author: AlsoNotMehh
 * License: GNU AGPL v3
 */

#include "AchievementMgr.h"
#include "Chat.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "PlayerScript.h"
#include "ScriptMgr.h"
#include "WorldScript.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace Acore::ChatCommands;

namespace
{
enum class RewardGroup : uint8
{
    BlizzlikeRaid,
    RestoredBeta,
    LightOfDawnFallback
};

struct RealmFirstTitleReward
{
    uint32 AchievementId;
    uint32 TitleId;
    RewardGroup Group;
    char const* Label;
};

std::array<RealmFirstTitleReward, 43> constexpr RealmFirstTitleRewards =
{{
    // Blizzlike Wrath realm-first raid titles.
    { 456,  139, RewardGroup::BlizzlikeRaid,        "Obsidian Slayer" },
    { 1400, 120, RewardGroup::BlizzlikeRaid,        "The Magic Seeker" },
    { 1402, 122, RewardGroup::BlizzlikeRaid,        "Conqueror of Naxxramas" },
    { 3117, 158, RewardGroup::BlizzlikeRaid,        "Death's Demise" },
    { 3259, 159, RewardGroup::BlizzlikeRaid,        "The Celestial Defender" },
    { 4078, 170, RewardGroup::BlizzlikeRaid,        "Grand Crusader" },

    // The realm-first Lich King achievement had no unique title in live 3.3.5.
    // This optional fallback only grants the normal 25H title if explicitly enabled.
    { 4576, 173, RewardGroup::LightOfDawnFallback,  "The Light of Dawn" },

    // Restored Level 80 Classes, Races, and Grand Master Profession Realm First titles.
    { 457,  85,  RewardGroup::RestoredBeta,         "The Supreme" },
    { 458,  95,  RewardGroup::RestoredBeta,         "Assassin" },
    { 459,  94,  RewardGroup::RestoredBeta,         "Warbringer" },
    { 460,  93,  RewardGroup::RestoredBeta,         "Archmage" },
    { 461,  92,  RewardGroup::RestoredBeta,         "Of the Ebon Blade" },
    { 462,  91,  RewardGroup::RestoredBeta,         "Stalker" },
    { 463,  90,  RewardGroup::RestoredBeta,         "The Malefic" },
    { 464,  89,  RewardGroup::RestoredBeta,         "Prophet" },
    { 465,  156, RewardGroup::RestoredBeta,         "Crusader" },
    { 466,  87,  RewardGroup::RestoredBeta,         "Of the Emerald Dream" },
    { 467,  86,  RewardGroup::RestoredBeta,         "Of the Ten Storms" },
    { 1404, 113, RewardGroup::RestoredBeta,         "Of Gnomeregan" },
    { 1405, 110, RewardGroup::RestoredBeta,         "Of Quel'Thalas" },
    { 1406, 111, RewardGroup::RestoredBeta,         "Of Argus" },
    { 1407, 112, RewardGroup::RestoredBeta,         "Of Khaz Modan" },
    { 1408, 114, RewardGroup::RestoredBeta,         "The Lion Hearted" },
    { 1409, 115, RewardGroup::RestoredBeta,         "Champion of Elune" },
    { 1410, 116, RewardGroup::RestoredBeta,         "Hero of Orgrimmar" },
    { 1411, 117, RewardGroup::RestoredBeta,         "Plainsrunner" },
    { 1412, 118, RewardGroup::RestoredBeta,         "Of the Darkspear" },
    { 1413, 119, RewardGroup::RestoredBeta,         "The Forsaken" },
    { 1414, 97,  RewardGroup::RestoredBeta,         "Grand Master Blacksmith" },
    { 1415, 96,  RewardGroup::RestoredBeta,         "Grand Master Alchemist" },
    { 1416, 98,  RewardGroup::RestoredBeta,         "Iron Chef" },
    { 1417, 99,  RewardGroup::RestoredBeta,         "Grand Master Enchanter" },
    { 1418, 100, RewardGroup::RestoredBeta,         "Grand Master Engineer" },
    { 1419, 101, RewardGroup::RestoredBeta,         "Doctor" },
    { 1420, 102, RewardGroup::RestoredBeta,         "Grand Master Angler" },
    { 1421, 103, RewardGroup::RestoredBeta,         "Grand Master Herbalist" },
    { 1422, 104, RewardGroup::RestoredBeta,         "Grand Master Scribe" },
    { 1423, 105, RewardGroup::RestoredBeta,         "Grand Master Jewelcrafter" },
    { 1424, 106, RewardGroup::RestoredBeta,         "Grand Master Leatherworker" },
    { 1425, 107, RewardGroup::RestoredBeta,         "Grand Master Miner" },
    { 1426, 108, RewardGroup::RestoredBeta,         "Grand Master Skinner" },
    { 1427, 109, RewardGroup::RestoredBeta,         "Grand Master Tailor" },
    { 1463, 123, RewardGroup::RestoredBeta,         "Hero of Northrend" },
}};

struct RealmFirstTitleSettings
{
    bool Enabled = true;
    bool GrantBlizzlikeRaidTitles = true;
    bool GrantRestoredBetaTitles = true;
    bool GrantLightOfDawnFallback = false;
    bool RequireCanonicalFirstHolder = true;
    bool SyncOnLogin = true;
    bool RepairOnStartup = true;
    bool AnnounceGrantedTitles = true;
    bool AutoSetCurrentTitle = true;
    bool AuditRepairOfflineCharacters = true;
    bool PreventGMAchievements = true;
    uint32 AuditOutputLimit = 25;
    uint32 ReportOutputLimit = 50;
};

RealmFirstTitleSettings Settings;

bool IsGMPlayer(Player const* player)
{
    if (!player)
        return false;

    if (player->IsGameMaster())
        return true;

    if (player->GetSession() && player->GetSession()->GetSecurity() > SEC_PLAYER)
        return true;

    return false;
}

bool IsRewardEnabled(RealmFirstTitleReward const& reward)
{
    switch (reward.Group)
    {
        case RewardGroup::BlizzlikeRaid:
            return Settings.GrantBlizzlikeRaidTitles;
        case RewardGroup::RestoredBeta:
            return Settings.GrantRestoredBetaTitles;
        case RewardGroup::LightOfDawnFallback:
            return Settings.GrantLightOfDawnFallback;
    }

    return false;
}

RealmFirstTitleReward const* GetRewardForAchievement(uint32 achievementId)
{
    if (!Settings.Enabled)
        return nullptr;

    auto itr = std::find_if(RealmFirstTitleRewards.begin(), RealmFirstTitleRewards.end(), [achievementId](RealmFirstTitleReward const& reward)
    {
        return reward.AchievementId == achievementId && IsRewardEnabled(reward);
    });

    return itr != RealmFirstTitleRewards.end() ? &*itr : nullptr;
}

bool IsRealmFirstAchievement(AchievementEntry const* achievement)
{
    if (!achievement)
        return false;

    if (achievement->flags & (ACHIEVEMENT_FLAG_REALM_FIRST_KILL | ACHIEVEMENT_FLAG_REALM_FIRST_REACH))
        return true;

    return GetRewardForAchievement(achievement->ID) != nullptr;
}

std::vector<RealmFirstTitleReward const*> GetActiveRewards()
{
    std::vector<RealmFirstTitleReward const*> rewards;
    if (!Settings.Enabled)
        return rewards;

    for (RealmFirstTitleReward const& reward : RealmFirstTitleRewards)
        if (IsRewardEnabled(reward))
            rewards.push_back(&reward);

    return rewards;
}

std::string BuildActiveAchievementCsv()
{
    std::ostringstream stream;
    bool first = true;

    for (RealmFirstTitleReward const* reward : GetActiveRewards())
    {
        if (!first)
            stream << ',';

        stream << reward->AchievementId;
        first = false;
    }

    return stream.str();
}

char const* GetTitleName(CharTitlesEntry const* title, Player const* player)
{
    if (!title || !player)
        return "";

    LocaleConstant locale = player->GetSession() ? player->GetSession()->GetSessionDbLocaleIndex() : LOCALE_enUS;
    char const* name = player->getGender() == GENDER_FEMALE ? title->nameFemale[locale] : title->nameMale[locale];
    if (!name || !*name)
        name = player->getGender() == GENDER_FEMALE ? title->nameFemale[LOCALE_enUS] : title->nameMale[LOCALE_enUS];

    return name ? name : "";
}

std::optional<ObjectGuid::LowType> GetCanonicalRealmFirstHolderGuid(uint32 achievementId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT `guid` FROM `character_achievement` "
        "WHERE `achievement` = {} "
        "ORDER BY `date` ASC, `guid` ASC LIMIT 1",
        achievementId);

    if (!result)
        return std::nullopt;

    return result->Fetch()[0].Get<uint32>();
}

bool IsCanonicalRealmFirstHolder(ObjectGuid::LowType guid, uint32 achievementId)
{
    std::optional<ObjectGuid::LowType> canonicalGuid = GetCanonicalRealmFirstHolderGuid(achievementId);
    return canonicalGuid && *canonicalGuid == guid;
}

bool RemoveRealmFirstTitle(Player* player, RealmFirstTitleReward const& reward)
{
    if (!player)
        return false;

    CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward.TitleId);
    if (!title || !player->HasTitle(title))
        return false;

    player->SetTitle(title, true);
    LOG_INFO("module.realmfirsttitles", "Removed title {} ({}) from {} because this character is not eligible for achievement {}.",
        reward.TitleId, reward.Label, player->GetName(), reward.AchievementId);
    return true;
}

bool EnsureRealmFirstTitle(Player* player, RealmFirstTitleReward const& reward, bool announce)
{
    if (!player || !Settings.Enabled)
        return false;

    AchievementEntry const* achievement = sAchievementStore.LookupEntry(reward.AchievementId);
    if (!achievement)
        return false;

    CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward.TitleId);
    if (!title)
    {
        LOG_WARN("module.realmfirsttitles", "RealmFirstTitles: missing CharTitles.dbc entry {} for achievement {} ({}).",
            reward.TitleId, reward.AchievementId, reward.Label);
        return false;
    }

    bool const hasAchieved = player->HasAchieved(reward.AchievementId);
    bool const isCanonical = !Settings.RequireCanonicalFirstHolder || IsCanonicalRealmFirstHolder(player->GetGUID().GetCounter(), reward.AchievementId);
    bool const shouldHaveTitle = hasAchieved && isCanonical;

    if (!shouldHaveTitle)
    {
        if (!hasAchieved)
            RemoveRealmFirstTitle(player, reward);
        return false;
    }

    if (player->HasTitle(title))
        return false;

    player->SetTitle(title);

    if (Settings.AutoSetCurrentTitle)
        player->SetCurrentTitle(title);

    if (announce && Settings.AnnounceGrantedTitles)
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            "Realm first title confirmed: {}.",
            GetTitleName(title, player));
    }

    LOG_INFO("module.realmfirsttitles", "Granted title {} ({}) to {} for achievement {}.",
        reward.TitleId, reward.Label, player->GetName(), reward.AchievementId);

    return true;
}

uint32 SyncRealmFirstTitles(Player* player, bool announce)
{
    if (!player || !Settings.Enabled)
        return 0;

    uint32 granted = 0;
    for (RealmFirstTitleReward const* reward : GetActiveRewards())
        if (EnsureRealmFirstTitle(player, *reward, announce))
            ++granted;

    return granted;
}

void ValidateRealmFirstTitleRewards()
{
    if (!Settings.Enabled)
        return;

    uint32 active = 0;
    uint32 missingAchievements = 0;
    uint32 missingTitles = 0;

    for (RealmFirstTitleReward const* reward : GetActiveRewards())
    {
        ++active;
        std::string const label = reward->Label ? reward->Label : "";

        if (!sAchievementStore.LookupEntry(reward->AchievementId))
        {
            ++missingAchievements;
            LOG_ERROR("module.realmfirsttitles",
                "RealmFirstTitles: active reward '{}' references missing Achievement.dbc id {}.",
                label, reward->AchievementId);
        }

        if (!sCharTitlesStore.LookupEntry(reward->TitleId))
        {
            ++missingTitles;
            LOG_ERROR("module.realmfirsttitles",
                "RealmFirstTitles: active reward '{}' references missing CharTitles.dbc id {}.",
                label, reward->TitleId);
        }
    }

    LOG_INFO("module.realmfirsttitles",
        "RealmFirstTitles: validated {} active reward mapping(s), missingAchievements={}, missingTitles={}.",
        active, missingAchievements, missingTitles);
}

void SendPlayerStatus(ChatHandler* handler, Player* player)
{
    if (!handler || !player)
        return;

    uint32 matchingAchievements = 0;
    uint32 missingTitles = 0;

    for (RealmFirstTitleReward const* reward : GetActiveRewards())
    {
        if (!player->HasAchieved(reward->AchievementId))
            continue;

        ++matchingAchievements;
        CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleId);
        if (!title || !player->HasTitle(title))
            ++missingTitles;
    }

    handler->PSendSysMessage(
        "RealmFirstTitles: {} has {} watched achievements; {} missing titles.",
        player->GetName(), matchingAchievements, missingTitles);
}

std::array<uint32, 6> ParseKnownTitles(std::string const& text)
{
    std::array<uint32, 6> values = { 0, 0, 0, 0, 0, 0 };
    std::stringstream stream(text);
    for (std::size_t i = 0; i < values.size() && (stream >> values[i]); ++i) { }
    return values;
}

bool HasKnownTitle(std::array<uint32, 6> const& knownTitles, CharTitlesEntry const* title)
{
    if (!title || title->bit_index >= knownTitles.size() * 32)
        return false;

    uint32 const offset = title->bit_index / 32;
    uint32 const flag = 1u << (title->bit_index % 32);
    return (knownTitles[offset] & flag) != 0;
}

bool AddKnownTitle(std::array<uint32, 6>& knownTitles, CharTitlesEntry const* title)
{
    if (!title || title->bit_index >= knownTitles.size() * 32)
        return false;

    uint32 const offset = title->bit_index / 32;
    uint32 const flag = 1u << (title->bit_index % 32);
    if (knownTitles[offset] & flag)
        return false;

    knownTitles[offset] |= flag;
    return true;
}

bool RemoveKnownTitle(std::array<uint32, 6>& knownTitles, CharTitlesEntry const* title)
{
    if (!title || title->bit_index >= knownTitles.size() * 32)
        return false;

    uint32 const offset = title->bit_index / 32;
    uint32 const flag = 1u << (title->bit_index % 32);
    if ((knownTitles[offset] & flag) == 0)
        return false;

    knownTitles[offset] &= ~flag;
    return true;
}

std::string KnownTitlesToString(std::array<uint32, 6> const& knownTitles)
{
    std::ostringstream stream;
    for (std::size_t i = 0; i < knownTitles.size(); ++i)
    {
        if (i)
            stream << ' ';

        stream << knownTitles[i];
    }

    return stream.str();
}

bool UpdateOfflineTitle(ObjectGuid::LowType guid, std::string const& knownTitlesText, CharTitlesEntry const* title, bool shouldHaveTitle)
{
    if (!Settings.AuditRepairOfflineCharacters || !title)
        return false;

    std::array<uint32, 6> knownTitles = ParseKnownTitles(knownTitlesText);
    bool changed = shouldHaveTitle ? AddKnownTitle(knownTitles, title) : RemoveKnownTitle(knownTitles, title);
    if (!changed)
        return false;

    std::string updatedTitles = KnownTitlesToString(knownTitles);
    if (shouldHaveTitle && Settings.AutoSetCurrentTitle)
    {
        CharacterDatabase.Execute(
            "UPDATE `characters` SET `knownTitles` = '{}', `chosenTitle` = IF(`chosenTitle` = 0, {}, `chosenTitle`) WHERE `guid` = {} AND `online` = 0",
            updatedTitles, title->bit_index, guid);
    }
    else
    {
        CharacterDatabase.Execute(
            "UPDATE `characters` SET `knownTitles` = '{}', `chosenTitle` = IF(`chosenTitle` = {}, 0, `chosenTitle`) WHERE `guid` = {} AND `online` = 0",
            updatedTitles, title->bit_index, guid);
    }

    return true;
}

struct AuditStats
{
    uint32 Rows = 0;
    uint32 Missing = 0;
    uint32 RepairedOnline = 0;
    uint32 RepairedOffline = 0;
    uint32 MissingUnrepaired = 0;
    uint32 Extra = 0;
    uint32 RemovedOnline = 0;
    uint32 RemovedOffline = 0;
};

bool CharacterHasWatchedTitle(ObjectGuid::LowType guid, std::string const& knownTitlesText, CharTitlesEntry const* title)
{
    if (!title)
        return false;

    if (Player* onlinePlayer = ObjectAccessor::FindPlayerByLowGUID(guid))
        return onlinePlayer->HasTitle(title);

    return HasKnownTitle(ParseKnownTitles(knownTitlesText), title);
}

bool CharacterHasAchievement(ObjectGuid::LowType guid, uint32 achievementId)
{
    return bool(CharacterDatabase.Query(
        "SELECT 1 FROM `character_achievement` WHERE `guid` = {} AND `achievement` = {} LIMIT 1",
        guid, achievementId));
}

AuditStats AuditRealmFirstTitles(ChatHandler* handler, bool repair)
{
    AuditStats stats;
    std::string achievementCsv = BuildActiveAchievementCsv();
    if (achievementCsv.empty())
        return stats;

    std::unordered_map<uint32, ObjectGuid::LowType> canonicalByAchievement;
    if (Settings.RequireCanonicalFirstHolder)
    {
        for (RealmFirstTitleReward const* reward : GetActiveRewards())
            if (std::optional<ObjectGuid::LowType> guid = GetCanonicalRealmFirstHolderGuid(reward->AchievementId))
                canonicalByAchievement.emplace(reward->AchievementId, *guid);
    }

    QueryResult result = CharacterDatabase.Query(
        "SELECT `ca`.`guid`, `c`.`name`, `ca`.`achievement`, `ca`.`date`, COALESCE(`c`.`knownTitles`, ''), `c`.`online` "
        "FROM `character_achievement` `ca` "
        "INNER JOIN `characters` `c` ON `c`.`guid` = `ca`.`guid` "
        "WHERE `ca`.`achievement` IN ({}) "
        "ORDER BY `ca`.`achievement`, `ca`.`date`, `ca`.`guid`",
        achievementCsv);

    if (!result)
        return stats;

    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType const guid = fields[0].Get<uint32>();
        std::string const name = fields[1].Get<std::string>();
        uint32 const achievementId = fields[2].Get<uint32>();
        uint32 const date = fields[3].Get<uint32>();
        std::string const knownTitlesText = fields[4].Get<std::string>();
        bool const online = fields[5].Get<uint8>() != 0;

        ++stats.Rows;

        RealmFirstTitleReward const* reward = GetRewardForAchievement(achievementId);
        if (!reward)
            continue;

        CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleId);
        if (!title)
            continue;

        bool const hasTitle = CharacterHasWatchedTitle(guid, knownTitlesText, title);
        bool const shouldHaveTitle = !Settings.RequireCanonicalFirstHolder || (canonicalByAchievement.contains(achievementId) && canonicalByAchievement[achievementId] == guid);

        if (hasTitle && !shouldHaveTitle)
        {
            ++stats.Extra;
            bool removed = false;

            if (repair)
            {
                if (Player* onlinePlayer = ObjectAccessor::FindPlayerByLowGUID(guid))
                {
                    if (RemoveRealmFirstTitle(onlinePlayer, *reward))
                    {
                        ++stats.RemovedOnline;
                        removed = true;
                    }
                }
                else if (!online)
                {
                    if (UpdateOfflineTitle(guid, knownTitlesText, title, false))
                    {
                        ++stats.RemovedOffline;
                        removed = true;
                    }
                }
            }

            if (handler && stats.Extra <= Settings.AuditOutputLimit)
            {
                handler->PSendSysMessage(
                    "{}: achievement {}, title {} {}. Date {}.",
                    name, achievementId, reward->TitleId,
                    removed ? "removed" : "extra",
                    date);
            }
            continue;
        }

        if (hasTitle || !shouldHaveTitle)
            continue;

        ++stats.Missing;
        bool repaired = false;

        if (repair)
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayerByLowGUID(guid))
            {
                if (EnsureRealmFirstTitle(onlinePlayer, *reward, false))
                {
                    ++stats.RepairedOnline;
                    repaired = true;
                }
            }
            else if (!online)
            {
                if (UpdateOfflineTitle(guid, knownTitlesText, title, true))
                {
                    ++stats.RepairedOffline;
                    repaired = true;
                }
            }
        }

        if (!repaired)
            ++stats.MissingUnrepaired;

        if (handler && stats.Missing <= Settings.AuditOutputLimit)
        {
            handler->PSendSysMessage(
                "{}: achievement {}, title {} {}. Date {}.",
                name, achievementId, reward->TitleId,
                repaired ? "repaired" : "missing",
                date);
        }
    } while (result->NextRow());

    return stats;
}

void ListRealmFirstAchievementHolders(ChatHandler* handler)
{
    if (!handler)
        return;

    std::string achievementCsv = BuildActiveAchievementCsv();
    if (achievementCsv.empty())
    {
        handler->PSendSysMessage("No active Realm First rewards.");
        return;
    }

    QueryResult result = CharacterDatabase.Query(
        "SELECT `ca`.`guid`, `c`.`name`, `ca`.`achievement`, `ca`.`date`, COALESCE(`c`.`knownTitles`, '') "
        "FROM `character_achievement` `ca` "
        "INNER JOIN `characters` `c` ON `c`.`guid` = `ca`.`guid` "
        "WHERE `ca`.`achievement` IN ({}) "
        "ORDER BY `ca`.`achievement`, `ca`.`date`, `ca`.`guid`",
        achievementCsv);

    if (!result)
    {
        handler->PSendSysMessage("No characters have watched Realm First achievements.");
        return;
    }

    uint32 rows = 0;
    uint32 printed = 0;
    uint32 missingTitles = 0;

    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType const guid = fields[0].Get<uint32>();
        std::string const name = fields[1].Get<std::string>();
        uint32 const achievementId = fields[2].Get<uint32>();
        uint32 const date = fields[3].Get<uint32>();
        std::string const knownTitlesText = fields[4].Get<std::string>();

        ++rows;

        RealmFirstTitleReward const* reward = GetRewardForAchievement(achievementId);
        if (!reward)
            continue;

        CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleId);
        bool const hasTitle = CharacterHasWatchedTitle(guid, knownTitlesText, title);
        if (!hasTitle)
            ++missingTitles;

        if (printed < Settings.ReportOutputLimit)
        {
            handler->PSendSysMessage(
                "{}: achievement {} -> title {} ({}) [{}]. Date {}.",
                name, achievementId, reward->TitleId, reward->Label,
                hasTitle ? "has title" : "missing title",
                date);
            ++printed;
        }
    } while (result->NextRow());

    handler->PSendSysMessage(
        "Realm First achievement summary: {} rows, {} shown, {} missing title.",
        rows, printed, missingTitles);
}

void ListRealmFirstTitleHolders(ChatHandler* handler)
{
    if (!handler)
        return;

    QueryResult result = CharacterDatabase.Query(
        "SELECT `guid`, `name`, COALESCE(`knownTitles`, '') FROM `characters` ORDER BY `name` ASC");

    if (!result)
    {
        handler->PSendSysMessage("No characters to inspect.");
        return;
    }

    uint32 holders = 0;
    uint32 printed = 0;
    uint32 withoutAchievement = 0;

    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType const guid = fields[0].Get<uint32>();
        std::string const name = fields[1].Get<std::string>();
        std::string const knownTitlesText = fields[2].Get<std::string>();

        for (RealmFirstTitleReward const* reward : GetActiveRewards())
        {
            CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleId);
            if (!CharacterHasWatchedTitle(guid, knownTitlesText, title))
                continue;

            ++holders;
            bool const hasAchievement = CharacterHasAchievement(guid, reward->AchievementId);
            if (!hasAchievement)
                ++withoutAchievement;

            if (printed < Settings.ReportOutputLimit)
            {
                handler->PSendSysMessage(
                    "{}: title {} ({}) -> achievement {} [{}].",
                    name, reward->TitleId, reward->Label, reward->AchievementId,
                    hasAchievement ? "has achievement" : "missing achievement");
                ++printed;
            }
        }
    } while (result->NextRow());

    handler->PSendSysMessage(
        "Realm First title summary: {} titles found, {} shown, {} without watched achievement.",
        holders, printed, withoutAchievement);
}

void LoadRealmFirstTitleConfig()
{
    Settings.Enabled = sConfigMgr->GetOption<bool>("RealmFirstTitles.Enabled", true);
    Settings.GrantBlizzlikeRaidTitles = sConfigMgr->GetOption<bool>("RealmFirstTitles.GrantBlizzlikeRaidTitles", true);
    Settings.GrantRestoredBetaTitles = sConfigMgr->GetOption<bool>("RealmFirstTitles.GrantRestoredBetaTitles", true);
    Settings.GrantLightOfDawnFallback = sConfigMgr->GetOption<bool>("RealmFirstTitles.GrantLightOfDawnFallback", false);
    Settings.RequireCanonicalFirstHolder = sConfigMgr->GetOption<bool>("RealmFirstTitles.RequireCanonicalFirstHolder", false);
    Settings.SyncOnLogin = sConfigMgr->GetOption<bool>("RealmFirstTitles.SyncOnLogin", true);
    Settings.RepairOnStartup = sConfigMgr->GetOption<bool>("RealmFirstTitles.RepairOnStartup", true);
    Settings.AnnounceGrantedTitles = sConfigMgr->GetOption<bool>("RealmFirstTitles.AnnounceGrantedTitles", true);
    Settings.AutoSetCurrentTitle = sConfigMgr->GetOption<bool>("RealmFirstTitles.AutoSetCurrentTitle", true);
    Settings.AuditRepairOfflineCharacters = sConfigMgr->GetOption<bool>("RealmFirstTitles.AuditRepairOfflineCharacters", true);
    Settings.PreventGMAchievements = sConfigMgr->GetOption<bool>("RealmFirstTitles.PreventGMAchievements", true);
    Settings.AuditOutputLimit = sConfigMgr->GetOption<uint32>("RealmFirstTitles.AuditOutputLimit", 25);
    Settings.ReportOutputLimit = sConfigMgr->GetOption<uint32>("RealmFirstTitles.ReportOutputLimit", 50);

    LOG_INFO("module.realmfirsttitles", "RealmFirstTitles: {}. BlizzlikeRaid={}, RestoredBeta={}, LightOfDawnFallback={}, CanonicalOnly={}, LoginSync={}, StartupRepair={}, OfflineRepair={}, PreventGMs={}.",
        Settings.Enabled ? "enabled" : "disabled",
        Settings.GrantBlizzlikeRaidTitles ? "on" : "off",
        Settings.GrantRestoredBetaTitles ? "on" : "off",
        Settings.GrantLightOfDawnFallback ? "on" : "off",
        Settings.RequireCanonicalFirstHolder ? "on" : "off",
        Settings.SyncOnLogin ? "on" : "off",
        Settings.RepairOnStartup ? "on" : "off",
        Settings.AuditRepairOfflineCharacters ? "on" : "off",
        Settings.PreventGMAchievements ? "on" : "off");
}
}

class RealmFirstTitlesWorldScript : public WorldScript
{
public:
    RealmFirstTitlesWorldScript() : WorldScript("RealmFirstTitlesWorldScript", {
        WORLDHOOK_ON_AFTER_CONFIG_LOAD,
        WORLDHOOK_ON_STARTUP
    }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        LoadRealmFirstTitleConfig();
    }

    void OnStartup() override
    {
        LoadRealmFirstTitleConfig();
        ValidateRealmFirstTitleRewards();

        if (!Settings.Enabled || !Settings.RepairOnStartup)
            return;

        AuditStats stats = AuditRealmFirstTitles(nullptr, true);
        LOG_INFO("module.realmfirsttitles",
            "RealmFirstTitles: startup repair scanned {} watched achievement row(s), missing={}, online repaired={}, offline repaired={}, pending={}.",
            stats.Rows, stats.Missing, stats.RepairedOnline, stats.RepairedOffline, stats.MissingUnrepaired);
    }
};

class RealmFirstTitlesPlayerScript : public PlayerScript
{
public:
    RealmFirstTitlesPlayerScript() : PlayerScript("RealmFirstTitlesPlayerScript", {
        PLAYERHOOK_ON_LOGIN,
        PLAYERHOOK_ON_ACHI_COMPLETE,
        PLAYERHOOK_ON_BEFORE_ACHI_COMPLETE,
        PLAYERHOOK_ON_BEFORE_CRITERIA_PROGRESS
    }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (Settings.SyncOnLogin)
            SyncRealmFirstTitles(player, false);
    }

    bool OnPlayerBeforeAchievementComplete(Player* player, AchievementEntry const* achievement) override
    {
        if (!player || !achievement || !Settings.Enabled || !Settings.PreventGMAchievements)
            return true;

        if (IsGMPlayer(player) && IsRealmFirstAchievement(achievement))
        {
            LOG_INFO("module.realmfirsttitles", "Prevented GM character {} from completing Realm First achievement {} ({}).",
                player->GetName(), achievement->ID, achievement->name[LOCALE_enUS]);
            return false;
        }

        return true;
    }

    bool OnPlayerBeforeCriteriaProgress(Player* player, AchievementCriteriaEntry const* criteria) override
    {
        if (!player || !criteria || !Settings.Enabled || !Settings.PreventGMAchievements)
            return true;

        if (!IsGMPlayer(player))
            return true;

        AchievementEntry const* achievement = sAchievementStore.LookupEntry(criteria->referredAchievement);
        if (achievement && IsRealmFirstAchievement(achievement))
            return false;

        return true;
    }

    void OnPlayerAchievementComplete(Player* player, AchievementEntry const* achievement) override
    {
        if (!achievement)
            return;

        if (RealmFirstTitleReward const* reward = GetRewardForAchievement(achievement->ID))
            EnsureRealmFirstTitle(player, *reward, true);
    }
};

class RealmFirstTitlesCommandScript : public CommandScript
{
public:
    RealmFirstTitlesCommandScript() : CommandScript("RealmFirstTitlesCommandScript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable realmFirstTable =
        {
            { "check",        HandleCheckCommand,        SEC_GAMEMASTER, Console::No },
            { "refresh",      HandleRefreshCommand,      SEC_GAMEMASTER, Console::Yes },
            { "sync",         HandleRefreshCommand,      SEC_GAMEMASTER, Console::Yes },
            { "grant",        HandleGrantCommand,        SEC_GAMEMASTER, Console::No },
            { "audit",        HandleAuditCommand,        SEC_GAMEMASTER, Console::Yes },
            { "missing",      HandleMissingCommand,      SEC_GAMEMASTER, Console::Yes },
            { "achievements", HandleAchievementsCommand, SEC_GAMEMASTER, Console::Yes },
            { "winners",      HandleAchievementsCommand, SEC_GAMEMASTER, Console::Yes },
            { "titles",       HandleTitlesCommand,       SEC_GAMEMASTER, Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "realmfirst", realmFirstTable },
        };

        return commandTable;
    }

    static bool HandleCheckCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player || !player->IsConnected())
        {
            handler->PSendSysMessage("Player not found.");
            return false;
        }

        SendPlayerStatus(handler, player->GetConnectedPlayer());
        return true;
    }

    static bool HandleRefreshCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player || !player->IsConnected())
        {
            handler->PSendSysMessage("Player not found.");
            return false;
        }

        Player* target = player->GetConnectedPlayer();
        uint32 granted = SyncRealmFirstTitles(target, true);
        SendPlayerStatus(handler, target);

        handler->PSendSysMessage("Sync complete for {}. Titles granted: {}.", target->GetName(), granted);
        return true;
    }

    static bool HandleGrantCommand(ChatHandler* handler, uint32 achievementId, Optional<PlayerIdentifier> player)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!player || !player->IsConnected())
        {
            handler->PSendSysMessage("Player not found.");
            return false;
        }

        Player* target = player->GetConnectedPlayer();
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementId);
        if (!achievement)
        {
            handler->PSendSysMessage("Achievement ID {} not found.", achievementId);
            return false;
        }

        RealmFirstTitleReward const* reward = GetRewardForAchievement(achievementId);
        if (!reward)
        {
            handler->PSendSysMessage("Achievement ID {} has no configured realm first title reward.", achievementId);
            return false;
        }

        if (!target->HasAchieved(achievementId))
            target->CompletedAchievement(achievement);

        CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(reward->TitleId);
        if (title)
        {
            target->SetTitle(title);
            if (Settings.AutoSetCurrentTitle)
                target->SetCurrentTitle(title);

            handler->PSendSysMessage("Granted achievement {} ({}) and title {} ({}) to {}.",
                achievementId, achievement->name[LOCALE_enUS], reward->TitleId, reward->Label, target->GetName());
        }

        return true;
    }

    static bool HandleAuditCommand(ChatHandler* handler)
    {
        AuditStats stats = AuditRealmFirstTitles(handler, true);

        handler->PSendSysMessage(
            "RealmFirstTitles audit: rows {}, missing {}, extra {}, online repaired {}, offline repaired {}, online removed {}, offline removed {}, pending {}.",
            stats.Rows, stats.Missing, stats.Extra, stats.RepairedOnline, stats.RepairedOffline, stats.RemovedOnline, stats.RemovedOffline, stats.MissingUnrepaired);

        return true;
    }

    static bool HandleMissingCommand(ChatHandler* handler)
    {
        AuditStats stats = AuditRealmFirstTitles(handler, false);

        handler->PSendSysMessage(
            "RealmFirstTitles missing: rows {}, missing titles {}.",
            stats.Rows, stats.Missing);

        return true;
    }

    static bool HandleAchievementsCommand(ChatHandler* handler)
    {
        ListRealmFirstAchievementHolders(handler);
        return true;
    }

    static bool HandleTitlesCommand(ChatHandler* handler)
    {
        ListRealmFirstTitleHolders(handler);
        return true;
    }
};

void Addmod_realm_first_titlesScripts()
{
    new RealmFirstTitlesWorldScript();
    new RealmFirstTitlesPlayerScript();
    new RealmFirstTitlesCommandScript();
}
