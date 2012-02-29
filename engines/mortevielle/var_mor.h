/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/*
 * This code is based on original Mortville Manor DOS source code
 * Copyright (c) 1988-1989 Lankhor
 */

#ifndef MORTEVIELLE_VAR_H
#define MORTEVIELLE_VAR_H

#include "common/rect.h"
#include "common/str.h"

namespace Mortevielle {

/*         Constantes, Types et Variables pour le

              M A N O I R   DE   M O R T E V I E L L E

                                                    ( version Nov 1988
                                                             +
                                                  musique & paroles Oct 88 )
                                                                             */

/*---------------------------------------------------------------------------*/
/*-------------------           MEMORY  MAP          ------------------------*/
/*---------------------------------------------------------------------------*/
/* The following is a list of physical addresses in memory currently used
 * by the game.
 *
 * Address
 * -------
 * 5000:0 - Music data
 * 6000:0 - Decompressed current image
 * 7000:0+ - Compressed images
 * 7000:2 - 16 words representing palette map
 * 7000:4138 - width, height, x/y offset of decoded image
 */


/*---------------------------------------------------------------------------*/
/*-------------------------     DEFINES    ----------------------------------*/
/*---------------------------------------------------------------------------*/

#define ord(v) ((int) v)
#define chr(v) ((unsigned char) v)
#define lo(v) ((v) & 0xff)
#define hi(v) (((v) >> 8) & 0xff)
#define swap(v) (((lo(v)) << 8) | ((hi(v)) >> 8)) 
#define odd(v) (((v) % 2) == 1)

/*---------------------------------------------------------------------------*/
/*-------------------------    CONSTANTS   ----------------------------------*/
/*---------------------------------------------------------------------------*/

const float freq0 = 1.19318e6;

const int adani = 0x7314;
const int adbruit = 0x5cb0;/*2C00;*/
const int adbruit1 = 0x6924;
const int adbruit3 = 0x6ba6;/*3AF6;*/
const int adbruit5 = 0x3b50;
const int adson = 0x5cb0;/*2C00;*/
const int adtroct = 0x406b;
const int adword = 0x4000;
const int offsetb1 = 6;
const int offsetb3 = 6;

// Useless constants
//const int segdon = 0x6c00;
//const int adbruit2 = 0x6b30;/*3A80;*/
//const int adson2 = 0x60b0;/*3000;*/
//const int seg_syst = 0x6fed;
//const int offsetb2 = 4;

const int null = 255;

const int tempo_mus = 71;
const int tempo_bruit = 78;
const int tempo_f = 80;
const int tempo_m = 89;

const int ti1 = 410;
const int ti2 = 250;
const int maxti = 7975;
const int maxtd = 600;
const int max_rect = 14;

const int c_repon = 0;
const int c_st41 = 186;
const int c_tparler = 247;
const int c_paroles = 292;
const int c_tmlieu = 435;
const int c_dialpre = 456;
const int c_action = 476;
const int c_saction = 497;
const int c_dis = 502;
const int c_fin = 510;    /*  =>   n'existe pas  ; si !! */

const int arega = 0;
const int asoul = 154;
const int aouvr = 282;
const int achai = 387;
const int acha = 492;
const int arcf = 1272;
const int arep = 1314;
const int amzon = 1650;
const int fleche = 1758;

const int OPCODE_NONE = 0;

enum verbs {OPCODE_ATTACH = 0x301, OPCODE_WAIT = 0x302,  OPCODE_FORCE = 0x303,   OPCODE_SLEEP = 0x304, OPCODE_LISTEN = 0x305, 
            OPCODE_ENTER = 0x306,  OPCODE_CLOSE = 0x307, OPCODE_SEARCH = 0x308,  OPCODE_KNOCK = 0x309, OPCODE_SCRATCH = 0x30a,
			OPCODE_READ = 0x30b,   OPCODE_EAT = 0x30c,   OPCODE_PLACE = 0x30d,   OPCODE_OPEN = 0x30e,  OPCODE_TAKE = 0x30f,
			OPCODE_LOOK = 0x310,   OPCODE_SMELL = 0x311, OPCODE_SOUND = 0x312,   OPCODE_LEAVE = 0x313, OPCODE_LIFT = 0x314,
			OPCODE_TURN = 0x315,   OPCODE_SHIDE = 0x401, OPCODE_SSEARCH = 0x402, OPCODE_SREAD = 0x403, OPCODE_SPUT = 0x404,
			OPCODE_SLOOK = 0x405};

const int max_patt = 20;

extern const byte tabdbc[18];
extern const byte tabdph[16];
extern const byte typcon[26];
extern const byte intcon[26];
extern const byte tnocon[364];

extern const byte _menuConstants[8][4];

extern const byte addv[2];

extern const byte rang[16];

/*---------------------------------------------------------------------------*/
/*--------------------------------   TYPES   --------------------------------*/
/*---------------------------------------------------------------------------*/

struct sav_chaine {
	int conf;
	byte pourc[11];
	byte teauto[43];
	byte sjer[31];
	int mlieu, iboul, ibag, icave, ivier, ipuit;
	int derobj, iloic, icryp;
	bool ipre;
	byte heure;
};

struct registres {
	int ax, bx, cx, dx, bp, si, di, ds, es, flags;
};

struct ind {
	int indis;
	byte point;
};

struct chariot {
	int val,
		code,
		acc,
		freq,
		rep;
};

/*
struct doublet {
	byte x, y;
};
*/

struct rectangle {
	int x1, x2, y1, y2;
	bool enabled;
};

struct pattern {
	byte tay, tax;
	byte des[max_patt+1][max_patt+1];
};


struct nhom {
	byte n;     /* numero entre 0 et 32 */
	byte hom[4];
};

typedef nhom t_nhom[16];

struct t_pcga {
	byte p;
	nhom a[16];
};

typedef int tablint[256];
//typedef Common::Point tabdb[17];
typedef int tfxx[108];

/*---------------------------------------------------------------------------*/
/*------------------------------     ENUMS     ------------------------------*/
/*---------------------------------------------------------------------------*/

enum GraphicModes { MODE_AMSTRAD1512 = 0, MODE_CGA = 1, MODE_EGA = 2, MODE_HERCULES = 3, MODE_TANDY = 4 };

/*---------------------------------------------------------------------------*/
/*------------------------------   VARIABLES   ------------------------------*/
/*---------------------------------------------------------------------------*/

extern bool blo,
        bh1,
        bf1,
        bh2,
        bh4,
        bf4,
        bh5,
        bh6,
        bh8,
        bt3,
        bt7,
        bh9,

