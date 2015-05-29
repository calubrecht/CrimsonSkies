/***************************************************************************
 *  Crimson Skies Mud - Copyright (C) 2015                                 *
 *  Blake Pell (blakepell@hotmail.com)                                     *
 ***************************************************************************/

/***************************************************************************
 *  Enchantors                                                             *
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
 * Spell that sets a rot-death flag on an item so it crumbles when the
 * player dies.
 */
void spell_withering_enchant (int sn, int level, CHAR_DATA * ch, void *vo,
                              int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
    act ("$p glows with a dark aura.", ch, obj, NULL, TO_CHAR);
    act ("$p glows with a dark aura.", ch, obj, NULL, TO_ROOM);

} // end withering_enchant
