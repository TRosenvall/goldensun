/* OvlFunc_969_200b6d0 -- 0x0200b6d0,
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_c.s (1 function, so landing needs NO split).
 *
 * NOT MATCHING: ONE INSTRUCTION SHORT -- 244 bytes against 240, 103 encodings against 101 -- and
 * it MUST be built with -fno-rerun-cse-after-loop (CSE_CFLAGS). Candidate below.
 *
 * WITHOUT THE FLAG it is 89 differing; with it, 49 and then two residues.
 *
 * THE FLAG IS NOT A NOVEL COST. This is the established flag-id constant-CSE class that the
 * CSE_CFLAGS group exists for (Makefile:414-470). Both `__GetFlag(0x236)` calls are bare literals;
 * at -O2 the second CSE pass hoists 0x236 into r6 and keeps it live across three calls where the
 * ROM re-loads the pool word. Confirmed PASS-level rather than source-level, exactly as
 * Makefile:424-437 records: five structural spellings of the guard (nested `if`, `&&`, `goto`,
 * `else if`) all give IDENTICAL output at 55 differing. -fno-cse-skip-blocks also fixes it;
 * -fno-gcse is inert; -fno-expensive-optimizations and -fno-cse-follow-jumps do not.
 *
 * RESIDUE 1, and it is the interesting one: a MISSING `mov r0, r7` before
 * `ldr r6, [r7, #0x50] / bl __Actor_SetScript`. Reload's find_equiv_reg / reload_cse DELETES the
 * copy because r0 still holds the __CreateActor result. That is exactly the recorded
 * "a one-instruction residue at a call-result copy is a pressure symptom" class from
 * Func_80b153c -- and the 4-byte deficit is what forces the ROM's extra `.short 0x0000` pool pad,
 * so it is the ONLY size difference.
 *
 * NINE SPELLINGS MEASURED, ALL IDENTICAL AT 49: `if (q != 0) {}` against an early return against
 * `goto have;` with the label at the body start; __CreateActor returning `int` with a cast; two
 * pointer variables; `&gScript[0]`; `e = q;` after the call. Hoisting a zero to the top is WORSE
 * (88/96 lines, 75-96 differing).
 *
 * The single-exit restructure that fixed Func_80b153c's version of this is what to try next, and
 * it was NOT among the nine.
 *
 * RESIDUE 2: a three-instruction rotation, ROM `ldr r1, [r5, #8] / ldr r3, [r5, #0x10] /
 * lsl r0, #1` against ours with the shift first. `0x11c` as a plain literal instead of
 * `0x8e << 1` is inert.
 *
 * TWO THINGS WORTH CARRYING:
 *   - the `.L37a4: .word 0` is NOT a symbol. `unsigned short z = 0;` byte-stored gives gcc's
 *     `ldrh r2, .L10 / .word 0` and reproduces it exactly -- the
 *     src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_c.c mechanism, second specimen. Here it COEXISTS
 *     with a plain `int zero = 0` used for another byte store and a halfword store in the same
 *     function, which is the clearest demonstration yet that the two forms are different tools.
 *   - THE STATEMENT-BOUNDARY LEVER (docs/elevation.md:12587) produces the ROM's
 *     `mov r2, r0 / lsl r2, #8`: `t = __Random(); t <<= 8;` as two statements, then
 *     `t >>= 16; t <<= 16; t += e->f0c; t += 0xffe40000;`. One expression is 55 differing;
 *     all-compound is 49 and every shift and add in the chain lands.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[0x50 - 0x34];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    short f66;
    unsigned char pad68[0x6c - 0x68];
    void *f6c;
};

extern unsigned int iwram_3001e40;
extern unsigned char gScript_969__0200e16c[];

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Func_80929d8(struct Actor *a, int n);
extern int __sin(int a);
extern void OvlFunc_969_200b600(struct Actor *a);

void OvlFunc_969_200b6d0(void)
{
    struct Actor *e;
    struct Actor *q;
    struct Sprite *s;
    unsigned int t;
    unsigned int r;
    int v;
    int zero;
    unsigned short z;

    if (__GetFlag(0x236) == 0) {
        if (iwram_3001e40 % 3 != 0)
            return;
    }
    e = __MapActor_GetActor(0x18);
    if (__GetFlag(0x236) != 0) {
        t = __Random();
        t <<= 8;
    } else {
        t = __Random();
        t <<= 6;
    }
    t >>= 16;
    t <<= 16;
    t += e->f0c;
    t += 0xffe40000;
    q = __CreateActor(0x8e << 1, e->f08, t, e->f10);
    if (q != 0) {
        s = q->f50;
        __Actor_SetScript(q, gScript_969__0200e16c);
        __Func_80929d8(q, 1);
        zero = 0;
        q->f55 = zero;
        v = 0xffff000 & __Random();
        *(short *)((char *)q + 0x64) = v;
        *(short *)((char *)q + 0x66) = zero;
        q->f6c = OvlFunc_969_200b600;
        z = 0;
        r = __Random();
        q->f30 = (__sin((r * 0xffff) >> 20) * 24) >> 16;
        s->f26 = z;
        s->b2 = 1;
    }
}
