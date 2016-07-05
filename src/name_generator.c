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
 *  Name Generator (Written 12/16/1999, Last Updated 04/16/2016            *
 *                                                                         *
 *  This code will generate mostly good names.  Instead of writing a       *
 *  method that attempts to create the name parts I have included a mass   *
 *  list of name parts that can be pieced together with a high rate of     *
 *  returning a decent name.  There are two large tables of these parts    *
 *  but in the grand scheme these take up minimial memory.  This was       *
 *  orignally written as a Java class for use in an applet back in the     *
 *  day, then converted to C# for use in a Windows Phone app (hold the     *
 *  laughter please) and now making it's way back to C for Diku Muds.      *
 *                                                                         *
 *      -Rhien                                                             *
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"

int max_name_part;  // So we only count them once per go around.

/*
 * Generates a random name and checks to make sure that a player doesn't already
 * exist with that name.
 */
char *generate_random_name()
{
    static char buf[64];
    int first_index = 0;
    int last_index = 0;
    int counter = 0;
    bool found = FALSE;
    char filename[MAX_STRING_LENGTH];

    // If the name parts haven't been initiatlized do that so we know the upper
    // bound for our random number generating.
    if (max_name_part == 0)
    {
        init_name_parts();
    }

    // Loop until a valid unused name is found, as a precaution we'll ditch out after
    // 1,000 guesses, highly unlikely this will ever be the case.
    do
    {
        counter++;
        first_index = number_range(0, max_name_part);
        last_index = number_range(0, max_name_part);

        // Construct where this pfile would live so we can check for its existence.
        sprintf(filename, "%s%s%s",
            PLAYER_DIR,
            capitalize(name_part_table[first_index].first_part),
            name_part_table[last_index].last_part);

        // Checks to try to ensure a good name
        if (name_part_table[first_index].first_part[(strlen(name_part_table[first_index].first_part)-1)]
            != name_part_table[last_index].last_part[0]
            && first_index != last_index
            && file_exists(filename) == FALSE)
        {
            found = TRUE;
        }

    } while (found == FALSE && counter < 1000);

    sprintf(buf, "%s%s",
        name_part_table[first_index].first_part,
        name_part_table[last_index].last_part);

    return buf;
}

/*
 * Returns the number of elements in the name_part_table.  This will be stored in a global
 * variable for re-use so it doesn't have to be counted again.  The count part table must
 * end with a NULL set.
 */
void init_name_parts()
{
    // It's already been counted, no need to count them again.
    if (max_name_part > 0)
    {
        return;
    }

    do
    {
        max_name_part++;
    } while (!IS_NULLSTR(name_part_table[max_name_part].first_part));

    // The last element is NULL, so we'll subtract 1 from the total count.
    max_name_part = max_name_part - 1;

    log_f("STATUS: Initializing Random Name Table Count - %d entries (%s maximum possibilities)",
        max_name_part, num_punct(max_name_part * max_name_part));

    // This would need to be be commented out or removed if moving this code to another mud.
    if (global.last_boot_result == UNKNOWN)
        global.last_boot_result = SUCCESS;

    return;
}

/*
 * Shows the player random names, 5 rows of 4 columns.
 */
void do_random_names(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int row = 0;
    int col = 0;

    for (row = 0; row < 5; row++)
    {
        // Since the random function returns a static char we have to use it in
        // separate calls.
        for (col = 0; col < 4; col++)
        {
            sprintf(buf, "%-18s", generate_random_name());
            send_to_char(buf, ch);
        }

        send_to_char("\r\n", ch);
    }

    return;
}

/*
 *  The name part list, the first and last parts aren't necessarily related, they
 *  kept in separate columns (instead of separate tables) to make this a little more
 *  compact.  I know, it's a big table. :p
 */
