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
#include "tables.h"
#include "interp.h"

SKILLTYPE *             skill_table[MAX_SKILL];
GROUPTYPE *             group_table[MAX_GROUP];
CLASSTYPE *             class_table[MAX_CLASS];

/* for position */
const struct position_type position_table[] = {
    {"dead",             "dead"},
    {"mortally wounded", "mort"},
    {"incapacitated",    "incap"},
    {"stunned",          "stun"},
    {"sleeping",         "sleep"},
    {"resting",          "rest"},
    {"sitting",          "sit"},
    {"fighting",         "fight"},
    {"standing",         "stand"},
    {NULL, NULL}
};

/* for sex */
const struct sex_type sex_table[] = {
    {"none"},
    {"male"},
    {"female"},
    {"either"},
    {NULL}
};

/* for sizes */
const struct size_type size_table[] = {
    {"tiny"},
    {"small"},
    {"medium"},
    {"large"},
    {"huge",},
    {"giant"},
    {NULL}
};

/* various flag tables */
const struct flag_type act_flags[] = {
    {"npc",            A, FALSE},
    {"sentinel",       B, TRUE},
    {"scavenger",      C, TRUE},
    {"portalmerchant", D, TRUE},
    {"tracker",        E, TRUE},
    {"aggressive",     F, TRUE},
    {"stay_area",      G, TRUE},
    {"wimpy",          H, TRUE},
    {"pet",            I, TRUE},
    {"train",          J, TRUE},
    {"practice",       K, TRUE},
    {"banker",         L, TRUE},
    {"undead",         O, TRUE},
    {"cleric",         Q, TRUE},
    {"mage",           R, TRUE},
    {"thief",          S, TRUE},
    {"warrior",        T, TRUE},
    {"noalign",        U, TRUE},
    {"nopurge",        V, TRUE},
    {"outdoors",       W, TRUE},
    {"indoors",        Y, TRUE},
    {"scribe",         Z, TRUE},
    {"healer",        aa, TRUE},
    {"gain",          bb, TRUE},
    {"update_always", cc, TRUE},
    {"changer",       dd, TRUE},
    {NULL,             0, FALSE}
};

const struct flag_type plr_flags[] = {
    {"npc",         A, FALSE},
    {"autoassist",  C, FALSE},
    {"autoexit",    D, FALSE},
    {"autoloot",    E, FALSE},
    {"autosac",     F, FALSE},
    {"autogold",    G, FALSE},
    {"autosplit",   H, FALSE},
    {"holylight",   N, FALSE},
    {"can_loot",    P, FALSE},
    {"nosummon",    Q, FALSE},
    {"nofollow",    R, FALSE},
    {"color",       T, FALSE},
    {"permit",      U, TRUE},
    {"log",         W, FALSE},
    {"deny",        X, FALSE},
    {"freeze",      Y, FALSE},
    {"thief",       Z, FALSE},
    {"killer",     aa, FALSE},
    {NULL,          0, 0}
};

const struct flag_type affect_flags[] = {
    {"blind", A, TRUE},
    {"invisible", B, TRUE},
    {"detect_evil", C, TRUE},
    {"detect_invis", D, TRUE},
    {"detect_magic", E, TRUE},
    {"detect_hidden", F, TRUE},
    {"detect_good", G, TRUE},
    {"sanctuary", H, TRUE},
    {"faerie_fire", I, TRUE},
    {"infrared", J, TRUE},
    {"curse", K, TRUE},
    {"poison", M, TRUE},
    {"protect_evil", N, TRUE},
    {"protect_good", O, TRUE},
    {"sneak", P, TRUE},
    {"hide", Q, TRUE},
    {"sleep", R, TRUE},
    {"charm", S, TRUE},
    {"flying", T, TRUE},
    {"pass_door", U, TRUE},
    {"haste", V, TRUE},
    {"calm", W, TRUE},
    {"plague", X, TRUE},
    {"weaken", Y, TRUE},
    {"dark_vision", Z, TRUE},
    {"berserk", aa, TRUE},
    {"swim", bb, TRUE},
    {"regeneration", cc, TRUE},
    {"slow", dd, TRUE},
    {NULL, 0, 0}
};

const struct flag_type off_flags[] = {
    {"area_attack", A, TRUE},
    {"backstab", B, TRUE},
    {"bash", C, TRUE},
    {"berserk", D, TRUE},
    {"disarm", E, TRUE},
    {"dodge", F, TRUE},
    {"fade", G, TRUE},
    {"fast", H, TRUE},
    {"kick", I, TRUE},
    {"dirt_kick", J, TRUE},
    {"parry", K, TRUE},
    {"rescue", L, TRUE},
    {"tail", M, TRUE},
    {"trip", N, TRUE},
    {"crush", O, TRUE},
    {"assist_all", P, TRUE},
    {"assist_align", Q, TRUE},
    {"assist_race", R, TRUE},
    {"assist_players", S, TRUE},
    {"assist_guard", T, TRUE},
    {"assist_vnum", U, TRUE},
    {NULL, 0, 0}
};

