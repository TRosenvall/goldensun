/* PARKED -- OvlFunc_933_2009638, --align 33 of 248 / objcmp 40 of 251
 * NON-MATCHING, 33 instructions in disagreeing regions of 248.  The SIZE and
 * the instruction count BOTH match (ref 251 encodings, ours 251), so the
 * --align figure IS a distance; objcmp's own 245 is positional, inflated by a
 * single register rename in instruction 1 that cascades through the prologue.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7bc690/2009638.c \
 *     asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s --func OvlFunc_933_2009638
 *   docker run ... python3 tools/tryc.py --align \
 *     --ref asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s src/non_matching/ovl_7bc690/2009638.c
 *
 * LANDING NEEDS A SPLIT.  ovl_4e4_c_c_c.s holds TWO functions
 * (OvlFunc_933_2009638, OvlFunc_933_2009874) plus a trailing `.section .data`
 * with Events_TolbiSpring, .L1f48 and .L1f70.  datacheck.py: this function
 * "reads no data label -> split needs NO new export", and the three data
 * symbols are ALREADY .global.  The two linker lines are
 *   overlays/rom_7bc690/overlay.ld:42  asm/.../ovl_4e4_c_c_c.o(.text)
 *   overlays/rom_7bc690/overlay.ld:58  asm/.../ovl_4e4_c_c_c.o(.data)
 * and the data must stay with the 2009874 half.
 *
 * ========================= WHAT IS SETTLED =========================
 *
 * 1. FOUR POOLED AREA IDS, ALL ALREADY IN area.sym AT 175-178.  The ROM does
 *    `ldr r3, =0x5c / cmp r2, r3` for each of 0x5c, 0x59, 0x5a, 0x5b where an
 *    eight-bit `cmp` would do.  `(int)&_AREA_5c` etc. is 36 of 248 -> 26 and
 *    makes the size right; plain literals are 4 pool words SHORT.
 *    Their pool words carry R_ARM_ABS32 relocations where the ROM object has
 *    bare 0x59..0x5c -- the recorded "a relocation FORM is not a residue";
 *    take it to `make compare`.
 *
 * 2. THE gState BASE IS A LOCAL POINTER OFF A STRUCT OBJECT, not an array.
 *    `typedef struct { unsigned char _bytes[704]; } GlobalState; extern
 *    GlobalState gState;` with `gs = (unsigned char *)&gState;` gives the ROM's
 *    `ldr r0, =gState` once plus a reg+reg `add r2, r0, r3` per offset.
 *    `extern char gState[]` instead folds each address into a
 *    `ldr r2, =gState+556` pool entry and is 13 instructions SHORT.
 *
 * 3. THE ZERO STORED AT gState+0x22e MUST BE A NAMED NARROW LOCAL.
 *    `short zero = 0; *(short *)(gs + 0x22e) = zero;` puts the 0 in its own
 *    register (ROM: `mov r1, #0`) and, by conflicting with the base pseudo,
 *    is part of what pushes the base off r1.  Writing the literal costs it.
 *
 * 4. THE TWO PER-ARM CONSTANTS ARE LOCALS.  0x7e (two arms) and 0x7c (third)
 *    live in r5 across every __Func_80105d4 call in their arm; one `int c`
 *    per arm reproduces that.  39 -> 33.
 *
 * 5. THE TWO ARMS SHARE A TAIL BY CROSS-JUMPING, not by source structure.
 *    .L17f2 is gcc's jump2 merge of the last call of the 0x59 arm and the last
 *    of the 0x5a arm; both are written out in full below.
 *
 * ========================== WHAT IS OPEN ===========================
 *
 * THE RESIDUE IS cse's CONSTANT-DELTA REUSE POINTING THE WRONG WAY, and the
 * pass responsible is `cse` (constant reuse via a related value), not reload.
 * The ROM materialises the byte offset 0x22c into a register, consumes it in
 * `add r2, r0, r3`, and then reaches the STORED VALUE 0x258 from it as
 * `add r3, #0x2c` -- while re-materialising every LATER offset from scratch
 * (`ldr r2, =0x22e`, `mov r3,#0x8c / lsl #2`, `mov r2,#0xe0 / lsl #1`).  We get
 * the mirror image: gcc keeps 0x22c live in r0 and reaches 0x22e as
 * `add r0, #0x2` and 0x1c0 as `sub r0, #0x6e`, then pays a pool word for the
 * value (`ldr r3, =0x258`).  The same ROM idiom appears twice more in the
 * function -- `sub r2, #0xc0` for the value 0x100 off the 0x1c0 offset -- and
 * it is the SAME idiom that OvlFunc_920_2008538 gets for free from a plain
 * `*(int *)(iwram_3001ebc + 0x1c0) = 0x204;` (there it is `add r2, #0x44`), so
 * the construct is reachable; what is not known is what makes cse prefer the
 * value over the neighbouring offsets.
 *
 * Measured and NOT it: the value as `0x96 << 2` (identical, 33); offsets as one
 * reused `int o` variable (5 instructions SHORT, 34); offsets as separate
 * variables; `extern unsigned char gState[]` with `gs = gState` (identical);
 * computing `p = (short *)(gs + 0x1c0)` before the three stores (2 LONG, 30 --
 * the lowest --align seen, but the wrong length); an early `return 0;` instead
 * of the wrapping `if` (3 LONG, 35).
 *
 * A SECOND, SMALLER OPEN ITEM: we emit one extra branch-to-the-next-label near
 * `bl OvlFunc_933_2009c1c` that the ROM does not have, balanced elsewhere so
 * the total stays 251.  It has not been isolated.
 *
 * SHIMS: NONE (shimcount reports no pins and no fakematch-class shim).
  *
 * NON-MATCHING, 245 of 251 encodings differ (POSITIONAL: aligncmp reads 33
 * in disagreeing regions of 248; size and count both match).
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7bc690/2009638.c \
 *       asm/overlays/rom_7bc690/ovl_4e4_c_c_c.s --func OvlFunc_933_2009638
*/
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern unsigned char iwram_3001ebc[];
extern GlobalState gState;   /* GlobalState @ 0x02000240 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int n);
extern int __StartTask(void (*fn)(void), int n);
extern void OvlFunc_933_2008cd0(void);
extern void OvlFunc_933_2009c1c(void);
extern void OvlFunc_933_20084e4(void);
extern int _AREA_59;
extern int _AREA_5a;
extern int _AREA_5b;
extern int _AREA_5c;

int OvlFunc_933_2009638(void)
{
    unsigned char *gs;
    short *p;
    void *base;
    short zero;

    gs = (unsigned char *)&gState;
    *(short *)(gs + 0x22c) = 0x258;
    zero = 0;
    *(short *)(gs + 0x22e) = zero;
    *(short *)(gs + 0x230) = 0x119;
    p = (short *)(gs + 0x1c0);
    if (*p != (int)&_AREA_5c) {
        base = *(void **)iwram_3001ebc;
        *(int *)((char *)base + 0x1c0) = 0x100;
        OvlFunc_933_2009c1c();
        __StartTask(OvlFunc_933_2008cd0, 0xc8 << 4);
        if (*p == (int)&_AREA_59) {
            int c = 0x7e;
            __Func_80105d4(0x16, 7, 4, 2, 0x40, c);
            __Func_80105d4(8, 0xa, 4, 2, 0x44, c);
            __Func_80105d4(0x17, 0x15, 4, 2, 0x48, c);
            __Func_80105d4(0x10, 0x2a, 4, 2, 0x4c, c);
            __Func_80105d4(0x24, 0x2c, 4, 2, 0x50, c);
            __Func_80105d4(0xe, 0x37, 4, 2, 0x54, c);
        } else if (*p == (int)&_AREA_5a) {
            int c = 0x7e;
            __Func_80105d4(0x2a, 5, 4, 2, 0x40, c);
            __Func_80105d4(0x14, 0xb, 4, 2, 0x44, c);
            __Func_80105d4(0xe, 0xc, 4, 2, 0x48, c);
            __Func_80105d4(0x38, 0x12, 4, 2, 0x4c, c);
            __Func_80105d4(7, 0x16, 4, 2, 0x50, c);
            __Func_80105d4(0x2c, 0x17, 4, 2, 0x54, c);
            __Func_80105d4(0x26, 0x18, 4, 2, 0x58, c);
            __Func_80105d4(0x1a, 0x1c, 4, 2, 0x5c, c);
            __Func_80105d4(0x11, 0x23, 4, 2, 0x60, c);
            __Func_80105d4(0x32, 0x24, 4, 2, 0x64, c);
            __Func_80105d4(0x22, 0x2b, 4, 2, 0x68, c);
            __Func_80105d4(6, 0x2e, 4, 2, 0x6c, c);
            __Func_80105d4(0x1b, 0x37, 4, 2, 0x70, c);
            __Func_80105d4(0x2b, 0x38, 4, 2, 0x74, c);
        } else if (*p == (int)&_AREA_5b) {
            int c = 0x7c;
            __Func_8091ff0(0xa9);
            __Func_80105d4(8, 0xe, 4, 4, 0x40, c);
            __Func_80105d4(6, 0x12, 4, 4, 0x44, c);
            __Func_80105d4(0xa, 0x15, 4, 4, 0x48, c);
        }
        OvlFunc_933_20084e4();
    }
    return 0;
}