const struct name_part_type name_part_table[] = {
        {"An", "cus"},
        {"Ani", "ny"},
        {"Jh", "mar"},
        {"Mar", "ti"},
        {"Jo", "tar"},
        {"Ha", "ter"},
        {"Da", "lor"},
        {"De", "ie"},
        {"Ka", "ain"},
        {"Kh", "ane"},
        {"Ki", "ing"},
        {"Lae", "sey"},
        {"Chae", "lund"},
        {"Mein", "ald"},
        {"Jaek", "hald"},
        {"Kail", "lie"},
        {"Kh", "ley"},
        {"Khet", "arn"},
        {"Kha", "ern"},
        {"Khod", "vin"},
        {"Day", "ca"},
        {"Kaz", "driel"},
        {"To", "nei"},
        {"Lea", "thar"},
        {"Bar", "dan"},
        {"Xa", "den"},
        {"Aur", "ene"},
        {"Auer", "uir"},
        {"Ain", "nus"},
        {"Aik", "ena"},
        {"Aill", "me"},
        {"El", "dor"},
        {"Elil", "orne"},
        {"Ekin", "ward"},
        {"Kw", "dol"},
        {"Ce", "ost"},
        {"Jer", "ein"},
        {"Jar", "ose"},
        {"Mur", "ia"},
        {"Jor", "berg"},
        {"Art", "aris"},
        {"Mu", "ares"},
        {"Sh", "indi"},
        {"Jo", "een"},
        {"Ran", "leen"},
        {"Mal", "ech"},
        {"Mad", "ach"},
        {"Mald", "anx"},
        {"Cald", "ala"},
        {"Chald", "erd"},
        {"Rel", "ard"},
        {"Van", "ia"},
        {"Var", "tuk"},
        {"Ve", "an"},
        {"Ved", "te"},
        {"Con", "hir"},
        {"Cai", "len"},
        {"Del", "us"},
        {"Aur", "is"},
        {"Ain", "ik"},
        {"Bla", "ses"},
        {"Bri", "ian"},
        {"Mag", "as"},
        {"Arg", "nan"},
        {"Arn", "nen"},
        {"Thor", "rot"},
        {"Ely", "ran"},
        {"Darn", "th"},
        {"Yev", "ith"},
        {"Za", "yth"},
        {"Ear", "mund"},
        {"Eae", "ion"},
        {"Eia", "tan"},
        {"Khir", "in"},
        {"Lha", "on"},
        {"Lhi", "ilis"},
        {"Lan", "iak"},
        {"Lho", "ova"},
        {"La", "na"},
        {"Lam", "ebus"},
        {"Na", "or"},
        {"Nat", "nor"},
        {"Nail", "io"},
        {"Nel", "li"},
        {"Sce", "lli"},
        {"Pal", "oion"},
        {"Pel", "eld"},
        {"Pell", "issa"},
        {"Qin", "eron"},
        {"Sar", "orn"},
        {"Sor", "gan"},
        {"Ser", "nyr"},
        {"Gin", "ne"},
        {"Darn'", "ais"},
        {"Ces", "rn"},
        {"Tan", "mi"},
        {"Ryd", "i"},
        {"Ry", "a"},
        {"Kan", "ais"},
        {"Can", "an"},
        {"Med", "rin"},
        {"Laun", "ma"},
        {"Zhan", "ay"},
        {"Drit", "anilis"},
        {"Dev", "orie"},
        {"Jha", "ec"},
        {"Sae", "ar"},
        {"Seg", "lynn"},
        {"Tra", "yle"},
        {"Ste", "le"},
        {"Kit", "mond"},
        {"Ar", "ea"},
        {"Ghal", "iam"},
        {"Gel", "gar"},
        {"Ghei", "eaus"},
        {"Hae", "lis"},
        {"Hel", "lug"},
        {"Yve", "ton"},
        {"Co", "aran"},
        {"Cov", "elle"},
        {"Rh", "edor"},
        {"Bla", "lin"},
        {"Be", "ales"},
        {"For", "eth"},
        {"Ghal", "et"},
        {"Lyn", "ett"},
        {"Lin", "ut"},
        {"Der", "ius"},
        {"Lae", "nir"},
        {"Lhe", "ea"},
        {"Nad", "ra"},
        {"Nae", "el"},
        {"Bre", "ael"},
        {"Bree", "al"},
        {"Dan", "tic"},
        {"Sara", "ana"},
        {"Kar", "per"},
        {"Car", "se"},
        {"Mis", "ari"},
        {"Ker", "aber"},
        {"Cel", "iva"},
        {"Felic", "en"},
        {"Ey", "ridge"},
        {"Er", "tho"},
        {"Eyr", "then"},
        {"Ary", "yon"},
        {"Ay", "than"},
        {"Ar", "ara"},
        {"Or", "ima"},
        {"Og", "kal"},
        {"Cy", "les"},
        {"Cyr", "min"},
        {"Vi", "lly"},
        {"Viv", "ess"},
        {"Saf", "geld"},
        {"Sahi", "lel"},
        {"Roa", "lela"},
        {"Jhil", "aym"},
        {"Ji", "yrk"},
        {"Jia", "ika"},
        {"Hae", "mael"},
        {"Elda", "ikan"},
        {"Cia", "nic"},
        {"Kier", "tras"},
        {"Mil", "ad"},
        {"Kal", "byne"},
        {"Kel", "alik"},
        {"Aes", "illia"},
        {"Rei", "mec"},
        {"Elion", "don"},
        {"Es", "din"},
        {"Qui", "aol"},
        {"Rosa", "irae"},
        {"Resa", "ira"},
        {"Rol", "ana"},
        {"Aphi", "ance"},
        {"Adri", "sia"},
        {"Lim", "da"},
        {"Swae", "dran"},
        {"Kev", "as"},
        {"Gray", "ias"},
        {"Ly", "droz"},
        {"Lys", "ri"},
        {"Rhan", "kle"},
        {"Dre", "ak"},
        {"Lhe", "shan"},
        {"Nev", "stos"},
        {"Nav", "umn"},
        {"Mor", "oal"},
        {"Nag", "es"},
        {"Su", "aie"},
        {"Den", "dar"},
        {"Sha", "nar"},
        {"Vera", "ner"},
        {"Tyg", "sal"},
        {"Mer", "ell"},
        {"Marl", "enne"},
        {"Merl", "ial"},
        {"Marli", "rial"},
        {"Merli", "thas"},
        {"Acth", "thes"},
        {"Amb", "thus"},
        {"Ammil", "vara"},
        {"Ammis", "gorn"},
        {"Emmis", "gon"},
        {"Loh", "daelf"},
        {"Rey", "un"},
        {"Ist", "lan"},
        {"Isil", "mor"},
        {"Illi", "mir"},
        {"Lind", "thor"},
        {"Rhye", "del"},
        {"Cor", "mas"},
        {"Chor", "ella"},
        {"Coal", "iten"},
        {"Chel", "phine"},
        {"Joul", "ryion"},
        {"Mel", "luk"},
        {"Nal", "uel"},
        {"Lad", "eton"},
        {"Lhy", "las"},
        {"Kham", "rei"},
        {"Khem", "vel"},
        {"Kem", "vil"},
        {"Kam", "val"},
        {"Khel", "eus"},
        {"Khal", "aus"},
        {"Bhal", "ga"},
        {"Bae", "loc"},
        {"Bhae", "lae"},
        {"Vhae", "lus"},
        {"Vae", "anne"},
        {"Whae", "ly"},
        {"Wae", "ean"},
        {"Phae", "eam"},
        {"Pae", "co"},
        {"Fae", "cor"},
        {"Fhae", "ity"},
        {"Thae", "ic"},
        {"Xae", "y"},
        {"Xith", "yk"},
        {"All", "ym"},
        {"Alex", "yn"},
        {"Alax", "e"},
        {"Erid", "ax"},
        {"Akir", "ix"},
        {"Jyr", "ex"},
        {"Jhyr", "y"},
        {"Jir", "eon"},
        {"Jil", "yla"},
        {"Arad", "ela"},
        {"Croe", "ale"},
        {"Rhe", "sea"},
        {"Len", "ory"},
        {"Lhen", "ary"},
        {"Quen", "ry"},
        {"Cori", "ke"},
        {"Coar", "acy"},
        {"Olli", "aecy"},
        {"Nata", "san"},
        {"Erli", "di"},
        {"Klor", "ere"},
        {"Klar", "ka"},
        {"Niko", "van"},
        {"Nihk", "ven"},
        {"Nika", "lon"},
        {"Neta", "il"},
        {"Jol", "ien"},
        {"Jhol", "aine"},
        {"Ibi", "oth"},
        {"Iba", "ath"},
        {"Ili", "dra"},
        {"Ile", "iad"},
        {"Ila", "tin"},
        {"Zog", "iel"},
        {"Tag", "sin"},
        {"Teg", "sen"},
        {"Taz", "men"},
        {"D'Ar", "man"},
        {"Drae", "kan"},
        {"Kwea", "ken"},
        {"Kea", "kin"},
        {"Jor'", "cal"},
        {"Lae'", "gra"},
        {"Mal'", "rik"},
        {"Kel'", "inx"},
        {"Shir'", "brie"},
        {"Rhal'", "era"},
        {"Dan'", "ura"},
        {"Drake", "this"},
        {"Drake'", "os"},
        {"Ug'", "'os"},
        {"Paor", "ia"},
        {"Laor", "tuk"},
        {"Halid", "an"},
        {"Hy", "a"},
        {"By", "us"},
        {"Ky", "is"},
        {"Kyl", "ik"},
        {"Kyae", "ian"},
        {"Wy", "as"},
        {"Xy", "li"},
        {"Zy", "ran"},
        {"Gy", "tan"},
        {"Fy", "in"},
        {"Dy", "on"},
        {"Gyah", "ly"},
        {"Fyah", "iak"},
        {"Py", "i"},
        {"Pyr", "or"},
        {"Fyr", "vor"},
        {"Kyr", "nor"},
        {"Dae", "io"},
        {"Cait", "tael"},
        {"Kait", "ead"},
        {"Kaet", "lyn"},
        {"Jaik", "ah"},
        {"Jake", "lok"},
        {"Ali", "at"},
        {"Ava", "ire"},
        {"Si", "par"},
        {"Sli", "tian"},
        {"Rhae", "ler"},
        {"Bael'", "andi"},
        {"Bael", "andus"},
        {"Kal'", "endus"},
        {"Jao'", "kel"},
        {"Pail'", "xia"},
        {"Kiri'", "los"},
        {"Kry", "rak"},
        {"Kry'", "riak"},
        {"Aea", "oan"},
        {"Aea", "son"},
        {"Sil", "lah"},
        {"Stae", "yson"},
        {"Stear", "ert"},
        {"Steer", "art"},
        {"St", "ahno"},
        {"Stou", "hes"},
        {"Stai'", "des"},
        {"Stai", "das"},
        {"Stah", "dis"},
        {"Stee", "tai"},
        {"Stan", "ami"},
        {"Shea", "emi"},
        {"Sheal", "ine"},
        {"Shel", "alis"},
        {"Shale", "la"},
        {"Sca", "aon"},
        {"Sci", "anna"},
        {"Lyt", "enna"},
        {"Lyi", "inna"},
        {"Lye", "thie"},
        {"Lae", "iya"},
        {"Lai", "aya"},
        {"Lahi", "eya"},
        {"Laeh", "enya"},
        {"Claris", "anya"},
        {"Charis", "can"},
        {"Char", "erus"},
        {"Chal", "arus"},
        {"Kyli", "irus"},
        {"Olg", "erak"},
        {"Oti", "arak"},
        {"Ber", "aes"},
        {"Bhar", "ies"},
        {"Bhil", "'ll"},
        {"Lac", "'el"},
        {"Cael", "'al"},
        {"Dyl", "gen"},
        {"Sa", "gin"},
        {"So", "cern"},
        {"Ra", "cen"},
        {"Pa", "ker"},
        {"Pe", "aox"},
        {"Per", "ery"},
        {"Ma", "kul"},
        {"Ca", "kner"},
        {"Ia", "eden"},
        {"En", "adan"},
        {"Ya", "icx"},
        {"Yal", "ash"},
        {"Val", "esh"},
        {"Va", "aust"},
        {"Vad", "eust"},
        {"Bi", "win"},
        {"Di", "wan"},
        {"Ri", "won"},
        {"Li", "dyn"},
        {"Ao", "tos"},
        {"Lo", "tas"},
        {"Do", "tes"},
        {"No", "sela"},
        {"Mo", "andar"},
        {"Ko", "endar"},
        {"Row", "aryx"},
        {"Ers", "goth"},
        {"Kat", "gath"},
        {"Cat", "geth"},
        {"Lev", "'zig"},
        {"Go", "der"},
        {"Gi", "ock"},
        {"Gu", "ocke"},
        {"Fa", "tal"},
        {"Fo", "tel"},
        {"Fi", "tol"},
        {"Fe", "lani"},
        {"Fel", "leni"},
        {"Fal", "lini"},
        {"Fol", "rett"},
        {"Fal", "ret"},
        {"Xi", "ratt"},
        {"Ba", "ack"},
        {"Rhi", "eck"},
        {"Jue", "ora"},
        {"Juel", "dea"},
        {"Cag", "athal"},
        {"Lan", "inia"},
        {"Olw", "inix"},
        {"Lon", "effe"},
        {"Aue", "tag"},
        {"Arth", "tog"},
        {"Blae", "teg"},
        {"Ay", "'la"},
        {"Jan", "arch"},
        {"Jen", "'arch"},
        {"Jon", "erch"},
        {"Gen", "cil"},
        {"Kas", "kil"},
        {"Cas", "cele"},
        {"Lun", "cela"},
        {"Ge", "wey"},
        {"He", "way"},
        {"Hea", "avan"},
        {"Bai", "evan"},
        {"Ai", "jer"},
        {"Ana", "jir"},
        {"Ena", "rain"},
        {"Eni", "rein"},
        {"Zi", "olik"},
        {"Zin", "olick"},
        {"Zen", "che"},
        {"Yae", "anic"},
        {"Uli", "anick"},
        {"Ul", "drik"},
        {"Ula", "drick"},
        {"Ela", "drak"},
        {"Ala", "sus"},
        {"Aer", "sis"},
        {"Aar", "sas"},
        {"Cir", "ras"},
        {"Ale", "res"},
        {"Iri", "ris"},
        {"Ben", "rus"},
        {"Cae", "fax"},
        {"Kae", "atch"},
        {"Cay", "etch"},
        {"Rae", "iot"},
        {"Gwi", "iat"},
        {"Gwa", "thax"},
        {"Gwe", "ase"},
        {"It", "ric"},
        {"Ro", "six"},
        {"Wil", "ila"},
        {"Wel", "alla"},
        {"Wa", "lisa"},
        {"Whit", "arian"},
        {"Mae", "onic"},
        {"Jae", "onick"},
        {"Sie", "ria"},
        {"Ni", "iah"},
        {"Nie", "iss"},
        {"Les", "isik"},
        {"Eva", "isick"},
        {"Syr", "aac"},
        {"Nor", "aak"},
        {"Nia", "imus"},
        {"Adr", "imas"},
        {"Myr", "pher"},
        {"Ras", "zca"},
        {"Res", "yri"},
        {"Vai", "ienne"},
        {"Vay", "shia"},
        {"Ghe", "shio"},
        {"Gah", "dair"},
        {"Me", "deir"},
        {"Mek", "more"},
        {"Lar", "imore"},
        {"Ann", "imare"},
        {"Zvi", "aw"},
        {"Nor", "aos"},
        {"Kath", "'tel"},
        {"Jak", "'til"},
        {"Jac", "werd"},
        {"Mac", "ick"},
        {"Fia", "lann"},
        {"Mia", "lenn"},
        {"Gia", "ace"},
        {"Mea", "ece"},
        {"Ria", "ige"},
        {"Rea", "age"},
        {"Te", "ege"},
        {"Ta", "risa"},
        {"Ti", "resa"},
        {"Nar", "rey"},
        {"Ner", "ray"},
        {"Orth", "vion"},
        {"Ox", "veon"},
        {"Ax", "cius"},
        {"Ty", "caus"},
        {"Tye", "stian"},
        {"Qu", "iant"},
        {"Gil", "eant"},
        {"Ga", "branth"},
        {"Era", "ter"},
        {"Hil", "tir"},
        {"Jes", "iaz"},
        {"Jas", "iax"},
        {"End", "hea"},
        {"And", "rit"},
        {"Kul", "trek"},
        {"Kil", "trak"},
        {"Kie", "trick"},
        {"Ida", "ki"},
        {"Cie", "ne"},
        {"Cob", "baen"},
        {"Zo", "bane"},
        {"Ir", "wyth"},
        {"Ath", "ew"},
        {"Eth", "elyn"},
        {"Al", "alyn"},
        {"Als", "elynn"},
        {"Els", "alynn"},
        {"Dau", "war"},
        {"Dar", "wer"},
        {"Ae", "ore"},
        {"Gra", "are"},
        {"Aed", "re"},
        {"Ed", "mire"},
        {"Brin", "arck"},
        {"Keu", "aire"},
        {"Bryn", "eire"},
        {"Dea", "ur"},
        {"Fre", "ir"},
        {"Keri", "lar"},
        {"Ken", "lier"},
        {"Cen", "wynn"},
        {"Kaie", "rea"},
        {"Neb", "erah"},
        {"Aid", "ku"},
        {"Hael", "reg"},
        {"Kol", "rig"},
        {"Eri", "aiya"},
        {"Tul", "zad"},
        {"Lao", "urn"},
        {"Sio", "riel"},
        {"Tue", "ich"},
        {"Tuel", "han"},
        {"Thar", "tzl"},
        {"Nei", "idy"},
        {"Fin", "edy"},
        {"Fen", "ady"},
        {"Fan", "tem"},
        {"Vau", "tam"},
        {"Wai", "am"},
        {"Bru", "ander"},
        {"Dol", "ender"},
        {"Tau", "inder"},
        {"Vin", "eah"},
        {"Kai", "taur"},
        {"Is", "aur"},
        {"Goth", "ron"},
        {"Od", "arte"},
        {"Oro", "anis"},
        {"Cyn", "enis"},
        {"Cin", "eris"},
        {"Sin", "uris"},
        {"Sam", "axe"},
        {"Ora", "ixe"},
        {"Asg", "alas"},
        {"Lie", "eles"},
        {"Che", "arick"},
        {"Cha", "arr"},
        {"Chi", "ask"},
        {"Lund", "isk"},
        {"Ex", "esk"},
        {"The", "arin"},
        {"Rem", "erin"},
        {"Ash", "iton"},
        {"Ap", "ile"},
        {"Ael", "ail"},
        {"Cuth", "eil"},
        {"Eath", "thil"},
        {"Xav", "hil"},
        {"Aio", "thael"},
        {"Thod", "relan"},
        {"Khyl", "er"},
        {"Shal", "alar"},
        {"Xan", "alis"},
        {"Kane", "s"},
        {"Kain", "aux"},
        {"Fau", "aix"},
        {"Tig", "emas"},
        {"Elle", "eshar"},
        {"Ell", "lian"},
        {"Para", "ier"},
        {"Svo", "iar"},
        {"Grun", "gris"},
        {"Gran", "oti"},
        {"Cre", "lius"},
        {"Cra", "lies"},
        {"Cru", "thal"},
        {"Math", "ide"},
        {"Slo", "mis"},
        {"Kae", "aed"},
        {"Zyph", "ark"},
        {"Mus", "mus"},
        {"Mes", "mes"},
        {"Daw", "rok"},
        {"Dew", "raek"},
        {"Merc", "erion"},
        {"Noa", "iana"},
        {"Blai", "iena"},
        {"Lhae", "raen"},
        {"Malch", "rach"},
        {"Isa", "isa"},
        {"Ari", "esa"},
        {"Dri", "esea"},
        {"Kyi", "aka"},
        {"Pix", "urik"},
        {"Wal", "taer"},
        {"Yog", "fer"},
        {"Tre", "roth"},
        {"Ara", "varis"},
        {"Tay", "lir"},
        {"Lir", "asik"},
        {"Tarn", "ael"},
        {"Die", "tath"},
        {"Luc", "ent"},
        {"Lyc", "int"},
        {"Cof", "nix"},
        {"Ham", "ham"},
        {"Hyl", "cort"},
        {"Hal", "gard"},
        {"Faer", "esik"},
        {"Xyt", "hor"},
        {"Dia", "aer"},
        {"Seba", "irn"},
        {"Ere", "aal"},
        {"Ereb", "oer"},
        {"Bron", "seon"},
        {"Yel", "tiak"},
        {"Phul", "yaer"},
        {"Chri", "ina"},
        {"Cais", "andra"},
        {"Kais", "endra"},
        {"Khis", "elic"},
        {"Cul", "taea"},
        {"Vand", "avar"},
        {"Brol", "tiac"},
        {"Laer", "su"},
        {"Teo", "wen"},
        {"Kyb", "ros"},
        {"Beh", "shar"},
        {"Ehyl", "olas"},
        {"Kala", "inn"},
        {"Cala", "'n"},
        {NULL, NULL},
};
