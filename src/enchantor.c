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
 *  Enchantors                                                             *
 *                                                                         *
 *  This class will be an expert at enchanting or altering items           *
 *  as well as having the potential to also enchant other characters       *
 *  abilities as well as receive bonuses on tradtional mage spells like    *
 *  charm.  The enchantor is a mage who specializes in enhancing others    *
 *  and objects.  -Rhien                                                   *
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
 * mana boost if the victim is not the caster as well as a small AC boost.
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

    af.modifier = -5;
    af.location = APPLY_AC;
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

/*
 * Spell that allows an enchantor to wizard mark an item.  A wizard marked item can always
 * be located in the world with locate if it exists, the enchantor can also see (for a lot of mana)
 * at the location of the object if it's not a private place or clan hall, etc.
 */
void spell_wizard_mark( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_INPUT_LENGTH];

    if (obj->item_type != ITEM_WEAPON  && obj->item_type != ITEM_ARMOR)
    {
        send_to_char("You can only wizard mark weapons or armor.\n\r", ch);
        return;
    }

    if (obj->wizard_mark != NULL)
    {
        if (strstr(obj->wizard_mark, ch->name) != NULL)
        {
            send_to_char ("This item already carries your wizard mark.\n\r", ch);
            return;
        }
    }

    sprintf(buf,"%s", ch->name);
    free_string(obj->wizard_mark);
    obj->wizard_mark = str_dup(buf);
    act("You mark $p with your name.",ch,obj,NULL,TO_CHAR);

} // spell_wizard_mark

/*
 * Spell that allows an enchantor to enchant any type of gem and turn it into
 * a warpstone.
 */
void spell_enchant_gem(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    OBJ_DATA *obj_warpstone;
    char buf[MAX_STRING_LENGTH];

    if (obj->item_type != ITEM_GEM)
    {
        send_to_char("That item is not a gem.\n\r",ch);
        return;
    }

    if (obj->wear_loc != -1)
    {
        send_to_char("The gem must be carried to be enchanted.\n\r",ch);
        return;
    }

    // Create the warpstone, create the message while both objects exist, then take the gem and give the
    // warpstone to the player
    obj_warpstone = create_object(get_obj_index(OBJ_VNUM_WARPSTONE), 0);
    sprintf(buf, "%s glows a bright {Mmagenta{x and changes into %s.", obj->short_descr, obj_warpstone->short_descr);
    obj_from_char( obj );
    obj_to_char( obj_warpstone, ch );

    // Show the caster and the room what has happened
    act(buf, ch, NULL, NULL, TO_ROOM);
    act(buf, ch, NULL, NULL, TO_CHAR);

} // end spell_enchant_gem

