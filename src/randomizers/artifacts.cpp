#include "artifacts.hpp"

#include <algorithm>
#include <cstring>
#include <string>
#include <unordered_map>

// Artifacts that can be freely shuffled
std::vector<std::string> valid_artifacts = {
    // Morrowind
    "mace of molag bal_unique",
    "daedric_scourge_unique",
    "staff_hasedoki_unique",
    "dwarven_hammer_volendrung",
    "longbow_shadows_unique",
    "katana_bluebrand_unique",
    "katana_goldbrand_unique",
    "daedric_crescent_unique",
    "longsword_umbra_unique",
    "dagger_fang_unique",
    "spear_mercy_unique",
    "tenpaceboots",
    "cuirass_savior_unique",
    "dragonbone_cuirass_unique",
    "gauntlet_fists_l_unique",
    "gauntlet_fists_r_unique",
    "helm_bearclaw_unique",
    "daedric_helm_clavicusvile",
    "ebony_shield_auriel",
    "towershield_eleidon_unique",
    "artifact_amulet of heartfire",
    "artifact_amulet of heartheal",
    "artifact_amulet of heartrime",
    "artifact_amulet of heartthrum",
    "amulet_usheeja",
    "belt of heartfire",
    "blood ring",
    "ring_denstagmer_unique",
    "heart ring",
    "ring_mentor_unique",
    "ring_khajiit_unique",
    "ring_phynaster_unique",
    "ring_surrounding_unique",
    "ring_wind_unique",
    "soul ring",
    "ring_vampiric_unique",
    "Misc_SoulGem_Azura",
    "skeleton_key",
    // Tribunal
    "Mace of Slurring",
    "stendar_hammer_unique",
    "Bipolar Blade",
    "robe_lich_unique",
    // Bloodmoon
    "M_Mace_Aevar_UNI",
    "BM_hunterspear_unique",
    "BM_ring_hircine",
    // Technically not artifacts in game, but in lore
    "helm_tohan_unique",
    "bloodworm_helm_unique",
    "boots of blinding speed[unique]",
    "necromancers_amulet_uniq",
    "daedric warhammer_ttgd",
    "Ward of Akavir",
};

// Artifacts *required* for quests (not rewards that are not needed later on)
std::vector<std::string> quest_artifacts = {
    "cleaverstfelms",
    "warhammer_crusher_unique",
    "warhammer_crusher_unique_x",
    "crosierstllothis",
    "staff_magnus_unique",
    "staff_magnus_unique_x",
    "ebony_bow_auriel",
    "ebony_bow_auriel_X",
    "claymore_chrysamere_unique",
    "claymore_chrysamere_unique_x",
    "claymore_iceblade_unique",
    "claymore_iceblade_unique_x",
    "fork_horripilation_unique",
    "mehrunes'_razor_unique",
    "boots_apostle_unique",
    "ebon_plate_cuirass_unique",
    "lords_cuirass_unique",
    "spell_breaker_unique",
    "ring_warlock_unique",
    "expensive_shirt_hair",
    "shoes of st. rilms",
    "artifact_bittercup_01",
};

// Artifacts required for the main quests
std::vector<std::string> mq_artifacts = {
    "sunder",
    "keening",
    "wraithguard",
    "wraithguard_jury_rig",
    "madstone",
    "teeth",
    "thong",
    "seizing",
    "moon_and_star",
    "Sword of Almalexia",
    "bladepiece_01",
    "nerevarblade_01",
    "nerevarblade_01_flame",
};

// List of propylon indices in case they should get shuffled
std::vector<std::string> propylon_indices = {
    "index_andra", "index_beran", "index_falas", "index_falen", "index_hlor",
    "index_indo",  "index_maran", "index_roth",  "index_telas", "index_valen",
};

// ID of the Master Propylon Index
const std::string master_propylon_index = "index_master";

