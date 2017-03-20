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
*  Psionicist Class                                                        *
*                                                                          *
*  Psionicst's are those with magics that come deep within the mind who    *
*  can relay them onto physical entities in the world.                     *
*                                                                          *
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
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"

extern char *target_name;

/*
 * Psionic's Spell - Psionics Blast
 */
void spell_psionic_blast(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    int dam;

    dam = dice(level, 12);

    // First saves check to see if the damage is halved
    if (saves_spell(level, victim, DAM_MENTAL))
    {
        dam /= 2;
    }

    damage(ch, victim, dam, sn, DAM_MENTAL, TRUE);

    // Second saves check to see if there is an additional stun, short duration
    if (!saves_spell(level, victim, DAM_MENTAL))
    {
        send_to_char("You are knocked back by the psionic blast!\r\n", victim);
        act("$n has been knocked back by the psionic blast!", victim, NULL, NULL, TO_ROOM);

        if (number_range(1, 10) != 10)
        {
            // 90% of hitting this after the saves check
            DAZE_STATE(victim, PULSE_VIOLENCE);
        }
        else
        {
            // 10% of hitting this after the saves check
            DAZE_STATE(victim, PULSE_VIOLENCE);
        }

        return;
    }

    return;
}

/*
 * This is a spell that can be cast on others if they are sleeping.  It
 * cannot be cast on one's self but when a psioncist goes to sleep it will
 * automatically be added to their affects.
 */
void spell_healing_dream(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    AFFECT_DATA af;

    // Must be awake and must not be an NPC
    if (IS_AWAKE(victim) || IS_NPC(victim))
    {
        send_to_char("You failed.\r\n", ch);
        return;
    }

    if (is_affected(victim, sn))
    {
        act("$N is already affected by a healing dream.", ch, NULL, victim, TO_CHAR);
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You fall into a deep and restful sleep.\r\n", victim);

    if (ch != victim)
    {
        act("Your magic lulls $N into a deep and restful sleep.", ch, NULL, victim, TO_CHAR);
    }

    return;
}

/*
 * Forget spell, will cause the victim to have a lower chanct to succeed on all skills/spells (lowers
 * both by 33%).
 */
void spell_forget(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    AFFECT_DATA af;

    if (is_affected(victim, gsn_forget))
    {
        send_to_char("They are already forgetful.\r\n", ch);
        return;
    }

    if (saves_spell(level, victim, DAM_MENTAL))
    {
        send_to_char("You failed.\r\n", ch);
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.duration = 3;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    send_to_char("Your mind feels clouded.\r\n", victim);
    act("$n looks as if their mind is clouded.", victim, NULL, NULL, TO_ROOM);
    return;
}

/*
 * Spell that will cause the victim to have 2x mana cost to cast a spell.
 */
void spell_mental_weight(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn))
    {
        send_to_char("Their mind is already burdened.\r\n", ch);
        return;
    }

    if (saves_spell(level, victim, DAM_MENTAL))
    {
        send_to_char("The spell failed.\r\n", ch);
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = -1 * (level / 5);
    af.bitvector = 0;
    affect_to_char(victim, &af);

    send_to_char("Your mind feels burdened and heavy.\r\n", victim);
    act("$n looks as if they are burdened.", victim, NULL, NULL, TO_ROOM);
    return;
}

/*
 * Psionic Focus - Adds a focus which will increase hit (and possibly give a few
 * other bonuses in skills checks (TODO)
 */
void spell_psionic_focus(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn))
    {
        if (victim == ch)
        {
            // Remove the affect so it can be re-added to yourself
            affect_strip(victim, sn);
        }
        else
        {
            act("$N's mind is already focused.", ch, NULL, victim, TO_CHAR);
            return;
        }
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 2;
    af.location = APPLY_HITROLL;
    af.modifier = 3;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    send_to_char("Your mind becomes acutely focused.\r\n", victim);
    act("$n looks as if their mind is acutely focused.", victim, NULL, NULL, TO_ROOM);
    return;
}

