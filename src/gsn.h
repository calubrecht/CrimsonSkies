/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  ROM 2.4 improvements copyright (C) 1993-1998 Russ Taylor, Gabrielle    *
 *  Taylor and Brian Moore                                                 *
 *                                                                         *
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell (Rhien)   *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
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
 *  merc.h will have the global include for this file and then the IN_DB_C *
 *  flag will be set in db.c.  That should be all we need to set this up.  *
 *                                                                         *
 *                                              - Rhien                    *
 **************************************************************************/

/*
 * Declare the GSN in every file (via this being included in merc.h).  The
 * variable with be defined as an sh_int in db.c and an extern in every
 * other file so they're accessible.
 */
#ifdef IN_DB_C
    #define DECLARE_GSN(gsn) sh_int gsn;
#else
    #define DECLARE_GSN(gsn) extern sh_int gsn;
#endif

/*
 * Assign the GSN to the appropriate skill/spell.  Unfortunately this
 * will need to be called from a C file (in our case, you will need
 * to go update assign_gsn in db.c to map this gsn to the appripriate
 * entry in the skill table.
 */
#define ASSIGN_GSN(gsn, skill)                                  \
do                                                              \
{                                                               \
    if ( ((gsn) = skill_lookup((skill))) == -1 )                \
        fprintf( stderr, "ASSIGN_GSN: Skill %s not found.\n",   \
                (skill) );                                      \
} while(0)

DECLARE_GSN(gsn_backstab)
DECLARE_GSN(gsn_dodge)
DECLARE_GSN(gsn_envenom)
DECLARE_GSN(gsn_hide)
DECLARE_GSN(gsn_peek)
DECLARE_GSN(gsn_pick_lock)
DECLARE_GSN(gsn_sneak)
DECLARE_GSN(gsn_steal)
DECLARE_GSN(gsn_disarm)
DECLARE_GSN(gsn_enhanced_damage)
DECLARE_GSN(gsn_kick)
DECLARE_GSN(gsn_parry)
DECLARE_GSN(gsn_rescue)
DECLARE_GSN(gsn_second_attack)
DECLARE_GSN(gsn_third_attack)
DECLARE_GSN(gsn_blindness)
DECLARE_GSN(gsn_charm_person)
DECLARE_GSN(gsn_curse)
DECLARE_GSN(gsn_invis)
DECLARE_GSN(gsn_mass_invis)
DECLARE_GSN(gsn_plague)
DECLARE_GSN(gsn_poison)
DECLARE_GSN(gsn_sleep)
DECLARE_GSN(gsn_fly)
DECLARE_GSN(gsn_sanctuary)
DECLARE_GSN(gsn_axe)
DECLARE_GSN(gsn_dagger)
DECLARE_GSN(gsn_flail)
DECLARE_GSN(gsn_mace)
DECLARE_GSN(gsn_polearm)
DECLARE_GSN(gsn_shield_block)
DECLARE_GSN(gsn_spear)
DECLARE_GSN(gsn_sword)
DECLARE_GSN(gsn_whip)
DECLARE_GSN(gsn_bash)
DECLARE_GSN(gsn_berserk)
DECLARE_GSN(gsn_dirt)
DECLARE_GSN(gsn_hand_to_hand)
DECLARE_GSN(gsn_trip)
DECLARE_GSN(gsn_fast_healing)
DECLARE_GSN(gsn_haggle)
DECLARE_GSN(gsn_lore)
DECLARE_GSN(gsn_meditation)
DECLARE_GSN(gsn_scrolls)
DECLARE_GSN(gsn_staves)
DECLARE_GSN(gsn_wands)
DECLARE_GSN(gsn_recall)
DECLARE_GSN(gsn_dual_wield)
DECLARE_GSN(gsn_weaken)
DECLARE_GSN(gsn_water_breathing)
DECLARE_GSN(gsn_spellcraft)
DECLARE_GSN(gsn_swim)
DECLARE_GSN(gsn_vitalizing_presence)
DECLARE_GSN(gsn_sense_affliction)
DECLARE_GSN(gsn_slow)