const struct flag_type imm_flags[] = {
    {"summon", A, TRUE},
    {"charm", B, TRUE},
    {"magic", C, TRUE},
    {"weapon", D, TRUE},
    {"bash", E, TRUE},
    {"pierce", F, TRUE},
    {"slash", G, TRUE},
    {"fire", H, TRUE},
    {"cold", I, TRUE},
    {"lightning", J, TRUE},
    {"acid", K, TRUE},
    {"poison", L, TRUE},
    {"negative", M, TRUE},
    {"holy", N, TRUE},
    {"energy", O, TRUE},
    {"mental", P, TRUE},
    {"disease", Q, TRUE},
    {"drowning", R, TRUE},
    {"light", S, TRUE},
    {"sound", T, TRUE},
    {"wood", X, TRUE},
    {"silver", Y, TRUE},
    {"iron", Z, TRUE},
    {NULL, 0, 0}
};

const struct flag_type form_flags[] = {
    {"edible", FORM_EDIBLE, TRUE},
    {"poison", FORM_POISON, TRUE},
    {"magical", FORM_MAGICAL, TRUE},
    {"instant_decay", FORM_INSTANT_DECAY, TRUE},
    {"other", FORM_OTHER, TRUE},
    {"animal", FORM_ANIMAL, TRUE},
    {"sentient", FORM_SENTIENT, TRUE},
    {"undead", FORM_UNDEAD, TRUE},
    {"construct", FORM_CONSTRUCT, TRUE},
    {"mist", FORM_MIST, TRUE},
    {"intangible", FORM_INTANGIBLE, TRUE},
    {"biped", FORM_BIPED, TRUE},
    {"centaur", FORM_CENTAUR, TRUE},
    {"insect", FORM_INSECT, TRUE},
    {"spider", FORM_SPIDER, TRUE},
    {"crustacean", FORM_CRUSTACEAN, TRUE},
    {"worm", FORM_WORM, TRUE},
    {"blob", FORM_BLOB, TRUE},
    {"mammal", FORM_MAMMAL, TRUE},
    {"bird", FORM_BIRD, TRUE},
    {"reptile", FORM_REPTILE, TRUE},
    {"snake", FORM_SNAKE, TRUE},
    {"dragon", FORM_DRAGON, TRUE},
    {"amphibian", FORM_AMPHIBIAN, TRUE},
    {"fish", FORM_FISH, TRUE},
    {"cold_blood", FORM_COLD_BLOOD, TRUE},
    {NULL, 0, 0}
};

const struct flag_type part_flags[] = {
    {"head", PART_HEAD, TRUE},
    {"arms", PART_ARMS, TRUE},
    {"legs", PART_LEGS, TRUE},
    {"heart", PART_HEART, TRUE},
    {"brains", PART_BRAINS, TRUE},
    {"guts", PART_GUTS, TRUE},
    {"hands", PART_HANDS, TRUE},
    {"feet", PART_FEET, TRUE},
    {"fingers", PART_FINGERS, TRUE},
    {"ear", PART_EAR, TRUE},
    {"eye", PART_EYE, TRUE},
    {"long_tongue", PART_LONG_TONGUE, TRUE},
    {"eyestalks", PART_EYESTALKS, TRUE},
    {"tentacles", PART_TENTACLES, TRUE},
    {"fins", PART_FINS, TRUE},
    {"wings", PART_WINGS, TRUE},
    {"tail", PART_TAIL, TRUE},
    {"claws", PART_CLAWS, TRUE},
    {"fangs", PART_FANGS, TRUE},
    {"horns", PART_HORNS, TRUE},
    {"scales", PART_SCALES, TRUE},
    {"tusks", PART_TUSKS, TRUE},
    {NULL, 0, 0}
};

const struct flag_type comm_flags[] = {
    {"quiet", COMM_QUIET, TRUE},
    {"deaf", COMM_DEAF, TRUE},
    {"nowiz", COMM_NOWIZ, TRUE},
    {"noclangossip", COMM_NOAUCTION, TRUE},
    {"nogossip", COMM_NOGOSSIP, TRUE},
    {"noquestion", COMM_NOQUESTION, TRUE},
    {"noclan", COMM_NOCLAN, TRUE},
    {"compact", COMM_COMPACT, TRUE},
    {"brief", COMM_BRIEF, TRUE},
    {"prompt", COMM_PROMPT, TRUE},
    {"combine", COMM_COMBINE, TRUE},
    {"telnet_ga", COMM_TELNET_GA, TRUE},
    {"show_affects", COMM_SHOW_AFFECTS, TRUE},
    {"nograts", COMM_NOGRATS, TRUE},
    {"noemote", COMM_NOEMOTE, FALSE},
    {"noshout", COMM_NOSHOUT, FALSE},
    {"notell", COMM_NOTELL, FALSE},
    {"nochannels", COMM_NOCHANNELS, FALSE},
    {"snoop_proof", COMM_SNOOP_PROOF, FALSE},
    {"afk", COMM_AFK, TRUE},
    {NULL, 0, 0}
};

