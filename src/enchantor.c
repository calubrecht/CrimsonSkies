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

    if (obj->wear_loc != 1)
    {
        send_to_char("You cannot wither an item that you cannot carry.\n\r", ch);
        return;
    }

    if (!IS_SET (obj->extra_flags, ITEM_ROT_DEATH))
    {
        SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
        act ("$p glows with a withered aura.", ch, obj, NULL, TO_CHAR);
        act ("$p glows with a withered aura.", ch, obj, NULL, TO_ROOM);
    }
    else
    {
        act ("$p already has a withered aura about it.", ch, obj, NULL, TO_CHAR);
    }

} // end withering_enchant

/*
 * Spell that enchants a person, it adds hitroll and damroll to the person
 * based off of the users casting level.  The target will also gain a small
 * mana boost if the victim is not the caster.
 */
void spell_enchant_person (int sn, int level, CHAR_DATA * ch, void *vo,
                              int target)
{
    /* character target */
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected (victim, sn))
    {
        if (victim == ch)
            send_to_char ("You have already been enchanted.\n\r", ch);
        else
            act ("$N has already been enchanted.", ch, NULL, victim, TO_CHAR);
        return;
    }

    int value = UMAX(1, ch->level / 10);

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_HITROLL;
    af.modifier = value;
    af.bitvector = 0;
    affect_to_char (victim, &af);

    af.location = APPLY_DAMROLL;
    af.modifier = value;

    affect_to_char (victim, &af);
    send_to_char ("You are surrounded with a light translucent {Bblue{x aura.\n\r", victim);

    if (ch != victim)
    {
        act ("$N is surrounded with a light translucent {Bblue{x aura.", ch, NULL, victim, TO_CHAR);

        // Small mana transfer from the caster to the target, but no more than their max mana.
        victim->mana += number_range(10, 20);

        if (victim->mana > victim->max_mana)
        {
            victim->mana = victim->max_mana;
        }
    }

} // end spell_enchant_person
