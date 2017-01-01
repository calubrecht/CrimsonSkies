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
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* item type list */
const struct item_type item_table[] = {
    {ITEM_LIGHT,        "light"},
    {ITEM_SCROLL,       "scroll"},
    {ITEM_WAND,         "wand"},
    {ITEM_STAFF,        "staff"},
    {ITEM_WEAPON,       "weapon"},
    {ITEM_TREASURE,     "treasure"},
    {ITEM_ARMOR,        "armor"},
    {ITEM_POTION,       "potion"},
    {ITEM_CLOTHING,     "clothing"},
    {ITEM_FURNITURE,    "furniture"},
    {ITEM_TRASH,        "trash"},
    {ITEM_CONTAINER,    "container"},
    {ITEM_DRINK_CON,    "drink"},
    {ITEM_KEY,          "key"},
    {ITEM_FOOD,         "food"},
    {ITEM_MONEY,        "money"},
    {ITEM_BOAT,         "boat"},
    {ITEM_CORPSE_NPC,   "npc_corpse"},
    {ITEM_CORPSE_PC,    "pc_corpse"},
    {ITEM_FOUNTAIN,     "fountain"},
    {ITEM_PILL,         "pill"},
    {ITEM_PROTECT,      "protect"},
    {ITEM_MAP,          "map"},
    {ITEM_PORTAL,       "portal"},
    {ITEM_WARP_STONE,   "warp_stone"},
    {ITEM_ROOM_KEY,     "room_key"},
    {ITEM_GEM,          "gem"},
    {ITEM_JEWELRY,      "jewelry"},
    {ITEM_SHOVEL,       "shovel"},
    {ITEM_FOG,          "fog"},
    {0,                 NULL}
};


