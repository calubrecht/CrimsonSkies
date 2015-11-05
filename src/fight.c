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
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

/*
 * Local functions.
 */
void check_assist args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_dodge args ((CHAR_DATA * ch, CHAR_DATA * victim));
void check_wanted args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_parry args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool check_shield_block args ((CHAR_DATA * ch, CHAR_DATA * victim));
void dam_message args ((CHAR_DATA * ch, CHAR_DATA * victim, int dam,
                        int dt, bool immune));
void death_cry args ((CHAR_DATA * ch));
void group_gain args ((CHAR_DATA * ch, CHAR_DATA * victim));
int  xp_compute args ((CHAR_DATA * gch, CHAR_DATA * victim, int total_levels));
bool is_safe args ((CHAR_DATA * ch, CHAR_DATA * victim));
void make_corpse args ((CHAR_DATA * ch));
void one_hit args ((CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool dual));
void mob_hit args ((CHAR_DATA * ch, CHAR_DATA * victim, int dt));
void raw_kill args ((CHAR_DATA * victim));
void set_fighting args ((CHAR_DATA * ch, CHAR_DATA * victim));
void disarm args ((CHAR_DATA * ch, CHAR_DATA * victim));
void toast args (( CHAR_DATA *ch, CHAR_DATA *victim));

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update (void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        ch_next = ch->next;

        if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
            continue;

        if (IS_AWAKE (ch) && ch->in_room == victim->in_room)
            multi_hit (ch, victim, TYPE_UNDEFINED);
        else
            stop_fighting (ch, FALSE);

        if ((victim = ch->fighting) == NULL)
            continue;

        // Fun for the whole family!
        check_assist (ch, victim);

        if (IS_NPC (ch))
        {
            if (HAS_TRIGGER (ch, TRIG_FIGHT))
                mp_percent_trigger (ch, victim, NULL, NULL, TRIG_FIGHT);
            if (HAS_TRIGGER (ch, TRIG_HPCNT))
                mp_hprct_trigger (ch, victim);
        }
    }

    return;
} // end void violence_update

/*
 * Auto assisting
 */
void check_assist (CHAR_DATA * ch, CHAR_DATA * victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
        rch_next = rch->next_in_room;

        if (IS_AWAKE (rch) && rch->fighting == NULL)
        {

            /* quick check for ASSIST_PLAYER */
            if (!IS_NPC (ch) && IS_NPC (rch)
                && IS_SET (rch->off_flags, ASSIST_PLAYERS)
                && rch->level + 6 > victim->level)
            {
                do_function (rch, &do_emote, "screams and attacks!");
                multi_hit (rch, victim, TYPE_UNDEFINED);
                continue;
            }

            /* PCs next */
            if (!IS_NPC (ch) || IS_AFFECTED (ch, AFF_CHARM))
            {
                if (((!IS_NPC (rch) && IS_SET (rch->act, PLR_AUTOASSIST))
                     || IS_AFFECTED (rch, AFF_CHARM))
                    && is_same_group (ch, rch) && !is_safe (rch, victim))
                    multi_hit (rch, victim, TYPE_UNDEFINED);

                continue;
            }

            /* now check the NPC cases */

            if (IS_NPC (ch) && !IS_AFFECTED (ch, AFF_CHARM))
            {
                if ((IS_NPC (rch) && IS_SET (rch->off_flags, ASSIST_ALL))
                    || (IS_NPC (rch) && rch->group && rch->group == ch->group)
                    || (IS_NPC (rch) && rch->race == ch->race
                        && IS_SET (rch->off_flags, ASSIST_RACE))
                    || (IS_NPC (rch) && IS_SET (rch->off_flags, ASSIST_ALIGN)
                        && ((IS_GOOD (rch) && IS_GOOD (ch))
                            || (IS_EVIL (rch) && IS_EVIL (ch))
                            || (IS_NEUTRAL (rch) && IS_NEUTRAL (ch))))
                    || (rch->pIndexData == ch->pIndexData
                        && IS_SET (rch->off_flags, ASSIST_VNUM)))
                {
                    CHAR_DATA *vch;
                    CHAR_DATA *target;
                    int number;

                    if (number_bits (1) == 0)
                        continue;

                    target = NULL;
                    number = 0;
                    for (vch = ch->in_room->people; vch; vch = vch->next)
                    {
                        if (can_see (rch, vch)
                            && is_same_group (vch, victim)
                            && number_range (0, number) == 0)
                        {
                            target = vch;
                            number++;
                        }
                    }

                    if (target != NULL)
                    {
                        do_function (rch, &do_emote, "screams and attacks!");
                        multi_hit (rch, target, TYPE_UNDEFINED);
                    }
                }
            }
        }
    }
} // end void check_assist

/*
 * Do one group of attacks.
 */
void multi_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance;

    /* decrement the wait */
    if (ch->desc == NULL)
        ch->wait = UMAX (0, ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
        ch->daze = UMAX (0, ch->daze - PULSE_VIOLENCE);


    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
        return;

    if (IS_NPC (ch))
    {
        mob_hit (ch, victim, dt);
        return;
    }

    // Attack 1 (Everyone gets this)
    one_hit (ch, victim, dt, FALSE);

    // Death check to see if fighting is over
    if (ch->fighting != victim)
        return;

    // Attack 2 (Haste)
    if (IS_AFFECTED (ch, AFF_HASTE))
        one_hit (ch, victim, dt, FALSE);

    // Death check
    if (ch->fighting != victim || dt == gsn_backstab)
        return;

    // Attack 3 (2nd attack skill)
    chance = get_skill (ch, gsn_second_attack) / 2;

    if (IS_AFFECTED (ch, AFF_SLOW))
        chance /= 2;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        check_improve (ch, gsn_second_attack, TRUE, 5);
        if (ch->fighting != victim)
            return;
    }

    // Attack 4 (3rd attack skill)
    chance = get_skill (ch, gsn_third_attack) / 4;

    if (IS_AFFECTED (ch, AFF_SLOW))
        chance = 0;;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        check_improve (ch, gsn_third_attack, TRUE, 6);
        if (ch->fighting != victim)
            return;
    }

    // Attack 5 (Dual Wield #1)
    if (number_percent() < get_skill(ch,gsn_dual_wield) )
    {
        one_hit( ch, victim, dt, TRUE );
    }

    if (ch->fighting != victim || dt == gsn_backstab)
        return;

    // Attack 6 (Dual Wield #2  + Haste)
    if (IS_AFFECTED (ch, AFF_HASTE) &&
         number_percent() < get_skill(ch,gsn_dual_wield))
    {
        one_hit( ch, victim, dt, TRUE );
    }

    // Attack 7 (Bladesong)
    if (is_affected(ch, gsn_bladesong))
    {
        one_hit(ch, victim, dt, FALSE );
    }

    if (ch->fighting != victim || dt == gsn_backstab)
        return;

    return;
} // end void multi_hit

/*
 * Procedure for all mobile attacks
 */
void mob_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
    int chance, number;
    CHAR_DATA *vch, *vch_next;

    one_hit (ch, victim, dt, FALSE);

    if (ch->fighting != victim)
        return;

    /* Area attack -- BALLS nasty! */

    if (IS_SET (ch->off_flags, OFF_AREA_ATTACK))
    {
        for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
        {
            vch_next = vch->next;
            if ((vch != victim && vch->fighting == ch))
                one_hit (ch, vch, dt, FALSE);
        }
    }

    if (IS_AFFECTED (ch, AFF_HASTE)
        || (IS_SET (ch->off_flags, OFF_FAST) && !IS_AFFECTED (ch, AFF_SLOW)))
        one_hit (ch, victim, dt, FALSE);

    if (ch->fighting != victim || dt == gsn_backstab)
        return;

    chance = get_skill (ch, gsn_second_attack) / 2;

    if (IS_AFFECTED (ch, AFF_SLOW) && !IS_SET (ch->off_flags, OFF_FAST))
        chance /= 2;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        if (ch->fighting != victim)
            return;
    }

    chance = get_skill (ch, gsn_third_attack) / 4;

    if (IS_AFFECTED (ch, AFF_SLOW) && !IS_SET (ch->off_flags, OFF_FAST))
        chance = 0;

    if (number_percent () < chance)
    {
        one_hit (ch, victim, dt, FALSE);
        if (ch->fighting != victim)
            return;
    }

    /* oh boy!  Fun stuff! */
    if (ch->wait > 0)
        return;

    number = number_range (0, 2);

    if (number == 1 && IS_SET (ch->act, ACT_MAGE))
    {
        /*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if (number == 2 && IS_SET (ch->act, ACT_CLERIC))
    {
        /* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */
    number = number_range (0, 8);

    switch (number)
    {
        case (0):
            if (IS_SET (ch->off_flags, OFF_BASH))
                do_function (ch, &do_bash, "");
            break;

        case (1):
            if (IS_SET (ch->off_flags, OFF_BERSERK)
                && !IS_AFFECTED (ch, AFF_BERSERK))
                do_function (ch, &do_berserk, "");
            break;


        case (2):
            if (IS_SET (ch->off_flags, OFF_DISARM)
                || (get_weapon_sn (ch, FALSE) != gsn_hand_to_hand
                    && (IS_SET (ch->act, ACT_WARRIOR)
                        || IS_SET (ch->act, ACT_THIEF))))
                do_function (ch, &do_disarm, "");
            break;

        case (3):
            if (IS_SET (ch->off_flags, OFF_KICK))
                do_function (ch, &do_kick, "");
            break;

        case (4):
            if (IS_SET (ch->off_flags, OFF_KICK_DIRT))
                do_function (ch, &do_dirt, "");
            break;

        case (5):
            if (IS_SET (ch->off_flags, OFF_TAIL))
            {
                /* do_function(ch, &do_tail, "") */ ;
            }
            break;

        case (6):
            if (IS_SET (ch->off_flags, OFF_TRIP))
                do_function (ch, &do_trip, "");
            break;

        case (7):
            if (IS_SET (ch->off_flags, OFF_CRUSH))
            {
                /* do_function(ch, &do_crush, "") */ ;
            }
            break;
        case (8):
            if (IS_SET (ch->off_flags, OFF_BACKSTAB))
            {
                do_function (ch, &do_backstab, "");
            }
    }
} // end void mob_hit


/*
 * Hit one guy once.
 */
