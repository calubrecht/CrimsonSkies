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
 * Although we are still using flat files to store all of the game data,   *
 * we will use this module to export that data into a sqlite database      *
 * which can be easily queried and used in other locations such as a web   *
 * site to hopefully provide better tools to mine the game data for        *
 * players and game admins. Some of the data maybe redudant to to allow    *
 * for less joins when querying (e.g. the object row will include the area *
 * name as well as the area_vnum.  I don't expect that this will be an     *
 * issue.
 *                                                                         *
 ***************************************************************************/

/*
    TODO
    ----
    Object Affects pObjData->affects

*/

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

#include "sqlite3\sqlite3.h"
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
void export_sector_type(void);
void export_clans(void);
void export_extra_flags(void);
void export_bits(void);

#define HEADER "--------------------------------------------------------------------------------\r\n"

/*
 * Command to initiate the exporting of all game data into a sqlite database that can then be used
 * elsewhere such as a web site.
 */
void do_dbexport(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    printf_to_char(ch, "%-55sStatus\r\n", "Action");
    send_to_char(HEADER, ch);

    printf_to_char(ch, "%-55s", "Exporting Bits");
    export_bits();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Continents");
    export_continents();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Item Types");
    export_item_type();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Sector Types");
    export_sector_type();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Areas");
    export_areas();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Objects");
    export_objects();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Extra Flags");
    export_extra_flags();
    send_to_char("[ {GComplete{x ]\r\n", ch);

    printf_to_char(ch, "%-55s", "Exporting Clans");
    export_clans();
    send_to_char("[ {GComplete{x ]\r\n", ch);


    send_to_char("\r\nExport of game data complete!\r\n", ch);
}

void export_objects(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *obj;
    int vnum = 0;
    int nMatch = 0;
    OBJ_INDEX_DATA *pObjIndex;

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

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS object(vnum INTEGER PRIMARY KEY, name TEXT, short_description TEXT, description TEXT, material TEXT, item_type INTEGER, extra_flags INTEGER, wear_flags INTEGER, level INTEGER, condition INTEGER, weight INTEGER, cost INTEGER, value1 INTEGER, value2 INTEGER, value3 INTEGER, value4 INTEGER, value5 INTEGER, area_name TEXT, area_vnum INTEGER);", 0, 0, 0)))
    {
        bugf("export_object -> Failed to create table: object");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO object (vnum, name, short_description, description, material, item_type, extra_flags, wear_flags, level, condition, weight, cost, value1, value2, value3, value4, value5, area_name, area_vnum) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_object -> Failed to prepare insert statement.");
        goto out;
    }

    // Loop over all object index data's
    for (vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ((pObjIndex = get_obj_index(vnum)) != NULL)
        {
            nMatch++;

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

        }
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_objects -> Failed to commit transaction.");
    }

    sqlite3_finalize(stmt);

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
    AREA_DATA *pArea;
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
    AREA_DATA *pArea;
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

void export_sector_type(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    AREA_DATA *pArea;
    int x;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_sector_type -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS sector_type;", 0, 0, 0)))
    {
        bugf("export_sector_type -> Failed to drop table: sector_type");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS sector_type(id INTEGER PRIMARY KEY, name TEXT);", 0, 0, 0)))
    {
        bugf("export_sector_type -> Failed to create table: sector_type");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO sector_type (id, name) VALUES (?1, ?2);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_sector_type -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all continents
    for (x = 0; sector_flags[x].name != NULL; x++)
    {
        sqlite3_bind_int(stmt, 1, sector_flags[x].bit);
        sqlite3_bind_text(stmt, 2, sector_flags[x].name, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", sector_flags[x].bit, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_sector_type -> Failed to commit transaction.");
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
    AREA_DATA *pArea;
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
            bugf("ERROR inserting data for %d: %s\n", sector_flags[x].bit, sqlite3_errmsg(db));
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

void export_extra_flags(void)
{
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    AREA_DATA *pArea;
    int x;

    rc = sqlite3_open(EXPORT_DATABASE_FILE, &db);

    if (rc != SQLITE_OK)
    {
        bugf("export_extra_flags -> Failed to open %s", EXPORT_DATABASE_FILE);
        goto out;
    }


    // Total reload everytime, drop the table if it exists.
    if ((sqlite3_exec(db, "DROP TABLE IF EXISTS extra_flags;", 0, 0, 0)))
    {
        bugf("export_extra_flags -> Failed to drop table: extra_flags");
        goto out;
    }

    // Create the tables they do not exist
    if ((sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS extra_flags(id INTEGER PRIMARY KEY, name TEXT);", 0, 0, 0)))
    {
        bugf("export_extra_flags -> Failed to create table: extra_flags");
        goto out;
    }

    // Begin a transaction
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    // Prepare the insert statement that we'll re-use in the loop
    if (sqlite3_prepare(db, "INSERT INTO extra_flags (id, name) VALUES (?1, ?2);", -1, &stmt, NULL) != SQLITE_OK)
    {
        bugf("export_extra_flags -> Failed to prepare insert statement");
        goto out;
    }

    // Loop over all extra flags
    for (x = 0; extra_flags[x].name != NULL; x++)
    {
        sqlite3_bind_int(stmt, 1, extra_flags[x].bit);
        sqlite3_bind_text(stmt, 2, extra_flags[x].name, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
        {
            bugf("ERROR inserting data for %d: %s\n", item_table[x].type, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
    }

    if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
    {
        bugf("export_extra_flags -> Failed to commit transaction.");
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
    sqlite3_stmt *stmt;
    AREA_DATA *pArea;
    int x;

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