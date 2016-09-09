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

/**************************************************************************
*  Login Menu                                                             *
***************************************************************************
*                                                                         *
*  Though control flow of the game state is handed in nanny.c, the        *
*  functions that deal with menu items on the login screen and the        *
*  rendering of the login screen will go through here.  The options       *
*  will be fired from the CON_LOGIN_MENU case in nanny.c                  *
*                                                                         *
***************************************************************************/


// System Specific Includes
#if defined(_WIN32)
#include <sys/types.h>
#include <time.h>
#include <io.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>                /* OLC -- for close read write etc */
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>                /* printf_to_char */
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

/*
* Sends the greeting on login.
*/
void show_greeting(DESCRIPTOR_DATA *d)
{
    extern char *help_greeting;

    if (help_greeting[0] == '.')
    {
        send_to_desc(help_greeting + 1, d);
    }
    else
    {
        send_to_desc(help_greeting, d);
    }
}

/*
* Shows random names simialiar to the do_randomnames command but formatted for
* the login screen.
*/
void show_random_names(DESCRIPTOR_DATA *d)
{
    char buf[MAX_STRING_LENGTH];
    int row = 0;
    int col = 0;

    send_to_desc("\r\n{W<{w-=-=-=-=-=-=-=-=-=-=-=-=-=-=  {R( {WRandom Names {R){w  =-=-=-=-=-=-=-=-=-=-=-=-=-=-{W>{x\r\n", d);

    for (row = 0; row < 6; row++)
    {
        // Since the random function returns a static char we have to use it in
        // separate calls.
        for (col = 0; col < 5; col++)
        {
            sprintf(buf, "%-16s", generate_random_name());
            send_to_desc(buf, d);
        }

        send_to_desc("\r\n", d);
    }

    return;
}

/*
* Show the credits to the login screen, this is a little hacky.  I would prefer to use the
* help system but it's tooled for CH's and down the line (in the string pager) has issues
* with descriptors on a a straight conversion of page_to_char.
*/
void show_login_credits(DESCRIPTOR_DATA *d)
{
    char buf[MAX_STRING_LENGTH];

    send_to_desc("\r\n{W<{w-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  {R( {WCredits {R){w  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-{W>{x\r\n\r\n", d);

    sprintf(buf, "  {G*{x {WCrimson Skies{x %s (1998-2016)\r\n", VERSION);
    send_to_desc(buf, d);
    send_to_desc("        Blake Pell (Rhien)\r\n", d);
    send_to_desc("  {G*{x {WROM 2.4{x (1993-1998)\r\n", d);
    send_to_desc("        Russ Taylor, Gabrielle Taylor, Brian Moore\r\n", d);
    send_to_desc("  {G*{x {WMerc DikuMUD{x (1991-1993)\r\n", d);
    send_to_desc("        Michael Chastain, Michael Quan, Mitchel Tse\r\n", d);
    send_to_desc("  {G*{x {WDikuMud{x (1990-1991)\r\n", d);
    send_to_desc("        Katja Nyboe, Tom Madsen, Hans Henrik Staerfeldt,\r\n", d);
    send_to_desc("        Michael Seifert, Sebastian Hammer\r\n", d);
    send_to_desc("\r\n", d);
    send_to_desc("  {G*{x Detailed additional credits can be viewed in game via the credits\r\n", d);
    send_to_desc("    command.  These additional credits include the names of many who\r\n", d);
    send_to_desc("    have contributed through the mud community over the years where\r\n", d);
    send_to_desc("    those contributions have been used here.\r\n", d);
    return;
}

/*
* Shows who is logged into the mud from the login menu.
*/
void show_login_who(DESCRIPTOR_DATA *d)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dl;
    int col = 0;
    int count = 0;
    int total_count = 0;

    // Top of the play bill, the immortals
    send_to_desc("\r\n{W<{w-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  {R( {WImmortals {R){w  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-{W>{x\r\n", d);

    for (dl = descriptor_list; dl != NULL; dl = dl->next)
    {
        CHAR_DATA *ch;

        if (dl->connected != CON_PLAYING)
        {
            continue;
        }

        ch = (dl->original != NULL) ? dl->original : dl->character;

        if (!IS_IMMORTAL(ch))
        {
            continue;
        }

        count++;

        sprintf(buf, "{C%-16s", ch->name);
        send_to_desc(buf, d);

        col++;

        if (col % 5 == 0)
        {
            send_to_desc("\r\n", d);
        }
    }

    total_count += count;

    // Display if there are no immortals online.
    if (count == 0)
    {
        send_to_desc("\r\n * {CThere are no immortals currently online.{x\r\n", d);
    }

    // The characters playing
    count = 0;
    col = 0;
    send_to_desc("\r\n{W<{w-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  {R(  {WMortals  {R){w  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-{W>{x\r\n", d);

    for (dl = descriptor_list; dl != NULL; dl = dl->next)
    {
        CHAR_DATA *ch;

        if (dl->connected != CON_PLAYING)
        {
            continue;
        }

        ch = (dl->original != NULL) ? dl->original : dl->character;

        if (IS_IMMORTAL(ch))
        {
            continue;
        }

        count++;

        sprintf(buf, "{x%-16s", ch->name);
        send_to_desc(buf, d);

        col++;

        if (col % 5 == 0)
        {
            send_to_desc("\r\n", d);
        }
    }

    total_count += count;

    if (count == 0)
    {
        send_to_desc("\r\n * {CThere are no mortals currently online.{x", d);
    }

    send_to_desc("\r\n", d);

    if (total_count > 0)
    {
        sprintf(buf, "\r\nTotal Players Online: %d\r\n", total_count);
        send_to_desc(buf, d);
    }

    return;
}