void one_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool dual)
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn, skill;
    int dam_type;
    bool result;

    sn = -1;

    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
        return;

    // Grab the weapon for this hit
    if (!dual)
    {
        wield = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
        wield = get_eq_char( ch, WEAR_SECONDARY_WIELD );

        // If it's dual wield and they're not wearing a second weapon get out.
        if (!wield)
            return;

        // If it's dual wield, check to see if the player gets better at it
        check_improve(ch, gsn_dual_wield, TRUE, 30);
    }

    /*
     * Figure out the type of damage message.
     */

    if (dt == TYPE_UNDEFINED)
    {
        dt = TYPE_HIT;
        if (wield != NULL && wield->item_type == ITEM_WEAPON)
            dt += wield->value[3];
        else
            dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
        if (wield != NULL)
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
        dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn (ch, dual);
    skill = 20 + get_weapon_skill (ch, sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if (IS_NPC (ch))
    {
        thac0_00 = 20;
        thac0_32 = -4;            /* as good as a thief */
        if (IS_SET (ch->act, ACT_WARRIOR))
            thac0_32 = -10;
        else if (IS_SET (ch->act, ACT_THIEF))
            thac0_32 = -4;
        else if (IS_SET (ch->act, ACT_CLERIC))
            thac0_32 = 2;
        else if (IS_SET (ch->act, ACT_MAGE))
            thac0_32 = 6;
    }
    else
    {
        thac0_00 = class_table[ch->class]->thac0_00;
        thac0_32 = class_table[ch->class]->thac0_32;
    }
    thac0 = interpolate (ch->level, thac0_00, thac0_32);

    if (thac0 < 0)
        thac0 = thac0 / 2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL (ch) * skill / 100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
        thac0 -= 10 * (100 - get_skill (ch, gsn_backstab));

    switch (dam_type)
    {
        case (DAM_PIERCE):
            victim_ac = GET_AC (victim, AC_PIERCE) / 10;
            break;
        case (DAM_BASH):
            victim_ac = GET_AC (victim, AC_BASH) / 10;
            break;
        case (DAM_SLASH):
            victim_ac = GET_AC (victim, AC_SLASH) / 10;
            break;
        default:
            victim_ac = GET_AC (victim, AC_EXOTIC) / 10;
            break;
    };

    if (victim_ac < -15)
        victim_ac = (victim_ac + 15) / 5 - 15;

    if (!can_see (ch, victim))
        victim_ac -= 4;

    if (victim->position < POS_FIGHTING)
        victim_ac += 4;

    if (victim->position < POS_RESTING)
        victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ((diceroll = number_bits (5)) >= 20);

    if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac))
    {
        /* Miss. */
        damage (ch, victim, 0, dt, dam_type, TRUE);
        tail_chain ();
        return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if (IS_NPC (ch) && wield == NULL)
    { 
        dam = dice (ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);
    }
    else
    {
        if (sn != -1)
            check_improve (ch, sn, TRUE, 5);
        if (wield != NULL)
        {
            dam = dice (wield->value[1], wield->value[2]) * skill / 100;

            if (get_eq_char (ch, WEAR_SHIELD) == NULL)    /* no shield = more */
                dam = dam * 11 / 10;

            /* sharpness! */
            if (IS_WEAPON_STAT (wield, WEAPON_SHARP))
            {
                int percent;

                if ((percent = number_percent ()) <= (skill / 8))
                    dam = 2 * dam + (dam * 2 * percent / 100);
            }
        }
        else
            dam =
                number_range (1 + 4 * skill / 100,
                              2 * ch->level / 3 * skill / 100);
    }

    /*
     * Bonuses.
     */
    if (get_skill (ch, gsn_enhanced_damage) > 0)
    {
        diceroll = number_percent ();
        if (diceroll <= get_skill (ch, gsn_enhanced_damage))
        {
            check_improve (ch, gsn_enhanced_damage, TRUE, 6);
            dam += 2 * (dam * diceroll / 300);
        }
    }

    if (!IS_AWAKE (victim))
        dam *= 2;
    else if (victim->position < POS_FIGHTING)
        dam = dam * 3 / 2;

    if (dt == gsn_backstab && wield != NULL)
    {
        if (wield->value[0] != 2)
            dam *= 2 + (ch->level / 10);
        else
            dam *= 2 + (ch->level / 8);
    }

    dam += GET_DAMROLL (ch) * UMIN (100, skill) / 100;

    if (dam <= 0)
        dam = 1;

    result = damage (ch, victim, dam, dt, dam_type, TRUE);

    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    {
        int dam;

        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_POISON))
        {
            int level;
            AFFECT_DATA *poison, af;

            if ((poison = affect_find (wield->affected, gsn_poison)) == NULL)
                level = wield->level;
            else
                level = poison->level;

            if (!saves_spell (level / 2, victim, DAM_POISON))
            {
                send_to_char ("You feel poison coursing through your veins.",
                              victim);
                act ("$n is poisoned by the venom on $p.",
                     victim, wield, NULL, TO_ROOM);

                af.where = TO_AFFECTS;
                af.type = gsn_poison;
                af.level = level * 3 / 4;
                af.duration = level / 2;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = AFF_POISON;
                affect_join (victim, &af);
            }

            /* weaken the poison if it's temporary */
            if (poison != NULL)
            {
                poison->level = UMAX (0, poison->level - 2);
                poison->duration = UMAX (0, poison->duration - 1);

                if (poison->level == 0 || poison->duration == 0)
                    act ("The poison on $p has worn off.", ch, wield, NULL,
                         TO_CHAR);
            }
        }


        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_VAMPIRIC))
        {
            dam = number_range (1, wield->level / 5 + 1);
            act ("$p draws life from $n.", victim, wield, NULL, TO_ROOM);
            act ("You feel $p drawing your life away.",
                 victim, wield, NULL, TO_CHAR);
            damage (ch, victim, dam, 0, DAM_NEGATIVE, FALSE);
            ch->hit += dam / 2;
        }

        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_FLAMING))
        {
            dam = number_range (1, wield->level / 4 + 1);
            act ("$n is burned by $p.", victim, wield, NULL, TO_ROOM);
            act ("$p sears your flesh.", victim, wield, NULL, TO_CHAR);
            fire_effect ((void *) victim, wield->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_FIRE, FALSE);
        }

        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_FROST))
        {
            dam = number_range (1, wield->level / 6 + 2);
            act ("$p freezes $n.", victim, wield, NULL, TO_ROOM);
            act ("The cold touch of $p surrounds you with ice.",
                 victim, wield, NULL, TO_CHAR);
            cold_effect (victim, wield->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_COLD, FALSE);
        }

        if (ch->fighting == victim && IS_WEAPON_STAT (wield, WEAPON_SHOCKING))
        {
            dam = number_range (1, wield->level / 5 + 2);
            act ("$n is struck by lightning from $p.", victim, wield, NULL, TO_ROOM);
            act ("You are shocked by $p.", victim, wield, NULL, TO_CHAR);
            shock_effect (victim, wield->level / 2, dam, TARGET_CHAR);
            damage (ch, victim, dam, 0, DAM_LIGHTNING, FALSE);
        }
    }
    tail_chain ();
    return;
} // end void one_hit

/*
 * Inflict damage from a hit.
 */