const struct flag_type mprog_flags[] = {
    {"act", TRIG_ACT, TRUE},
    {"bribe", TRIG_BRIBE, TRUE},
    {"death", TRIG_DEATH, TRUE},
    {"entry", TRIG_ENTRY, TRUE},
    {"fight", TRIG_FIGHT, TRUE},
    {"give", TRIG_GIVE, TRUE},
    {"greet", TRIG_GREET, TRUE},
    {"grall", TRIG_GRALL, TRUE},
    {"kill", TRIG_KILL, TRUE},
    {"hpcnt", TRIG_HPCNT, TRUE},
    {"random", TRIG_RANDOM, TRUE},
    {"speech", TRIG_SPEECH, TRUE},
    {"exit", TRIG_EXIT, TRUE},
    {"exall", TRIG_EXALL, TRUE},
    {"delay", TRIG_DELAY, TRUE},
    {"surr", TRIG_SURR, TRUE},
    {NULL, 0, TRUE}
};

const struct flag_type area_flags[] = {
    {"none", AREA_NONE, FALSE},
    {"changed", AREA_CHANGED, TRUE},
    {"added", AREA_ADDED, TRUE},
    {"loading", AREA_LOADING, FALSE},
    {"norecall", AREA_NO_RECALL, TRUE},
    {"nosummon", AREA_NO_SUMMON, TRUE},
    {"nogate", AREA_NO_GATE, TRUE},
    {NULL, 0, 0}
};



const struct flag_type sex_flags[] = {
    {"male", SEX_MALE, TRUE},
    {"female", SEX_FEMALE, TRUE},
    {"neutral", SEX_NEUTRAL, TRUE},
    {"random", 3, TRUE},        /* ROM */
    {"none", SEX_NEUTRAL, TRUE},
    {NULL, 0, 0}
};



const struct flag_type exit_flags[] = {
    {"door", EX_ISDOOR, TRUE},
    {"closed", EX_CLOSED, TRUE},
    {"locked", EX_LOCKED, TRUE},
    {"pickproof", EX_PICKPROOF, TRUE},
    {"nopass", EX_NOPASS, TRUE},
    {"easy", EX_EASY, TRUE},
    {"hard", EX_HARD, TRUE},
    {"infuriating", EX_INFURIATING, TRUE},
    {"noclose", EX_NOCLOSE, TRUE},
    {"nolock", EX_NOLOCK, TRUE},
    {NULL, 0, 0}
};



const struct flag_type door_resets[] = {
    {"open and unlocked", 0, TRUE},
    {"closed and unlocked", 1, TRUE},
    {"closed and locked", 2, TRUE},
    {NULL, 0, 0}
};



const struct flag_type room_flags[] = {
    {"dark", ROOM_DARK, TRUE},
    {"no_mob", ROOM_NO_MOB, TRUE},
    {"indoors", ROOM_INDOORS, TRUE},
    {"private", ROOM_PRIVATE, TRUE},
    {"safe", ROOM_SAFE, TRUE},
    {"solitary", ROOM_SOLITARY, TRUE},
    {"pet_shop", ROOM_PET_SHOP, TRUE},
    {"no_recall", ROOM_NO_RECALL, TRUE},
    {"imp_only", ROOM_IMP_ONLY, TRUE},
    {"gods_only", ROOM_GODS_ONLY, TRUE},
    {"heroes_only", ROOM_HEROES_ONLY, TRUE},
    {"newbies_only", ROOM_NEWBIES_ONLY, TRUE},
    {"law", ROOM_LAW, TRUE},
    {"nowhere", ROOM_NOWHERE, TRUE},
    {"arena", ROOM_ARENA, TRUE}, /* Arena where a player can die and not lose items */
    {"no_gate", ROOM_NO_GATE, TRUE},
    {"guild", ROOM_GUILD, TRUE},
    {NULL, 0, 0}
};

const struct flag_type sector_flags[] = {
    {"inside", SECT_INSIDE, TRUE},
    {"city", SECT_CITY, TRUE},
    {"field", SECT_FIELD, TRUE},
    {"forest", SECT_FOREST, TRUE},
    {"hills", SECT_HILLS, TRUE},
    {"mountain", SECT_MOUNTAIN, TRUE},
    {"swim", SECT_WATER_SWIM, TRUE},
    {"noswim", SECT_WATER_NOSWIM, TRUE},
    {"unused", SECT_UNUSED, TRUE},
    {"air", SECT_AIR, TRUE},
    {"desert", SECT_DESERT, TRUE},
    {"ocean", SECT_OCEAN, TRUE},
    {"beach", SECT_BEACH, TRUE},
    {"underwater", SECT_UNDERWATER, TRUE},
    {NULL, 0, 0}
};

