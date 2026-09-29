/* Func_80a35f8 -- 0x080a35f8, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a.s.
 *
 * EXACT.  objcmp --func Func_80a35f8, verbatim:
 *   OK Func_80a35f8 -- 688 bytes, 308 encodings and 32 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a.s --func Func_80a35f8
 *
 * THE SPLIT: the .s holds TWO functions, Func_80a355c (still parked, see
 * src/non_matching/rom_a1000/80a355c.c) and this one, and NO data --
 * tools/datacheck.py reports nothing.  A plain code/code split with ZERO
 * exports: `.thumb_func_start` already emits `.global`, and the C function is
 * non-static, so Func_80a355c's `bl Func_80a35f8` resolves across the halves
 * with no exports.s change.  Shape:
 *
 *   asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_a.s   Func_80a355c  (stays asm)
 *   src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_b.c   Func_80a35f8  (this file)
 *
 * stage1.ld line 1271 becomes those two .o lines in that order.  The two
 * mid-function literal pools and the `b` that jumps over each are reproduced.
 *
 * WHAT IT IS: the party-strip picker on the item screen.  Opens the two
 * windows at state+0x20 and state+0x28, then loops on the d-pad until save bit
 * 0x150 is set, redrawing the selected member's panel through Func_80a112c and
 * returning the chosen item id, -1 on B, or 0 on the flag.  BOTH PARAMETERS ARE
 * DEAD in the ROM -- r1 is clobbered before it is read, and r0 is only ever
 * used as the roster base -- so `list` is declared and unused on purpose;
 * Func_80a355c passes state+0x1c8 there.
 *
 * ================= THE FIVE LEVERS, IN THE ORDER THEY PAID =================
 * First draft read 119 of 308 differing (aligncmp 72.1%) and was 3 instructions
 * long with a 28-byte frame against the ROM's 32.
 *
 *  1. gKeyPress, gKeyHeld AND gKeyRepeat ARE `volatile`.  This was worth more
 *     than everything else combined: 96 differing -> 30, and it fixed the frame
 *     and the spill map as a side effect.  The tell is the pair at .La37f0 /
 *     .La3808 -- two `ldr r3, [r6]` off one address register with NO call
 *     between them on the fall-through path.  Already established in this bank
 *     (80a5388.c, 80a63e4.c, 80ab314.c); I re-derived it the slow way, so:
 *     ON ANY rom_a1000 FUNCTION THAT READS A KEY GLOBAL TWICE, DECLARE IT
 *     VOLATILE IN THE FIRST DRAFT.
 *
 *  2. DO NOT GIVE `sel * 2` A NAME.  The ROM computes it at the loop head into
 *     r7, copies it to r9, and RE-COMPUTES it on the loop-exit edge
 *     (`.La3872: mov r1, r8 / lsl r7, r1, #1`).  That is gcse's PRE: the
 *     expression is available on the two `goto`-out paths (sel unchanged) and
 *     NOT on the normal exit (sel may have been stepped), so gcc inserts the
 *     computation on that edge and a `reaching_reg` copy at the definition.
 *     An `idx2` variable produces neither -- it also lowered register pressure
 *     enough that `roster` stayed in fp instead of spilling, which is where the
 *     28-byte frame came from.  Writing `sel * 2` inline at all six sites:
 *     96 -> 89 differing AND the frame and `str r0, [sp, #28]` came right.
 *     A THIRD-PARTY READING OF THE SAME FINDING: r9 in the ROM is not a
 *     variable, it is PRE's temp; do not look for a source-level second copy.
 *
 *  3. A SIGNED-CHAR LOAD IS ALWAYS reg+reg, BUT ONLY IF NOTHING SPILLS INTO
 *     THE ADDRESS.  Thumb-1 has no `ldrsb rD, [rN, #imm]`, so
 *     *thumb_extendqisi2 prints `ldrsb` only when the PLUS's second operand is
 *     a REG and otherwise falls back to `ldrb` + `lsl #24` + `asr #24`.  A bare
 *     `*(signed char *)(st + 0x1e)` compiled to the three-instruction form here
 *     while a probe of the same expression in isolation gave the ROM's two.
 *     Naming the offset (`o = 0x1e; *(signed char *)((int)st + o)`) forces
 *     reg+reg unconditionally: 112 -> (with lever 2) 89.
 *
 *  4. STATEMENT ORDER DECIDES WHICH REGISTER HOLDS THE ZERO.  Moving
 *     `redraw = 1;` from after the `_GetUnit` call to BEFORE it, together with
 *     reading state+0x1c before state+0x1e, took 30 differing to 8 and moved
 *     the first divergence from index 15 to index 32.  The mechanism is the one
 *     80a1a40.c records: the ROM frees r1 by moving `sel` to r8 and then uses
 *     r1 for the `0`, while ours freed r3 by moving `st` to r10 early and used
 *     r3.  Nothing about the zero was edited; only where two other statements
 *     sat.
 *
 *  5. THE LAST THREE INSTRUCTIONS WERE THREE SEPARATE ONE-LINE MOVES:
 *       - the 6th argument of both Func_80a10d0 calls as a LITERAL `2` rather
 *         than a shared `two` local (gcc CSEs it into r7 either way, but the
 *         local puts `mov r7, #2` one slot early): 8 -> 6;
 *       - loading `box` BEFORE `r` in the redraw block rather than after,
 *         which swaps roster and state between r2 and r3: 6 -> 3;
 *       - `(int)roster + sel * 2` written `sel * 2 + (int)roster`, which is the
 *         only one of the three that gcc does NOT canonicalise away: 3 -> 2;
 *       - `j = 3;` hoisted out of the `for` header so the counter is
 *         initialised BEFORE the `q` pointer, which defers `add r3, sl` past
 *         `mov r2, #3`: 2 -> 0.
 *     The `sel * 2 + (int)roster` flip had NO effect when tried three rounds
 *     earlier on a worse base and worked on the last one.  A spelling that
 *     measures neutral is not disproved; it is untested until the structure
 *     around it is right.
 *
 * Everything else fell out: the 0x1e and 0x1a halfword stores pool by
 * themselves (gcc-2.96 has no HImode immediate), `0xa8 << 1` is the house
 * spelling for flag 0x150, and `_GetFlag` at the bottom of a `while` is the
 * ordinary rotated-loop shape.
 *
 * ZERO SHIMS: no `register ... __asm__` declaration in this file.
 */
