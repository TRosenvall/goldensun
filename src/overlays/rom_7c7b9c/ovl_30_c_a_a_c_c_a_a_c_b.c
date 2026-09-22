/* Cluster OvlFunc_943_200b5ec..OvlFunc_943_200b5ec extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a_c.s.
 *
 * Total .text for this TU = 292 bytes (= 0x124). Never attempted before batch 279.
 * NO PINS, NO FLAGS, NO volatile -- and the last two instructions came from a `union`, which is
 * why this file is worth reading even though the function is unremarkable.
 *
 * 1. `bhi` RATHER THAN `bgt` ON THE SWITCH SELECTOR MEANS THE TABLE IS `unsigned int[]`, not
 *    `int[]`. Read the branch condition before typing a jump table.
 *
 * 2. `L5b90[i] + t*4 + t*2` FOLDS TO `6*t` (`lsl / add / lsl`); the ROM keeps two `add`s. The fix
 *    is TWO STATEMENTS ENDING IN AN EXPRESSION:
 *        v = L5b90[i] + t * 4;
 *        *(int *)(a + 0x10) = v + t * 2;
 *    Making BOTH arms multi-statement instead permutes r9/r10/r11 (22 differing) and duplicates
 *    the cross-jumped store (16) -- THE ASYMMETRY IS LOAD-BEARING, which is the batch-278 rule
 *    that two similar blocks can want different statement order, seen again.
 *
 * 3. THE LAST TWO: `ldr r3, =.L5b90` scheduled one slot BEFORE the `strh r0, [r1, #0x1e]`. This
 *    is NOT one of the five allocation entry points -- it is `.23.sched2` LIST ORDER, and the
 *    dump says so exactly. In the true branch insn 197 (the pool load) and insn 194 (the strh)
 *    are BOTH prio 7 and the tie falls to the strh; in the FALSE branch the pool load is prio 8
 *    against 7, because that block has no jump insn so its tail chain is one longer.
 *
 *    THE FIX IS TO GIVE THE STORE A REAL DEPENDENCE ON THE LOAD BY PUTTING THEM IN ONE ALIAS SET
 *    -- `union U5b90 { int i; short h; };` on the table, read as `L5b90[i].i`. Verified in
 *    `.23.sched2`: insn 260 gains successors 273 and 284 and rises to prio 8, winning the tie.
 *
 *    THAT IS A NEW USE OF AN OLD RULE. The recorded note says a missing ANTI-dependence is an
 *    ALIAS problem and that a union or typed struct field cures it, with `volatile` not helping.
 *    Here the missing dependence is a TRUE one, in the same direction, and the same cure works --
 *    so read the rule as being about alias SETS rather than about anti-dependences specifically.
 *
 *    Measured for that pair: `-fno-strict-aliasing` ALSO reaches 0 (same mechanism), but would
 *    need an ALIAS_CFLAGS Makefile row, so the union is preferred -- it is ordinary C and costs
 *    nothing. The union on the STORE side is also exact. `volatile` on the table 2; `volatile` on
 *    the store 2; a `char *`-cast table 4; -fno-schedule-insns2 40; and a 30-permutation sweep of
 *    the two arms' statement orders bottoms out at 2.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8092b54(int a, int b);
extern int __sin(int a);
extern unsigned int L5b70[] __asm__(".L5b70");
extern unsigned short L5b30[] __asm__(".L5b30");
extern unsigned short L5b40[] __asm__(".L5b40");
union U5b90 { int i; short h; };
extern union U5b90 L5b90[] __asm__(".L5b90");

void OvlFunc_943_200b5ec(int slot, int i, int flags)
{
    unsigned char *a;
    unsigned char *b;
    int t;
    int v;

    a = __MapActor_GetActor(slot);
    b = *(unsigned char **)(a + 0x50);
    if (!(flags & 2)) {
        switch (L5b70[i]) {
        case 1:
            L5b40[i] = L5b30[0];
            __Func_8092b54(slot, 8);
            break;
        case 2:
            L5b40[i] = L5b30[1];
            __Func_8092b54(slot, 9);
            break;
        case 3:
            L5b40[i] = L5b30[2];
            __Func_8092b54(slot, 0xa);
            break;
        case 4:
            L5b40[i] = L5b30[3];
            __Func_8092b54(slot, 0xb);
            break;
        }
    }
    if (flags & 1) {
        t = __sin(L5b40[i]);
        *(short *)(b + 0x1e) = __sin(L5b40[i] + 0x8000) >> 5;
        *(int *)(a + 0x10) = L5b90[i].i - t * 4 - t * 2;
    } else {
        t = __sin(L5b40[i] + 0x8000);
        *(short *)(b + 0x1e) = __sin(L5b40[i]) >> 5;
        v = L5b90[i].i + t * 4;
        *(int *)(a + 0x10) = v + t * 2;
    }
}
