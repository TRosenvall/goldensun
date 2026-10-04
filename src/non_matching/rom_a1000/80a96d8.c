/* Func_80a96d8 -- 0x080a96d8.  NON-MATCHING, 2 of 319 encodings.
 *
 * Figure RE-MEASURED this batch (brief 321-B), not inherited.  SIZE 728/728,
 * 319 instructions both sides, relocations identical in symbol and order.
 * PIN COUNT 0: tools/shimcount.py reports "empty asm : 1 (NOT treated as a
 * fakematch in this tree)" and nothing else.  The empty `__asm__ volatile ("")`
 * is a scheduling barrier.
 *
 * Verify with -- NAMES THE INSTALLED PATH:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a96d8.c \
 *     asm/rom_a1000/rom_a8604_c_a_a_c.s --func Func_80a96d8
 *     -> XX ENCODINGS differ in 2 place(s) (ref 319, ours 319)
 *           first at index 44: ref 2618  ours 4d99
 *   `--whole` prints the same 2 of 319 with no SIZE and no relocation line.
 *
 * SPLIT SHAPE: NONE.  tools/datacheck.py is silent, and
 *   tools/split_s.py asm/rom_a1000/rom_a8604_c_a_a_c.s Func_80a96d8 --dry-run
 *   says "holds only Func_80a96d8 and no data; convert it directly".
 *
 * THE RESIDUE, ONE ADJACENT SWAP, both halves real code (no pool word):
 *   idx 44  ref 2618 `mov r6,#0x18`   ours 4d99 `ldr r5,=0xb06`
 *   idx 45  ref 4d99 `ldr r5,=0xb06`  ours 2618 `mov r6,#0x18`
 *
 * ================================================================
 * WHICH RUNG DECIDES, READ OFF -fsched-verbose=6
 * ================================================================
 * .23.sched2, basic block 4, the decision at t = 34:
 *
 *   ;;      insn  code    bb   dep  prio  cost   blockage units
 *   ;;      102   239     0     1    42    32    1 - 32   core : 797 121 120 118 114 110 104
 *   ;;      104    -1     0     2    42     1    0 -  0   none : 797 ... 1133 110 107
 *   ;;      107   173     0     1    42     2    1 - 32   core : 797 124 114
 *   ;;     1133   173     0     1    42     1    1 - 32   core : 797 1134
 *   ;;      110   173     0     2    40     2    1 - 32   core : 797 126 121
 *   ;;      118   173     0     2    40     1    1 - 32   core : 797 134 121
 *   ;;      Ready list (t = 34):    118  110  1133  107
 *   ;;              --> scheduling insn <<<107>>>          (ROM picks 1133)
 *
 *   rung 1 PRIORITY        42 == 42 for 107 and 1133; 110 and 118 are excluded
 *                          here at 40.                              ties
 *   rung 4 CLASS vs last_scheduled_insn (= insn 104, the barrier, scheduled at
 *          t = 33 on unit `none`).  BOTH carry the SAME note on the SAME insn:
 *            (insn  107 ... (insn_list:REG_DEP_OUTPUT 104 (nil)))
 *            (insn 1133 ... (insn_list:REG_DEP_OUTPUT 104 (nil)))
 *          class 2 == class 2.                                      ties
 *          (The class formula is haifa-sched.c:4068 -- class 3 if there is no
 *          link OR insn_cost == 1, else 1 for a true dependence, else 2.  Here
 *          both links are REG_DEP_OUTPUT on the SAME insn, and arm_adjust_cost
 *          prices an output dependence at 0, so both land in class 2 and the
 *          rung cannot separate them whatever the source says.)
 *   rung 5 DEPENDENT COUNT  107 -> {797,124,114} = 3
 *                          1133 -> {797,1134}    = 2
 *          MORE WINS.  ***THIS RUNG DECIDES.***
 *   rung 6 INSN_LUID        never reached -- and (insn 107 104 1133 ...) puts
 *          107 first in the chain, so equalising at 2-2 still loses it.
 *
 * ================================================================
 * WHY RUNG 5 CANNOT BE REACHED FROM SOURCE -- A NEW, STRONGER ARGUMENT
 * ================================================================
 * 107 cannot drop below 2: the ROM's own stream gives the pooled 0xb06 two users
 * in this block, `mov r0,r5` (arg0 of the first `_Func_801e7c0`) and `add r5,#3`
 * (the second call's msg), and any C spelling of `msg + 3` reads msg.  So 1133
 * must RISE to 3, i.e. the +0x18 must have TWO readers before `neg r6,r6`
 * overwrites it.
 *
 * *** AND IT CANNOT, FOR A REASON THAT GENERALISES: insns 1133/1134 ARE RELOAD
 * *** OUTPUT materialising a COMPILE-TIME CONSTANT, and every expression that
 * *** could relate 0x18 to another value in this block relates two compile-time
 * *** constants -- so cse and gcse's cprop fold it away long before sched2.
 * *** YOU CANNOT GIVE A RELOAD-MATERIALISED CONSTANT AN EXTRA DEPENDENT FROM C.
 * This is exactly why this function's earlier 4 -> 2 advance came from a
 * DECLARATION (`extern int Func_80a10d0(...)` instead of `void`, which turns
 * `*call_insn` into `*call_value_insn` and moves an output dependence onto the
 * CALL) and not from an expression.  Measured here, from 2:
 *   `y = 0x18;` named, both arg3s written `-y` ........ 2   exactly inert
 *     (cprop folds `-y` back to `-24`; reload re-splits it in the same place)
 *   the same body under -fno-gcse ..................... 241 of 319 at 321 insns
 *   the same body under -fno-rerun-cse-after-loop ..... 13
 *   so the flags that would stop the folding cost far more than they buy.
 *
 * THE PARK'S OWN ESCAPE FROM RUNG 4 IS ALSO SHUT, AND THIS IS NEW.  The park
 * says "to break rung 4 the last-scheduled insn would have to touch r5 and NOT
 * r6".  An inline-asm clobber cannot do it: `__asm__ volatile ("" ::: "r5")` is
 * EXACTLY INERT at 2, because a volatile asm is dependent on EVERYTHING
 * regardless of its clobber list (sched_analyze sets reg_pending_sets_all for
 * any volatile ASM_OPERANDS / ASM_INPUT), so the named register narrows nothing.
 * Do not spend a round on a clobber list here.
 *
 * MEASURED THIS BATCH (tools/crossfire.py, 6 edits to depth 2, 319 encodings):
 *   base ................................................. 2
 *   __asm__ volatile ("" ::: "r5") ....................... 2   exactly inert
 *   `msg = 0xb06;` moved ABOVE the barrier ............... 2   exactly inert
 *   `msg += 3;` then pass `msg` to the second call ....... 2   exactly inert
 *   named `int y = 0x18` with `-y` at both calls ......... 2   exactly inert
 *   those four crossed pairwise .......................... 2   exactly inert
 *   `msg` above the barrier + clobber "r5" ............... 8
 *   __attribute__((packed)) on struct State .............. 552 of 319 at 563
 *                                                            insns, RELOC + MEM
 * THE BRIEF'S `packed` LEVER IS MEASURED BADLY WORSE HERE, and the reason is
 * structural rather than bad luck: this struct's layout is already pinned by
 * explicit `pad` arrays at the ROM's offsets, so `packed` cannot change the
 * layout -- it can only drop every member's alignment, which turns aligned word
 * and halfword accesses into byte-wise ones (hence +244 instructions and a
 * failing MEM screen).  `packed` is a lever for a struct whose layout gcc is
 * choosing; it is inert-at-best and destructive-at-worst on a pad-array struct.
 *
 * Blocker class: sched2 ready-list ranking, rank_for_schedule RUNG 5 (dependent
 * count).  Closed on rungs 1, 4 and 5 together, each by a property of the ROM's
 * own instruction stream or of the competitors' RTL form.  Needs a different
 * rank_for_schedule.
 *
 * Everything below this line is unchanged from the batch-315 park and still
 * load-bearing; it is kept because it is the record of how the body reached 2.
 * (iwram_3001f2c IS A STRUCT, worth ~290 encodings; a pointer field must point at
 * a struct with no `int` member; the outer-loop rotation lever;
 * FRAME_GROWS_DOWNWARD so `short buf[15]` is declared before `int ctx[7]`; the
 * dead one-line `t` temp, 299 -> 17; `msg = 0xb06` with `msg + 3`; every
 * `extern void` re-swept to `extern int` in batch 315 -- all inert at 2 except
 * `_Func_801e7c0`, which is 4.  Re-ablations from 2: dropping the barrier 8,
 * dropping `n = 0xc80` 4, inlining `c = 0xf0` 5, `int ctx[7]` first 7, `f14`
 * back to `unsigned char *` 151, dropping the dead `t` 299.)
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
    StopTask(Func_80a19a0);
    __asm__ volatile ("");
    msg = 0xb06;
    _Func_801e7c0(msg, s->f24, 0x40, -0x18);
    _Func_801e7c0(msg + 3, s->f24, 0, -0x18);
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