/*
*  Spell that enchants a piece of armor.  Mages and certain mage classes will be able
*  to enchant but enchantors are far more skilled at it. This spell is the merc/rom
*  version slightly modified for enchantor bonuses.  This is modified from the base
*  Diku/Merc/Rom version.
*/
void spell_enchant_armor(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
	OBJ_DATA *obj = (OBJ_DATA *)vo;
	AFFECT_DATA *paf;
	int result, fail;
	int ac_bonus, added;
	bool ac_found = FALSE;
	char buf[MAX_STRING_LENGTH];

	if (obj->item_type != ITEM_ARMOR)
	{
		send_to_char("That isn't an armor.\n\r", ch);
		return;
	}

	if (obj->wear_loc != -1)
	{
		send_to_char("The item must be carried to be enchanted.\n\r", ch);
		return;
	}

	// this means they have no bonus
	ac_bonus = 0;
	// base 25% chance of failure
	fail = 25;

	// Enchantors have a lower base chance of failure
	if (ch->class == ENCHANTOR_CLASS_LOOKUP)
	{
		fail = 5;
	}

	// find the bonuses
	if (!obj->enchanted)
	{
		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location == APPLY_AC)
			{
				ac_bonus = paf->modifier;
				ac_found = TRUE;
				fail += 5 * (ac_bonus * ac_bonus);
			}
			else
			{
				// things get a little harder
				fail += 20;
			}
		}
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location == APPLY_AC)
		{
			ac_bonus = paf->modifier;
			ac_found = TRUE;
			fail += 5 * (ac_bonus * ac_bonus);
		}
		else
		{
			// things get a little harder
			fail += 20;
		}
	}

	/* apply other modifiers */
	fail -= level;

	if (IS_OBJ_STAT(obj, ITEM_BLESS))
		fail -= 15;
	if (IS_OBJ_STAT(obj, ITEM_GLOW))
		fail -= 5;

	fail = URANGE(5, fail, 85);

	result = number_percent();

	// Cap in case they get over -14AC
	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location == APPLY_AC)
		{
			ac_bonus = paf->modifier;
			if (ac_bonus <= -14)
			{
				result = 0;
			}
		}
	}

	// the moment of truth
	if (result < (fail / 5))
	{
		// item destroyed
		act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_CHAR);
		act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_ROOM);
		extract_obj(obj);
		return;
	}

	if (result < (fail / 3))
	{
		// item disenchanted
		AFFECT_DATA *paf_next;

		act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
		act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
		obj->enchanted = TRUE;

		// remove all affects
		for (paf = obj->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			free_affect(paf);
		}
		obj->affected = NULL;

		// clear all flags
		obj->extra_flags = 0;
		return;
	}

	if (result <= fail)
	{
		// failed, no bad result
		send_to_char("Nothing seemed to happen.\n\r", ch);
		return;
	}

	// move all the old flags into new vectors if we have to
	if (!obj->enchanted)
	{
		AFFECT_DATA *af_new;
		obj->enchanted = TRUE;

		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where = paf->where;
			af_new->type = UMAX(0, paf->type);
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
		}
	}

	// They have a success at this point, go ahead and set the enchanted_by
	sprintf(buf, "%s", ch->name);
	free_string(obj->enchanted_by);
	obj->enchanted_by = str_dup(buf);

	if (ch->class == ENCHANTOR_CLASS_LOOKUP && result <= (90 - level))
	{
		// Enchantors only have a chance for the highest enchant.
		act("$p glows a brilliant {Wwhite{x!", ch, obj, NULL, TO_CHAR);
		act("$p glows a brilliant {Wwhite{x!", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		SET_BIT(obj->extra_flags, ITEM_GLOW);
		added = -3;
	}
	else if (result <= (90 - level / 5))
	{
		// success!
		act("$p shimmers with a gold aura.", ch, obj, NULL, TO_CHAR);
		act("$p shimmers with a gold aura.", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		added = -1;
	}

	else
	{
		// exceptional enchant, highest a non enchantor can go.
		act("$p glows a brilliant gold!", ch, obj, NULL, TO_CHAR);
		act("$p glows a brilliant gold!", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		SET_BIT(obj->extra_flags, ITEM_GLOW);
		added = -2;
	}

	// now, add the enchantments
	// level of the object will increase by one
	if (obj->level < LEVEL_HERO)
		obj->level = UMIN(LEVEL_HERO - 1, obj->level + 1);

	if (ac_found)
	{
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location == APPLY_AC)
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX(paf->level, level);
			}
		}
	}
	else
	{                            /* add a new affect */

		paf = new_affect();

		paf->where = TO_OBJECT;
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_AC;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}

} // end spell_enchant_armor

