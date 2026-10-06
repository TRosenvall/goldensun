/* Cluster OvlFunc_954_200842c..OvlFunc_954_200842c extracted from goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.s.
 *
 * MATCHES.  0 of 44 encodings, 100 bytes, 6 relocations identical, --whole green.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.s --whole
 *
 * NO SPLIT, NO EXPORTS, NO FLAG GROUP.  The .s holds exactly one
 * `.thumb_func_start` and emits no .data/.rodata/.bss;
 * overlays/rom_7db0c8/overlay.ld:29 already names
 * `asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.o(.text)` and no Makefile
 * line reaches this stem, so the TU falls to the tree default at -O2 and the
 * match depends on no flag.  Pin count 0: no inline asm, no register asm.
 *
 * THE PARK WAS RIGHT ABOUT THE REGISTER AND RIGHT THAT IT WAS ONE CAUSE.  Both
 * copies of `ldr r3,[r0,#0x10] / asr r0,r3,#20` put the shift result in the
 * register that held the actor pointer; we put it in the register that held the
 * loaded field, and the two argument-move swaps are anti-dependence
 * consequences of that, not a second cause.  Batch 329 brief I established that
 * with dump evidence and also closed, with dump evidence, both routes it had
 * considered: `global.c:set_preference` (:1595-1667) cannot name r0 because the
 * actor pointer dies at the LOAD and not at the insn that defines the shift
 * (`REG_DEAD (reg 33)` sits on the load), and exhaustion cannot reach r0
 * because `;; 34 conflicts: 34 35 36 5 13` says only r5 and sp conflict.
 *
 * WHAT WAS NEVER VARIED IS HOW MANY NAMES THE TWO COPIES SHARE.  The park's
 * whole negative list varies the FIRST copy -- naming the loaded field, the
 * `?:` form, `< 9` against `<= 8`, the shared 0x40, withholding the callee's
 * prototype -- while the second copy stayed an inline
 * `__MapActor_GetActor(0x11)->f10 >> 20` inside the final call's argument list.
 * Written that way the second copy's pointer and result are two fresh
 * block-local pseudos and `local-alloc.c:combine_regs` (:1593) ties the shift's
 * destination to the dying field, which is r3.
 *
 * REUSING BOTH `a` AND `y` FOR THE SECOND COPY IS THE EDIT, AND BOTH HALVES ARE
 * REQUIRED.  Measured against this reference:
 *
 *     the parked form, second copy inline                      9 of 44
 *     reuse `y` only, pointer left inline                      9 of 44
 *     reuse `y`, second pointer in a FRESH name `b`            9 of 44
 *     a second result name `y2`, pointer inline                9 of 44
 *     the field named and shared as well                       9 of 44
 *     reuse BOTH `a` and `y`                                    MATCH
 *
 * Sharing the pointer name makes the two actor pointers one pseudo whose live
 * range spans the middle call, and sharing the result name makes the two shift
 * results one pseudo; with both shared the pair is allocated once and the
 * pointer's register is what the shift writes, in both copies, which is the
 * ROM.  Either half alone leaves the second copy with its own block-local pair
 * and `combine_regs` ties it back to r3.  This is batch 330's `Func_801a910`
 * lever -- how many NAMES the two arms share -- running in the opposite
 * direction: there the two arms had to be SEPARATED, here they have to be
 * MERGED, and in both cases one dimension alone reads inert.
 *
 * ALSO MEASURED: hoisting `e = 0x40;` above the gState read is WORSE, 17 of 44,
 * so the position of that assignment is load-bearing and must stay where the
 * park had it.  The park's own correction stands: the figure is 9 of 44, not
 * "9 of 43" -- 43 is the instruction count and 44 the encoding count.
 */
struct A { unsigned char pad00[0x10]; int f10; };

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_954_200833c(int a, int b, int c);

void OvlFunc_954_200842c(void)
{
    unsigned char *g;
    struct A *a;
    int y;
    int dir;
    int e;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    y = a->f10 >> 20;
    dir = -0x30;
    if (y <= 8)
        dir = 0x30;
    e = 0x40;
    __Func_8010704(0x43, 8, 3, 1, e, y);
    OvlFunc_954_200833c(0x11, 0, dir);
    a = __MapActor_GetActor(0x11);
    y = a->f10 >> 20;
    __Func_8010704(0x40, 0x18, 3, 1, e, y);
}
