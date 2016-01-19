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
*  File: settings.c                                                       *
*                                                                         *
*  This file contains the code to load, save and manipulate game settings *
*  that are persisted across boots.  It currently reads in writes in      *
*  standard ROM fashion but is going to be changed to use the new INI     *
*  parsing code the near future.  - Rhien                                 *
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

// Local functions
void save_settings(void);

/*
 * Command to show a character the current game settings, locks, etc. setup by the mud
 * mud admin.  Potentially also show player based settings here as that list grows.
 */
void do_settings(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
        send_to_char("NPC's are not allowed to use the settings command.\r\n", ch);
        return;
    }

    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    // Show the game and player settings
    if (IS_NULLSTR(argument))
    {
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);
        send_to_char("{WGame Settings{x\r\n", ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);

        sprintf(buf, "%-25s %-7s  %-25s %-7s\r\n",
            "Game Locked (Wizlock)", settings.wizlock ? "{GON{x" : "{ROFF{x",
            "New Lock", settings.newlock ? "{GON{x" : "{ROFF{x");
        send_to_char(buf, ch);

        sprintf(buf, "%-25s %-7s  %-25s %-7s\r\n",
            "Double Experience", settings.double_exp ? "{GON{x" : "{ROFF{x",
            "Double Gold", settings.double_gold ? "{GON{x" : "{ROFF{x");
        send_to_char(buf, ch);

        send_to_char("\r\n", ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);
        send_to_char("{WGame Mechanics{x\r\n", ch);
        send_to_char("--------------------------------------------------------------------------------\r\n", ch);

        sprintf(buf, "%-25s %-7s  %-25s %-7s\r\n",
            "Gain Convert", settings.gain_convert ? "{GON{x" : "{ROFF{x",
            "Shock Spread", settings.shock_spread ? "{GON{x" : "{ROFF{x");
        send_to_char(buf, ch);
    }

    send_to_char("\r\n", ch);

    // Since this is a dual mortal/immortal command, kick the mortals out here and lower level imms.
    if (ch->level < CODER)
    {
        return;
    }

    // Get the arguments we need
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1))
    {
        send_to_char("\r\n{YProvide an argument to set or toggle a setting.{x\r\n\r\n", ch);
        send_to_char("Syntax: settings <wizlock|newlock|doublegold|doubleexperience|\r\n", ch);
        send_to_char("                  gainconvert|shockspread>\r\n", ch);
        return;
    }

    if (!str_prefix(arg1, "wizlock"))
    {
        do_wizlock(ch, "");
    }
    else if (!str_prefix(arg1, "newlock"))
    {
        do_newlock(ch, "");
    }
    else if (!str_prefix(arg1, "doubleexperience"))
    {
        settings.double_exp = !settings.double_exp;

        if (settings.double_exp)
        {
            wiznet("$N has enabled double experience.", ch, NULL, 0, 0, 0);
            send_to_char("Double experience enabled.\r\n", ch);
        }
        else
        {
            wiznet("$N has disabled double experience.", ch, NULL, 0, 0, 0);
            send_to_char("Double experience disabled.\r\n", ch);
        }

        // Save the settings out to file.
        save_settings();

    }
    else if (!str_prefix(arg1, "doublegold"))
    {
        settings.double_gold = !settings.double_gold;

        if (settings.double_gold)
        {
            wiznet("$N has enabled double gold.", ch, NULL, 0, 0, 0);
            send_to_char("Double gold enabled.\r\n", ch);
        }
        else
        {
            wiznet("$N has disabled double gold.", ch, NULL, 0, 0, 0);
            send_to_char("Double gold disabled.\r\n", ch);
        }

        // Save the settings out to file.
        save_settings();

    }
    else if (!str_prefix(arg1, "gainconvert"))
    {
        settings.gain_convert = !settings.gain_convert;

        if (settings.gain_convert)
        {
            wiznet("$N has the gain convert feature.", ch, NULL, 0, 0, 0);
            send_to_char("The gain convert feature has been enabled.\r\n", ch);
        }
        else
        {
            wiznet("$N has disabled the gain convert feature.", ch, NULL, 0, 0, 0);
            send_to_char("The gain convert feature has been disabled.\r\n", ch);
        }

        // Save the settings out to file.
        save_settings();

    }
    else if (!str_prefix(arg1, "shockspread"))
    {
        settings.shock_spread = !settings.shock_spread;

        if (settings.shock_spread)
        {
            wiznet("$N has enabled the shock spread mechanic.", ch, NULL, 0, 0, 0);
            send_to_char("The shock spread mechanic has been enabled.\r\n", ch);
        }
        else
        {
            wiznet("$N has disabled the shock spread mechanic.", ch, NULL, 0, 0, 0);
            send_to_char("The shock spread mechanic has been disabled.\r\n", ch);
        }

        // Save the settings out to file.
        save_settings();

    }
    else
    {
        send_to_char("settings <wizlock|newlock|doublegold|doubleexperience|\r\n", ch);
        send_to_char("          gainconvert|shockspread\r\n", ch);
    }

} // end do_settings

 /*
  * Loads the game settings from the settings.dat file.  I skipped using the KEY macro in this
  * case an opt'd for an if ladder.  If an invalid setting is found in the file it will attempt to
  * log it.  I don't read ahead on that value in case it's corrupt, I want to hit the #END marker
  * eventually and get out without crashing (if an unfound setting is logged, it will then read in
  * its value if it has one and do the if/else on it.. looking for a 0 or 1 case maybe that won't
  * be found).  This file should only be generated through the game or OLC so this case shouldn't
  * happen.  - Rhien.
  */
