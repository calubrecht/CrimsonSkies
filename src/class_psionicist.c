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