/*
* Renders the current login menu to the player.
*/
void show_login_menu(DESCRIPTOR_DATA *d)
{
    int x = 0;

    // This probably shouldn't happen but better safe than sorry on a high run method.
    if (d == NULL)
    {
        return;
    }

    char buf[MAX_STRING_LENGTH];
    bool ban_permit = check_ban(d->host, BAN_PERMIT);
    bool ban_newbie = check_ban(d->host, BAN_NEWBIES);
    bool ban_all = check_ban(d->host, BAN_ALL);

    // The login menu header
    send_to_desc("\r\n\r\n", d);
    send_to_desc("  _________________________________________________________________________\r\n", d);
    send_to_desc(" /`                                                                        \\\r\n", d);
    send_to_desc(" \\_|       {W-=-=-=-=-=-=))) {RCrimson {rSkies: {WLogin Menu {w(((=-=-=-=-=-=-{x        |\r\n", d);
    send_to_desc("   |                                                                        |\r\n", d);

    // Column 1.1 - Create a new character option.  The option is disabled if the game is wizlocked
    // newlocked, if their host is banned all together or if they are newbie banned.
    if (settings.wizlock || settings.newlock || ban_permit || ban_newbie)
    {
        sprintf(buf, "   |    {x({DN{x){Dew Character{x                ");
    }
    else
    {
        sprintf(buf, "   |    {x({GN{x){gew Character{x                ");
    }
    
    // Column 1.2 - Game Status
    strcat(buf, "{WGame Status: ");

    if (global.is_copyover == TRUE)
    {
        strcat(buf, "{RRebooting{x               |\r\n");
    }
    else if (settings.wizlock)
    {
        strcat(buf, "{RLocked{x                  |\r\n");
    }
    else if (settings.newlock)
    {
        strcat(buf, "{RNew Locked{x              |\r\n");
    }
    else if (settings.test_mode)
    {
        strcat(buf, "{YTest Mode{x               |\r\n");
    }
    else
    {
        strcat(buf, "{gOpen{x                    |\r\n");
    }

    send_to_desc(buf, d);

    // Column 2.1 - Play existing character, the login option is disabled if the player is banned or the game is wizlocked.
    if (ban_permit || settings.wizlock)
    {
        sprintf(buf, "   |    {x({DP{x){Dlay Existing Character{x        ");
    }
    else
    {
        sprintf(buf, "   |    {x({GP{x){glay Existing Character{x        ");
    }

    // Column 2.2 - Site status
    strcat(buf, "{WYour Site: ");
    if (ban_permit || ban_all)
    {
        strcat(buf, "{rBanned{x                  |\r\n");
    }
    else
    {
        if (ban_newbie)
        {
            strcat(buf, "{rNew Player Banned{x       |\r\n");
        }
        else
        {
            strcat(buf, "{gWelcome{x                 |\r\n");
        }
    }

    send_to_desc(buf, d);

    // Column 3.1 - Who is currently online
    sprintf(buf, "   |    {x({GW{x){gho is on now?{x                                                    |\r\n");
    send_to_desc(buf, d);

    // Column 4.1 - Random name generator
    sprintf(buf, "   |    {x({GR{x){gandom Name Generator{x                                             |\r\n");
    send_to_desc(buf, d);

    // Column 5.1 - Credits
    sprintf(buf, "   |    {x({GC{x){gredits{x                                                           |\r\n");
    send_to_desc(buf, d);

    // Column 6.1 & 6.2 - Quit and System Time
    sprintf(buf, "   |    {x({GQ{x){guit{x                                                              |\r\n");
    send_to_desc(buf, d);

    // Column 7.1 - Prompt
    send_to_desc("   |                                                                        |\r\n", d);
    sprintf(buf, "   |     {WYour selection? {x->                                                 |\r\n");
    send_to_desc(buf, d);

    send_to_desc("  _|                                                                        |\r\n", d);
    send_to_desc(" / |  -==================================================================-  |\r\n", d);
    send_to_desc(" \\/________________________________________________________________________/\r\n", d);

    // Since we've wrapped the menu in an ASCII graphic that looks like a parchment, we'll need to
    // reposition the cursor to move up to "Your selection? ->".. then after the selection we'll
    // need to move the cursor back down before displaying whatever we're going to display.

    // Move up 4 rows, then to the right 28
    sprintf(buf, "%s%s%s%s", UP, UP, UP, UP);

    // Move to the right 28
    for (x = 0; x < 28; x++)
    {
        strcat(buf, RIGHT);
    }

    send_to_desc(buf, d);

    return;
}
