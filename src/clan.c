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
 *                                                                         *
 *  Clan Code                                                              *
 *                                                                         *
 *  We consolidate code relating to clans here excluding the communication *
 *  commands which will continue to live in act_comm.c                     *
 *                                                                         *
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/*
 * Clan Table
 *
 * Name, Who Name, Friendly Name, Death Transfer Room, Recall VNUM, Independent
 *
 * Independent should be FALSE if it is a real clan
 */
const struct clan_type clan_table[MAX_CLAN] = {
    { "",           "",                      "",             ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         TRUE,  FALSE },
    { "loner",      "[ {WLoner{x ] ",        "Loner",        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         TRUE,  TRUE  },
    { "renegade",   "[ {WRenegade{x ] ",     "Renegade",     ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         TRUE,  TRUE  },
    { "midgaard",   "[ {BMidgaard{x ] ",     "Midgaard",     ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         FALSE, TRUE  },
    { "newthalos",  "[ {cNew Thalos{x ] ",   "New Thalos",   ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         FALSE, TRUE  },
    { "redoakarmy", "[ {RRed Oak Army{x ] ", "Red Oak Army", ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         FALSE, TRUE  },
    { "cult",       "[ {wCult{x ] ",         "Cult",         ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,         FALSE, TRUE  },
    { "sylvan",     "[ {gSylvan{x ] ",       "Sylvan",       ROOM_VNUM_SYLVAN_ALTER, ROOM_VNUM_SYLVAN_RECALL, FALSE, TRUE  }
};

/*
 * Whether or not the character is in a clan or not.
 */
bool is_clan(CHAR_DATA * ch)
{
    return ch->clan;
}

/*
 * Whether or not two characters are in the same clan.
 */
bool is_same_clan(CHAR_DATA * ch, CHAR_DATA * victim)
{
    // Loner's and renegades should never be considered in the same clan.
    if (clan_table[ch->clan].independent)
    {
        return FALSE;
    }
    else
    {
        return (ch->clan == victim->clan);
    }
}


/*
 * Gets the int for a clan based on it's name.
 */
int clan_lookup(const char *name)
{
    int clan;

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
        if (LOWER(name[0]) == LOWER(clan_table[clan].name[0])
            && !str_prefix(name, clan_table[clan].name))
        {
            return clan;
        }
    }

    return 0;
}

/*
 * A command to allow a player to become a loner without having to have an
 * immortal guild them.
 */
void do_loner(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
        send_to_char("NPC's can not guild as a loner.\r\n", ch);
        return;
    }

    // We will require them to enter their name as a confirmation
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Syntax: loner <your name>\r\n",ch);
        return;
    }

    if (str_cmp(argument, ch->name))
    {
        send_to_char("This is not your name...\r\n", ch);
        return;
    }

    if (ch->level < 5 || ch->level > 25)
    {
        send_to_char("You can only choose to become a loner between levels 5 and 25.\r\n", ch);
        return;
    }

    if (is_clan(ch))
    {
        send_to_char("You are already part of the clanned world.\r\n", ch);
        return;
    }

    // Guild the player and send a global message to those online
    ch->clan = clan_lookup("loner");

    sprintf(buf, "\r\n%s walks alone %s\r\n", ch->name, clan_table[ch->clan].who_name);
    send_to_all_char(buf);

    log_f("%s guilds themselves to [ Loner ]", ch->name);
}

/*
 * Lists all of the clans in the game.  Consider making the guild command that allows for getting
 * info as well as putting players in commands with both immortal and player based features so players
 * can manage their own clans without immortals (will need leader and recruiter flags).
 */
void do_guildlist(CHAR_DATA *ch, char *argument)
{
    int clan;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;

    send_to_char("--------------------------------------------------------------------------------\r\n", ch);
    send_to_char("{WClan               Independent   Recall Point{x\r\n", ch);
    send_to_char("--------------------------------------------------------------------------------\r\n", ch);

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
        if (IS_NULLSTR(clan_table[clan].name) || !clan_table[clan].enabled)
        {
           continue;
        }

        location = get_room_index(clan_table[clan].recall_vnum);

        sprintf(buf, "%-22s{x %-13s {c%-20s{x\r\n",
            clan_table[clan].who_name,
            clan_table[clan].independent == TRUE ? "True" : "False",
            (location != NULL && !IS_NULLSTR(location->name)) ? location->name : "Unknown"
            );

        send_to_char(buf, ch);
    }

} // end guildlist