bool damage (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
             int dam_type, bool show)
{
    OBJ_DATA *corpse;
    bool immune;

    if (victim->position == POS_DEAD)
        return FALSE;

    /*
     * Stop up any residual loopholes.
     */
    if (dam > 1200 && dt >= TYPE_HIT)
    {
        bug ("Damage: %d: more than 1200 points!", dam);
        dam = 1200;
        if (!IS_IMMORTAL (ch))
        {
            OBJ_DATA *obj;
            obj = get_eq_char (ch, WEAR_WIELD);
            send_to_char ("You really shouldn't cheat.\n\r", ch);
            if (obj != NULL)
                extract_obj (obj);
        }

    }

    /* damage reduction */
    if (dam > 35)
        dam = (dam - 35) / 2 + 35;
    if (dam > 80)
        dam = (dam - 80) / 2 + 80;

    if (victim != ch)
    {
        /*
         * Certain attacks are forbidden.
         * Most other attacks are returned.
         */
        if (is_safe (ch, victim))
            return FALSE;
        check_wanted (ch, victim);

        if (victim->position > POS_STUNNED)
        {
            if (victim->fighting == NULL)
            {
                set_fighting (victim, ch);
                if (IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_KILL))
                    mp_percent_trigger (victim, ch, NULL, NULL, TRIG_KILL);
            }
            if (victim->timer <= 4)
                victim->position = POS_FIGHTING;
        }

        if (victim->position > POS_STUNNED)
        {
            if (ch->fighting == NULL)
                set_fighting (ch, victim);
        }

        /*
         * More charm stuff.
         */
        if (victim->master == ch)
            stop_follower (victim);
    }

    /*
     * Inviso attacks ... not.
     */
    if (IS_AFFECTED (ch, AFF_INVISIBLE))
    {
        affect_strip (ch, gsn_invis);
        affect_strip (ch, gsn_mass_invis);
        REMOVE_BIT (ch->affected_by, AFF_INVISIBLE);
        act ("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
    }

    /*
     * Damage modifiers.
     */

    if (dam > 1 && !IS_NPC (victim)
        && victim->pcdata->condition[COND_DRUNK] > 10)
        dam = 9 * dam / 10;

    if (dam > 1 && IS_AFFECTED (victim, AFF_SANCTUARY))
        dam /= 2;

    if (dam > 1 && ((IS_AFFECTED (victim, AFF_PROTECT_EVIL) && IS_EVIL (ch))
                    || (IS_AFFECTED (victim, AFF_PROTECT_GOOD)
                        && IS_GOOD (ch))))
        dam -= dam / 4;

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if (dt >= TYPE_HIT && ch != victim)
    {
        if (check_parry (ch, victim))
            return FALSE;
        if (check_dodge (ch, victim))
            return FALSE;
        if (check_shield_block (ch, victim))
            return FALSE;

    }

    switch (check_immune (victim, dam_type))
    {
        case (IS_IMMUNE):
            immune = TRUE;
            dam = 0;
            break;
        case (IS_RESISTANT):
            dam -= dam / 3;
            break;
        case (IS_VULNERABLE):
            dam += dam / 2;
            break;
    }

    // Lightning/shockers cause lots of damage in the water or under water.  This doubles
    // the damage and has to happen before the dam_message is displayed so it displays
    // correctly (later on, we'll spread the shock through the water).
    if( dam_type == DAM_LIGHTNING &&
        victim->in_room != NULL &&
        (victim->in_room->sector_type == SECT_UNDERWATER ||
         victim->in_room->sector_type == SECT_OCEAN))
    {
        dam *= 2;
    }

    if (show)
        dam_message (ch, victim, dam, dt, immune);

    // We want the shock to spread even if the initial person is immune to lightning.  This will
    // only fire if the setting for shock_spread is set.
    if( dam_type == DAM_LIGHTNING &&
        victim->in_room != NULL &&
        settings.shock_spread &&
        (victim->in_room->sector_type == SECT_UNDERWATER ||
         victim->in_room->sector_type == SECT_OCEAN))
    {
        // Spread the shocking effect... one level.. we'll have to change this
        // from DAM shocking so we don't endlessly loop on ourselves with shocks
        // since we're recalling damage from here.
        CHAR_DATA * rch;

        for (rch = ch->in_room->people; rch; rch = rch->next_in_room)
        {
            // Skip under these circumstances, same group, not in the same room, can't
            // be the initiator of the attack or the initial victim (they've already been hit).
            // We also don't want to hit immortals that might be in the room.
            if (is_same_group (rch, ch) ||
                rch->in_room != ch->in_room ||
                rch == ch ||
                rch == victim ||
                IS_IMMORTAL(rch))
            {
                continue;
            }

            // 25 damage for the spread, not lightning to stop an endless loop
            act ("The electrical shock spreads through the water to $n.", rch, NULL, NULL, TO_ROOM);
            send_to_char ("The electrical shock spreads through the water to you.\n\r", rch);
            damage (ch, rch, 25, TYPE_UNDEFINED, DAM_NONE, FALSE);
        }

    }

    if (dam == 0)
        return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if (!IS_NPC (victim)
        && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
        victim->hit = 1;
    update_pos (victim);

    switch (victim->position)
    {
        case POS_MORTAL:
            act ("$n is mortally wounded, and will die soon, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are mortally wounded, and will die soon, if not aided.\n\r",
                 victim);
            break;

        case POS_INCAP:
            act ("$n is incapacitated and will slowly die, if not aided.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char
                ("You are incapacitated and will slowly die, if not aided.\n\r",
                 victim);
            break;

        case POS_STUNNED:
            act ("$n is stunned, but will probably recover.",
                 victim, NULL, NULL, TO_ROOM);
            send_to_char ("You are stunned, but will probably recover.\n\r",
                          victim);
            break;

        case POS_DEAD:
            act ("$n is {RDEAD{x!!", victim, 0, 0, TO_ROOM);
            send_to_char ("You have been {RKILLED{x!!{x\n\r\n\r", victim);
            break;

        default:
            if (dam > victim->max_hit / 4)
                send_to_char ("That really did {RHURT{x!{x\n\r", victim);
            if (victim->hit < victim->max_hit / 4)
                send_to_char ("You sure are {RBLEEDING{x!{x\n\r", victim);
            break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE (victim))
        stop_fighting (victim, FALSE);

    /*
     * Payoff for killing things.
     */
    if (victim->position == POS_DEAD)
    {
        group_gain(ch, victim);
        char buf[MAX_STRING_LENGTH];

        if (!IS_NPC (victim))
        {
            sprintf (buf, "%s killed by %s at %d",
                     victim->name,
                     (IS_NPC (ch) ? ch->short_descr : ch->name),
                     ch->in_room->vnum);
            log_string(buf);

            /*
             * Dying penalty:
             * 2/3 way back to previous level.
             */
            if (victim->exp > exp_per_level (victim, victim->pcdata->points)
                * victim->level)
                gain_exp (victim,
                          (2 *
                           (exp_per_level (victim, victim->pcdata->points) *
                            victim->level - victim->exp) / 3) + 50);
        }

        sprintf (buf, "%s got toasted by %s at %s [room %d]",
                 (IS_NPC (victim) ? victim->short_descr : victim->name),
                 (IS_NPC (ch) ? ch->short_descr : ch->name),
                 ch->in_room->name, ch->in_room->vnum);

        if (IS_NPC (victim))
            wiznet(buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
        else
            wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);

        // Send a toast message to the players.
        if (!IS_NPC(ch) && !IS_NPC(victim))
            toast(ch, victim);

        /*
         * Death trigger
         */
        if (IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_DEATH))
        {
            victim->position = POS_STANDING;
            mp_percent_trigger (victim, ch, NULL, NULL, TRIG_DEATH);
        }

        raw_kill (victim);
        /* dump the flags */
        if (ch != victim && !IS_NPC (ch) && !is_same_clan (ch, victim))
        {
            if (IS_SET (victim->act, PLR_WANTED))
                REMOVE_BIT (victim->act, PLR_WANTED);
        }

        /* RT new auto commands */

        if (!IS_NPC (ch)
            && (corpse =
                get_obj_list (ch, "corpse", ch->in_room->contents)) != NULL
            && corpse->item_type == ITEM_CORPSE_NPC
            && can_see_obj (ch, corpse))
        {
            OBJ_DATA *coins;

            corpse = get_obj_list (ch, "corpse", ch->in_room->contents);

            if (IS_SET (ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)
            {                    /* exists and not empty */
                do_function (ch, &do_get, "all corpse");
            }

            if (IS_SET (ch->act, PLR_AUTOGOLD) && corpse && corpse->contains &&    /* exists and not empty */
                !IS_SET (ch->act, PLR_AUTOLOOT))
            {
                if ((coins = get_obj_list (ch, "gcash", corpse->contains))
                    != NULL)
                {
                    do_function (ch, &do_get, "all.gcash corpse");
                }
            }

            if (IS_SET (ch->act, PLR_AUTOSAC))
            {
                if (IS_SET (ch->act, PLR_AUTOLOOT) && corpse
                    && corpse->contains)
                {
                    return TRUE;    /* leave if corpse has treasure */
                }
                else
                {
                    do_function (ch, &do_sacrifice, "corpse");
                }
            }
        }

        return TRUE;
    }

    if (victim == ch)
        return TRUE;

    /*
     * Take care of link dead people.
     */
    if (!IS_NPC (victim) && victim->desc == NULL)
    {
        if (number_range (0, victim->wait) == 0)
        {
            do_function (victim, &do_recall, "");
            return TRUE;
        }
    }

    /*
     * Wimp out?
     */
    if (IS_NPC (victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
        if ((IS_SET (victim->act, ACT_WIMPY) && number_bits (2) == 0
             && victim->hit < victim->max_hit / 5)
            || (IS_AFFECTED (victim, AFF_CHARM) && victim->master != NULL
                && victim->master->in_room != victim->in_room))
        {
            do_function (victim, &do_flee, "");
        }
    }

    if (!IS_NPC (victim)
        && victim->hit > 0
        && victim->hit <= victim->wimpy && victim->wait < PULSE_VIOLENCE / 2)
    {
        do_function (victim, &do_flee, "");
    }

    tail_chain ();
    return TRUE;
} // bool damage

/*
 * Whether one player can attack another.
 */
bool is_safe (CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim->fighting == ch || victim == ch)
        return FALSE;

    if (IS_IMMORTAL (ch) && ch->level > LEVEL_IMMORTAL)
        return FALSE;

    /* killing mobiles */
    if (IS_NPC (victim))
    {

        /* safe room? */
        if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
        {
            send_to_char ("Not in this room.\n\r", ch);
            return TRUE;
        }

        if (victim->pIndexData->pShop != NULL)
        {
            send_to_char ("The shopkeeper wouldn't like that.\n\r", ch);
            return TRUE;
        }

        /* no killing healers, trainers, etc */
        if (IS_SET (victim->act, ACT_TRAIN)
            || IS_SET (victim->act, ACT_PRACTICE)
            || IS_SET (victim->act, ACT_IS_HEALER)
            || IS_SET (victim->act, ACT_IS_CHANGER))
        {
            send_to_char ("I don't think the gods would approve.\n\r", ch);
            return TRUE;
        }

        if (!IS_NPC (ch))
        {
            /* no pets */
            if (IS_SET (victim->act, ACT_PET))
            {
                act ("But $N looks so cute and cuddly...",
                     ch, NULL, victim, TO_CHAR);
                return TRUE;
            }

            /* no charmed creatures unless owner */
            if (IS_AFFECTED (victim, AFF_CHARM) && ch != victim->master)
            {
                send_to_char ("You don't own that monster.\n\r", ch);
                return TRUE;
            }
        }
    }
    /* killing players */
    else
    {
        /* NPC doing the killing */
        if (IS_NPC (ch))
        {
            /* safe room check */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            {
                send_to_char ("Not in this room.\n\r", ch);
                return TRUE;
            }

            /* charmed mobs and pets cannot attack players while owned */
            if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
                && ch->master->fighting != victim)
            {
                send_to_char ("Players are your friends!\n\r", ch);
                return TRUE;
            }
        }
        /* player doing the killing */
        else
        {
            // All bets are off in the arena, let the best player win.
            if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
            {
                return FALSE;
            }

            if (!is_clan (ch))
            {
                send_to_char ("Join a clan if you want to kill players.\n\r",
                              ch);
                return TRUE;
            }

            if (IS_SET (victim->act, PLR_WANTED))
                return FALSE;

            if (!is_clan (victim))
            {
                act ("$N is not in a clan, leave them alone!", ch, NULL, victim, TO_CHAR);
                return TRUE;
            }

            if (ch->level > victim->level + 8)
            {
                send_to_char ("Pick on someone your own size.\n\r", ch);
                return TRUE;
            }
        }
    }
    return FALSE;
} // end bool is_safe

/*
 * Whether a player is safe from a spell.
 */
bool is_safe_spell (CHAR_DATA * ch, CHAR_DATA * victim, bool area)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
        return TRUE;

    if (victim->fighting == ch || victim == ch)
        return FALSE;

    if (IS_IMMORTAL (ch) && ch->level > LEVEL_IMMORTAL && !area)
        return FALSE;

    /* killing mobiles */
    if (IS_NPC (victim))
    {
        /* safe room? */
        if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
            return TRUE;

        if (victim->pIndexData->pShop != NULL)
            return TRUE;

        /* no killing healers, trainers, etc */
        if (IS_SET (victim->act, ACT_TRAIN)
            || IS_SET (victim->act, ACT_PRACTICE)
            || IS_SET (victim->act, ACT_IS_HEALER)
            || IS_SET (victim->act, ACT_IS_CHANGER))
            return TRUE;

        if (!IS_NPC (ch))
        {
            /* no pets */
            if (IS_SET (victim->act, ACT_PET))
                return TRUE;

            /* no charmed creatures unless owner */
            if (IS_AFFECTED (victim, AFF_CHARM)
                && (area || ch != victim->master))
                return TRUE;

            /* legal kill? -- cannot hit mob fighting non-group member */
            if (victim->fighting != NULL
                && !is_same_group (ch, victim->fighting)) return TRUE;
        }
        else
        {
            /* area effect spells do not hit other mobs */
            if (area && !is_same_group (victim, ch->fighting))
                return TRUE;
        }
    }
    /* killing players */
    else
    {
        if (area && IS_IMMORTAL (victim) && victim->level > LEVEL_IMMORTAL)
            return TRUE;

        /* NPC doing the killing */
        if (IS_NPC (ch))
        {
            /* charmed mobs and pets cannot attack players while owned */
            if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL
                && ch->master->fighting != victim)
                return TRUE;

            /* safe room? */
            if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
                return TRUE;

            /* legal kill? -- mobs only hit players grouped with opponent */
            if (ch->fighting != NULL && !is_same_group (ch->fighting, victim))
                return TRUE;
        }

        /* player doing the killing */
        else
        {
            if (!is_clan (ch))
                return TRUE;

            if (IS_SET (victim->act, PLR_WANTED))
                return FALSE;

            if (!is_clan (victim))
                return TRUE;

            if (ch->level > victim->level + 8)
                return TRUE;
        }

    }
    return FALSE;
} // end bool is_safe_spell

