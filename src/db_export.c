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
 * Although we are still using flat files to store all of the game data,   *
 * we will use this module to export that data into a sqlite database      *
 * which can be easily queried and used in other locations such as a web   *
 * site to hopefully provide better tools to mine the game data for        *
 * players and game admins. Some of the data maybe redudant to to allow    *
 * for less joins when querying (e.g. the object row will include the area *
 * name as well as the area_vnum.  I don't expect that this will be an     *
 * issue.                                                                  *
 *                                                                         *
 ***************************************************************************/

// System Specific Includes
#if defined(_WIN32)
#include <sys/types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#endif

#include "sqlite/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"

void export_continents(void);
void export_areas(void);
void export_objects(void);
void export_item_type(void);
void export_clans(void);
void export_bits(void);
void export_help(void);
void export_rooms(void);
void export_classes(void);
void export_stats(void);
void export_directions(void);
void export_flags(char *table_name, const struct flag_type *flags);

char *flag_string(const struct flag_type *flag_table, int bits);

/*
 * Command to initiate the exporting of all game data into a sqlite database that can then be used
 * elsewhere such as a web site.
 */
void do_dbexport(CHAR_DATA * ch, char *argument)
{
    int x = 0;

    writef(ch->desc, "%-55sStatus\r\n", "Action");
    writef(ch->desc, HEADER);

    writef(ch->desc, "%-55s", "Exporting Bits");
    export_bits();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Continents");
    export_continents();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Item Types");
    export_item_type();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Areas");
    export_areas();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Objects");
    export_objects();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Clans");
    export_clans();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Rooms");
    export_rooms();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Help Files");
    export_help();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Classes");
    export_classes();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Stat Lookup");
    export_stats();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    writef(ch->desc, "%-55s", "Exporting Directions");
    export_directions();
    writef(ch->desc, "[ {GComplete{x ]\r\n");

    // These are all of the tables that are flag_types (makes it easy to export generically).
    for (x = 0; export_flags_table[x].table_name != NULL; x++)
    {
        writef(ch->desc, "Exporting %-45s", export_flags_table[x].friendly_name);
        export_flags(export_flags_table[x].table_name, export_flags_table[x].flags);
        writef(ch->desc, "[ {GComplete{x ]\r\n");
    }

    writef(ch->desc, "\r\nExport of game data complete!\r\n");
}

/*
 * All of the tables to export that are from the "flag_type" struct type.
 */
const struct export_flags_type export_flags_table[] = {
    { "apply_flags", "Apply Flags", apply_flags },
    { "apply_types", "Apply Types", apply_types },
    { "extra_flags", "Extra Flags", extra_flags },
    { "room_flags", "Room Flags", room_flags },
    { "sector_flags", "Sector Flags", sector_flags },
    { "weapon_flags", "Weapon Flags", weapon_type2 },
    { "wear_flags", "Wear Flags", wear_flags },
    { "act_flags", "Act Flags", act_flags },
    { "plr_flags", "Player Flags", plr_flags },
    { "affect_flags", "Affect Flags", affect_flags},
    { "off_flags", "Offensive Flags", affect_flags},
    { "immune_flags", "Immunity Flags", affect_flags},
    { "res_flags", "Resistance Flags", res_flags },
    { "vuln_flags", "Vulneribility Flags", vuln_flags },
    { "form_flags", "Form Flags", form_flags},
    { "part_flags", "Part Flags", part_flags },
    { "comm_flags", "Communication Flags", comm_flags},
    { "mprog_flags", "Mob Prog Flags", mprog_flags},
    { "area_flags", "Area Flags", area_flags},
    { "sex_flags", "Gender Flags", sex_flags},
    { "exit_flags", "Exit Flags", exit_flags},
    { "door_resets", "Door Resets", door_resets},
    { "type_flags", "Item Type Flags", type_flags},
    { "wear_loc_strings", "Wear Location Strings", wear_loc_strings},
    { "wear_loc_flags", "Wear Location Flags", wear_loc_flags},
    { "container_flags", "Container Flags", container_flags},
    { "ac_type", "Armor Class Flags", ac_type},
    { "size_flags", "Size Flags", size_flags },
    { "weapon_class", "Weapon Class", weapon_class },
    { "position_flags", "Position Flags", position_flags },
    { "portal_flags", "Portal Floags", portal_flags },
    { "furniture_flags", "Furniture Flags", furniture_flags },
    { NULL, NULL, NULL }
};

/*
* Export a generic flags table (the name of the table and a pointer to the stucture is
* required.  This should be generic in that it can export anything that's a flag_type.  The
* flags table must have a final entry with a NULL or this will ya know, doing something not
* great.
*/
void export_flags(char *table_name, const struct flag_type *flags)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    int x;
    char sql[MSL];

    if (IS_NULLSTR(table_name) || flags == NULL)
    {
        bugf("Null table_name or flags_type in export_flags_table");
        return;
    }

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_flags_table(%s) -> Failed to open %s", table_name, EXPORT_DATABASE_FILE);
        goto out;
    }

    // Total reload everytime, drop the table if it exists.
    sprintf(sql, "DROP TABLE IF EXISTS %s;", table_name);

    if ((sqlite3_exec(db, sql, 0, 0, 0)))
    {
        bugf("export_flags_table(%s) -> Failed to drop table", table_name);
        goto out;
    }

    // Create the tables they do not exist
    sprintf(sql, "CREATE TABLE IF NOT EXISTS %s(id INTEGER PRIMARY KEY, name TEXT);", table_name);

    if ((sqlite3_exec(db, sql, 0, 0, 0)))
    {
        bugf("export_flags_table(%s) -> Failed to create table", table_name);
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    sprintf(sql, "INSERT INTO %s (id, name) VALUES (?1, ?2);", table_name);

    if (sqlite3_prepare(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_flags_table(%s) -> Failed to prepare insert statement", table_name);
        goto out;
    }

    // Loop over all extra flags
    for (x = 0; flags[x].name != NULL; x++)
    {
        sqlite3_bind_int(stmt, 1, flags[x].bit);
        sqlite3_bind_text(stmt, 2, flags[x].name, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("export_flags(%s) ERROR inserting data for %s: %s\n", table_name, flags[x].name, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_flags_table(%s) -> Failed to commit transaction.", table_name);
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_objects(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stmt_affect;
    int vnum = 0;
    int nMatch = 0;
    OBJ_INDEX_DATA *pObjIndex;
    AFFECT_DATA *paf;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_objects -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS object;", 0, 0, 0)))
    {
        bugf("export_object -> Failed to drop table: object");
        goto out;
    }

    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS object_affect;", 0, 0, 0)))
    {
        bugf("export_object -> Failed to drop table: object_affect");
        goto out;
    }


    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS object(vnum INTEGER PRIMARY KEY, name TEXT, short_description TEXT, long_description TEXT, material TEXT, item_type INTEGER, extra_flags INTEGER, wear_flags INTEGER, level INTEGER, condition INTEGER, weight INTEGER, cost INTEGER, value1 INTEGER, value2 INTEGER, value3 INTEGER, value4 INTEGER, value5 INTEGER, area_name TEXT, area_vnum INTEGER);", 0, 0, 0)))
    {
        bugf("export_object -> Failed to create table: object");
        goto out;
    }

    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS object_affect(vnum INTEGER, apply_id INTEGER, name TEXT, modifier INTEGER);", 0, 0, 0)))
    {
        bugf("export_object -> Failed to create table: object");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO object (vnum, name, short_description, long_description, material, item_type, extra_flags, wear_flags, level, condition, weight, cost, value1, value2, value3, value4, value5, area_name, area_vnum) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_object -> Failed to prepare insert statement for object.");
        goto out;
    }

    if (sqlite3_prepare(db, "INSERT INTO object_affect (vnum, apply_id, name, modifier) VALUES (?1, ?2, ?3, ?4);", -1, &stmt_affect, NULL) != SQLITE_OK)
    {
        bugf("export_object -> Failed to prepare insert statement object_affect.");
        goto out;
    }

    // Loop over all object index data's
    for (vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ((pObjIndex = get_obj_index(vnum)) != NULL)
        {
            nMatch++;

            // The basic object index data
            sqlite3_bind_int(stmt, 1, pObjIndex->vnum);
            sqlite3_bind_text(stmt, 2, pObjIndex->name, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, pObjIndex->short_descr, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, pObjIndex->description, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, pObjIndex->material, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 6, pObjIndex->item_type);
            sqlite3_bind_int(stmt, 7, pObjIndex->extra_flags);
            sqlite3_bind_int(stmt, 8, pObjIndex->wear_flags);
            sqlite3_bind_int(stmt, 9, pObjIndex->level);
            sqlite3_bind_int(stmt, 10, pObjIndex->condition);
            sqlite3_bind_int(stmt, 11, pObjIndex->weight);
            sqlite3_bind_int(stmt, 12, pObjIndex->cost);
            sqlite3_bind_int(stmt, 13, pObjIndex->value[0]);
            sqlite3_bind_int(stmt, 14, pObjIndex->value[1]);
            sqlite3_bind_int(stmt, 15, pObjIndex->value[2]);
            sqlite3_bind_int(stmt, 16, pObjIndex->value[3]);
            sqlite3_bind_int(stmt, 17, pObjIndex->value[4]);
            sqlite3_bind_text(stmt, 18, pObjIndex->area->name, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 19, pObjIndex->area->vnum);

            rc = sqlite3_step(stmt);

            if (rc != SQLITE_DONE)
            {
                bugf("ERROR inserting data for %d: %s\n", pObjIndex->vnum, sqlite3_errmsg(db));
            }

            sqlite3_reset(stmt);

            // Object affects for this item
            for (paf = pObjIndex->affected; paf; paf = paf->next)
            {
                sqlite3_bind_int(stmt_affect, 1, pObjIndex->vnum);
                sqlite3_bind_int(stmt_affect, 2, paf->location);
                sqlite3_bind_text(stmt_affect, 3, flag_string(apply_flags, paf->location), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt_affect, 4, paf->modifier);

                rc = sqlite3_step(stmt_affect);

                if (rc != SQLITE_DONE)
                {
                    bugf("ERROR inserting data for %d: %s\n", pObjIndex->vnum, sqlite3_errmsg(db));
                }

                sqlite3_reset(stmt_affect);
            }

        }
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_objects -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);
    sqlite3_finalize(stmt_affect);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_continents(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    int continent;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_continents -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS continent;", 0, 0, 0)))
    {
        bugf("export_continents -> Failed to drop table: continent");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS continent(id INTEGER PRIMARY KEY, name TEXT);", 0, 0, 0)))
    {
        bugf("export_continents -> Failed to create table: continent");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO continent (id, name) VALUES (?1, ?2);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_continents -> Failed to prepare insert");
        goto out;
    }

    // Loop over all continents
    for (continent = 0; continent_table[continent].name != NULL; continent++)
    {
        sqlite3_bind_int(stmt, 1, continent_table[continent].type);
        sqlite3_bind_text(stmt, 2, continent_table[continent].name, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", continent_table[continent].type, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_continents -> Failed to commit transaction");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_item_type(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    int x;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_item_type -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS item_type;", 0, 0, 0)))
    {
        bugf("export_item_type -> Failed to drop table: item_type");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS item_type(id INTEGER PRIMARY KEY, name TEXT);", 0, 0, 0)))
    {
        bugf("export_item_type -> Failed to create table: item_type");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO item_type (id, name) VALUES (?1, ?2);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_item_type -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all continents
    for (x = 0; item_table[x].name != NULL; x++)
    {
        sqlite3_bind_int(stmt, 1, item_table[x].type);
        sqlite3_bind_text(stmt, 2, item_table[x].name, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", item_table[x].type, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_item_type -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_areas(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    AREA_DATA *pArea;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_areas -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS area;", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to drop table: area");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS area(vnum INTEGER PRIMARY KEY, name TEXT, min_level INTEGER, max_level INTEGER, builders TEXT, continent INTEGER, area_flags INTEGER);", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to create table: area");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO area (vnum, name, min_level, max_level, builders, continent, area_flags) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_areas -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all areas and save the area data for each entry.
    for (pArea = area_first; pArea; pArea = pArea->next)
    {
        sqlite3_bind_int(stmt, 1, pArea->vnum);
        sqlite3_bind_text(stmt, 2, pArea->name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, pArea->min_level);
        sqlite3_bind_int(stmt, 4, pArea->max_level);
        sqlite3_bind_text(stmt, 5, pArea->builders, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, pArea->continent);
        sqlite3_bind_int(stmt, 7, pArea->area_flags);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %s: %s\n", pArea->name, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_areas -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_clans(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    int x;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_clans -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS clan;", 0, 0, 0)))
    {
        bugf("export_clans -> Failed to drop table: clan");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS clan(name TEXT PRIMARY KEY, who_name TEXT, friendly_name TEXT, hall_vnum INTEGER, independent BOOLEAN);", 0, 0, 0)))
    {
        bugf("export_clans -> Failed to create table: clan");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO clan (name, who_name, friendly_name, hall_vnum, independent) VALUES (?1, ?2, ?3, ?4, ?5);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_clan -> Failed to prepare insert statement.");
        goto out;
    }

    // Loop over all continents
    for (x = 0; x < MAX_CLAN; x++)
    {
        sqlite3_bind_text(stmt, 1, clan_table[x].name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, clan_table[x].who_name, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, clan_table[x].friendly_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, clan_table[x].hall);
        sqlite3_bind_int(stmt, 5, clan_table[x].independent);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", clan_table[x].name, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_clan -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_bits(void)
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_bits -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS bit;", 0, 0, 0)))
    {
        bugf("export_bits -> Failed to drop table: bit");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS bit(id INTEGER PRIMARY KEY, name TEXT);", 0, 0, 0)))
    {
        bugf("export_bits -> Failed to create table: bit");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (1, 'A')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (2, 'B')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (4, 'C')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (8, 'D')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (16, 'E')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (32, 'F')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (64, 'G')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (128, 'H')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (256, 'I')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (512, 'J')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (1024, 'K')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (2048, 'L')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (4096, 'M')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (8192, 'N')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (16384, 'O')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (32768, 'P')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (65536, 'Q')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (131072, 'R')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (262144, 'S')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (524288, 'T')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (1048576, 'U')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (2097152, 'V')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (4194304, 'W')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (8388608, 'X')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (16777216, 'Y')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (33554432, 'Z')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (67108864, 'aa')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (134217728, 'bb')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (268435456, 'cc')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (536870912, 'dd')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (1073741824, 'ee')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (2147483648, 'ff')", 0, 0, 0);
    sqlite3_exec(db, "INSERT INTO bit(id, name) VALUES (4294967296, 'gg')", 0, 0, 0);

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_bits -> Failed to commit transaction.");
    }

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_help(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    HELP_DATA *pHelp;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_help -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS help;", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to drop table: help");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS help(id INTEGER PRIMARY KEY, level INTEGER, keyword TEXT, help_text TEXT);", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to create table: help");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO help (level, keyword, help_text) VALUES (?1, ?2, ?3);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_help -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all areas and save the area data for each entry.
    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
        sqlite3_bind_int(stmt, 1, pHelp->level);
        sqlite3_bind_text(stmt, 2, pHelp->keyword, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, pHelp->text, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %s: %s\n", pHelp->keyword, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_help -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

/*
 * TODO - Resets, extra descriptions.
 */
void export_rooms(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stmt_exit;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *room;
    int x = 0;
    int door = 0;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_rooms -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS room;", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to drop table: room");
        goto out;
    }

    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS room_exits;", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to drop table: room_exits");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS room(vnum INTEGER PRIMARY KEY, room_name TEXT, room_description TEXT, owner TEXT, room_flags INTEGER, light INTEGER, sector_type INTEGER, heal_rate INTEGER, mana_rate INTEGER, clan INTEGER, area_vnum INTEGER, area_name TEXT);", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to create table: room");
        goto out;
    }

    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS room_exits(vnum INTEGER, to_vnum INTEGER, dir INTEGER, dir_name TEXT, keyword TEXT, description TEXT, orig_door INTEGER, rs_flags INTEGER, exits_area BOOLEAN);", 0, 0, 0)))
    {
        bugf("Db Export -> Failed to create table: room");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO room(vnum, room_name, room_description, owner, room_flags, light, sector_type, heal_rate, mana_rate, clan, area_vnum, area_name) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_rooms -> Failed to prepare insert statement");
        goto out;
    }

    if (sqlite3_prepare(db, "INSERT INTO room_exits(vnum, to_vnum, dir, dir_name, keyword, description, orig_door, rs_flags, exits_area) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9);", -1, &stmt_exit, NULL) != SQLITE_OK)
    {
        bugf("export_rooms -> Failed to prepare insert statement for room_exits");
        goto out;
    }

    for (x = 0; x < MAX_KEY_HASH; x++) /* room index hash table */
    {
        for (room = room_index_hash[x]; room != NULL; room = room->next)
        {
            sqlite3_bind_int(stmt, 1, room->vnum);
            sqlite3_bind_text(stmt, 2, room->name, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, room->description, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, room->owner, -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 5, room->room_flags);
            sqlite3_bind_int(stmt, 6, room->light);
            sqlite3_bind_int(stmt, 7, room->sector_type);
            sqlite3_bind_int(stmt, 8, room->heal_rate);
            sqlite3_bind_int(stmt, 9, room->mana_rate);
            sqlite3_bind_int(stmt, 10, room->clan);
            sqlite3_bind_int(stmt, 11, room->area->vnum);
            sqlite3_bind_text(stmt, 12, room->area->name, -1, SQLITE_STATIC);

            rc = sqlite3_step(stmt);

            if (rc != SQLITE_DONE)
            {
                bugf("ERROR inserting data for %s (%d): %s\n", room->name, room->vnum, sqlite3_errmsg(db));
            }

            sqlite3_reset(stmt);

            // Now, insert the exits
            for (door = 0; door < MAX_DIR; door++)
            {
                if ((pexit = room->exit[door]) != NULL
                    && pexit->u1.to_room != NULL)
                {
                    sqlite3_bind_int(stmt_exit, 1, room->vnum);
                    sqlite3_bind_int(stmt_exit, 2, pexit->u1.to_room->vnum);
                    sqlite3_bind_int(stmt_exit, 3, door);
                    sqlite3_bind_text(stmt_exit, 4, dir_name[door], -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt_exit, 5, pexit->keyword, -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt_exit, 6, pexit->description, -1, SQLITE_STATIC);
                    sqlite3_bind_int(stmt_exit, 7, pexit->orig_door);
                    sqlite3_bind_int(stmt_exit, 8, pexit->rs_flags);
                    sqlite3_bind_int(stmt_exit, 9, (room->area->vnum != pexit->u1.to_room->area->vnum) ? TRUE : FALSE);

                    rc = sqlite3_step(stmt_exit);

                    if (rc != SQLITE_DONE)
                    {
                        bugf("ERROR inserting exit data for room %s (vnum=%d, exit=%d): %s\n", room->name, room->vnum, door, sqlite3_errmsg(db));
                    }

                    sqlite3_reset(stmt_exit);
                }
            }

        }
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_rooms -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);
    sqlite3_finalize(stmt_exit);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_classes(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    int x;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_extra_flags -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS class;", 0, 0, 0)))
    {
        bugf("export_classes -> Failed to drop table: class");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS class(id INTEGER PRIMARY KEY, name TEXT, who_name TEXT, attr_prime INTEGER, weapon INTEGER, skill_adept INTEGER, thac0_00 INTEGER, thac0_32 INTEGER, hp_min INTEGER, hp_max INTEGER, mana BOOLEAN, base_group TEXT, default_group TEXT, is_reclass BOOLEAN, is_enabled BOOLEAN);", 0, 0, 0)))
    {
        bugf("export_classes -> Failed to create table: class");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO class(id, name, who_name, attr_prime, weapon, skill_adept, thac0_00, thac0_32, hp_min, hp_max, mana, base_group, default_group, is_reclass, is_enabled) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_classes -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all extra flags
    for (x = 0; x < top_class; x++)
    {
        sqlite3_bind_int(stmt, 1, x);
        sqlite3_bind_text(stmt, 2, capitalize(class_table[x]->name), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, class_table[x]->who_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, class_table[x]->attr_prime);
        sqlite3_bind_int(stmt, 5, class_table[x]->weapon);
        sqlite3_bind_int(stmt, 6, class_table[x]->skill_adept);
        sqlite3_bind_int(stmt, 7, class_table[x]->thac0_00);
        sqlite3_bind_int(stmt, 8, class_table[x]->thac0_32);
        sqlite3_bind_int(stmt, 9, class_table[x]->hp_min);
        sqlite3_bind_int(stmt, 10, class_table[x]->hp_max);
        sqlite3_bind_int(stmt, 11, class_table[x]->mana);
        sqlite3_bind_text(stmt, 12, class_table[x]->base_group, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 13, class_table[x]->default_group, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 14, class_table[x]->is_reclass);
        sqlite3_bind_int(stmt, 15, class_table[x]->is_enabled);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", capitalize(class_table[x]->name), sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_classes -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_stats(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_stats -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS stat;", 0, 0, 0)))
    {
        bugf("export_stats -> Failed to drop table: stat");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS stat(id INTEGER PRIMARY KEY, name TEXT, short_name TEXT);", 0, 0, 0)))
    {
        bugf("export_stat -> Failed to create table: stat");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO stat(id, name, short_name) VALUES (?1, ?2, ?3);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_stats -> Failed to prepare insert statement");
        goto out;
    }

    sqlite3_bind_int(stmt, 1, 0);
    sqlite3_bind_text(stmt, 2, "Strength", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Str", -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, 1);
    sqlite3_bind_text(stmt, 2, "Intelligence", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Int", -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, 2);
    sqlite3_bind_text(stmt, 2, "Wisdom", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Wis", -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, 3);
    sqlite3_bind_text(stmt, 2, "Dexterity", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Dex", -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, 4);
    sqlite3_bind_text(stmt, 2, "Constitution", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, "Con", -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_stats -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}

void export_directions(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_directions -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS directions;", 0, 0, 0)))
    {
        bugf("export_directions -> Failed to drop table: directions");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS directions(dir_id INTEGER PRIMARY KEY, name TEXT, reverse_dir_id INTEGER, reverse_name TEXT);", 0, 0, 0)))
    {
        bugf("export_directions -> Failed to create table: directions");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO directions(dir_id, name, reverse_dir_id, reverse_name) VALUES (?1, ?2, ?3, ?4);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_directions -> Failed to prepare insert statement");
        goto out;
    }

    sqlite3_bind_int(stmt, 1, DIR_NORTH);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_NORTH], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_SOUTH);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_SOUTH], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_EAST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_EAST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_WEST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_WEST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_SOUTH);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_SOUTH], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_NORTH);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_NORTH], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_WEST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_WEST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_EAST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_EAST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_UP);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_UP], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_DOWN);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_DOWN], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_DOWN);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_DOWN], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_UP);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_UP], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_NORTHWEST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_NORTHWEST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_SOUTHEAST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_SOUTHEAST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_NORTHEAST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_NORTHEAST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_SOUTHWEST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_SOUTHWEST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_SOUTHWEST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_SOUTHWEST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_NORTHEAST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_NORTHEAST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    sqlite3_bind_int(stmt, 1, DIR_SOUTHEAST);
    sqlite3_bind_text(stmt, 2, dir_name[DIR_SOUTHEAST], -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, DIR_NORTHWEST);
    sqlite3_bind_text(stmt, 4, dir_name[DIR_NORTHWEST], -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_reset(stmt);

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_directions -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

out:
    // Cleanup
    sqlite3_close(db);
    return;
}
