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
 *                                                                         *
 *  Bladesingers                                                           *
 *                                                                         *
 *  Bladesingers have been perfected by the elven race to be a perfect     *
 *  blend of sword play and magic.  They are nimble and elegant in battle. *
 *  Only elves and elf sub races can be bladesingers.                      *
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

/*
 * Bladesong is the underpinning skill of all bladesingers.  They must be
 * dancing the bladesong for any of their other skills to work.
 */
void do_bladesong( CHAR_DATA *ch, char *arg )
{
    AFFECT_DATA af;
    OBJ_DATA *weapon;
    int modifier = 0;

    if (IS_NPC(ch))
    {
        send_to_char("You cannot dance the bladesong.\n\r", ch);
        return;
    }

    if (ch->class != BLADESINGER_CLASS_LOOKUP)
    {
        send_to_char("Only bladesingers can dance the bladesong.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_bladesong))
    {
        send_to_char("You are already using the Bladesong.\n\r", ch);
        return;
    }

    if ( ch->level < skill_table[gsn_bladesong]->skill_level[ch->class] )
    {
        send_to_char("You are not yet skilled enough.\n\r", ch);
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char("You aren't in combat.\n\r", ch);
        return;
    }

    if ((weapon = get_eq_char( ch, WEAR_WIELD )) == NULL)
    {
        send_to_char("You must be wielding a weapon to dance the bladesong.\n\r", ch);
        return;
    }

    switch (weapon->value[0])
    {
        case(WEAPON_SWORD):
        case(WEAPON_DAGGER):
            break;
        default:
            send_to_char("The bladesong art is only possible with swords and daggers.\n\r", ch);
            return;
    }

    if (!CHANCE_SKILL(ch, gsn_bladesong))
    {
        act("You lose your concentration and stumble.", ch, NULL, NULL, TO_CHAR);
        act("$n loses their concentration and stumbles.", ch, NULL, NULL, TO_ROOM);
        check_improve(ch, gsn_bladesong, FALSE, 5);
        return;
    }

    act("You hum a haunting tune and begin to dance about.", ch, NULL, NULL, TO_CHAR);
    act("$n hums a haunting tune and begins to dance about.", ch, NULL, NULL, TO_ROOM);

    modifier = UMAX(1, ch->level / 10);

    af.where = TO_AFFECTS;
    af.type = gsn_bladesong;
    af.level = ch->level;
    af.duration = 5;
    af.modifier = modifier;
    af.location = APPLY_HITROLL;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location = APPLY_DAMROLL;
    affect_to_char( ch, &af );

    af.modifier = modifier * -10;
    af.location = APPLY_AC;
    affect_to_char( ch, &af );

    check_improve(ch, gsn_bladesong, TRUE, 5);

} // end do_bladesong
