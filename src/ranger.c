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
 *                                                                         *
 *  Rangers                                                                *
 *                                                                         *
 *  Rangers are warriors who are associated with the wisdom of nature.     *
 *  They tend to be wise, cunning, and perceptive in addition to being     *
 *  skilled woodsmen. Many are skilled in stealth, wilderness survival,    *
 *  beast-mastery, herbalism, and tracking.  They are adept at swordplay   *
 *  and fight much better in a wilderness setting.  Rangers typically have *
 *  keen eye sight and are excellent at hiding in forest areas.            *
 *                                                                         *
 *  Skills                                                                 *
 *                                                                         *
 *    - Acute Vision                                                       *
 *    - Butcher                                                            *
 *    - Track (hunt.c)                                                     *
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
 * Skill a ranger can use to butcher steaks from PC and NPC corpses.  Yum.
 */
void do_butcher(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    OBJ_DATA *steak;

    // What do they want to butcher?
    if (IS_NULLSTR(argument))
    {
        send_to_char("What do you want to butcher?\r\n", ch);
        return;
    }

    // Find it in the room
    obj = get_obj_list(ch, argument, ch->in_room->contents);

    if (obj == NULL)
    {
        send_to_char("You can't find it.\r\n", ch);
        return;
    }

    // Is it a corpse of some kind?
    if ((obj->item_type != ITEM_CORPSE_NPC) && (obj->item_type != ITEM_CORPSE_PC))
    {
        send_to_char("You can only butcher corpses.\r\n", ch );
        return;
    }

    // Instead of dumping items to the ground, we'll just make them clean the corpse first
    if (obj->contains)
    {
       send_to_char("You will need to remove all items from the corpse first.\r\n",ch);
       return;
    }

    switch (ch->substate)
    {
        default:
            add_timer(ch, TIMER_DO_FUN, 12, do_butcher, 1, argument);
            send_to_char("You begin to prepare a steak.\r\n", ch);
            act("$n begins to prepare a steak.", ch, NULL, NULL, TO_ROOM);
            return;
        case 1:
            // Continue onward with said butchering.
            break;
        case SUB_TIMER_DO_ABORT:
            ch->substate = SUB_NONE;
            send_to_char("You stop butchering.\r\n", ch);
            act("$n stops butchering.", ch, NULL, NULL, TO_ROOM);
            return;
    }

    ch->substate = SUB_NONE;

    // The moment of truth, do they fail and mutilate the corpse?
    if (!check_skill_improve(ch, gsn_butcher, 3, 3))
    {
        send_to_char("You fail your attempt to butcher the corpse.\r\n", ch);
        separate_obj(obj);
        extract_obj(obj);
        return;
    }

    // Require that an object with a VNUM of 27 is created (as the steak), I didn't
    // feel the need to make a global constant when it's only used once here.
    steak = create_object(get_obj_index(27), 0);
    obj_to_char(steak, ch);

    // Show the player and the room the spoils (not spoiled)
    act( "$n prepares $p.", ch, steak, NULL, TO_ROOM );
    act( "You prepare $p.", ch, steak, NULL, TO_CHAR );

    // Seprate and extract the corpse from the room.
    separate_obj(obj);
    extract_obj(obj);

    return;

} // end do_butcher
