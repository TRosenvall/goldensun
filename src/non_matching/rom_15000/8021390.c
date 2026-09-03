/* Func_8021390  --  0x08021390
 * asm/rom_15000/rom_20198_c_c_c_a_a_a_c.s, line 6.
 *
 * PARKED at 36 aligned regions of 97 (ours 102 lines, ROM 97).
 *
 * BLOCKER CLASS: the dominance contradiction -- a constant with one dominating
 * use and one guarded use, where the ROM rebuilds it and gcc hoists. Third
 * recorded instance, after OvlFunc_952_200be40 and OvlFunc_891_2008098, and the
 * first where the constant is ZERO and where the hoist is provably NOT CSE or
 * PRE: twelve flags, -fno-gcse and -fno-rerun-cse-after-loop among them, are
 * byte-identical.
 *
 * THE WHOLE RESIDUE IS ONE THING. The `0` passed as CreateUIBox's fifth
 * argument and the five `0`s in the guarded body are ONE pseudo for gcc and TWO
 * for the ROM. The ROM materialises `mov r2, #0 / str r2, [sp]` for the call
 * and then a FRESH `mov r6, #0` after the `bl`; gcc commons them into a single
 * pseudo whose live range starts at function entry -- the longest of the four
 * call-crossing values -- so it loses the allocno race, lands in r8, and every
 * one of its five uses pays a `mov rN, r8` copy first. Five instructions, plus
 * a four-way register rotation:
 *
 *      rom    r5=p  r6=zero  r7=&buf  r8=box
 *      ours   r5=p  r6=&buf  r7=box   r8=zero
 *
 * MEASURED BY CONSTRUCTION, which is the part worth keeping. Change nothing but
 * the pre-call argument and gcc's whole allocation flips to the ROM's:
 *
 *      h = f5(2,1,0x1a,5,0);  if (h==0) return;  ... five zeros in the body ...
 *          -> `mov r7, #0` at ENTRY, `str r7,[sp]`, r7 serves the whole body
 *
 *      h = f5(2,1,0x1a,5,9);  ... the same body ...
 *          -> `mov r3,#9 / str r3,[sp]`, then `mov r5, #0` INSIDE the guard
 *
 * So the two-materialisation shape is reachable whenever the pre-call constant
 * differs from the body's, and unreachable when they are equal. This is the
 * batch-182 split lever meeting a case it cannot serve: the split is about
 * VARIABLES, and two source variables both holding 0 are folded back to one
 * const_int by constant propagation before allocation ever runs. Every attempt
 * confirmed it -- a named zero at five different assignment positions, two
 * separate named zeros in initialiser and assignment form, and the zero typed
 * char / unsigned char / short / unsigned short / register int all measured
 * EXACTLY 36.
 *
 * TWO LEVERS DID FIRE and are worth recording on their own:
 *
 *   - The pooled-small-constant tell holds at 0x1b. The ROM has `ldr r0, =0x1b`
 *     where `mov r0, #0x1b` would do, and `(int)&_MSG_1b` is worth exactly one
 *     instruction (36 against 37). `_MSG_1b` is NOT yet in message.sym; it is
 *     left out deliberately, because a symbol that closes nothing should not
 *     enter the namespace until the function that needs it lands.
 *
 *   - A named pointer to the stack vector is required AND must be born early.
 *     `int *q = buf;` as the first statement gives the ROM's `add r7, sp, #0x10`
 *     BEFORE the first call; indexing buf directly sinks the add to the store
 *     site and costs 14.
 *
 * THE CEILING IF THE ZERO WERE SOLVED is 11 differing of 97, measured on a
 * diagnostic build with the shared zero forced non-constant. Six of those 11
 * are the missing `mov r6, #0` and the entry-region reshuffle it drags along.
 * The other TWO are a separate, smaller thing worth its own note: the ROM
 * writes buf[1] and buf[2] as `str r3, [sp, #0x14]` / `[sp, #0x18]` while gcc
 * rewrites them to `[r7, #4]` / `[r7, #8]` through the live pointer. Insensitive
 * to spelling -- q[1], buf[1], *(buf+1), struct members and three scalar locals
 * all give the same -- and the sibling Func_8021488 has the identical shape
 * twice, so it is a family property rather than noise.
 *
 * FAMILY NOTE, and it is actionable. Func_8021488 (same .s) and Func_8021228
 * (asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c.s) are the same routine with
 * different entries. Func_8021488 has this function's exact two-materialisation
 * shape and will hit the same wall. Func_8021228 will NOT: its ROM shows
 * `mov r0, #0 / mov r9, r0 / mov r10, r0`, which is one materialisation with two
 * live copies -- exactly what gcc-2.96 emits from two source variables. Attack
 * that one.
 *
 * Screened with tools/tryc.py --align; 20+ source spellings and 12 flags
 * measured. Not built.
 */
extern unsigned char *iwram_3001e8c;
extern volatile int gKeyPress;

extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(void *box, int b);
extern void Func_801e41c(void *box, int b, int c, int d, int e);
extern int Func_8021360(unsigned int i);
extern int GetPortrait(int id);
extern void LoadPortrait(int id, int b, int *c, int *d, int e, int f);
extern void Func_8019908(int a, int b);
extern int Func_8019ba0(int id);
extern int _MSG_1b;
extern void Func_80165d8(void *box, int b, int c, int d, int e);
extern void _PlaySound(int id);
extern void Func_8003dec(int *p, int n);
extern void WaitFrames(int n);
extern int _Func_80f954c(void);
extern void Func_8003f3c(int h);

void Func_8021390(int a)
{
    unsigned char *p;
    void *box;
    int buf[3];
    int *q;
    int v;
    int t;

    q = buf;
    p = iwram_3001e8c;
    box = CreateUIBox(2, 1, 0x1a, 5, 0);
    if (box == 0)
        return;
    Func_801e41c(box, 4, 0, 4, 4);
    p[0xea3] = 1;
    LoadPortrait(GetPortrait(Func_8021360(a)), 0, &v, &t, 0xe, 0);
    q[0] = 0;
    q[1] = 0x8014000c;
    q[2] = t | 0xe000;
    *(short *)(p + 0x12f4) = 0;
    *(short *)(p + 0x12f6) = 0;
    Func_8019908(a, 1);
    Func_80165d8(box, Func_8019ba0((int)&_MSG_1b), 0x24, 2, 0);
    _PlaySound(0x51);
    do {
        Func_8003dec(q, 0xfa);
        WaitFrames(1);
    } while (_Func_80f954c() != 0 && (gKeyPress & 0x303) == 0);
    CloseUIBox(box, 2);
    WaitFrames(1);
    Func_8003f3c(v);
}
