/* CutsceneStart -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_8a000/rom_91584_a_c_a_c_c_c.s
 * Best screen: 60 instructions against the ROM's 64, 52 differing.
 *
 * THE INSTRUCTION FORMS ARE ALL RIGHT AND THE RESIDUE IS TWO THINGS.
 *
 * 1. FOUR INSTRUCTIONS SHORT, because gcc derives three of the field offsets
 *    from each other instead of rebuilding them:
 *
 *        rom    mov r3, #0xee / lsl r3, #1 / add r2, r6, r3
 *        ours   add r1, #0x10 / add r2, r6, r1
 *
 *    0x1cc, 0x1da, 0x1dc and 0x1de are close together, so gcc keeps one in a
 *    register and adjusts it. The ROM rebuilds each with its own `mov`/`lsl`.
 *    This is the constant-reuse family that also blocks the 1000+ instruction
 *    park src/non_matching/overlays/200b4c8.c, and nothing reaches it here:
 *    five separate `int` offset locals assigned at the top of the function are
 *    byte-identical (gcc constant-propagates them straight back), and
 *    -fno-rerun-cse-after-loop is byte-identical too.
 *
 * 2. A REGISTER-NAMING CASCADE from the second instruction. Once the forms
 *    matched, the whole body is the same sequence with r1/r2/r3 assigned
 *    differently -- the r2/r3 exchange class again, which is why 52 lines
 *    "differ" from a handful of causes.
 *
 * FOUR THINGS WERE SOLVED GETTING HERE and all four generalise.
 *
 * THE ROM'S THREE-OPERAND `add` MEANS A NAMED POINTER, at eight sites. Written
 * as `*(short *)(p + off) = z;` with `off` a variable, gcc emits the
 * register-offset form `strh r5, [r6, r3]` -- one instruction where the ROM has
 * two. `s = (short *)(p + off); *s = z;` gives the ROM's
 * `add r3, r6, r2 / strh r5, [r3]`. This is the rule in docs/elevation.md read
 * in the direction it is usually needed.
 *
 * 0xffff NEEDS AN `unsigned short *`. Three consecutive halfword stores write
 * 0xffff, -1 and -1. Through a `short *`, the 0xffff converts to -1 and gcc
 * merges all three into one pool load of 0xffffffff -- the ROM has
 * `ldr r3, =0xffff` for the first and `mov r3, #1 / neg r3, r3` for the other
 * two, which is two different constants. Typing the first store's pointer
 * `unsigned short *` keeps them apart.
 *
 * THE TWO -1s ARE REBUILT, not carried: the ROM builds `mov r3, #1 / neg r3, r3`
 * twice. Separate locals in a dominating block is the batch-107 rule, and it
 * does fix the values -- it just costs three instructions elsewhere here
 * (63 lines, 58 differing), so this file keeps the literals.
 *
 * `gState` IS AN ABSOLUTE SYMBOL (`wram.sym`: `gState = 0x02000240`), so a
 * reference to it can canonicalise as either `=gState` or `=0x2000240`
 * depending on whether gcc emits a relocation or the folded value. tryc counts
 * that as a differing line. Worth knowing before chasing it: four spellings of
 * the read were tried and all four are the same 52.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void _Func_801c428(void);
extern void Func_8091660(void);
extern void Func_808e118(void);
extern void Task_Cutscene(void);
extern void StartTask(void *fn, int prio);
extern void _ClearFlag(int id);

void CutsceneStart(void)
{
    char *p;
    short *s;
    unsigned short *u;
    unsigned char *g;
    int *w;
    int z;
    int o;
    int k;
    int prio;

    p = iwram_3001ebc;
    prio = 0xc8 << 4;
    _Func_801c428();
    Func_8091660();
    s = (short *)(p + 0xcb6);
    if (*s != 0)
        Func_808e118();
    z = 0;
    o = 0xcc2;
    s = (short *)(p + o);
    *s = z;
    o += 2;
    s = (short *)(p + o);
    *s = z;
    w = (int *)(p + (0xe4 << 1));
    *w = 0x10;
    w = (int *)(p + (0xe6 << 1));
    *w = z;
    u = (unsigned short *)(p + (0xed << 1));
    *u = 0xffff;
    s = (short *)(p + (0xee << 1));
    *s = -1;
    s = (short *)(p + (0xef << 1));
    *s = -1;
    StartTask(Task_Cutscene, prio);
    _ClearFlag(0x99 << 1);
    g = gState;
    k = 0xfa << 1;
    *(int *)(p + k) = *(int *)(g + k);
    k += 4;
    w = (int *)(p + k);
    *w = z;
}
