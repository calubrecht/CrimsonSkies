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
void spell_withering_enchant (int sn, int level, CHAR_DATA * ch, void *vo, int target)
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
void spell_enchant_person (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
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

/*
 * Enchantor spell that toggles whether an object is no locate or not.
 */
void spell_sequestor( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if (IS_OBJ_STAT(obj,ITEM_NOLOCATE))
    {
        act("$p turns translucent and then slowly returns to it's orginal form.",ch,obj,NULL,TO_CHAR);
        act("$p turns translucent and then slowly returns to it's orginal form.",ch,obj,NULL,TO_ROOM);
        REMOVE_BIT(obj->extra_flags,ITEM_NOLOCATE);
    }
    else
    {
       SET_BIT(obj->extra_flags,ITEM_NOLOCATE);
       act("$p turns translucent and then slowly returns to it's orginal form.",ch,obj,NULL,TO_CHAR);
       act("$p turns translucent and then slowly returns to it's orginal form.",ch,obj,NULL,TO_ROOM);
    }

} // end spell_sequestor

/*
 * Spell that makes it so the weapon or armor is only usable by people of the same alignment.
 * This could potentially make an object unusable if it's already set as ANTI something.
 */
void spell_interlace_spirit(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_STRING_LENGTH];

    if (obj->item_type != ITEM_WEAPON  && obj->item_type != ITEM_ARMOR)
    {
	send_to_char("You can only interlace your spirit into weapons or armor.\n\r",ch);
	return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("You must be able to carry an item to interlace your spirit into it.\n\r",ch);
	return;
    }

    if (IS_GOOD(ch))
    {
        SET_BIT(obj->extra_flags,ITEM_ANTI_EVIL);
        SET_BIT(obj->extra_flags,ITEM_ANTI_NEUTRAL);
    }
    else if (IS_NEUTRAL(ch))
    {
        SET_BIT(obj->extra_flags,ITEM_ANTI_GOOD);
        SET_BIT(obj->extra_flags,ITEM_ANTI_EVIL);
    }
    else
    {
        SET_BIT(obj->extra_flags,ITEM_ANTI_GOOD);
        SET_BIT(obj->extra_flags,ITEM_ANTI_NEUTRAL);
    }

    sprintf(buf, "%s has had %s's spirit interlaced into it.", obj->short_descr, ch->name);
    act(buf, ch, NULL, NULL, TO_ROOM);
    act(buf, ch, NULL, NULL, TO_CHAR);

    // If it's not unwieldable by all alignments send an additional message.
    if (IS_OBJ_STAT(obj,ITEM_ANTI_GOOD) &&
        IS_OBJ_STAT(obj,ITEM_ANTI_NEUTRAL) &&
        IS_OBJ_STAT(obj,ITEM_ANTI_EVIL))
    {
        sprintf(buf, "%s is surrounded by a dull gray aura that quickly dissipates.", obj->short_descr);
        act(buf, ch, NULL, NULL, TO_ROOM);
        act(buf, ch, NULL, NULL, TO_CHAR);
    }

} // end interlace_spirit
