/* OvlFunc_917_2009070 (0x02009070) -- NON-MATCHING: 98 encodings of 151 differ (objcmp).
 * ref 376 bytes / 151 encodings, ours 360 bytes / 145 encodings -- SIX INSTRUCTIONS SHORT.
 *
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_a_c_c.s holds TWO functions
 * (OvlFunc_917_2008488 first, this one second) and no data section, so a text split is
 * needed; the ROM function carries an INLINE POOL (`.align 2,0 / .L10d4: .word 0 /
 * .pool` between the `b .L10ec` and `.L10ec:`) which a split must preserve.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a4370/2009070.c asm/overlays/rom_7a4370/ovl_30_c_c_c_a_c_c.s --func OvlFunc_917_2009070
 * (in the container: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build ...)
 *
 * THE READING IS RIGHT AND THE WHOLE RESIDUE IS ONE REGION -- THREE STORES.  From
 * `__Func_8092b08` onward (65 of the 145 instructions: the three 0x55 byte stores, the
 * three 0xc word stores, the two SetAnim calls and all five `[0x59] |= 8` blocks) the
 * candidate matches the ROM register-for-register, offset by the six missing
 * instructions.  Everything that differs is upstream of it, in:
 *
 *     *(int   *)(iwram_3001ebc + (0xe0 << 1)) = 0x81 << 2;      -- 0x1c0 and 0x204
 *     *(short *)(gState + 0x240) = 0x28;
 *     *(short *)(gState + 0x242) = 4;
 *
 * THE ROM RELATES 0x1c0 -> 0x240; WE RELATE 0x1c0 -> 0x204.  The ROM keeps 0x1c0 in r0,
 * builds 0x204 independently (`mov r2,#0x81 / lsl r2,#2`), then reuses r0 with
 * `add r0,#0x80` to reach 0x240 and forms BOTH gState addresses as register offsets
 * (`add r3,r2,r0` and `ldr r3,=0x242 / add r2,r3`).  gcc instead derives 0x204 from
 * 0x1c0 with `add r2,#0x44` and then, having no 0x240 in a register, uses one gState
 * base with `[r2]` / `[r2,#2]` -- four instructions fewer, and the different constant
 * state is what the rest of the function inherits.
 *
 * MEASURED, all against ref 151 / 376 bytes:
 *   plain literals throughout                            129 of 151, 147 insns, 368 B
 *   `int h` carriers for the 4 and the 6 (THIS)          101 of 151, 145 insns, 360 B
 *   THIS + block-scoped `int v = 0x81 << 2`               98 of 151, 145 insns, 360 B
 *   THIS + `register int q2 __asm__("r2")` pin on 0x204   98 of 151, 145 insns, 360 B
 *   THIS + pins on BOTH 0x1c0 and 0x204                  100 of 151, 145 insns, 360 B
 *   THIS + a shared `k = 0xe0 << 1` offset variable       118 of 151, 147 insns, 364 B
 *   THIS + `struct HalfWord` carrier for the zero        129 of 151, 146 insns, 364 B
 *
 * THE `int h` CARRIER IS THE ONE REAL FINDING AND IT CUTS BOTH WAYS IN ONE FUNCTION.
 * Three halfword stores, three different materialisations in the ROM:
 *   gState+0x240 = 0x28   ->  `ldr r1, =0x28`   (a POOL load: plain `short` store, i.e.
 *                             a HImode constant -- a bare literal is CORRECT here)
 *   gState+0x242 = 4      ->  `mov r3, #4`      (needs an `int` carrier)
 *   actor->f20   = 6      ->  `mov r5, #6`      (needs an `int` carrier)
 * Writing all three as bare literals pools all three (`ldrh rN, .L4`); giving all three
 * an `int` carrier would `mov` all three.  Only the mixed spelling is right, and it is
 * worth 28 encodings.  A PC-relative halfword pool load has no Thumb encoding, so the
 * ROM's `ldr r1, =0x28` for a value `mov` could reach IS the HImode-pool case -- do not
 * read it as a missing .sym symbol.
 *
 * `struct HalfWord` IS A FALSE POSITIVE HERE, and that is worth recording.  The ROM's
 * `ldr r6, .L10d4 @ 0` is a pooled zero feeding three `strb`, which is exactly the
 * signature docs/elevation.md's "A POOLED ZERO REACHING A `strb` IS THE `struct
 * HalfWord` CASE" points at.  Applying it costs 28 encodings (101 -> 129).  Ours emits
 * `mov r6, #0` and that is closer.
 *
 * `extern int __StartTask` is not applicable (no StartTask).  An `int` return type on
 * this function IS load-bearing: the epilogue is `mov r0,#0 / pop {r1} / bx r1`, so it
 * returns int and the `return 0;` is explicit, not implicit.
 *
 * NEXT: get 0x204 out of cse's related-value chain WITHOUT shortening 0x1c0's life, so
 * that 0x1c0 is still the cheapest source for 0x240 at the next use.  Neither a
 * block-scoped local nor an r2 pin does it (both still fold 0x240 into a gState
 * displacement).  Read the .16.cse2 dump for which constant cse elects as the base.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void __Func_8092950(int a, int b);
extern int __GetFlag(int id);
extern void __Actor_SetSpriteFlags(unsigned char *p, int f);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_917_2009768(int a);

int OvlFunc_917_2009070(void)
{
    unsigned char *A;
    unsigned char *B;
    unsigned char *C;
    int z;
    int h;

    A = __MapActor_GetActor(0xa);
    B = __MapActor_GetActor(0xe);
    C = __MapActor_GetActor(0xb);
    __WaitFrames(1);
    __Func_8092950(0xe, 0xf);
    { int v = 0x81 << 2; *(int *)(iwram_3001ebc + (0xe0 << 1)) = v; }
    *(short *)(gState + 0x240) = 0x28;
    h = 4;
    *(short *)(gState + 0x242) = h;
    z = 0;
    if (!__GetFlag(0x845))
        OvlFunc_917_2009768(3);
    h = 6;
    *(short *)(__MapActor_GetActor(8) + 0x20) = h;
    *(short *)(__MapActor_GetActor(9) + 0x20) = h;
    *(short *)(__MapActor_GetActor(0xc) + 0x20) = h;
    *(short *)(__MapActor_GetActor(0xd) + 0x20) = h;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    __Func_8092b08(8, 2);
    __Func_8092b08(0xe, 2);
    __Func_8092b08(9, 2);
    A[0x55] = z;
    *(int *)(A + 0xc) = 0xe0 << 13;
    C[0x55] = z;
    *(int *)(C + 0xc) = 0xe0 << 13;
    B[0x55] = z;
    *(int *)(B + 0xc) = 0xe0 << 13;
    __MapActor_SetAnim(9, 3);
    __MapActor_SetAnim(8, 3);
    __MapActor_GetActor(8)[0x59] |= 8;
    __MapActor_GetActor(9)[0x59] |= 8;
    __MapActor_GetActor(0xa)[0x59] |= 8;
    __MapActor_GetActor(0xb)[0x59] |= 8;
    __MapActor_GetActor(0xe)[0x59] |= 8;
    return 0;
}
