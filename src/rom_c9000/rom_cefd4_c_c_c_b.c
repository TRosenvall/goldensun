/* BaseAnim_Spasm -- 0x080ceff8.  *** MATCHING *** (was parked at 2 of 291.)
 *
 * Batch 318, brief C.  The park's figure was correct and a TRUE DISTANCE; its
 * FLOOR CLAIM was wrong, and both halves of the cure were ALREADY IN THE PARK'S
 * OWN TWO LISTS, never crossed:
 *
 *   park's INERT list   `DrawFn d[2]` as a real local array instead of two
 *                       scalars ................................. 2 (inert)
 *   park's WORSE note   "d1 read placed between the two stores" .. 10 (worse)
 *   CROSSED                                                        0  <-- here
 *
 * This is the brief's named shape ("one half in the rejected list and the other
 * in the inert list, NEVER CROSSED") on a park that had already done the work
 * of measuring both halves.
 *
 * THE TWO EDITS, and nothing else, against the parked body:
 *   1. `DrawFn d1; DrawFn d0;`  ->  `DrawFn d[2];`   (d0 = d[0], d1 = d[1])
 *   2. the `d[1] = (DrawFn)gPtrs[0xbc / 4];` read moves from AFTER both
 *      base+0x77xx stores to BETWEEN them.
 *
 * VERIFIED (tools/objcmp.py, THE AUTHORITY, inside the container):
 *   OK BaseAnim_Spasm -- 680 bytes, 291 encodings and 38 relocations identical
 * Memory-access screen identical to the ROM in every opcode:
 *   ldmia 1  ldr 41  ldrb 4  ldrh 2  ldrsh 3  push 3  pop 3  str 18  strh 2.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c9000/rom_cefd4_c_c_c_b.c \
 *     asm/rom_c9000/rom_cefd4_c_c_c.s --func BaseAnim_Spasm
 * (While it still sits in src/non_matching/rom_c9000/80ceff8.c, use that path.)
 *
 * ================================================================
 * WHY IT WORKS -- the mechanism, read off the dumps, not guessed
 * ================================================================
 *
 * The residue was a two-element TRANSPOSITION in sched2's chosen order, and
 * nothing else: base scheduled ... 262, 269, 271, 281, 862 ... where the ROM
 * schedules ... 262, 281, 271, 269, 862 ...  (262 = `str r6,[r3]`,
 * 269 = `add r2,r8`, 271 = `movs r3,#50`, 281 = `ldr r5,[r5]`).
 *
 * At the cycle where they compete, insns 269 and 281 are EXACTLY SYMMETRIC on
 * every live rung of `rank_for_schedule`:
 *     INSN_PRIORITY      4   vs   4
 *     class vs last      3   vs   3     (see the note below -- this rung is dead)
 *     dependent count    3   vs   3     {block-end jump, call output-dep, one use}
 * so the tie falls through to **INSN_LUID, lower wins**, and 269's statement
 * came first in the source.  Edit 2 inverts that LUID order.
 *
 * Edit 2 ALONE reads 10 because inverting it drags reload's spill store of d1
 * (`str r5,[sp,#0xc]`) across the `base+0x7784` store.  That spill MEM is
 * **ALIAS SET 0** (`(mem:SI (plus (reg sp) (const_int 12)) 0)` in .19.flow2 --
 * reload's own slots always are), so it conflicts with everything; moving it
 * flips a FALSE memory dependence, which lengthens the r5 priority chain by one
 * (`r5+=0xbc` goes from priority 5 to 6) and schedules `adds r5,#188` two slots
 * early.  THAT is what the park measured as "the block lands one slot early".
 *
 * Edit 1 is what defuses it.  `DrawFn d[2]` gives the two function pointers a
 * REAL AGGREGATE with a REAL ALIAS SET in place of two reload spill slots at
 * alias set 0, so the d1 store no longer conflicts with the `base+0x7784`
 * store, the priority chain keeps its original length, and `adds r5,#188`
 * keeps its slot while the LUID inversion stands.  The aggregate is
 * byte-neutral on layout: d[0] lands at sp+0x8 and d[1] at sp+0xc, exactly the
 * slots the park's `DrawFn d1; DrawFn d0;` declaration order produced, which is
 * why the loop's `ldr r4,[sp,#8]` / `ldr r4,[sp,#0xc]` are unchanged.  (This is
 * batch 317's "the aggregate must keep the scalars' DECLARATION SLOT", and its
 * two-word size gate: d[2] is exactly two words.)
 *
 * SO THE AGGREGATE LEVER HAS A SECOND MODE, worth recording as new:
 *   batch 316/317 used a two-word aggregate to CHOOSE A REGISTER PAIR.  Here it
 *   is used to **take a value OUT OF ALIAS SET 0** -- replacing a reload spill
 *   slot with a declared object removes false memory dependences that no
 *   C-level alias edit can reach, because the offending MEM is reload's and not
 *   the source's.  Reach for it whenever a residue moves when you reorder
 *   statements but drags a spill store with it.
 *
 * ================================================================
 * WHAT OF THE PARK SURVIVED, AND WHAT DID NOT
 * ================================================================
 * SURVIVED (reproduced exactly):
 *   - the figure, 2 of 291, and that it is a true distance: SIZE 680/680,
 *     instruction count 291/291, all 38 relocations identical in type, symbol
 *     and offset.  (`--whole` on the UNSPLIT .s shows 4 of 291 + SIZE 692/680 +
 *     a `.rodata`-vs-`.Lee09x` relocation difference; that is the split artefact
 *     the park documents -- the 12 bytes of .rodata stay behind in a residual
 *     .s -- NOT a hidden RELOCDIFF.  `--func` is the shipping measurement.)
 *   - the pass: sched2 / `rank_for_schedule`, confirmed, not assumed.
 *   - every one of the eleven load-bearing constructs (a)-(k): each is still a
 *     single drop from this file.
 *   - the whole MEASURED INERT list, re-measured at 2 one at a time.
 *   - the batch-305 correction that sched1 does not run in this build.
 *
 * REFUTED:
 *   - "Reaching the ROM would need the load ranked above `add r2,r8` WITHOUT
 *     moving `adds r5,#188`, i.e. an anti-dependence on r2 or r8 that the
 *     instruction set does not contain -- the Func_8077f70 shape of proof
 *     rather than an exhausted search."  NO anti-dependence was needed.  The
 *     rung in play is INSN_LUID, the inversion is reachable from source order,
 *     and the collateral damage is removable with the aggregate.  The park
 *     reached for an impossibility proof one cross short of the answer.
 *   - "WHY SOURCE ORDER CANNOT REACH IT" as a heading: source order DOES reach
 *     it; it just cannot reach it alone.
 *
 * ================================================================
 * LANDING SHAPE (unchanged from the park -- re-verified)
 * ================================================================
 * `python3 tools/datacheck.py asm/rom_c9000/rom_cefd4_c_c_c.s`:
 *     data sections : .rodata
 *     functions     : BaseAnim_Spasm
 *     -> converting a function here needs a TEXT/DATA SPLIT
 *     BaseAnim_Spasm  reads .Lee096, .Lee09c, .Lee09f
 *                     *** SPLIT MUST EXPORT: .global .Lee096 .Lee09c .Lee09f
 * `python3 tools/split_s.py asm/rom_c9000/rom_cefd4_c_c_c.s BaseAnim_Spasm --dry-run`
 * REFUSES until those three `.global` lines are added, and says so by name.  So:
 *   1. add `.global .Lee096` / `.global .Lee09c` / `.global .Lee09f` to
 *      asm/rom_c9000/rom_cefd4_c_c_c.s and confirm `make compare` STILL GREEN
 *      (a .global emits no bytes -- keep this change separable);
 *   2. then split: function -> asm/rom_c9000/rom_cefd4_c_c_c_b.s (discarded),
 *      data -> asm/rom_c9000/rom_cefd4_c_c_c_c.s;
 *   3. install this file as src/rom_c9000/rom_cefd4_c_c_c_b.c;
 *   4. DO NOT emit the three blobs from C -- they sit mid-run between
 *      asm/rom_c9000/rom_ceb30_c_c_c_c.s (0xee090..0xee096) and
 *      asm/rom_c9000/rom_cf2a0_c_c.s (0xee0a2..); moving them moves addresses.
 *   5. re-run `make` + `make compare`.
 * The .s holds ONE function, so no other park's Verify recipe is invalidated.
 *
 * FAKEMATCH ROWS NEEDED: THREE, exactly the park's three pinned hard registers,
 * unchanged by this batch (`tools/shimcount.py` reports `register pins : 3`):
 *   1. `register int q0 __asm__("r0")`    copy site 1
 *   2. `register void *q1 __asm__("r1")`  copy site 1
 *   3. `register void *tf __asm__("r0")`  StartTask
 * No `.equ`, no fictitious alias, no never-read union member, no `"+r"`
 * barrier, no measurement device of any kind: the two edits of this batch are
 * ordinary C, and the body compiles at PRODUCTION FLAGS with no per-file
 * Makefile override.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

extern int *iwram_3001eec[];
extern void *gPtrs[];
extern u16 Lee096[] __asm__(".Lee096");
extern u8 Lee09c[] __asm__(".Lee09c");
extern u8 Lee09f[] __asm__(".Lee09f");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int e);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

static inline void Copy0(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

static inline void Copy1(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

static inline void Copy2(CopyFn f, void *src)
{
    f((volatile u16 *)(0xa0 << 19), src, 0x80);
}

void BaseAnim_Spasm(void *context, int mode)
{
    vec3_t pos;
    void *ctx;
    DrawFn d[2];
    int **tbl;
    int **pp;
    u8 *base;
    State **slot;
    CopyFn copy;
    u8 *data;
    int frame;
    int n;
    int x;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    data = GetFile(FILE_7b);
    {
        register int q0 __asm__("r0");
        register void *q1 __asm__("r1");
        q0 = 0xa0;
        q1 = data;
        copy = Func_8001af8;
        data += 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, q1, 0x80);
    }
    DecompressLZ(data, base);
    data = GetFile(FILE_8d);
    Copy1(copy, data);
    if (mode == 2) {
        data = GetFile(FILE_68);
        Copy2(copy, data);
    }
    GetBattleActorPos3((*slot)->ids[0], &pos);
    if (mode == 0) {
        REG_BG2PA = 0x100;
        x = 0x40 - pos.x;
        REG_BG2X = x << 8;
    } else {
        REG_BG2PA = 0xcc;
        x = -pos.x * 4 / 5 + 0x40;
        REG_BG2X = x << 8;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    d[0] = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    *(int *)(base + (0xef << 7)) = 2;
    d[1] = (DrawFn)gPtrs[0xbc / 4];
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    if (mode == 2) {
        *(int *)(base + 0x77a8) = 0;
        _PlaySound(0xd4);
    } else if (mode == 1) {
        *(int *)(base + 0x77a8) = 8;
        _PlaySound(0xd4);
    } else {
        *(int *)(base + 0x77a8) = 0x20;
    }
    frame = 0;
    do {
        if (frame == 0) {
            if (mode == 2) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, -1, 0, 0x20);
            } else {
                Func_80d6888((*(State **)(base + 0x7828))->ids[0], 0xa, -1, 0, 0x20);
            }
        }
        if (frame == 0x18) {
            _Func_80bd7dc(0);
        }
        if (frame == 8 && mode == 0) {
            _PlaySound(0x7e);
        }
        if (frame < 0x20) {
            n = frame / 4;
            if (n > 2) {
                n = (n & 1) + 1;
            }
            if (frame < 0x1c) {
                d[0](ctx, base + Lee096[n], 0x40 - Lee09c[n], pos.y - Lee09f[n] + 8,
                   Lee09c[n], Lee09f[n]);
                d[1](ctx, base + Lee096[n], 0x40, pos.y - Lee09f[n] + 8,
                   Lee09c[n], Lee09f[n]);
            }
        }
        if (mode == 0) {
            UpdateScreenShake(2, 2);
        } else {
            UpdateScreenShake(0x10, 0x10);
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x30);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
