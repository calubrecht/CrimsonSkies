/***************************************************************************
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2016 by Blake Pell (Rhien)   *
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
 *  Experience                                                             *
 ***************************************************************************
 *                                                                         *
 *  This will house code related to calculating and distributing           *
 *  experience to players in order to gain levels.  Initially this will    *
 *  include the traditional Diku/Rom style experience but will and should  *
 *  also in the future include other ways to gain experience or different  *
 *  types of experience.                                                   *
 *                                                                         *
 ***************************************************************************/

// System Specific Includes
#if defined(_WIN32)
    #include <sys/types.h>
    #include <time.h>
#else
    #include <sys/types.h>
    #include <sys/time.h>
    #include <time.h>
#endif

// General Includes
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute(CHAR_DATA * gch, CHAR_DATA * victim, int total_levels)
{
    int xp, base_exp;
    int level_range;
    int time_per_level;

    if (IS_SET(gch->affected_by, AFF_CHARM))
        return 0;

    xp = 0;
    level_range = victim->level - gch->level;

    /* compute the base exp */
    switch (level_range)
    {
        default:
            base_exp = 0;
            break;
        case -9:
            base_exp = 1;
            break;
        case -8:
            base_exp = 2;
            break;
        case -7:
            base_exp = 5;
            break;
        case -6:
            base_exp = 9;
            break;
        case -5:
            base_exp = 11;
            break;
        case -4:
            base_exp = 22;
            break;
        case -3:
            base_exp = 33;
            break;
        case -2:
            base_exp = 50;
            break;
        case -1:
            base_exp = 66;
            break;
        case 0:
            base_exp = 83;
            break;
        case 1:
            base_exp = 99;
            break;
        case 2:
            base_exp = 121;
            break;
        case 3:
            base_exp = 143;
            break;
        case 4:
            base_exp = 165;
            break;
    }

    // If level range is greater than 4 between player and victim then
    // give them a sliding bonus
    if (level_range > 4)
        base_exp = 160 + 20 * (level_range - 4);

    // Instead adding up the base, going to create slight increase here
    // that can be tweaked as needed.
    base_exp = base_exp * 4 / 3;

    /* do alignment computations */
    // XP was getting wiped out for certain setups here, put cases in for
    // everything.  - Rhien
    switch (gch->alignment)
    {
        case ALIGN_GOOD:
            switch (victim->alignment)
            {
                case ALIGN_GOOD: xp += base_exp * 2 / 3; break;
                case ALIGN_NEUTRAL: xp = base_exp; break;
                case ALIGN_EVIL: xp += base_exp * 3 / 2; break;
                default: break;
            }
            break;
        case ALIGN_NEUTRAL:
            switch (victim->alignment)
            {
                case ALIGN_EVIL: xp = base_exp; break;
                case ALIGN_GOOD: xp += base_exp * 5 / 4; break;
                case ALIGN_NEUTRAL: xp += base_exp * 4 / 5; break;
                default: break;
            }
            break;
        case ALIGN_EVIL:
            switch (victim->alignment)
            {
                case ALIGN_EVIL: xp += base_exp * 2 / 3; break;
                case ALIGN_NEUTRAL: xp = base_exp; break;
                case ALIGN_GOOD: xp += base_exp * 3 / 2; break;
                default: break;
            }
            break;
        default: break;
    }

    /* more exp at the low levels */
    if (gch->level < 6)
        xp = 10 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > 35)
        xp = 15 * xp / (gch->level - 25);

    /* reduce for playing time */
    {
        /* compute quarter-hours per level */
        time_per_level = 4 *
            (gch->played + (int)(current_time - gch->logon)) / 3600
            / gch->level;

        time_per_level = URANGE(2, time_per_level, 12);
        if (gch->level < 15)    /* make it a curve */
            time_per_level = UMAX(time_per_level, (15 - gch->level));
        xp = xp * time_per_level / 12;
    }

    /* randomize the rewards */
    xp = number_range(xp * 3 / 4, xp * 5 / 4);

    /* adjust for grouping */
    xp = xp * gch->level / (UMAX(1, total_levels - 1));

    /* bonus for intelligence */
    xp = (xp * (100 + (get_curr_stat(gch, STAT_INT) * 4))) / 100;

    // Double experience bonus (this must be the last check to truly double
    // the calculation.
    if (settings.double_exp)
        xp *= 2;

    return xp;
}

/*
 * Gain experience points in a group
 */
void group_gain(CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (victim == ch)
        return;

    members = 0;
    group_levels = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        if (is_same_group(gch, ch))
        {
            members++;
            group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
        }
    }

    if (members == 0)
    {
        bug("Group_gain: members.", members);
        members = 1;
        group_levels = ch->level;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        if (!is_same_group(gch, ch) || IS_NPC(gch))
            continue;

        if (gch->level - lch->level >= 8)
        {
            send_to_char("You are too high for this group.\r\n", gch);
            continue;
        }

        if (gch->level - lch->level <= -8)
        {
            send_to_char("You are too low for this group.\r\n", gch);
            continue;
        }

        xp = xp_compute(gch, victim, group_levels);

        sprintf(buf, "You receive %d experience points.\r\n", xp);
        send_to_char(buf, gch);
        gain_exp(gch, xp);

        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc == WEAR_NONE)
                continue;

            if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
                || (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
                || (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)))
            {
                act("You are zapped by $p.", ch, obj, NULL, TO_CHAR);
                act("$n is zapped by $p.", ch, obj, NULL, TO_ROOM);
                obj_from_char(obj);
                obj_to_room(obj, ch->in_room);
            }
        }
    }

    return;
}
