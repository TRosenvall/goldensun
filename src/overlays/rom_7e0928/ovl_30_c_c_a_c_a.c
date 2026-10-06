/* Cluster OvlFunc_956_2008ad4..OvlFunc_956_2008ad4 extracted from goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.s.
 *
 * MATCHES.  0 of 39 encodings, 92 bytes, 6 relocations identical, --whole green.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7e0928/ovl_30_c_c_a_c_a.c \
 *     asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.s --whole
 *
 * NO SPLIT, NO EXPORTS, NO FLAG GROUP.  The .s holds exactly one
 * `.thumb_func_start` and emits no .data/.rodata/.bss;
 * overlays/rom_7e0928/overlay.ld:42 already names
 * `asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.o(.text)` and that is the only line
 * in any linker script naming this .o.  No Makefile line mentions rom_7e0928,
 * so the TU falls to the tree default `asm/%.o: src/%.c` at -O2 and the match
 * depends on no flag.  Pin count 0: no inline asm, no register asm, no shim.
 *
 * WHY IT WAS PARKED AT NINE, AND WHAT THE NINE REALLY WERE.  The park (batch
 * 329 brief I) had the residue decomposed correctly into three runs -- the
 * `0xfa << 1` offset build in r0 against our r2, the `0xc0 << 13` addend
 * hoisted above the `and` in the ROM, and the `ldr r1 / mov r0` pair at the
 * TravelTo setup -- and had proved both contested registers to be RELOAD
 * registers (`.18.greg` reads `;; 0 regs to allocate:`).  It then concluded
 * that the choice came from `allocate_reload_reg`'s round-robin phase and that
 * the only cure was giving the mask a second reader, which this program does
 * not have.  That conclusion was wrong in one specific way, and the correction
 * is the whole landing:
 *
 *   THE RELOAD REGISTER IS CHOSEN BY `find_reg` (reload1.c:1588-1659), NOT BY
 *   THE ROUND-ROBIN.  `find_reg` scans hard registers and takes the one with
 *   the lowest `spill_cost[regno]` -- the summed `REG_N_REFS` of the pseudos
 *   allocated to that register that are live at this insn -- and breaks ties
 *   among equal costs with `inv_reg_alloc_order` (reload1.c:1645-1648), i.e.
 *   REG_ALLOC_ORDER `{3,2,1,0,...}` (arm.h:989).  So the reload takes the
 *   FIRST ZERO-COST register in the order 3, 2, 1, 0.  `allocate_reload_reg`'s
 *   round-robin runs inside the set `find_reg` has already built, and is not
 *   what picks the number.
 *
 *   That makes the lever concrete.  In the parked spelling, r3 holds the
 *   accumulator and r2 holds the mask -- which DIES in the `and` one insn
 *   earlier, so its cost at the `add` is zero -- and the reload therefore takes
 *   r2, the register the `and` just freed.  Having taken r2 the `mov r2,#0xc0`
 *   carries an anti-dependence against `and r3,r3,r2` and sched2 cannot hoist
 *   it, which is exactly the shape the park named.
 *
 *   The fix is not to keep the mask alive.  It is to make r2 and r1 BUSY at the
 *   `add`, so that r0 is the first zero-cost register.  Loading the two
 *   TravelTo coordinates into named locals makes them pseudos that take r1 and
 *   r2 by call-argument preference; placing those two statements BETWEEN the
 *   `and` and the `+` puts them live across the `add` but NOT across the mask's
 *   own range, so the mask still gets r2 and the reload is pushed to r0.  With
 *   the reload in r0 there is no anti-dependence on the `and` and sched2 hoists
 *   `mov r0,#0xc0` above it by itself -- the ROM's shape, for free.
 *
 * THE POSITION IS THE WHOLE LEVER, AND IT IS A THREE-WAY CROSSING.  Measured,
 * all against this reference:
 *
 *     the parked form, one compound expression                 9 of 39
 *     x and y named but assigned BEFORE the whole statement    3 of 39
 *     x only named                                             9 of 39
 *     y only named                                            11 of 39
 *     y, then the add, then x                                  9 of 39
 *     x, then the add, then y                                  9 of 39
 *     the `and` split out, then x, then y, then the add         MATCH
 *
 * So all three of -- splitting the `and` from the `+`, naming BOTH coordinates,
 * and putting both between them -- are required together.  Naming one
 * coordinate is inert or worse; naming both without splitting the expression
 * stalls at 3.  This is the "do not search the diagonal of a square" rule from
 * batch 330 holding on a THREE-dimensional square.
 *
 * MEASURED AND INERT at the parked figure of nine, so none of these is a lever:
 * naming the mask `0xfff00000`; naming the addend `0xc0 << 13`; both named
 * together; naming the `a + 0x10` field; splitting the gState base into a
 * second pointer; `unsigned int` for the result.  Naming the slot number
 * `*(int *)q` is MUCH worse (33 of 39 with dirty relocations) and confirms the
 * park's standing requirement that the offset stay a variable shifted in place.
 * Once the landing form is reached, the declaration ORDER of x, y, u and t is a
 * tie in every permutation tried, as is `0xfff00000 & field` against
 * `field & 0xfff00000` -- gcc canonicalises -- so nothing in the shipped
 * spelling beyond the three statement positions is load-bearing.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);

void OvlFunc_956_2008ad4(void)
{
    unsigned char *q;
    unsigned char *a;
    unsigned int off;
    int u;
    int x;
    int y;
    int t;

    q = (unsigned char *)&gState;
    off = 0xfa;
    off <<= 1;
    q += off;
    a = (unsigned char *)__MapActor_GetActor(*(int *)q);
    *(int *)(a + 0x34) = 0x80 << 9;
    *(int *)(a + 0x30) = 0x80 << 10;
    __MapActor_Surprise(*(int *)q, 0x81 << 1);
    __Actor_SetAnim(a, 5);
    u = *(int *)(a + 0x10) & 0xfff00000;
    x = *(int *)(a + 8);
    y = *(int *)(a + 0xc);
    t = u + (0xc0 << 13);
    __Actor_TravelTo(a, x, y, t);
    __Actor_WaitMovement(a);
}