void load_settings()
{
    FILE *fp;
    char *word;

    fclose(fpReserve);
    fp = fopen(SETTINGS_FILE, "r");

    if (!fp)
    {
        log_f("WARNING: Settings file '%s' was not found or is inaccessible.", SETTINGS_FILE);
        fpReserve = fopen(NULL_FILE, "r");
        global.last_boot_result = DEFAULT;
        return;
    }

    for (;;)
    {
        word = feof(fp) ? "#END" : fread_word(fp);

        // End marker?  Exit cleanly
        if (!str_cmp(word, "#END"))
        {
            global.last_boot_result = SUCCESS;
            return;
        }

        if (!str_cmp(word, "WizLock"))
        {
            settings.wizlock = fread_number(fp);
        }
        else if (!str_cmp(word, "NewLock"))
        {
            settings.newlock = fread_number(fp);
        }
        else if (!str_cmp(word, "DoubleExp"))
        {
            settings.double_exp = fread_number(fp);
        }
        else if (!str_cmp(word, "DoubleGold"))
        {
            settings.double_gold = fread_number(fp);
        }
        else if (!str_cmp(word, "ShockSpread"))
        {
            settings.shock_spread = fread_number(fp);
        }
        else if (!str_cmp(word, "GainConvert"))
        {
            settings.gain_convert = fread_number(fp);
        }
        else
        {
            log_f("Invalid setting '%s' found.", word);
        }

    }

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

    global.last_boot_result = SUCCESS;
    return;

} // end load_settings

 /*
  * Saves all of the game settings to file.
  */
void save_settings(void)
{
    FILE *fp;

    fclose(fpReserve);
    fp = fopen(SETTINGS_FILE, "w");

    if (!fp)
    {
        bug("Could not open SETTINGS_FILE for writing.", 0);
        return;
    }

    // Locks / Bonuses
    fprintf(fp, "WizLock         %d\n", settings.wizlock);
    fprintf(fp, "NewLock         %d\n", settings.newlock);
    fprintf(fp, "DoubleExp       %d\n", settings.double_exp);
    fprintf(fp, "DoubleGold      %d\n", settings.double_gold);
    // Game Mechanics
    fprintf(fp, "ShockSpread     %d\n", settings.shock_spread);
    fprintf(fp, "GainConvert     %d\n", settings.gain_convert);

    fprintf(fp, "#END\n");
    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

} // end save_settings
