/* OvlFunc_969_200871c  --  0x0200871c.  MATCHED, BYTE-EXACT AT PLAIN -O2.
 *
 *   OK OvlFunc_969_200871c -- 376 bytes, 149 encodings and 34 relocations identical
 *
 * NO MAKEFILE FLAG ROW.  This is a default-flags match; do not add the object to
 * CSE_CFLAGS or any other group.
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_a_a.s is
 * the whole file -- ONE function, no data, so datacheck.py reports nothing and
 * split_s.py is not needed.  overlays/rom_7f6e64/overlay.ld:35 names the object
 * once, `asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_a_a.o(.text)`, and no .ld
 * .data/.bss list names it.  The landing is one line, asm/ -> src/.
 * No label needs `.global`.
 *
 * PIN-FREE: shimcount.py reports no register pins, no .equ shims, no "+r"
 * barriers and no empty asm.  No fakematch.txt row.
 *
 * TWO SPELLINGS ARE LOAD-BEARING.
 *
 *   1. THE gState OFFSET IS A LOCAL int, NOT A LITERAL.  Written
 *      `*(short *)(gState + 0x1c2)` gcc folds base+offset into ONE pool word
 *      (`ldr r3, =gState+450`, an R_ARM_ABS32 with an addend) and the whole
 *      switch dispatch shifts.  The ROM pools the BARE symbol and materialises
 *      the offset in a register -- `mov r2, #0xe1 / lsl r2, #1 / add r3, r2` --
 *      which is what an offset that is a PSEUDO at expand time produces:
 *      address legitimisation runs at expand and splits the address, then cse
 *      propagates the constant into the register it already allocated.  So
 *      `k = 0x1c2; *(short *)((char *)gState + k)` is the form.  Worth 93 of
 *      159 -> (with item 2) 12 of 159.
 *
 *      NOTE the LIVE RANGE matters as much as the spelling: `k` is read ONCE
 *      here.  The same lever in OvlFunc_959_200938c needs `k` assigned INSIDE
 *      each arm, because a `k` live across a call takes a callee-saved high
 *      register and costs a whole extra allocno.
 *
 *   2. THE FOUR __GiveItemTo CALLS ARE FOUR SEPARATE STATEMENTS.  Written as
 *      an if/else-if chain assigning one `n` and a single call, gcc emits ONE
 *      call site; the ROM has TWO.  With four call statements jump.c's
 *      cross-jumping merges the identical tails of the first three arms (each
 *      ends `mov r1, #0x41 / bl __GiveItemTo / b .L84c`) and CANNOT merge the
 *      fourth, whose tail has no branch because it falls through to the join.
 *      That is exactly the ROM's shape: three arms sharing one call, the `n=3`
 *      arm carrying its own.
 *
 * ONE MORE TRAP, worth recording because it cost a measurement.  The tail
 * actor pointer must be a FRESH local.  Reusing the loop's `a` put it in r5
 * (callee-saved, because `a` is live across the loop's calls) and produced an
 * extra `mov r5, r0`; a separate `b` stays in r0 as the ROM does.  That single
 * spelling is the difference between 12 of 159 and exact.
 *
 * The switch is a sparse 6-way on a signed short; gcc's balanced decision tree
 * reproduces the ROM's `cmp #3 / bgt` range tests directly out of stmt.c's
 * emit_case_nodes.  Case bodies are emitted in SOURCE order, and the ROM's
 * block order reads 1, 2, 3, 0x5d, 4, 9 -- which is the order they are written
 * in below, and is how the source order was recovered.
 */
struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x18 - 0x10];
    int f18;
    unsigned char pad1c[0x23 - 0x1c];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern unsigned char gState[];
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __CutsceneStart(void);
extern void __GiveItemTo(int who, int item);
extern void __Func_8091e9c(int a);
extern void __Func_8092b08(int slot, int a);
extern void OvlFunc_969_20088b4(void);
extern void OvlFunc_969_200a360(void);
extern void OvlFunc_969_200b8c0(void);
extern void OvlFunc_969_200b8dc(void);
extern void OvlFunc_969_200b924(void);
extern int OvlFunc_969_20084bc(void);

int OvlFunc_969_200871c(void)
{
    struct Actor *a;
    unsigned int i;
    int n;
    struct Actor *b;
    int k;

    k = 0x1c2;
    __SetFlag(0x144);
    __WaitFrames(1);
    __SetFlag(0x110);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
    __MapActor_GetActor(0xa)->f18 = 0xffff0000;
    __MapActor_GetActor(0xb)->f18 = 0xffff0000;
    for (i = 0xc; i <= 0x11; i++) {
        a = __MapActor_GetActor(i);
        __Actor_SetSpriteFlags(__MapActor_GetActor(i), 0);
        __Func_8092b08(i, 1);
        a->f55 = 4;
        a->f23 |= 2;
        a->fc = 0x8000;
    }
    switch (*(short *)((char *)gState + k)) {
    case 1:
        if (__GetFlag(0x109) == 0)
            OvlFunc_969_20088b4();
        break;
    case 2:
        OvlFunc_969_200a360();
        break;
    case 3:
        OvlFunc_969_200b8c0();
        break;
    case 0x5d:
        OvlFunc_969_200b8dc();
        break;
    case 4:
        OvlFunc_969_200b924();
        break;
    case 9:
        __CutsceneStart();
        if (__GetFlag(0x345))
            __GiveItemTo(0, 0x41);
        else if (__GetFlag(0x346))
            __GiveItemTo(1, 0x41);
        else if (__GetFlag(0x347))
            __GiveItemTo(2, 0x41);
        else
            __GiveItemTo(3, 0x41);
        __Func_8091e9c(9);
        break;
    }
    if (__GetFlag(0x109)) {
        n = OvlFunc_969_20084bc();
        if (n) {
            b = __MapActor_GetActor(n);
            if (b)
                b->f55 = 0;
        }
    }
    return 0;
}
