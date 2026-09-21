/* Cluster OvlFunc_923_2009ec8..OvlFunc_923_2009ec8 extracted from
 * goldensun/asm/overlays/rom_7aa430/ovl_1a3c_a_c_a.s.
 *
 * Total .text for this TU = 288 bytes (= 0x120). Never attempted before batch 278.
 * No pins, no volatile, no flags. 124 instructions; four levers took it 54 -> 40 -> 11 -> 7 -> 0.
 *
 * TWIN: OvlFunc_924_200d458, at src/overlays/rom_7ac2d8/ovl_35b8_a_c_a_c.c. tools/dupfuncs.py
 * paired them; the streams differ in exactly TWO lines, both per-overlay symbols (the script
 * pointer and the called sibling). NO CONSTANT DIFFERS. Each was verified by objcmp against its
 * OWN reference. If you edit one, edit both.
 *
 * 1. THE gState INDEX MUST BE BUILT AT RUNTIME. `gState + (0xfa << 1)` written inline folds to
 *    one `=gState+500` relocation; the iwram_3001edc / -0x20 / `0xfa << 1` statement chain from
 *    the elevated file-mate ovl_1a3c_a_c_c_b.c is what the ROM has. 54 -> 40.
 *
 * 2. `s = n->f50;` MUST BE THE FIRST STATEMENT OF THE SPAWN BLOCK, before the f14 copy. Pure
 *    allocno priority, and the biggest single win: 40 -> 11. `.18.greg`'s order `43 40 60 53 39
 *    41 38 36 ...` is exactly floor_log2(R)*R/L -- `n` 1.286, `a` 0.516, `s` 0.500, `t` 0.429,
 *    `e` 0.342. The ROM needs n,a,t,e in r5..r8 with `s` forced onto CALL-USED r4.
 *
 *    SO THE LEVER IS TO LENGTHEN `s`'s LIVE RANGE, NOT SHORTEN IT. Loading it first drops it
 *    below `t` and the whole order falls into place. That is the batch-277 priority formula run
 *    BACKWARDS -- from the ROM's register assignment to the one variable whose range had to
 *    change -- and it is worth 29 instructions from a one-line statement move. The inverse move
 *    (sprite block relocated to the end) measured 61.
 *
 *    AND THE COROLLARY IS A READING RULE: A CALLER-SAVE `str rN,[sp]` / `ldr rN,[sp]` PAIR IN
 *    THE ROM IS A SPEC, NOT NOISE. It says that allocno is LAST in priority order among the
 *    LO-class contenders, which pins the relative order of all the others. Here it also explains
 *    the ROM's `sub sp, #4`.
 *
 * 3. THE TWO `4`s ARE TWO SEPARATE CONSTANTS. `n[0x55] = 4` and the `| 4` were CSE'd into one
 *    pseudo that survived into allocation in r1; the ROM rematerialises `mov r3, #4`. Wrapping
 *    the FIRST in `{ int v = 4; n[0x55] = v; }` splits them: 11 -> 7. Naming the second instead
 *    is 11; naming both is 11. A named local wrapping ONE of two identical constants splits a
 *    CSE that survives allocation -- and which one you name matters.
 *
 * 4. THE `& -z` MUST ACCUMULATE INTO THE CONSTANT'S REGISTER. `s[9] = (s[9] & -z) | 4;` ties the
 *    `and` output to the loaded byte; the ROM ties it to `-z`. Reusing `z` as its own
 *    accumulator is what picks the other side of the commutative `and`, and no operand
 *    reordering inside the expression reaches it: original 7, `b` named 4, `(-z & b)` 4,
 *    `4 | (...)` 4, `unsigned char b` 4, `unsigned z` 4, `b &= -z` 4, `m = -z; (m & b)` 3,
 *    REUSING `z` 0.
 *
 * Also load-bearing: `n[0x54] = 0` and `s[0x26] = 0` are written as BARE LITERALS, so cse1
 * substitutes the branch-proven zero already in a register -- the ROM's `strb r6,[r3]`. That is
 * the batch-277 zero-substitution rule used deliberately rather than worked around.
 */
extern unsigned char *iwram_3001edc;
extern unsigned char gState[];
extern unsigned char gScript_923__0200a7e8[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void OvlFunc_923_2009df8(void);

void OvlFunc_923_2009ec8(void)
{
    unsigned int r3; unsigned int r1; unsigned int r4;
    unsigned char *q; unsigned char *e; unsigned char *w; unsigned char *t;
    unsigned char *a; unsigned char *n; unsigned char *s;
    unsigned int off; int i;

    r3 = (unsigned int)&iwram_3001edc;
    q = *(unsigned char **)r3;
    r3 -= 0x20;
    e = *(unsigned char **)q;
    r1 = 0xfa;
    w = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r1 <<= 1;
    r4 += r1;
    off = *(unsigned int *)r4;
    off <<= 2;
    off += 0x14;
    t = *(unsigned char **)(w + off);
    if (*(unsigned int *)e > 2)
        return;
    __CutsceneStart();
    a = *(unsigned char **)(e + 0x14);
    if (a == 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8),
                          *(int *)(t + 0xc) + (0xc0 << 13),
                          *(int *)(t + 0x10));
        if (n != 0) {
            s = *(unsigned char **)(n + 0x50);
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            __Actor_SetScript(n, gScript_923__0200a7e8);
            *(unsigned char **)(n + 0x68) = t;
            {
                int v = 4;
                n[0x55] = v;
            }
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                int z = 0xd;
                int b;
                s[0x26] = 0;
                b = s[9];
                z = -z;
                z &= b;
                s[9] = z | 4;
            }
            n[0x54] = 0;
            *(unsigned char **)(e + 0x14) = n;
            a = n;
        } else {
            a = *(unsigned char **)(e + 0x14);
        }
    }
    for (i = *(int *)e; i <= 2; i++) {
        OvlFunc_923_2009df8();
        __WaitFrames(0x1e);
        a[0x54] = 1;
        __Actor_SetAnim(a, 5 - i);
    }
    *(int *)e = 3;
    *(int *)(e + 0xc) = (*(int *)(a + 8) & 0xfff00000) + (0x80 << 12);
    *(int *)(e + 0x10) = (*(int *)(a + 0x10) & 0xfff00000) + (0x80 << 12);
    __SetFlag(0x161);
    __CutsceneEnd();
}
