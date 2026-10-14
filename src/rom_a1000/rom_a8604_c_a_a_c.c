/* Func_80a96d8 -- 0x080a96d8  [rom_a1000]  *** LANDS: 0 of 319. ***
 * Batch 349 brief C.  WAS PARKED AT 2 AND DECLARED "TERMINAL AT 2" FOR FOUR
 * BATCHES (321, 328, 329, 336).  The park's verdict is REFUTED; its blocker
 * class was wrong; and the lever was sitting in its own recorded-negative list
 * as TWO SEPARATE "exactly inert" rows that had never been crossed.
 *
 * THE FOUR NUMBERS, re-derived first, not inherited:
 *   base park body   2 differing encodings of 319  (273 instructions + 46
 *                    32-bit entries), size 728/728, 42 relocations, first at
 *                    index 44.  So the 2 was a real distance.
 *   this body        objcmp --func : "OK Func_80a96d8 -- 728 bytes, 319
 *                    encodings and 42 relocations identical"
 *                    objcmp --whole: "OK whole file" (the .s holds this
 *                    function alone).
 *
 * Verify with -- NAMES THE INSTALLED PATH:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a8604_c_a_a_c.c \
 *     asm/rom_a1000/rom_a8604_c_a_a_c.s --whole
 *
 * SPLIT SHAPE: NONE.  tools/piece.py -> rom_a8604_c_a_a_c.s, 1 function, all 1
 * parked, no exported data.  tools/datacheck.py exits 0 silently.
 * tools/split_s.py --dry-run: "holds only Func_80a96d8 and no data; convert it
 * directly, no split needed."  stage1.ld:1390 names the .o.
 *
 * ===========================================================================
 * THE EDIT IS TWO LINES, AND NEITHER IS NEW TO THE PARK
 * ===========================================================================
 *   (1) `int y; y = 0x18;` before StopTask, with `-y` at both _Func_801e7c0
 *       calls.  The park recorded this THREE TIMES as "2 inert".
 *   (2) the existing bare `__asm__ volatile ("")` gains a clobber list.
 *       The park recorded `__asm__ volatile ("" ::: "r5")` as "2 inert" too,
 *       and even proposed it by name as an escape it had ruled out.
 * Each alone is BIT-IDENTICAL TO THE BASE OBJECT.  Together they match.
 * `y = 0x18` then `-y` is `-0x18`; the program is unchanged.
 *
 * ===========================================================================
 * THE MECHANISM, WITH FILE AND LINE.  IT IS **combine**, NOT sched2, AND NOT
 * gcse's cprop.  THE PARK NAMED BOTH WRONG.
 * ===========================================================================
 * Dumps A/B, identical bodies differing ONLY in the asm's colon section
 * (scratch_elev/b349/C/dmp/{bare,mem}).  Pseudo 47 is `y`:
 *   .12.life   `(reg/v:SI 47)` present in BOTH  (3 occurrences each)
 *   .13.combine  bare: 0 occurrences   clobber form: 3    <-- DIVERGES HERE
 *   .17.lreg   clobber form: "Register 47 used 2 times across 16 insns in
 *              block 4; set 1 time; user var; crosses 1 call; pref LO_REGS."
 *              and then *** ";; Register 47 in 6." ***  -- r6 is exactly the
 *              register the ROM uses for the shared -0x18 argument.
 *   .18.greg   allocno 47 appears in the conflict lists of 44, 61 and 62 in
 *              the clobber build and NOWHERE in the bare build.
 *
 * WHY THE COLON SECTION DECIDES IT, read in the compiler and not recalled:
 *   combine.c:1183-1188 rejects a combination if ANY insn between i2 and i3
 *   satisfies `volatile_insn_p (PATTERN (p))`:
 *       for (p = NEXT_INSN (insn); p != i3; p = NEXT_INSN (p))
 *         if (GET_RTX_CLASS (GET_CODE (p)) == 'i'
 *             && p != succ && volatile_insn_p (PATTERN (p)))
 *           return 0;
 *   and `volatile_insn_p` (rtlanal.c:1619) answers by RTX CODE:
 *       rtlanal.c:1637   case ASM_INPUT:     ... return 0;
 *       rtlanal.c:1648   case ASM_OPERANDS:  if (MEM_VOLATILE_P (x)) return 1;
 *   A bare `__asm__ volatile ("")` has NO colon section, so the C front end
 *   routes it to `expand_asm` and its pattern is a plain **ASM_INPUT** --
 *   `volatile_insn_p` returns 0, combine walks straight across it, propagates
 *   `(const_int 24)` into the `neg`, and deletes pseudo 47.  Reload then has to
 *   materialise 0x18 itself, as the park-documented insn pair 1133/1134, placed
 *   immediately before the call that needs it -- which is where its LUID comes
 *   from and therefore where the whole rung-4/rung-5 argument came from.
 *   ANY colon section routes it to `expand_asm_operands` instead, so the
 *   pattern contains an **ASM_OPERANDS** with MEM_VOLATILE_P set,
 *   `volatile_insn_p` returns 1, combine refuses, `y` survives as a real
 *   pseudo, local-alloc hands it r6, and there is no scheduling contest left.
 *
 * *** SO THE PARK'S CENTRAL CLAIM IS FALSE AS WRITTEN. ***  It reads:
 *     "a volatile asm does not stop gcse's cprop from folding a named integer
 *      constant back to an immediate"
 *   Two errors.  The fold is **combine's**, not cprop's -- pseudo 47 is alive
 *   in .12.life in both builds.  And a volatile asm DOES stop it, provided its
 *   pattern is ASM_OPERANDS.  The park only ever tested the ASM_INPUT form
 *   against a named `y`, and only ever tested the ASM_OPERANDS form WITHOUT
 *   one.  The cross it never ran is the landing.
 *
 * ===========================================================================
 * THE DEVICE LADDER, MEASURED IN FULL.  ALL THREE FIGURES, FOR PASS 3.
 * ===========================================================================
 *   no asm at all, `y` named ............................. 8 of 319  RELOCDIFF
 *   bare `__asm__ volatile ("")`, ASM_INPUT  (the park) .. 2 of 319
 *   ANY ASM_OPERANDS form, `y` named ..................... *** 0 ***
 * and the ASM_OPERANDS forms are ALL ONE PROGRAM -- bit-identical objects:
 *   ::: "memory"   ::: "cc"   ::: "r0" "r1" "r2" "r3" "r4" "r5" "r7"
 *   :: "i" (0)     :: "r" (msg)   :: "r" (y)   : "+r" (msg)
 * So the clobber's CONTENT is irrelevant; only the EXISTENCE of a colon
 * section is.  Two exceptions, both explained:
 *   ::: "r6" ... 271 of 319 -- r6 is the register `y` must land in.
 *   no `y`   ... 2, bit-identical to base, for every one of those forms.
 *
 * WHY `::: "memory"` AND NOT `:: "i" (0)`:  `:: "i" (0)` is an OPEN OWNER
 * QUESTION (docs/owner-decisions.md, "Is `__asm__ volatile ("" :: "i"(0))`
 * admissible as a landed device?") -- it appears in 13 files, "every one of
 * them a park", and has never shipped.  `::: "memory"` HAS shipped, in four
 * landed files, each with a fakematch.txt row:
 *   src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_b.c  OvlFunc_968_2009808
 *   src/overlays/common/common1_a_c_c_b.c                 OvlFunc_common1_1578
 *   src/overlays/rom_794ac0/ovl_30_a_c_c_c_c_c_b.c        OvlFunc_899_200aba0
 *   src/rom_9000/rom_11ce0_c_a_a_b.c                      Func_8011fd8
 * Since the forms are bit-identical, the choice is pure bookkeeping and pass 3
 * may swap freely.
 *
 * DEVICE-FREE WAS TRIED FIRST, per docs/owner-decisions.md decision 5.  It does
 * not reach it:
 *   `do { } while (0)` anchor replacing the asm, `y` named ...... 2  identical
 *   anchor around `StopTask(...)` / around `y =` + StopTask ..... 2  identical
 *   anchor BEFORE and anchor AFTER the bare barrier ............. 2  identical
 *   anchor around `msg = 0xb06;` ............................... 5  WORSE
 *   a SECOND set of `y` to block combine via REG_N_SETS, five
 *     spellings (`y = 0; y = 0x18;` adjacent / split across
 *     StopTask / a dead store after the calls / `0x18` twice) ... 2  all
 *                                     BIT-IDENTICAL to base; combine's
 *                                     get_last_value still finds the 24.
 *   those two spellings with NO asm ............................. 8  RELOCDIFF
 *   `volatile int y;` .......................................... 275 of 319
 * The anchor cannot substitute here because an anchor is a SCHEDULING object
 * (haifa-sched.c:3744-3757) and this residue is a COMBINE object.  That is the
 * first case in this tree where the batch-337 anchor-for-barrier substitution
 * provably does NOT apply, and the reason is exactly `volatile_insn_p`: a loop
 * note is not an insn pattern at all, so combine never consults it.
 *
 * PINS / FAKEMATCH: tools/shimcount.py SEES THIS BODY AND BOOKS IT:
 *     asm + memory  : 1  (PARALLEL -- fences combine, NOT cse; fakematch-class)
 *     *** has a fakematch-class shim and NO fakematch.txt row
 * so this landing needs a fakematch.txt row and the manifest says
 * `pins: 1, fakematch: true`.  shimcount's own label independently confirms the
 * mechanism above -- the tree already knew this form "fences combine"; what is
 * new here is the file:line (`combine.c:1183-1188` via `rtlanal.c:1637/1648`)
 * and the fact that it is the form's ONLY relevant property on this function.
 *
 * *** AND A SHIMCOUNT BLIND SPOT FOUND WHILE CHOOSING THE FORM, worth fixing:
 * of the ASM_OPERANDS forms that all produce this same object, shimcount counts
 * `::: "memory"` ("asm + memory") and `:: "i"(0)` / `: "+r"(x)` ("asm +
 * operand"), but counts `::: "cc"` and `::: "r5"` -- and by inspection any
 * `::: "rN"` -- under NO CLASS AT ALL, printing zero rows.  Its CLOBBER regex
 * (tools/shimcount.py) is literally
 *     __asm__\s*volatile\s*\(\s*""\s*:{3}\s*"memory"
 * i.e. the token `"memory"` is hard-coded.  So a body shipping a REGISTER
 * clobber -- strictly worse than a memory clobber, since it names a hard
 * register -- reads as 0 pins / 0 devices.  There are already 3 occurrences of
 * `::: "r5"` in src/.  This is a third blind spot alongside the two
 * docs/owner-decisions.md decision 3 names.
 *
 * EVERYTHING THE PARK LISTED AS LOAD-BEARING IS UNCHANGED AND STILL IS:
 * iwram_3001f2c as a struct; `short buf[15]` declared before `int ctx[7]`
 * (FRAME_GROWS_DOWNWARD); the dead one-line `t` temp; `msg = 0xb06` with
 * `msg + 3`; `n = 0xc80` named; `c = 0xf0` named; f14 as `struct W *`.
 */
