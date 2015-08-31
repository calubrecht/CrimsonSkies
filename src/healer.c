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
 *  Healer Class (not to be confused with a healer mob who you can buy     *
 *  healing spels from)                                                    *
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

/*
 * Mass refresh will refresh every visible character in the room for only
 * 3 more mana than a normal refresh.  It also has an additional 1 to 10
 * movement random bonus on top of the normal refresh.
 */
void spell_mass_refresh (int sn, int level, CHAR_DATA * ch, void *vo,
                         int target)
{
    CHAR_DATA *gch;
    char buf[MAX_STRING_LENGTH];

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        // If the character can't be seen they can't be refreshed, we don't
        // want this used to sniff out hidden characters.
        if (!can_see (ch, gch))
        {
            continue;
        }

        gch->move = UMIN(gch->move + level + number_range(1, 10), gch->max_move);

        if (gch->max_move == gch->move)
        {
            send_to_char("You feel fully refreshed!\n\r", gch);
        }
        else
        {
            send_to_char("You feel less tired.\n\r", gch);
        }

        if (gch != ch)
        {
            sprintf(buf, "%s has been refreshed.\n\r", gch->name);
            send_to_char(buf, ch);
        }

    }

} // end spell_mass_refresh

/*
 * A healing spell that will return small bits of health over the period of a few ticks
 * outside of the normal tick cycle.  This will cost half as much as a heal (because it
 * can only be cast once and is spread out.  Players will get 10hp every half tick if
 * they are below their max health.
 */
void spell_healing_presence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn))
    {
        if (victim == ch)
        {
            // Remove the affect so it can be re-added to yourself
            affect_strip (victim, sn);
        }
        else
        {
            act("$N is already affected by the healing presence.",ch,NULL,victim,TO_CHAR);
            return;
        }
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 5;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You are vitalized with a healing presence.\n\r", victim );

    if ( ch != victim )
    {
        act("$N is vitalized with a healing presence.",ch,NULL,victim,TO_CHAR);
    }

    return;

} // end spell_delayed_heal

/*
 * Allows a healer to boost the life force of a recipient (e.g. increase their
 * max health points and movement for a level's worth of ticks).  The modifier
 * (how much hp and move they receive) will be calculated by the healer's casting
 * level.  This cannot be cast on NPC's as a way to make them stronger.
 */
void spell_life_boost( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    // Not on NPC's
    if (IS_NPC(victim))
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }

    if (is_affected(victim, sn))
    {
        if (victim == ch)
        {
            // Remove the affect so it can be re-added to yourself
            affect_strip (victim, sn);
        }
        else
        {
            act("$N is already affected by the increased vitality.",ch,NULL,victim,TO_CHAR);
            return;
        }
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = ch->level;
    af.location  = APPLY_HIT;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location = APPLY_MOVE;
    affect_to_char (victim, &af);

    send_to_char( "You feel an increased vitality.\n\r", victim );

    if ( ch != victim )
    {
        act("$N has been vitalized.",ch,NULL,victim,TO_CHAR);
    }

    return;
}

/*
 * A spell to help the healer resist some offensive magics against it.  Healer's don't
 * have many offensive weapons and thus are vulnerable characters, this should help
 * at least protect them a little more from spells.  This should be set to target
 * char_self
 */
void spell_magic_resistance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn))
    {
        affect_strip(victim, sn);
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.modifier  = (ch->level / 10) * -1;
    af.location  = APPLY_SAVING_SPELL;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "You feel an enhanced resistance to magic.\n\r", victim );

    if ( ch != victim )
    {
        act("$N has an enhanced resistance to magic.",ch,NULL,victim,TO_CHAR);
    }

    return;

} // end magic resistance

/*
 * Mana transfer will allow the healer to transfer mana from themselves to a
 * recipient.  We'll start at 50 for casting and do a 1 to 1 transfer for now.
 */
void spell_mana_transfer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (IS_NPC(victim))
    {
        send_to_char("You cannot cast mana transfer on them.\n\r", ch);
        return;
    }

    if (victim->mana == victim->max_mana)
    {
        send_to_char("Their mana is at full capacity.\n\r", ch);
        return;
    }

    victim->mana = UMIN (victim->mana + 50, victim->max_mana);

    send_to_char ("You feel a surge of mana course through your body.\n\r", victim);

    // Show the caster it's done
    if (ch != victim)
    {
        if (victim->mana == victim->max_mana)
        {
            // The caster is now at max with that cast, let the caster know.
            send_to_char("Their mana is now at full capacity.\n\r", ch);
        }
        else
        {
            send_to_char ("Ok.\n\r", ch);
        }
    }

    return;
} // end spell_mana_transfer
