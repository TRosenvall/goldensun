/* Func_8099678 -- 0x08099678
 *
 * Reads the terrain byte under the party leader and stamps a state word unless
 * it is the "blocked" value. Which of two tile grids is consulted depends on
 * the map mode: a 32x32 grid at 1/0x200000 scale in mode 3, otherwise a
 * 128-wide grid at 1/0x100000 taken from a three-slot pointer table hanging off
 * the derived global, falling back to gBuffer.
 *
 * THIS FUNCTION IS WHY tools/objcmp.py EXISTS, and it is the clearest case yet.
 * Writing the final store as a cast -- `*(short *)(p + (0xbf << 1)) = 0x2092;`
 * -- SCREENS CLEAN ON tryc.py. It is wrong: gcc emits `ldrh r3, .L12` and
 * hoists 0x2092 to the FRONT of the literal pool, ahead of gState, so every
 * relocation shifts by four and twelve encodings differ. tryc normalises
 * PC-relative loads to `=value` and cannot see pool order. Only the object-level
 * check catches it, and without that check this would have gone into the build
 * and turned it red.
 *
 * THE TYPED STRUCT FIELD IS THE FIX, for the third batch running, and here the
 * reason is specifically pool ORDER rather than an immediate-versus-pool
 * choice: `p->f17e = 0x2092` with the map typed as `struct Map *` puts the
 * SImode pool entry in the reference's order at no register cost. Two
 * alternatives were measured and both fail differently -- a named `int`
 * intermediate fixes the pool order but COSTS A REGISTER, transposing r1 and r2
 * through the prologue (12 differing); reusing an existing local for it is
 * worse still, adding a whole callee-saved register to the push mask.
 *
 * A NAMED BYTE OFFSET BLOCKS REASSOCIATION -- the only real defect in the first
 * draft (41 differing). `off = a->f22 * 48 + (0x98 << 1); t = *(T **)(q + off);`
 * gives the ROM's register-offset `ldr r5, [r5, r3]`; written as one expression
 * gcc folds the base in early and emits an extra `add` plus a zero-displacement
 * load.
 *
 * Two more on file: the second global is DERIVED from the first at a fixed
 * distance off one pooled address (`(char *)&iwram_3001ebc - 0x4c`), which is
 * the only spelling giving the ROM's `sub r5, #0x4c`; and both scalings are
 * SIGNED DIVISIONS, not shifts -- `cmp / bge / ldr =0x1fffff / add / asr` is
 * `/ 0x200000`.
 *
 * The same-family sibling src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_c_b.c is very
 * nearly this function's twin and is where the declarations came from.
 *
 * Verified with tools/objcmp.py: 192 bytes, 84 encodings and 5 relocations
 * identical.
 */
extern unsigned char gState[];
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

struct Map {
    unsigned char pad000[0x17e];
    short f17e;
    unsigned char pad180[0x1e];
    short f19e;
};

extern struct Map *iwram_3001ebc;

struct Actor {
    unsigned char pad00[8];
    int f08;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
};

extern struct Actor *GetFieldActor(int id);

void Func_8099678(void)
{
    unsigned char *g;
    struct Map *p;
    unsigned char *q;
    unsigned char *t;
    struct Actor *a;
    int off;

    g = gState;
    p = iwram_3001ebc;
    a = GetFieldActor(*(int *)(g + (0xfa << 1)));
    q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
    if (p->f19e == 3) {
        t = ewram_2020000 + (((a->f08 / 0x200000) & 0x1f) + (((a->f10 / 0x200000) & 0x1f) << 5)) * 4;
    } else {
        if (a->f22 <= 2) {
            off = a->f22 * 48 + (0x98 << 1);
            t = *(unsigned char **)(q + off);
        } else {
            t = gBuffer;
        }
        t += ((a->f08 / 0x100000) + ((a->f10 / 0x100000) << 7)) * 4;
    }
    if (t[2] != 0xfb)
        p->f17e = 0x2092;
}
