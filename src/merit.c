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
 *  Merits add abilities to a character, they cost creation points when a  *
 *  character is created.  They can also be earned from in game mechansims.*
 *  They are added and removed through add_merit and remove_merit which    *
 *  will set the bits necessary and then setup any conditions that need to *
 *  happen (like lower saves, etc).  Anything like this will also need to  *
 *  be accounted for in the reset_char function in handler.c if it affects *
 *  something like allow a stat above the maximum for that race/class.     *
 *  Merits that don't toggle stats like that (e.g. magic affinity which    *
 *  raises casting level) don't need to worry about that in either         *
 *  location (or can use a permanent affect added to apply_merit_affects). *
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
#include "merc.h"
#include "tables.h"
#include "interp.h"

/*
 * Merits that can increase a players ability in given areas.
 */
const struct merit_type merit_table[] = {
    { MERIT_DAMAGE_REDUCTION, "Damage Reduction", TRUE },   // -5% damage
    { MERIT_LIGHT_FOOTED, "Light Footed", TRUE },           // Chance of reduced movement cost, access to sneak skill
    { MERIT_MAGIC_AFFINITY, "Magic Affinity", TRUE },       // +1 Casting Level
    { MERIT_MAGIC_RESISTANCE, "Magic Resistance", TRUE },   // -4 Saves
    { MERIT_PERCEPTION, "Perception", TRUE},                // Permanent affects for -> detect hidden, detect invis, detect good, detect evil, detect magic
    { 0, NULL, FALSE}
};

/*
 * Adds a merit to a character and initially puts the affect in place if it's
 * a merit that needs an attribute to change on a char.  remove_merit must be
 * called to properly remove the merit.
 */
void add_merit(CHAR_DATA *ch, long merit)
{
    // Not on NPC's and don't process the change (that reset_char would also process) if
    // the player already has the merit.
    if (IS_NPC(ch) || IS_SET(ch->pcdata->merit, merit))
    {
        return;
    }

    SET_BIT(ch->pcdata->merit, merit);

    // Every case here must have a corresponding removal case in remove_merit
    switch (merit)
    {
        case MERIT_MAGIC_RESISTANCE:
            ch->saving_throw -= 4;
            break;
        case MERIT_PERCEPTION:
            apply_merit_affects(ch);
            break;
    }
}

/*
 * Removes a merit from a character and removes the affects (these affects would have
 * been removed on the next login or whenever reset_char was called).
 */
void remove_merit(CHAR_DATA *ch, long merit)
{
    // Not on NPC's and don't process the change (that reset_char would also process) if
    // the player already has the merit.
    if (IS_NPC(ch) || !IS_SET(ch->pcdata->merit, merit))
    {
        return;
    }

    REMOVE_BIT(ch->pcdata->merit, merit);

    // Every case here must have a corresponding removal case in remove_merit
    switch (merit)
    {
        case MERIT_MAGIC_RESISTANCE:
            ch->saving_throw += 4;
            break;
        case MERIT_PERCEPTION:
            affect_strip(ch, gsn_detect_hidden);
            affect_strip(ch, gsn_detect_invis);
            affect_strip(ch, gsn_detect_magic);
            affect_strip(ch, gsn_detect_good);
            affect_strip(ch, gsn_detect_evil);
            break;
    }

}

/*
 * Applies any offical affects to a player for their given merits.  A merit affect can and should
 * still be cancelled off or dispelled if it's a normal spell, but will re-take affect on tick.
 */
void apply_merit_affects(CHAR_DATA *ch)
{
    AFFECT_DATA af;

    if (ch == NULL || IS_NPC(ch))
    {
        return;
    }

    if (IS_SET(ch->pcdata->merit, MERIT_PERCEPTION))
    {
        // Base affect settings
        af.where = TO_AFFECTS;
        af.level = ch->level;
        af.duration = -1;
        af.location = APPLY_NONE;
        af.modifier = 0;

        // Detect Hidden
        if (!is_affected(ch, gsn_detect_hidden))
        {
            af.type = gsn_detect_hidden;
            af.bitvector = AFF_DETECT_HIDDEN;
            affect_to_char(ch, &af);
        }

        // Detect Invisible
        if (!is_affected(ch, gsn_detect_invis))
        {
            af.type = gsn_detect_invis;
            af.bitvector = AFF_DETECT_INVIS;
            affect_to_char(ch, &af);
        }

        // Detect Magic
        if (!is_affected(ch, gsn_detect_magic))
        {
            af.type = gsn_detect_magic;
            af.bitvector = AFF_DETECT_MAGIC;
            affect_to_char(ch, &af);
        }

        // Detect Good
        if (!is_affected(ch, gsn_detect_good))
        {
            af.type = gsn_detect_good;
            af.bitvector = AFF_DETECT_GOOD;
            affect_to_char(ch, &af);
        }

        // Detect Evil
        if (!is_affected(ch, gsn_detect_evil))
        {
            af.type = gsn_detect_evil;
            af.bitvector = AFF_DETECT_EVIL;
            affect_to_char(ch, &af);
        }
    }

}