/* weapon selection table */
const struct weapon_type weapon_table[] = {
    {"sword",   OBJ_VNUM_SCHOOL_SWORD,   WEAPON_SWORD,   &gsn_sword},
    {"mace",    OBJ_VNUM_SCHOOL_MACE,    WEAPON_MACE,    &gsn_mace},
    {"dagger",  OBJ_VNUM_SCHOOL_DAGGER,  WEAPON_DAGGER,  &gsn_dagger},
    {"axe",     OBJ_VNUM_SCHOOL_AXE,     WEAPON_AXE,     &gsn_axe},
    {"staff",   OBJ_VNUM_SCHOOL_STAFF,   WEAPON_SPEAR,   &gsn_spear},
    {"flail",   OBJ_VNUM_SCHOOL_FLAIL,   WEAPON_FLAIL,   &gsn_flail},
    {"whip",    OBJ_VNUM_SCHOOL_WHIP,    WEAPON_WHIP,    &gsn_whip},
    {"polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM, &gsn_polearm},
    {NULL,      0,                       0,              NULL}
};



/* wiznet table and prototype for future flag setting */
const struct wiznet_type wiznet_table[] = {
    {"on",        WIZ_ON,        IM},
    {"prefix",    WIZ_PREFIX,    IM},
    {"ticks",     WIZ_TICKS,     IM},
    {"logins",    WIZ_LOGINS,    IM},
    {"sites",     WIZ_SITES,     L4},
    {"links",     WIZ_LINKS,     L7},
    {"newbies",   WIZ_NEWBIE,    IM},
    {"spam",      WIZ_SPAM,      L5},
    {"deaths",    WIZ_DEATHS,    IM},
    {"resets",    WIZ_RESETS,    L4},
    {"mobdeaths", WIZ_MOBDEATHS, L4},
    {"flags",     WIZ_FLAGS,     L5},
    {"penalties", WIZ_PENALTIES, L5},
    {"saccing",   WIZ_SACCING,   L5},
    {"levels",    WIZ_LEVELS,    IM},
    {"load",      WIZ_LOAD,      L2},
    {"restore",   WIZ_RESTORE,   L2},
    {"snoops",    WIZ_SNOOPS,    L2},
    {"switches",  WIZ_SWITCHES,  L2},
    {"secure",    WIZ_SECURE,    L1},
    {"general",   WIZ_GENERAL,   IM},
    {NULL, 0, 0}
};

/* attack table  -- not very organized :( */
const struct attack_type attack_table[MAX_DAMAGE_MESSAGE] = {
    {"none", "hit", -1},        /*  0 */
    {"slice", "slice", DAM_SLASH},
    {"stab", "stab", DAM_PIERCE},
    {"slash", "slash", DAM_SLASH},
    {"whip", "whip", DAM_SLASH},
    {"claw", "claw", DAM_SLASH},    /*  5 */
    {"blast", "blast", DAM_BASH},
    {"pound", "pound", DAM_BASH},
    {"crush", "crush", DAM_BASH},
    {"grep", "grep", DAM_SLASH},
    {"bite", "bite", DAM_PIERCE},    /* 10 */
    {"pierce", "pierce", DAM_PIERCE},
    {"suction", "suction", DAM_BASH},
    {"beating", "beating", DAM_BASH},
    {"digestion", "digestion", DAM_ACID},
    {"charge", "charge", DAM_BASH},    /* 15 */
    {"slap", "slap", DAM_BASH},
    {"punch", "punch", DAM_BASH},
    {"wrath", "wrath", DAM_ENERGY},
    {"magic", "magic", DAM_ENERGY},
    {"divine", "divine power", DAM_HOLY},    /* 20 */
    {"cleave", "cleave", DAM_SLASH},
    {"scratch", "scratch", DAM_PIERCE},
    {"peck", "peck", DAM_PIERCE},
    {"peckb", "peck", DAM_BASH},
    {"chop", "chop", DAM_SLASH},    /* 25 */
    {"sting", "sting", DAM_PIERCE},
    {"smash", "smash", DAM_BASH},
    {"shbite", "shocking bite", DAM_LIGHTNING},
    {"flbite", "flaming bite", DAM_FIRE},
    {"frbite", "freezing bite", DAM_COLD},    /* 30 */
    {"acbite", "acidic bite", DAM_ACID},
    {"chomp", "chomp", DAM_PIERCE},
    {"drain", "life drain", DAM_NEGATIVE},
    {"thrust", "thrust", DAM_PIERCE},
    {"slime", "slime", DAM_ACID},
    {"shock", "shock", DAM_LIGHTNING},
    {"thwack", "thwack", DAM_BASH},
    {"flame", "flame", DAM_FIRE},
    {"chill", "chill", DAM_COLD},
    {NULL, NULL, 0}
};

/* race table */
const struct race_type race_table[] = {
/*
    {
    name,        pc_race?,
    act bits,    aff_by bits,    off bits,
    imm,        res,        vuln,
    form,        parts 
    },
*/
    {"unique", FALSE, 0, 0, 0, 0, 0, 0, 0, 0},

    {
     "human", TRUE,
     0, 0, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "elf", TRUE,
     0, AFF_INFRARED, 0,
     0, RES_CHARM, VULN_IRON,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "dwarf", TRUE,
     0, AFF_INFRARED, 0,
     0, RES_POISON | RES_DISEASE, VULN_DROWNING,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "ogre", TRUE,
     0, 0, 0,
     0, RES_FIRE | RES_COLD, VULN_MENTAL | VULN_LIGHTNING,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "kender", TRUE,
     0, 0, 0,
     0, RES_BASH, VULN_COLD | VULN_POISON,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},
    {
     "minotaur", TRUE,
     0, 0, 0,
     0, 0, VULN_FIRE,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},
    {
     "bat", FALSE,
     0, AFF_FLYING | AFF_DARK_VISION, OFF_DODGE | OFF_FAST,
     0, 0, VULN_LIGHT,
     A | G | V, A | C | D | E | F | H | J | K | P},

    {
     "bear", FALSE,
     0, 0, OFF_CRUSH | OFF_DISARM | OFF_BERSERK,
     0, RES_BASH | RES_COLD, 0,
     A | G | V, A | B | C | D | E | F | H | J | K | U | V},

    {
     "cat", FALSE,
     0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | Q | U | V},

    {
     "centipede", FALSE,
     0, AFF_DARK_VISION, 0,
     0, RES_PIERCE | RES_COLD, VULN_BASH,
     A | B | G | O, A | C | K},

    {
     "dog", FALSE,
     0, 0, OFF_FAST,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | U | V},

    {
     "doll", FALSE,
     0, 0, 0,
     IMM_COLD | IMM_POISON | IMM_HOLY | IMM_NEGATIVE | IMM_MENTAL |
     IMM_DISEASE | IMM_DROWNING, RES_BASH | RES_LIGHT,
     VULN_SLASH | VULN_FIRE | VULN_ACID | VULN_LIGHTNING | VULN_ENERGY,
     E | J | M | cc, A | B | C | G | H | K},

    {"dragon", FALSE,
     0, AFF_INFRARED | AFF_FLYING, 0,
     0, RES_FIRE | RES_BASH | RES_CHARM,
     VULN_PIERCE | VULN_COLD,
     A | H | Z, A | C | D | E | F | G | H | I | J | K | P | Q | U | V | X},

    {
     "fido", FALSE,
     0, 0, OFF_DODGE | ASSIST_RACE,
     0, 0, VULN_MAGIC,
     A | B | G | V, A | C | D | E | F | H | J | K | Q | V},

    {
     "fox", FALSE,
     0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K | Q | V},

    {
     "goblin", FALSE,
     0, AFF_INFRARED, 0,
     0, RES_DISEASE, VULN_MAGIC,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "hobgoblin", FALSE,
     0, AFF_INFRARED, 0,
     0, RES_DISEASE | RES_POISON, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Y},

    {
     "kobold", FALSE,
     0, AFF_INFRARED, 0,
     0, RES_POISON, VULN_MAGIC,
     A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | Q},

    {
     "lizard", FALSE,
     0, 0, 0,
     0, RES_POISON, VULN_COLD,
     A | G | X | cc, A | C | D | E | F | H | K | Q | V},

    {
     "modron", FALSE,
     0, AFF_INFRARED, ASSIST_RACE | ASSIST_ALIGN,
     IMM_CHARM | IMM_DISEASE | IMM_MENTAL | IMM_HOLY | IMM_NEGATIVE,
     RES_FIRE | RES_COLD | RES_ACID, 0,
     H, A | B | C | G | H | J | K},

    {
     "orc", FALSE,
     0, AFF_INFRARED, 0,
     0, RES_DISEASE, VULN_LIGHT,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},

    {
     "pig", FALSE,
     0, 0, 0,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K},

    {
     "rabbit", FALSE,
     0, 0, OFF_DODGE | OFF_FAST,
     0, 0, 0,
     A | G | V, A | C | D | E | F | H | J | K},

    {
     "school monster", FALSE,
     ACT_NOALIGN, 0, 0,
     IMM_CHARM | IMM_SUMMON, 0, VULN_MAGIC,
     A | M | V, A | B | C | D | E | F | H | J | K | Q | U},

    {
     "snake", FALSE,
     0, 0, 0,
     0, RES_POISON, VULN_COLD,
     A | G | X | Y | cc, A | D | E | F | K | L | Q | V | X},

    {
     "song bird", FALSE,
     0, AFF_FLYING, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | W, A | C | D | E | F | H | K | P},

    {
     "troll", FALSE,
     0, AFF_REGENERATION | AFF_INFRARED | AFF_DETECT_HIDDEN,
     OFF_BERSERK,
     0, RES_CHARM | RES_BASH, VULN_FIRE | VULN_ACID,
     A | B | H | M | V, A | B | C | D | E | F | G | H | I | J | K | U | V},

    {
     "water fowl", FALSE,
     0, AFF_SWIM | AFF_FLYING, 0,
     0, RES_DROWNING, 0,
     A | G | W, A | C | D | E | F | H | K | P},

    {
     "wolf", FALSE,
     0, AFF_DARK_VISION, OFF_FAST | OFF_DODGE,
     0, 0, 0,
     A | G | V, A | C | D | E | F | J | K | Q | V},

    {
     "wyvern", FALSE,
     0, AFF_FLYING | AFF_DETECT_INVIS | AFF_DETECT_HIDDEN,
     OFF_BASH | OFF_FAST | OFF_DODGE,
     IMM_POISON, 0, VULN_LIGHT,
     A | B | G | Z, A | C | D | E | F | H | J | K | Q | V | X},

    {
     "fish", FALSE,
     0, AFF_SWIM, 0,
     0, RES_DROWNING, 0,
     A | G | H | M | V | W, A | D | E | F | K | O},

    {
     "unique", FALSE,
     0, 0, 0,
     0, 0, 0,
     0, 0},


    {
     NULL, 0, 0, 0, 0, 0, 0}
};

struct pc_race_type pc_race_table[] = {
    {"null race", "", "", 0, {100, 100, 100, 100, 100},
     {""}, {13, 13, 13, 13, 13}, {18, 18, 18, 18, 18}, 0},

/*
    {
    "race name", who name, name with article, creation points, { class multipliers },
    { bonus skills },
    { base stats (str, int, wis, dex, con) }, { max stats (str, int, wis, dex, con) },
    size
    },
*/
    {
     // 65 starting stat line, 90 max stat line
     "human", "Human", "a human", 0, {100, 100, 100, 100, 100},
     {"swim"},
     {13, 13, 13, 13, 13}, {18, 18, 18, 18, 18}, SIZE_MEDIUM},
    {
     // 65 starting stat line, 91 max stat line
     "elf", " Elf ", "an elf", 5, {100, 125, 100, 120, 100},
     {"sneak", "hide", "swim"},
     {12, 14, 13, 15, 11}, {16, 20, 18, 21, 16}, SIZE_SMALL},

    {
     // 65 starting stat line, 90 max stat line
     "dwarf", "Dwarf", "a dwarf", 8, {150, 100, 125, 100, 150},
     {"berserk"},
     {14, 12, 14, 10, 15}, {20, 16, 19, 14, 21}, SIZE_MEDIUM},

    {
     // 65 starting stat line, 90 max stat line
     "ogre", "Ogre", "an ogre", 6, {200, 150, 150, 105, 200},
     {"bash", "fast healing"},
     {16, 11, 13, 11, 14}, {22, 15, 18, 15, 20}, SIZE_LARGE},

    {
     // 64 starting stat line, 88 max stat line
     "kender", "Kender", "a kender", 5, {100, 100, 100, 100, 100},
     {"sneak", "swim", "peek", "dodge", "pick lock"},
     {10, 12, 14, 18, 10}, {14, 18, 19, 22, 15}, SIZE_SMALL},
    {
     // 64 starting stat line, 89 max stat line
     "minotaur", "Minotr", "a minotaur", 6, {200, 150, 150, 105, 200},
     {"gore"},
     {13, 12, 12, 14, 13}, {20, 18, 17, 14, 20}, SIZE_LARGE}
};

/*
 * Modifiers for Strength based items.
 * Field 1: tohit - Hit roll bonus
 * Field 2: todam - Dam roll bonus
 * Field 3: carry - Carry modifier
 * Field 4: wield - Wield modifier (factors into dropping weapon when weak)
 */
const struct str_app_type str_app[26] = {
    {-5, -4, 0, 0},                /* 0  */
    {-5, -4, 3, 1},                /* 1  */
    {-3, -2, 3, 2},
    {-3, -1, 10, 3},            /* 3  */
    {-2, -1, 25, 4},
    {-2, -1, 55, 5},            /* 5  */
    {-1, 0, 80, 6},
    {-1, 0, 90, 7},
    {0, 0, 100, 8},
    {0, 0, 100, 9},
    {0, 0, 115, 10},            /* 10  */
    {0, 0, 115, 11},
    {0, 0, 130, 12},
    {0, 0, 130, 13},            /* 13  */
    {0, 1, 140, 14},
    {1, 1, 150, 15},            /* 15  */
    {1, 2, 165, 16},
    {2, 3, 180, 22},
    {2, 3, 200, 25},            /* 18  */
    {3, 4, 225, 30},
    {3, 5, 250, 35},            /* 20  */
    {4, 6, 300, 40},
    {4, 6, 350, 45},
    {5, 7, 400, 50},
    {5, 8, 450, 55},
    {6, 9, 500, 60}                /* 25   */
};


/*
 * Modifiers for Intelligence based items.
 * Field 1:  Learning modifier (higher the better)
 */
const struct int_app_type int_app[26] = {
    {3},                        /*  0 */
    {5},                        /*  1 */
    {7},
    {8},                        /*  3 */
    {9},
    {10},                        /*  5 */
    {11},
    {12},
    {13},
    {15},
    {17},                        /* 10 */
    {19},
    {22},
    {25},
    {28},
    {31},                        /* 15 */
    {34},
    {37},
    {40},                        /* 18 */
    {44},
    {49},                        /* 20 */
    {55},
    {60},
    {70},
    {80},
    {85}                        /* 25 */
};


/*
 * Modifiers for Wisdom based items.
 * Field 1: Number of practices gained for player wisdom
 */
const struct wis_app_type wis_app[26] = {
    {0},                        /*  0 */
    {0},                        /*  1 */
    {0},
    {0},                        /*  3 */
    {0},
    {1},                        /*  5 */
    {1},
    {1},
    {1},
    {1},
    {1},                        /* 10 */
    {2},
    {2},
    {2},
    {2},
    {3},                        /* 15 */
    {3},
    {3},
    {3},                        /* 18 */
    {3},
    {4},                        /* 20 */
    {4},
    {5},
    {5},
    {5},
    {6}                         /* 25 */
};

/*
 * Modifiers for Dexterity based items.
 * Field 1:  Does not appear to be in use anymore, repurpose later.
 */
const struct dex_app_type dex_app[26] = {
    {60},                        /* 0 */
    {50},                        /* 1 */
    {50},
    {40},
    {30},
    {20},                        /* 5 */
    {10},
    {0},
    {0},
    {0},
    {0},                        /* 10 */
    {0},
    {0},
    {0},
    {0},
    {-10},                        /* 15 */
    {-15},
    {-20},
    {-30},
    {-40},
    {-50},                        /* 20 */
    {-60},
    {-75},
    {-90},
    {-105},
    {-120}                        /* 25 */
};


/*
 * Constitution modifiers.
 * Field 1: HP bonus on level
 * Field 2: Shock
 */
const struct con_app_type con_app[26] = {
    {-4, 20},   /*  0 */
    {-3, 25},   /*  1 */
    {-2, 30},   /*  2 */
    {-2, 35},   /*  3 */
    {-1, 40},   /*  4 */
    {-1, 45},   /*  5 */
    {-1, 50},   /*  6 */
    {0, 55},    /*  7 */
    {0, 60},    /*  8 */
    {0, 65},    /*  9 */
    {0, 70},    /* 10 */
    {0, 75},    /* 11 */
    {0, 80},    /* 12 */
    {0, 85},    /* 13 */
    {0, 88},    /* 14 */
    {1, 90},    /* 15 */
    {2, 95},    /* 16 */
    {2, 97},    /* 17 */
    {3, 99},    /* 18 */
    {3, 99},    /* 19 */
    {4, 99},    /* 20 */
    {4, 99},    /* 21 */
    {5, 99},    /* 22 */
    {6, 99},    /* 23 */
    {7, 99},    /* 24 */
    {8, 99}     /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[] = {
/*    name            color    proof, full, thirst, food, ssize */
    {"water", "clear", {0, 1, 10, 0, 16}},
    {"beer", "amber", {12, 1, 8, 1, 12}},
    {"red wine", "burgundy", {30, 1, 8, 1, 5}},
    {"ale", "brown", {15, 1, 8, 1, 12}},
    {"dark ale", "dark", {16, 1, 8, 1, 12}},

    {"whisky", "golden", {120, 1, 5, 0, 2}},
    {"lemonade", "pink", {0, 1, 9, 2, 12}},
    {"firebreather", "boiling", {190, 0, 4, 0, 2}},
    {"local specialty", "clear", {151, 1, 3, 0, 2}},
    {"slime mold juice", "green", {0, 2, -8, 1, 2}},

    {"milk", "white", {0, 2, 9, 3, 12}},
    {"tea", "tan", {0, 1, 8, 0, 6}},
    {"coffee", "black", {0, 1, 8, 0, 6}},
    {"blood", "red", {0, 2, -1, 2, 6}},
    {"salt water", "clear", {0, 1, -2, 0, 1}},

    {"root beer", "brown", {0, 2, 9, 2, 12}},
    {"elvish wine", "green", {35, 2, 8, 1, 5}},
    {"white wine", "golden", {28, 1, 8, 1, 5}},
    {"champagne", "golden", {32, 1, 8, 1, 5}},

    {"mead", "honey-colored", {34, 2, 8, 2, 12}},
    {"rose wine", "pink", {26, 1, 8, 1, 5}},
    {"benedictine wine", "burgundy", {40, 1, 8, 1, 5}},
    {"vodka", "clear", {130, 1, 5, 0, 2}},
    {"cranberry juice", "red", {0, 1, 9, 2, 12}},

    {"orange juice", "orange", {0, 2, 9, 3, 12}},
    {"absinthe", "green", {200, 1, 4, 0, 2}},
    {"brandy", "golden", {80, 1, 5, 0, 4}},
    {"aquavit", "clear", {140, 1, 5, 0, 2}},
    {"schnapps", "clear", {90, 1, 5, 0, 2}},

    {"icewine", "purple", {50, 2, 6, 1, 5}},
    {"amontillado", "burgundy", {35, 2, 8, 1, 5}},
    {"sherry", "red", {38, 2, 7, 1, 5}},
    {"framboise", "red", {50, 1, 7, 1, 5}},
    {"rum", "amber", {151, 1, 4, 0, 2}},

    {"cordial", "clear", {100, 1, 5, 0, 2}},
    {NULL, NULL, {0, 0, 0, 0, 0}}
};
