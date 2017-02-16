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
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "sha256.h"
#include "grid.h"

char *const where_name[] = {
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<floating nearby>   ",
    "<secondary weapon>  ",
};

/*
 * Local functions.
 */
char *format_obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort);
void show_list_to_char   (OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing);
void show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch);
void show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch);
void show_char_to_char   (CHAR_DATA * list, CHAR_DATA * ch);
bool check_blind         (CHAR_DATA * ch);
bool char_in_list        (CHAR_DATA * ch);
char *flag_string        (const struct flag_type *flag_table, int bits); // used in do_class


/*
 * Formats an object to be shown to a character.  The determination of whether an object
 * can be seen or not happens before here.
 */
char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
        || (obj->description == NULL || obj->description[0] == '\0'))
        return buf;

    if (IS_OBJ_STAT(obj, ITEM_INVIS))
        strcat(buf, "({wInvis{x) ");
    if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_EVIL))
        strcat(buf, "({RRed Aura{x) ");
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_BLESS))
        strcat(buf, "({BBlue Aura{x) ");
    //if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC))
    //    strcat(buf, "({YMagical{x) ");
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
        strcat(buf, "({cGlowing{x) ");
    if (IS_OBJ_STAT(obj, ITEM_HUM))
        strcat(buf, "({YHumming{x) ");
    if (IS_OBJ_STAT(obj, ITEM_BURIED))
        strcat(buf, "({yBuried{x) ");

    if (fShort)
    {
        if (obj->short_descr != NULL)
        {
            strcat(buf, obj->short_descr);
        }
    }
    else
    {
        if (obj->description != NULL)
        {
            strcat(buf, obj->description);
        }
    }

    return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if (ch->desc == NULL)
        return;

    // If there's no list... then don't do all this crap!  -Thoric
    if (!list)
    {
        if (fShowNothing)
        {
            if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
            {
                send_to_char("     ", ch);
            }
            send_to_char("Nothing.\r\n", ch);
        }
        return;
    }

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
        count++;
    prgpstrShow = alloc_mem(count * sizeof(char *));
    prgnShow = alloc_mem(count * sizeof(int));
    nShow = 0;

    /*
     * Format the list of objects.
     */
    for (obj = list; obj != NULL; obj = obj->next_content)
    {
        if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
        {
            pstrShow = format_obj_to_char(obj, ch, fShort);

            fCombine = FALSE;

            if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
            {
                /*
                 * Look for duplicates, case sensitive.
                 * Matches tend to be near end so run loop backwords.
                 */
                for (iShow = nShow - 1; iShow >= 0; iShow--)
                {
                    if (!strcmp(prgpstrShow[iShow], pstrShow))
                    {
                        prgnShow[iShow] += obj->count;
                        fCombine = TRUE;
                        break;
                    }
                }
            }

            /*
             * Couldn't combine, or didn't want to.
             */
            if (!fCombine)
            {
                prgpstrShow[nShow] = str_dup(pstrShow);
                prgnShow[nShow] = obj->count;
                nShow++;
            }
        }
    }

    /*
     * Output the formatted list.
     */
    for (iShow = 0; iShow < nShow; iShow++)
    {
        if (prgpstrShow[iShow][0] == '\0')
        {
            free_string(prgpstrShow[iShow]);
            continue;
        }

        if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
        {
            if (prgnShow[iShow] != 1)
            {
                sprintf(buf, "(%2d) ", prgnShow[iShow]);
                add_buf(output, buf);
            }
            else
            {
                add_buf(output, "     ");
            }
        }
        add_buf(output, prgpstrShow[iShow]);
        add_buf(output, "\r\n");
        free_string(prgpstrShow[iShow]);
    }

    if (fShowNothing && nShow == 0)
    {
        if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
        {
            send_to_char("     ", ch);
        }
        send_to_char("Nothing.\r\n", ch);
    }

    page_to_char(buf_string(output), ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem(prgpstrShow, count * sizeof(char *));
    free_mem(prgnShow, count * sizeof(int));

    return;
}



