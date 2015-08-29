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
 *  Healers                                                                *
 *                                                                         *
 *  Healers are masters of healing people's bodies as well as having       *
 *  curative expertise such as removing disease, poison, etc.  They are    *
 *  more of a support class that are effective group individuals.  In the  *
 *  new pk system even support classes can benefit if the group they are   *
 *  in does well.  -Rhien                                                  *
 ***************************************************************************/

// General Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"

/*
 * Will heal an individual to full health at great cost to the healer who will lose most of
 * their health .  In order to use this spell the healer must have most of their health
 * themselves.  This is the hail mary of heals.
 */
void spell_sacrificial_heal(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    victim = (CHAR_DATA *) vo;

    // Cannot cast the spell on themselves, credit their mana back.
    if (ch == victim)
    {
        ch->mana += 250;
        send_to_char("You cannot cast this spell on yourself.\n\r", ch);
        return;
    }

    // They can't use this if they're in in the yellow on damage, credit their mana
    // back to them.  Consider getting the mana cost out of the table in case it changes
    // down the line.
    if (ch->hit < ch->max_hit * 3 / 4)
    {
        ch->mana += 250;
        send_to_char("You are too weak currently to cast this spell\n\r", ch);
        return;
    }

    // Set the target to full health, set the healer to near death.
    victim->hit = victim->max_hit;
    ch->hit = 100;

    act ("$n raises $e hands to the sky and surrounds $N with a healing aura.", ch, NULL, victim, TO_NOTVICT);
    act ("$n raises $e hands to the sky and surrounds you with a healing aura.", ch, NULL, victim, TO_VICT);
    act ("You raise your hands to the sky and surround $N with a healing aura.", ch, NULL, victim, TO_CHAR);

} // end spell_sacrificial_heal

void spell_mass_refresh (int sn, int level, CHAR_DATA * ch, void *vo,
                         int target)
{
    CHAR_DATA *gch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        gch->move = UMIN(gch->move + level, gch->max_move);

        if (gch->max_move == gch->move)
        {
            send_to_char("You feel fully refreshed!\n\r", gch);
        }
        else
        {
            send_to_char("You feel less tired.\n\r", gch);
        }
    }

    send_to_char("Ok.\n\r", ch);

} // end spell_mass_refresh
