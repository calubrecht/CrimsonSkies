/***************************************************************************
*  Crimson Skies (CS-Mud) copyright (C) 1998-2017 by Blake Pell (Rhien)   *
***************************************************************************
*  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
*                                                                         *
*  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
*  Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                         *
*  ROM 2.4 improvements copyright (C) 1993-1998 Russ Taylor, Gabrielle    *
*  Taylor and Brian Moore                                                 *
*                                                                         *
*  In order to use any part of this Diku Mud, you must comply with        *
*  both the original Diku license in 'license.doc' as well the Merc       *
*  license in 'license.txt' as well as the ROM license.  In particular,   *
*  you may not remove these copyright notices.                            *
*                                                                         *
*  Much time and thought has gone into this software and you are          *
*  benefitting.  We hope that you share your changes too.  What goes      *
*  around, comes around.                                                  *
**************************************************************************/

/***************************************************************************
*  Portions of this code originated from a post made by Erwin Andreasen   *
*  to the MERC/Envy mail list in January of 1998.  Additional code for    *
*  assigning the GSN's originated from the Smaug code base and is needed  *
*  to for setting the pointers in the skill table to the gsn where        *
*  necessary.                                                             *
*                                                                         *
*  merc.h will have the global include for this file and then the         *
*  IN_GSN_C flag will be set in db.c.  That should be all we need to set  *
*  this up.                                                               *
*                                                                         *
*                                              - Rhien                    *
**************************************************************************/

// System Specific Includes
#if defined(_WIN32)
#include <sys/types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#endif

/* Needed for automatic GSN assignment */
#define IN_GSN_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