extern int iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int gKeyRepeat;

extern char *_GetUnit(int id);
extern int Func_80a10d0(void *p, int b, int c, int d, int e, int f);
extern void Func_80a33d4(char *st, unsigned int win);
extern void *_Func_801eb64(int a, int b, void *box, int d, int e);
extern void _Func_801e7c0(int id, void *box, int x, int y);
extern void Func_80a1a40(int x, int y);
extern int Func_80a3ddc(char *unit, unsigned short *p, int f);
extern void Func_80a38a8(int id);
extern void Func_80a112c(void *box, int id, int slot, int mode);
extern void Func_80a3e88(int id, int f);
extern void Func_80a1e38(unsigned short *p, int f);
extern int Func_80a3d6c(int id);
extern void _PlaySound(int sfx);
extern int _GetFlag(int flag);
extern void WaitFrames(int n);

int Func_80a35f8(unsigned short *roster, unsigned short *list)
{
    int cnt;
    int ret;
    char *unit;
    int a;
    int b;
    char *st;
    char *w1;
    char *w2;
    void *box;
    void *p;
    unsigned short *q;
    unsigned short *r;
    int sel;
    int redraw;
    int s;
    int j;
    int v;
    int o;
    int o2;
    int o3;

    st = (char *)iwram_3001f2c;
    o = 0x1c;
    sel = *(signed char *)((int)st + o);
    o2 = 0x1e;
    cnt = *(signed char *)((int)st + o2);
    redraw = 1;
    ret = 0;
    a = 0;
    b = 0;
    unit = _GetUnit(*(unsigned short *)((int)roster + sel * 2));
    w1 = st + 0x20;
    if (Func_80a10d0(w1, 0xd, 3, 0x11, 0xa, 2) != 0)
        Func_80a33d4(st, *(unsigned int *)w1);
    w2 = st + 0x28;
    if (Func_80a10d0(w2, 0xd, 0xd, 0x11, 4, 2) != 0) {
        p = _Func_801eb64(2, 0, *(void **)w2, 0, ret);
        *(void **)(st + 0x87 * 4) = p;
        *((char *)p + 5) = 0xd;
    }
    s = 0xb87;
    _Func_801e7c0(s, *(void **)w2, 0, 0);
    _Func_801e7c0(s + 1, *(void **)w2, 0, 8);
    *(*(char **)(st + 0x14) + 5) = redraw;
    while (_GetFlag(0xa8 * 2) == 0) {
        sel = (sel + cnt) % cnt;
        Func_80a1a40(sel * 24 - 0xa, 0x10);
        if (redraw != 0) {
            a = 0;
            box = *(void **)(st + 0x24);
            r = (unsigned short *)(sel * 2 + (int)roster);
            redraw = 0;
            unit = _GetUnit(*r);
            if (b != 0) {
                *(unsigned char *)(st + 0x86 * 4) =
                    Func_80a3ddc(_GetUnit(*r), (unsigned short *)(st + 0xe4 * 2), 0);
                Func_80a38a8(*r);
                Func_80a112c(box, *r, 0, 8);
            } else {
                Func_80a3e88(*r, 0);
                Func_80a112c(box, *r, 0, 0);
            }
            j = 3;
            q = (unsigned short *)(st + 0xa5 * 2);
            for (; j >= 0; j--)
                *q-- = 0x1e;
            o3 = 0xa2 * 2 + sel * 2;
            *(unsigned short *)((int)st + o3) = 0x1a;
        }
        WaitFrames(1);
        if ((gKeyPress & 1) != 0) {
            if ((gKeyHeld & 0x200) != 0) {
                a = (unsigned char)((a + 4) % 4);
                Func_80a1e38((unsigned short *)(unit + 0xd8), a);
                a = (unsigned char)(a + 1);
                Func_80a3e88(*(unsigned short *)((int)roster + sel * 2), 0);
                _PlaySound(0x70);
            } else {
                r = (unsigned short *)(sel * 2 + (int)roster);
                if (Func_80a3d6c(*r) != 0) {
                    _PlaySound(0x70);
                    ret = *r;
                    goto out;
                }
                _PlaySound(0x72);
            }
        }
        if ((gKeyPress & 2) != 0) {
            _PlaySound(0x71);
            ret = -1;
            goto out;
        }
        if ((gKeyPress & 0x100) != 0) {
            b = 1;
            redraw = 1;
        }
        if ((gKeyHeld & 0x100) == 0 && b == 1) {
            b = 0;
            redraw = 1;
        }
        if ((gKeyRepeat & 0x20) != 0) {
            _PlaySound(0x6f);
            sel--;
            redraw = 1;
        }
        if ((gKeyRepeat & 0x10) != 0) {
            _PlaySound(0x6f);
            sel++;
            redraw = 1;
        }
    }
out:
    st[0x1c] = sel;
    v = *(unsigned short *)((int)roster + sel * 2);
    *(int *)(st + 8) = v;
    st[0x21a] = v;
    return ret;
}