// Uniques that can be freely shuffled
std::vector<std::string> valid_uniques = {
    // Morrowind
    "conoon_chodala_boots_unique",
    "daedric_cuirass_htab",
    "dreugh_cuirass_ttrm",
    "erur_dan_cuirass_unique",
    "Mountain Spirit",
    "Gauntlet_of_Glory_left",
    "gauntlet_of_glory_right",
    "gauntlet_horny_fist_l",
    "daedric_greaves_htab",
    "cephalopod_helm_HTNK",
    "imperial_helm_frald_uniq",
    "ebony_closed_helm_fghl",
    "icecap_unique",
    "darksun_shield_unique",
    "shield of the undaunted",
    "conoon_chodala_axe_unique",
    "Karpal's Friend",
    "Wind of Ahaz",
    "dwarven axe_soultrap",
    "Stormkiss",
    "ane_teria_mace_unique",
    "lightofday_unique",
    "steelstaffancestors_ttsa",
    "banhammer_unique",
    "we_hellfirestaff",
    "ebony_staff_tges",
    "silver staff of hunger",
    "ebony_staff_trebonius",
    "Fury",
    "glass claymore_magebane",
    "claymore_Agustas",
    "we_temreki",
    "shortbow of sanguine sureflight",
    "daedric dagger_mtas",
    "Dagger of Judgement",
    "dwe_jinksword_curse_Unique",
    "racerbeak",
    "we_shimsil",
    "daedric dagger_soultrap",
    "Airan_Ahhe's_Spirit_Spear_uniq",
    "dwarven halberd_soultrap",
    "ebony spear_hrce_unique",
    "erur_dan_spear_unique",
    "Greed",
    "we_illkurok",
    "we_stormforge",
    "amulet of admonition",
    "amulet of domination",
    "amulet_gem_feeding",
    "amulet of igniis",
    "amulet of levitating",
    "amulet of shadows",
    "amulet_unity_uniq",
    "Exquisite_Amulet_Arobar1",
    "amulet_aundae",
    "amulet_berne",
    "amulet_quarra",
    "Daedric_special",
    "Maran Amulet",
    "expensive_amulet_methas",
    "necromancers_amulet_uniq",
    "amulet_Pop00",
    "zenithar_whispers",
    "Belt of Northern Knuck Knuck",
    "belt of the armor of god",
    "peakstar_belt_unique",
    "Stendarran Belt",
    "aryongloveleft",
    "aryongloveright",
    "bitter_hand",
    "ember hand",
    "common_glove_l_moragtong",
    "common_glove_r_moragtong",
    "Left_Hand_of_Zenithar",
    "Right_Hand_of_Zenithar",
    "Zenithar's_Warning",
    "Zenithar's_Wiles",
    "Caius_pants",
    "tailored_trousers",
    "peakstar_pants_unique",
    "Akatosh Ring",
    "Caius_ring",
    "ring_fathasa_unique",
    "foe-grinder",
    "foe-quern",
    "ring_marara_unique",
    "Nuccius_ring",
    "ondusi's key",
    "othril_ring",
    "ring of azura",
    "ring_equity_uniq",
    "ring of telekinesis_UNIQUE",
    "cl_ringofregeneration",
    "Septim Ring",
    "ring_shashev_unique",
    "Adusamsi's_robe",
    "hort_ledd_robe_unique",
    "exquisite_shirt_01_wedding",
    "caius_shirt",
    "Maras_Blouse",
    "Restoration_Shirt",
    "Zenithar_Frock",
    "Shoes_of_Conviction",
    "common_shoes_02_surefeet",
    "Maras_Skirt",
    // Tribunal
    "Gravedigger",
    "King's_Oath_pc",
    "daedric dagger_bar",
    "glass dagger_symmachus_unique",
    "silver dagger_iryon_unique",
    "silver dagger_othril_unique",
    "silver dagger_rathalas_unique",
    "ebony spear_blessed_unique",
    "goblin_shield_durgok_uni",
    "amulet of verbosity",
    "amulet_gaenor",
    "Helseth's Collar",
    "belt_Goval",
    "Helseth's Ring",
    "Variner's Ring",
    "common_robe_EOT",
    "Helseth's Robe",
    // Bloodmoon
    "BM nordic silver axe_spurius",
    "BM_axe_Heartfang_Unique",
    "BM Nord Leg",
    "Lucky_Break",
    "warhammer_rammekald_unique",
    "bm_ebony_staff_necro",
    "BM_nordic_silver_lgswd_bloodska",
    "BM nordic silver longsword_cft",
    "bm_saber_seasplitter",
    "bm_ebonylongsword_s",
    "solvistapp",
    "BM ice longsword_FG_Unique",
    "nordic claymore_stormfang",
    "BM frostgore",
    "steel spear snow prince",
    "wolfwalkers",
    "BM Bear Helmet_ber",
    "BM Bear Helmet eddard",
    "BM Wolf Helmet_heartfang",
    "bm_amulspd1",
    "bm_amulstr1",
    "bm_black_glove_l_s",
    "bm_black_glove_r_s",
    "BM_ring_Aesliip",
    "bm_ring_marksman",
    "bm_ring_view",
    "BM_Nordic01_Robe_whitewalk",
    "mantle of woe",
    "light_com_lantern_bm_unique",
};

