/* OvlFunc_909_200809c (0x0200809c) -- LANDING.
 *
 * MATCHES: 36 of 36 instructions, stream 43/43, SIZE equal. objcmp --func and
 * --whole both report ENCODINGS differ in 1 place, at stream index 39: that
 * index is the POOL WORD for `_AREA_21`. The reference holds the resolved
 * literal 0x21; our object holds an R_ARM_ABS32 against `_AREA_21`, which
 * reads 0 before the link. `area.sym` already carries `_AREA_21 = 0x21`, and
 * an absolute linker-script symbol emits no bytes, so the LINKED bytes are
 * identical. This is the documented phantom of docs/elevation.md's "THE
 * RELOCATION CHECK CANNOT REFUTE A SYMBOL, ONLY CONFIRM A LITERAL" -- the
 * reference .s is a disassembly and cannot carry a relocation for a symbol
 * that was already resolved. `make compare` is what settles it.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_79c738/ovl_30_c_c_a_a_a_a.c \
 *     asm/overlays/rom_79c738/ovl_30_c_c_a_a_a_a.s --whole
 *
 * SPLIT SHAPE: none. The piece holds ONE function (one .thumb_func_start) and
 * `tools/datacheck.py` reports no data, so "split": null and "exports": [].
 * `.L29b4` and `.L299c` are already `.global` in
 * asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c.s, so nothing has to
 * be exported for this one. PINS: 0 (tools/shimcount.py).
 *
 * WHAT THE PARK'S RESIDUE WAS. The park read 30 of 43 at ref 36 / ours 34 --
 * TWO SHORT -- and blamed strength reduction: "gcc chains four byte stores
 * that the ROM recomputes ... gcc sees the arithmetic progression and
 * strength-reduces all four into one walking register". That observation was
 * exactly right. Its conclusion -- that no spelling reproduces it, after
 * trying base-relative subscripts for the first three and an explicitly
 * advanced pointer for the fourth -- was wrong.
 *
 * THE LEVER, AND IT IS ONE LEVER WORTH THE PAIR: name the ARRAY, not the
 * pointer, at each store.
 *
 *     L29b4[0xa6] = 2;   instead of   p[0xa6] = 2;
 *
 * Each store is then its own `symbol_ref`, so there is no single pseudo for
 * cse's related-value machinery to hang an `add Rd, Rd, #0x18` chain off.
 * cse commons the four symbol loads down to ONE pool load plus `mov rX, r5`
 * copies -- which is precisely the ROM's `mov r1, r5 / add r1, #0xa6`,
 * `mov r3, r5 / add r3, #0xbe`, `mov r2, r5 / add r2, #0xd6` -- and the
 * FOURTH store still chains (`add r2, #0x18`) because by then the third
 * store's register is a live equivalent. Three recomputed, one chained: the
 * ROM's own split, from one spelling change.
 *
 * `p` stays as the call argument and the return value, which is why the
 * `ldr r5, =.L29b4 / mov r0, r5` pair at the head is unchanged.
 *
 * MEASURED INERT (all still ref 36 / ours 34, 30 of 43):
 *   three named pointers `q1/q2/q3 = p + K` with the fourth advanced;
 *   one named pointer reassigned from `p` before each store;
 *   the same through `L29b4 + K` assigned to a pointer FIRST (the copy dies --
 *     the symbol has to be named AT the store, not hoisted into a local);
 *   a `unsigned char (*)[0x18]` array-of-rows view;
 *   subscripts for the first two and an advanced pointer for the last two
 *     (the park's own probe, reproduced).
 * Writing the fourth store as `L29b4[0xd6 + 0x18]` instead of `L29b4[0xee]`
 * is byte-identical, so the chain really is cse's and not the source's.
 */
extern unsigned char gState[];
extern int _AREA_21;
extern unsigned char L29b4[] __asm__(".L29b4");
extern unsigned char L299c[] __asm__(".L299c");
extern void __Func_808b868(unsigned char *p);
extern int __GetFlag(int id);

unsigned char *OvlFunc_909_200809c(void)
{
    unsigned char *gp;
    unsigned char *p;

    gp = gState;
    if (*(short *)(gp + (0xe0 << 1)) == (int)&_AREA_21) {
        p = L29b4;
        __Func_808b868(p);
        if (__GetFlag(0x84e)) {
            L29b4[0xa6] = 2;
            L29b4[0xbe] = 0;
            L29b4[0xd6] = 3;
            L29b4[0xee] = 1;
        }
        return p;
    } else {
        return L299c;
    }
}