/*
 * See if an attack justifies a (WANTED) flag.
 */
void check_wanted (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];

    // Player and/or victim cannot logout for 3 ticks after the last
    // attack on another player started.
    if (!IS_NPC(victim) && !IS_NPC(ch) && ch != victim)
    {
        victim->pcdata->pk_timer = 3;
        ch->pcdata->pk_timer = 3;
    }

    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while (IS_AFFECTED (victim, AFF_CHARM) && victim->master != NULL)
        victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if (IS_NPC (victim) || IS_SET (victim->act, PLR_WANTED))
        return;

    /*
     * Charm-o-rama.
     */
    if (IS_SET (ch->affected_by, AFF_CHARM))
    {
        if (ch->master == NULL)
        {
            char buf[MAX_STRING_LENGTH];

            sprintf (buf, "check_wanted: %s bad AFF_CHARM",
                     IS_NPC (ch) ? ch->short_descr : ch->name);
            bug (buf, 0);
            affect_strip (ch, gsn_charm_person);
            REMOVE_BIT (ch->affected_by, AFF_CHARM);
            return;
        }

        stop_follower (ch);
        return;
    }

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if (IS_NPC (ch)
        || ch == victim || ch->level >= LEVEL_IMMORTAL || !is_clan (ch)
        || IS_SET (ch->act, PLR_WANTED) || ch->fighting == victim)
        return;

    send_to_char ("*** You are now ({RWANTED{x)!! ***\n\r", ch);
    SET_BIT (ch->act, PLR_WANTED);
    sprintf (buf, "$N is attempting to murder %s", victim->name);
    wiznet (buf, ch, NULL, WIZ_FLAGS, 0, 0);
    save_char_obj (ch);
    return;
} // end void check_wanted

/*
 * Check for parry.
 */
bool check_parry (CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE (victim))
        return FALSE;

    chance = get_skill (victim, gsn_parry) / 2;

    if (get_eq_char (victim, WEAR_WIELD) == NULL)
    {
        if (IS_NPC (victim))
            chance /= 2;
        else
            return FALSE;
    }

    if (!can_see (ch, victim))
        chance /= 2;

    // Bladesinger bonus on parry
    if (is_affected(victim, gsn_bladesong))
    {
        chance += 10;
    }

    if (number_percent () >= chance + victim->level - ch->level)
        return FALSE;

    act ("You parry $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N parries your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_parry, TRUE, 6);
    return TRUE;
} // end bool check_parry

/*
 * Check for shield block.
 */
bool check_shield_block (CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE (victim))
        return FALSE;

    chance = get_skill (victim, gsn_shield_block) / 5 + 3;

    if (get_eq_char (victim, WEAR_SHIELD) == NULL)
        return FALSE;

    if (number_percent () >= chance + victim->level - ch->level)
        return FALSE;

    act ("You block $n's attack with your shield.", ch, NULL, victim, TO_VICT);
    act ("$N blocks your attack with a shield.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_shield_block, TRUE, 6);
    return TRUE;
} // end bool check_shield_block

/*
 * Check for dodge.
 */
bool check_dodge (CHAR_DATA * ch, CHAR_DATA * victim)
{
    int chance;

    if (!IS_AWAKE (victim))
        return FALSE;

    chance = get_skill (victim, gsn_dodge) / 2;

    if (!can_see (victim, ch))
        chance /= 2;

    // Bladesinger bonus on dodge
    if (is_affected(victim, gsn_bladesong))
    {
        chance += 10;
    }

    if (number_percent () >= chance + victim->level - ch->level)
        return FALSE;

    act ("You dodge $n's attack.", ch, NULL, victim, TO_VICT);
    act ("$N dodges your attack.", ch, NULL, victim, TO_CHAR);
    check_improve (victim, gsn_dodge, TRUE, 6);
    return TRUE;
} // end bool check_dodge

/*
 * Set position of a victim.
 */
void update_pos (CHAR_DATA * victim)
{
    if (victim->hit > 0)
    {
        if (victim->position <= POS_STUNNED)
            victim->position = POS_STANDING;
        return;
    }

    if (IS_NPC (victim) && victim->hit < 1)
    {
        victim->position = POS_DEAD;
        return;
    }

    if (victim->hit <= -11)
    {
        victim->position = POS_DEAD;
        return;
    }

    if (victim->hit <= -6)
        victim->position = POS_MORTAL;
    else if (victim->hit <= -3)
        victim->position = POS_INCAP;
    else
        victim->position = POS_STUNNED;

    return;
} // end void update_pos

/*
 * Start fights.
 */
void set_fighting (CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (ch->fighting != NULL)
    {
        bug ("Set_fighting: already fighting", 0);
        return;
    }

    if (IS_AFFECTED (ch, AFF_SLEEP))
        affect_strip (ch, gsn_sleep);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
} // end set_fighting

/*
 * Stop fights.
 */
void stop_fighting (CHAR_DATA * ch, bool fBoth)
{
    CHAR_DATA *fch;

    for (fch = char_list; fch != NULL; fch = fch->next)
    {
        if (fch == ch || (fBoth && fch->fighting == ch))
        {
            fch->fighting = NULL;
            fch->position = IS_NPC (fch) ? fch->default_pos : POS_STANDING;
            update_pos (fch);
        }
    }

    return;
} // end void stop_fighting

/*
 * Make a corpse out of a character.
 */
