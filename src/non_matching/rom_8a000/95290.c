/* StartThunder2 -- 0x08095290, asm/rom_8a000/rom_944ec_a_a_c_c_a.s (1 function).
 *
 * NON-MATCHING, 39 of 80 encodings -- AND THAT IS A REAL DISTANCE FOR THE FIRST
 * TIME.  MEASURED batch 344 brief E.  Pin-free, device-free (shimcount: none).
 *
 * THE LENGTH GAP IS CLOSED.  The park this replaces could not be read at all --
 * objcmp stopped at the size check (ref 184 bytes against ours 192) and no
 * encoding figure existed.  Now:
 *
 *     ref    68 16-bit instructions   12 wide entries   80 stream   184 bytes
 *     ours   68                       12                80         184
 *
 * so the positional figure is no longer phase and 39 is a distance.  objcmp's
 * `first at index 6` is the first genuine difference, not an offset.
 *
 * RELOCATIONS still differ, by OFFSET ONLY and in one entry: `iwram_3001ed0`
 * sits at 0xa0 for us and 0xa4 in the reference, `Task_Thunder` at 0xb4 in both.
 * Both symbols are present under the same names, so this is the POOL ENTRY ORDER
 * following from residue B/C below -- not a missing or an extra symbol.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/95290.c \
 *     asm/rom_8a000/rom_944ec_a_a_c_c_a.s --func StartThunder2
 *
 * SPLIT SHAPE when it lands: none.  tools/datacheck.py on the reference is
 * silent (no data section) and tools/piece.py reports 1 function, all 1 parked,
 * CONVERTS WHOLE -- install path src/rom_8a000/rom_944ec_a_a_c_c_a.c.
 *
 * ============ WHAT CLOSED IT: A HImode LITERAL STORE GOES TO THE POOL ============
 *
 * The park body wrote the halfword store as a literal:
 *
 *     *(unsigned short *)(buf + (0xfc << 5)) = 0x78;
 *
 * and that ONE statement cost all three length figures.  The expander
 * `movhi` (arm.md:4269-4271, TARGET_THUMB branch) reads
 *
 *     if (GET_CODE (operands[0]) != REG)
 *       operands[1] = force_reg (HImode, operands[1]);
 *
 * so a halfword STORE forces its source into an HImode register, and the
 * literal reaches `*thumb_movhi_insn` (arm.md:4318) as a HImode fix.  That
 * pattern's alternative 1 (`l` <- `mn`) carries **pool_range 64**
 * (arm.md:4353), against SImode's 1020 (`*thumb_movsi_insn`, arm.md:3856).
 * 64 bytes is not far enough to reach an end-of-function pool, so `arm_reorg`
 * dumps a MID-FUNCTION pool and gcc emits a `b` over it.  Hence:
 *
 *     +1 pool word (`.word 120`)  +1 instruction (`b .L21`)  +1 pad  = +8 bytes
 *
 * THIS IS THE SAME MECHANISM include/dma.h's DMA3_FILL16 comment documents, read
 * backwards.  That header exists to CREATE the mid-function pool on purpose and
 * states the rule in the other direction: "An INLINE FUNCTION cannot get there:
 * a `u16` parameter is PROMOTED, so the store becomes a `subreg:HI` of an SImode
 * register and the constant never enters `movhi`."  Feeding the store from an
 * `int` is exactly that promotion, so the HImode fix never appears, the pool
 * goes back to the end of the function, and the `b` is not needed.
 *
 * Both of this function's halfword stores are fed from ints in the ROM, and the
 * reference says so plainly: `mov r3,#0x78 / strh r3,[r2]` for one and
 * `mov r3,r9 / strh r3,[r5]` for the other, r9 holding an int zero kept across
 * every call.  The `= 0` store already reproduced for that reason; only the 0x78
 * one did not.
 *
 * MEASURED (gap = ours - ref 16-bit instruction count):
 *   park body, literal 0x78                     gap +1  w32 13  ALIGN 34  (no figure: size)
 *   `v = 0x78;` then store from `v`              gap  0  w32 12  ALIGN 33  NDIFF 39  <= THIS
 *   same, buf assigned before y                 gap  0  w32 12  ALIGN 35  NDIFF 39
 *   same, plus `y += 0xe0 << 4;` in place        gap  0  w32 12  ALIGN 33  NDIFF 39
 *   `int v = 0x78;` initialised at the TOP       gap +5  w32 12  ALIGN 48  NDIFF 84
 *   adding `int ret = 0;` for the second store   gap +1  w32 12  ALIGN 41  NDIFF 69-73
 *
 * Note the last two rows.  `v` must be assigned IMMEDIATELY BEFORE the store:
 * initialised at the top it becomes live across four calls, takes a
 * callee-saved register and costs five.  And an explicit `int ret = 0;` for the
 * `= 0` store is WORSE, not better -- the zero that reaches r9 in both builds is
 * already the one DMA3_CLEAR stages, CSEd with the halfword store's zero.  Do
 * not add it; the rom_a1000 "int x = 0 at the top" shape does not apply here.
 *
 * ===================== WHAT THE REMAINING 39 IS =====================
 *
 * Three things, from tools/aligncmp.py (19 hunks, 33 position-tolerant):
 *
 *   A. THE `iwram_3001ed0` LOAD POSITION.  The ref loads it AFTER
 *      `bl galloc_ewram`; we load it before, because our source assigns `y`
 *      first.  Assigning `buf` first DOES move the load to the ref's position
 *      -- and flips the allocation the other way: `a` lands in r8 and `y` in r6
 *      where the ref has `a`=r6, `y`=r8, and `p` then needs an extra
 *      `mov r8, r3`.  The two halves are one instruction apart in OPPOSITE
 *      directions, the same shape Func_80a355c had.  With `y` first the
 *      allocation is EXACTLY the ROM's (buf=r5, a=r6, y=r8, zero=r9, b=r10,
 *      p reusing r6) and only the position is wrong.  That is the thing to
 *      attack and it is NOT a declaration-order question (see below).
 *   B. SCRATCH NUMBERING.  Three windows use r1 where the ref uses r3
 *      (`movs r1,#0`, `movs r1,#168`, `movs r1,#252`), and the final store has
 *      address and value in r3/r2 where the ref has r2/r3.  The ref reuses ONE
 *      scratch register (r3) for the offset temp and then the stored value;
 *      we spend two.
 *   C. ARGUMENT SETUP ORDER.  At three call sites the ref sets r0 LAST
 *      (`mov r1,r8 / mov r2,r5 / mov r0,r6 / mov r3,#1 / bl`) and we set it
 *      first.  Same instructions, one slot apart, three times.
 *
 * MEASURED NEGATIVES, so nobody re-screens them:
 *   - DECLARATION ORDER IS INERT AT N=24.  All six permutations of
 *     buf/y/p x `int v` first-or-last x both assignment orders read NDIFF
 *     exactly 39; ALIGN only moves between 33 (y assigned first) and 35 (buf
 *     first).  "Two is not N" does not rescue this one: twenty-four is not two.
 *   - THE FLAG AXIS IS CLOSED ON THIS BODY, re-crossed after the source change
 *     as docs/elevation.md requires.  All eight Makefile groups and thirteen
 *     probe flags hold gap=0; -O1 and -fno-schedule-insns2 both go to 40 of 80
 *     with ALIGN rising 33 -> 39, i.e. worse.  (On the OLD body autosweep called
 *     -O1 46 and sched2 46 -- both worse there too.)
 *   - THE ANCHOR AXIS IS CLOSED ON THIS BODY: all 16 statement boundaries at any
 *     brace depth (three of the first four are inside the declaration block and
 *     do not compile under C89).  Best is `do { } while (0);` after `v = 0x78;`
 *     at 37 of 80, ALIGN 31; next is after the 0x78 store at 38, ALIGN 31.
 *     NOT SHIPPED: two encodings is not worth an unexplained construct in the
 *     body, and it does not touch A, B or C.  Recorded so it is not re-swept.
 *
 * THE DMA BLOCK AND THE TAIL ARE EXACT, as the original park said: DMA3_CLEAR
 * reproduces `mov r0,sp / mov r3,#0 / str r3,[r0] / mov r1,r5 /
 * ldr r3,=REG_DMA3SAD / ldr r2,=0x850007e2 / stmia r3!,{r0,r1,r2} / sub r3,#0xc`
 * byte for byte, and 0x850007e2 decodes as 0x85000000 | (0x1f88 / 4).
 *
 * The park's two inherited levers remain correctly recorded as inert: copying
 * the first parameter into a local (batch 115) and swapping declarations
 * (batch 115) do nothing here.  Its "32 of 74" and "39" figures were measured on
 * a stream eight bytes short and were phase, not distance -- which is why the
 * declaration-order rows all read the same number then and still do now.
 */
#include "dma.h"

extern void *galloc_ewram(int kind, int size);
extern unsigned char *iwram_3001ed0;
extern void Func_8090a5c(int a, void *b, void *c, int d);
extern void Func_809088c(void *a, void *b, void *c, int d);
extern void StartTask(void *f, int n);
extern void Task_Thunder(void);

void StartThunder2(int a, int b)
{
    unsigned char *buf;
    unsigned char *y;
    unsigned char *p;
    int v;

    y = iwram_3001ed0;
    buf = galloc_ewram(0x1e, 0x1f88);
    DMA3_CLEAR(buf, 0x1f88);
    Func_8090a5c(a, y, buf, 1);
    p = buf + (0xa8 << 4);
    Func_8090a5c(b, y, p, 1);
    Func_809088c(p, buf, buf + (0xa8 << 5), 0xc);
    Func_8090a5c((int)buf, 0, y + (0xe0 << 4), 1);
    v = 0x78;
    *(unsigned short *)(buf + (0xfc << 5)) = v;
    *(unsigned short *)(buf + 0x1f82) = 0;
    StartTask(Task_Thunder, 0xc8 << 4);
}
