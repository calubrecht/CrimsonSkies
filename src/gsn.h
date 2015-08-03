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
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell           *
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
 *  This code originated from a post made by Erwin Andreasen to the MERC/  *
 *  Envy mail list in January of 1998.  The original GSN's were defined in *
 *  db.c and merc.h.  This should simplify the process of adding new gsn   *
 *  entries.                                                               *
 *                                                                         *
 *  http://www.andreasen.org/letters/autogsn.txt                           *
 *  http://www.andreasen.org/letters/gsns.txt                              *
 *                                                                         *
 *  merc.h will have the global include for this file and then the IN_DB_C *
 *  flag will be set in db.c.  That should be all we need to set this up.  *
 *                                              - Rhien                    *
 **************************************************************************/

#ifdef IN_DB_C
    #define GSN(gsn) sh_int gsn;
#else
    #define GSN(gsn) extern sh_int gsn;
#endif

GSN(gsn_backstab)
GSN(gsn_dodge)
GSN(gsn_envenom)
GSN(gsn_hide)
GSN(gsn_peek)
GSN(gsn_pick_lock)
GSN(gsn_sneak)
GSN(gsn_steal)
GSN(gsn_disarm)
GSN(gsn_enhanced_damage)
GSN(gsn_kick)
GSN(gsn_parry)
GSN(gsn_rescue)
GSN(gsn_second_attack)
GSN(gsn_third_attack)
GSN(gsn_blindness)
GSN(gsn_charm_person)
GSN(gsn_curse)
GSN(gsn_invis)
GSN(gsn_mass_invis)
GSN(gsn_plague)
GSN(gsn_poison)
GSN(gsn_sleep)
GSN(gsn_fly)
GSN(gsn_sanctuary)
GSN(gsn_axe)
GSN(gsn_dagger)
GSN(gsn_flail)
GSN(gsn_mace)
GSN(gsn_polearm)
GSN(gsn_shield_block)
GSN(gsn_spear)
GSN(gsn_sword)
GSN(gsn_whip)
GSN(gsn_bash)
GSN(gsn_berserk)
GSN(gsn_dirt)
GSN(gsn_hand_to_hand)
GSN(gsn_trip)
GSN(gsn_fast_healing)
GSN(gsn_haggle)
GSN(gsn_lore)
GSN(gsn_meditation)
GSN(gsn_scrolls)
GSN(gsn_staves)
GSN(gsn_wands)
GSN(gsn_recall)
GSN(gsn_dual_wield)
GSN(gsn_weaken)