const struct flag_type type_flags[] = {
    {"light", ITEM_LIGHT, TRUE},
    {"scroll", ITEM_SCROLL, TRUE},
    {"wand", ITEM_WAND, TRUE},
    {"staff", ITEM_STAFF, TRUE},
    {"weapon", ITEM_WEAPON, TRUE},
    {"treasure", ITEM_TREASURE, TRUE},
    {"armor", ITEM_ARMOR, TRUE},
    {"potion", ITEM_POTION, TRUE},
    {"furniture", ITEM_FURNITURE, TRUE},
    {"trash", ITEM_TRASH, TRUE},
    {"container", ITEM_CONTAINER, TRUE},
    {"drinkcontainer", ITEM_DRINK_CON, TRUE},
    {"key", ITEM_KEY, TRUE},
    {"food", ITEM_FOOD, TRUE},
    {"money", ITEM_MONEY, TRUE},
    {"boat", ITEM_BOAT, TRUE},
    {"npccorpse", ITEM_CORPSE_NPC, TRUE},
    {"pc corpse", ITEM_CORPSE_PC, FALSE},
    {"fountain", ITEM_FOUNTAIN, TRUE},
    {"pill", ITEM_PILL, TRUE},
    {"protect", ITEM_PROTECT, TRUE},
    {"map", ITEM_MAP, TRUE},
    {"portal", ITEM_PORTAL, TRUE},
    {"warpstone", ITEM_WARP_STONE, TRUE},
    {"roomkey", ITEM_ROOM_KEY, TRUE},
    {"gem", ITEM_GEM, TRUE},
    {"jewelry", ITEM_JEWELRY, TRUE},
    {"shovel", ITEM_SHOVEL, TRUE},
    {"fog", ITEM_FOG, TRUE},
    {"parchment", ITEM_PARCHMENT, TRUE},
    {"seed", ITEM_SEED, TRUE},
    {NULL, 0, 0}
};


const struct flag_type extra_flags[] = {
    {"glow", ITEM_GLOW, TRUE},
    {"hum", ITEM_HUM, TRUE},
    {"dark", ITEM_DARK, TRUE},
    {"lock", ITEM_LOCK, TRUE},
    {"evil", ITEM_EVIL, TRUE},
    {"invis", ITEM_INVIS, TRUE},
    {"magic", ITEM_MAGIC, TRUE},
    {"nodrop", ITEM_NODROP, TRUE},
    {"bless", ITEM_BLESS, TRUE},
    {"antigood", ITEM_ANTI_GOOD, TRUE},
    {"antievil", ITEM_ANTI_EVIL, TRUE},
    {"antineutral", ITEM_ANTI_NEUTRAL, TRUE},
    {"noremove", ITEM_NOREMOVE, TRUE},
    {"inventory", ITEM_INVENTORY, TRUE},
    {"nopurge", ITEM_NOPURGE, TRUE},
    {"rotdeath", ITEM_ROT_DEATH, TRUE},
    {"visdeath", ITEM_VIS_DEATH, TRUE},
    {"nonmetal", ITEM_NONMETAL, TRUE},
    {"meltdrop", ITEM_MELT_DROP, TRUE},
    {"hadtimer", ITEM_HAD_TIMER, TRUE},
    {"sellextract", ITEM_SELL_EXTRACT, TRUE},
    {"burnproof", ITEM_BURN_PROOF, TRUE},
    {"nouncurse", ITEM_NOUNCURSE, TRUE},
    {"buried", ITEM_BURIED, TRUE},
    {NULL, 0, 0}
};