/*
* Spell that enchants a weapon increasing it's hit and dam roll.  Some classes like
* mages can enchant but  Enchantors specialize in this and get bonuses.  This spell
* is the merc/rom version slightly modified for enchantor bonuses.
*/
void spell_enchant_weapon(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
	OBJ_DATA *obj = (OBJ_DATA *)vo;
	AFFECT_DATA *paf;
	int result, fail;
	int hit_bonus, dam_bonus, added;
	bool hit_found = FALSE, dam_found = FALSE;
	char buf[MAX_STRING_LENGTH];

	if (obj->item_type != ITEM_WEAPON)
	{
		send_to_char("That isn't a weapon.\n\r", ch);
		return;
	}

	if (obj->wear_loc != -1)
	{
		send_to_char("The item must be carried to be enchanted.\n\r", ch);
		return;
	}

	// this means they have no bonus
	hit_bonus = 0;
	dam_bonus = 0;
	// base 25% chance of failure
	fail = 25;

	// Enchantors have a lower base chance of failure
	if (ch->class == ENCHANTOR_CLASS_LOOKUP)
	{
		fail = 5;
	}

	// find the bonuses
	if (!obj->enchanted)
	{
		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location == APPLY_HITROLL)
			{
				hit_bonus = paf->modifier;
				hit_found = TRUE;
				fail += 2 * (hit_bonus * hit_bonus);
			}

			else if (paf->location == APPLY_DAMROLL)
			{
				dam_bonus = paf->modifier;
				dam_found = TRUE;
				fail += 2 * (dam_bonus * dam_bonus);
			}
			else
			{
				// things get a little harder
				fail += 25;
			}
		}
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location == APPLY_HITROLL)
		{
			hit_bonus = paf->modifier;
			hit_found = TRUE;
			fail += 2 * (hit_bonus * hit_bonus);
		}
		else if (paf->location == APPLY_DAMROLL)
		{
			dam_bonus = paf->modifier;
			dam_found = TRUE;
			fail += 2 * (dam_bonus * dam_bonus);
		}
		else
		{
			// things get a little harder
			fail += 25;
		}
	}

	// apply other modifiers
	fail -= 3 * level / 2;

	if (IS_OBJ_STAT(obj, ITEM_BLESS))
		fail -= 15;
	if (IS_OBJ_STAT(obj, ITEM_GLOW))
		fail -= 5;

	fail = URANGE(5, fail, 95);
	result = number_percent();

	// Enchanting cap at 14 hit or 14 damage (which should be impossibly rare)
	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location == APPLY_HITROLL)
		{
			hit_bonus = paf->modifier;
			if (hit_bonus >= 14) { result = 0; }
		}
		else if (paf->location == APPLY_DAMROLL)
		{
			dam_bonus = paf->modifier;
			if (dam_bonus >= 14) { result = 0; }
		}
	}

	// the moment of truth
	if (result < (fail / 5))
	{
		// item destroyed
		act("$p shivers violently and explodes!", ch, obj, NULL, TO_CHAR);
		act("$p shivers violently and explodeds!", ch, obj, NULL, TO_ROOM);
		extract_obj(obj);
		return;
	}

	if (result < (fail / 2))
	{
		// item disenchant
		AFFECT_DATA *paf_next;

		act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
		act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
		obj->enchanted = TRUE;

		// remove all affects
		for (paf = obj->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			free_affect(paf);
		}
		obj->affected = NULL;

		// clear all flags
		obj->extra_flags = 0;
		return;
	}

	if (result <= fail)
	{
		// failed, no bad result
		send_to_char("Nothing seemed to happen.\n\r", ch);
		return;
	}

	// move all the old flags into new vectors if we have to 
	if (!obj->enchanted)
	{
		AFFECT_DATA *af_new;
		obj->enchanted = TRUE;

		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where = paf->where;
			af_new->type = UMAX(0, paf->type);
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
		}
	}

	// They have a success at this point, go ahead and set the enchanted_by
	sprintf(buf, "%s", ch->name);
	free_string(obj->enchanted_by);
	obj->enchanted_by = str_dup(buf);

	if (ch->class == ENCHANTOR_CLASS_LOOKUP && result >= (110 - level / 5))
	{
		// exceptional enchant for an enchantor
		act("$p glows a brilliant {Wwhite{x!", ch, obj, NULL, TO_CHAR);
		act("$p glows a brilliant {Wwhite{x!", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		SET_BIT(obj->extra_flags, ITEM_GLOW);
		added = 3;
	}
	else if (result <= (100 - level / 5))
	{
		// success
		act("$p glows blue.", ch, obj, NULL, TO_CHAR);
		act("$p glows blue.", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		added = 1;
	}
	else
	{
		// exceptional enchant
		act("$p glows a brillant blue!", ch, obj, NULL, TO_CHAR);
		act("$p glows a brillant blue!", ch, obj, NULL, TO_ROOM);
		SET_BIT(obj->extra_flags, ITEM_MAGIC);
		SET_BIT(obj->extra_flags, ITEM_GLOW);
		added = 2;
	}

	// add the enchantments
	if (obj->level < LEVEL_HERO - 1)
		obj->level = UMIN(LEVEL_HERO - 1, obj->level + 1);

	if (dam_found)
	{
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location == APPLY_DAMROLL)
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX(paf->level, level);
				if (paf->modifier > 4)
					SET_BIT(obj->extra_flags, ITEM_HUM);
			}
		}
	}
	else
	{
		// add a new affect
		paf = new_affect();
		paf->where = TO_OBJECT;
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_DAMROLL;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}

	if (hit_found)
	{
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location == APPLY_HITROLL)
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX(paf->level, level);
				if (paf->modifier > 4)
					SET_BIT(obj->extra_flags, ITEM_HUM);
			}
		}
	}
	else
	{
		// add a new affect
		paf = new_affect();
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->location = APPLY_HITROLL;
		paf->modifier = added;
		paf->bitvector = 0;
		paf->next = obj->affected;
		obj->affected = paf;
	}

} // end spell_enchant_weapon