void make_corpse (CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if (IS_NPC (ch))
    {
        // Mob
        name = ch->short_descr;
        corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
        corpse->timer = number_range (3, 6);
        if (ch->gold > 0)
        {
            // Double gold bonus
            if (settings.double_gold)
            {
                ch->gold *= 2;
            }

            obj_to_obj (create_money (ch->gold, ch->silver), corpse);
            ch->gold = 0;
            ch->silver = 0;
        }
        corpse->cost = 0;
    }
    else
    {
        // Player
        name = ch->name;
        corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
        corpse->timer = number_range (25, 40);
        corpse->owner = str_dup (ch->name);

        if (ch->gold > 1 || ch->silver > 1)
        {
            obj_to_obj (create_money (ch->gold / 2, ch->silver / 2),
                        corpse);
            ch->gold -= ch->gold / 2;
            ch->silver -= ch->silver / 2;
        }

        corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf (buf, corpse->short_descr, name);
    free_string (corpse->short_descr);
    corpse->short_descr = str_dup (buf);

    sprintf (buf, corpse->description, name);
    free_string (corpse->description);
    corpse->description = str_dup (buf);

    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
        bool floating = FALSE;

        obj_next = obj->next_content;
        if (obj->wear_loc == WEAR_FLOAT)
            floating = TRUE;
        obj_from_char (obj);
        if (obj->item_type == ITEM_POTION)
            obj->timer = number_range (500, 1000);
        if (obj->item_type == ITEM_SCROLL)
            obj->timer = number_range (1000, 2500);
        if (IS_SET (obj->extra_flags, ITEM_ROT_DEATH) && !floating)
        {
            obj->timer = number_range (5, 10);
            // Rhien, commented this out, not sure why it needs to be removed if it's going to *poof* anyway.
            //REMOVE_BIT (obj->extra_flags, ITEM_ROT_DEATH);
        }
        REMOVE_BIT (obj->extra_flags, ITEM_VIS_DEATH);

        if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
        {
            extract_obj (obj);
        }
        else if (floating)
        {
            if (IS_OBJ_STAT (obj, ITEM_ROT_DEATH))
            {                    /* get rid of it! */
                if (obj->contains != NULL)
                {
                    OBJ_DATA *in, *in_next;

                    act ("$p evaporates,scattering its contents.",
                         ch, obj, NULL, TO_ROOM);
                    for (in = obj->contains; in != NULL; in = in_next)
                    {
                        in_next = in->next_content;
                        obj_from_obj (in);
                        obj_to_room (in, ch->in_room);
                    }
                }
                else
                    act ("$p evaporates.", ch, obj, NULL, TO_ROOM);
                extract_obj (obj);
            }
            else
            {
                act ("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
                obj_to_room (obj, ch->in_room);
            }
        }
        else
        {
            obj_to_obj (obj, corpse);
        }
    }

    obj_to_room (corpse, ch->in_room);
    return;
} // end void make_corpse

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry (CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";

    switch (number_bits (4))
    {
        case 0:
            msg = "$n hits the ground ... DEAD.";
            break;
        case 1:
            if (ch->material == 0)
            {
                msg = "$n splatters blood on your armor.";
                break;
            }
        case 2:
            if (IS_SET (ch->parts, PART_GUTS))
            {
                msg = "$n spills $s guts all over the floor.";
                vnum = OBJ_VNUM_GUTS;
            }
            break;
        case 3:
            if (IS_SET (ch->parts, PART_HEAD))
            {
                msg = "$n's severed head plops on the ground.";
                vnum = OBJ_VNUM_SEVERED_HEAD;
            }
            break;
        case 4:
            if (IS_SET (ch->parts, PART_HEART))
            {
                msg = "$n's heart is torn from $s chest.";
                vnum = OBJ_VNUM_TORN_HEART;
            }
            break;
        case 5:
            if (IS_SET (ch->parts, PART_ARMS))
            {
                msg = "$n's arm is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_ARM;
            }
            break;
        case 6:
            if (IS_SET (ch->parts, PART_LEGS))
            {
                msg = "$n's leg is sliced from $s dead body.";
                vnum = OBJ_VNUM_SLICED_LEG;
            }
            break;
        case 7:
            if (IS_SET (ch->parts, PART_BRAINS))
            {
                msg =
                    "$n's head is shattered, and $s brains splash all over you.";
                vnum = OBJ_VNUM_BRAINS;
            }
    }

    act (msg, ch, NULL, NULL, TO_ROOM);

    if (vnum != 0)
    {
        char buf[MAX_STRING_LENGTH];
        OBJ_DATA *obj;
        char *name;

        name = IS_NPC (ch) ? ch->short_descr : ch->name;
        obj = create_object (get_obj_index (vnum), 0);
        obj->timer = number_range (4, 7);

        sprintf (buf, obj->short_descr, name);
        free_string (obj->short_descr);
        obj->short_descr = str_dup (buf);

        sprintf (buf, obj->description, name);
        free_string (obj->description);
        obj->description = str_dup (buf);

        if (obj->item_type == ITEM_FOOD)
        {
            if (IS_SET (ch->form, FORM_POISON))
                obj->value[3] = 1;
            else if (!IS_SET (ch->form, FORM_EDIBLE))
                obj->item_type = ITEM_TRASH;
        }

        obj_to_room (obj, ch->in_room);
    }

    if (IS_NPC (ch))
        msg = "You hear something's death cry.";
    else
        msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for (door = 0; door < MAX_DIR; door++)
    {
        EXIT_DATA *pexit;

        if ((pexit = was_in_room->exit[door]) != NULL
            && pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room)
        {
            ch->in_room = pexit->u1.to_room;
            act (msg, ch, NULL, NULL, TO_ROOM);
        }
    }
    ch->in_room = was_in_room;

    return;
} // end void death_cry

/*
 * Kills a player or NPC, makes a corpse, etc.
 */
void raw_kill (CHAR_DATA * victim)
{
    int i;

    stop_fighting (victim, TRUE);
    death_cry (victim);

    // If the victim is not an NPC and they were in an arena room, transfer them to their
    // death repop spot with all of their gear.  There wil be no corpse and the victim
    // won't be extracted which is where the gear bombing happens.
    if (!IS_NPC(victim) && victim->in_room != NULL && IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    {
        char_from_room (victim);
        char_to_room (victim, get_room_index (clan_table[victim->clan].hall));
    }
    else
    {
        // Otherwise, make corpse, then send them to the repop room.
        make_corpse(victim);
        char_from_room (victim);
        char_to_room (victim, get_room_index (clan_table[victim->clan].hall));
    }

    if (IS_NPC (victim))
    {
        victim->pIndexData->killed++;
        extract_char (victim, TRUE);
        return;
    }

    // Extract the char (nuking their objects) if it's not an arena kill.
    if (!IS_NPC(victim) && victim->in_room != NULL && IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    {
        extract_char (victim, FALSE);
    }

    while (victim->affected)
        affect_remove (victim, victim->affected);
    victim->affected_by = race_table[victim->race].aff;
    for (i = 0; i < 4; i++)
        victim->armor[i] = 100;
    victim->position = POS_RESTING;
    victim->hit = UMAX (1, victim->hit);
    victim->mana = UMAX (1, victim->mana);
    victim->move = UMAX (1, victim->move);

    return;

} // void raw_kill

/*
 * Gain experience points in a group
 */
void group_gain (CHAR_DATA * ch, CHAR_DATA * victim)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (victim == ch)
        return;

    members = 0;
    group_levels = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        if (is_same_group (gch, ch))
        {
            members++;
            group_levels += IS_NPC (gch) ? gch->level / 2 : gch->level;
        }
    }

    if (members == 0)
    {
        bug ("Group_gain: members.", members);
        members = 1;
        group_levels = ch->level;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        if (!is_same_group (gch, ch) || IS_NPC (gch))
            continue;

        if ( gch->level - lch->level >= 8 )
        {
            send_to_char( "You are too high for this group.\n\r", gch );
            continue;
        }

        if ( gch->level - lch->level <= -8 )
        {
            send_to_char( "You are too low for this group.\n\r", gch );
            continue;
        }

        xp = xp_compute (gch, victim, group_levels);

        // Double experience bonus
        if (settings.double_exp)
            xp *= 2;

        sprintf (buf, "You receive %d experience points.\n\r", xp);
        send_to_char (buf, gch);
        gain_exp (gch, xp);

        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc == WEAR_NONE)
                continue;

            if ((IS_OBJ_STAT (obj, ITEM_ANTI_EVIL) && IS_EVIL (ch))
                || (IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_GOOD (ch))
                || (IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL (ch)))
            {
                act ("You are zapped by $p.", ch, obj, NULL, TO_CHAR);
                act ("$n is zapped by $p.", ch, obj, NULL, TO_ROOM);
                obj_from_char (obj);
                obj_to_room (obj, ch->in_room);
            }
        }
    }

    return;
} // end group_gain

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute (CHAR_DATA * gch, CHAR_DATA * victim, int total_levels)
{
    int xp, base_exp;
    int level_range;
    int time_per_level;

    if (IS_SET(gch->affected_by, AFF_CHARM))
        return 0;

    xp = 0;
    level_range = victim->level - gch->level;

    /* compute the base exp */
    switch (level_range)
    {
        default:
            base_exp = 0;
            break;
        case -9:
            base_exp = 1;
            break;
        case -8:
            base_exp = 2;
            break;
        case -7:
            base_exp = 5;
            break;
        case -6:
            base_exp = 9;
            break;
        case -5:
            base_exp = 11;
            break;
        case -4:
            base_exp = 22;
            break;
        case -3:
            base_exp = 33;
            break;
        case -2:
            base_exp = 50;
            break;
        case -1:
            base_exp = 66;
            break;
        case 0:
            base_exp = 83;
            break;
        case 1:
            base_exp = 99;
            break;
        case 2:
            base_exp = 121;
            break;
        case 3:
            base_exp = 143;
            break;
        case 4:
            base_exp = 165;
            break;
    }

    if (level_range > 4)
        base_exp = 160 + 20 * (level_range - 4);

    /* do alignment computations */
    // XP was getting wiped out for certain setups here, put cases in for
    // everything.  - Rhien
    switch(gch->alignment)
    {
        case ALIGN_GOOD:
            switch(victim->alignment)
            {
                case ALIGN_GOOD: xp += base_exp * 2 / 3; break;
                case ALIGN_NEUTRAL: xp = base_exp; break;
                case ALIGN_EVIL: xp += base_exp * 3 / 2; break;
                default: break;
            }
            break;
        case ALIGN_NEUTRAL:
            switch(victim->alignment)
            {
                case ALIGN_EVIL: xp = base_exp; break;
                case ALIGN_GOOD: xp += base_exp * 5 / 4; break;
                case ALIGN_NEUTRAL: xp += base_exp * 4 / 5; break;
                default: break;
            }
            break;
        case ALIGN_EVIL:
            switch(victim->alignment)
            {
                case ALIGN_EVIL: xp += base_exp * 2 / 3; break;
                case ALIGN_NEUTRAL: xp = base_exp; break;
                case ALIGN_GOOD: xp += base_exp * 3 / 2; break;
                default: break;
            }
            break;
        default: break;
    }

    /* more exp at the low levels */
    if (gch->level < 6)
        xp = 10 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > 35)
        xp = 15 * xp / (gch->level - 25);

    /* reduce for playing time */
    {
        /* compute quarter-hours per level */
        time_per_level = 4 *
            (gch->played + (int) (current_time - gch->logon)) / 3600
            / gch->level;

        time_per_level = URANGE (2, time_per_level, 12);
        if (gch->level < 15)    /* make it a curve */
            time_per_level = UMAX (time_per_level, (15 - gch->level));
        xp = xp * time_per_level / 12;
    }

    /* randomize the rewards */
    xp = number_range (xp * 3 / 4, xp * 5 / 4);

    /* adjust for grouping */
    xp = xp * gch->level / (UMAX (1, total_levels - 1));

    /* bonus for intelligence */
    xp = (xp * (100 + (get_curr_stat(gch, STAT_INT) * 4))) / 100;

    return xp;
} // end int xp_compute

/*
 * Displays a damage message based off of how much damage was done from the attacker
 * to the victim.  Testers will see the actual damage inflicted or received.
 */
