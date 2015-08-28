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
 *  Healers are masters of healing people's bodies as well as curative     *
 *  such as removing disease, poison, etc.  They are more of a support     *
 *  class that are effective group individuals.  -Rhien                    *
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
 * their health, mana and movement.  In order to use this spell the healer must have most of
 * their health themselves.  This is the hail mary of heals.
 */
void spell_sacrificial_heal(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    victim = (CHAR_DATA *) vo;

    if (ch == victim)
    {
        send_to_char("You cannot cast this spell on yourself.\n\r", ch);
        return;
    }

    send_to_char("stub.\n\r", ch);

} // end spell_sacrificial_heal