// Uniques *required* for quests (not rewards that are not needed later on)
std::vector<std::string> quest_uniques = {
    "cloudcleaver_unique",       "widowmaker_unique",        "silver_staff_dawn_uniq", "amulet of ashamanu (unique)",
    "amuletfleshmadewhole_uniq", "black_blindfold_glove",    "Adusamsi's_Ring",        "Detect_Enchantment_ring",
    "ring_dahrkmezalf_uniq",     "Sheogorath's Signet Ring",
};

// Uniques required for the main quests
std::vector<std::string> mq_uniques = {
    "shadow_shield",    "war_axe_airan_ammu", "Wind of Ahaz", "bonebiter_bow_unique", "heart_of_fire",
    "robe_of_erur_dan", "hortatorbelt",       "hortatorring", "hortatorrobe",         "malipu_ataman's_belt",
    // "mazed_band_end", -- This will NEVER get shuffled since not having it at some point might softlock the player
};


struct TopicReplacer
{
    std::string id;              // Topic-ID
    std::string index;           // Actor
    std::string text_to_replace; // text that should get replace with the item's name
};

/**
 * Maps an item to its journal quests and stages to replace text
 * map<journal_id, map<quest_id, map<quest_stage, text_to_replace[]>>>
 */
auto journal_replacementsmap =
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<int, std::vector<std::string>>>>{
        { "mace of molag bal_unique", { { "DA_MolagBal", { { 30, { "his mace, the Mace of Molag Bal" } } } } } },
        { "katana_goldbrand_unique", { { "DA_Boethiah", { { 20, { "Goldbrand" } }, { 70, { "Goldbrand" } } } } } },
        { "spear_mercy_unique",
          { { "DA_Sheogorath",
              {
                  { 70, { "Spear of Bitter Mercy" } },
              } } } },
        { "helm_bearclaw_unique",
          { { "DA_Malacath",
              {
                  { 10, { "a helm of great power" } },
                  { 70, { "Helm of Oreyn Bearclaw" } },
              } } } },
        { "ring_khajiit_unique",
          { { "DA_Mephala",
              {
                  { 60, { "Ring of Khajiit" } },
              } } } },
        { "ring_wind_unique",
          { { "IC25_JonHawker",
              {
                  { 1, { "Ring of the Wind" } },
                  { 50, { "Ring of the Wind" } },
              } } } },
        { "skeleton_key",
          { { "TG_KillHardHeart",
              {
                  { 100, { "Skeleton Key" } },
              } } } },
        { "M_Mace_Aevar_UNI",
          { { "BM_Stones",
              {
                  { 100, { "Mace of Aevar Stonesinger" } },
              } } } },
        { "boots of blinding speed[unique]",
          { { "MV_TraderAbandoned",
              {
                  { 0, { "Boots of Blinding Speed" } },
                  { 30, { "Boots of Blinding Speed" } },
                  { 100, { "Boots of Blinding Speed" } },
              } } } },
        { "cleaverstfelms",
          { { "TT_FelmsCleaver",
              {
                  { 0, { "Cleaver of St. Felms" } },
                  { 10, { "Cleaver of St. Felms" } },
                  { 100, { "Cleaver of St. Felms the Bold" } },
                  { 110, { "Cleaver of St. Felms the Bold" } },
              } } } },
        { "warhammer_crusher_unique",
          { { "IC29_Crusher",
              {
                  { 0, { "Skull-Crusher" } },
                  { 1, { "ancient warhammer Skull-Crusher" } },
                  { 1, { "Skull-Crusher" } },
                  { 15, { "Skull-Crusher" } },
                  { 50, { "Skull-Crusher" } },
              } } } },
        { "staff_magnus_unique",
          { { "MG_StaffMagnus",
              {
                  { 0, { "Staff of Magnus" } },
                  { 10, { "Staff of Magnus" } },
                  { 100, { "Staff of Magnus" } },
              } } } },
        { "ebony_bow_auriel",
          { { "HT_AurielBow",
              {
                  { 0, { "Auriel's Bow" } },
                  { 10, { "Auriel's Bow" } },
                  { 100, { "Auriel's Bow" } },
              } } } },
        { "claymore_chrysamere_unique",
          { { "IL_KnightShield",
              {
                  { 60, { "Paladin's Blade, Chrysamere" } },
                  { 100, { "Chrysamere" } },
              } } } },
        { "lords_cuirass_unique",
          { { "IL_KnightShield",
              {
                  { 0, { "Lord's Mail" } },
                  { 10, { "Lord's Mail" } },
                  { 30, { "Lord's Mail" } },
                  { 50, { "Lord's Mail" } },
              } } } },
        { "claymore_iceblade_unique",
          { { "IC27_Oracle",
              {
                  { 0, { "Ice Blade of the Monarch" } },
                  { 1, { "Ice Blade of the Monarch" } },
                  { 50, { "Ice Blade of the Monarch" } },
                  { 55, { "Ice Blade of the Monarch" } },
              } },
            { "IC27_Oracle_A",
              {
                  { 0, { "Ice Blade of the Monarch" } },
                  { 40, { "Ice Blade of the Monarch" } },
                  { 45, { "Ice Blade of the Monarch" } },
              } } } },
        { "fork_horripilation_unique",
          { { "DA_Sheogorath",
              {
                  { 10, { "Fork of Horripilation" } },
                  { 30, { "Fork of Horripilation" } },
                  { 50, { "Fork of Horripilation" } },
                  { 55, { "Fork of Horripilation" } },
                  { 60, { "Fork of Horripilation" } },
              } } } },
        { "mehrunes'_razor_unique",
          { { "DA_Mehrunes",
              {
                  { 20, { "Mehrunes' Razor" } },
                  { 20, { "Razor" } },
                  { 30, { "Mehrunes' Razor" } },
                  { 40, { "dagger" } },
                  { 40, { "Razor" } },
              } } } },
        { "boots_apostle_unique",
          { { "IC26_AmaNin_free",
              {
                  { 0, { "Boots of the Apostle" } },
              } },
            { "IC26_AmaNin",
              {
                  { 0, { "Boots of the Apostle" } },
                  { 1, { "Boots of the Apostle" } },
                  { 50, { "Boots of the Apostle", "boots" } },
                  { 60, { "Boots of the Apostle", "boots" } },
              } } } },
        { "ebon_plate_cuirass_unique",
          { { "TT_Assarnibibi",
              {
                  { 0, { "Ebony Mail" } },
                  { 10, { "Ebony Mail" } },
                  { 100, { "Ebony Mail" } },
                  { 110, { "Ebony Mail" } },
              } } } },
        { "spell_breaker_unique",
          { { "VA_VampCurse",
              {
                  { 40, { "Spell Breaker" } },
                  { 55, { "Spell Breaker" } },
                  { 60, { "Spell Breaker" } },
              } } } },
        { "ring_warlock_unique",
          { { "MG_WarlocksRing",
              {
                  { 0, { "Warlock's Ring" } },
                  { 10, { "Warlock's Ring" } },
                  { 100, { "Warlock's Ring" } },
              } } } },
        { "expensive_shirt_hair",
          { { "TT_HairShirt",
              {
                  { 0, { "Hair Shirt of St. Aralor" } },
                  { 10, { "Hair Shirt of St. Aralor" } },
                  { 100, { "Hair Shirt of St. Aralor" } },
                  { 110, { "Hair Shirt of St. Aralor the Penitent" } },
              } } } },
        { "shoes of st. rilms",
          { { "TT_RilmsShoes",
              {
                  { 0, { "Shoes of St. Rilms" } },
                  { 10, { "Shoes of St. Rilms" } },
                  { 10, { "shoes" } },
                  { 100, { "Shoes of St. Rilms" } },
                  { 110, { "Shoes of St. Rilms" } },
              } } } },
        { "artifact_bittercup_01",
          { { "TG_BitterBribe",
              {
                  { 0, { "Bitter Cup" } },
                  { 10, { "Bitter Cup" } },
                  { 25, { "Bitter Cup" } },
                  { 30, { "Bitter Cup" } },
                  { 50, { "Bitter Cup" } },
                  { 100, { "Bitter Cup" } },
              } } } },
        // continued: mq_artifacts...
    };