void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,register int dam,int dt,bool immune )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if (ch == NULL || victim == NULL)
        return;

    if ( dam <=   0 )      { vs = "{ymiss{x";                vp = "{ymisses{x";              }
    else if ( dam <=   4 ) { vs = "{gscratch{x";             vp = "{gscratches{x";           }
    else if ( dam <=   8 ) { vs = "{ggraze{x";               vp = "{ggrazes{x";              }
    else if ( dam <=  12 ) { vs = "{ghit{x";                 vp = "{ghits{x";                }
    else if ( dam <=  16 ) { vs = "{ginjure{x";              vp = "{ginjures{x";             }
    else if ( dam <=  20 ) { vs = "{gwound{x";               vp = "{gwounds{x";              }
    else if ( dam <=  24 ) { vs = "{gmaul{x";                vp = "{gmauls{x";               }
    else if ( dam <=  28 ) { vs = "{gdecimate{x";            vp = "{gdecimates{x";           }
    else if ( dam <=  32 ) { vs = "{gdevastate{x";           vp = "{gdevastates{x";          }
    else if ( dam <=  36 ) { vs = "{gmaim{x";                vp = "{gmaims{x";               }
    else if ( dam <=  40 ) { vs = "{YMUTILATE{x";            vp = "{YMUTILATES{X";           }
    else if ( dam <=  44 ) { vs = "{YDISEMBOWEL{x";          vp = "{YDISEMBOWELS{X";         }
    else if ( dam <=  48 ) { vs = "{YDISMEMBER{x";           vp = "{YDISMEMBERS{x";          }
    else if ( dam <=  52 ) { vs = "{YMASSACRE{x";            vp ="{YMASSACRES{x";            }
    else if ( dam <=  56 ) { vs = "{YMANGLE{x";              vp = "{YMANGLES{x";             }
    else if ( dam <=  60 ) { vs = "*** {RDEMOLISH{x ***";    vp = "*** {RDEMOLISHES{X ***";  }
    else if ( dam <=  75 ) { vs = "*** {RDEVASTATE{x ***";   vp = "*** {RDEVASTATES{X ***";  }
    else if ( dam <= 100)  { vs = "=== {ROBLITERATE{x ===";  vp = "=== {ROBLITERATES{x ==="; }
    else if ( dam <= 125)  { vs = ">>> {RANNIHILATE{x <<<";  vp = ">>> {RANNIHILATES{x <<<"; }
    else if ( dam <= 150)  { vs = "<<< {RERADICATE{x >>>";   vp = "<<< {RERADICATES{x >>>";  }
    else                   { vs = "do {RU{WN{RS{WP{RE{WA{RK{WA{RB{WL{RE{x things to";
			     vp = "does {RU{WN{RS{WP{RE{WA{RK{WA{RB{WL{RE{x things to"; }

    punct   = (dam <= 24) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
        if (ch  == victim)
        {
            sprintf( buf1, "$N %s $Melf%c",vp,punct);
            sprintf( buf2, "You %s yourself%c",vs,punct);
        }
        else
        {
            sprintf( buf1, "$N %s $n%c",  vp, punct );
            sprintf( buf2, "You %s $N%c", vs, punct );
            sprintf( buf3, "$n %s you%c", vp, punct );
	}
    }
    else
    {
        if ( dt >= 0 && dt < top_sn )
        {
            attack = skill_table[dt]->noun_damage;
        }
	else if ( dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
        {
            attack = attack_table[dt - TYPE_HIT].noun;
        }
	else
	{
            bug( "Dam_message: bad dt %d.", dt );
            dt  = TYPE_HIT;
            attack = attack_table[0].name;
	}

	if (immune)
	{
	    if (ch == victim)
	    {
		sprintf(buf1,"$N is unaffected by $S own %s.",attack);
		sprintf(buf2,"Luckily, you are immune to that.");
	    }
	    else
	    {
	    	sprintf(buf1,"$n is unaffected by $N's %s!",attack);
	    	sprintf(buf2,"$N is unaffected by your %s!",attack);
	    	sprintf(buf3,"$n's %s is powerless against you.",attack);
	    }
	}
	else
	{
	    if (ch == victim)
	    {
 		sprintf( buf1, "$N's %s %s $M%c",attack,vp,punct);
                sprintf( buf2, "Your %s %s you%c",attack,vp,punct);
	    }
	    else
	    {
                sprintf( buf1, "$N's %s %s $n%c",  attack, vp, punct );
                sprintf( buf3, "$n's %s %s you%c", attack, vp, punct );
                sprintf( buf2, "Your %s %s $N%c",  attack, vp, punct );
	    }
	}
    }

    if (ch == victim)
    {
	act(buf1,ch,NULL,ch,TO_ROOM);
	act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else
    {
    	act( buf1, victim, NULL, ch, TO_NOTVICT );
    	act( buf2, ch, NULL, victim, TO_CHAR );
    	act( buf3, ch, NULL, victim, TO_VICT );
    }

    // Testers can see the damage being done and received.
    if (IS_TESTER(ch))
    {
        sprintf(buf1, "[Damage Inflicted {W%d{x]\n\r", dam);
        send_to_char(buf1, ch);
    }

    if (IS_TESTER(victim))
    {
        sprintf(buf1, "[Damage Received {W%d{x]\n\r", dam);
        send_to_char(buf1, victim);
    }

    return;
} // end void dam_message

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm (CHAR_DATA * ch, CHAR_DATA * victim)
{
    OBJ_DATA *obj;
    OBJ_DATA *vobj;

    if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
        return;

    if (IS_OBJ_STAT (obj, ITEM_NOREMOVE))
    {
        act ("$S weapon won't budge!", ch, NULL, victim, TO_CHAR);
        act ("$n tries to disarm you, but your weapon won't budge!", ch, NULL, victim, TO_VICT);
        act ("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    act ("$n DISARMS you and sends your weapon flying!", ch, NULL, victim, TO_VICT);
    act ("You disarm $N!", ch, NULL, victim, TO_CHAR);
    act ("$n disarms $N!", ch, NULL, victim, TO_NOTVICT);

    obj_from_char (obj);

    if (IS_OBJ_STAT (obj, ITEM_NODROP) || IS_OBJ_STAT (obj, ITEM_INVENTORY))
        obj_to_char (obj, victim);
    else
    {
        obj_to_room (obj, victim->in_room);
        if (IS_NPC (victim) && victim->wait == 0 && can_see_obj (victim, obj))
        {
            separate_obj(obj);
            get_obj (victim, obj, NULL);
        }
    }

    // Can't dual with without a primary, consider changing this to moving the
    // dual wielded weapon to the primary arm.
    if ((vobj = get_eq_char(victim,WEAR_SECONDARY_WIELD)) != NULL)
    {
        act ("$n stops using $p.", victim, vobj, NULL, TO_ROOM);
        act ("You stop using $p.", victim, vobj, NULL, TO_CHAR);
        unequip_char(victim,vobj);
    }

    // A bladesinger can't use bladesong without their weapon
    if (is_affected(victim, gsn_bladesong))
    {
        affect_strip(victim, gsn_bladesong);
    }

    return;
} // end void disarm

/*
 * Berserk skill, get mad, crush things harder. Rawr.
 */
void do_berserk (CHAR_DATA * ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill (ch, gsn_berserk)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_BERSERK))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_berserk]->skill_level[ch->class]))
    {
        send_to_char ("You turn red in the face, but nothing happens.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_BERSERK) || is_affected (ch, gsn_berserk)
        || is_affected (ch, skill_lookup ("frenzy")))
    {
        send_to_char ("You get a little madder.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CALM))
    {
        send_to_char ("You're feeling to mellow to berserk.\n\r", ch);
        return;
    }

    if (ch->mana < 50)
    {
        send_to_char ("You can't get up enough energy.\n\r", ch);
        return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
        chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit / ch->max_hit;
    chance += 25 - hp_percent / 2;

    if (number_percent () < chance)
    {
        AFFECT_DATA af;

        WAIT_STATE (ch, PULSE_VIOLENCE);
        ch->mana -= 50;
        ch->move /= 2;

        /* heal a little damage */
        ch->hit += ch->level * 2;
        ch->hit = UMIN (ch->hit, ch->max_hit);

        send_to_char ("Your pulse races as you are consumed by rage!\n\r",
                      ch);
        act ("$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM);
        check_improve (ch, gsn_berserk, TRUE, 2);

        af.where = TO_AFFECTS;
        af.type = gsn_berserk;
        af.level = ch->level;
        af.duration = number_fuzzy (ch->level / 8);
        af.modifier = UMAX (1, ch->level / 5);
        af.bitvector = AFF_BERSERK;

        af.location = APPLY_HITROLL;
        affect_to_char (ch, &af);

        af.location = APPLY_DAMROLL;
        affect_to_char (ch, &af);

        af.modifier = UMAX (10, 10 * (ch->level / 5));
        af.location = APPLY_AC;
        affect_to_char (ch, &af);
    }

    else
    {
        WAIT_STATE (ch, 3 * PULSE_VIOLENCE);
        ch->mana -= 25;
        ch->move /= 2;

        send_to_char ("Your pulse speeds up, but nothing happens.\n\r", ch);
        check_improve (ch, gsn_berserk, FALSE, 2);
    }
} // end do_berserk

/*
 * Bash, one character slams into another and if succesful dazes them.
 */
void do_bash (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument (argument, arg);

    if ((chance = get_skill (ch, gsn_bash)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_BASH))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_bash]->skill_level[ch->class]))
    {
        send_to_char ("Bashing? What's that?\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->position < POS_FIGHTING)
    {
        act ("You'll have to let $M get back up first.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You try to bash your brains out, but fail.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is your friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 15;
    else
        chance += (ch->size - victim->size) * 10;


    /* stats */
    chance += get_curr_stat (ch, STAT_STR);
    chance -= (get_curr_stat (victim, STAT_DEX) * 4) / 3;
    chance -= GET_AC (victim, AC_BASH) / 25;

    /* speed */
    if (IS_SET (ch->off_flags, OFF_FAST) || IS_AFFECTED (ch, AFF_HASTE))
        chance += 10;
    if (IS_SET (victim->off_flags, OFF_FAST) || IS_AFFECTED (victim, AFF_HASTE))
        chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    // Kender are very hard to bash
    if (victim->race == KENDER_RACE_LOOKUP)
        chance -= 20;

    if (!IS_NPC (victim) && chance < get_skill (victim, gsn_dodge))
    {
        chance -= 3 * (get_skill (victim, gsn_dodge) - chance);
    }

    /* now the attack */
    if (number_percent () < chance)
    {

        act ("$n sends you sprawling with a powerful bash!", ch, NULL, victim, TO_VICT);
        act ("You slam into $N, and send $M flying!", ch, NULL, victim, TO_CHAR);
        act ("$n sends $N sprawling with a powerful bash.", ch, NULL, victim, TO_NOTVICT);
        check_improve (ch, gsn_bash, TRUE, 1);

        DAZE_STATE (victim, 3 * PULSE_VIOLENCE);
        WAIT_STATE (ch, skill_table[gsn_bash]->beats);
        victim->position = POS_RESTING;
        damage (ch, victim, number_range (2, 2 + 2 * ch->size + chance / 20), gsn_bash, DAM_BASH, FALSE);
    }
    else
    {
        damage (ch, victim, 0, gsn_bash, DAM_BASH, FALSE);
        act ("You fall flat on your face!", ch, NULL, victim, TO_CHAR);
        act ("$n falls flat on $s face.", ch, NULL, victim, TO_NOTVICT);
        act ("You evade $n's bash, causing $m to fall flat on $s face.", ch, NULL, victim, TO_VICT);
        check_improve (ch, gsn_bash, FALSE, 1);
        ch->position = POS_RESTING;
        WAIT_STATE (ch, skill_table[gsn_bash]->beats * 3 / 2);
    }
    check_wanted(ch, victim);
} // end do_bash

/*
 * Dirt kicking skill to temporarly blind an opponent.
 */
void do_dirt (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument (argument, arg);

    if ((chance = get_skill (ch, gsn_dirt)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_KICK_DIRT))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_dirt]->skill_level[ch->class]))
    {
        send_to_char ("You get your feet dirty.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't in combat!\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (victim, AFF_BLIND))
    {
        act ("$E's already been blinded.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Very funny.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat (ch, STAT_DEX);
    chance -= 2 * get_curr_stat (victim, STAT_DEX);

    /* speed  */
    if (IS_SET (ch->off_flags, OFF_FAST) || IS_AFFECTED (ch, AFF_HASTE))
        chance += 10;
    if (IS_SET (victim->off_flags, OFF_FAST)
        || IS_AFFECTED (victim, AFF_HASTE))
        chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
        chance += 1;

    /* terrain */

    switch (ch->in_room->sector_type)
    {
        case (SECT_INSIDE):
            chance -= 20;
            break;
        case (SECT_CITY):
            chance -= 10;
            break;
        case (SECT_FIELD):
            chance += 5;
            break;
        case (SECT_FOREST):
            break;
        case (SECT_HILLS):
            break;
        case (SECT_MOUNTAIN):
            chance -= 10;
            break;
        case (SECT_WATER_SWIM):
            chance = 0;
            break;
        case (SECT_WATER_NOSWIM):
            chance = 0;
            break;
        case (SECT_OCEAN):
            chance = 0;
            break;
        case (SECT_UNDERWATER):
            chance = 0;
            break;
        case (SECT_AIR):
            chance = 0;
            break;
        case (SECT_DESERT):
            chance += 15;
            break;
        case (SECT_BEACH):
            chance += 15;
            break;
    }

    if (chance == 0)
    {
        send_to_char ("There isn't any dirt to kick.\n\r", ch);
        return;
    }

    /* now the attack */
    if (number_percent () < chance)
    {
        AFFECT_DATA af;
        act ("$n is blinded by the dirt in $s eyes!", victim, NULL, NULL, TO_ROOM);
        act ("$n kicks dirt in your eyes!", ch, NULL, victim, TO_VICT);
        damage (ch, victim, number_range (2, 5), gsn_dirt, DAM_NONE, FALSE);
        send_to_char ("You can't see a thing!\n\r", victim);
        check_improve (ch, gsn_dirt, TRUE, 2);
        WAIT_STATE (ch, skill_table[gsn_dirt]->beats);

        af.where = TO_AFFECTS;
        af.type = gsn_dirt;
        af.level = ch->level;
        af.duration = 0;
        af.location = APPLY_HITROLL;
        af.modifier = -4;
        af.bitvector = AFF_BLIND;

        affect_to_char (victim, &af);
    }
    else
    {
        damage (ch, victim, 0, gsn_dirt, DAM_NONE, TRUE);
        check_improve (ch, gsn_dirt, FALSE, 2);
        WAIT_STATE (ch, skill_table[gsn_dirt]->beats);
    }
    check_wanted(ch, victim);
} // end do_dirt

/*
 * Trip skill, if successful will daze the victim and knock them down.
 */
void do_trip (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument (argument, arg);

    if ((chance = get_skill (ch, gsn_trip)) == 0
        || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_TRIP))
        || (!IS_NPC (ch)
            && ch->level < skill_table[gsn_trip]->skill_level[ch->class]))
    {
        send_to_char ("Tripping?  What's that?\n\r", ch);
        return;
    }


    if (arg[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            send_to_char ("But you aren't fighting anyone!\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (victim, AFF_FLYING))
    {
        act ("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim->position < POS_FIGHTING)
    {
        act ("$N is already down.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("You fall flat on your face!\n\r", ch);
        WAIT_STATE (ch, 2 * skill_table[gsn_trip]->beats);
        act ("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;    /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat (ch, STAT_DEX);
    chance -= get_curr_stat (victim, STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET (ch->off_flags, OFF_FAST) || IS_AFFECTED (ch, AFF_HASTE))
        chance += 10;
    if (IS_SET (victim->off_flags, OFF_FAST)
        || IS_AFFECTED (victim, AFF_HASTE))
        chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* now the attack */
    if (number_percent () < chance)
    {
        act ("$n trips you and you go down!", ch, NULL, victim, TO_VICT);
        act ("You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR);
        act ("$n trips $N, sending $M to the ground.", ch, NULL, victim,
             TO_NOTVICT);
        check_improve (ch, gsn_trip, TRUE, 1);

        DAZE_STATE (victim, 2 * PULSE_VIOLENCE);
        WAIT_STATE (ch, skill_table[gsn_trip]->beats);
        victim->position = POS_RESTING;
        damage (ch, victim, number_range (2, 2 + 2 * victim->size), gsn_trip,
                DAM_BASH, TRUE);
    }
    else
    {
        damage (ch, victim, 0, gsn_trip, DAM_BASH, TRUE);
        WAIT_STATE (ch, skill_table[gsn_trip]->beats * 2 / 3);
        check_improve (ch, gsn_trip, FALSE, 1);
    }
    check_wanted(ch, victim);
} // end do_trip

/*
 * Starts a fight with an NPC.  It will also initiate a pkill if the
 * victim is wanted, if not, they must use do_murder to start the fight.
 */
void do_kill (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Kill whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    /* Allow player killing */
    if ( !IS_NPC(victim) )
    {
        if (!IS_SET(victim->act, PLR_WANTED))
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
    }

    if (victim == ch)
    {
        send_to_char ("You hit yourself.  Ouch!\n\r", ch);
        multi_hit (ch, ch, TYPE_UNDEFINED);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("You do the best you can!\n\r", ch);
        return;
    }

    WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
    check_wanted(ch, victim);
    multi_hit (ch, victim, TYPE_UNDEFINED);
    return;
} // end do_kill

/*
 * Method to make them spell out murder.
 */
void do_murde (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to MURDER, spell it out.\n\r", ch);
    return;
} // end do_murde

/*
 * Murder a character or NPC.
 */
void do_murder (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Murder whom?\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM)
        || (IS_NPC (ch) && IS_SET (ch->act, ACT_PET)))
        return;

    if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("Suicide is a mortal sin.\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master == victim)
    {
        act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("You do the best you can!\n\r", ch);
        return;
    }

    WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
    if (IS_NPC (ch))
        sprintf (buf, "Help! I am being attacked by %s!", ch->short_descr);
    else
        sprintf (buf, "Help!  I am being attacked by %s!", ch->name);
    do_function (victim, &do_yell, buf);
    check_wanted(ch, victim);
    multi_hit (ch, victim, TYPE_UNDEFINED);
    return;
} // end do_murder

/*
 * Backstab skill
 */
void do_backstab (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Backstab whom?\n\r", ch);
        return;
    }

    if (ch->fighting != NULL)
    {
        send_to_char ("You're facing the wrong end.\n\r", ch);
        return;
    }

    else if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("How can you sneak up on yourself?\n\r", ch);
        return;
    }

    if (is_safe (ch, victim))
        return;

    if (IS_NPC (victim) &&
        victim->fighting != NULL && !is_same_group (ch, victim->fighting))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    if ((obj = get_eq_char (ch, WEAR_WIELD)) == NULL)
    {
        send_to_char ("You need to wield a weapon to backstab.\n\r", ch);
        return;
    }

    if (victim->hit < victim->max_hit / 3)
    {
        act ("$N is hurt and suspicious ... you can't sneak up.", ch, NULL, victim, TO_CHAR);
        return;
    }

    check_wanted(ch, victim);
    WAIT_STATE (ch, skill_table[gsn_backstab]->beats);
    if (number_percent () < get_skill (ch, gsn_backstab)
        || (get_skill (ch, gsn_backstab) >= 2 && !IS_AWAKE (victim)))
    {
        check_improve (ch, gsn_backstab, TRUE, 1);
        multi_hit (ch, victim, gsn_backstab);
    }
    else
    {
        check_improve (ch, gsn_backstab, FALSE, 1);
        damage (ch, victim, 0, gsn_backstab, DAM_NONE, TRUE);
    }

    return;

} // end do_backstab

/*
 * Flee command to attempt to get yourself out of battle.
 */
void do_flee (CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ((victim = ch->fighting) == NULL)
    {
        if (ch->position == POS_FIGHTING)
            ch->position = POS_STANDING;
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }

    was_in = ch->in_room;
    for (attempt = 0; attempt < 6; attempt++)
    {
        EXIT_DATA *pexit;
        int door;

        door = number_door ();
        if ((pexit = was_in->exit[door]) == 0
            || pexit->u1.to_room == NULL
            || IS_SET (pexit->exit_info, EX_CLOSED)
            || number_range (0, ch->daze) != 0 || (IS_NPC (ch)
                                                   && IS_SET (pexit->u1.
                                                              to_room->
                                                              room_flags,
                                                              ROOM_NO_MOB)))
            continue;

        move_char (ch, door, FALSE);
        if ((now_in = ch->in_room) == was_in)
            continue;

        ch->in_room = was_in;
        act ("$n has fled!", ch, NULL, NULL, TO_ROOM);
        ch->in_room = now_in;

        if (!IS_NPC (ch))
        {
            send_to_char ("You flee from combat!\n\r", ch);
            if ((ch->class == 2) && (number_percent () < 3 * (ch->level / 2)))
                send_to_char ("You snuck away safely.\n\r", ch);
            else
            {
                send_to_char ("You lost 10 exp.\n\r", ch);
                gain_exp (ch, -10);
            }
        }

        stop_fighting (ch, TRUE);
        return;
    }

    send_to_char ("PANIC! You couldn't escape!\n\r", ch);
    return;
} // end do_flee


/*
 * Skill to put yourself in front of another player being attacked to make
 * yourself the primary target.
 */
void do_rescue (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument (argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char ("Rescue whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char ("What about fleeing instead?\n\r", ch);
        return;
    }

    if (!IS_NPC (ch) && IS_NPC (victim))
    {
        send_to_char ("Doesn't need your help!\n\r", ch);
        return;
    }

    if (ch->fighting == victim)
    {
        send_to_char ("Too late.\n\r", ch);
        return;
    }

    if ((fch = victim->fighting) == NULL)
    {
        send_to_char ("That person is not fighting right now.\n\r", ch);
        return;
    }

    if (IS_NPC (fch) && !is_same_group (ch, victim))
    {
        send_to_char ("Kill stealing is not permitted.\n\r", ch);
        return;
    }

    WAIT_STATE (ch, skill_table[gsn_rescue]->beats);
    if (number_percent () > get_skill (ch, gsn_rescue))
    {
        send_to_char ("You fail the rescue.\n\r", ch);
        check_improve (ch, gsn_rescue, FALSE, 1);
        return;
    }

    act("You rescue $N!", ch, NULL, victim, TO_CHAR);
    act("$n rescues you!", ch, NULL, victim, TO_VICT);
    act("$n rescues $N!", ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, gsn_rescue, TRUE, 1);

    stop_fighting(fch, FALSE);
    stop_fighting(victim, FALSE);

    check_wanted(ch, fch);
    set_fighting(ch, fch);
    set_fighting(fch, ch);
    return;
} // end do_rescue

/*
 * Kick, does a little extra damage.
 */
void do_kick (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;

    if (!IS_NPC (ch)
        && ch->level < skill_table[gsn_kick]->skill_level[ch->class])
    {
        send_to_char ("You better leave the martial arts to fighters.\n\r", ch);
        return;
    }

    if (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_KICK))
        return;

    if ((victim = ch->fighting) == NULL)
    {
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }

    WAIT_STATE (ch, skill_table[gsn_kick]->beats);
    if (get_skill (ch, gsn_kick) > number_percent ())
    {
        damage (ch, victim, number_range (1, ch->level), gsn_kick, DAM_BASH, TRUE);
        check_improve (ch, gsn_kick, TRUE, 1);
    }
    else
    {
        damage (ch, victim, 0, gsn_kick, DAM_BASH, TRUE);
        check_improve (ch, gsn_kick, FALSE, 1);
    }
    check_wanted(ch, victim);
    return;
} // end do_kick

/*
 * Disarm - attempt to knock your opponents weapon out of their hands.
 */
void do_disarm (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;

    hth = 0;

    if ((chance = get_skill (ch, gsn_disarm)) == 0)
    {
        send_to_char ("You don't know how to disarm opponents.\n\r", ch);
        return;
    }

    if (get_eq_char (ch, WEAR_WIELD) == NULL
        && ((hth = get_skill (ch, gsn_hand_to_hand)) == 0
            || (IS_NPC (ch) && !IS_SET (ch->off_flags, OFF_DISARM))))
    {
        send_to_char ("You must wield a weapon to disarm.\n\r", ch);
        return;
    }

    if ((victim = ch->fighting) == NULL)
    {
        send_to_char ("You aren't fighting anyone.\n\r", ch);
        return;
    }

    if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
    {
        send_to_char ("Your opponent is not wielding a weapon.\n\r", ch);
        return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill (ch, get_weapon_sn (ch, FALSE));
    vict_weapon = get_weapon_skill (victim, get_weapon_sn (victim, FALSE));
    ch_vict_weapon = get_weapon_skill (ch, get_weapon_sn (victim, FALSE));

    /* modifiers */

    /* skill */
    if (get_eq_char (ch, WEAR_WIELD) == NULL)
        chance = chance * hth / 150;
    else
        chance = chance * ch_weapon / 100;

    chance += (ch_vict_weapon / 2 - vict_weapon) / 2;

    /* dex vs. strength */
    chance += get_curr_stat (ch, STAT_DEX);
    chance -= 2 * get_curr_stat (victim, STAT_STR);

    /* level */
    chance += (ch->level - victim->level) * 2;

    // Bladesingers get a disarm bonus attacking and a disarm bonus defending
    if (is_affected(ch, gsn_bladesong))
    {
        chance += 8;
    }

    if (is_affected(victim, gsn_bladesong))
    {
        chance /= 2;
    }

    /* and now the attack */
    if (number_percent () < chance)
    {
        WAIT_STATE (ch, skill_table[gsn_disarm]->beats);
        disarm (ch, victim);
        check_improve (ch, gsn_disarm, TRUE, 1);
    }
    else
    {
        WAIT_STATE (ch, skill_table[gsn_disarm]->beats);
        act ("You fail to disarm $N.", ch, NULL, victim, TO_CHAR);
        act ("$n tries to disarm you, but fails.", ch, NULL, victim, TO_VICT);
        act ("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT);
        check_improve (ch, gsn_disarm, FALSE, 1);
    }
    check_wanted(ch, victim);
    return;
} // end do_disarm

/*
 * Attempt to surrender to your opponent to stop a fight.
 */
void do_surrender (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *mob;
    if ((mob = ch->fighting) == NULL)
    {
        send_to_char ("But you're not fighting!\n\r", ch);
        return;
    }
    act ("You surrender to $N!", ch, NULL, mob, TO_CHAR);
    act ("$n surrenders to you!", ch, NULL, mob, TO_VICT);
    act ("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT);
    stop_fighting (ch, TRUE);

    if (!IS_NPC (ch) && IS_NPC (mob)
        && (!HAS_TRIGGER (mob, TRIG_SURR)
            || !mp_percent_trigger (mob, ch, NULL, NULL, TRIG_SURR)))
    {
        act ("$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR);
        multi_hit (mob, ch, TYPE_UNDEFINED);
    }
} // end do_surrender

/*
 * Displays a toast message to the mud whenever a pkill occurs.
 */
void toast( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    char verb[25];

    // Both have to be players
    if (IS_NPC(victim) || IS_NPC(ch))
        return;

    // You can't toast yourself
    if( ch == victim )
        return;

    // How bad where they beaten?
    if( ch->hit < ch->max_hit / 5 ) {
        sprintf(verb,"{yoffed{x");
    }
    else if (ch->hit < ch->max_hit / 4 ) {
        sprintf(verb,"{gkilled{x");
    }
    else if (ch->hit < ch->max_hit / 3 ) {
	sprintf(verb,"{gtoasted{x");
    }
    else if (ch->hit < ch->max_hit / 2 ) {
        sprintf(verb,"{YROCKED{x");
    }
    else if (ch->hit <  (ch->max_hit * 2 / 3 ) ) {
        sprintf(verb,"{YRAMPAGED{x");
    }
    else {
        sprintf(verb,"***{RDESTROYED{x***");
    }

    // The final message
    sprintf(buf, "%s%s got %s by %s.%s\n\r",
        (victim->desc == NULL ) ? "({YLink Dead{x) " : "",
         victim->name, verb, ch->name,
        (IS_SET(ch->in_room->room_flags, ROOM_ARENA)) ? " {W({cArena{W){x": "");

    // Log it
    log_string(buf);

    // Send it to all the players
    send_to_all_char(buf);

    return;

}  // end void toast

/*
 * This method can be used to check the death status of a character who has had damage inflicted
 * on them by something other than a fight (like, drowning in the ocean, dying from a spell or
 * disease, etc).  The damage type must be passed in and can be used to display custom messages
 * about the death.  The case messages for the updated positions have been borrowed from fight.c.
 * This will only currently work on players, not NPC's.
 */
void check_death(CHAR_DATA *victim, int dt)
{
    // If IS_NPC then get out.
    if (IS_NPC(victim))
        return;

    // Immortals will be immune from dying in this manner, if they go below 0 then set
    // them to 1 hp.
    if (victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
    {
        victim->hit = 1;
    }

    update_pos(victim);

    switch(victim->position)
    {
        case POS_MORTAL:
            act("$n is mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_ROOM);
            send_to_char("You are mortally wounded, and will die soon, if not aided.\n\r", victim);
            break;
        case POS_INCAP:
            act("$n is incapacitated and will slowly die, if not aided.", victim, NULL, NULL, TO_ROOM);
            send_to_char("You are incapacitated and will slowly die, if not aided.\n\r", victim);
            break;
        case POS_STUNNED:
            act("$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM);
            send_to_char("You are stunned, but will probably recover.\n\r", victim);
            break;
        case POS_DEAD:
            act( "$n is {RDEAD{x!!", victim, 0, 0, TO_ROOM);
            send_to_char("You have been {RKILLED{x!!\n\r\n\r", victim);
            break;
        default:
            break;
    }

    // The character is dead, commence with officially killing them off
    if (victim->position == POS_DEAD)
    {
        char buf[MAX_STRING_LENGTH];

        // EXP penalty for dying
        if ( victim->exp > exp_per_level(victim,victim->pcdata->points) * victim->level )
        {
            gain_exp(victim, (2 * (exp_per_level(victim,victim->pcdata->points)
                                 * victim->level - victim->exp)/3) + 50 );
        }

	    if (dt == DAM_DROWNING)
	    {
            sprintf(buf, "%s drowns at %s [room %d]", victim->name, victim->in_room->name, victim->in_room->vnum);
	    }
	    else
	    {
            sprintf(buf, "%s was killed at %s [room %d]", victim->name, victim->in_room->name, victim->in_room->vnum);
	    }

        // Report the death and finish the process
        wiznet(buf, NULL, NULL, WIZ_DEATHS, 0, 0);
        log_string(buf);
        raw_kill(victim);
        return;

    }

} // end check_death

/*
 * Gore command that will allow minotaur's to charge a victim.
 */
void do_gore( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    int chance;
    int dam;

    // Not for NPC's (currently)
    if (IS_NPC(ch))
    {
        return;
    }

    if (ch->race != MINOTAUR_RACE_LOOKUP)
    {
        send_to_char("You do not have the horns necessary to gore someone.\n\r", ch);
        return;
    }

    one_argument(argument, arg);

    // Can't use this skill if you don't have it.. this will be a racial skill not
    // selectable by classes.
    if (get_skill(ch, gsn_gore) == 0 || ch->level < skill_table[gsn_gore]->skill_level[ch->class])
    {
        send_to_char("You do not have the skill to gore someone.\n\r", ch);
        return;
    }

    // Get the target
    if (arg[0] == '\0')
    {
        victim = ch->fighting;

        if (victim == NULL)
        {
            send_to_char("Gore who?\n\r", ch);
            return;
        }
    }
    else if ((victim = get_char_room(ch, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    // Can't charge yourself.
    if (victim == ch)
    {
        send_to_char("You cannot gore yourself...\n\r", ch);
        return;
    }

    // Is the victim a safe target.. like a non-clanner.
    if (is_safe(ch, victim))
    {
        return;
    }

    // Can't gore someone who has you charmed.
    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
        act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
        return;
    }

    // Base skill, higher base chance against NPC's
    if (IS_NPC(victim))
    {
        // Start at 90% if the skill is 100%.
        chance = (get_skill(ch, gsn_gore ) * 9) / 10;
    }
    else
    {
        // Start at 80% if the skill is 100%.
        chance = (get_skill(ch, gsn_gore ) * 8) / 10;
    }

    // Adjust for level difference
    chance += (ch->level - victim->level) * 5;

    // Factor in dexterity difference
    chance += (get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim, STAT_DEX)) * 4;

    // If the character is affected by haste and the victim isn't they get a bonus, conversely, if the
    // victim is affected by haste and the player isn't, they get a penalty.
    if (IS_AFFECTED(ch, AFF_HASTE) && !IS_AFFECTED(victim, AFF_HASTE))
    {
        chance += 10;
    }
    else if (!IS_AFFECTED(ch, AFF_HASTE) && IS_AFFECTED(victim, AFF_HASTE))
    {
        chance -= 10;
    }

    // Is the minotaur blind?  Penalize (heavily) if so.
    if (IS_AFFECTED(ch, AFF_BLIND))
    {
        chance -= 30;
    }

    // Bonus or penalty for size
    chance += (ch->size - victim->size) * 4;

    // The moment of truth
    if (number_percent() > chance)
    {
        act("You charge at $N and miss!", ch, NULL, victim, TO_CHAR);
        act("$n charges at you and miss!", ch, NULL, victim, TO_VICT);
        act("$n charges at $N and misses.", ch, NULL, victim, TO_NOTVICT);

        damage(ch, victim, 0, TYPE_UNDEFINED, DAM_PIERCE, TRUE);
        check_improve(ch, gsn_gore, FALSE, 4);
        WAIT_STATE(ch, skill_table[gsn_gore]->beats);
        return;
    }
    else
    {
        act("You charge hard at $N!", ch, NULL, victim, TO_CHAR);
        act("$n charges hard into you!", ch, NULL, victim, TO_VICT);
        act("$n charges hard at $N.", ch, NULL, victim, TO_NOTVICT);

        // Calculate base damage for hit.
        dam = number_range(ch->level, ch->level * 2);

        // Factor in both strength of the character and victim for the stun chance
        chance = 80 + (get_curr_stat(ch, STAT_STR) * 4) - (get_curr_stat(victim, STAT_STR) * 4);

        // Factor in the weight that the victim is carrying in the stun chance similiar to bash
        chance -= victim->carry_weight / 100;

        if (number_percent() < chance)
        {
            act("You toss $N through the air!", ch, NULL, victim, TO_CHAR);
            act("$n tosses you into the air!", ch, NULL, victim, TO_VICT);
            act("$n tosses $N into the air!", ch, NULL, victim, TO_NOTVICT);
            DAZE_STATE(victim, 2 * PULSE_VIOLENCE);
        }

        damage(ch, victim, dam, gsn_gore, DAM_PIERCE, TRUE);
        check_improve(ch, gsn_gore, TRUE, 2);
        WAIT_STATE(ch, skill_table[gsn_gore]->beats);
    }

} // end do_gore
