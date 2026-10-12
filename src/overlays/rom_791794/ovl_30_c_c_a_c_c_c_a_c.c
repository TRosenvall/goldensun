/* OvlFunc_897_200ac1c -- MATCHING (batch 338, brief B).
 *
 * Was NON-MATCHING at 14 of 53 encodings.  Now BYTE-IDENTICAL:
 *   objcmp --func : OK -- 128 bytes, 53 encodings and 11 relocations identical
 *   objcmp --whole: OK whole file -- 128 bytes, 53 encodings, 11 relocations
 * Instruction counts agreed on both sides throughout (41/41), so the park's
 * figure was a true distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_791794/ovl_30_c_c_a_c_c_c_a_c.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_a_c_c_c_a_c.s --whole
 *
 * Pin-free.  tools/piece.py: the reference holds this function ALONE, and
 * tools/datacheck.py is silent -- whole-file conversion, no split, no exports.
 *
 * ===================== WHAT THE 14 WERE, AND THE FIX =====================
 *
 * The park's SYMPTOM was right: `mov r8,r1 / mov r6,r0` in the ROM against our
 * `mov r8,r0 / mov r6,r1`, i.e. the two parameters exchanged between the two
 * callee-saved registers, and the exchange then renames every later
 * instruction that names either one.  Its VERDICT -- "the parameters arrive
 * where the ABI puts them and nothing in the body reorders which one gets
 * saved first" -- is REFUTED.  A field-store order does exactly that.
 *
 * IT IS NOT global.c.  `.18.greg` opens with `;; 0 regs to allocate:` -- the
 * whole function is a single basic block, so every pseudo is a LOCAL-ALLOC
 * quantity and allocno_compare never runs.  The ordering is QTY_CMP_PRI,
 * local-alloc.c:1496-1498, the same `floor_log2(n_refs)*n_refs*size /
 * (death - birth)` shape but over the quantity's BIRTH AND DEATH INSN NUMBERS.
 *
 * `fx`'s death is the `a->f8` store and `fy`'s is the `a->f10` store.  So the
 * order of those two stores sets the two quantities' deaths, hence their
 * priority order, hence which of r6 / r8 each takes.  With `a->f10` first
 * (the park) `fy` dies early, wins r6, and `fx` is pushed to r8.  With
 * `a->f8` first the pair comes out the ROM's way and the figure drops 14 -> 9
 * with the first difference moving from index 3 to index 24 -- both prologue
 * saves correct.
 *
 * THE SECOND HALF IS THE EMISSION ORDER OF THE SAME BLOCK, AND IT IS NOT
 * sched2'S TO CHOOSE.  All six stores go to disjoint constant offsets off r5,
 * so no two are memory-dependent; but after reload the three VALUE groups --
 * the `mov r3,r8` copy that feeds `f10`, the `mov r3,#0x80 / lsl r3,#0x18`
 * pair that feeds `f38`/`f40`, and the `mov r3,#0` that feeds `f24`/`f2c` --
 * all write r3, and those register anti-dependences fix their relative order.
 * So the group order in the output IS the group order in the source.  The ROM
 * emits the r8 copy FIRST; therefore `a->f10` is the first of the three groups
 * in the source.  The lone `str r6,[r5,#8]` carries no r3 dependence and
 * sched2 moves it past `mov r0,#5` to the end of the block in BOTH streams,
 * which is why `f8` being first in the source is invisible in the output.
 *
 * Those two constraints leave exactly one order, and it is byte-identical:
 * f8, f10, then the 0x80<<24 pair, then the zero pair.  Full sweep of the
 * twelve group interleavings that keep f8 before f10
 * (`scratch_elev/b338/B/b_gen_p2b.py`):
 *     f8, f10, A, Z        0   <- this body
 *     f8, A, f10, Z        6      A, f8, f10, Z   6
 *     f8, f10, Z, A        7
 *     the other eight      9
 *
 * MEASURED INERT, ALL AT 14 (so the park's negative list stands and grows):
 *   * batch 336's "shift the parameter in place" lever, four ways:
 *     `y <<= 16; x <<= 16;`, `x <<= 16; y <<= 16;`, and each parameter in place
 *     with the other into a fresh local.  All 14.  Once the store order is
 *     right it is still inert -- `f8`-first is 9 with fresh locals AND with
 *     either in-place spelling.
 *   * swapping the two shift statements; swapping the declaration order of the
 *     two locals.  Both 14, as the park recorded.
 *   * autosweep: both cheap axes CLOSED -- 8 flag groups (best 14; -O1 and
 *     -fno-schedule-insns2 each 27 of 53) and 20 anchor placements (best 14).
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x24 - 0x14];
    int f24;
    unsigned char pad28[4];
    int f2c;
    unsigned char pad30[8];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__Func_8093554(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);

void OvlFunc_897_200ac1c(int x, int y)
{
    struct A *a;
    int fx;
    int fy;

    a = __Func_8093554();
    fy = y << 16;
    fx = x << 16;
    __Func_80933f8(fx, -1, fy, 1);
    __Func_8091200(0, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    a->f8 = fx;
    a->f10 = fy;
    a->f38 = 0x80 << 24;
    a->f40 = 0x80 << 24;
    a->f24 = 0;
    a->f2c = 0;
    __WaitFrames(5);
    __Func_800fe9c();
    __WaitFrames(5);
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(0x14);
    __WaitFrames(0x1e);
}