/*
 * Spell that restores a faded, altered or enchanted weapon to it's original state.  This
 * will basically return it to it's stock value.
 */
void spell_restore_weapon(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    OBJ_DATA *obj2;
    char buf[MAX_STRING_LENGTH];
    int chance;

    if (obj->item_type != ITEM_WEAPON)
    {
        send_to_char("That isn't a weapon.\n\r",ch);
        return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("The weapon must be carried to be restored.\n\r",ch);
	return;
    }

    // Default, 40% won't work (20% nothing happens, 20% crumble)
    chance = number_range(1, 10);

    // bless increase chances and lowers crumbling chance
    if (IS_OBJ_STAT(obj, ITEM_BLESS))
        chance += 1;

    // Contiual light increases chance and lowers crumbling chance
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
        chance += 1;

    if (chance >= 1 && chance <= 2)
    {
        sprintf(buf, "%s crumbles into dust...", obj->short_descr);
        extract_obj(obj);
        act(buf, ch, NULL, NULL, TO_ROOM);
        act(buf, ch, NULL, NULL, TO_CHAR);
        return;
    }
    else if (chance >= 3 && chance <= 4)
    {
        send_to_char("Nothing happened.\n\r", ch);
        return;
    }
    else
    {
        obj2 = create_object( get_obj_index(obj->pIndexData->vnum), obj->pIndexData->level );
        obj_to_char( obj2, ch );
        extract_obj(obj);
        sprintf(buf, "With a {Wbright{x flash of light, %s has been restored to it's original form.", obj2->short_descr);

        if(IS_OBJ_STAT(obj2,ITEM_VIS_DEATH))
            REMOVE_BIT(obj2->extra_flags, ITEM_VIS_DEATH);

        act(buf, ch, NULL, NULL, TO_ROOM);
        act(buf, ch, NULL, NULL, TO_CHAR);
        return;
    }

} // end spell_restore_weapon

/*
 * Spell that restores a faded, altered or enchanted piece or armor  to it's original state.
 * This will basically return it to it's stock value.
 */
