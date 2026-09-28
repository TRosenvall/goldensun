/* OvlFunc_926_200be58  --  whole-file conversion of
 * asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_c_c.s
 * (grep -ci func_start = 1, datacheck: no data -- so the file lands WHOLE:
 * no split, no linker-script edit, and deleting the .s is the whole landing).
 *
 * BYTE-IDENTICAL: objcmp --whole reports
 *     OK whole file -- 644 bytes, 267 encodings and 42 relocations identical
 * with `_AREA_3d` supplied as an absolute symbol.  Shipped it needs the
 * area.sym row below and carries ONE relocation the reference .s does not
 * (R_ARM_ABS32 _AREA_3d on the pool word the reference writes as `.word 0x3d`);
 * the linker resolves it to the same word, exactly as batch 253's
 * OvlFunc_925_2008b24 records for _AREA_3a.
 *
 * REQUIRES ONE area.sym ROW:   _AREA_3d = 0x3d;
 *
 * SYMBOL EVIDENCE for _AREA_3d, which is area.sym's own criterion verbatim plus
 * in-function control:
 *   - the ROM writes `ldr r3, =0x3d` where `cmp r2, #0x3d` would do, and the
 *     operand is the signed halfword at gState+0x1C0 -- the area id slot.
 *   - THIRTY-ONE other constants of 0xff or less in this same function are all
 *     built with `mov` (0xe0 0xe1 0x49 0x55 0x59 0x13 0x12 0x10 0xdf 0xaa 0x98
 *     0x54 0x1a 0xc4 0x16 0x18 0xd 0xe 0xf 0xa 0xb 8 9 6 5 4 3 2 1 0xc0 0x80).
 *     0x3d is the only one the ROM pools.
 *   - NEGATIVE CONTROL, measured: spelling it `0x3d` gives 263 instructions
 *     against 265 -- TWO SHORT, because the pool load collapses into
 *     `cmp r2, #0x3d` -- and 16 differing regions.  The literal is not merely
 *     equal-and-simpler, it is arithmetically unreachable.
 *
 * FOUR LEVERS, each measured by dropping it alone from the finished file
 * (tryc --align, out of 265):
 *
 * 1. THE UNION STORE, 5 -> 3 (and it is what closed region (a) entirely).
 *    `((union W *)(b + 0x1c0))->w = 0x209;` reaches alias set 0, so the
 *    `ldrsh` of gState[0xe1] becomes DEPENDENT on the store; that puts the
 *    store on the critical path and sched2 then interleaves
 *    `mov r1,#0xe1 / add r2,#0x49 / str / lsl r1,#1` the ROM's way instead of
 *    hoisting the whole index chain first.  A plain `*(int *)` store is in a
 *    different alias set and the dependence never forms.  This is the recorded
 *    "union member access is a sched2 ORDERING lever", and the reason it is
 *    needed here is visible in the diff as a four-instruction rotation.
 *
 * 2. `m`/`n` LOCALS AT THE THREE __Func_8010704 SITES, 17 -> 8.  Assigned
 *    immediately before each call, the two stack arguments are live together
 *    and get r3/r2 with both `mov`s ahead of both `str`s, which is the ROM.
 *    Written as literals gcc reuses r3 for both -- `mov r3 / str / mov r3 /
 *    str`.  Same instruction count, different register: a true distance that
 *    the count alone cannot see.
 *
 * 3. `px`/`pz` ASSIGNED AT THE TOP OF THE FUNCTION, 8 -> 6, for the one
 *    `__MapActor_SetPos(9, 0x98 << 16, 0xc4 << 17)` in the else arm.  As
 *    literals their values are `rtx_cost > 2` so `precompute_register_parameters`
 *    (calls.c:850) copies each to a pseudo BEFORE `load_register_parameters`
 *    runs, which puts arg0's `mov r0,#9` last and sched2 leaves it last.  As
 *    far-assigned locals the args are already REGs, precompute skips them, the
 *    r0 load comes first in LUID order, and sched2 lands it third -- the ROM.
 *    Assigning them immediately before the call does NOT work: they must be far
 *    enough away that cse propagates the constant into the copy at the call.
 *
 * 4. THE `0x98 << 17` SPLIT PLUS ONE r3 PIN, 6 -> 1 -> 0 bytes.  The ROM emits
 *    `mov r0,#0x98 / mov r1 / mov r2 / lsl r1 / lsl r2 / mov r3,#0xdf /
 *    lsl r0,#17` -- r0's shift LAST, six instructions after its own `mov`.  No
 *    spelling of a literal reaches that, because both precompute and
 *    load_register_parameters emit a constant's `mov` and `lsl` adjacently, and
 *    sched2's rank_for_schedule (haifa-sched.c:4029) then keeps them within one
 *    slot of each other.  Splitting the statement (`a0 = 0x98; ... a0 <<= 17;`)
 *    is necessary but not sufficient -- cse folds it straight back to one
 *    constant -- so the `"+r"` barrier is what makes the split survive: 6 -> 3.
 *    The remaining one was `mov r3,#0xdf` scheduled AFTER `lsl r0`; pinning r3
 *    fixes it, 3 -> 0.  Copied from src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_a_b.c,
 *    which uses exactly this barrier-plus-split shape for the same
 *    mov-early/shift-late signature.
 *
 * INERT, each measured: `do { } while (0)` anywhere in region (a) or (b);
 * naming the iwram offset or the stored value as a local; an int* carrier for
 * iwram_3001ebc; reading gState[0xe1] inline at each test instead of into `v`;
 * splitting `0x98` into a local WITHOUT the barrier; a `volatile int`
 * round-trip in place of the barrier; pinning r0, r1 or r2 at the
 * OvlFunc_common0_18 site (each a regression, 8 -> 10/11).
 *
 * SHIMS -- TWO, both in the OvlFunc_common0_18 block:
 *   register class:  1  -- register int q3 __asm__("r3")
 *   .equ class:      0
 *   other __asm__:   1  -- __asm__ volatile ("" : "+r" (a0))
 *
 * The `emit_case_nodes` source-order lever does NOT apply: the four-way
 * dispatch on gState[0xe1] is an `if / else if (v == 2 || v == 4) / else if`
 * chain, not a switch -- the ROM tests 1, then 2, then 4, then 3, which is the
 * short-circuit `||` shape and not a balanced case tree.
 */
