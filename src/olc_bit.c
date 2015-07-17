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
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell           *
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
 *  File: olc_bit.c                                                        *
 *                                                                         *
 *  This code was written by Jason Dinkel and inspired by Russ Taylor,     *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 The code below uses a table lookup system that is based on suggestions
 from Russ Taylor.  There are many routines in handler.c that would benefit
 with the use of tables.  You may consider simplifying your code base by
 implementing a system like below with such functions. -Jason Dinkel
 */

// System Specific Includes
#if defined(__APPLE__)
    #include <types.h>
    #include <time.h>
#elif defined(_WIN32)
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
#include "tables.h"
#include "lookup.h"

struct flag_stat_type {
    const struct flag_type *structure;
    bool stat;
};

/*****************************************************************************
 Name:        flag_stat_table
 Purpose:    This table catagorizes the tables following the lookup
         functions below into stats and flags.  Flags can be toggled
         but stats can only be assigned.  Update this table when a
         new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] = {
/*  {    structure        stat    }, */
    {area_flags, FALSE},
    {sex_flags, TRUE},
    {exit_flags, FALSE},
    {door_resets, TRUE},
    {room_flags, FALSE},
    {sector_flags, TRUE},
    {type_flags, TRUE},
    {extra_flags, FALSE},
    {wear_flags, FALSE},
    {act_flags, FALSE},
    {affect_flags, FALSE},
    {apply_flags, TRUE},
    {wear_loc_flags, TRUE},
    {wear_loc_strings, TRUE},
    {container_flags, FALSE},
    {continent_flags, TRUE},

/* ROM specific flags: */

    {form_flags, FALSE},
    {part_flags, FALSE},
    {ac_type, TRUE},
    {size_flags, TRUE},
    {position_flags, TRUE},
    {off_flags, FALSE},
    {imm_flags, FALSE},
    {res_flags, FALSE},
    {vuln_flags, FALSE},
    {weapon_class, TRUE},
    {weapon_type2, FALSE},
    {apply_types, TRUE},
    {0, 0}
};



/*****************************************************************************
 Name:        is_stat( table )
 Purpose:    Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:    flag_value and flag_string.
 Note:        This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat (const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++)
    {
        if (flag_stat_table[flag].structure == flag_table
            && flag_stat_table[flag].stat)
            return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 Name:        flag_value( table, flag )
 Purpose:    Returns the value of the flags entered.  Multi-flags accepted.
 Called by:    olc.c and olc_act.c.
 ****************************************************************************/
int flag_value (const struct flag_type *flag_table, char *argument)
{
    char word[MAX_INPUT_LENGTH];
    int bit;
    int marked = 0;
    bool found = FALSE;

    if (is_stat (flag_table))
        return flag_lookup (argument, flag_table);

    /*
     * Accept multiple flags.
     */
    for (;;)
    {
        argument = one_argument (argument, word);

        if (word[0] == '\0')
            break;

        if ((bit = flag_lookup (word, flag_table)) != NO_FLAG)
        {
            SET_BIT (marked, bit);
            found = TRUE;
        }
    }

    if (found)
        return marked;
    else
        return NO_FLAG;
}



/*****************************************************************************
 Name:        flag_string( table, flags/stat )
 Purpose:    Returns string with name(s) of the flags or stat entered.
 Called by:    act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *flag_string (const struct flag_type *flag_table, int bits)
{
    static char buf[2][512];
    static int cnt = 0;
    int flag;

    if (++cnt > 1)
        cnt = 0;

    buf[cnt][0] = '\0';

    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
        if (!is_stat (flag_table) && IS_SET (bits, flag_table[flag].bit))
        {
            strcat (buf[cnt], " ");
            strcat (buf[cnt], flag_table[flag].name);
        }
        else if (flag_table[flag].bit == bits)
        {
            strcat (buf[cnt], " ");
            strcat (buf[cnt], flag_table[flag].name);
            break;
        }
    }
    return (buf[cnt][0] != '\0') ? buf[cnt] + 1 : "none";
}

const struct flag_type continent_flags[]= {
    { "limbo",   CONTINENT_LIMBO,   TRUE },
    { "oceans",  CONTINENT_OCEANS,  TRUE },
    { "arcanis", CONTINENT_ARCANIS, TRUE },
    { NULL, 0, 0 }
};
