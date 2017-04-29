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
 *                                                                         *
 *  Barbarians                                                             *
 *                                                                         *
 *  Barbarians are highly skilled fighters who are unable to learn the     *
 *  ways of magic (at all).  They typically have higher health and         *
 *  movement available to them and try to win victory with their sheer     *
 *  power.  This makes larger, dumber races much better barbarians.        *
 *                                                                         *
 *  Skills                                                                 *
 *                                                                         *
 *    - 4th attack                                                         *
 *    - Warcry                                                             *
 *    - Acute Vision                                                       *
 *                                                                         *
 ***************************************************************************/

// General Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

/*
 * Warcry, like berserk only better.
 */
void do_warcry(CHAR_DATA * ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill(ch, gsn_warcry)) == 0
        || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BERSERK))
        || (!IS_NPC(ch) && ch->level < skill_table[gsn_warcry]->skill_level[ch->class]))
    {
        send_to_char("You turn red in the face, but nothing happens.\r\n", ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_BERSERK)
        || is_affected(ch, gsn_berserk)
        || is_affected(ch, gsn_frenzy)
        || is_affected(ch, gsn_warcry))
    {
        send_to_char("You get a little madder.\r\n", ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_CALM))
    {
        send_to_char("You are far too calm to use your warcry.\r\n", ch);
        return;
    }

    if (ch->mana < 50)
    {
        send_to_char("You can't get up enough energy.\r\n", ch);
        return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
    {
        chance += 15;
    }

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit / ch->max_hit;
    chance += 25 - hp_percent / 2;

    if (number_percent() < chance)
    {
        AFFECT_DATA af;

        WAIT_STATE(ch, PULSE_VIOLENCE);
        ch->mana -= 50;
        ch->move /= 2;

        /* heal a little damage */
        ch->hit += ch->level * 2;
        ch->hit = UMIN(ch->hit, ch->max_hit);

        send_to_char("Your pulse races as you let out as massive WARCRY!\r\n", ch);
        act("$n lets out a massive WARCRY!", ch, NULL, NULL, TO_ROOM);
        check_improve(ch, gsn_warcry, TRUE, 3);

        af.where = TO_AFFECTS;
        af.type = gsn_warcry;
        af.level = ch->level;
        af.duration = number_fuzzy(ch->level / 6);
        af.modifier = UMAX(1, ch->level / 4);
        af.bitvector = AFF_BERSERK;

        af.location = APPLY_HITROLL;
        affect_to_char(ch, &af);

        af.location = APPLY_DAMROLL;
        affect_to_char(ch, &af);

        af.location = APPLY_DEX;
        af.modifier = -1;
        affect_to_char(ch, &af);

        af.location = APPLY_STR;
        af.modifier = 1;
        affect_to_char(ch, &af);
    }

    else
    {
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        ch->mana -= 25;
        ch->move = (ch->move * 4) / 5;

        send_to_char("Your attempt to muster a warcry but nothing happens.\r\n", ch);
        check_improve(ch, gsn_warcry, FALSE, 3);
    }
}

