#include "artifacts.hpp"

#include <algorithm>
#include <cstring>
#include <string>

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

std::vector<Record *> Artifacts::Randomize(std::vector<std::vector<Record *>> record_groups, Settings &settings)
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
    for (std::vector<Record *> records : record_groups)
    {
        for (Record *r : records)
        {
            auto srs = r->GetSubrecords("NAME");
            if (srs.size() == 0)
                continue;

            auto name = std::string((char *)srs[0]->GetData().get());
            if (is_in(to_shuffle_artifacts_base, name))
            {
                artifacts.push_back(r);
                shuffle_artifacts.push_back(name);
            }
            else if (is_in(to_shuffle_uniques_base, name))
            {
                uniques.push_back(r);
                shuffle_uniques.push_back(name);
            }
        }
    }

    auto rng = [&settings](int i) { return settings.GetNext(i); };

    // Step 2: Shuffle the items
    std::random_shuffle(shuffle_artifacts.begin(), shuffle_artifacts.end(), rng);
    std::random_shuffle(shuffle_uniques.begin(), shuffle_uniques.end(), rng);

    // Step 3: Reassign artifacts/uniques
    auto assigner = [&result](std::vector<Record *> &artuniq, std::vector<std::string> &names) {
        for (size_t i = 0; i < artuniq.size(); ++i)
        {
            Record *r = artuniq[i];
            auto srs  = r->GetSubrecords("NAME");
            if (srs.size() == 0)
                continue;

            std::string newname = names[i];
            size_t datalen      = newname.length() + 1;
            auto srname         = std::make_unique<uint8_t[]>(datalen);
            memcpy(srname.get(), newname.c_str(), datalen);
            srs[0]->SetData(std::move(srname), datalen);

            r->ClearSubrecords({ "NAME" });
            r->AddSubrecord(std::move(std::make_unique<Subrecord>(*srs[0])));

            result.push_back(r);
        }
    };

    assigner(artifacts, shuffle_artifacts);
    if (!settings.ConsiderUniquesEqualToArtifacts)
        assigner(uniques, shuffle_uniques); // If they are considered the same then they'd already be put into the
                                            // artifact data structures, making this step unnecessary

    // Step 4: Fix dialogue/scripts using those artifacts

    return result;
}
