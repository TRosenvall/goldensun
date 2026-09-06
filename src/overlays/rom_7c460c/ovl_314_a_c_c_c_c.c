/* Cluster OvlFunc_939_2008d30..OvlFunc_939_2008eb0 extracted from
 * goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_c_c.s.
 *
 * The .s held ONLY these two functions and no data, so no split was needed and
 * NO LINKER EDIT is needed: overlays/rom_7c460c/overlay.ld keeps its single
 * line `asm/overlays/rom_7c460c/ovl_314_a_c_c_c_c.o(.text)` and the Makefile's
 * cross-dir rule `asm/%.o: src/%.c` builds that .o from this file. Default
 * GCC296_CFLAGS: NO FLAG GROUP -- no explicit and no wildcard rule in the
 * Makefile reaches this stem, and objcmp's own flag detection reports [].
 *
 * Both functions carry the SAME tell -- a constant used twice, commoned into a
 * callee-saved register behind a push the ROM does not have -- and they take
 * OPPOSITE recorded remedies. Neither recorded remedy would have shipped this
 * file whole; one lever spelling does both. See the NEW FINDING at the end.
 *
 * ---- OvlFunc_939_2008d30 -- fifteen __Func_80105d4 calls -------------------
 *
 * A straight-line call script whose fifth and sixth arguments cycle through
 * (1, 0xe), (0x21, 0xe), (1, 0x2e). All four constants sit in CALLEE-SAVED
 * registers the prologue pushes -- r6=1, r5=0xe, r8=0x21, r10=0x2e -- which is
 * the tell that the source names them and carries them across every call
 * (docs/elevation.md "A value in a CALLEE-SAVED register around a call is shared across both
 * calls"). Four plain `int` locals, each assigned immediately before its FIRST
 * use, reproduce all fifteen sites including the per-site store order: the
 * high-register value is copied into r3 and stored BEFORE the r0-r3 argument
 * moves, the low-register one stored after. That needed no lever; it falls out
 * of which local is which.
 *
 * The register assignment falls out too. REG_ALLOC_ORDER's call-saved sequence
 * is r5, r6, r7, r8, r10, r9, r11. 1 and 0xe have ten references each, but 1
 * lives one instruction longer at BOTH ends, so it takes the lower-priority
 * slot (r6) and 0xe takes r5; likewise 0x21 (r8) against the longer-lived
 * 0x2e (r10). Nothing had to be said about it in the C.
 *
 * THE ONE LEVER: `p0 = 0x202;` through `register int p0 __asm__("r0")`, at the
 * SECOND __GetFlag. The flag id 0x202 is used TWICE -- the guard and the
 * trailing __SetFlag -- and at -O2 the rerun-CSE pass commons the two pool
 * loads into r7 (`ldr r7, .L5+4 / mov r0, r7` ... `mov r0, r7`), which costs
 * one instruction AND a fourth callee-saved register, so the prologue comes out
 * `push {r5,r6,r7,lr}` against the ROM's `push {r5,r6,lr}`. Prologue read by
 * CONTENT, not width: the encoding diff opens at index 0, ref b560 / ours b5e0,
 * and 157 of 164 halfwords then disagree from the one-instruction shift.
 *
 * THE PIN SWEEP HAS A DIRECTION -- measured, on the FIRST of the two uses:
 *
 *   nothing pinned                                    157 of 164
 *   r0 pinned at the trailing __SetFlag only          157 of 164 (identical)
 *   r0 pinned at the SECOND __GetFlag                 EXACT
 *   r0 pinned at both __GetFlag calls and __SetFlag   EXACT (2 pins inert)
 *
 * The FIRST __GetFlag needs no pin: gcc already puts 0x201 straight into r0.
 *
 * ---- OvlFunc_939_2008eb0 -- gState guard, then a cutscene -----------------
 *
 * Head is the established gState shape: a walked `unsigned char *` with a
 * destructive `+=`, so symbol and offset cannot fold into one pool entry and
 * `mov r2,#0x93 / lsl r2,#2 / add r3,r2 / mov r2,#0 / ldrsh r3,[r3,r2]` comes
 * back verbatim. Taken from the sibling
 * src/overlays/rom_7c460c/ovl_314_a_c_a_c_c_c_c_c.c, which reads the same
 * halfword; that file is a template for the head and for nothing else here.
 *
 * THE ONE LEVER: both __MapActor_Emote calls pass 0x100. gcc commons the
 * `mov #0x80 / lsl #1` pair into r5 and HOISTS IT ABOVE __CutsceneStart; the
 * ROM rebuilds it at each site. A REPEATED CONSTANT CSE'd WITH NO DOMINATING
 * BRANCH IS NOT BLOCKED: r1 is call-clobbered and cannot survive a `bl`, so
 * binding the value to r1 forces the rebuild. Two pins are load-bearing:
 *
 *   plain int locals, one per site, no pin           8 of 95 (hoist survives)
 *   r1 pinned only, r0/r2 literals                   2 of 95 (CSE killed, the
 *                                                    ROM's interleave lost --
 *                                                    ref `mov r0,#8` where we
 *                                                    emit `lsl r1,#1`)
 *   r1 + r2 pinned, r0 literal                       3 of 95
 *   r0 + r1 pinned                                   EXACT
 *   r0 + r1 + r2 pinned                              EXACT (the r2 pin is
 *                                                    INERT and is not shipped)
 *
 * The two sites want DIFFERENT orders -- `q1, q0, q1<<=1, q2` at the first and
 * `q1, q1<<=1, q2, q0` at the second. Read off the listing, not guessed.
 *
 * The trailing three __Func_8010704 calls share 0xb in r5 (pushed) as the
 * sixth argument while REBUILDING 0xb as the second argument each time; one
 * named local for the sixth and a bare literal for the second is what the ROM
 * says, and it needs nothing else.
 *
 * ---- NEW: a call-clobbered PIN is a THIRD remedy for the commoned-constant
 *      tell, and it reaches both halves of the guessed distinction ----------
 *
 * docs/elevation.md "The commoned-constant tell has TWO remedies and they are not
 * interchangeable" lists exactly two -- CSE_CFLAGS, and separate named locals
 * -- and guesses the distinction is WHERE the commoning happens: "commoning
 * across a branch wants the flag, commoning within a block wants separate
 * locals" (five cases to one). docs/elevation.md "`GetFlag(id)` guarding a block that ends
 * `SetFlag(id)` means `CSE_CFLAGS`" says separate locals defeat that shape in
 * none of five. This file is one specimen of each half, measured:
 *
 *   fn                  shape                    CSE_CFLAGS  locals  r0/r1 pin
 *   OvlFunc_939_2008d30 GetFlag(id)/SetFlag(id)  EXACT       n/a     EXACT
 *                       across a branch
 *   OvlFunc_939_2008eb0 same constant at two     8 of 95     8 of 95 EXACT
 *                       calls in ONE block
 *
 * Two things are new. (1) The r0 pin is an exact SUBSTITUTE for CSE_CFLAGS on
 * the GetFlag/SetFlag shape -- the standing note says "the fix is the flag
 * group, not the C", and here the C reaches it, with no Makefile edit.
 * (2) The "within a block wants separate locals" half now has a COUNTER-
 * EXAMPLE: separate locals leave the hoist untouched (8 of 95) and so do
 * -fno-rerun-cse-after-loop, -fno-gcse and -fno-cse-follow-jumps (8 of 95 each,
 * all three measured), while the pin is exact. So the distinction does not
 * predict the remedy; the pin covered both halves here.
 *
 * The mechanism is why it generalises: naming a value only tells gcc the value
 * exists, and gcc is free to keep one copy in a call-saved register. Binding it
 * to r0-r3 tells gcc WHERE it lives, and no value in r0-r3 can cross a `bl`, so
 * a second use after a call has to be rebuilt. That is a property of the
 * ABI, not of this overlay -- and it is worth one screen before parking a
 * commoned-constant function on "neither recorded remedy takes".
 *
 * VERIFIED with tools/objcmp.py against the ORIGINAL asm/ path,
 * asm/overlays/rom_7c460c/ovl_314_a_c_c_c_c.s:
 *
 *   OK OvlFunc_939_2008d30 -- 384 bytes, 164 encodings and 26 relocations identical
 *   OK OvlFunc_939_2008eb0 -- 236 bytes, 95 encodings and 22 relocations identical
 *
 * and WHOLE FILE against the same .s assembled entire (both functions, pool
 * order and all): 620 bytes, 259 encodings and 48 relocations identical.
 * `make compare` was not run -- it is out of scope for this session and remains
 * the gate.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_WaitMovement(int a);
extern void __MapActor_SetIdle(int a);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_939_2008d30(void)
{
    int a;
    int b;
    int c;
    int d;
    register int p0 __asm__("r0");

    if (__GetFlag(0x201))
        return;
    p0 = 0x202;
    if (__GetFlag(p0))
        return;
    __MapActor_SetPos(0x13, 0, 0);
    __PlaySound(0xd2);
    __WaitFrames(1);
    a = 1;
    b = 0xe;
    __Func_80105d4(0x20, 0x2d, 3, 4, a, b);
    c = 0x21;
    __Func_80105d4(0x23, 0x2d, 3, 4, c, b);
    d = 0x2e;
    __Func_80105d4(0x26, 0x2d, 3, 4, a, d);
    __WaitFrames(0xa);
    __Func_80105d4(0x29, 0x2d, 3, 4, a, b);
    __Func_80105d4(0x2c, 0x2d, 3, 4, c, b);
    __Func_80105d4(0x2f, 0x2d, 3, 4, a, d);
    __WaitFrames(0xa);
    __Func_80105d4(0x32, 0x2d, 3, 4, a, b);
    __Func_80105d4(0x35, 0x2d, 3, 4, c, b);
    __Func_80105d4(0x38, 0x2d, 3, 4, a, d);
    __WaitFrames(0xa);
    __Func_80105d4(0x20, 0x31, 3, 4, a, b);
    __Func_80105d4(0x23, 0x31, 3, 4, c, b);
    __Func_80105d4(0x26, 0x31, 3, 4, a, d);
    __WaitFrames(0xa);
    __Func_80105d4(0x29, 0x31, 3, 4, a, b);
    __Func_80105d4(0x2c, 0x31, 3, 4, c, b);
    __Func_80105d4(0x2f, 0x31, 3, 4, a, d);
    __WaitFrames(0xa);
    __SetFlag(0x202);
}

void OvlFunc_939_2008eb0(void)
{
    unsigned char *g;
    int s;
    register int q0 __asm__("r0");
    register int q1 __asm__("r1");

    g = (unsigned char *)&gState;
    g += 0x93 << 2;
    if (*(short *)g != 0)
        return;
    __CutsceneStart();
    q1 = 0x80; q0 = 8; q1 <<= 1;
    __MapActor_Emote(q0, q1, 2);
    q1 = 0x80; q1 <<= 1; q0 = 9;
    __MapActor_Emote(q0, q1, 0xf);
    __CutsceneWait(0x1e);
    __Func_809218c(8, 0x98, 0xa8);
    __Func_809218c(9, 0xa8, 0xa8);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    __MapActor_SetIdle(8);
    __MapActor_SetAnim(8, 0);
    __Func_8092adc(8, 0xc0 << 6, 0);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(9, 0);
    __Func_8092adc(9, 0xa0 << 7, 0);
    __MessageID(0x24da);
    __ActorMessage(8, 0);
    __SetFlag(0x90 << 2);
    s = 0xb;
    __Func_8010704(6, 0xb, 1, 1, 7, s);
    __Func_8010704(6, 0xb, 1, 1, 8, s);
    __Func_8010704(6, 0xb, 1, 1, 9, s);
    __CutsceneEnd();
}