struct W {
    unsigned char pad00[5];
    unsigned char f5;
};

struct Unit {
    unsigned char pad00[0xd8];
    unsigned short items[15];
};

struct State {
    unsigned char pad00[8];
    unsigned int f08;
    unsigned char pad0c[0x14 - 0x0c];
    struct W *f14;
    unsigned char pad18[0x1c - 0x18];
    signed char f1c;
    unsigned char pad1d[0x24 - 0x1d];
    unsigned int f24;
    unsigned char pad28[0x2c - 0x28];
    unsigned int f2c;
    unsigned int f30;
    unsigned char pad34[0x48 - 0x34];
    unsigned char *objs[32];
    unsigned char padc8[0x10c - 0xc8];
    unsigned int f10c;
    unsigned char pad110[0x1c8 - 0x110];
    unsigned short list[(0x208 - 0x1c8) / 2];
    unsigned short arr[(0x218 - 0x208) / 2];
    unsigned char f218;
    unsigned char f219;
    unsigned char f21a;
    unsigned char pad21b[0x260 - 0x21b];
    unsigned char f260[4];
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern struct Unit *_GetUnit(int id);
extern int Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern void Func_80a9cbc(void);
extern void Func_80a19a0(void);
extern void StopTask(void *task);
extern void StartTask(void *task, int priority);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void WaitFrames(int n);
extern void Func_80a33d4(struct State *s, unsigned int a);
extern int _GetFlag(int id);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern int Func_80a3ddc(struct Unit *unit, unsigned short *p, int f);
extern void Func_80a1e38(unsigned short *p, int f);
extern int Func_80a8b8c(int *dest, int cursor);
extern void Func_80a9a5c(unsigned int win, unsigned int sel, int a);
extern void Func_80a3e28(unsigned short *p, int f);
extern void Func_80a9598(unsigned int win, int a, int *ctx);
extern void Func_80a93a4(unsigned int win, int a, int *ctx);
extern void Func_80a1a40(int a, int b);
extern void _PlaySound(int id);
extern int Func_80a1fd4(int swap, int total, int cols, int *col, int *row);
extern void Func_80a1804(struct State *s, unsigned int id);
extern void Func_80a345c(void);

int Func_80a96d8(void)
{
    short buf[15];
    int ctx[7];
    struct State *s;
    struct Unit *u;
    unsigned char **q;
    unsigned char *x;
    int ret;
    int done;
    int redraw;
    int full;
    int r;
    int i;
    int cur;
    int n;
    int msg;
    int c;
    int t;
    int y;

    s = iwram_3001f2c;
    t = s->f21a;
    ret = 0;
    _GetUnit(s->arr[t]);
    Func_80a10d0(&s->f30, 0, 0xa, 0xf, 0xa, 2);
    Func_80a9cbc();
    c = 0xf0;
    q = s->objs;
    i = 0x1f;
    do {
        x = *q++;
        if (x != 0)
            x[0xf] = c;
        i--;
    } while (i >= 0);
    y = 0x18;
    StopTask(Func_80a19a0);
    __asm__ volatile ("" ::: "memory");
    msg = 0xb06;
    _Func_801e7c0(msg, s->f24, 0x40, -y);
    _Func_801e7c0(msg + 3, s->f24, 0, -y);
    Func_80a9cbc();
    WaitFrames(1);
    Func_80a33d4(s, s->f10c);
    done = 0;
    while (1) {
        if (done == 0 && _GetFlag(0x150) == 0) {
            Func_80a9cbc();
            _Func_8016498(s->f24);
            u = _GetUnit(s->f21a);
            i = 0;
        } else {
            break;
        }
        while (i <= 0xe) {
            buf[i] = u->items[i];
            i++;
        }
        s->f218 = Func_80a3ddc(u, s->list, 0);
        Func_80a1e38(s->list, 0);
        Func_80a8b8c(ctx, 0);
        _Func_8016498(s->f30);
        Func_80a9a5c(s->f30, s->f21a, 1);
        WaitFrames(1);
        Func_80a3e28(s->list, 0);
        redraw = 1;
        full = 1;
        while (_GetFlag(0x150) == 0) {
            if (redraw != 0) {
                _Func_80164ac(s->f2c);
                redraw = 0;
                if (full != 0) {
                    full = 0;
                    Func_80a9598(s->f24, 0, ctx);
                }
                Func_80a93a4(s->f24, 0, ctx);
            }
            s->f14->f5 = 1;
            Func_80a1a40(0x60, ctx[4] * 16 + 0x34);
            WaitFrames(1);
            r = Func_80a1fd4(0, ctx[5], 5, &ctx[4], &ctx[2]);
            if (r == 1) {
                full = 1;
                redraw = 1;
            }
            if (r == 0)
                redraw = 1;
            if (r == -1)
                redraw = 0;
            if ((gKeyPress & 1) != 0) {
                _PlaySound(0x70);
                ret = 1;
                done = 1;
                break;
            }
            if ((gKeyPress & 2) != 0) {
                _PlaySound(0x71);
                ret = -1;
                done = 1;
                break;
            }
            if ((gKeyRepeat & 0x100) != 0 || (gKeyRepeat & 0x200) != 0) {
                _PlaySound(0x6f);
                cur = s->f1c;
                s->f260[s->arr[cur]] = ctx[6];
                if ((gKeyRepeat & 0x100) != 0)
                    cur++;
                else
                    cur--;
                for (i = 0; i <= 0xe; i++)
                    u->items[i] = buf[i];
                n = s->f219;
                cur = (cur + n) % n;
                s->f08 = s->arr[cur];
                s->f21a = s->arr[cur];
                s->f1c = cur;
                Func_80a1804(s, s->arr[cur]);
                break;
            }
        }
    }
    _Func_80164ac(s->f2c);
    _Func_8016498(s->f2c);
    _Func_80164ac(s->f10c);
    Func_80a345c();
    _Func_8016498(s->f24);
    n = 0xc80;
    StartTask(Func_80a19a0, n);
    return ret;
}