/*
    Record types for artifacts and uniques:
    - WEAP
    - ARMO
    - CLOT
    - LOCK
    For editing dialogues:
    - DIAL
    - INFO

    DIAL and INFO are not yet correctly implemented. There's one DIAL and then following all INFOs that belong to it.
    But this connection is currently lost when reading and exporting the file.
*/

bool is_in(std::vector<std::string> &v, std::string item)
{
    return std::find(v.begin(), v.end(), item) != v.end();
}

std::vector<Record *> Artifacts::Randomize(std::vector<std::vector<Record *>> artifact_record_groups,
                                           std::vector<Record *> dialog_journal_recs, std::vector<Record *> info_recs,
                                           Settings &settings)
{
    // Records
    std::vector<Record *> result;
    std::vector<Record *> artifacts;
    std::vector<Record *> uniques;
    // Things we're looking for depending on the settings
    std::vector<std::string> to_shuffle_artifacts_base;
    std::vector<std::string> to_shuffle_uniques_base;
    // Found record names in the same order as put into the artifacts/uniques vectors
    std::vector<std::string> shuffle_artifacts;
    std::vector<std::string> shuffle_uniques;
    // Maps the old item name to the new one so we can tell how dialogue needs to be modified later
    auto old_to_new_artifacts = std::vector<std::pair<std::string, std::string>>();
    auto old_to_new_uniques   = std::vector<std::pair<std::string, std::string>>();
    std::unordered_map<std::string, std::string> id_to_readable_name;

    if (settings.Artifacts == ShuffleType::None && settings.Uniques == ShuffleType::None)
        return artifacts;

    if (settings.Artifacts != ShuffleType::None)
        to_shuffle_artifacts_base.insert(to_shuffle_artifacts_base.end(), valid_artifacts.begin(),
                                         valid_artifacts.end());
    if (settings.Uniques != ShuffleType::None)
        to_shuffle_uniques_base.insert(to_shuffle_uniques_base.end(), valid_uniques.begin(), valid_uniques.end());
    if (settings.ShuffleQuestRequirementArtifactsOrUniques)
    {
        to_shuffle_artifacts_base.insert(to_shuffle_artifacts_base.end(), quest_artifacts.begin(),
                                         quest_artifacts.end());
        to_shuffle_uniques_base.insert(to_shuffle_uniques_base.end(), quest_uniques.begin(), quest_uniques.end());
    }
    if (settings.ShuffleMAINQuestRequirementArtifactsOrUniques)
    {
        to_shuffle_artifacts_base.insert(to_shuffle_artifacts_base.end(), mq_artifacts.begin(), mq_artifacts.end());
        to_shuffle_uniques_base.insert(to_shuffle_uniques_base.end(), mq_uniques.begin(), mq_uniques.end());
    }
    if (settings.ShufflePropylonIndices)
        to_shuffle_uniques_base.insert(to_shuffle_uniques_base.end(), propylon_indices.begin(), propylon_indices.end());
    if (settings.ShuffleMasterIndex)
        to_shuffle_uniques_base.push_back(master_propylon_index);

    if (settings.ConsiderUniquesEqualToArtifacts)
        to_shuffle_artifacts_base.insert(to_shuffle_artifacts_base.end(), to_shuffle_uniques_base.begin(),
                                         to_shuffle_uniques_base.end());

    // Step 1: Go over all records and find the artifacts!
    for (std::vector<Record *> records : artifact_record_groups)
    {
        for (Record *r : records)
        {
            auto srs = r->GetSubrecords("NAME");
            if (srs.size() == 0)
                continue;

            auto readable_names = r->GetSubrecords("FNAM");
            if (readable_names.size() == 0)
                continue;
            std::string readable_name = std::string((char *)readable_names[0]->Data->data());

            std::string name = std::string((char *)srs[0]->Data->data());
            if (is_in(to_shuffle_artifacts_base, name))
            {
                artifacts.push_back(r);
                shuffle_artifacts.push_back(name);
                old_to_new_artifacts.push_back(std::pair(name, ""));
                id_to_readable_name[name] = readable_name;
            }
            else if (is_in(to_shuffle_uniques_base, name))
            {
                uniques.push_back(r);
                shuffle_uniques.push_back(name);
                old_to_new_uniques.push_back(std::pair(name, ""));
                id_to_readable_name[name] = readable_name;
            }
        }
    }

    auto rng = [&settings](int i) { return settings.GetNext(i); };

    // Step 2: Shuffle the items
    std::random_shuffle(shuffle_artifacts.begin(), shuffle_artifacts.end(), rng);
    std::random_shuffle(shuffle_uniques.begin(), shuffle_uniques.end(), rng);

    // Step 3: Reassign artifacts/uniques

    auto assigner = [&result](std::vector<Record *> &artuniq, std::vector<std::string> &names,
                              std::vector<std::pair<std::string, std::string>> &old_to_new) {
        for (size_t i = 0; i < artuniq.size(); ++i)
        {
            Record *r = artuniq[i];
            auto srs  = r->GetSubrecords("NAME");
            if (srs.size() == 0)
                continue;

            std::string newname  = names[i];
            old_to_new[i].second = newname;
            auto newdata         = std::vector<uint8_t>(newname.size() + 1);
            for (size_t i = 0; i < newname.size(); ++i)
                newdata[i] = newname[i];
            newdata[newdata.size() - 1] = 0;
            *srs[0]->Data               = newdata;

            result.push_back(r);
        }
    };

    assigner(artifacts, shuffle_artifacts, old_to_new_artifacts);
    if (!settings.ConsiderUniquesEqualToArtifacts)
        assigner(uniques, shuffle_uniques,
                 old_to_new_uniques); // If they are considered the same then they'd already be put into the
                                      // artifact data structures, making this step unnecessary

    // Step 4: Fix dialogue/scripts using those artifacts
    // if (settings.Artifacts != ShuffleType::None)
    // {
    //     for (std::pair<std::string, std::string> old_to_new : old_to_new_artifacts)
    //     {
    //         std::string &old_artifact_id = old_to_new.first;
    //         std::string &new_artifact_readable_name = old_to_new.second;

    //         // TODO: Shouldn't be necessary in the final version
    //         if (!journal_replacementsmap.contains(old_artifact_id))
    //             continue;

    //         for(Record *jrec : dialog_journal_recs)
    //         {
    //         }
    //     }
    // }
    // TODO: same with uniques

    return result;
}
