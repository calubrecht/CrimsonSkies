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
 * A command to allow a player to become a loner without having to have an
 * immortal guild them.
 */
void do_loner(CHAR_DATA *ch, char *argument )
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
        send_to_char("This is not your name...\n\r", ch);
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

    log_f("%s guilds themselves to [ Loner ].", ch->name);
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

    send_to_char("--------------------------------------------------------------------------------\r\n", ch);
    send_to_char("{WClan                  Independent{x\r\n", ch);
    send_to_char("--------------------------------------------------------------------------------\r\n", ch);

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
        if (IS_NULLSTR(clan_table[clan].name))
            continue;

        sprintf(buf, "%-25s{x %-5s\r\n",
            clan_table[clan].who_name,
            clan_table[clan].independent == TRUE ? "True" : "False"
            );

        send_to_char(buf, ch);
    }

} // end guildlist
