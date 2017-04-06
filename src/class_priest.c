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
*  Priest Class                                                            *
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
 * Priest rank table (integer rank, the name and the hours needed to achieve
 * the rank.  The higher the priest's rank the more powerful their spells and
 * skills will be.
 */
const struct priest_rank_type priest_rank_table[] = {
    { PRIEST_RANK_NOVITIATE,   "Novitiate",      0},
    { PRIEST_RANK_DEACON,      "Deacon",       100},
    { PRIEST_RANK_PRIEST,      "Priest",       250},
    { PRIEST_RANK_BISHOP,      "Bishop",       500},
    { PRIEST_RANK_ARCHBISHOP,  "Archbishop",   750},
    { PRIEST_RANK_CARDINAL,    "Cardinal",    1000},
    { PRIEST_RANK_HIGH_PRIEST, "High Priest", 1500}
};

/*
 * Calculates the players priest rank.  This will only work for priests and
 * will return otherwise.  This will be called on tick from the update handler.
 */
void calculate_priest_rank(CHAR_DATA *ch)
{
    if (ch == NULL || IS_NPC(ch) || ch->class != PRIEST_CLASS_LOOKUP)
    {
        return;
    }

    int x;
    int hours = hours_played(ch);
    int rank = 0;

    // Find the highest rank they qualify for
    for (x = 0; x <= PRIEST_RANK_HIGH_PRIEST; x++)
    {
        if (hours >= priest_rank_table[x].hours)
        {
            rank = x;
        }
    }

    // They have attained a new rank, rank them, then congratulate them.
    if (ch->pcdata->priest_rank != rank)
    {
        ch->pcdata->priest_rank = rank;

        printf_to_char(ch, "You have attained the priest rank of %s.\r\n", priest_rank_table[rank].name);
        log_f("%s has attained the priest rank %s.", ch->name, priest_rank_table[rank].name);

        save_char_obj(ch);
    }

}

/*
 * A priest must pray to their god who will bestow upon them the ability to
 * cast the specialized spells they have been gifted.  E.g. This affect needs
 * to be on the player for them to cast their priest spells.  The priest will
 * have to time out when their prayer affect runs out while in PK.. since they
 * can't prayer back up until the pk_timer zeros out.
 */
void do_prayer(CHAR_DATA * ch, char *argument)
{
    if (ch == NULL)
    {
        return;
    }

    if (ch->class != PRIEST_CLASS_LOOKUP || IS_NPC(ch))
    {
        send_to_char("You bow your head and pray.\r\n", ch);
        act("$n bows their head.", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (is_affected(ch, gsn_prayer))
    {
        send_to_char("You are still affected by the wisdom of your god.\r\n", ch);
        return;
    }

    if (ch->pcdata->pk_timer > 0)
    {
        send_to_char("You cannot concentrate on prayer this closely after battle.\r\n", ch);
        return;
    }

    switch (ch->substate)
    {
        default:
            add_timer(ch, TIMER_DO_FUN, 8, do_prayer, 1, NULL);
            send_to_char("You kneel and pray for the blessing of wisdom.\r\n", ch);
            act("$n kneels and prays for the blessing of wisdom.", ch, NULL, NULL, TO_ROOM);
            return;
        case 1:
            // Continue onward with the prayer.
            break;
        case SUB_TIMER_DO_ABORT:
            ch->substate = SUB_NONE;
            send_to_char("Your concentration was broken before your prayers were finished.\r\n", ch);
            return;
    }

    ch->substate = SUB_NONE;

    send_to_char("A feeling of divinity overtakes your presence.\r\n", ch);
    act("A feeling of divinity overtakes the room.", ch, NULL, NULL, TO_ROOM);

    // Add the prayer affect
    AFFECT_DATA af;

    af.where = TO_AFFECTS;
    af.type = gsn_prayer;
    af.level = ch->level;
    af.duration = (ch->level / 3) + ch->pcdata->priest_rank + 1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char(ch, &af);

    return;
}

/*
 * A priest must have recently praied to their god in order to cast the spells blessed on them
 * by their deity.
 */
bool prayer_check(CHAR_DATA *ch)
{
    if (is_affected(ch, gsn_prayer))
    {
        return TRUE;
    }
    else
    {
        send_to_char("Your gods wisdom must be present to produce such a magic.\r\n", ch);
        return FALSE;
    }
}

/*
 * Agony spell will allow the priest to maladict the victim in a way that will cause
 * them damage when they recall, word of recall, teleport or gate.
 */
void spell_agony(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *)vo;
    AFFECT_DATA af;

    if (!prayer_check(ch))
    {
        return;
    }

    if (is_affected(victim, sn))
    {
        send_to_char("They are already affected by your deity's agony.\r\n", ch);
        return;
    }

    if (saves_spell(level, victim, DAM_OTHER))
    {
        send_to_char("You failed.\r\n", ch);
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.modifier = ((ch->pcdata->priest_rank + 1) * 10);
    af.location = APPLY_NONE;
    af.bitvector = 0;
    af.caster = ch;
    affect_to_char(victim, &af);

    send_to_char("You feel an agony overshadow your soul.\r\n", victim);
    act("$n has an agony overshadow their soul.", victim, NULL, NULL, TO_ROOM);

    return;
}

/*
 * This procedure will check to see if a character takes agony damage
 */
void agony_damage_check(CHAR_DATA *ch)
{
    if (ch != NULL && !IS_NPC(ch) && is_affected(ch, gsn_agony))
    {
        AFFECT_DATA *af;
        int agony_damage = 0;

        // If they are affected by agony get the affect so that we can
        // get the modifier to know how much damage to process.  If the
        // afffect for some reason is null, ditch out.
        if ((af = affect_find(ch->affected, gsn_agony)) == NULL)
        {
            return;
        }

        // If this ends up on a ghost somehow, strip it and return, they shouldn't
        // receive damage as a ghost.
        if (IS_GHOST(ch))
        {
            affect_strip(ch, gsn_agony);
            return;
        }

        // The caster must still be valid for this damage to take place.
        if (af->caster != NULL)
        {
            agony_damage = af->modifier;
            damage(af->caster, ch, agony_damage, gsn_agony, DAM_HOLY, TRUE);

            // Check if this damage is occuring with the caster and victim in a different
            // room, if it is, stop fighting, they can't physically fight if they are in
            // different places.
            if (ch->in_room != af->caster->in_room)
            {
                stop_fighting(af->caster, FALSE);
            }

            return;
        }
        else
        {
            // The pointer to the caster is gone, strip the agony affect.
            affect_strip(ch, gsn_agony);
            return;
        }
    }
}
