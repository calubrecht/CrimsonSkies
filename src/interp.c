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
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell (Rhien)   *
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"

bool check_social args ((CHAR_DATA * ch, char *command, char *argument));

/*
 * Command logging types.
 */
#define LOG_NORMAL  0
#define LOG_ALWAYS  1
#define LOG_NEVER   2

/*
 * Log-all switch.
 */
bool fLogAll = FALSE;

/*
 * Command table.
 */
const struct cmd_type cmd_table[] = {
    /*
     * Common movement commands.
     */
    { "north",     do_north,     POS_STANDING, 0, LOG_NEVER, FALSE},
    { "east",      do_east,      POS_STANDING, 0, LOG_NEVER, FALSE},
    { "south",     do_south,     POS_STANDING, 0, LOG_NEVER, FALSE},
    { "west",      do_west,      POS_STANDING, 0, LOG_NEVER, FALSE},
    { "up",        do_up,        POS_STANDING, 0, LOG_NEVER, FALSE},
    { "down",      do_down,      POS_STANDING, 0, LOG_NEVER, FALSE},
    { "northeast", do_northeast, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "ne",        do_northeast, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "northwest", do_northwest, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "nw",        do_northwest, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "southeast", do_southeast, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "se",        do_southeast, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "southwest", do_southwest, POS_STANDING, 0, LOG_NEVER, FALSE},
    { "sw",        do_southwest, POS_STANDING, 0, LOG_NEVER, FALSE},

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    {"at",        do_at,        POS_DEAD,     L6, LOG_NORMAL, TRUE},
    {"cast",      do_cast,      POS_FIGHTING, 0,  LOG_NORMAL, TRUE},
    {"auction",   do_auction,   POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"buy",       do_buy,       POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"channels",  do_channels,  POS_DEAD,     0,  LOG_NORMAL, TRUE},
    {"exits",     do_exits,     POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"get",       do_get,       POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"goto",      do_goto,      POS_DEAD,     L8, LOG_NORMAL, TRUE},
    {"group",     do_group,     POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"guild",     do_guild,     POS_DEAD,     L7, LOG_ALWAYS, TRUE},
    {"hit",       do_kill,      POS_FIGHTING, 0,  LOG_NORMAL, FALSE},
    {"inventory", do_inventory, POS_DEAD,     0,  LOG_NORMAL, TRUE},
    {"kill",      do_kill,      POS_FIGHTING, 0,  LOG_NORMAL, TRUE},
    {"look",      do_look,      POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"clan",      do_clantalk,  POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"order",     do_order,     POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"practice",  do_practice,  POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"rest",      do_rest,      POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"scan",      do_scan,      POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"sit",       do_sit,       POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"sockets",   do_sockets,   POS_DEAD,     L7, LOG_NORMAL, TRUE},
    {"stand",     do_stand,     POS_SLEEPING, 0,  LOG_NORMAL, TRUE},
    {"tell",      do_tell,      POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"unlock",    do_unlock,    POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"wield",     do_wear,      POS_RESTING,  0,  LOG_NORMAL, TRUE},
    {"wizhelp",   do_wizhelp,   POS_DEAD,     IM, LOG_NORMAL, TRUE},
    {"clear",     do_clear,     POS_DEAD,     0,  LOG_NORMAL, TRUE},
    {"second",    do_second,    POS_RESTING,  0,  LOG_NORMAL, FALSE},
    {"dual",      do_second,    POS_RESTING,  0,  LOG_NORMAL, TRUE},

    /*
     * Informational commands.
     */
    {"affects",   do_affects,   POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"areas",     do_areas,     POS_DEAD,     0, LOG_NORMAL, TRUE},
/*  {"bug",       do_bug,       POS_DEAD,     0, LOG_NORMAL, 1}, */
    {"commands",  do_commands,  POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"compare",   do_compare,   POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"consider",  do_consider,  POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"count",     do_count,     POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"credits",   do_credits,   POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"equipment", do_equipment, POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"examine",   do_examine,   POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"touch",     do_touch,     POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"help",      do_help,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"info",      do_groups,    POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"motd",      do_motd,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"read",      do_read,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"report",    do_report,    POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"rules",     do_rules,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"score",     do_score,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"oldscore",  do_oldscore,  POS_DEAD,     0, LOG_NORMAL, FALSE},
    {"skills",    do_skills,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"socials",   do_socials,   POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"show",      do_show,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"spells",    do_spells,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"time",      do_time,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"typo",      do_typo,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"weather",   do_weather,   POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"who",       do_who,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"whois",     do_whois,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"whoami",    do_whoami,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"wizlist",   do_wizlist,   POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"worth",     do_worth,     POS_SLEEPING, 0, LOG_NORMAL, TRUE},

    /*
     * Configuration commands.
     */
    {"alia",      do_alia,        POS_DEAD,     0, LOG_NORMAL, FALSE},
    {"alias",     do_alias,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autolist",  do_autolist,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autoall",   do_autoall,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autoassist",do_autoassist,  POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autoexit",  do_autoexit,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autogold",  do_autogold,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autoloot",  do_autoloot,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autosac",   do_autosac,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"autosplit", do_autosplit,   POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"brief",     do_brief,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"color",     do_color,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"combine",   do_combine,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"compact",   do_compact,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"description", do_description,POS_DEAD,    0, LOG_NORMAL, TRUE},
    {"delet",     do_delet,       POS_DEAD,     0, LOG_ALWAYS, FALSE},
    {"delete",    do_delete,      POS_STANDING, 0, LOG_ALWAYS, TRUE},
    {"nofollow",  do_nofollow,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"noloot",    do_noloot,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"nosummon",  do_nosummon,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"outfit",    do_outfit,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"password",  do_password,    POS_DEAD,     0, LOG_NEVER,  TRUE},
    {"prompt",    do_prompt,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"scroll",    do_scroll,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"telnetga",  do_telnetga,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"title",     do_title,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"unalias",   do_unalias,     POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"wimpy",     do_wimpy,       POS_DEAD,     0, LOG_NORMAL, TRUE},

    /*
     * Communication commands.
     */
    {"afk",             do_afk,         POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"answer",          do_answer,      POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"deaf",            do_deaf,        POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"emote",           do_emote,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"pmote",           do_pmote,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"gossip",          do_gossip,      POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"cgossip",         do_cgossip,     POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"ooc",             do_ooc,         POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"grats",           do_grats,       POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"gtell",           do_gtell,       POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"question",        do_question,    POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"quiet",           do_quiet,       POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"reply",           do_reply,       POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"replay",          do_replay,      POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"say",             do_say,         POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"shout",           do_yell,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"yell",            do_yell,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"pray",            do_pray,        POS_DEAD,     0, LOG_ALWAYS, TRUE},
    {"immtalk",         do_immtalk,     POS_DEAD,    IM, LOG_NORMAL, TRUE},
    {"direct",          do_direct,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {">",               do_direct,      POS_RESTING,  0, LOG_NORMAL, FALSE},

    /*
     * Note commands.
     */
    {"oocn",            do_ooc_spool,    POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"note",            do_note,         POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"news",            do_news,         POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"changes",         do_changes,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"storynote",       do_story_spool,  POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"history",         do_history_spool,POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"penalty",         do_penalty,      POS_DEAD,    IM, LOG_NORMAL, TRUE},
    {"immnote",         do_imm_spool,    POS_DEAD,    IM, LOG_NORMAL, TRUE},
    {"catchup",         do_catchup,      POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"unread",          do_unread,       POS_DEAD,     0, LOG_NORMAL, TRUE},

    /*
     * Object manipulation commands.
     */
    {"brandish",        do_brandish,    POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"close",           do_close,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"drink",           do_drink,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"drop",            do_drop,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"eat",             do_eat,         POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"envenom",         do_envenom,     POS_RESTING,  0, LOG_NORMAL, FALSE},
    {"fill",            do_fill,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"give",            do_give,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"heal",            do_heal,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"hold",            do_wear,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"list",            do_list,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"lock",            do_lock,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"open",            do_open,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"pick",            do_pick,        POS_RESTING,  0, LOG_NORMAL, FALSE},
    {"pour",            do_pour,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"put",             do_put,         POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"quaff",           do_quaff,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"recite",          do_recite,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"remove",          do_remove,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"sell",            do_sell,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"take",            do_get,         POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"sacrifice",       do_sacrifice,   POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"junk",            do_sacrifice,   POS_RESTING,  0, LOG_NORMAL, FALSE},
    {"value",           do_value,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"wear",            do_wear,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"zap",             do_zap,         POS_RESTING,  0, LOG_NORMAL, TRUE},

    /*
     * Combat commands.
     */
    {"backstab",        do_backstab,    POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"bash",            do_bash,        POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"bs",              do_backstab,    POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"berserk",         do_berserk,     POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"dirt",            do_dirt,        POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"disarm",          do_disarm,      POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"flee",            do_flee,        POS_FIGHTING, 0, LOG_NORMAL, TRUE},
    {"kick",            do_kick,        POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"murde",           do_murde,       POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"murder",          do_murder,      POS_FIGHTING, 5, LOG_ALWAYS, TRUE},
    {"rescue",          do_rescue,      POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"surrender",       do_surrender,   POS_FIGHTING, 0, LOG_NORMAL, TRUE},
    {"trip",            do_trip,        POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"bladesong",       do_bladesong,   POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"circle",          do_circle,      POS_FIGHTING, 0, LOG_NORMAL, FALSE},
    {"circlestab",      do_circlestab,  POS_FIGHTING, 0, LOG_NORMAL, FALSE},

    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    {"mob",             do_mob,         POS_DEAD,     0, LOG_NEVER, FALSE},

    /*
     * Miscellaneous commands.
     */
    {"enter",           do_enter,       POS_STANDING, 0, LOG_NORMAL, TRUE},
    {"follow",          do_follow,      POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"gain",            do_gain,        POS_STANDING, 0, LOG_NORMAL, TRUE},
    {"go",              do_enter,       POS_STANDING, 0, LOG_NORMAL, FALSE},
    {"groups",          do_groups,      POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"hide",            do_hide,        POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"qui",             do_qui,         POS_DEAD,     0, LOG_NORMAL, FALSE},
    {"quit",            do_quit,        POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"recall",          do_recall,      POS_FIGHTING, 0, LOG_NORMAL, TRUE},
    {"save",            do_save,        POS_DEAD,     0, LOG_NORMAL, TRUE},
    {"sleep",           do_sleep,       POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"sneak",           do_sneak,       POS_STANDING, 0, LOG_NORMAL, FALSE},
    {"split",           do_split,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"steal",           do_steal,       POS_STANDING, 0, LOG_NORMAL, FALSE},
    {"train",           do_train,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"visible",         do_visible,     POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"wake",            do_wake,        POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"where",           do_where,       POS_RESTING,  0, LOG_NORMAL, TRUE},
    {"reclass",         do_reclass,     POS_SLEEPING, 0, LOG_ALWAYS, TRUE},
    {"guildlist",       do_guildlist,   POS_SLEEPING, 0, LOG_NORMAL, TRUE},
    {"land",            do_land,        POS_STANDING, 0, LOG_NORMAL, TRUE},
    {"class",           do_class,       POS_SLEEPING, 0, LOG_NORMAL, TRUE},

    /*
     * Immortal commands.
     */
    {"advance",         do_advance,     POS_DEAD, ML, LOG_ALWAYS, TRUE},
    {"copyover",        do_copyover,    POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"dump",            do_dump,        POS_DEAD, ML, LOG_ALWAYS, FALSE},
    {"trust",           do_trust,       POS_DEAD, ML, LOG_ALWAYS, TRUE},
    {"violate",         do_violate,     POS_DEAD, ML, LOG_ALWAYS, TRUE},
    {"allow",           do_allow,       POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"ban",             do_ban,         POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"deny",            do_deny,        POS_DEAD, L1, LOG_ALWAYS, TRUE},
    {"disconnect",      do_disconnect,  POS_DEAD, L3, LOG_ALWAYS, TRUE},
    {"flag",            do_flag,        POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"freeze",          do_freeze,      POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"permban",         do_permban,     POS_DEAD, L1, LOG_ALWAYS, TRUE},
    {"protect",         do_protect,     POS_DEAD, L1, LOG_ALWAYS, TRUE},
    {"reboo",           do_reboo,       POS_DEAD, L1, LOG_NORMAL, FALSE},
    {"reboot",          do_reboot,      POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"set",             do_set,         POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"shutdow",         do_shutdow,     POS_DEAD, L1, LOG_NORMAL, FALSE},
    {"shutdown",        do_shutdown,    POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"wizlock",         do_wizlock,     POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"force",           do_force,       POS_DEAD, L7, LOG_ALWAYS, TRUE},
    {"load",            do_load,        POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"newlock",         do_newlock,     POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"nochannels",      do_nochannels,  POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"noemote",         do_noemote,     POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"noshout",         do_noshout,     POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"notell",          do_notell,      POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"pecho",           do_pecho,       POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"pardon",          do_pardon,      POS_DEAD, L3, LOG_ALWAYS, TRUE},
    {"purge",           do_purge,       POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"restore",         do_restore,     POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"sla",             do_sla,         POS_DEAD, L3, LOG_NORMAL, FALSE},
    {"slay",            do_slay,        POS_DEAD, L3, LOG_ALWAYS, TRUE},
    {"teleport",        do_transfer,    POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"transfer",        do_transfer,    POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"poofin",          do_bamfin,      POS_DEAD, L8, LOG_NORMAL, TRUE},
    {"poofout",         do_bamfout,     POS_DEAD, L8, LOG_NORMAL, TRUE},
    {"gecho",           do_echo,        POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"cecho",           do_cecho,       POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"broadcast",       do_broadcast,   POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"holylight",       do_holylight,   POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"incognito",       do_incognito,   POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"invis",           do_invis,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"log",             do_log,         POS_DEAD, L2, LOG_ALWAYS, TRUE},
    {"logout",          do_quit,        POS_DEAD,  0, LOG_NORMAL, TRUE},
    {"memory",          do_memory,      POS_DEAD, L3, LOG_NORMAL, TRUE},
    {"mwhere",          do_mwhere,      POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"owhere",          do_owhere,      POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"peace",           do_peace,       POS_DEAD, L5, LOG_NORMAL, TRUE},
    {"echo",            do_recho,       POS_DEAD, L6, LOG_ALWAYS, TRUE},
    {"return",          do_return,      POS_DEAD, L6, LOG_NORMAL, TRUE},
    {"snoop",           do_snoop,       POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"stat",            do_stat,        POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"stats",           do_stats,       POS_DEAD,  0, LOG_NORMAL, TRUE},
    {"string",          do_string,      POS_DEAD, L7, LOG_ALWAYS, TRUE},
    {"switch",          do_switch,      POS_DEAD, L6, LOG_ALWAYS, TRUE},
    {"wizinvis",        do_invis,       POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"vnum",            do_vnum,        POS_DEAD, L7, LOG_NORMAL, TRUE},
    {"zecho",           do_zecho,       POS_DEAD, L4, LOG_ALWAYS, TRUE},
    {"clone",           do_clone,       POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"wiznet",          do_wiznet,      POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"imotd",           do_imotd,       POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"smote",           do_smote,       POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"prefi",           do_prefi,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"prefix",          do_prefix,      POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"mpdump",          do_mpdump,      POS_DEAD, IM, LOG_NEVER,  TRUE},
    {"mpstat",          do_mpstat,      POS_DEAD, IM, LOG_NEVER,  TRUE},
    {"debug",           do_debug,       POS_DEAD, 1,  LOG_NEVER,  FALSE},
    {"forcetick",       do_forcetick,   POS_DEAD, ML, LOG_ALWAYS, TRUE},
    {"rename",          do_rename,      POS_DEAD, L6, LOG_ALWAYS, TRUE},
    {"vnumgap",         do_vnumgap,     POS_DEAD, L1, LOG_NEVER,  TRUE},
    {"exlist",          do_exlist,      POS_DEAD, L7, LOG_NEVER,  TRUE},
    {"pathfind",        do_pathfind,    POS_DEAD, L3, LOG_ALWAYS, TRUE},
    {"test",            do_test,        POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"wizcancel",       do_wizcancel,   POS_DEAD, L5, LOG_ALWAYS, TRUE},
    {"wizbless",        do_wizbless,    POS_DEAD, IM, LOG_ALWAYS, TRUE},

    /*
     * OLC
     */
    {"edit",            do_olc,         POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"asave",           do_asave,       POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"alist",           do_alist,       POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"resets",          do_resets,      POS_DEAD, IM, LOG_NORMAL, TRUE},
    {"redit",           do_redit,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"medit",           do_medit,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"aedit",           do_aedit,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"oedit",           do_oedit,       POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"mpedit",          do_mpedit,      POS_DEAD, IM, LOG_NORMAL, FALSE},
    {"hedit",           do_hedit,       POS_DEAD, IM, LOG_NORMAL, FALSE},

    /*
     * End of list.
     */
    {"", 0, POS_DEAD, 0, LOG_NORMAL, FALSE}
};


/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret (CHAR_DATA * ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    /*
     * Strip leading spaces.
     */
    while (isspace (*argument))
        argument++;
    if (argument[0] == '\0')
        return;

    /*
     * No hiding.
     */
    REMOVE_BIT (ch->affected_by, AFF_HIDE);

    /*
     * Implement freeze command.
     */
    if (!IS_NPC (ch) && IS_SET (ch->act, PLR_FREEZE))
    {
        send_to_char ("You're totally frozen!\n\r", ch);
        return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     * also no spaces needed after punctuation.
     */
    strcpy (logline, argument);
    if (!isalpha (argument[0]) && !isdigit (argument[0]))
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while (isspace (*argument))
            argument++;
    }
    else
    {
        argument = one_argument (argument, command);
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust (ch);
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (command[0] == cmd_table[cmd].name[0]
            && !str_prefix (command, cmd_table[cmd].name)
            && cmd_table[cmd].level <= trust)
        {
            found = TRUE;
            break;
        }
    }

    /*
     * Log and snoop.
     */
    smash_dollar(logline);

    if (cmd_table[cmd].log == LOG_NEVER)
        strcpy (logline, "");

    /* Replaced original block of code with fix from Edwin
     * to prevent crashes due to dollar signs in logstrings.
     * I threw in the above call to smash_dollar() just for
     * the sake of overkill :) JR -- 10/15/00
     */
    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
        char    s[2*MAX_INPUT_LENGTH],*ps;
        int     i;

        ps=s;
        sprintf( log_buf, "Log %s: %s", ch->name, logline );
        /* Make sure that was is displayed is what is typed */
        for (i=0;log_buf[i];i++)
        {
            *ps++=log_buf[i];
            if (log_buf[i]=='$')
                *ps++='$';
            if (log_buf[i]=='{')
                *ps++='{';
        }
        *ps=0;
        wiznet(s,ch,NULL,WIZ_SECURE,0,get_trust(ch));
        log_string( log_buf );
    }

    if (ch->desc != NULL && ch->desc->snoop_by != NULL)
    {
        write_to_buffer (ch->desc->snoop_by, "% ", 2);
        write_to_buffer (ch->desc->snoop_by, logline, 0);
        write_to_buffer (ch->desc->snoop_by, "\n\r", 2);
    }

    if (!found)
    {
        /*
         * Look for command in socials table.
         */
        if (!check_social (ch, command, argument))
            send_to_char ("Huh?\n\r", ch);
        return;
    }

    /*
     * Character not in position for command?
     */
    if (ch->position < cmd_table[cmd].position)
    {
        switch (ch->position)
        {
            case POS_DEAD:
                send_to_char ("Lie still; you are DEAD.\n\r", ch);
                break;

            case POS_MORTAL:
            case POS_INCAP:
                send_to_char ("You are hurt far too bad for that.\n\r", ch);
                break;

            case POS_STUNNED:
                send_to_char ("You are too stunned to do that.\n\r", ch);
                break;

            case POS_SLEEPING:
                send_to_char ("In your dreams, or what?\n\r", ch);
                break;

            case POS_RESTING:
                send_to_char ("Nah... You feel too relaxed...\n\r", ch);
                break;

            case POS_SITTING:
                send_to_char ("Better stand up first.\n\r", ch);
                break;

            case POS_FIGHTING:
                send_to_char ("No way!  You are still fighting!\n\r", ch);
                break;

        }
        return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) (ch, argument);

    tail_chain ();
    return;
}

/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA * ch, DO_FUN * do_fun, char *argument)
{
    char *command_string;

    /* copy the string */
    command_string = str_dup (argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string (command_string);
}

bool check_social (CHAR_DATA * ch, char *command, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found = FALSE;
    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
    {
        if (command[0] == social_table[cmd].name[0]
            && !str_prefix (command, social_table[cmd].name))
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
        return FALSE;

    if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
        send_to_char ("You are anti-social!\n\r", ch);
        return TRUE;
    }

    switch (ch->position)
    {
        case POS_DEAD:
            send_to_char ("Lie still; you are DEAD.\n\r", ch);
            return TRUE;

        case POS_INCAP:
        case POS_MORTAL:
            send_to_char ("You are hurt far too bad for that.\n\r", ch);
            return TRUE;

        case POS_STUNNED:
            send_to_char ("You are too stunned to do that.\n\r", ch);
            return TRUE;

        case POS_SLEEPING:
            /*
             * I just know this is the path to a 12" 'if' statement.  :(
             * But two players asked for it already!  -- Furey
             */
            if (!str_cmp (social_table[cmd].name, "snore"))
                break;
            send_to_char ("In your dreams, or what?\n\r", ch);
            return TRUE;

    }

    one_argument (argument, arg);
    victim = NULL;
    if (arg[0] == '\0')
    {
        act (social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);
        act (social_table[cmd].char_no_arg, ch, NULL, victim, TO_CHAR);
    }
    else if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
    }
    else if (victim == ch)
    {
        act (social_table[cmd].others_auto, ch, NULL, victim, TO_ROOM);
        act (social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR);
    }
    else
    {
        act (social_table[cmd].others_found, ch, NULL, victim, TO_NOTVICT);
        act (social_table[cmd].char_found, ch, NULL, victim, TO_CHAR);
        act (social_table[cmd].vict_found, ch, NULL, victim, TO_VICT);

        if (!IS_NPC (ch) && IS_NPC (victim)
            && !IS_AFFECTED (victim, AFF_CHARM)
            && IS_AWAKE (victim) && victim->desc == NULL)
        {
            switch (number_bits (4))
            {
                case 0:

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                    act (social_table[cmd].others_found,
                         victim, NULL, ch, TO_NOTVICT);
                    act (social_table[cmd].char_found, victim, NULL, ch,
                         TO_CHAR);
                    act (social_table[cmd].vict_found, victim, NULL, ch,
                         TO_VICT);
                    break;

                case 9:
                case 10:
                case 11:
                case 12:
                    act ("$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
                    act ("You slap $N.", victim, NULL, ch, TO_CHAR);
                    act ("$n slaps you.", victim, NULL, ch, TO_VICT);
                    break;
            }
        }
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number (char *arg)
{

    if (*arg == '\0')
        return FALSE;

    if (*arg == '+' || *arg == '-')
        arg++;

    for (; *arg != '\0'; arg++)
    {
        if (!isdigit (*arg))
            return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument (char *argument, char *arg)
{
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++)
    {
        if (*pdot == '.')
        {
            *pdot = '\0';
            number = atoi (argument);
            *pdot = '.';
            strcpy (arg, pdot + 1);
            return number;
        }
    }

    strcpy (arg, argument);
    return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
 */
int mult_argument (char *argument, char *arg)
{
    char *pdot;
    int number;

    for (pdot = argument; *pdot != '\0'; pdot++)
    {
        if (*pdot == '*')
        {
            *pdot = '\0';
            number = atoi (argument);
            *pdot = '*';
            strcpy (arg, pdot + 1);
            return number;
        }
    }

    strcpy (arg, argument);
    return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument (char *argument, char *arg_first)
{
    char cEnd;

    while (isspace (*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0')
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *arg_first = LOWER (*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while (isspace (*argument))
        argument++;

    return argument;
}

/*
 * Contributed by Alander.
 */
void do_commands (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (cmd_table[cmd].level <= LEVEL_HERO
            && cmd_table[cmd].level <= get_trust (ch) && cmd_table[cmd].show)
        {
            sprintf (buf, "%-12s", cmd_table[cmd].name);
            send_to_char (buf, ch);
            if (++col % 6 == 0)
                send_to_char ("\n\r", ch);
        }
    }

    if (col % 6 != 0)
        send_to_char ("\n\r", ch);
    return;
}

/*
 * Shows all immortal commands available
 */
void do_wizhelp (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
    {
        if (cmd_table[cmd].level > LEVEL_HERO
            && cmd_table[cmd].level <= get_trust (ch) && cmd_table[cmd].show)
        {
            sprintf (buf, "%-12s", cmd_table[cmd].name);
            send_to_char (buf, ch);
            if (++col % 6 == 0)
                send_to_char ("\n\r", ch);
        }
    }

    if (col % 6 != 0)
        send_to_char ("\n\r", ch);
    return;
}

/*
 * Begin Alias Functions and Methods
 */

/*
 * Performs the aliasing and other fun stuff.
 */
void substitute_alias (DESCRIPTOR_DATA * d, char *argument)
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH], prefix[MAX_INPUT_LENGTH],
        name[MAX_INPUT_LENGTH];
    char *point;
    int alias;

    ch = d->original ? d->original : d->character;

    /* check for prefix */
    if (ch->prefix[0] != '\0' && str_prefix ("prefix", argument))
    {
        if (strlen (ch->prefix) + strlen (argument) > MAX_INPUT_LENGTH - 2)
            send_to_char ("Line to long, prefix not processed.\r\n", ch);
        else
        {
            sprintf (prefix, "%s %s", ch->prefix, argument);
            argument = prefix;
        }
    }

    if (IS_NPC (ch) || ch->pcdata->alias[0] == NULL
        || !str_prefix ("alias", argument) || !str_prefix ("una", argument)
        || !str_prefix ("prefix", argument))
    {
        interpret (d->character, argument);
        return;
    }

    strcpy (buf, argument);

    for (alias = 0; alias < MAX_ALIAS; alias++)
    {                            /* go through the aliases */
        if (ch->pcdata->alias[alias] == NULL)
            break;

        if (!str_prefix (ch->pcdata->alias[alias], argument))
        {
            point = one_argument (argument, name);
            if (!strcmp (ch->pcdata->alias[alias], name))
            {
                /* More Edwin inspired fixes. JR -- 10/15/00 */
                buf[0] = '\0';
                strcat(buf,ch->pcdata->alias_sub[alias]);
                if (point[0])
                {
                    strcat(buf," ");
                    strcat(buf,point);
                }

                if (strlen (buf) > MAX_INPUT_LENGTH - 1)
                {
                    send_to_char
                        ("Alias substitution too long. Truncated.\r\n", ch);
                    buf[MAX_INPUT_LENGTH - 1] = '\0';
                }
                break;
            }
        }
    }
    interpret (d->character, buf);
} // end void substitute_alias

/*
 * Function to force the player to fully type out alias.
 */
void do_alia (CHAR_DATA * ch, char *argument)
{
    send_to_char ("I'm sorry, alias must be entered in full.\n\r", ch);
    return;
} // end do_alia

/*
 * Command to allow a player to set one of their aliases.
 */
void do_alias (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    int pos;

    smash_tilde (argument);

    if (ch->desc == NULL)
        rch = ch;
    else
        rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC (rch))
        return;

    argument = one_argument (argument, arg);


    if (arg[0] == '\0')
    {

        if (rch->pcdata->alias[0] == NULL)
        {
            send_to_char ("You have no aliases defined.\n\r", ch);
            return;
        }
        send_to_char ("Your current aliases are:\n\r", ch);

        for (pos = 0; pos < MAX_ALIAS; pos++)
        {
            if (rch->pcdata->alias[pos] == NULL
                || rch->pcdata->alias_sub[pos] == NULL)
                break;

            sprintf (buf, "    %s:  %s\n\r", rch->pcdata->alias[pos],
                     rch->pcdata->alias_sub[pos]);
            send_to_char (buf, ch);
        }
        return;
    }

    if (!str_prefix ("una", arg) || !str_cmp ("alias", arg))
    {
        send_to_char ("Sorry, that word is reserved.\n\r", ch);
        return;
    }

    /* More Edwin-inspired fixes. JR -- 10/15/00 */
    if (strchr(arg,' ')||strchr(arg,'"')||strchr(arg,'\''))
    {
        send_to_char("The word to be aliased should not contain a space, "
            "a tick or a double-quote.\n\r",ch);
        return;
    }

    if (argument[0] == '\0')
    {
        for (pos = 0; pos < MAX_ALIAS; pos++)
        {
            if (rch->pcdata->alias[pos] == NULL
                || rch->pcdata->alias_sub[pos] == NULL)
                break;

            if (!str_cmp (arg, rch->pcdata->alias[pos]))
            {
                sprintf (buf, "%s aliases to '%s'.\n\r",
                         rch->pcdata->alias[pos],
                         rch->pcdata->alias_sub[pos]);
                send_to_char (buf, ch);
                return;
            }
        }

        send_to_char ("That alias is not defined.\n\r", ch);
        return;
    }

    if (!str_prefix (argument, "delete") || !str_prefix (argument, "prefix"))
    {
        send_to_char ("That shall not be done!\n\r", ch);
        return;
    }

    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
        if (rch->pcdata->alias[pos] == NULL)
            break;

        if (!str_cmp (arg, rch->pcdata->alias[pos]))
        {                        /* redefine an alias */
            free_string (rch->pcdata->alias_sub[pos]);
            rch->pcdata->alias_sub[pos] = str_dup (argument);
            sprintf (buf, "%s is now realiased to '%s'.\n\r", arg, argument);
            send_to_char (buf, ch);
            return;
        }
    }

    if (pos >= MAX_ALIAS)
    {
        send_to_char ("Sorry, you have reached the alias limit.\n\r", ch);
        return;
    }

    /* make a new alias */
    rch->pcdata->alias[pos] = str_dup (arg);
    rch->pcdata->alias_sub[pos] = str_dup (argument);
    sprintf (buf, "%s is now aliased to '%s'.\n\r", arg, argument);
    send_to_char (buf, ch);
} // end do_alias

/*
 * Command to allow a player to remove one of their aliases
 */
void do_unalias (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH];
    int pos;
    bool found = FALSE;

    if (ch->desc == NULL)
        rch = ch;
    else
        rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC (rch))
        return;

    argument = one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Unalias what?\n\r", ch);
        return;
    }

    for (pos = 0; pos < MAX_ALIAS; pos++)
    {
        if (rch->pcdata->alias[pos] == NULL)
            break;

        if (found)
        {
            rch->pcdata->alias[pos - 1] = rch->pcdata->alias[pos];
            rch->pcdata->alias_sub[pos - 1] = rch->pcdata->alias_sub[pos];
            rch->pcdata->alias[pos] = NULL;
            rch->pcdata->alias_sub[pos] = NULL;
            continue;
        }

        if (!strcmp (arg, rch->pcdata->alias[pos]))
        {
            send_to_char ("Alias removed.\n\r", ch);
            free_string (rch->pcdata->alias[pos]);
            free_string (rch->pcdata->alias_sub[pos]);
            rch->pcdata->alias[pos] = NULL;
            rch->pcdata->alias_sub[pos] = NULL;
            found = TRUE;
        }
    }

    if (!found)
        send_to_char ("No alias of that name to remove.\n\r", ch);
} // end do_unalias
