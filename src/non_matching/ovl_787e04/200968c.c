/* OvlFunc_887_200968c (0x0200968c) -- NON-MATCHING, 12 encodings of 133 differ, SAME LENGTH
 * (ref 133 encodings; ours identical length, all relocations identical).
 *
 * asm/overlays/rom_787e04/ovl_30_c_c_a_a_c_c.s (1 function).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_787e04/200968c.c asm/overlays/rom_787e04/ovl_30_c_c_a_a_c_c.s --func OvlFunc_887_200968c
 *
 * FAMILY: a near-twin of OvlFunc_884_200a440 (identical but for the
 * leading __PlaySound) and of the parked OvlFunc_883_200dd68 / OvlFunc_882_200c41c
 * (src/non_matching/ovl_780898/200dd68.c, ovl_77dd1c/200c41c.c), whose "CHAIN" residue
 * this is: the ROM writes a->f55 / a->f64 through ONE address register,
 *     mov r3,r0 / add r3,#0x55 / strb r2,[r3] / add r3,#0xf / strh r2,[r3]
 * The tail here differs from those parks (f50->f9 bits 2-3 := 2 on both actors, and
 * the second block wants the bitfield BEFORE f6c; the first block after it).
 *
 * THE CHAIN IS MOVE2ADD (reload1.c:8840 reload_cse_move2add), NOT cse. It turns
 * "mov rX,r0 / add rX,#0x64" into "add rX,#0xf" when rX still holds r0+0x55 -- so it
 * needs BOTH address pseudos in the SAME hard register.
 *
 * WHAT THIS CANDIDATE SOLVES (vs. the 200dd68 park's two exclusive routes): the walk
 * spelling gets the chain, and a BLKmode struct at the walk pointer keeps the SI zero
 * shared by the strb and strh (the batch-287 "BLKmode members share one SImode zero"
 * rule) -- a plain u8* / u16* walk instead binds the strh to the pooled HImode zero
 * (79 differ).  Instruction count now equals the ROM's; what is left is r2/r3 swapped
 * on the walk pointer and the zero, plus one-slot schedule shifts that follow.
 *
 * THE BLOCKER, read from .17.lreg / .18.greg: store_fixed_bit_field (the BLKmode field
 * store) leaves a REG_UNUSED "(set (reg:QI) (const_int 0))"; local-alloc gives that dead
 * QI r3 (1-insn life, top priority), and the SI zero also gets r3 locally (no overlap
 * with the dead QI).  The walk pointer p is GLOBAL -- two deaths; and a+0x55 on its own
 * prefers STACK_REG (the add needs its tied alternative, LO costs 8, CLASS_LIKELY_SPILLED,
 * local-alloc.c:362) -- and conflicts with r3 through both, so global gives it r2.  The
 * ROM has p in r3 and the zero in r2.  Same "dead QImode zero steals an address register"
 * as
 * batch 287's Func_809b450 park.
 *
 * INERT: all 24 orders of {f14 copy, s = f50, walk, f68}; declaring p at 6 positions;
 * p[0]/p+0xf/(ZH*)(p+0xf) spellings (16); an int zero local in the walk (loop.c hoists
 * it: 300 bytes); a->f64 = a->f55 = 0 (17); u8 store + BLK u16 store (21);
 * BLK u8 store + plain u16 store (12, same).
 * Progression: direct fields 23 -> tail order (block 0: f6c then bitfield; block 1:
 * bitfield then f6c) 17 -> BLKmode walk 12.
 *
 * NEXT: a spelling whose f55 store has no dead QI but still shares the SI zero with the
 * f64 strh -- or one that makes the walk pointer LOCAL (one death, non-STACK_REG pref), so
 * local-alloc places it before the dead QI.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Spr9 {
    unsigned char pad[9];
    unsigned char a : 2;
    unsigned char b : 2;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

/* BLKmode (3 and 6 bytes) on purpose -- see the header. */
struct ZB { unsigned char b; unsigned char c[2]; };
struct ZH { unsigned short h; unsigned char c[4]; };

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void __PlaySound(int id);
extern void OvlFunc_887_2009638(void);
extern void OvlFunc_887_20095e8(void);

void OvlFunc_887_200968c(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    unsigned char *p;
    int i;

    w = iwram_3001f30;
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            p = &a->f55;
            ((struct ZB *)p)->b = 0;
            p += 0xf;
            ((struct ZH *)p)->h = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    arr[0]->f6c = OvlFunc_887_2009638;
    ((struct Spr9 *)arr[0]->f50)->b = 2;
    ((struct Spr9 *)arr[1]->f50)->b = 2;
    arr[1]->f6c = OvlFunc_887_20095e8;
    arr[1]->f23 = 2;
}