/*
* Assign GSN's to the proper skill.  If the gsn value is referenced instead
* of calls to skill_lookup it will be much more efficient as these are only
* loaded once at startup (as opposed to say, looking up 50 spells everytime
* the cancel spell or dispel spell is cast).
*/
void assign_gsn()
{
    ASSIGN_GSN(gsn_backstab, "backstab");
    ASSIGN_GSN(gsn_dodge, "dodge");
    ASSIGN_GSN(gsn_envenom, "envenom");
    ASSIGN_GSN(gsn_hide, "hide");
    ASSIGN_GSN(gsn_peek, "peek");
    ASSIGN_GSN(gsn_pick_lock, "pick lock");
    ASSIGN_GSN(gsn_sneak, "sneak");
    ASSIGN_GSN(gsn_steal, "steal");
    ASSIGN_GSN(gsn_disarm, "disarm");
    ASSIGN_GSN(gsn_enhanced_damage, "enhanced damage");
    ASSIGN_GSN(gsn_kick, "kick");
    ASSIGN_GSN(gsn_parry, "parry");
    ASSIGN_GSN(gsn_rescue, "rescue");
    ASSIGN_GSN(gsn_second_attack, "second attack");
    ASSIGN_GSN(gsn_third_attack, "third attack");
    ASSIGN_GSN(gsn_blindness, "blindness");
    ASSIGN_GSN(gsn_charm_person, "charm person");
    ASSIGN_GSN(gsn_change_sex, "change sex");
    ASSIGN_GSN(gsn_curse, "curse");
    ASSIGN_GSN(gsn_chill_touch, "chill touch");
    ASSIGN_GSN(gsn_invis, "invisibility");
    ASSIGN_GSN(gsn_mass_invis, "mass invis");
    ASSIGN_GSN(gsn_plague, "plague");
    ASSIGN_GSN(gsn_poison, "poison");
    ASSIGN_GSN(gsn_sleep, "sleep");
    ASSIGN_GSN(gsn_fly, "fly");
    ASSIGN_GSN(gsn_sanctuary, "sanctuary");
    ASSIGN_GSN(gsn_axe, "axe");
    ASSIGN_GSN(gsn_dagger, "dagger");
    ASSIGN_GSN(gsn_flail, "flail");
    ASSIGN_GSN(gsn_mace, "mace");
    ASSIGN_GSN(gsn_polearm, "polearm");
    ASSIGN_GSN(gsn_shield_block, "shield block");
    ASSIGN_GSN(gsn_spear, "spear");
    ASSIGN_GSN(gsn_sword, "sword");
    ASSIGN_GSN(gsn_whip, "whip");
    ASSIGN_GSN(gsn_bash, "bash");
    ASSIGN_GSN(gsn_berserk, "berserk");
    ASSIGN_GSN(gsn_dirt, "dirt");
    ASSIGN_GSN(gsn_hand_to_hand, "hand to hand");
    ASSIGN_GSN(gsn_trip, "trip");
    ASSIGN_GSN(gsn_fast_healing, "fast healing");
    ASSIGN_GSN(gsn_haggle, "haggle");
    ASSIGN_GSN(gsn_lore, "lore");
    ASSIGN_GSN(gsn_meditation, "meditation");
    ASSIGN_GSN(gsn_scrolls, "scrolls");
    ASSIGN_GSN(gsn_staves, "staves");
    ASSIGN_GSN(gsn_wands, "wands");
    ASSIGN_GSN(gsn_recall, "recall");
    ASSIGN_GSN(gsn_dual_wield, "dual wield");
    ASSIGN_GSN(gsn_weaken, "weaken");
    ASSIGN_GSN(gsn_water_breathing, "water breathing");
    ASSIGN_GSN(gsn_spellcraft, "spellcraft");
    ASSIGN_GSN(gsn_swim, "swim");
    ASSIGN_GSN(gsn_vitalizing_presence, "vitalizing presence");
    ASSIGN_GSN(gsn_sense_affliction, "sense affliction");
    ASSIGN_GSN(gsn_slow, "slow");
    ASSIGN_GSN(gsn_immortal_blessing, "immortal blessing");
    ASSIGN_GSN(gsn_enhanced_recovery, "enhanced recovery");
    ASSIGN_GSN(gsn_bladesong, "bladesong");
    ASSIGN_GSN(gsn_circle, "circle");
    ASSIGN_GSN(gsn_disorientation, "disorientation");
    ASSIGN_GSN(gsn_blind_fighting, "blind fighting");
    ASSIGN_GSN(gsn_armor, "armor");
    ASSIGN_GSN(gsn_bless, "bless");
    ASSIGN_GSN(gsn_song_of_dissonance, "song of dissonance");
    ASSIGN_GSN(gsn_song_of_protection, "song of protection");
    ASSIGN_GSN(gsn_enhanced_recall, "enhanced recall");
    ASSIGN_GSN(gsn_circlestab, "circle stab");
    ASSIGN_GSN(gsn_gore, "gore");
    ASSIGN_GSN(gsn_ghost, "ghost");
    ASSIGN_GSN(gsn_enchant_person, "enchant person");
    ASSIGN_GSN(gsn_track, "track");
    ASSIGN_GSN(gsn_acute_vision, "acute vision");
    ASSIGN_GSN(gsn_butcher, "butcher");
    ASSIGN_GSN(gsn_bandage, "bandage");
    ASSIGN_GSN(gsn_quiet_movement, "quiet movement");
    ASSIGN_GSN(gsn_camping, "camping");
    ASSIGN_GSN(gsn_camouflage, "camouflage");
    ASSIGN_GSN(gsn_ambush, "ambush");
    ASSIGN_GSN(gsn_find_water, "find water");
    ASSIGN_GSN(gsn_poison_prick, "poison prick");
    ASSIGN_GSN(gsn_shiv, "shiv");
    ASSIGN_GSN(gsn_protection_good, "protection good");
    ASSIGN_GSN(gsn_protection_evil, "protection evil");
    ASSIGN_GSN(gsn_protection_neutral, "protection neutral");
    ASSIGN_GSN(gsn_escape, "escape");
    ASSIGN_GSN(gsn_peer, "peer");
    ASSIGN_GSN(gsn_bludgeon, "bludgeon");
    ASSIGN_GSN(gsn_revolt, "revolt");
    ASSIGN_GSN(gsn_imbue, "imbue");
    ASSIGN_GSN(gsn_preserve, "preserve");
    ASSIGN_GSN(gsn_haste, "haste");
    ASSIGN_GSN(gsn_giant_strength, "giant strength");
    ASSIGN_GSN(gsn_calm, "calm");
    ASSIGN_GSN(gsn_detect_evil, "detect evil");
    ASSIGN_GSN(gsn_detect_good, "detect good");
    ASSIGN_GSN(gsn_detect_hidden, "detect hidden");
    ASSIGN_GSN(gsn_detect_invis, "detect invis");
    ASSIGN_GSN(gsn_detect_magic, "detect magic");
    ASSIGN_GSN(gsn_faerie_fire, "faerie fire");
    ASSIGN_GSN(gsn_frenzy, "frenzy");

    if (global.last_boot_result == UNKNOWN)
    {
        global.last_boot_result = SUCCESS;
    }

} // end assign_gsn