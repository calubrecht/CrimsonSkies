/***************************************************************************
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell (Rhien)   *
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
 *  This code file houses miscellaneous methods that don't fit anywhere    *
 *  else.  - Rhien                                                         *
 **************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "merc.h"

/*
 * Extended Bitvector Routines (originating in the Smaug code base by Thoric)
 */

/*
 * Check to see if the extended bitvector is completely empty
 */
bool ext_is_empty(EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        if (bits->bits[x] != 0)
            return FALSE;

    return TRUE;
}

/*
 * Clears any bits from the extended
 */
void ext_clear_bits(EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        bits->bits[x] = 0;
}

/*
 * For use by xHAS_BITS() -- works like IS_SET()
 */
int ext_has_bits(EXT_BV *var, EXT_BV *bits)
{
    int x, bit;

    for (x = 0; x < XBI; x++)
        if ((bit = (var->bits[x] & bits->bits[x])) != 0)
            return bit;

    return 0;
}

/*
 * For use by xSAME_BITS() -- works like ==
 */
bool ext_same_bits(EXT_BV *var, EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        if (var->bits[x] != bits->bits[x])
            return FALSE;

    return TRUE;
}

/*
 * For use by xSET_BITS() -- works like SET_BIT()
 */
void ext_set_bits(EXT_BV *var, EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        var->bits[x] |= bits->bits[x];
}

/*
 * For use by xREMOVE_BITS() -- works like REMOVE_BIT()
 */
void ext_remove_bits(EXT_BV *var, EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        var->bits[x] &= ~(bits->bits[x]);
}

/*
 * For use by xTOGGLE_BITS() -- works like TOGGLE_BIT()
 */
void ext_toggle_bits(EXT_BV *var, EXT_BV *bits)
{
    int x;

    for (x = 0; x < XBI; x++)
        var->bits[x] ^= bits->bits[x];
}

/*
 * Read an extended bitvector from a file.                      -Thoric
 */
EXT_BV fread_bitvector(FILE *fp)
{
    EXT_BV ret;
    int c, x = 0;
    int num = 0;

    memset(&ret, '\0', sizeof(ret));
    for (;; )
    {
        num = fread_number(fp);
        if (x < XBI)
            ret.bits[x] = num;
        ++x;
        if ((c = getc(fp)) != '&')
        {
            ungetc(c, fp);
            break;
        }
    }

    return ret;
}

/*
 * Return a string for writing a bitvector to a file
 */
char *print_bitvector(EXT_BV *bits)
{
    static char buf[XBI * 12];
    char *p = buf;
    int x, cnt = 0;

    for (cnt = XBI - 1; cnt > 0; cnt--)
        if (bits->bits[cnt])
            break;
    for (x = 0; x <= cnt; x++)
    {
        sprintf(p, "%d", bits->bits[x]);
        p += strlen(p);
        if (x < cnt)
            *p++ = '&';
    }
    *p = '\0';

    return buf;
}

/*
 * Write an extended bitvector to a file                        -Thoric
 */
void fwrite_bitvector(EXT_BV *bits, FILE *fp)
{
    fputs(print_bitvector(bits), fp);
}

EXT_BV meb(int bit)
{
    EXT_BV bits;

    xCLEAR_BITS(bits);
    if (bit >= 0)
        xSET_BIT(bits, bit);

    return bits;
}

EXT_BV multimeb(int bit, ...)
{
    EXT_BV bits;
    va_list param;
    int b;

    xCLEAR_BITS(bits);
    if (bit < 0)
        return bits;

    xSET_BIT(bits, bit);

    va_start(param, bit);

    while ((b = va_arg(param, int)) != -1)
        xSET_BIT(bits, b);

    va_end(param);

    return bits;
}