extern short gState[];
extern unsigned char *iwram_3001ebc;
extern int _AREA_3d;

union W { int w; short h; };

struct Sub {
    unsigned char pad00[0x1e];
    short f1e;
};

struct Actor {
    unsigned char pad00[0xc];
    int f0c;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x3c - 0x20];
    int f3c;
    unsigned char pad40[0x50 - 0x40];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x6c - 0x5a];
    void (*f6c)(void);
};

extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091ff0(int a);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_common0_18(int x, int y, int z, int d);

extern void OvlFunc_926_200a7ec(void);
extern void OvlFunc_926_200aad0(void);
extern void OvlFunc_926_2008324(void);

int OvlFunc_926_200be58(void)
{
    struct Actor *a;
    unsigned char *b;
    int v;
    int m;
    int n;
    int px;
    int pz;

    px = 0x98 << 16;
    pz = 0xc4 << 17;

    if (gState[0xe0] == (int)&_AREA_3d) {
        b = iwram_3001ebc;
        ((union W *)(b + 0x1c0))->w = 0x209;
        v = gState[0xe1];
        if (v == 1) {
            if (__GetFlag(0x88f) != 0) {
                __MapActor_SetAnim(8, 6);
            } else {
                __MapActor_SetAnim(8, 5);
                if (__GetFlag(0xf14) != 0 && __GetFlag(0x893) == 0
                    && __GetFlag(0x109) == 0)
                    OvlFunc_926_200a7ec();
            }
        } else if (v == 2 || v == 4) {
            __ClearFlag(0x12f);
            if (__GetFlag(0x895) == 0) {
                a = __MapActor_GetActor(0x13);
                a->f55 = 0;
                a->f0c = 0xc0 << 12;
                a->f3c = 0xc0 << 12;
                a->f18 = 0xcccc;
                a->f1c = 0x80 << 8;
                a->f50->f1e = 0x80 << 8;
                if (__GetFlag(0x89a) != 0) {
                    __MapActor_SetPos(0x12, 0xf8 << 16, 0xd0 << 16);
                    if (__GetFlag(0x89b) == 0) {
                        __MapActor_SetPos(0x10, 0x80 << 17, 0xf0 << 16);
                        __MapActor_GetActor(0x12)->f6c = OvlFunc_926_2008324;
                        __MapActor_GetActor(0xd)->f6c = OvlFunc_926_2008324;
                        __MapActor_GetActor(0xe)->f6c = OvlFunc_926_2008324;
                        __MapActor_GetActor(0xf)->f6c = OvlFunc_926_2008324;
                        __MapActor_GetActor(0x10)->f6c = OvlFunc_926_2008324;
                    }
                }
            } else {
                a = __MapActor_GetActor(0x13);
                a->f55 = 0;
                a->f0c = 0xc0 << 12;
                a->f3c = 0xc0 << 12;
                a->f18 = 0xcccc;
                a->f1c = 0x80 << 8;
                a->f59 |= 8;
                a->f50->f1e = 0x80 << 8;
                m = 0xe;
                n = 0xa;
                __Func_8010704(0xe, 0xb, 1, 1, m, n);
            }
            {
                int a0, a1, a2;
                register int q3 __asm__("r3");

                a0 = 0x98;
                __asm__ volatile ("" : "+r" (a0));
                a1 = 0xc0 << 13;
                a2 = 0xe0 << 16;
                q3 = 0xdf;
                a0 <<= 17;
                OvlFunc_common0_18(a0, a1, a2, q3);
            }
            __MapActor_SetAnim(0xa, 5);
            __MapActor_SetAnim(0xb, 5);
        } else if (v == 3) {
            __ClearFlag(0x12f);
            if (__GetFlag(0x895) == 0) {
                OvlFunc_926_200aad0();
            } else if (__GetFlag(0x8b2) == 0) {
                __MapActor_SetPos(8, 0, 0);
                __MapActor_SetPos(9, 0, 0);
            }
        }
    } else {
        __Func_8091ff0(0xaa);
        __MapActor_GetActor(9)->f59 |= 0x10;
        if (gState[0xe1] == 3 && __GetFlag(0xf14) != 0
            && __GetFlag(0x894) == 0)
        {
            m = 0xa;
            n = 0x18;
            __Func_8010704(0xa, 0x54, 1, 1, m, n);
        }
        if (__GetFlag(0x892) != 0) {
            __MapActor_SetPos(9, px, pz);
            __Func_8092adc(9, 0, 0);
            m = 0xa;
            n = 0x16;
            __Func_8010704(0xa, 0x1a, 1, 1, m, n);
        }
    }
    return 0;
}
