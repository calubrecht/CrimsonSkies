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
#include "ini.h"

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
  * Loads the game settings from the settings.ini file.  This is leveraging the INI parsing
  * utilities contained in "ini.c".  Any values not found will have default values set.  The
  * INI parser will read in 0, 1, True, False, Yes, No and convert it accordingly.  It will be
  * then re-saved out with a standard 'True' or 'False' for readability.
  */
void load_settings()
{
    dictionary  *ini;

    ini = iniparser_load(SETTINGS_FILE);

    if (ini == NULL)
    {
        log_f("WARNING: Settings file '%s' was not found or is inaccessible.", SETTINGS_FILE);
        global.last_boot_result = DEFAULT;
        return;
    }

    settings.wizlock = iniparser_getboolean(ini, "Settings:WizLock", FALSE);
    settings.newlock = iniparser_getboolean(ini, "Settings:NewLock", FALSE);
    settings.double_exp = iniparser_getboolean(ini, "Settings:DoubleExp", FALSE);
    settings.double_gold = iniparser_getboolean(ini, "Settings:DoubleGold", FALSE);
    settings.shock_spread = iniparser_getboolean(ini, "Settings:ShockSpread", FALSE);
    settings.gain_convert = iniparser_getboolean(ini, "Settings:GainConvert", FALSE);

    iniparser_freedict(ini);

    global.last_boot_result = SUCCESS;
    return;

} // end load_settings

/*
 * Saves all of the game settings to an INI file.
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

    // Section
    fprintf(fp, "[Settings]\n");

    // Game Locks and Bonuses
    fprintf(fp, "WizLock = %s\n", settings.wizlock ? "True" : "False");
    fprintf(fp, "NewLock = %s\n", settings.newlock ? "True" : "False");
    fprintf(fp, "DoubleExp = %s\n", settings.double_exp ? "True" : "False");
    fprintf(fp, "DoubleGold = %s\n", settings.double_gold ? "True" : "False");

    // Game Mechanics Settings
    fprintf(fp, "ShockSpread = %s\n", settings.shock_spread ? "True" : "False");
    fprintf(fp, "GainConvert = %s\n", settings.gain_convert ? "True" : "False");

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");

} // end save_settings