void spell_restore_armor(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    OBJ_DATA *obj2;
    char buf[MAX_STRING_LENGTH];
    int chance;

    if (obj->item_type != ITEM_ARMOR)
    {
	send_to_char("That isn't armor.\n\r",ch);
	return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("The armor must be carried to be restored.\n\r",ch);
	return;
    }

    // Default, 40% won't work (20% nothing happens, 20% crumble)
    chance = number_range(1, 10);

    // bless increase chances and lowers crumbling chance
    if (IS_OBJ_STAT(obj, ITEM_BLESS))
        chance += 1;

    // Contiual light increases chance and lowers crumbling chance
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
        chance += 1;

    if (chance >= 1 && chance <= 2)
    {
        sprintf(buf, "%s crumbles into dust...", obj->short_descr);
        extract_obj(obj);
        act(buf, ch, NULL, NULL, TO_ROOM);
        act(buf, ch, NULL, NULL, TO_CHAR);
        return;
    }
    else if (chance >= 3 && chance <= 4)
    {
        send_to_char("Nothing happened.\n\r", ch);
        return;
    }
    else
    {
        obj2 = create_object( get_obj_index(obj->pIndexData->vnum), obj->pIndexData->level );
        obj_to_char( obj2, ch );
        extract_obj(obj);
        sprintf(buf, "With a {Wbright{x flash of light, %s has been restored to it's original form.", obj2->short_descr);

        if(IS_OBJ_STAT(obj2,ITEM_VIS_DEATH))
            REMOVE_BIT(obj2->extra_flags, ITEM_VIS_DEATH);

        act(buf, ch, NULL, NULL, TO_ROOM);
        act(buf, ch, NULL, NULL, TO_CHAR);
        return;
    }

} // end spell_restore_armor

/*
 * Spell to disenchant a weapon or piece of armor.
 */
void spell_disenchant(int sn,int level,CHAR_DATA *ch, void *vo,int target) {
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;


    if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR)
    {
	send_to_char("That item is neither weapon or armor.\n\r",ch);
	return;
    }

    if (obj->wear_loc != -1)
    {
	send_to_char("The item must be carried to be disenchanted.\n\r",ch);
	return;
    }

    act("$p glows brightly, then fades to a dull color",ch,obj,NULL,TO_CHAR);
    act("$p glows brightly, then fades to a dull color.",ch,obj,NULL,TO_ROOM);
    obj->enchanted = TRUE;

    /* remove all affects */
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
        paf_next = paf->next;
        free_affect(paf);
    }

    obj->affected = NULL;

    /* clear all flags */
    obj->extra_flags = 0;
    if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC))
        REMOVE_BIT(obj->value[4],WEAPON_VAMPIRIC);

    if (IS_WEAPON_STAT(obj,WEAPON_FLAMING))
        REMOVE_BIT(obj->value[4],WEAPON_FLAMING);

    if (IS_WEAPON_STAT(obj,WEAPON_FROST))
        REMOVE_BIT(obj->value[4],WEAPON_FROST);

    if (IS_WEAPON_STAT(obj,WEAPON_SHOCKING))
        REMOVE_BIT(obj->value[4],WEAPON_SHOCKING);

    return;

} // end spell_disenchant

/*
 * This spell is a offshoot of locate object that just searches for Wizard Marks.  It
 * gives more detail about the objects found and has a significantly higher chance of
 * locating the objects.  This is an enchantor spell but it will be in the detection
 * group.
 */
void spell_locate_wizard_mark (int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = IS_IMMORTAL (ch) ? 200 : 2 * level;

    buffer = new_buf ();

    for (obj = object_list; obj != NULL; obj = obj->next)
    {
        // The enchantor's name
        if (!is_name (ch->name, obj->wizard_mark))
            continue;

        found = TRUE;
        number++;

        for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);
        {
            if (in_obj->carried_by != NULL)
            {
                sprintf (buf, "%s is carried by %s\n\r", in_obj->short_descr, PERS (in_obj->carried_by, ch));
            }
            else
            {
                if (IS_IMMORTAL (ch) && in_obj->in_room != NULL)
                    sprintf (buf, "%s is in %s [Room %d]\n\r",
                         in_obj->short_descr, in_obj->in_room->name, in_obj->in_room->vnum);
                else
                    sprintf (buf, "%s is in %s\n\r", in_obj->short_descr,
                         in_obj->in_room == NULL
                         ? "somewhere" : in_obj->in_room->name);
            }
        }
        buf[0] = UPPER (buf[0]);
        add_buf (buffer, buf);

        if (number >= max_found)
            break;
    }

    if (!found)
        send_to_char ("You found no wizard markings of your own in heaven or earth.\n\r", ch);
    else
        page_to_char (buf_string (buffer), ch);

    free_buf (buffer);

    return;

} // end spell_locate_wizard_mark