void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (IS_SET(victim->comm, COMM_AFK))
        strcat(buf, "[{cAFK{x] ");
    if (IS_AFFECTED(victim, AFF_INVISIBLE))
        strcat(buf, "({wInvis{x) ");
    if (victim->invis_level >= LEVEL_HERO)
        strcat(buf, "({WWizi{x) ");
    if (IS_AFFECTED(victim, AFF_HIDE))
        strcat(buf, "({wHide{x) ");
    if (IS_AFFECTED(victim, AFF_SNEAK))
        strcat(buf, "({wSneak{x) ");
    if (victim->desc == NULL && !IS_NPC(victim))
        strcat(buf, "({Y{*Link Dead{x) ");
    if (IS_AFFECTED(victim, AFF_CHARM))
        strcat(buf, "({YCharmed{x) ");
    if (IS_AFFECTED(victim, AFF_PASS_DOOR))
        strcat(buf, "({CTranslucent{x) ");
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
        strcat(buf, "({RPink Aura{x) ");
    if (IS_EVIL(victim) && IS_AFFECTED(ch, AFF_DETECT_EVIL))
        strcat(buf, "({RRed Aura{x) ");
    if (IS_GOOD(victim) && IS_AFFECTED(ch, AFF_DETECT_GOOD))
        strcat(buf, "({YGolden Aura{x) ");
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
        strcat(buf, "({WWhite Aura{x) ");
    if (IS_AFFECTED(victim, AFF_DEAFEN))
        strcat(buf, "({wDeaf{x) ");
    if (is_affected(victim, gsn_camouflage))
        strcat(buf, "({gCamouflage{x) ");

    // Questing
    if (!IS_NPC(ch))
    {
        if (IS_NPC(victim) && ch->pcdata->quest_mob > 0 && victim->pIndexData->vnum == ch->pcdata->quest_mob)
            strcat( buf, "[{RTARGET{x] ");
    }

    // Healers sense affliction
    if (is_affected(ch, gsn_sense_affliction))
    {
        if (is_affected(victim, gsn_blindness) ||
            is_affected(victim, gsn_plague) ||
            is_affected(victim, gsn_curse) ||
            is_affected(victim, gsn_poison) ||
            is_affected(victim, gsn_slow) ||
            is_affected(victim, gsn_sleep) ||
            is_affected(victim, gsn_song_of_dissonance) ||
            is_affected(victim, gsn_weaken))
        {
            strcat(buf, "({RAfflicted{x) ");
        }
    }

    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WANTED))
        strcat(buf, "({RWANTED{x) ");
    if (victim->position == victim->start_pos
        && victim->long_descr[0] != '\0')
    {
        strcat(buf, victim->long_descr);
        send_to_char(buf, ch);
        return;
    }

    strcat(buf, PERS(victim, ch));
    if (!IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
        && victim->position == POS_STANDING && ch->on == NULL)
        strcat(buf, victim->pcdata->title);

    switch (victim->position)
    {
        case POS_DEAD:
            strcat(buf, " is DEAD!!");
            break;
        case POS_MORTAL:
            strcat(buf, " is mortally wounded.");
            break;
        case POS_INCAP:
            strcat(buf, " is incapacitated.");
            break;
        case POS_STUNNED:
            strcat(buf, " is lying here stunned.");
            break;
        case POS_SLEEPING:
            if (victim->on != NULL)
            {
                if (IS_SET(victim->on->value[2], SLEEP_AT))
                {
                    sprintf(message, " is sleeping at %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else if (IS_SET(victim->on->value[2], SLEEP_ON))
                {
                    sprintf(message, " is sleeping on %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else
                {
                    sprintf(message, " is sleeping in %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
            }
            else
                strcat(buf, " is sleeping here.");
            break;
        case POS_RESTING:
            if (victim->on != NULL)
            {
                if (IS_SET(victim->on->value[2], REST_AT))
                {
                    sprintf(message, " is resting at %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else if (IS_SET(victim->on->value[2], REST_ON))
                {
                    sprintf(message, " is resting on %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else
                {
                    sprintf(message, " is resting in %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
            }
            else
                strcat(buf, " is resting here.");
            break;
        case POS_SITTING:
            if (victim->on != NULL)
            {
                if (IS_SET(victim->on->value[2], SIT_AT))
                {
                    sprintf(message, " is sitting at %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else if (IS_SET(victim->on->value[2], SIT_ON))
                {
                    sprintf(message, " is sitting on %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else
                {
                    sprintf(message, " is sitting in %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
            }
            else
                strcat(buf, " is sitting here.");
            break;
        case POS_STANDING:
            if (victim->on != NULL)
            {
                if (IS_SET(victim->on->value[2], STAND_AT))
                {
                    sprintf(message, " is standing at %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else if (IS_SET(victim->on->value[2], STAND_ON))
                {
                    sprintf(message, " is standing on %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
                else
                {
                    sprintf(message, " is standing in %s.",
                        victim->on->short_descr);
                    strcat(buf, message);
                }
            }
            else
                strcat(buf, " is here.");
            break;
        case POS_FIGHTING:
            strcat(buf, " is here, fighting ");
            if (victim->fighting == NULL)
                strcat(buf, "thin air??");
            else if (victim->fighting == ch)
                strcat(buf, "YOU!");
            else if (victim->in_room == victim->fighting->in_room)
            {
                strcat(buf, PERS(victim->fighting, ch));
                strcat(buf, ".");
            }
            else
                strcat(buf, "someone who left??");
            break;
    }

    strcat(buf, "\r\n");
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    return;
}

void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if (can_see(victim, ch))
    {
        if (ch == victim)
            act("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
        else
        {
            act("$n looks at you.", ch, NULL, victim, TO_VICT);
            act("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
        }
    }

    if (victim->description[0] != '\0')
    {
        send_to_char(victim->description, ch);
    }
    else
    {
        act("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
    }

    if (victim->max_hit > 0)
        percent = (100 * victim->hit) / victim->max_hit;
    else
        percent = -1;

    strcpy(buf, PERS(victim, ch));

    if (percent >= 100)
        strcat(buf, " is in excellent condition.\r\n");
    else if (percent >= 90)
        strcat(buf, " has a few scratches.\r\n");
    else if (percent >= 75)
        strcat(buf, " has some small wounds and bruises.\r\n");
    else if (percent >= 50)
        strcat(buf, " has quite a few wounds.\r\n");
    else if (percent >= 30)
        strcat(buf, " has some big nasty wounds and scratches.\r\n");
    else if (percent >= 15)
        strcat(buf, " looks pretty hurt.\r\n");
    else if (percent >= 0)
        strcat(buf, " is in awful condition.\r\n");
    else
        strcat(buf, " is bleeding to death.\r\n");

    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);

    // Detect magic will show you how much mana a person has left
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC))
    {
        if (victim->max_mana > 0)
        {
            percent = (100 * victim->mana) / victim->max_mana;
        }
        else
        {
            percent = 0;
        }

        if (percent >= 100)
        {
            sprintf(buf, "%s has full magically ability.\r\n", PERS(victim, ch));
        }
        else if (percent >= 75)
        {
            sprintf(buf, "%s has a good amount of magical ability left.\r\n", PERS(victim, ch));
        }
        else if (percent >= 40)
        {
            sprintf(buf, "%s has a fair amount of magical ability left.\r\n", PERS(victim, ch));
        }
        else if (percent >= 25)
        {
            sprintf(buf, "%s has a low amount of magical ability left.\r\n", PERS(victim, ch));
        }
        else if (percent >= 1)
        {
            sprintf(buf, "%s has an almost depleted amount of magical ability left.\r\n", PERS(victim, ch));
        }
        else
        {
            sprintf(buf, "%s has an no magical ability left.\r\n", PERS(victim, ch));
        }

        send_to_char(buf, ch);
    }

    // Healers sense affliction
    if (is_affected(ch, gsn_sense_affliction))
    {
        if (is_affected(victim, gsn_blindness) ||
            is_affected(victim, gsn_plague) ||
            is_affected(victim, gsn_curse) ||
            is_affected(victim, gsn_poison) ||
            is_affected(victim, gsn_slow) ||
            is_affected(victim, gsn_sleep) ||
            is_affected(victim, gsn_song_of_dissonance) ||
            is_affected(victim, gsn_weaken))
        {
            // The victim will be affected by one of these so we know we can
            // safely lop off the last two characters.
            sprintf(buf, "\r\n%s appears to be afflicted by ", PERS(victim, ch));

            if (is_affected(victim, gsn_blindness))
                strcat(buf, "blindness, ");
            if (is_affected(victim, gsn_plague))
                strcat(buf, "plague, ");
            if (is_affected(victim, gsn_curse))
                strcat(buf, "curse, ");
            if (is_affected(victim, gsn_poison))
                strcat(buf, "poison, ");
            if (is_affected(victim, gsn_slow))
                strcat(buf, "slow, ");
            if (is_affected(victim, gsn_sleep))
                strcat(buf, "sleep, ");
            if (is_affected(victim, gsn_weaken))
                strcat(buf, "weaken, ");
            if (is_affected(victim, gsn_song_of_dissonance))
                strcat(buf, "deaf, ");

            // It should always be greater than 2 if it gets here, but I'm paranoid, if someone
            // where to add a check in the first if but not add the strcat of the affliction it could
            // access another memory location if the length was 0 - 2. :p
            size_t len = strlen(buf);

            if (len > 2)
            {
                buf[len - 2] = '\0';
                send_to_char(buf, ch);
                send_to_char("\r\n", ch);
            }
        }
    }

    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
        if ((obj = get_eq_char(victim, iWear)) != NULL
            && can_see_obj(ch, obj))
        {
            if (!found)
            {
                send_to_char("\r\n", ch);
                act("$N is using:", ch, NULL, victim, TO_CHAR);
                found = TRUE;
            }
            send_to_char(where_name[iWear], ch);
            send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
            send_to_char("\r\n", ch);
        }
    }

    if (victim != ch && !IS_NPC(ch)
        && number_percent() < get_skill(ch, gsn_peek))
    {
        send_to_char("\r\nYou peek at the inventory:\r\n", ch);
        check_improve(ch, gsn_peek, TRUE, 4);
        show_list_to_char(victim->carrying, ch, TRUE, TRUE);
    }

    return;
}

void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
    CHAR_DATA *rch;

    for (rch = list; rch != NULL; rch = rch->next_in_room)
    {
        if (rch == ch)
            continue;

        if (get_trust(ch) < rch->invis_level)
            continue;

        if (can_see(ch, rch))
        {
            show_char_to_char_0(rch, ch);
        }
        else if (room_is_dark(ch->in_room)
            && IS_AFFECTED(rch, AFF_INFRARED))
        {
            send_to_char("You see glowing red eyes watching YOU!\r\n", ch);
        }
    }

    return;
}

/*
 * Whether or not a character is in the list already.  This was originally added when
 * reclasses so we don't slot a player in the list who is already in the list.
 * Rhien, 5/22/2015
 */
bool char_in_list(CHAR_DATA *ch)
{
    CHAR_DATA *vch;
    bool found = FALSE;

    for (vch = char_list; vch != NULL; vch = vch->next)
    {
        if (vch == ch)
        {
            found = TRUE;
            break;
        }
    }
    return found;
}

/*
 * Checks whether a player is currently blind or not.
 */
bool check_blind(CHAR_DATA * ch)
{
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    {
        return TRUE;
    }

    if (IS_AFFECTED(ch, AFF_BLIND))
    {
        send_to_char("You can't see a thing!\r\n", ch);
        return FALSE;
    }

    return TRUE;
}

/*
 * Checks whether or not fog exists in the room and whether a player can currently
 * see through it.
 */
bool check_fog(CHAR_DATA *ch)
{
    if (ch == NULL)
    {
        return FALSE;
    }

    int fog_density = 0;
    OBJ_DATA *fog_obj;

    for (fog_obj = ch->in_room->contents; fog_obj; fog_obj = fog_obj->next_content)
    {
        if (fog_obj->item_type == ITEM_FOG)
        {
            fog_density = fog_obj->value[0];
            break;
        }
    }

    // We have fog
    if (fog_density > 0)
    {
        if (number_range(1, 100) < fog_density)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        if (ch->lines == 0)
            send_to_char("You do not page long messages.\r\n", ch);
        else
        {
            sprintf(buf, "You currently display %d lines per page.\r\n",
                ch->lines + 2);
            send_to_char(buf, ch);
        }
        return;
    }

    if (!is_number(arg))
    {
        send_to_char("You must provide a number.\r\n", ch);
        return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        send_to_char("Paging disabled.\r\n", ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
        send_to_char("You must provide a reasonable number.\r\n", ch);
        return;
    }

    sprintf(buf, "Scroll set to %d lines.\r\n", lines);
    send_to_char(buf, ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
        sprintf(buf, "%-12s", social_table[iSocial].name);
        send_to_char(buf, ch);
        if (++col % 6 == 0)
            send_to_char("\r\n", ch);
    }

    if (col % 6 != 0)
        send_to_char("\r\n", ch);
    return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA * ch, char *argument)
{
    send_to_char("\r\n", ch);
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA * ch, char *argument)
{
    send_to_char("\r\n", ch);
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA * ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_wizlist(CHAR_DATA * ch, char *argument)
{
    do_function(ch, &do_help, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA * ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
        return;

    send_to_char("   action     status\r\n", ch);
    send_to_char("---------------------\r\n", ch);

    send_to_char("autoassist     ", ch);
    if (IS_SET(ch->act, PLR_AUTOASSIST))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("autoexit       ", ch);
    if (IS_SET(ch->act, PLR_AUTOEXIT))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("autogold       ", ch);
    if (IS_SET(ch->act, PLR_AUTOGOLD))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("autoloot       ", ch);
    if (IS_SET(ch->act, PLR_AUTOLOOT))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("autosac        ", ch);
    if (IS_SET(ch->act, PLR_AUTOSAC))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("autosplit      ", ch);
    if (IS_SET(ch->act, PLR_AUTOSPLIT))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("telnetga       ", ch);
    if (IS_SET(ch->comm, COMM_TELNET_GA))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("compact mode   ", ch);
    if (IS_SET(ch->comm, COMM_COMPACT))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("prompt         ", ch);
    if (IS_SET(ch->comm, COMM_PROMPT))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    send_to_char("combine items  ", ch);
    if (IS_SET(ch->comm, COMM_COMBINE))
        send_to_char("{GON{x\r\n", ch);
    else
        send_to_char("{ROFF{x\r\n", ch);

    if (!IS_SET(ch->act, PLR_CANLOOT))
        send_to_char("Your corpse is safe from thieves.\r\n", ch);
    else
        send_to_char("Your corpse may be looted.\r\n", ch);

    if (IS_SET(ch->act, PLR_NOSUMMON))
        send_to_char("You cannot be summoned.\r\n", ch);
    else
        send_to_char("You can be summoned.\r\n", ch);

    if (IS_SET(ch->act, PLR_NOFOLLOW))
        send_to_char("You do not welcome followers.\r\n", ch);
    else
        send_to_char("You accept followers.\r\n", ch);

    if (IS_SET(ch->act, PLR_NOCANCEL))
        send_to_char("You cannot have the cancel spell cast on you by other players.\r\n", ch);
    else
        send_to_char("You can now have the cancel spell cast on you by other players.\r\n", ch);

}

void do_autoassist(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOASSIST))
    {
        send_to_char("Autoassist removed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOASSIST);
    }
    else
    {
        send_to_char("You will now assist when needed.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOEXIT))
    {
        send_to_char("Exits will no longer be displayed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOEXIT);
    }
    else
    {
        send_to_char("Exits will now be displayed.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOGOLD))
    {
        send_to_char("Autogold removed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOGOLD);
    }
    else
    {
        send_to_char("Automatic gold looting set.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOLOOT))
    {
        send_to_char("Autolooting removed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOLOOT);
    }
    else
    {
        send_to_char("Automatic corpse looting set.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOSAC))
    {
        send_to_char("Autosacrificing removed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOSAC);
    }
    else
    {
        send_to_char("Automatic corpse sacrificing set.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_AUTOSPLIT))
    {
        send_to_char("Autosplitting removed.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_AUTOSPLIT);
    }
    else
    {
        send_to_char("Automatic gold splitting set.\r\n", ch);
        SET_BIT(ch->act, PLR_AUTOSPLIT);
    }
}

void do_autoall(CHAR_DATA *ch, char * argument)
{
    if (IS_NPC(ch))
        return;

    if (!strcmp(argument, "on"))
    {
        SET_BIT(ch->act, PLR_AUTOASSIST);
        SET_BIT(ch->act, PLR_AUTOEXIT);
        SET_BIT(ch->act, PLR_AUTOGOLD);
        SET_BIT(ch->act, PLR_AUTOLOOT);
        SET_BIT(ch->act, PLR_AUTOSAC);
        SET_BIT(ch->act, PLR_AUTOSPLIT);

        send_to_char("All autos turned on.\r\n", ch);
    }
    else if (!strcmp(argument, "off"))
    {
        REMOVE_BIT(ch->act, PLR_AUTOASSIST);
        REMOVE_BIT(ch->act, PLR_AUTOEXIT);
        REMOVE_BIT(ch->act, PLR_AUTOGOLD);
        REMOVE_BIT(ch->act, PLR_AUTOLOOT);
        REMOVE_BIT(ch->act, PLR_AUTOSAC);
        REMOVE_BIT(ch->act, PLR_AUTOSPLIT);

        send_to_char("All autos turned off.\r\n", ch);
    }
    else
        send_to_char("Usage: autoall [on|off]\r\n", ch);
}

void do_brief(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_BRIEF))
    {
        send_to_char("Full descriptions activated.\r\n", ch);
        REMOVE_BIT(ch->comm, COMM_BRIEF);
    }
    else
    {
        send_to_char("Short descriptions activated.\r\n", ch);
        SET_BIT(ch->comm, COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_COMPACT))
    {
        send_to_char("Compact mode removed.\r\n", ch);
        REMOVE_BIT(ch->comm, COMM_COMPACT);
    }
    else
    {
        send_to_char("Compact mode set.\r\n", ch);
        SET_BIT(ch->comm, COMM_COMPACT);
    }
}

void do_show(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
    {
        send_to_char("Affects will no longer be shown in score.\r\n", ch);
        REMOVE_BIT(ch->comm, COMM_SHOW_AFFECTS);
    }
    else
    {
        send_to_char("Affects will now be shown in score.\r\n", ch);
        SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0')
    {
        if (IS_SET(ch->comm, COMM_PROMPT))
        {
            send_to_char("You will no longer see prompts.\r\n", ch);
            REMOVE_BIT(ch->comm, COMM_PROMPT);
        }
        else
        {
            send_to_char("You will now see prompts.\r\n", ch);
            SET_BIT(ch->comm, COMM_PROMPT);

            if (ch->prompt != NULL)
            {
                sprintf(buf, "Your current prompt is: %s\r\n", ch->prompt);
                send_to_char(buf, ch);
            }

        }
        return;
    }

    // Rhien, 04/10/2015, added advanced prompt as the start prompt, can default
    // back to the basic one if desired.  Added default also because I keep typing it. :p
    if (!strcmp(argument, "all") || !strcmp(argument, "reset") || !strcmp(argument, "default")) {
        strcpy(buf, "<%hhp %mm %vmv {g%r {x({C%e{x)>{x ");
    }
    else if (!strcmp(argument, "basic")) {
        strcpy(buf, "<%hhp %mm %vmv> ");
    }
    else if (!strcmp(argument, "list") || !strcmp(argument, "current"))
    {
        if (ch->prompt != NULL)
        {
            sprintf(buf, "Your current prompt is: %s\r\n", ch->prompt);
            send_to_char(buf, ch);
            return;
        }
        else
        {
            send_to_char("You do not currently have a prompt set.\r\n", ch);
            return;
        }

    }
    else
    {
        if (strlen(argument) > 50)
            argument[50] = '\0';
        strcpy(buf, argument);
        smash_tilde(buf);
        if (str_suffix("%c", buf))
            strcat(buf, " ");

    }

    free_string(ch->prompt);
    ch->prompt = str_dup(buf);
    sprintf(buf, "Prompt set to %s\r\n", ch->prompt);
    send_to_char(buf, ch);
    return;
}

void do_combine(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_COMBINE))
    {
        send_to_char("Long inventory selected.\r\n", ch);
        REMOVE_BIT(ch->comm, COMM_COMBINE);
    }
    else
    {
        send_to_char("Combined inventory selected.\r\n", ch);
        SET_BIT(ch->comm, COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act, PLR_CANLOOT))
    {
        send_to_char("Your corpse is now safe from thieves.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_CANLOOT);
    }
    else
    {
        send_to_char("Your corpse may now be looted.\r\n", ch);
        SET_BIT(ch->act, PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM))
        return;

    if (IS_SET(ch->act, PLR_NOFOLLOW))
    {
        send_to_char("You now accept followers.\r\n", ch);
        REMOVE_BIT(ch->act, PLR_NOFOLLOW);
    }
    else
    {
        send_to_char("You no longer accept followers.\r\n", ch);
        SET_BIT(ch->act, PLR_NOFOLLOW);
        die_follower(ch);
    }
}

void do_nocancel(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->act,PLR_NOCANCEL))
    {
        send_to_char("You can now be cancelled by others.\n\r", ch);
        REMOVE_BIT(ch->act,PLR_NOCANCEL);
    }
    else
    {
        send_to_char("You are now immune to cancel by others.\n\r", ch);
        SET_BIT(ch->act, PLR_NOCANCEL);
    }
}

void do_nosummon(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
    {
        if (IS_SET(ch->imm_flags, IMM_SUMMON))
        {
            send_to_char("You are no longer immune to summon.\r\n", ch);
            REMOVE_BIT(ch->imm_flags, IMM_SUMMON);
        }
        else
        {
            send_to_char("You are now immune to summoning.\r\n", ch);
            SET_BIT(ch->imm_flags, IMM_SUMMON);
        }
    }
    else
    {
        if (IS_SET(ch->act, PLR_NOSUMMON))
        {
            send_to_char("You are no longer immune to summon.\r\n", ch);
            REMOVE_BIT(ch->act, PLR_NOSUMMON);
        }
        else
        {
            send_to_char("You are now immune to summoning.\r\n", ch);
            SET_BIT(ch->act, PLR_NOSUMMON);
        }
    }
}

void do_look(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number, count;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;

    if (ch->desc == NULL)
        return;

    if (ch->position < POS_SLEEPING)
    {
        send_to_char("You can't see anything but stars!\r\n", ch);
        return;
    }

    if (ch->position == POS_SLEEPING)
    {
        send_to_char("You can't see anything, you're sleeping!\r\n", ch);
        return;
    }

    if (!check_blind(ch))
        return;

    if (!IS_NPC(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT) && room_is_dark(ch->in_room))
    {
        send_to_char("It is pitch black ... \r\n", ch);
        show_char_to_char(ch->in_room->people, ch);
        return;
    }

    // Check for whether fog exists in the room and whether the player can see through
    // it via the check_fog function.
    if (check_fog(ch))
    {
        // We have fog
        sprintf(buf, "{c%s [%s]{x\r\n", ch->in_room->name, ch->in_room->area->name);
        send_to_char(buf, ch);
        send_to_char("  The fog here is too thick to see through.\r\n", ch);
        return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    number = number_argument(arg1, arg3);
    count = 0;

    if (arg1[0] == '\0' || !str_cmp(arg1, "auto"))
    {
        /* 'look' or 'look auto' */
        sprintf(buf, "{c%s [%s]{x", ch->in_room->name, ch->in_room->area->name);
        send_to_char(buf, ch);

        if ((IS_IMMORTAL(ch)
            && (IS_NPC(ch) || IS_SET(ch->act, PLR_HOLYLIGHT)))
            || IS_BUILDER(ch, ch->in_room->area))
        {
            sprintf(buf, "{r [{RRoom %d{r]{x", ch->in_room->vnum);
            send_to_char(buf, ch);
        }

        send_to_char("\r\n", ch);

        if (arg1[0] == '\0' || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)))
        {
            send_to_char("  ", ch);
            send_to_char(ch->in_room->description, ch);
            send_to_char("{x", ch);
        }

        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT))
        {
            send_to_char("\r\n", ch);
            do_function(ch, &do_exits, "auto");
        }

        show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);
        show_char_to_char(ch->in_room->people, ch);
        return;
    }

    if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in")
        || !str_cmp(arg1, "on"))
    {
        /* 'look in' */
        if (arg2[0] == '\0')
        {
            send_to_char("Look in what?\r\n", ch);
            return;
        }

        if ((obj = get_obj_here(ch, arg2)) == NULL)
        {
            send_to_char("You do not see that here.\r\n", ch);
            return;
        }

        switch (obj->item_type)
        {
            default:
                send_to_char("That is not a container.\r\n", ch);
                break;

            case ITEM_DRINK_CON:
                if (obj->value[1] <= 0)
                {
                    send_to_char("It is empty.\r\n", ch);
                    break;
                }

                sprintf(buf, "It's %sfilled with a %s liquid.\r\n",
                    obj->value[1] < obj->value[0] / 4
                    ? "less than half-" :
                    obj->value[1] < 3 * obj->value[0] / 4
                    ? "about half-" : "more than half-",
                    liq_table[obj->value[2]].liq_color);

                send_to_char(buf, ch);
                break;

            case ITEM_CONTAINER:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                if (IS_SET(obj->value[1], CONT_CLOSED))
                {
                    send_to_char("It is closed.\r\n", ch);
                    break;
                }

                act("$p holds:", ch, obj, NULL, TO_CHAR);
                show_list_to_char(obj->contains, ch, TRUE, TRUE);
                break;
            case ITEM_PORTAL:
                act( "$n peers into $o.", ch, obj, NULL, TO_ROOM);
                act( "You peer into $o.", ch, obj, NULL, TO_CHAR);

                if (IS_SET(obj->value[2], GATE_RANDOM) || obj->value[3] == -1)
                {
                    // If the portal has no destination or it's set to GATE_RANDOM
                    location = get_random_room(ch);
                    obj->value[3] = location->vnum; /* for record keeping :) */
                }
                else if (IS_SET(obj->value[2], GATE_BUGGY) && (number_percent() < 5))
                {
                    // If the portal is buggy... randomness
                    location = get_random_room(ch);
                }
                else
                {
                    // And, the case hit almost always
                    location = get_room_index(obj->value[3]);
                }

                // Make the check to see if they can see to the other side or not
                if (location == NULL
                    || location == ch->in_room
                    || !can_see_room(ch, location)
                    || ch->fighting != NULL
                    || (room_is_private(location)
                    && !is_room_owner(ch, location)))
                {
                    send_to_char("You see swirling chaos...\r\n", ch);
                    return;
                }

                // Show the other side to the player, this will move them, look and then
                // move them back.
                original = ch->in_room;
                char_from_room(ch);
                char_to_room(ch, location);
                do_look(ch, "auto");
                char_from_room(ch);
                char_to_room(ch, original);
                return;
                break;
        }
        return;
    }

    if ((victim = get_char_room(ch, arg1)) != NULL)
    {
        show_char_to_char_1(victim, ch);
        return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
        if (can_see_obj(ch, obj))
        {                        /* player can see object */
            pdesc = get_extra_descr(arg3, obj->extra_descr);
            if (pdesc != NULL)
            {
                if (++count == number)
                {
                    send_to_char(pdesc, ch);
                    show_lore(ch, obj);
                    return;
                }
                else
                    continue;
            }

            pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
            if (pdesc != NULL)
            {
                if (++count == number)
                {
                    send_to_char(pdesc, ch);
                    show_lore(ch, obj);
                    return;
                }
                else
                    continue;
            }

            if (is_name(arg3, obj->name))
                if (++count == number)
                {
                    send_to_char(obj->description, ch);
                    send_to_char("\r\n", ch);
                    show_lore(ch, obj);
                    return;
                }
        }
    }

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
    {
        if (can_see_obj(ch, obj))
        {
            pdesc = get_extra_descr(arg3, obj->extra_descr);
            if (pdesc != NULL)
                if (++count == number)
                {
                    send_to_char(pdesc, ch);
                    return;
                }

            pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
            if (pdesc != NULL)
                if (++count == number)
                {
                    send_to_char(pdesc, ch);
                    return;
                }

            if (is_name(arg3, obj->name))
                if (++count == number)
                {
                    send_to_char(obj->description, ch);
                    send_to_char("\r\n", ch);
                    return;
                }
        }
    }

    pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
        if (++count == number)
        {
            send_to_char(pdesc, ch);
            return;
        }
    }

    if (count > 0 && count != number)
    {
        if (count == 1)
            sprintf(buf, "You only see one %s here.\r\n", arg3);
        else
            sprintf(buf, "You only see %d of those here.\r\n", count);

        send_to_char(buf, ch);
        return;
    }

    if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
        door = 0;
    else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
        door = 1;
    else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
        door = 2;
    else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
        door = 3;
    else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
        door = 4;
    else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
        door = 5;
    else if (!str_cmp(arg1, "nw") || !str_cmp(arg1, "northwest"))
        door = 6;
    else if (!str_cmp(arg1, "ne") || !str_cmp(arg1, "northeast"))
        door = 7;
    else if (!str_cmp(arg1, "sw") || !str_cmp(arg1, "southwest"))
        door = 8;
    else if (!str_cmp(arg1, "se") || !str_cmp(arg1, "southeast"))
        door = 9;
    else
    {
        send_to_char("You do not see that here.\r\n", ch);
        return;
    }

    /* 'look direction' */
    if ((pexit = ch->in_room->exit[door]) == NULL)
    {
        send_to_char("Nothing special there.\r\n", ch);
        return;
    }

    if (pexit->description != NULL && pexit->description[0] != '\0')
        send_to_char(pexit->description, ch);
    else
        send_to_char("Nothing special there.\r\n", ch);

    if (pexit->keyword != NULL
        && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ')
    {
        if (IS_SET(pexit->exit_info, EX_CLOSED))
        {
            act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
        }
        else if (IS_SET(pexit->exit_info, EX_ISDOOR))
        {
            act("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
        }
    }

    return;
}

// do_scan setup
char *const distance[4] = {
    "right here.", "nearby to the %s.", "not far %s.",
    "off in the distance %s."
};

void scan_list(ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch, int depth, int door);
void scan_char(CHAR_DATA * victim, CHAR_DATA * ch, int depth, int door);

void do_scan(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *scan_room;
    EXIT_DATA *pExit;
    int door, depth;

    argument = one_argument(argument, arg1);

    if (arg1[0] == '\0')
    {
        act("$n looks all around.", ch, NULL, NULL, TO_ROOM);
        send_to_char("Looking around you see:\r\n", ch);
        scan_list(ch->in_room, ch, 0, -1);

        for (door = 0; door < 9; door++)
        {
            if ((pExit = ch->in_room->exit[door]) != NULL)
                scan_list(pExit->u1.to_room, ch, 1, door);
        }
        return;
    }
    else if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
        door = 0;
    else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
        door = 1;
    else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
        door = 2;
    else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
        door = 3;
    else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
        door = 4;
    else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
        door = 5;
    else if (!str_cmp(arg1, "nw") || !str_cmp(arg1, "northwest"))
        door = 6;
    else if (!str_cmp(arg1, "ne") || !str_cmp(arg1, "northeast"))
        door = 7;
    else if (!str_cmp(arg1, "sw") || !str_cmp(arg1, "southwest"))
        door = 8;
    else if (!str_cmp(arg1, "se") || !str_cmp(arg1, "southeast"))
        door = 9;
    else
    {
        send_to_char("Which way do you want to scan?\r\n", ch);
        return;
    }

    act("You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR);
    act("$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM);
    sprintf(buf, "Looking %s you see:\r\n", dir_name[door]);

    scan_room = ch->in_room;

    for (depth = 1; depth < 4; depth++)
    {
        if ((pExit = scan_room->exit[door]) != NULL)
        {
            scan_room = pExit->u1.to_room;
            scan_list(pExit->u1.to_room, ch, depth, door);
        }
    }
    return;
}

void scan_list(ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch, int depth, int door)
{
    CHAR_DATA *rch;

    if (scan_room == NULL)
        return;
    for (rch = scan_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (rch == ch)
            continue;
        if (!IS_NPC(rch) && rch->invis_level > get_trust(ch))
            continue;
        if (can_see(ch, rch))
            scan_char(rch, ch, depth, door);
    }
    return;
}

void scan_char(CHAR_DATA * victim, CHAR_DATA * ch, int depth, int door)
{
    extern char *const dir_name[];
    extern char *const distance[];
    char buf[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];

    buf[0] = '\0';

    strcat(buf, PERS(victim, ch));
    strcat(buf, ", ");
    sprintf(buf2, distance[depth], dir_name[door]);
    strcat(buf, buf2);
    strcat(buf, "\r\n");

    send_to_char(buf, ch);
    return;
}

void do_examine(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Examine what?\r\n", ch);
        return;
    }

    do_function(ch, &do_look, arg);

    if ((obj = get_obj_here(ch, arg)) != NULL)
    {
        switch (obj->item_type)
        {
            default:
                break;

            case ITEM_MONEY:
                if (obj->value[0] == 0)
                {
                    if (obj->value[1] == 0)
                        sprintf(buf,
                            "Odd...there's no coins in the pile.\r\n");
                    else if (obj->value[1] == 1)
                        sprintf(buf, "Wow. One gold coin.\r\n");
                    else
                        sprintf(buf,
                            "There are %d gold coins in the pile.\r\n",
                            obj->value[1]);
                }
                else if (obj->value[1] == 0)
                {
                    if (obj->value[0] == 1)
                        sprintf(buf, "Wow. One silver coin.\r\n");
                    else
                        sprintf(buf,
                            "There are %d silver coins in the pile.\r\n",
                            obj->value[0]);
                }
                else
                    sprintf(buf,
                        "There are %d gold and %d silver coins in the pile.\r\n",
                        obj->value[1], obj->value[0]);
                send_to_char(buf, ch);
                break;

            case ITEM_DRINK_CON:
            case ITEM_CONTAINER:
            case ITEM_CORPSE_NPC:
            case ITEM_CORPSE_PC:
                sprintf(buf, "in %s", argument);
                do_function(ch, &do_look, buf);
        }
    }

    return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    fAuto = !str_cmp(argument, "auto");

    if (!check_blind(ch))
        return;

    if (fAuto)
        sprintf(buf, "[Exits:");
    else if (IS_IMMORTAL(ch))
        sprintf(buf, "Obvious exits from room %d:\r\n", ch->in_room->vnum);
    else
        sprintf(buf, "Obvious exits:\r\n");

    found = FALSE;
    for (door = 0; door < MAX_DIR; door++)
    {
        if ((pexit = ch->in_room->exit[door]) != NULL
            && pexit->u1.to_room != NULL
            && can_see_room(ch, pexit->u1.to_room))
        {

            found = TRUE;
            if (fAuto)
            {
                // Open for the door indicator
                if (IS_SET(pexit->exit_info, EX_CLOSED))
                {
                    strcat(buf, " (");
                }
                else
                {
                    strcat(buf, " ");
                }

                // Show the ocean and underwater exits as a light cyan
                if (pexit->u1.to_room->sector_type == SECT_OCEAN
                    || pexit->u1.to_room->sector_type == SECT_UNDERWATER)
                {
                    strcat(buf, "{c");
                    strcat(buf, dir_name[door]);
                    strcat(buf, "{x");
                }
                else
                {
                    // Default coloring, normal exit
                    strcat(buf, dir_name[door]);
                }

                // Close for the door indicator
                if (IS_SET(pexit->exit_info, EX_CLOSED))
                {
                    strcat(buf, ")");
                }

            }
            else
            {
                // If the player (immortal) has holy light on always show them the
                // room name, otherwise if it's a player see if the room is dark.
                if (IS_SET(ch->act, PLR_HOLYLIGHT))
                {
                    sprintf(buf + strlen(buf), "%-9s - %s",
                        capitalize(dir_name[door]),
                        pexit->u1.to_room->name);
                }
                else
                {
                    sprintf(buf + strlen(buf), "%-9s - %s",
                        capitalize(dir_name[door]),
                        room_is_dark(pexit->u1.to_room)
                        ? "Too dark to tell" : pexit->u1.to_room->name);
                }

                // Immortals additionally see the vnum of the room
                if (IS_IMMORTAL(ch))
                {
                    sprintf(buf + strlen(buf), " (room %d)\r\n", pexit->u1.to_room->vnum);
                }
                else
                {
                    sprintf(buf + strlen(buf), "\r\n");
                }
            }
        }
    }

    if (!found)
        strcat(buf, fAuto ? " none" : "None.\r\n");

    if (fAuto)
        strcat(buf, "]{x\r\n");

    // If immortal Holy Light is set, show all the available exits
    if (IS_SET(ch->act, PLR_HOLYLIGHT) && fAuto)
    {
        buf[0] = '\0';
        strcat(buf, "[Exits:");

        found = FALSE;
        for (door = 0; door < MAX_DIR; door++)
        {
            if ((pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room != NULL)
            {
                found = TRUE;

                // Open for the door indicator
                if (IS_SET(pexit->exit_info, EX_CLOSED))
                {
                    strcat(buf, " (");
                }
                else
                {
                    strcat(buf, " ");
                }

                if (pexit->u1.to_room->sector_type == SECT_OCEAN
                    || pexit->u1.to_room->sector_type == SECT_UNDERWATER)
                {
                    strcat(buf, "{c");
                    strcat(buf, dir_name[door]);
                    strcat(buf, "{x");
                }
                else
                {
                    // Default coloring, normal exit
                    strcat(buf, dir_name[door]);
                }

                // Open for the door indicator
                if (IS_SET(pexit->exit_info, EX_CLOSED))
                {
                    strcat(buf, ")");
                }

            }
        }

        if (!found)
        {
            strcat(buf, fAuto ? " none" : "None.\r\n");
        }

        strcat(buf, "]\r\n");
    }

    send_to_char(buf, ch);
    return;
}

void do_worth(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
        sprintf(buf, "You have %ld gold and %ld silver.\r\n",
            ch->gold, ch->silver);
        send_to_char(buf, ch);
        return;
    }

    sprintf(buf,
        "You have %ld gold, %ld silver, %d quest points and %d experience (%d exp to level).\r\n",
        ch->gold, ch->silver, ch->pcdata->quest_points, ch->exp,
        (ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp);

    send_to_char(buf, ch);

    return;
}

/*
 * Show all useful player data in a basic score card.
 */
void do_score(CHAR_DATA * ch, char *argument)
{
    GRID_DATA *grid;
    GRID_ROW *row;
    //GRID_CELL *cell;
    char buf[MAX_STRING_LENGTH];
    char center_text[MAX_STRING_LENGTH];
    OBJ_DATA *wield;
    OBJ_DATA *dual_wield;

    // Create the grid
    grid = create_grid(75);

    // Row 1
    // capitalize uses static memory therefore it will overwrite the second variable
    // where it's used in a *printf.
    sprintf(buf, "%s", capitalize(class_table[ch->class]->name));

    row = create_row_padded(grid, 0, 0, 2, 2);

    if (!IS_NPC(ch))
    {
        sprintf(center_text, "{WScore for %s, %s %s{x",
            capitalize(ch->name), pc_race_table[ch->race].article_name, buf);
    }
    else
    {
        sprintf(center_text, "{WScore for %s, an NPC{x", capitalize(ch->name));
    }

    sprintf(center_text, "%s", center_string_padded(center_text, 71));

    row_append_cell(row, 75, center_text);

    // Row 2 - Headers
    row = create_row_padded(grid, 0, 0, 2, 2);
    row_append_cell(row, 21, "     {WStats{x");
    row_append_cell(row, 26, "    {WCurrent State{x");
    row_append_cell(row, 28, "      {WPlayer Info{x");

    // Row 3
    row = create_row_padded(grid, 0, 0, 2, 2);

    row_append_cell(row, 21, "  Str: {C%d{x({C%d{x)\n  Int: {C%d{x({C%d{x)\n  Wis: {C%d{x({C%d{x)\n  Dex: {C%d{x({C%d{x)\n  Con: {C%d{x({C%d{x)",
        ch->perm_stat[STAT_STR],
        get_curr_stat (ch, STAT_STR),
        ch->perm_stat[STAT_INT],
        get_curr_stat (ch, STAT_INT),
        ch->perm_stat[STAT_WIS],
        get_curr_stat (ch, STAT_WIS),
        ch->perm_stat[STAT_DEX],
        get_curr_stat (ch, STAT_DEX),
        ch->perm_stat[STAT_CON], get_curr_stat (ch, STAT_CON));

    // The immortal weight is boosted to a number that won't fit, just show 9999
    row_append_cell(row, 26, "    HP: {C%-4d{x of {C%-4d{x\n  Mana: {C%-4d{x of {C%-4d{x\n  Move: {C%-4d{x of {C%-4d{x\n Items: {C%-4d{x of {C%-4d{x\nWeight: {C%-4d{x of {C%-4d{x",
        ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move,
        ch->carry_number, can_carry_n(ch),
        get_carry_weight(ch) / 10, !IS_IMMORTAL(ch) ? can_carry_w(ch) / 10 : 9999);

    row_append_cell(row, 28, " Level: {C%d{x\nPlayed: {C%d hours{x\nGender: {C%s{x\n Align: {C%s{x\n   Age: {C%d{x",
        ch->level,
        (ch->played + (int) (current_time - ch->logon)) / 3600,
        ch->sex == 0 ? "No Gender" : ch->sex == 1 ? "Male" : "Female",
        IS_GOOD(ch) ? "Good" : IS_EVIL(ch) ? "Evil" : "Neutral",
        get_age(ch)
     );

    // Row 4 - Headers
    row = create_row_padded(grid, 0, 0, 2, 2);
    row_append_cell(row, 21, "     {WWorth{x");
    row_append_cell(row, 26, "       {WBattle{x");
    row_append_cell(row, 28, "  {WStatistics and Info{x");

    // Row 5
    row = create_row_padded(grid, 0, 0, 2, 2);

    row_append_cell(row, 21, "     Gold: {C%-4d{x\n   Silver: {C%-5d{x\n   Trains: {C%-3d{x\nPractices: {C%-3d{x\n Q-Points: {C%-5d{x",
        ch->gold, ch->silver,
        ch->train, ch->practice,
        !IS_NPC(ch) ? ch->pcdata->quest_points : 0);


    wield = get_eq_char(ch, WEAR_WIELD);
    dual_wield = get_eq_char(ch, WEAR_SECONDARY_WIELD);

    // If they are dual wielding (and have a secondary weapon wielded then show the hit roll/dam roll for that also.
    if (get_skill(ch, gsn_dual_wield) > 0 && dual_wield != NULL)
    {
        row_append_cell(row, 26, "AC Pierce: {C%d{x\n  AC Bash: {C%d{x\n AC Slash: {C%d{x\n AC Magic: {C%d{x\n   Stance: {C%s{x\nWimpy @HP: {C%d{x\n Hit Roll: {C%d %d %d{x\n Dam Roll: {C%d %d %d{x\n    Saves: {C%d{x",
            GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH), GET_AC(ch, AC_SLASH), GET_AC(ch, AC_EXOTIC),
            capitalize(get_stance_name(ch)), ch->wimpy,
            GET_HITROLL(ch, NULL), GET_HITROLL(ch, wield), GET_HITROLL(ch, dual_wield),
            GET_DAMROLL(ch, NULL), GET_DAMROLL(ch, wield), GET_DAMROLL(ch, dual_wield),
            ch->saving_throw
        );
    }
    else
    {
        row_append_cell(row, 26, "AC Pierce: {C%d{x\n  AC Bash: {C%d{x\n AC Slash: {C%d{x\n AC Magic: {C%d{x\n   Stance: {C%s{x\nWimpy @HP: {C%d{x\n Hit Roll: {C%d %d{x\n Dam Roll: {C%d %d{x\n    Saves: {C%d{x",
            GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH), GET_AC(ch, AC_SLASH), GET_AC(ch, AC_EXOTIC),
            capitalize(get_stance_name(ch)), ch->wimpy,
            GET_HITROLL(ch, NULL), GET_HITROLL(ch, wield),
            GET_DAMROLL(ch, NULL), GET_DAMROLL(ch, wield),
            ch->saving_throw
        );
    }

    row_append_cell(row, 28, "   Player Kills: {C%d{x\n  XP Next Level: {C%d{x\nCreation Points: {C%d{x\nPK Logout Timer: {C%d{x",
        !IS_NPC(ch) ? ch->pcdata->pkills : 0,
        !IS_NPC(ch) && ch->level < 51 ? (ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp : 0,
        !IS_NPC(ch) ? ch->pcdata->points : 0,
        !IS_NPC(ch) ? ch->pcdata->pk_timer : 0
    );

    grid_to_char (grid, ch, TRUE);

    if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
    {
        do_function (ch, &do_affects, "");
    }
}

/*
 * Shows the character any affects that they have on them.  This spell now shows
 * the specific affect and duration to characters of all levels (it used to hide
 * the specifics and durations to those under level 20.
 */
void do_affects(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];

    if (ch->affected != NULL)
    {
        send_to_char("You are affected by the following spells:\r\n", ch);
        for (paf = ch->affected; paf != NULL; paf = paf->next)
        {
            if (paf->type < 0)
            {
                // There won't be negagive index entries in the skills table, something has
                // gone wrong here, like eq trying to add an affect that doesn't exist.  Log
                // the error and go on.
                sprintf(buf, "%s had a bad affect type (%d).", ch->name, paf->type);
                bug(buf, 0);
                wiznet(buf, NULL, NULL, WIZ_GENERAL, 0, 0);
                continue;
            }

            if (paf_last != NULL && paf->type == paf_last->type)
            {
                // This is part of a previous spell that has multiple affects
                // so just push it over so it's clear that say bless lowers saves
                // and adds hit.
                sprintf(buf, "                           ");
            }
            else
            {
                sprintf(buf, "Spell: %-20s", skill_table[paf->type]->name);
            }

            send_to_char(buf, ch);

            sprintf(buf, ": modifies %s by %d ", affect_loc_name(paf->location), paf->modifier);
            send_to_char(buf, ch);

            if (paf->duration == -1)
            {
                sprintf(buf, "permanently");
            }
            else
            {
                sprintf(buf, "for %d hours", paf->duration);
            }

            send_to_char(buf, ch);

            send_to_char("\r\n", ch);
            paf_last = paf;
        }
    }
    else
    {
        send_to_char("You are not affected by any spells.\r\n", ch);
    }
    return;
} // end do_affects



char *const day_name[] = {
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *const month_name[] = {
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day = time_info.day + 1;

    if (day > 4 && day < 20)
        suf = "th";
    else if (day % 10 == 1)
        suf = "st";
    else if (day % 10 == 2)
        suf = "nd";
    else if (day % 10 == 3)
        suf = "rd";
    else
        suf = "th";

    sprintf(buf,
        "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\r\n\r\n",
        (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
        time_info.hour >= 12 ? "pm" : "am",
        day_name[day % 7], day, suf, month_name[time_info.month]);
    send_to_char(buf, ch);

    if (!IS_NULLSTR(settings.mud_name))
    {
        sprintf(buf, "%s started up on %s\r\n", settings.mud_name, global.boot_time);
    }
    else
    {
        sprintf(buf, "The game started up on %s\r\n", global.boot_time);
    }

    send_to_char(buf, ch);

    sprintf(buf, "The system time is %s", (char *)ctime(&current_time));
    send_to_char(buf, ch);

    return;
}



void do_weather(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    static char *const sky_look[4] = {
        "cloudless",
        "cloudy",
        "rainy",
        "lit by flashes of lightning"
    };

    if (!IS_OUTSIDE(ch))
    {
        send_to_char("You can't see the weather indoors.\r\n", ch);
        return;
    }

    sprintf(buf, "The sky is %s and %s.\r\n",
        sky_look[weather_info.sky],
        weather_info.change >= 0
        ? "a warm southerly breeze blows"
        : "a cold northern gust blows");
    send_to_char(buf, ch);
    return;
}

/*
 * Help command to access the games help files that are loaded into memory
 * at startup (helps are storedin in area files).
 */
void do_help(CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if (argument[0] == '\0')
        argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0')
    {
        argument = one_argument(argument, argone);
        if (argall[0] != '\0')
            strcat(argall, " ");
        strcat(argall, argone);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
        level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

        if (level > get_trust(ch))
            continue;

        if (is_name(argall, pHelp->keyword))
        {
            /* add seperator if found */
            if (found)
                add_buf(output,
                    "\r\n================================================================================\r\n\r\n");
            if (pHelp->level >= 0 && str_cmp(argall, "imotd"))
            {
                add_buf(output, pHelp->keyword);
                add_buf(output, "\r\n");
            }

            /*
             * Strip leading '.' to allow initial blanks.
             */
            if (pHelp->text[0] == '.')
                add_buf(output, pHelp->text + 1);
            else
                add_buf(output, pHelp->text);
            found = TRUE;
            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
                && ch->desc->connected != CON_GEN_GROUPS)
                break;
        }
    }

    if (!found)
    {
        send_to_char("No help on that word.\r\n", ch);

        /*
         * Let's log unmet help requests so studious IMP's can improve their help files ;-)
         * But to avoid idiots, we will check the length of the help request, and trim to
         * a reasonable length (set it by redefining MAX_CMD_LEN in merc.h).  -- JR
         */
        if (strlen(argall) > MAX_CMD_LEN)
        {
            argall[MAX_CMD_LEN - 1] = '\0';
            log_f("Excessive command length: %s requested %s.", ch->name, argall);
        }
        /* OHELPS_FILE is the "orphaned helps" files. Defined in merc.h -- JR */
        else
        {
            append_file(ch, OHELPS_FILE, argall);
        }
    }
    else
        page_to_char(buf_string(output), ch);
    free_buf(output);
}

/*
 * Shows the player who they're logged in as, ya know, in case they forgot.
 */
void do_whoami(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    sprintf(buf, "You are logged on as %s.\r\n", ch->name);
    send_to_char(buf, ch);
} // end do_whoami

/*
 * whois command allows a player to specifically look to see if another player
 * is logged in (if they can see them that is).
 */
void do_whois(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char immbuf[15];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("You must provide a name.\r\n", ch);
        return;
    }

    output = new_buf();

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *wch;
        char const *class;

        if (d->connected != CON_PLAYING || !can_see(ch, d->character))
            continue;

        wch = (d->original != NULL) ? d->original : d->character;

        if (!can_see(ch, wch))
            continue;

        if (!str_prefix(arg, wch->name))
        {
            found = TRUE;

            /* work out the printing */
            class = class_table[wch->class]->who_name;
            switch (wch->level)
            {
                case MAX_LEVEL - 0:
                    sprintf(immbuf, "%s", " IMPLEMENTOR ");
                    break;
                case MAX_LEVEL - 1:
                    sprintf(immbuf, "%s", "    ADMIN    ");
                    break;
                case MAX_LEVEL - 2:
                    sprintf(immbuf, "%s", "    CODER    ");
                    break;
                case MAX_LEVEL - 3:
                    sprintf(immbuf, "%s", "   BUILDER   ");
                    break;
                case MAX_LEVEL - 4:
                    sprintf(immbuf, "%s", "    QUEST    ");
                    break;
                case MAX_LEVEL - 5:
                    sprintf(immbuf, "%s", "  ROLE-PLAY  ");
                    break;
                case MAX_LEVEL - 6:
                    sprintf(immbuf, "%s", "    STORY    ");
                    break;
                case MAX_LEVEL - 7:
                    sprintf(immbuf, "%s", "   RETIRED   ");
                    break;
                case MAX_LEVEL - 8:
                    sprintf(immbuf, "%s", "  TRIAL IMM  ");
                    break;
                default:
                    sprintf(immbuf, "%2d %6s %s", wch->level,
                        wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
                    class);
            }

            /* a little formatting */
            if (!IS_IMMORTAL(wch))
            {
                sprintf(buf, "[%2d %6s %s] %s%s%s%s%s%s%s%s%s\r\n",
                    wch->level,
                    wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
                class,
                    wch->incog_level >= LEVEL_HERO ? "({wIncog{x) " : "",
                    wch->invis_level >= LEVEL_HERO ? "({wWizi{x) " : "",
                    clan_table[wch->clan].who_name,
                    IS_SET(wch->comm, COMM_AFK) ? "[{cAFK{x] " : "",
                    IS_SET(wch->comm, COMM_QUIET) ? "[{cQuiet{x] " : "",
                    IS_TESTER(wch) ? "({WTester{x) " : "",
                    IS_SET(wch->act, PLR_WANTED) ? "({RWANTED{x) " : "",
                    wch->name,
                    IS_NPC(wch) ? "" : wch->pcdata->title);
            }
            else
            {
                sprintf(buf, "[%s] %s%s%s%s%s%s%s%s%s\r\n",
                    immbuf,
                    wch->incog_level >= LEVEL_HERO ? "({wIncog{x) " : "",
                    wch->invis_level >= LEVEL_HERO ? "({wWizi{x) " : "",
                    clan_table[wch->clan].who_name,
                    IS_SET(wch->comm, COMM_AFK) ? "[{cAFK{x] " : "",
                    IS_SET(wch->comm, COMM_QUIET) ? "[{cQuiet{x] " : "",
                    IS_TESTER(wch) ? "({WTester{x) " : "",
                    IS_SET(wch->act, PLR_WANTED) ? "({RWANTED{x) " : "",
                    wch->name,
                    IS_NPC(wch) ? "" : wch->pcdata->title);
            }
            add_buf(output, buf);
        }
    }

    if (!found)
    {
        send_to_char("No one of that name is playing.\r\n", ch);
        return;
    }

    page_to_char(buf_string(output), ch);
    free_buf(output);
} // end do_whois

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char immbuf[15];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    extern int top_class;
    int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;

#if !defined(_WIN32)
    bool rgfClass[top_class];
#else
    // This is a hack for Windows/Visual C++, the gcc compiler allows for variable length arrays so
    // top_class is the number of classes dynamically read in.  This is akin to the old
    // MAX_CLASS (which now defines the ceiling and not the actual max classes).
    bool rgfClass[9];
#endif

    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[MAX_CLAN];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;

    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < top_class; iClass++)
        rgfClass[iClass] = FALSE;
    for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < MAX_CLAN; iClan++)
        rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for (;;)
    {
        char arg[MAX_STRING_LENGTH];

        argument = one_argument(argument, arg);
        if (arg[0] == '\0')
            break;

        if (is_number(arg))
        {
            switch (++nNumber)
            {
                case 1:
                    iLevelLower = atoi(arg);
                    break;
                case 2:
                    iLevelUpper = atoi(arg);
                    break;
                default:
                    send_to_char("Only two level numbers allowed.\r\n", ch);
                    return;
            }
        }
        else
        {

            /*
             * Look for classes to turn on.
             */
            if (!str_prefix(arg, "immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else
            {
                iClass = class_lookup(arg);
                if (iClass == -1)
                {
                    iRace = race_lookup(arg);

                    if (iRace == 0 || iRace >= MAX_PC_RACE)
                    {
                        if (!str_prefix(arg, "clan"))
                            fClan = TRUE;
                        else
                        {
                            iClan = clan_lookup(arg);
                            if (iClan)
                            {
                                fClanRestrict = TRUE;
                                rgfClan[iClan] = TRUE;
                            }
                            else
                            {
                                send_to_char
                                    ("That's not a valid race, class, or clan.\r\n",
                                        ch);
                                return;
                            }
                        }
                    }
                    else
                    {
                        fRaceRestrict = TRUE;
                        rgfRace[iRace] = TRUE;
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }
        }
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *wch;
        char const *class;

        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if (d->connected != CON_PLAYING || !can_see(ch, d->character))
            continue;

        wch = (d->original != NULL) ? d->original : d->character;

        if (!can_see(ch, wch))
            continue;

        if (wch->level < iLevelLower
            || wch->level > iLevelUpper
            || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
            || (fClassRestrict && !rgfClass[wch->class])
            || (fRaceRestrict && !rgfRace[wch->race])
            || (fClan && !is_clan(wch))
            || (fClanRestrict && !rgfClan[wch->clan]))
            continue;

        nMatch++;

        /*
         * Figure out what to print for class.
         */
        class = class_table[wch->class]->who_name;
        switch (wch->level)
        {
            case MAX_LEVEL - 0:
                sprintf(immbuf, "%s", " IMPLEMENTOR ");
                break;
            case MAX_LEVEL - 1:
                sprintf(immbuf, "%s", "    ADMIN    ");
                break;
            case MAX_LEVEL - 2:
                sprintf(immbuf, "%s", "    CODER    ");
                break;
            case MAX_LEVEL - 3:
                sprintf(immbuf, "%s", "   BUILDER   ");
                break;
            case MAX_LEVEL - 4:
                sprintf(immbuf, "%s", "    QUEST    ");
                break;
            case MAX_LEVEL - 5:
                sprintf(immbuf, "%s", "  ROLE-PLAY  ");
                break;
            case MAX_LEVEL - 6:
                sprintf(immbuf, "%s", "    STORY    ");
                break;
            case MAX_LEVEL - 7:
                sprintf(immbuf, "%s", "   RETIRED   ");
                break;
            case MAX_LEVEL - 8:
                sprintf(immbuf, "%s", "  TRIAL IMM  ");
                break;
            default:
                sprintf(immbuf, "%2d %6s %s", wch->level,
                    wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
                class);
        }

    /*
     * Format it up.
     */
        if (!IS_IMMORTAL(wch))
        {
            sprintf(buf, "[%2d %6s %s] %s%s%s%s%s%s%s%s%s\r\n",
                wch->level,
                wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
            class,
                wch->incog_level >= LEVEL_HERO ? "({wIncog{x) " : "",
                wch->invis_level >= LEVEL_HERO ? "({wWizi{x) " : "",
                clan_table[wch->clan].who_name,
                IS_SET(wch->comm, COMM_AFK) ? "[{cAFK{x] " : "",
                IS_SET(wch->comm, COMM_QUIET) ? "[{cQuiet{x] " : "",
                IS_TESTER(wch) ? "({WTester{x) " : "",
                IS_SET(wch->act, PLR_WANTED) ? "({RWANTED{x) " : "",
                wch->name,
                IS_NPC(wch) ? "" : wch->pcdata->title);
        }
        else
        {
            sprintf(buf, "[%s] %s%s%s%s%s%s%s%s%s\r\n",
                immbuf,
                wch->incog_level >= LEVEL_HERO ? "({wIncog{x) " : "",
                wch->invis_level >= LEVEL_HERO ? "({wWizi{x) " : "",
                clan_table[wch->clan].who_name,
                IS_SET(wch->comm, COMM_AFK) ? "[{cAFK{x] " : "",
                IS_SET(wch->comm, COMM_QUIET) ? "[{cQuiet{x] " : "",
                IS_TESTER(wch) ? "({WTester{x) " : "",
                IS_SET(wch->act, PLR_WANTED) ? "({RWANTED{x) " : "",
                wch->name,
                IS_NPC(wch) ? "" : wch->pcdata->title);
        }
        add_buf(output, buf);
    }

    sprintf(buf2, "\r\nPlayers found: %d\r\n", nMatch);
    add_buf(output, buf2);
    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
} // end do_who

/*
 * Counts and shows the number of players currently on as well as showing
 * the historical statisic for what the most we've ever had on is.
 */
void do_count(CHAR_DATA * ch, char *argument)
{
    int count;
    char buf[MAX_STRING_LENGTH];

    count = player_online_count();

    global.max_on_boot = UMAX(count, global.max_on_boot);

    if (global.max_on_boot == count)
    {
        sprintf(buf, "There are %d characters on, the most so far today.\r\n", count);
    }
    else
    {
        sprintf(buf, "There are %d characters on, the most on today was %d.\r\n", count, global.max_on_boot);
    }

    send_to_char(buf, ch);

    sprintf(buf, "The most we've ever had on was {R%d{x.\r\n", statistics.max_players_online);
    send_to_char(buf, ch);

} // end do_count

/*
 * Returns the current number of players online (connected == CON_PLAYING)
 */
int player_online_count()
{
    int count = 0;
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING)
            count++;
    }

    return count;
}

/*
 * This procedure will check the number of players online and if it's greater than the
 * statistic for max players online will set that and then save the statistics.
 */
void max_players_check()
{
    DESCRIPTOR_DATA *d;
    int count = 0;

    // Check if we've broken our total online characters count.
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING)
            count++;
    }

    // This should happen rarely, but if the max players online is met go ahead
    // and also save the statistics (which is why we didn't use UMAX here).
    if (count > statistics.max_players_online)
    {
        statistics.max_players_online = count;
        save_statistics();
    }

} // end max_players_check

void do_inventory(CHAR_DATA * ch, char *argument)
{
    send_to_char("You are carrying:\r\n", ch);
    show_list_to_char(ch->carrying, ch, TRUE, TRUE);
    return;
}

void do_equipment(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    int iWear;

    send_to_char("You are using:\r\n", ch);
    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
        if ((obj = get_eq_char(ch, iWear)) == NULL)
        {
            send_to_char(where_name[iWear], ch);
            send_to_char("(Nothing)\r\n", ch);
            continue;
        }

        send_to_char(where_name[iWear], ch);
        if (can_see_obj(ch, obj))
        {
            send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
            send_to_char("\r\n", ch);
        }
        else
        {
            send_to_char("something.\r\n", ch);
        }
    }

    return;
}



void do_compare(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if (arg1[0] == '\0')
    {
        send_to_char("Compare what to what?\r\n", ch);
        return;
    }

    if ((obj1 = get_obj_carry(ch, arg1, ch)) == NULL)
    {
        send_to_char("You do not have that item.\r\n", ch);
        return;
    }

    if (arg2[0] == '\0')
    {
        for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
        {
            if (obj2->wear_loc != WEAR_NONE && can_see_obj(ch, obj2)
                && obj1->item_type == obj2->item_type
                && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
                break;
        }

        if (obj2 == NULL)
        {
            send_to_char("You aren't wearing anything comparable.\r\n", ch);
            return;
        }
    }

    else if ((obj2 = get_obj_carry(ch, arg2, ch)) == NULL)
    {
        send_to_char("You do not have that item.\r\n", ch);
        return;
    }

    msg = NULL;
    value1 = 0;
    value2 = 0;

    if (obj1 == obj2)
    {
        msg = "You compare $p to itself.  It looks about the same.";
    }
    else if (obj1->item_type != obj2->item_type)
    {
        msg = "You can't compare $p and $P.";
    }
    else
    {
        switch (obj1->item_type)
        {
            default:
                msg = "You can't compare $p and $P.";
                break;

            case ITEM_ARMOR:
                value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
                value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
                break;

            case ITEM_WEAPON:
                value1 = obj1->value[1] + obj1->value[2];
                value2 = obj2->value[1] + obj2->value[2];
                break;
        }
    }

    if (msg == NULL)
    {
        if (value1 == value2)
            msg = "$p and $P look about the same.";
        else if (value1 > value2)
            msg = "$p looks better than $P.";
        else
            msg = "$p looks worse than $P.";
    }

    act(msg, ch, obj1, obj2, TO_CHAR);
    return;
}

/*
 * The credits showing where most known credit should be given starting at
 * the Diku coders onward.
 */
void do_credits(CHAR_DATA * ch, char *argument)
{
    do_function(ch, &do_help, "credits");
    return;
}

/*
 * The where command allows a player to see the other players near
 * them in an area.  An immortal can additionally run pass the "all"
 * argument in to see where all players are in the world.
 */
void do_where(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        // Players in the current area that are visible to you.
        send_to_char("Players near you:\r\n", ch);
        found = FALSE;
        for (d = descriptor_list; d; d = d->next)
        {
            if (d->connected == CON_PLAYING
                && (victim = d->character) != NULL && !IS_NPC(victim)
                && victim->in_room != NULL
                && !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
                && (is_room_owner(ch, victim->in_room)
                    || !room_is_private(victim->in_room))
                && victim->in_room->area == ch->in_room->area
                && can_see(ch, victim))
            {
                found = TRUE;
                sprintf(buf, "%-20s %s\r\n",
                    victim->name, victim->in_room->name);
                send_to_char(buf, ch);
            }
        }

        if (!found)
        {
            send_to_char("None\r\n", ch);
        }
    }
    else if (IS_IMMORTAL(ch) && !str_cmp(arg, "all"))
    {
        // Immortals only option to show where all players are in all areas
        send_to_char("Players in the world:\r\n", ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);
        sprintf(buf, "{W%-15s %-30s %s{x\r\n", "Player", "Area", "Room");
        send_to_char(buf, ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);

        for (d = descriptor_list; d; d = d->next)
        {
            if (d->connected == CON_PLAYING
                && (victim = d->character) != NULL && !IS_NPC(victim)
                && victim->in_room != NULL)
            {
                sprintf(buf, "%-15s %-30s %s\r\n", victim->name, victim->in_room->area->name, victim->in_room->name);
                send_to_char(buf, ch);
            }
        }

    }
    else
    {
        found = FALSE;
        for (victim = char_list; victim != NULL; victim = victim->next)
        {
            if (victim->in_room != NULL
                && victim->in_room->area == ch->in_room->area
                && !IS_AFFECTED(victim, AFF_HIDE)
                && !IS_AFFECTED(victim, AFF_SNEAK)
                && !is_affected(victim, gsn_camouflage)
                && can_see(ch, victim) && is_name(arg, victim->name))
            {
                found = TRUE;
                sprintf(buf, "%-28s %s\r\n",
                    PERS(victim, ch), victim->in_room->name);
                send_to_char(buf, ch);
                break;
            }
        }
        if (!found)
            act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }

    return;
} // end do_where

void do_consider(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Consider killing whom?\r\n", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg)) == NULL)
    {
        send_to_char("They're not here.\r\n", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        send_to_char("Don't even think about it.\r\n", ch);
        return;
    }

    diff = victim->level - ch->level;

    if (diff <= -10)
        msg = "You can kill $N naked and weaponless.";
    else if (diff <= -5)
        msg = "$N is no match for you.";
    else if (diff <= -2)
        msg = "$N looks like an easy kill.";
    else if (diff <= 1)
        msg = "The perfect match!";
    else if (diff <= 4)
        msg = "$N says 'Do you feel lucky, punk?'.";
    else if (diff <= 9)
        msg = "$N laughs at you mercilessly.";
    else
        msg = "Death will thank you for your gift.";

    act(msg, ch, NULL, victim, TO_CHAR);
    return;
}



void set_title(CHAR_DATA * ch, char *title)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
        bug("Set_title: NPC.", 0);
        return;
    }

    if (title[0] != '.' && title[0] != ',' && title[0] != '!'
        && title[0] != '?')
    {
        buf[0] = ' ';
        strcpy(buf + 1, title);
    }
    else
    {
        strcpy(buf, title);
    }

    free_string(ch->pcdata->title);
    ch->pcdata->title = str_dup(buf);
    return;
}



void do_title(CHAR_DATA * ch, char *argument)
{
    int i;

    if (IS_NPC(ch))
        return;

    /* Changed this around a bit to do some sanitization first   *
     * before checking length of the title. Need to come up with *
     * a centralized user input sanitization scheme. FIXME!      *
     * JR -- 10/15/00                                            */

    if (strlen(argument) > 45)
        argument[45] = '\0';

    i = strlen(argument);
    if (argument[i - 1] == '{' && argument[i - 2] != '{')
        argument[i - 1] = '\0';

    if (argument[0] == '\0')
    {
        send_to_char("Change your title to what?\r\n", ch);
        return;
    }

    smash_tilde(argument);
    set_title(ch, argument);
    send_to_char("Ok.\r\n", ch);
}



void do_description(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] != '\0')
    {
        buf[0] = '\0';
        smash_tilde(argument);

        if (argument[0] == '-')
        {
            int len;
            bool found = FALSE;

            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char("No lines left to remove.\r\n", ch);
                return;
            }

            strcpy(buf, ch->description);

            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)
                    {
                        /* back it up */
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else
                    {            /* found the second one */

                        buf[len + 1] = '\0';
                        free_string(ch->description);
                        ch->description = str_dup(buf);
                        send_to_char("Your description is:\r\n", ch);
                        send_to_char(ch->description ? ch->description :
                            "(None).\r\n", ch);
                        return;
                    }
                }
            }
            buf[0] = '\0';
            free_string(ch->description);
            ch->description = str_dup(buf);
            send_to_char("Description cleared.\r\n", ch);
            return;
        }

        if (!str_cmp(argument, "++") || !str_cmp(argument, "edit"))
        {
            string_append( ch, &ch->description);
            return;
        }
        else if (!str_cmp(argument, "format"))
        {
            ch->description = format_string(ch->description);
            send_to_char("Your description has been formatted.\r\n", ch);
            return;
        }
        else if (argument[0] == '+')
        {
            if (ch->description != NULL)
            {
                strcat(buf, ch->description);
            }

            argument++;

            while (isspace(*argument))
            {
                argument++;
            }
        }

        if (strlen(buf) >= 1024)
        {
            send_to_char("Description too long.\r\n", ch);
            return;
        }

        strcat(buf, argument);
        strcat(buf, "\r\n");
        free_string(ch->description);
        ch->description = str_dup(buf);
    }

    send_to_char("Your description is:\r\n", ch);
    send_to_char(ch->description ? ch->description : "(None).\r\n", ch);
    return;
}



void do_report(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    sprintf(buf,
        "You say 'I have %d/%d hp %d/%d mana %d/%d mv.'\r\n",
        ch->hit, ch->max_hit,
        ch->mana, ch->max_mana, ch->move, ch->max_move);

    send_to_char(buf, ch);

    sprintf(buf, "$n says 'I have %d/%d hp %d/%d mana %d/%d mv.'",
        ch->hit, ch->max_hit,
        ch->mana, ch->max_mana, ch->move, ch->max_move);

    act(buf, ch, NULL, NULL, TO_ROOM);

    return;
}

/*
 * The practice command can be used to both show a player all of their skills
 * and spells as well as actually practice them once they find a trainer.
 */
void do_practice(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if (IS_NPC(ch))
        return;

    if (argument[0] == '\0')
    {
        int col;

        col = 0;
        for (sn = 0; sn < top_sn; sn++)
        {
            if (skill_table[sn]->name == NULL)
                break;

            // If it is a racial skill, but not the players race then continue.
            if (skill_table[sn]->race > 0 && skill_table[sn]->race != ch->race)
                continue;

            if (ch->level < skill_table[sn]->skill_level[ch->class]
                || ch->pcdata->learned[sn] < 1 /* skill is not known */)
                continue;

            sprintf(buf, "%-19.19s %3d%%  ",
                skill_table[sn]->name, ch->pcdata->learned[sn]);
            send_to_char(buf, ch);
            if (++col % 3 == 0)
                send_to_char("\r\n", ch);
        }

        if (col % 3 != 0)
            send_to_char("\r\n", ch);

        sprintf(buf, "You have %d practice sessions left.\r\n", ch->practice);
        send_to_char(buf, ch);
    }
    else
    {
        CHAR_DATA *mob;
        int adept;

        if (!IS_AWAKE(ch))
        {
            send_to_char("In your dreams, or what?\r\n", ch);
            return;
        }

        for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
        {
            if (IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE))
                break;
        }

        if (mob == NULL)
        {
            send_to_char("You can't do that here.\r\n", ch);
            return;
        }

        if (ch->practice <= 0)
        {
            send_to_char("You have no practice sessions left.\r\n", ch);
            return;
        }

        if ((sn = find_spell(ch, argument)) < 0 ||
            (!IS_NPC(ch)
                && (ch->level < skill_table[sn]->skill_level[ch->class]
                    || ch->pcdata->learned[sn] < 1    /* skill is not known */
                    || (skill_table[sn]->race > 0 && skill_table[sn]->race != ch->race)
                    || skill_table[sn]->rating[ch->class] == 0)))
        {
            send_to_char("You can't practice that.\r\n", ch);
            return;
        }

        adept = IS_NPC(ch) ? 100 : class_table[ch->class]->skill_adept;

        if (ch->pcdata->learned[sn] >= adept)
        {
            sprintf(buf, "You are already learned at %s.\r\n", skill_table[sn]->name);
            send_to_char(buf, ch);
        }
        else
        {
            ch->practice--;
            ch->pcdata->learned[sn] +=
                int_app[get_curr_stat(ch, STAT_INT)].learn /
                skill_table[sn]->rating[ch->class];

            if (ch->pcdata->learned[sn] < adept)
            {
                sprintf(buf, "You practice $T to %d%% proficiency.", ch->pcdata->learned[sn]);
                act(buf, ch, NULL, skill_table[sn]->name, TO_CHAR);
                act("$n practices $T.", ch, NULL, skill_table[sn]->name, TO_ROOM);
            }
            else
            {
                ch->pcdata->learned[sn] = adept;
                act("You are now learned at $T.", ch, NULL, skill_table[sn]->name, TO_CHAR);
                act("$n is now learned at $T.", ch, NULL, skill_table[sn]->name, TO_ROOM);
            }
        }
    }

    return;

} // end do_practice



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument(argument, arg);

    if (arg[0] == '\0')
        wimpy = ch->max_hit / 5;
    else
        wimpy = atoi(arg);

    if (wimpy < 0)
    {
        send_to_char("Your courage exceeds your wisdom.\r\n", ch);
        return;
    }

    if (wimpy > ch->max_hit / 2)
    {
        send_to_char("Such cowardice ill becomes you.\r\n", ch);
        return;
    }

    ch->wimpy = wimpy;
    sprintf(buf, "Wimpy set to %d hit points.\r\n", wimpy);
    send_to_char(buf, ch);
    return;
}

/*
 * Allows a user to change their password.
 */
void do_password(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if (IS_NPC(ch))
        return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while (isspace(*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace(*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Syntax: password <old> <new>.\r\n", ch);
        return;
    }


    if (strcmp(sha256_crypt_with_salt(arg1, ch->name), ch->pcdata->pwd))
    {
        if (IS_IMMORTAL(ch))
        {
            send_to_char("Wrong password.\r\n", ch);
            return;
        }
        else
        {
            WAIT_STATE(ch, 40);
            send_to_char("Wrong password.  Wait 10 seconds.\r\n", ch);
            return;
        }
    }

    if (strlen(arg2) < 5)
    {
        send_to_char("New password must be at least five characters long.\r\n", ch);
        return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = sha256_crypt_with_salt(arg2, ch->name);
    for (p = pwdnew; *p != '\0'; p++)
    {
        if (*p == '~')
        {
            send_to_char("New password not acceptable, try again.\r\n", ch);
            return;
        }
    }

    free_string(ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup(pwdnew);
    save_char_obj(ch);
    send_to_char("Ok.\r\n", ch);
    log_f("%s changed their password.", ch->name);
    return;
}

void do_telnetga(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (IS_SET(ch->comm, COMM_TELNET_GA))
    {
        send_to_char("Telnet GA removed.\r\n", ch);
        REMOVE_BIT(ch->comm, COMM_TELNET_GA);
    }
    else
    {
        send_to_char("Telnet GA enabled.\r\n", ch);
        SET_BIT(ch->comm, COMM_TELNET_GA);
    }
} // end do_telnetga

/*
 * Shows the player their current stats and the available max stats.  This function
 * will color code the stat, white for at perm, green for above (via a spell or eqipment)
 * and red if it's below (via a spell or equipment).
 */
void do_stats(CHAR_DATA *ch, char *argument)
{
    // Check if they just want the brief stats as was displayed in the old score command, if
    // so, show them then get out.
    if (!IS_NULLSTR(argument) && !str_prefix(argument, "brief"))
    {
        printf_to_char(ch, "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\r\n",
            ch->perm_stat[STAT_STR], get_curr_stat(ch,STAT_STR),
            ch->perm_stat[STAT_INT], get_curr_stat(ch,STAT_INT),
            ch->perm_stat[STAT_WIS], get_curr_stat(ch,STAT_WIS),
            ch->perm_stat[STAT_DEX], get_curr_stat(ch,STAT_DEX),
            ch->perm_stat[STAT_CON], get_curr_stat(ch,STAT_CON));
        return;
    }

    char buf[MAX_STRING_LENGTH];

    send_to_char("------------------------------------------------------\r\n", ch);
    sprintf(buf, "{WStatistic       Permanent  Current  Max for Race/Class{x\r\n");
    send_to_char(buf, ch);
    send_to_char("------------------------------------------------------\r\n", ch);

    sprintf(buf, "Strength        {W%-2d{x         %s%-2d{x       {W%-2d{x\r\n",
        ch->perm_stat[STAT_STR],
        get_curr_stat(ch, STAT_STR) < ch->perm_stat[STAT_STR] ? "{R" : get_curr_stat(ch, STAT_STR) > ch->perm_stat[STAT_STR] ? "{G" : "{W",
        get_curr_stat(ch, STAT_STR),
        get_max_train(ch, STAT_STR));
    send_to_char(buf, ch);

    sprintf(buf, "Intelligence    {W%-2d{x         %s%-2d{x       {W%-2d{x\r\n",
        ch->perm_stat[STAT_INT],
        get_curr_stat(ch, STAT_INT) < ch->perm_stat[STAT_INT] ? "{R" : get_curr_stat(ch, STAT_INT) > ch->perm_stat[STAT_INT] ? "{G" : "{W",
        get_curr_stat(ch, STAT_INT),
        get_max_train(ch, STAT_INT));
    send_to_char(buf, ch);

    sprintf(buf, "Wisdom          {W%-2d{x         %s%-2d{x       {W%-2d{x\r\n",
        ch->perm_stat[STAT_WIS],
        get_curr_stat(ch, STAT_WIS) < ch->perm_stat[STAT_WIS] ? "{R" : get_curr_stat(ch, STAT_WIS) > ch->perm_stat[STAT_WIS] ? "{G" : "{W",
        get_curr_stat(ch, STAT_WIS),
        get_max_train(ch, STAT_WIS));
    send_to_char(buf, ch);

    sprintf(buf, "Dexterity       {W%-2d{x         %s%-2d{x       {W%-2d{x\r\n",
        ch->perm_stat[STAT_DEX],
        get_curr_stat(ch, STAT_DEX) < ch->perm_stat[STAT_DEX] ? "{R" : get_curr_stat(ch, STAT_DEX) > ch->perm_stat[STAT_DEX] ? "{G" : "{W",
        get_curr_stat(ch, STAT_DEX),
        get_max_train(ch, STAT_DEX));
    send_to_char(buf, ch);

    sprintf(buf, "Constitution    {W%-2d{x         %s%-2d{x       {W%-2d{x\r\n",
        ch->perm_stat[STAT_CON],
        get_curr_stat(ch, STAT_CON) < ch->perm_stat[STAT_CON] ? "{R" : get_curr_stat(ch, STAT_CON) > ch->perm_stat[STAT_CON] ? "{G" : "{W",
        get_curr_stat(ch, STAT_CON),
        get_max_train(ch, STAT_CON));
    send_to_char(buf, ch);

    send_to_char("------------------------------------------------------\r\n", ch);

    return;
} // end do_stats

/*
 * Displays information about all classes and reclasses.  This can be expanded in the future
 * to allow for more information and to be smarter in telling people what the qualifications are
 * for the class also.
 */
void do_class(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char prime_stat[5];
    int i = 0;

    if (IS_NULLSTR(argument))
    {
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);
        send_to_char("{WClass        Type         Who  Prime Stat   Casts @ Level  Adept  Enabled {x\r\n", ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);

        for (i = 0; i < top_class; i++)
        {
            if (class_table[i]->name == NULL)
            {
                log_string("BUG: null class");
                continue;
            }

            // We can only use capitalize once per sprintf.. it dups up otherwise due to the use
            // of a static char.
            sprintf(prime_stat, "%s", capitalize(flag_string(stat_flags, class_table[i]->attr_prime)));

            sprintf(buf, "%-12s{x %-12s %-4s %-12s %-18s %-2d%%    %s\r\n",
                capitalize(class_table[i]->name),
                class_table[i]->is_reclass == FALSE ? "Base Class" : "Reclass",
                class_table[i]->who_name,
                prime_stat,
                class_table[i]->fMana == TRUE ? "{GTrue{x" : "{RFalse{x",
                class_table[i]->skill_adept,
                class_table[i]->is_enabled == TRUE  ? "{GTrue{x" : "{RFalse{x"
                );

            send_to_char(buf, ch);
        }

        send_to_char("--------------------------------------------------------------------------------\r\n", ch);

        sprintf(buf, "Total Classes: %d\r\n\r\n", top_class);
        send_to_char(buf, ch);
        send_to_char("For more detailed information about a class please use these commands\r\n\r\n", ch);
        send_to_char("  - class <class name>\r\n", ch);
        send_to_char("  - help <class name>\r\n", ch);

        return;
    }

    // We have an argument, try to find that class
    i = class_lookup(argument);

    // Check that it's a valid class and that the player can be that class
    if (i == -1)
    {
        send_to_char("That's not a valid class.\r\n", ch);
        return;
    }

    send_to_char("--------------------------------------------------------------------------------\r\n", ch);
    send_to_char("{WDetailed Class Information{x\r\n", ch);
    send_to_char("--------------------------------------------------------------------------------\r\n", ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WClass:{x", capitalize(class_table[i]->name));
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WClass Type{x:", class_table[i]->is_reclass == FALSE ? "Base Class" : "Reclass");
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WWho Name{x:", class_table[i]->who_name);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WPrime Attribute{x:", capitalize(flag_string(stat_flags, class_table[i]->attr_prime)));
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%2d%%]\r\n", "{WSkill Adept{x:", class_table[i]->skill_adept);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%2d]\r\n", "{WTo Hit AC 0 @ Level 0{x:", class_table[i]->thac0_00);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%2d]\r\n", "{WTo Hit AC 0 @ Level 32{x:", class_table[i]->thac0_32);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%2d]\r\n", "{WHP Minimum Per Level{x:", class_table[i]->hp_min);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%2d]\r\n", "{WHP Maximum Per Level{x:", class_table[i]->hp_max);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WCasts at Level{x:", class_table[i]->fMana == TRUE ? "{GTrue{x" : "{RFalse{x");
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WBase Group{x:", class_table[i]->base_group);
    send_to_char(buf, ch);

    sprintf(buf, "%-30s [%s]\r\n", "{WDefault Group{x:", class_table[i]->default_group);
    send_to_char(buf, ch);

    send_to_char("--------------------------------------------------------------------------------\r\n", ch);
    send_to_char("Note:  Some of these settings are the default and maybe influenced by other in\r\n", ch);
    send_to_char("       game factors.\r\n", ch);
    send_to_char("--------------------------------------------------------------------------------\r\n", ch);

} // end do_class

/*
 * A function for use in the PERS macro in merc.h that will allow us to format
 * what a player looks like to the room.  This function name comes from the macro's
 * corresponding name.
 */
char *pers(CHAR_DATA *ch, CHAR_DATA *looker)
{
    static char buf[MAX_STRING_LENGTH];

    if (IS_GHOST(ch))
    {
        buf[0] = '\0';
        sprintf(buf, "The ghost of %s", ch->name);
        return buf;
    }
    else
    {
        return ch->name;
    }

} // end pers

/*
 * Command to allow the player to show the terrain type of the room they
 * are in.
 */
void do_terrain(CHAR_DATA *ch, char *argument)
{

    if (ch == NULL || ch->in_room == NULL)
        return;

    switch (ch->in_room->sector_type)
    {
        case(SECT_INSIDE) :
            send_to_char("You are indoors.\r\n", ch);
            break;
        case(SECT_CITY) :
            send_to_char("You see the city about you... not a lot of terrain.\r\n", ch);
            break;
        case(SECT_FIELD) :
            send_to_char("The terrain is that of fields.\r\n", ch);
            break;
        case(SECT_FOREST) :
            send_to_char("The terrain is that of the forest.\r\n", ch);
            break;
        case(SECT_HILLS) :
            send_to_char("The terrain is that of the hills.\r\n", ch);
            break;
        case(SECT_MOUNTAIN) :
            send_to_char("The terrain is that of the mountains.\r\n", ch);
            break;
        case(SECT_AIR) :
            send_to_char("There is no terrain, your in the air!\r\n", ch);
            break;
        case(SECT_DESERT) :
            send_to_char("The terrain is that of the desert.\r\n", ch);
            break;
        case(SECT_BEACH) :
            send_to_char("The terrain is that of the beach.\r\n", ch);
            break;
        case(SECT_OCEAN) :
            send_to_char("You are in the ocean!\r\n", ch);
            break;
        case(SECT_UNDERWATER) :
            send_to_char("You are underwater!\r\n", ch);
            break;
        default:
            send_to_char("The terrain type is undetermined.\r\n", ch);
            bugf("Unhandled terrain type in do_terrain for vnum %d", ch->in_room->vnum);
            break;
    }

    act("$n takes a look around $mself and examines the terrain.", ch, NULL, NULL, TO_ROOM);
    return;

} // end do_terrain

/*
 * Game version and build information.
 */
void do_version(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (!IS_NULLSTR(settings.mud_name))
    {
        sprintf(buf, "%s Version %s last built on %s at %s\r\n", settings.mud_name, VERSION, __DATE__, __TIME__);
    }
    else
    {
        sprintf(buf, "Version %s last built on %s at %s\r\n", VERSION, __DATE__, __TIME__);
    }

    send_to_char(buf, ch);
}