const struct flag_type wear_flags[] = {
    {"take", ITEM_TAKE, TRUE},
    {"finger", ITEM_WEAR_FINGER, TRUE},
    {"neck", ITEM_WEAR_NECK, TRUE},
    {"body", ITEM_WEAR_BODY, TRUE},
    {"head", ITEM_WEAR_HEAD, TRUE},
    {"legs", ITEM_WEAR_LEGS, TRUE},
    {"feet", ITEM_WEAR_FEET, TRUE},
    {"hands", ITEM_WEAR_HANDS, TRUE},
    {"arms", ITEM_WEAR_ARMS, TRUE},
    {"shield", ITEM_WEAR_SHIELD, TRUE},
    {"about", ITEM_WEAR_ABOUT, TRUE},
    {"waist", ITEM_WEAR_WAIST, TRUE},
    {"wrist", ITEM_WEAR_WRIST, TRUE},
    {"wield", ITEM_WIELD, TRUE},
    {"hold", ITEM_HOLD, TRUE},
    {"nosac", ITEM_NO_SAC, TRUE},
    {"wearfloat", ITEM_WEAR_FLOAT, TRUE},
/*    {   "twohands",            ITEM_TWO_HANDS,         TRUE    }, */
    {NULL, 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] = {
    {"none", APPLY_NONE, TRUE},
    {"strength", APPLY_STR, TRUE},
    {"dexterity", APPLY_DEX, TRUE},
    {"intelligence", APPLY_INT, TRUE},
    {"wisdom", APPLY_WIS, TRUE},
    {"constitution", APPLY_CON, TRUE},
    {"sex", APPLY_SEX, TRUE},
    {"class", APPLY_CLASS, TRUE},
    {"level", APPLY_LEVEL, TRUE},
    {"age", APPLY_AGE, TRUE},
    {"height", APPLY_HEIGHT, TRUE},
    {"weight", APPLY_WEIGHT, TRUE},
    {"mana", APPLY_MANA, TRUE},
    {"hp", APPLY_HIT, TRUE},
    {"move", APPLY_MOVE, TRUE},
    {"gold", APPLY_GOLD, TRUE},
    {"experience", APPLY_EXP, TRUE},
    {"ac", APPLY_AC, TRUE},
    {"hitroll", APPLY_HITROLL, TRUE},
    {"damroll", APPLY_DAMROLL, TRUE},
    {"saves", APPLY_SAVES, TRUE},
    {"spellaffect", APPLY_SPELL_AFFECT, FALSE},
    {NULL, 0, 0}
};



/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] = {
    {"in the inventory", WEAR_NONE, TRUE},
    {"as a light", WEAR_LIGHT, TRUE},
    {"on the left finger", WEAR_FINGER_L, TRUE},
    {"on the right finger", WEAR_FINGER_R, TRUE},
    {"around the neck (1)", WEAR_NECK_1, TRUE},
    {"around the neck (2)", WEAR_NECK_2, TRUE},
    {"on the body", WEAR_BODY, TRUE},
    {"over the head", WEAR_HEAD, TRUE},
    {"on the legs", WEAR_LEGS, TRUE},
    {"on the feet", WEAR_FEET, TRUE},
    {"on the hands", WEAR_HANDS, TRUE},
    {"on the arms", WEAR_ARMS, TRUE},
    {"as a shield", WEAR_SHIELD, TRUE},
    {"about the shoulders", WEAR_ABOUT, TRUE},
    {"around the waist", WEAR_WAIST, TRUE},
    {"on the left wrist", WEAR_WRIST_L, TRUE},
    {"on the right wrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"held in the hands", WEAR_HOLD, TRUE},
    {"floating nearby", WEAR_FLOAT, TRUE},
    {NULL, 0, 0}
};


const struct flag_type wear_loc_flags[] = {
    {"none", WEAR_NONE, TRUE},
    {"light", WEAR_LIGHT, TRUE},
    {"lfinger", WEAR_FINGER_L, TRUE},
    {"rfinger", WEAR_FINGER_R, TRUE},
    {"neck1", WEAR_NECK_1, TRUE},
    {"neck2", WEAR_NECK_2, TRUE},
    {"body", WEAR_BODY, TRUE},
    {"head", WEAR_HEAD, TRUE},
    {"legs", WEAR_LEGS, TRUE},
    {"feet", WEAR_FEET, TRUE},
    {"hands", WEAR_HANDS, TRUE},
    {"arms", WEAR_ARMS, TRUE},
    {"shield", WEAR_SHIELD, TRUE},
    {"about", WEAR_ABOUT, TRUE},
    {"waist", WEAR_WAIST, TRUE},
    {"lwrist", WEAR_WRIST_L, TRUE},
    {"rwrist", WEAR_WRIST_R, TRUE},
    {"wielded", WEAR_WIELD, TRUE},
    {"hold", WEAR_HOLD, TRUE},
    {"floating", WEAR_FLOAT, TRUE},
    {NULL, 0, 0}
};

const struct flag_type container_flags[] = {
    {"closeable", 1, TRUE},
    {"pickproof", 2, TRUE},
    {"closed", 4, TRUE},
    {"locked", 8, TRUE},
    {"puton", 16, TRUE},
    {NULL, 0, 0}
};

/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/




const struct flag_type ac_type[] = {
    {"pierce", AC_PIERCE, TRUE},
    {"bash", AC_BASH, TRUE},
    {"slash", AC_SLASH, TRUE},
    {"exotic", AC_EXOTIC, TRUE},
    {NULL, 0, 0}
};


const struct flag_type size_flags[] = {
    {"tiny", SIZE_TINY, TRUE},
    {"small", SIZE_SMALL, TRUE},
    {"medium", SIZE_MEDIUM, TRUE},
    {"large", SIZE_LARGE, TRUE},
    {"huge", SIZE_HUGE, TRUE},
    {"giant", SIZE_GIANT, TRUE},
    {NULL, 0, 0},
};


const struct flag_type weapon_class[] = {
    {"exotic", WEAPON_EXOTIC, TRUE},
    {"sword", WEAPON_SWORD, TRUE},
    {"dagger", WEAPON_DAGGER, TRUE},
    {"spear", WEAPON_SPEAR, TRUE},
    {"mace", WEAPON_MACE, TRUE},
    {"axe", WEAPON_AXE, TRUE},
    {"flail", WEAPON_FLAIL, TRUE},
    {"whip", WEAPON_WHIP, TRUE},
    {"polearm", WEAPON_POLEARM, TRUE},
    {NULL, 0, 0}
};


const struct flag_type weapon_type2[] = {
    {"flaming", WEAPON_FLAMING, TRUE},
    {"frost", WEAPON_FROST, TRUE},
    {"vampiric", WEAPON_VAMPIRIC, TRUE},
    {"sharp", WEAPON_SHARP, TRUE},
    {"vorpal", WEAPON_VORPAL, TRUE},
    {"twohands", WEAPON_TWO_HANDS, TRUE},
    {"shocking", WEAPON_SHOCKING, TRUE},
    {"poison", WEAPON_POISON, TRUE},
    {"leech", WEAPON_LEECH, TRUE},
    {"stun", WEAPON_STUN, TRUE},
    {NULL, 0, 0}
};

const struct flag_type res_flags[] = {
    {"summon", RES_SUMMON, TRUE},
    {"charm", RES_CHARM, TRUE},
    {"magic", RES_MAGIC, TRUE},
    {"weapon", RES_WEAPON, TRUE},
    {"bash", RES_BASH, TRUE},
    {"pierce", RES_PIERCE, TRUE},
    {"slash", RES_SLASH, TRUE},
    {"fire", RES_FIRE, TRUE},
    {"cold", RES_COLD, TRUE},
    {"lightning", RES_LIGHTNING, TRUE},
    {"acid", RES_ACID, TRUE},
    {"poison", RES_POISON, TRUE},
    {"negative", RES_NEGATIVE, TRUE},
    {"holy", RES_HOLY, TRUE},
    {"energy", RES_ENERGY, TRUE},
    {"mental", RES_MENTAL, TRUE},
    {"disease", RES_DISEASE, TRUE},
    {"drowning", RES_DROWNING, TRUE},
    {"light", RES_LIGHT, TRUE},
    {"sound", RES_SOUND, TRUE},
    {"wood", RES_WOOD, TRUE},
    {"silver", RES_SILVER, TRUE},
    {"iron", RES_IRON, TRUE},
    {NULL, 0, 0}
};


const struct flag_type vuln_flags[] = {
    {"summon", VULN_SUMMON, TRUE},
    {"charm", VULN_CHARM, TRUE},
    {"magic", VULN_MAGIC, TRUE},
    {"weapon", VULN_WEAPON, TRUE},
    {"bash", VULN_BASH, TRUE},
    {"pierce", VULN_PIERCE, TRUE},
    {"slash", VULN_SLASH, TRUE},
    {"fire", VULN_FIRE, TRUE},
    {"cold", VULN_COLD, TRUE},
    {"lightning", VULN_LIGHTNING, TRUE},
    {"acid", VULN_ACID, TRUE},
    {"poison", VULN_POISON, TRUE},
    {"negative", VULN_NEGATIVE, TRUE},
    {"holy", VULN_HOLY, TRUE},
    {"energy", VULN_ENERGY, TRUE},
    {"mental", VULN_MENTAL, TRUE},
    {"disease", VULN_DISEASE, TRUE},
    {"drowning", VULN_DROWNING, TRUE},
    {"light", VULN_LIGHT, TRUE},
    {"sound", VULN_SOUND, TRUE},
    {"wood", VULN_WOOD, TRUE},
    {"silver", VULN_SILVER, TRUE},
    {"iron", VULN_IRON, TRUE},
    {NULL, 0, 0}
};

const struct flag_type position_flags[] = {
    {"dead", POS_DEAD, FALSE},
    {"mortal", POS_MORTAL, FALSE},
    {"incap", POS_INCAP, FALSE},
    {"stunned", POS_STUNNED, FALSE},
    {"sleeping", POS_SLEEPING, TRUE},
    {"resting", POS_RESTING, TRUE},
    {"sitting", POS_SITTING, TRUE},
    {"fighting", POS_FIGHTING, TRUE},
    {"standing", POS_STANDING, TRUE},
    {NULL, 0, 0}
};

const struct flag_type portal_flags[] = {
    {"normal_exit", GATE_NORMAL_EXIT, TRUE},
    {"no_curse", GATE_NOCURSE, TRUE},
    {"go_with", GATE_GOWITH, TRUE},
    {"buggy", GATE_BUGGY, TRUE},
    {"random", GATE_RANDOM, TRUE},
    {NULL, 0, 0}
};

const struct flag_type furniture_flags[] = {
    {"stand_at", STAND_AT, TRUE},
    {"stand_on", STAND_ON, TRUE},
    {"stand_in", STAND_IN, TRUE},
    {"sit_at", SIT_AT, TRUE},
    {"sit_on", SIT_ON, TRUE},
    {"sit_in", SIT_IN, TRUE},
    {"rest_at", REST_AT, TRUE},
    {"rest_on", REST_ON, TRUE},
    {"rest_in", REST_IN, TRUE},
    {"sleep_at", SLEEP_AT, TRUE},
    {"sleep_on", SLEEP_ON, TRUE},
    {"sleep_in", SLEEP_IN, TRUE},
    {"put_at", PUT_AT, TRUE},
    {"put_on", PUT_ON, TRUE},
    {"put_in", PUT_IN, TRUE},
    {"put_inside", PUT_INSIDE, TRUE},
    {NULL, 0, 0}
};

const struct flag_type apply_types[] = {
    {"affects", TO_AFFECTS, TRUE},
    {"object", TO_OBJECT, TRUE},
    {"immune", TO_IMMUNE, TRUE},
    {"resist", TO_RESIST, TRUE},
    {"vuln", TO_VULN, TRUE},
    {"weapon", TO_WEAPON, TRUE},
    {NULL, 0, TRUE}
};

const struct bit_type bitvector_type[] = {
    {affect_flags, "affect"},
    {apply_flags, "apply"},
    {imm_flags, "imm"},
    {res_flags, "res"},
    {vuln_flags, "vuln"},
    {weapon_type2, "weapon"}
};

/*
 * This is a list of the continents currently used in the game.  Each area
 * is associated with a continent.
 */
const struct continent_type continent_table[] = {
    { CONTINENT_LIMBO,    "limbo"    },
    { CONTINENT_MIDGAARD, "midgaard" },
    { CONTINENT_ARCANIS,  "arcanis"  },
    { CONTINENT_OCEANS,   "oceans"   },
    { 0, NULL }
};

/*
 * This is the portal shop type that defines where portal shops are allowed
 * to send players and how much it costs.  Each shop will manipulate the price
 * depending on where the player is and where they are going (e.g. if you're
 * sending a player across continents the price may be doubled, etc.).  As the
 * world grows I'd make these more general types of places that will get players
 * closer to where they want to go.
 */
const struct portal_shop_type portal_shop_table[] = {
    { "midgaard",     3014, 2000 },
    { "newthalos",    9506, 2000 },
    { "arcanisport", 11232, 2000 },
    { "catacombs",    3458, 3000 },
    { "cave",         3675, 3000 },
    { "mahn-tor",     2302, 3000 },
    { "plains",        324, 2500 },
    { NULL,              0,    0 }
};

/*
 * The gods or goddesses in which a player may follow.
 */
const struct deity_type deity_table[] = {
    { "Athiest", "No belief", ALIGN_ALL, TRUE },
    { "Aurias", "God of Light", ALIGN_GOOD, TRUE },
    { "Kiyria", "Goddess of Balance", ALIGN_NEUTRAL, TRUE },
    { "Drakylar", "God of Darkness", ALIGN_EVIL, TRUE },
    { "Caia", "Goddess of Nature", ALIGN_ALL, TRUE },
    { "Raije", "God of War", ALIGN_ALL, TRUE },
    { "Pathas", "God of Magicks", ALIGN_ALL, TRUE },
    { NULL, NULL, 0 }
};

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
    {ITEM_PARCHMENT,    "parchment"},
    {ITEM_SEED,         "seed"},
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
    {"secure",    WIZ_SECURE,    L2},
    {"general",   WIZ_GENERAL,   IM},
    {"bank",      WIZ_BANK,      L6},
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
     "half elf", TRUE,
     0, AFF_INFRARED, 0,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},
    {
     "wild elf", TRUE,
     0, AFF_INFRARED, VULN_IRON,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K },

    {
     "sea elf", TRUE,
     0, AFF_INFRARED, VULN_IRON,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K },

    {
     "dark elf", TRUE,
     0, AFF_INFRARED, VULN_IRON,
     0, 0, 0,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K },
    {
     "dwarf", TRUE,
     0, AFF_INFRARED, 0,
     0, RES_POISON | RES_DISEASE, VULN_DROWNING,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},
    {
     "hill dwarf", TRUE,
     0, AFF_INFRARED, 0,
     0, RES_POISON | RES_DISEASE, VULN_DROWNING,
     A | H | M | V, A | B | C | D | E | F | G | H | I | J | K},
    {
     "mountain dwarf", TRUE,
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
     {"sneak", "hide", "swim", "meditation"},
     {12, 14, 13, 15, 11}, {16, 20, 18, 21, 16}, SIZE_SMALL},

    {
     // 65 starting stat line, 90 max stat line
     "half elf", "H-Elf", "a half elf", 5,{ 100, 125, 100, 120, 100 },
     { "sneak", "hide", "swim"},
     { 12, 14, 13, 15, 11 },{ 17, 19, 18, 19, 17 }, SIZE_SMALL },

    {
     // 65 starting stat line, 90 max stat line
     "wild elf", "W-Elf", "a wild elf", 5,{ 100, 125, 100, 120, 100 },
     { "sneak", "hide", "swim", "fast healing" },
     { 12, 14, 13, 15, 11 },{ 17, 17, 17, 22, 17 }, SIZE_SMALL },

     {
      // 64 starting stat line, 89 max stat line
      "sea elf", "Sea-E", "a sea elf", 5,{ 100, 125, 100, 120, 100 },
      { "swim" },
      { 12, 13, 13, 15, 11 },{ 16, 18, 18, 21, 16 }, SIZE_SMALL },
     {
      // 65 starting stat line, 91 max stat line
      "dark elf", "D-Elf", "a dark elf", 5,{ 100, 125, 100, 120, 100 },
      { "sneak", "hide", "swim", "meditation" },
      { 12, 14, 13, 15, 11 },{ 16, 20, 18, 21, 16 }, SIZE_SMALL },

    {
     // 65 starting stat line, 89 max stat line
     "dwarf", "Dwarf", "a dwarf", 8, {150, 100, 125, 100, 150},
     {"berserk"},
     {14, 12, 14, 10, 15}, {19, 15, 21, 14, 20}, SIZE_MEDIUM},

    {
     // 65 starting stat line, 89 max stat line
     "hill dwarf", "H-Dwf", "a hill dwarf", 8, {150, 100, 125, 100, 150},
     {"berserk"},
     {14, 12, 14, 10, 15}, {18, 15, 21, 15, 20}, SIZE_MEDIUM},

    {
     // 65 starting stat line, 89 max stat line
     "mountain dwarf", "Dwarf", "a mountain dwarf", 8, {150, 100, 125, 100, 150},
     {"berserk"},
     {14, 12, 14, 10, 15}, {20, 15, 21, 13, 20}, SIZE_MEDIUM},

    {
     // 65 starting stat line, 90 max stat line
     "ogre", "Ogre", "an ogre", 6, {200, 150, 150, 105, 200},
     {"bash", "fast healing"},
     {16, 11, 13, 11, 14}, {22, 15, 17, 15, 21}, SIZE_LARGE},

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
 * Field 1: todam - Dam roll bonus
 * Field 2: carry - Carry modifier
 * Field 3: wield - Wield modifier (factors into dropping weapon when weak)
 */