        _soundOff,
        main1,
        choisi,
        test0,
        f2_all,
        imen,
        cache,
        iesc,
        col,
        syn,
        fouil,
        zuul,
        tesok,
        obpart,
        okdes,
        anyone,
        brt,
        rect,
        rech_cfiec;


extern int x,
        y,
        t,
        vj,
        li,
        vh,
        vm,
        jh,
        mh,
        cs,
        _currGraphicalDevice,
        hdb,
        hfb,
        _hour,
        _day,
        key,
        _minute,
        num,
        max,
        res,
        ment,
        haut,
        caff,
        _maff,
        crep,
        ades,
        iouv,
        inei,
        ctrm,
        dobj,
        msg3,
        msg4,
        mlec,
        _newGraphicalDevice,
        c_zzz,
        mchai,
        menup,
        ipers,
        ledeb,
        lefin,
        mpers,
        mnumo,
        xprec,
        yprec,
        perdep,
        prebru,
        typlec,
        num_ph,
        numpal,
        lderobj,
        nb_word,
        ptr_oct,
        k_tempo,
        ptr_tcph,
        ptr_word,
        color_txt;

extern int t_cph[6401];
extern byte tabdon[4001];

extern Common::String _hintPctMessage;      // gives the pct of hints found
extern byte is;
extern byte mode;

extern int nbrep[9];
extern int nbrepm[9];
extern int msg[5];
extern byte touv[8];
extern sav_chaine s, s1;
extern byte bufcha[391];

extern byte lettres[7][24];

extern byte palher[16];

extern uint16 t_mot[maxti + 1];
extern int tay_tchar;
extern ind t_rec[maxtd + 1];
//file<ind> sauv_t;
//untyped_file fibyte;
extern byte v_lieu[8][25];
extern int l[108];
extern int tbi[256];
extern chariot c1, c2, c3;
extern float addfix;
extern t_pcga palsav[91];
extern Common::Point tabpal[91][17];
extern t_pcga palcga[91];
extern pattern tpt[15];

extern byte adcfiec[822 * 128];

/*---------------------------------------------------------------------------*/
/*-------------------   PROCEDURES  AND  FONCTIONS   ------------------------*/
/*---------------------------------------------------------------------------*/

void hirs();
Common::String copy(const Common::String &s, int idx, size_t size);
void Val(const Common::String &s, int &V, int Code);

/*---------------------------------------------------------------------------*/
/*------------------------------     STUBS     ------------------------------*/
/*---------------------------------------------------------------------------*/
// TODO: Replace the following with proper implementations, or refactor out the code using them

extern int port[0xfff];
extern byte mem[65536 * 16];

#define hires {}

// Stubs for input functions
extern bool keypressed();
extern char get_ch();

extern void palette(int v1);
extern void intr(int intNum, registres &regs);
extern int get_random_number(int minval, int maxval);

// (* external 'c:\mc\charecr.com'; *)
extern void s_char(int Gd, int y, int dy);
// (* external 'c:\mc\sauvecr.com'; *)
extern void s_sauv(int Gd, int y, int dy);

} // End of namespace Mortevielle

#endif
