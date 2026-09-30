/* Func_80bd898 -- NON-MATCHING, 737 of 838 encodings differ.
 * Reference asm/rom_b5000/rom_bbb0c_a_c_a_a_c_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/80bd898.c \
 *       asm/rom_b5000/rom_bbb0c_a_c_a_a_c_c.s --func Func_80bd898
 *
 * FIRST ATTEMPT (batch 307, brief F).  794 instructions, screened CLEAN for the
 * inline `.call_via` macro (0 sites).
 *
 * SIZE AND COUNT ARE **NOT** EXACT: ref 1876 bytes / 838 encodings, ours 1852 /
 * 828 -- ten instructions SHORT.  So 737 SATURATES and CANNOT RANK; every
 * candidate below was ranked by tools/aligncmp.py instead.
 *   aligncmp: 505 aligned-equal, 60.3% of ref, 404 differing/ins/del in 136 hunks.
 *
 * NO SPLIT NEEDED -- Func_80bd898 is the SOLE function in
 * asm/rom_b5000/rom_bbb0c_a_c_a_a_c_c.s and the file carries no data section, so
 * it converts whole.  Confirmed: the only .thumb_func_start/.func_end pair in the
 * file is this function's (lines 8 and 908).
 *
 * PIN-FREE.  tools/shimcount.py reports nothing at all.
 *
 * ================= THE PROGRAM SHAPE IS CONFIRMED CORRECT =================
 *
 * THE RELOCATION SYMBOL SEQUENCE MATCHES THE ROM EXACTLY except for pool
 * PLACEMENT.  86 relocations in the ROM, 85 in ours, and a positional diff of the
 * two symbol sequences is empty apart from the trailing pool block:
 *   ref  ... Func_80bac6c GetBattleActor Func_80b7f70 ... _Func_800be70 x4
 *            THEN iwram_3001e40 gKeyHeld iwram_3001af8 iwram_3001800
 *   ours ... Func_80bac6c GetBattleActor
 *            THEN gKeyHeld iwram_3001af8 iwram_3001800  (mid-function)
 *            THEN Func_80b7f70 ... _Func_800be70 x4
 * That is the cheapest confirmation available that the reconstruction is the right
 * program: every call, in the ROM's order, with the ROM's operands.  Read the
 * SEQUENCE separately from the offsets, per batch 298.
 *
 * ONE MISSING POOL WORD: the ROM spends TWO pool words on iwram_3001e40 (one at
 * 0x3f4, one at 0x730) because gcc flushes a mid-function pool between the two
 * reads -- the ROM's own `.pool` sits between them, at the .Lbdc84/.Lbdc88 block
 * holding 0x3ff and 0x1ff.  We read it twice too but get one pool word.  That is a
 * size-and-count defect (4 bytes, 1 encoding) and it CASCADES into every later
 * pc-relative offset, which is why so many `ldr rN,[pc,#K]` read as differing.
 * It is a CONSEQUENCE of being 24 bytes short, not an independent defect -- close
 * the size gap and the pool split should land on its own.  Do not chase it first.
 *
 * ===================== LEVERS THAT PAID, IN ORDER, WITH FIGURES ===============
 *
 * (0) BASELINE, plain struct-free `char *` + casts: 773 of 838, 60.0% aligned,
 *     150 hunks, and the relocation sequence ALREADY nearly right.
 *
 * (1) SWITCH ARM ORDER IS SOURCE ORDER -- worth 3 (773 -> 770).
 *     gcc emits jump-table case BODIES where they appear in the source, so the
 *     ROM's body order PUBLISHES the source's arm order.  Read off the label
 *     sequence after the table, it is
 *         14, 13, 0, 1, 2, 3, 6, 4, 5, 7, 12, 8, 9, 10, 11
 *     -- cases 14 and 13 come FIRST, then an almost-ascending run with 6 spliced
 *     between 3 and 4 and 12 between 7 and 8.  Writing the arms in that order is
 *     free and it is the first thing to do on any jump-table function: the
 *     reference tells you the answer.
 *
 * (2) REG_WINOUT IS AN ABSOLUTE, NOT A RELOCATED SYMBOL -- removed a spurious
 *     relocation.  The ROM's pool holds `=REG_WINOUT` but objcmp shows NO
 *     relocation for it, because include/gba.inc `.set`s it to 0x0400004A and the
 *     reference resolves it inside the TU.  Declaring `extern int REG_WINOUT`
 *     manufactures an R_ARM_ABS32 the ROM does not have.  Use the io.h form --
 *     `(*(vu16 *)0x0400004a)` -- and pass its address.  GENERAL RULE: a hardware
 *     register named in a pool is an absolute; only iwram_* and gData symbols relocate.
 *
 * (3) THE UNLIKELY ARM IS AN `else`, NOT AN EARLY `continue` -- part of 770 -> 752.
 *     In the state==1 arm the ROM branches FORWARD past the whole body
 *     (`cmp r3,r2 / bge .Lbd918`) to a three-instruction `state = 4` block placed
 *     AFTER it.  An early-exit `if (cond) { state = 4; continue; }` inverts that:
 *     gcc emits the store inline and branches `blt` over it.  Writing it as
 *     `if (i < limit) { body } else { state = 4; }` puts the store where the ROM
 *     has it.  A guard and its inversion are NOT interchangeable at -O2; the arm
 *     that is emitted last is the one written last.
 *
 * (4) THE COUNTDOWN LOOP WITH A WALKING POINTER -- worth 18 and 10 HUNKS
 *     (755 -> 737, 146 -> 136 hunks).  MEASURED BOTH WAYS.
 *     The ROM's two list loops are
 *         subs r6,#1 / ldmia r5!,{r0} / ... / cmp r6,#0 / bne
 *     i.e. a DECREMENTING trip count and a genuinely walking pointer.  An
 *     ascending `for (k = 0; k < n; k++) f(list[k])` measures 755/146 hunks; the
 *     countdown `p = list; m = n; do { f(*p++); m--; } while (m);` measures
 *     737/136.  THIS IS THE ONE PLACE LEVER 7 DOES NOT APPLY: `ldmia rX!,{rY}`
 *     alone does not prove a walking pointer, but `ldmia` TOGETHER WITH a
 *     `subs/cmp #0/bne` countdown does -- the counter is the tell, not the load,
 *     because a strength-reduced subscript keeps an ASCENDING compare against the
 *     bound.  Check the loop-control insn before deciding.
 *
 * (5) `iwram_3001e40` IS UNSIGNED -- the ROM shifts it with `lsrs r3,r3,#2`,
 *     ours emitted `asrs`.  One encoding, and it is a TYPE fact worth recording
 *     for every other reader of that global: declare it `unsigned int`.
 *
 * (6) AN `int` INTERMEDIATE HOLDS THE LOAD WIDTH.  Writing the bob sum straight
 *     into the byte store `*(u8 *)(o + 4) = ... *(u16 *)(w + 0xe) ...` lets gcc
 *     narrow the FEEDING loads to QImode -- ours emitted `ldrb r3,[r1,#14]` where
 *     the ROM has `ldrh r3,[r4,#14]`.  Assigning to an `int y` first and storing
 *     `y` keeps the halfword loads.  This is batch 306's narrowing rule in its
 *     most direct form: the store's width reaches back through the whole
 *     expression unless a declared int interrupts it.
 *
 * ===================== A REAL RECONSTRUCTION BUG, FOUND BY THE HUNKS ==========
 *
 * The four _Func_800be70 calls do NOT take d, d-0x13, d-0x12, d-0x11.  The ROM
 * derives all four from x = timer << 2:
 *     mov r1,#0x14 / neg r1,r1 / add r1,r3   -> x - 0x14   (== d)
 *     mov r4,#0x13 / neg r4,r4 / add r4,r3   -> x - 0x13
 *     mov r1,#0x12 / neg r1,r1 / add r1,r3   -> x - 0x12
 *     sub r3,#0x11                           -> x - 0x11
 * so they are d, d+1, d+2, d+3.  My first reading subtracted the constants from
 * `d` instead of from `x`, which compiled to `subs r3,#37` where the ROM has
 * `subs r3,#17` -- visible in ONE hunk and nowhere else.  THE LESSON: when a ROM
 * derives several constants from one held register, the SOURCE shares that
 * register's value as a named subexpression; re-deriving them from an already
 * offset variable is a DIFFERENT PROGRAM that still compiles and still measures
 * plausibly.  A hunk showing the same insn with a different immediate is the
 * cheapest bug detector in the toolchain -- read the immediates, not just the
 * opcodes.
 *
 * ===================== MEASURED INERT -- UNTESTED, NOT DISPROVED ==============
 *
 * Each of these compiled and measured EXACTLY 737 / 136 hunks / 60.3%, i.e. byte
 * -for-byte identical output to the candidate below.  They are recorded so nobody
 * spends the builds again, not as evidence about the mechanism:
 *   * LEVER 4 (the offset as a named local) on `h + 0x160`, which the ROM holds in
 *     r6 across three uses in the state==5 block: `int *pg = (int *)(h + 0x160)`
 *     -- IDENTICAL OUTPUT.  The precondition (more than one use) is satisfied and
 *     it still did nothing, so on THIS function the address is already being
 *     commoned and the lever has no count to change.
 *   * An `int g` intermediate for the UploadSprite2 result before the o+8 mask.
 *   * Hoisting the twice-dereferenced iwram_3001ee4 base into a local `wb`.
 *
 * NOT MEASURABLE AS WRITTEN: brace-scoping `j` per region (lever 3) is a C89
 * error here -- the loop body opens with the timer `if`, so `int j = i;` after it
 * is a declaration after a statement and gcc-2.96 rejects it.  To probe lever 3
 * the declaration has to open its own nested block.
 *
 * ===================== THE BLOCKER, AND WHAT RULES OUT THE ALTERNATIVES =======
 *
 * TWO PSEUDOS WHERE WE GET ONE, at the inner loop's index.  The ROM emits
 * `adds r6,r5,#0` TWICE -- once in the loop preheader and once at the loop bottom
 * (ref[79:81] and ref[311:320], the latter an 8-instruction hunk and the single
 * largest residue in the function):
 *     ldr r5,[r3]        ; i = h->idx      (r3 = h + 0x14c, its own address reg)
 *     ldr r3,[r2]        ; count           (r2 = h + 0x144, its own address reg)
 *     cmp r5,r3 / blt
 *     adds r6,r5,#0      ; j = i
 * We get ONE register: `ldr r6,[r3] / cmp r6,r3`, with no copy at all.
 *
 * ATTRIBUTED TO COPY COALESCING (regmove / local-alloc), not to scheduling and not
 * to declaration order:
 *   * It is NOT sched2.  sched1 DOES NOT RUN in this build (verified in batch 305
 *     with -da), and a missing COPY INSTRUCTION is a count difference, which no
 *     scheduler can create or destroy.
 *   * It is NOT the spelling of the index.  Writing the two quantities as two
 *     declared variables -- `i = h->idx` cached in the guard and `j = i` inside
 *     the body -- is exactly what landed lever 1's inverse elsewhere, and it DID
 *     pay (770 -> 752) by fixing the surrounding block, but the copy STILL does
 *     not appear: gcc coalesces `j = i` because `i` is DEAD at the copy (the loop
 *     bottom re-reads h->idx from memory rather than using `i`), so the two
 *     pseudos do not interfere and coalescing is legal.
 *   * THIS IS LEVER 1 RUN BACKWARDS, and it is the harder direction.  Collapsing
 *     two variables into one is something a spelling can always do; FORCING two
 *     that gcc is entitled to coalesce is only possible by making their live
 *     ranges OVERLAP.  For that, `i` needs a use AFTER the copy -- and in the ROM
 *     it has one: r5 is live across the count compare AND the state load before
 *     the copy is emitted.  The next probe is therefore to keep `i` live past the
 *     switch, e.g. by using `i` (not a reload of h->idx) in the loop-bottom
 *     increment, so that the copy's source is still needed when the copy happens.
 *     I did not get to measure that; it is the single highest-value open probe.
 *   * The ROM also builds the two field ADDRESSES into two separate registers
 *     (`adds r3,r7,r4` and `adds r2,r7,r1`, with 0x14c and 0x144 materialised
 *     `mov/lsl` into r4 and r1) before either load.  Ours builds one.  That is
 *     two more allocnos, and it is consistent with the same cause: the ROM's
 *     source holds these as named quantities and ours lets gcc fold them.
 *
 * SECONDARY RESIDUE, also allocation: the high registers carry different roles in
 * the state==5 and state==0xb blocks -- ref `mov r1,r9 / mov r4,sl` against ours
 * `mov r1,fp / mov r2,sl / mov r4,r8`, i.e. a ROTATION among r8/r9/sl/fp rather
 * than a wrong count (batch 301: a high-register rotation is ONE EXTRA ALLOCNO,
 * and is not an ordering problem).  Worth attacking only after the index copy,
 * since closing that changes the allocno set these compete in.
 *
 * WHAT THIS FUNCTION IS.  A battle-HUD animation driver.  It runs a small state
 * machine (states 1, 2, 3/0xd, 5, 0xa, 0xb, 4=done) over a command list held in
 * the battle-state block at +0x6b8: a byte array of 15 opcodes at +0x00 and an
 * int operand array at +0x40, walked by the index at +0x14c against the count at
 * +0x144.  State 5 draws the action cursor, bobbing it with sin(), and polls the
 * keys to skip; state 0xa flashes a target list; state 0xb runs the actor's
 * attack animation and fades the sprites through _Func_800be70.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern int iwram_3001e74;
extern unsigned int iwram_3001e40;
extern int iwram_3001ee4;
extern int iwram_3001af8;
extern int iwram_3001800;
extern int gKeyHeld;
extern const u8 Data_c3734[];
#define REG_WINOUT (*(volatile unsigned short *)0x0400004a)

extern void Func_80bbb0c(void *p, int n);
extern void _PlaySound(int id);
extern void Func_80bb928(void *h, int a);
extern void _Func_8019908(int a, int b);
extern void _PrintBattleText(int a);
extern void _Func_80198dc(void);
extern void Func_80bb8e8(int a);
extern int *GetBattleActor(int a);
extern void _Actor_SetAnim(int a, int b);
extern void Func_80c24f0(int a, int b);
extern void Func_80bb588(int a);
extern char *_GetUnit(int a);
extern void _Sprite_SetAnim(int a, int b);
extern int Func_80b7f70(int a, int b);
extern void _Func_801f200(int a);
extern void Func_80b78e4(int a, int *b);
extern int Func_80b6cd0(int a);
extern void Func_80ba918(int a, int b);
extern void Func_80b7aac(int a);
extern int _Func_8017364(void);
extern void Func_80039fc(volatile unsigned short *p, int v);
extern void Func_800393c(volatile unsigned short *p, int v);
extern int UploadSprite2(int a, const u8 *g);
extern int sin(int a);
extern void Func_8003dec(void *o, int v);
extern void _Func_802281c(void *p);
extern int Func_80c2368(int a);
extern int __modsi3(int a, int b);
extern void Func_80bac6c(int a);
extern void Func_80b7e60(int a);
extern void Func_80bd850(int a, int b, void *e);
extern void _Func_800be70(int a, int b);

void Func_80bd898(void)
{
    char *s;
    char *h;
    int i;
    int j;
    int anim;
    int t;
    int n;
    int k;
    int v;
    int *p;
    int m;
    int x;
    int y;
    int a;
    int d;
    int list[8];
    u16 tgt[2];

    s = (char *)iwram_3001e74;
    h = s + 0x6b8;
    if (*(int *)(s + 0x800) == 0)
        return;

    while (1) {
        if (*(int *)(h + 0x148) == 4)
            return;

        if (*(int *)(h + 0x148) == 1) {
            if (*(int *)(h + 0x140) < *(signed char *)(s + 0x655)) {
                *(int *)(h + 0x144) = 0;
                *(int *)(h + 0x14c) = 0;
                *(int *)(h + 0x150) = 0;
                Func_80bbb0c(s + 0x654, *(int *)(h + 0x140));
                *(int *)(h + 0x140) = *(int *)(h + 0x140) + 1;
                *(int *)(h + 0x148) = 2;
            } else {
                *(int *)(h + 0x148) = 4;
            }
            continue;
        }

        if (*(int *)(h + 0x148) == 2) {
            i = *(int *)(h + 0x14c);
            while (i < *(int *)(h + 0x144)) {
                if (*(int *)(h + 0x150) != 0) {
                    *(int *)(h + 0x150) = *(int *)(h + 0x150) - 1;
                    return;
                }
                j = i;
                switch (*(u8 *)(h + j)) {
                case 14:
                    _PlaySound(*(int *)(h + 0x40 + j * 4));
                    break;
                case 13:
                    Func_80bb928(h, *(int *)(h + 0x40 + j * 4));
                    break;
                case 0:
                    _Func_8019908(*(int *)(h + 0x40 + j * 4), 1);
                    break;
                case 1:
                    _Func_8019908(*(int *)(h + 0x40 + j * 4), 5);
                    break;
                case 2:
                    _Func_8019908(*(int *)(h + 0x40 + j * 4) & 0x1ff, 2);
                    break;
                case 3:
                    _Func_8019908(*(int *)(h + 0x40 + j * 4) & 0x3fff, 4);
                    break;
                case 6:
                    *(int *)(*(int *)iwram_3001ee4 + 8) = 1;
                    break;
                case 4:
                    if (*(int *)(h + 0x40 + j * 4) >= 0)
                        _PrintBattleText(*(int *)(h + 0x40 + j * 4));
                    *(int *)(h + 0x148) = 3;
                    iwram_3001af8 = 0;
                    break;
                case 5:
                    if (*(int *)(h + 0x40 + j * 4) >= 0)
                        _PrintBattleText(*(int *)(h + 0x40 + j * 4));
                    *(int *)(h + 0x148) = 0xd;
                    break;
                case 7:
                    _Func_80198dc();
                    break;
                case 12:
                    Func_80bb8e8(*(int *)(h + 0x40 + j * 4));
                    break;
                case 8:
                    if (*(int *)(h + 0x168) > 0)
                        _PlaySound(*(int *)(h + 0x168));
                    a = *(int *)(h + 0x40 + j * 4);
                    *(int *)(h + 0x164) = a;
                    _Actor_SetAnim(*GetBattleActor(a), 5);
                    *(int *)(h + 0x148) = 0xa;
                    *(int *)(h + 0x150) = 0;
                    break;
                case 9:
                    *(int *)(h + 0x164) = *(int *)(h + 0x40 + j * 4);
                    Func_80c24f0(*(int *)(h + 0x40 + j * 4), *(int *)(h + 0x16c));
                    Func_80bb588(*(int *)(h + 0x164));
                    {
                        char *u = _GetUnit(*(int *)(h + 0x164));
                        k = 0;
                        while ((v = Func_80b7f70(*GetBattleActor(*(int *)(h + 0x164)), k)) != 0) {
                            if (*(u8 *)(u + 0x12a) == 1)
                                _Sprite_SetAnim(v, 5);
                            else
                                _Sprite_SetAnim(v, 4);
                            k++;
                        }
                        if (*(u8 *)(u + 0x12a) == 1) {
                            *(int *)(h + 0x148) = 0xb;
                            *(int *)(h + 0x150) = v;
                        }
                    }
                    break;
                case 10:
                    _Func_801f200(*(u8 *)((char *)iwram_3001e74 + 0x41));
                    break;
                case 11:
                    Func_80b78e4(*(int *)(h + 0x40 + j * 4),
                                 GetBattleActor(*(int *)(h + 0x40 + j * 4)));
                    Func_80ba918(*GetBattleActor(*(int *)(h + 0x40 + j * 4)),
                                 Func_80b6cd0(*(int *)(h + 0x40 + j * 4)));
                    Func_80b7aac(*(int *)(h + 0x40 + j * 4));
                    break;
                }
                *(int *)(h + 0x14c) = *(int *)(h + 0x14c) + 1;
                i = *(int *)(h + 0x14c);
                if (*(int *)(h + 0x148) != 2)
                    break;
            }
            if (*(int *)(h + 0x148) == 2)
                *(int *)(h + 0x148) = 1;
            continue;
        }

        if (*(int *)(h + 0x148) == 3 || *(int *)(h + 0x148) == 0xd) {
            if (_Func_8017364() == 0)
                return;
            if (*(int *)(h + 0x148) == 0xd) {
                *(int *)(h + 0x148) = 2;
                *(int *)(h + 0x150) = 0;
            } else {
                *(int *)(h + 0x148) = 5;
                *(int *)(h + 0x160) = -1;
                *(int *)(h + 0x150) = iwram_3001800;
            }
            continue;
        }

        if (*(int *)(h + 0x148) == 5) {
            const u8 *gfx;
            char *o;
            char *w;
            char *q;
            int x;

            gfx = Data_c3734 + ((iwram_3001e40 >> 2 & 7) << 7);
            o = h + 0x154;
            w = *(char **)(*(int *)iwram_3001ee4);
            q = *(char **)(*(int *)iwram_3001ee4 + 4);
            if (*(int *)(h + 0x160) == -1)
                *(int *)(h + 0x160) = *(int *)(s + 0x54);
            _Func_80198dc();
            Func_80039fc(&REG_WINOUT, 4);
            Func_800393c(&REG_WINOUT, 0x10);
            *(int *)(o + 4) = 0xa000;
            *(int *)(o + 8) = 0;
            *(u16 *)(o + 8) = (*(u16 *)(o + 8) & 0xfffffc00)
                | (UploadSprite2(*(int *)(h + 0x160), gfx) & 0x3ff);
            x = (*(u16 *)(w + 0xc) << 3) + (*(u16 *)(q + 4) >> 8) + 4;
            *(u16 *)(o + 6) = (*(u16 *)(o + 6) & 0xfffffe00) | (x & 0x1ff);
            y = (sin(iwram_3001e40 << 12) / 0x8000)
                + (*(u16 *)(w + 0xe) << 3) + (*(u16 *)(q + 6) >> 8) + 6;
            *(u8 *)(o + 4) = y;
            if ((gKeyHeld & 2) != 0
                || (iwram_3001af8 & 0x303) != 0
                || ((unsigned int)(iwram_3001800 - *(int *)(h + 0x150)) > 0xa
                    && (gKeyHeld & 0x303) != 0)) {
                _PlaySound(0x6f);
                *(int *)(h + 0x148) = 2;
                *(int *)(h + 0x150) = 0;
                continue;
            }
            Func_8003dec(o, 0xf0);
            return;
        }

        if (*(int *)(h + 0x148) == 0xa) {
            t = *(int *)(h + 0x150);
            if ((t & 1) != 0) {
                if ((t & 2) != 0) {
                    tgt[0] = 0xff;
                    Func_80ba918(*GetBattleActor(*(int *)(h + 0x164)),
                                 Func_80b6cd0(*(int *)(h + 0x164)));
                } else {
                    tgt[0] = *(int *)(h + 0x164);
                    tgt[1] = 0xff;
                    Func_80ba918(*GetBattleActor(*(int *)(h + 0x164)), 7);
                }
                _Func_802281c(tgt);
            }
            *(int *)(h + 0x150) = *(int *)(h + 0x150) + 1;
            if (*(int *)(h + 0x150) > 8) {
                *(int *)(h + 0x148) = 2;
                *(int *)(h + 0x150) = 0;
                continue;
            }
            return;
        }

        if (*(int *)(h + 0x148) != 0xb)
            continue;

        t = *(int *)(h + 0x150);
        if (t == 0 || t >= 0x400) {
            anim = 6;
            if (t == 0 && *(int *)(h + 0x16c) != 0) {
                v = Func_80c2368(*(u8 *)(_GetUnit(*(int *)(h + 0x164)) + 0x128));
                if (v >= 0) {
                    v--;
                    if (v < 0)
                        v = 0;
                    _PlaySound(v + 0x92);
                }
                *(int *)(h + 0x150) = 0x400;
            }
            t = *(int *)(h + 0x150);
            if (t > 0x41d)
                *(int *)(h + 0x150) = 0;
            if (t == 0) {
                v = Func_80c2368(*(u8 *)(_GetUnit(*(int *)(h + 0x164)) + 0x128));
                if (v >= 0)
                    _PlaySound(v + 0x92);
            }
            if (*(int *)(h + 0x150) >= 0x400)
                anim = __modsi3((*(int *)(h + 0x150) - 0x400) / 8, 5) + 1;
            if (anim == 6 || (*(int *)(h + 0x150) & 7) == 0) {
                k = 0;
                while ((v = Func_80b7f70(*GetBattleActor(*(int *)(h + 0x164)), k)) != 0) {
                    char *ob = *(char **)(v + 0x28);
                    int m = *(u8 *)(ob + 0x16) | 0xff;
                    list[k] = v;
                    *(u8 *)(ob + 5) = anim;
                    *(u8 *)(ob + 0x16) = m;
                    k++;
                }
            }
        } else if (t == 4) {
            Func_80bac6c(*(int *)(h + 0x164));
            *(int *)(h + 0x150) = *(int *)(h + 0x150) + 1;
            return;
        } else if (t > 4) {
            int *ap = GetBattleActor(*(int *)(h + 0x164));
            *(u16 *)((char *)ap + 0x2a) = 1;
            n = 0;
            while ((v = Func_80b7f70(*ap, n)) != 0) {
                list[n] = v;
                n++;
            }
            x = *(int *)(h + 0x150) << 2;
            d = x - 0x14;
            if (d > 0x7f) {
                if (n > 0) {
                    p = list;
                    m = n;
                    do {
                        Func_80bd850(*p++, 0, &list[8]);
                        m--;
                    } while (m != 0);
                }
                Func_80b7e60(*(int *)(h + 0x164));
                *(int *)(h + 0x148) = 2;
                *(int *)(h + 0x150) = 0;
                return;
            }
            if (n > 0) {
                p = list;
                m = n;
                do {
                    _Func_800be70(*p, d);
                    _Func_800be70(*p, x - 0x13);
                    _Func_800be70(*p, x - 0x12);
                    m--;
                    _Func_800be70(*p++, x - 0x11);
                } while (m != 0);
            }
        } else {
            *(int *)(h + 0x150) = *(int *)(h + 0x150) + 1;
            return;
        }
        *(int *)(h + 0x150) = *(int *)(h + 0x150) + 1;
        return;
    }
}