const struct str_app_type str_app[26] = {
    {-4, 0, 0},             /* 0  */
    {-4, 3, 1},             /* 1  */
    {-2, 3, 2},
    {-1, 10, 3},            /* 3  */
    {-1, 25, 4},
    {-1, 55, 5},            /* 5  */
    { 0, 80, 6},
    { 0, 90, 7},
    { 0, 100, 8},
    { 0, 100, 9},
    { 0, 115, 10},          /* 10  */
    { 0, 115, 11},
    { 0, 130, 12},
    { 0, 130, 13},          /* 13  */
    { 1, 140, 14},
    { 1, 150, 15},          /* 15  */
    { 2, 165, 16},
    { 3, 180, 22},
    { 3, 200, 25},          /* 18  */
    { 4, 225, 30},
    { 5, 250, 35},          /* 20  */
    { 6, 300, 40},
    { 6, 350, 45},
    { 7, 400, 50},
    { 8, 450, 55},
    { 9, 500, 60}           /* 25   */
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
 * Field 1:  defensive - (AC Bonus)
 * Field 2:  hitroll_bonus (Hit roll bonus)
 */
const struct dex_app_type dex_app[26] = {
    {60,  -5},           /* 0 */
    {50,  -5},           /* 1 */
    {50,  -3},
    {40,  -3},
    {30,  -2},
    {20,  -2},           /* 5 */
    {10,  -1},
    { 0,  -1},
    { 0,   0},
    { 0,   0},
    { 0,   0},           /* 10 */
    { 0,   0},
    { 0,   0},
    { 0,   0},
    { 0,   0},
    {-10,  1},           /* 15 */
    {-15,  1},
    {-20,  2},
    {-30,  2},
    {-40,  3},
    {-50,  3},           /* 20 */
    {-60,  4},
    {-75,  4},
    {-90,  5},
    {-105, 5},
    {-120, 6}            /* 25 */
};


/*
 * Constitution modifiers.
 * Field 1: HP bonus on level
 */
const struct con_app_type con_app[26] = {
    {-4},   /*  0 */
    {-3},   /*  1 */
    {-2},   /*  2 */
    {-2},   /*  3 */
    {-1},   /*  4 */
    {-1},   /*  5 */
    {-1},   /*  6 */
    {0},    /*  7 */
    {0},    /*  8 */
    {0},    /*  9 */
    {0},    /* 10 */
    {0},    /* 11 */
    {0},    /* 12 */
    {0},    /* 13 */
    {0},    /* 14 */
    {1},    /* 15 */
    {2},    /* 16 */
    {2},    /* 17 */
    {3},    /* 18 */
    {3},    /* 19 */
    {4},    /* 20 */
    {4},    /* 21 */
    {5},    /* 22 */
    {6},    /* 23 */
    {7},    /* 24 */
    {8}     /* 25 */
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
