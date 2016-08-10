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
 *  Rogues                                                                 *
 *                                                                         *
 *  Rogue are sly individuals who prey on their victims vulnerabilities    *
 *  and borrow skills from various devious professions relating to the     *
 *  thieving trade.                                                        *
 *                                                                         *
 ***************************************************************************/

 /*
    TODO list
    Smoke Bomb
    Shiv - Stab for damage with possible blind
    Set Trap
    Bludgeon - Lower int/wis
 */

// General Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"

/*
 * A skill that allows a Rogue to prick a victim with a poison needle which
 * will lower their strength and add a poison effect although less so than
 * a poison spell.  This will land with a high efficiency and thus will have
 * a smaller effect than a poison spell which goes through saves.
 */
void do_poisonprick(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance = 0;
    char buf[MAX_STRING_LENGTH];

    // No NPC's can use this skill.
    if (IS_NPC(ch))
    {
        return;
    }

    // Must be over the level to use this skill.
    if (ch->level < skill_table[gsn_poison_prick]->skill_level[ch->class]
        || (chance = get_skill(ch, gsn_poison_prick)) == 0)
    {
        send_to_char("That is not something you are skilled at.\r\n", ch);
        return;
    }

    if (IS_NULLSTR(argument))
    {
        send_to_char("Who do you want to prick with a poison needle?\r\n", ch);
        return;
    }

    if ((victim = get_char_room(ch, argument)) == NULL)
    {
        send_to_char("They aren't here.\r\n", ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
    {
        act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (is_affected(victim, gsn_poison_prick))
    {
        send_to_char("They are already poisoned.\r\n", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        return;
    }

    // See if a WANTED flag is warranted
    check_wanted(ch, victim);

    // The chance is going to start high with it being the users skill (assuming
    // daze state doesn't lower that.  We'll add/subtract based off of that.
    chance += (ch->level - victim->level) * 2;

    // Raise or lower based off of dex difference
    chance += ((get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim, STAT_DEX)) * 2);

    if (IS_NPC(victim))
    {
        chance += 10;
    }

    // There is always some chance to succeed or fail (this will almost always be a
    // high chance skill).
    chance = URANGE( 5, chance, 95 );

    if (IS_TESTER(ch))
    {
        sprintf(buf, "[Poison Prick Chance {W%d%%{x]\r\n", chance);
        send_to_char(buf, ch);
    }

    if (!CHANCE(chance))
    {
        act("$N evades your poisoning attempt!", ch, NULL, victim, TO_CHAR);
        act("$n attempts to poison $N but fails.", ch, NULL, victim, TO_NOTVICT);
        act("You evade $n's attempt to poison you.", ch, NULL, victim, TO_VICT);

        check_improve(ch, gsn_poison_prick, FALSE, 5);

        // The time that must be waited after this command
        WAIT_STATE(ch, skill_table[gsn_poison_prick]->beats);

        return;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_poison_prick;
    af.level = ch->level / 2;
    af.duration = 1;
    af.location = APPLY_STR;

    // More strength adjustment to mobs than players.
    if (IS_NPC(victim))
    {
        af.modifier = -2;
    }
    else
    {
        af.modifier = -1;
    }

    af.bitvector = AFF_POISON;
    affect_join(victim, &af);

    act("$n poisons you, you begin to feel very ill.", ch, NULL, victim, TO_VICT);
    act("You poison $N, then begin to look very ill.", ch, NULL, victim, TO_CHAR);
    act("$n poisons $N. $N begins to look very ill.", ch, NULL, victim, TO_NOTVICT);

    check_improve(ch, gsn_poison_prick, TRUE, 5);

    // The time that must be waited after this command
    WAIT_STATE(ch, skill_table[gsn_poison_prick]->beats);

}

/*
 * Shiv skill.  This is a stabblinb skill for Rogues.  This will be similiar to backstab but with
 * less damage but can be used beyond the % requirement for hp on the victim.
 */
void do_shiv(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj;
    int chance = 0;
    int dam = 0;

    // Must be over the level to use this skill.
    if (ch->level < skill_table[gsn_shiv]->skill_level[ch->class] || (chance = get_skill(ch, gsn_shiv)) == 0)
    {
        send_to_char("That is not something you are skilled at.\r\n", ch);
        return;
    }

    if (IS_NULLSTR(argument))
    {
        victim = ch->fighting;

        if (victim == NULL)
        {
            send_to_char("But you aren't fighting anyone!\r\n", ch);
            return;
        }
    }

    if (victim == NULL)
    {
        if ((victim = get_char_room(ch, argument)) == NULL)
        {
            send_to_char("They aren't here.\r\n", ch);
            return;
        }
    }

    if (victim == ch)
    {
        send_to_char("You cannot shiv yourself.\r\n", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        return;
    }

    if (IS_NPC(victim) &&
        victim->fighting != NULL && !is_same_group(ch, victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\r\n", ch);
        return;
    }

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
    {
        send_to_char("You need to wield a primary weapon to shiv someone.\r\n", ch);
        return;
    }

    check_wanted(ch, victim);

    chance = (chance * 2) / 3; // Base starting point
    chance += ((get_curr_stat(ch, STAT_DEX) * 4) - (get_curr_stat(victim, STAT_DEX) * 4)); // Dex vs. Dex

    // Haste vs. Haste vs. Slow (dex is already factor'd but the spells will add or subtract more)
    if (IS_AFFECTED(ch, AFF_HASTE))
    {
        chance += 10;
    }

    if (IS_AFFECTED(victim, AFF_HASTE))
    {
        chance -= 10;
    }

    if (IS_AFFECTED(ch, AFF_SLOW))
    {
        chance -= 20;
    }

    if (IS_AFFECTED(victim, AFF_SLOW))
    {
        chance += 20;
    }

    chance += ((ch->level - victim->level) * 2); // Level difference factors greatly
    chance += number_range(0, 10); // Randomness

    // Great chance against mobs
    if (IS_NPC(victim))
    {
        chance += 10;
    }

    // Reduce for AC vs. Pierce
    chance += GET_AC(victim, AC_PIERCE) / 25;

    // Lag for the command
    WAIT_STATE(ch, skill_table[gsn_shiv]->beats);

    // Show the percent to testers here
    if (IS_TESTER(ch))
    {
        sprintf(buf, "[Shiv Chance {W%d%%{x]\r\n", chance);
        send_to_char(buf, ch);
    }

    // Moment of truth
    if (!CHANCE(chance))
    {
        // Fail!  *sad trombone*
        act("$n attempts to shiv you but misses.", ch, NULL, victim, TO_VICT);
        act("You attempt to shiv $N but miss.", ch, NULL, victim, TO_CHAR);
        act("$n attempts to shiv $N and misses.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, victim, 0, gsn_shiv, DAM_PIERCE, TRUE);
        check_improve(ch, gsn_shiv, FALSE, 2);
        return;
    }

    // Stab, calculate the damage
    dam = dice(obj->value[1], obj->value[2]) * (obj->level / 10);

    // More against NPC's or if the attacker is of a much greater level
    if (IS_NPC(victim) || (ch->level - victim->level) > 6)
    {
        dam += 30;
    }

    // Slight boost based on luck
    if (number_range(1, 3) == 3)
    {
        dam += 20;
    }

    // Do the damage
    damage(ch, victim, dam, gsn_shiv, DAM_PIERCE, TRUE);
    check_improve(ch, gsn_shiv, TRUE, 1);

    return;
}

/*
 * Escape command to get yourself out of battle with a high chance of
 * success.  This can only be used once every few ticks, it will require
 * a cool down period.  This will be similiar to flee except will work at
 * least once ignoring daze state (which is not something we will do for
 * other skills, this is unique and is likely a one time thing for a fight, if
 * a rogue flees and then is re-attacked it will no longer work until the cool
 * down period is over (if the cooldown is in effect, this will default to flee).
 * Closed doors will also be fleeable exits *IF* the character has pass door up.
 */
void do_escape(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;
    AFFECT_DATA af;
    int skill = 0;

    if (IS_NPC(ch))
    {
        send_to_char("Huh?\r\n", ch);
        return;
    }

    if (is_affected(ch, gsn_escape))
    {
        send_to_char("You cannot escape again so quickly.\r\n", ch);
        return;
    }

    if ((victim = ch->fighting) == NULL)
    {
        if (ch->position == POS_FIGHTING)
        {
            ch->position = POS_STANDING;
        }

        send_to_char("You aren't fighting anyone.\r\n", ch);
        return;
    }

    // We're going to check the skill this way here so it skips the daze
    // state.. this is a rare specialized skill that will avoid failing
    // because of stun... although it can still fail because you're not
    // good at it yet.
    if (ch->level < skill_table[gsn_escape]->skill_level[ch->class])
    {
        skill = 0;
    }
    else
    {
        skill = ch->pcdata->learned[gsn_escape];
    }


    if (!CHANCE(skill))
    {
        send_to_char("You stumble trying to make your escape!\r\n", ch);
        check_improve(ch, gsn_escape, FALSE, 8);
        return;
    }

    was_in = ch->in_room;
    for (attempt = 0; attempt < MAX_DIR; attempt++)
    {
        EXIT_DATA *pexit;
        int door;

        door = number_door();
        if ((pexit = was_in->exit[door]) == 0
            || pexit->u1.to_room == NULL
            || (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(ch, AFF_PASS_DOOR)))
        {
            continue;
        }

        move_char(ch, door, FALSE);

        if ((now_in = ch->in_room) == was_in)
            continue;

        ch->in_room = was_in;
        act("$n has escaped!", ch, NULL, NULL, TO_ROOM);
        ch->in_room = now_in;

        send_to_char("You have escaped from combat!\r\n", ch);

        stop_fighting(ch, TRUE);

        af.where = TO_AFFECTS;
        af.type = gsn_escape;
        af.level = 0;
        af.duration = 5;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = 0;
        affect_join(ch, &af);

        check_improve(ch, gsn_escape, TRUE, 9);

        return;
    }

    send_to_char("PANIC! You couldn't escape!\r\n", ch);
    return;
} // end do_flee

/*
 * Skill that will allow the Rogue to peer around the room with high
 * accuracy the number of other people in the room.  Even if they can't
 * see who is there they can determine if someone might be hiding.
 */
void do_peer(CHAR_DATA * ch, char *argument)
{
    int chance = 0;
    int count = 0;
    CHAR_DATA *victim;

    if (IS_NPC(ch))
    {
        send_to_char("Huh?\r\n", ch);
        return;
    }

    if (ch->fighting != NULL)
    {
        send_to_char("You're too busy fighting\r\n", ch);
        return;
    }

    // Must be over the level to use this skill.
    if (ch->level < skill_table[gsn_peer]->skill_level[ch->class]
        || (chance = get_skill(ch, gsn_peer)) == 0)
    {
        send_to_char("That is not something you are skilled at.\r\n", ch);
        return;
    }

    // There is always some chance to succeed or fail (this will almost always be a
    // high chance skill).
    chance = URANGE( 5, chance, 95 );

    if (IS_TESTER(ch))
    {
        printf_to_char(ch, "[Peer Chance {W%d%%{x]\r\n", chance);
    }

    act("$N peers around the immediate area.", ch, NULL, NULL, TO_ROOM);

    // The time that must be waited after this command
    WAIT_STATE(ch, skill_table[gsn_peer]->beats);

    if (!CHANCE(chance))
    {
        check_improve(ch, gsn_peer, FALSE, 3);
        send_to_char("You do not see traces of anyone as you peer around the area.\r\n", ch);
        return;
    }

    // Get the count of eligble people in the room, which is everyone but immortals.
    for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
    {
        if (!IS_IMMORTAL(victim) && victim != ch)
        {
            count++;
        }
    }

    if (count > 0)
    {
        printf_to_char(ch, "You see signs of %d other individual%s here.\r\n", count, count > 1 ? "s" : "");
        check_improve(ch, gsn_peer, TRUE, 3);
    }
    else
    {
        check_improve(ch, gsn_peer, TRUE, 3);
        send_to_char("You do not see traces of anyone as you peer around the area.\r\n", ch);
    }

    return;
}
