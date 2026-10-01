/* OvlFunc_936_2008590 (0x02008590) -- NON-MATCHING, 1452 of 1585 differ (objcmp,
 * production flags, SATURATED -- see below).  1,528 instructions.  Never attempted.
 *
 * COUNT IS EXACT.  ref 1585 encodings, ours 1585.  SIZE ref 4092 bytes, ours 4096 --
 * FOUR BYTES LONG, and it is one duplicated pool word, not an instruction: 406 `bl` on
 * both sides, 414 relocations against our 415, and the one surplus is a second copy of
 * gScript_936__0200be00 that falls into a different pool than the ROM's.
 * aligncmp: aligned-equal 1236 (78.0% of ref), 522 differing/ins/del in 291 hunks.
 * objcmp's 1452 is SATURATED and must not be read as a distance.
 * shimcount: 0 shims -- NO PIN, NO FAKEMATCH ROW NEEDED.
 *
 * Verify with (this file, where it sits today):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7c097c/2008590.c \
 *     asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c_c.s --func OvlFunc_936_2008590
 * Intended install path is src/non_matching/ovl_7c097c/2008590.c; substitute that path
 * for the scratch one after the move.
 *
 * NO SPLIT IS NEEDED, AND THAT IS UNUSUAL FOR THIS BAND.
 * asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c_c.s holds exactly ONE function
 * (`grep -c thumb_func_start` = 1) and tools/datacheck.py reports no data section, so
 * the file is already one-function and its slot at overlays/rom_7c097c/overlay.ld:35
 * takes the .c directly.  split_s.py is not involved.  Whoever lands this should check
 * that first -- it removes the whole class of split mistakes from the job.
 *
 * ===== THE PROGRAM IS PROVEN RIGHT =====
 *
 * THE DISTINCT-CONSTANT SET: all 42 of the reference's entries are present on our side
 * and NOTHING the reference has is missing -- every speed pair, the five message bases
 * (0x1a91 0x1a92 0x1a9e 0x1aa2 0x1ab2), both save bits (0x910 0x911), the seven
 * gScript_936 bases, .L4948, OvlFunc_936_2009f14 and iwram_3001ebc.  Ours carries ONE
 * entry the reference lacks: a pooled 0 (see the carrier section).
 *
 * THE RELOCATION SEQUENCE: every one of the 406 R_ARM_THM_CALL entries is in identical
 * order on both sides -- not one call out of place in a 1,528-instruction cutscene.
 * The only diff hunks are R_ARM_ABS32 pool words moving between pools.  No
 * `_call_via_rN` veneer on either side (0 and 0).
 *
 * ===== A LONG `bl` TO A LOCAL LABEL IS A BRANCH, NOT A CALL (new, and it decides the
 * ===== WHOLE CONTROL-FLOW READING)
 *
 * The reference opens with TWO of these:
 *     bl __GetFlag / cmp r0,#0 / bne .L5ae / bl .L151c
 *   .L5ae:
 *     ldr r0,=0x911 / bl __GetFlag / cmp r0,#0 / beq .L5bc / bl .L151c
 *
 * `bl .L151c` is a `bl` to a LABEL INSIDE THE SAME FUNCTION, 1,520 instructions away.
 * It is NOT a call and there is no function there.  Thumb-1's unconditional `b` has an
 * 11-bit range (+/- 2 KB) and this function is 4 KB long, so for an in-function jump
 * out of `b` range gcc-2.96's Thumb backend emits `bl`, which reaches +/- 4 MB -- it
 * clobbers lr, which is dead on a path heading for the epilogue.  gcc then INVERTS each
 * condition and branches over the `bl` with `bne`/`beq` to a label one instruction
 * ahead, which is why both guards look backwards.
 *
 * READ IT AS ONE `if`/`else` AND IT COMES OUT EXACTLY:
 *     if (__GetFlag(0x910) != 0 && __GetFlag(0x911) == 0) { the whole cutscene }
 *     else { __PlaySound(0x7b); __Func_8091e9c(...); __MapTransitionOut();
 *            __WaitMapTransition(); }
 *     __CutsceneEnd();
 * The `else` arm IS `.L151c`, and `.L1538` is the shared tail both arms fall into.
 * TAKING `bl .L151c` FOR A CALL WOULD HAVE COST THE FUNCTION -- it reads as a call to
 * a non-existent helper, and no amount of probing recovers from that.  Four-digit `.L`
 * labels being safe as externs (they contain a hex letter) is a SEPARATE fact and does
 * not apply here: `.L151c` and `.L1538` are gcc's own labels in this very function.
 *
 * ===== WHICH POPULATION: ORDINARY PER-REGION STRUCTURE, AND THE CONTRAST WITH 2008488 =
 *
 * The brief asked which of the two band-800plus populations dominated.  For THIS target
 * it is the ORDINARY one, and that is the opposite of its batch-mate OvlFunc_917_2008488
 * -- worth stating plainly because the two were briefed as one group:
 *
 *   OvlFunc_917_2008488  1122 insns  reference uses 2 call-saved regs, 0 high regs
 *                        residue = cse1 commoning constants we cannot deny it
 *   OvlFunc_936_2008590  1528 insns  reference uses r5 r6 r7 AND r8 (high-save
 *                        prologue), and the residue is ORDINARY: halfword carriers,
 *                        a pointer read once, a walked message base
 *
 * The reference here pushes {r5, r6, r7, lr} plus `mov r7,r8 / push {r7}` and mentions
 * r5 41 times, r6 10, r7 8, r8 5.  FOUR call-saved quantities is a shape reachable from
 * ordinary C, so the levers that work at 500 instructions all still work -- and three
 * of them paid.  INSTRUCTION COUNT PREDICTED NEITHER FUNCTION'S POPULATION; the
 * reference's OWN register pressure did.  THAT is the triage axis to add to
 * docs/band-800plus.md section 1: before assigning, grep the reference for the
 * high-save prologue.  A reference WITHOUT one is the hard population, because it means
 * the ROM's compiler kept to two or three call-saved registers and ours will not.
 *
 * ===== WHAT PAID, MEASURED =====
 *
 * All rows against ref 4092 bytes / 1585 encodings.  Size-and-count ranked first.
 *
 *   candidate                                        size   count   aligned   hunks
 *   all literals                                     +28    +6      76.9%     299
 *   int carriers on all 5 halfword VALUE stores
 *     and all 3 halfword ZERO stores, shared var     -16     -7      77.9%     293
 *   same, one carrier variable PER VALUE             -16     -7      75.6%     293
 *   same, two 0xd0 stores split into two variables   -16     -7      75.6%     293
 *   5 VALUE carriers, zeros left as literals         +16     +4      77.2%     297
 *   same with shared carrier variables               +16     +4      77.2%     297
 *   + zero carrier on the SENTINEL site only (THIS)   +4    EXACT    78.0%     291
 *   + zero carrier on the second site only           +4    EXACT    74.5%     297
 *   + zero carrier on the third site only            +8      +1      77.4%     297
 *   + carriers on sentinel and second                 -8     -4      75.3%     294
 *   + carriers on sentinel and third                  -4     -3      78.2%     291
 *   + carriers on second and third                    -4     -3      74.8%     296
 *
 * THE HImode CARRIER IS NOT ALL-OR-NOTHING, AND THE SPLIT IS 5-OF-5 AND 1-OF-3.
 * The sibling-overlay precedent (src/non_matching/ovl_7a4370/2009070.c) already said
 * "only the mixed spelling is right"; this function says the mixture is not even per
 * CONSTANT, it is per SITE.  Five `strh` of a non-zero halfword ALL want an `int`
 * carrier (0xd0 << 8 twice, 0xa0 << 7, 0xc0 << 8, 0x80 << 7 -- the ROM builds each with
 * `mov`/`lsl`).  Three `strh` of ZERO: only ONE wants a carrier, and giving carriers to
 * all three overshoots by ELEVEN instructions.
 *
 * WHY, and this is the mechanism worth carrying: Thumb-1 `ldrsh` HAS NO IMMEDIATE-OFFSET
 * FORM.  Every signed-halfword load in this function is `mov rN,#0 / ldrsh rX,[rY,rN]`,
 * so the ROM is already littered with registers holding 0 for free, and cse hands one
 * of them to a nearby `strh` of 0 with no carrier in the source at all.  Only the site
 * with NO `ldrsh` near it -- the sentinel store `*w = 0` that precedes the
 * `while (*w == 0) __WaitFrames(1);` spin loop -- has to say 0 in the source.  THE
 * POOLED-ZERO LEVER IS REAL BUT ITS PRECONDITION IS LOCAL: apply it at a site, measure,
 * and do NOT sweep it across the function.  Ours is still one pooled 0 away from a
 * clean pool set, which is where the remaining 4 bytes live.
 *
 * ===== THE READ-ONCE GLOBAL POINTER, CONFIRMED AS A PROOF =====
 *
 * `ldr r3,=iwram_3001ebc / ldr r7,[r3]` is the second and third instruction of the
 * function, and r7 then survives to instructions 1513 and 1548 across FOUR HUNDRED
 * calls.  A call clobbers memory, so cse can NEVER re-use a global's load across one;
 * a value that survives 400 of them was read ONCE by the source.  So
 * `unsigned char *p = iwram_3001ebc;` at the top, with THREE dereferences
 * (`*(short *)(p + (0xb6 << 1))`) -- the opening branch, the closing branch and the
 * `else` arm.  This spelling was right first time and never needed a probe, which is
 * the opposite of the same lever's behaviour on OvlFunc_917_2008488, where naming the
 * base was BYTE-IDENTICAL to not naming it.  THE DISCRIMINATOR IS WHICH THING CROSSES
 * THE CALL: here it is the pointer VALUE (so a local is forced), there it was only the
 * global's ADDRESS (a constant, which cse commons for free).
 *
 * ===== THE MESSAGE BASE IS ONE WALKED LOCAL, NOT FOUR CONSTANTS =====
 *
 * The reference does `ldr r5,=0x1a9e / mov r0,r5 / bl __MessageID`, later
 * `add r0,r5,#6 / bl __MessageID`, later `add r5,#5 / mov r0,r5`, then
 * `ldr r5,=0x1aa2 / mov r0,r5`, then `add r5,#3 / mov r0,r5`.  0x1a9e+6 and 0x1a9e+5
 * are NOT in the pool -- only the two bases are.  That is the brief's base-and-walking-
 * pointer lever on an integer: ONE `int m`, read as `m`, `m + 6`, then advanced in place
 * with `m += 5`, re-based to 0x1aa2, advanced again with `m += 3`.  Writing the five
 * values as five literals would pool five words; the walk pools two.  Note the ROM
 * keeps BOTH forms in one variable -- `m + 6` leaves m alone, `m += 5` does not -- so
 * the distinction between a read-with-offset and an advance is SOURCE INFORMATION, not
 * a compiler choice.
 *
 * ===== THE BYTE-FLAG PAIR, AND THE OR/AND ASYMMETRY HOLDING AGAIN =====
 *
 * Four sites on `__MapActor_GetActor(0x14) + 0x5a`, two clearing bit 0 and two setting
 * it, and the ROM holds BOTH the mask and the bit in call-saved registers across the
 * calls between them (r5 = 0xfe, r6 = 1).  So they are two source locals, not literals:
 *     mask = 0xfe;  *b = mask & *b;    ... later ...  *b = mask & *b;
 *     bit  = 1;     *b = bit  | *b;    ... later ...  *b = bit  | *b;
 * The second use of each consumes the register (`and r5,r3 / strb r5` and
 * `orr r6,r3 / strb r6`), which is the accumulator shape the same-overlay precedent
 * src/overlays/rom_7a4370/ovl_30_c_c_c_a_c_b.c recorded for `|` -- and here it is
 * needed for `&` TOO, because the operand is a VARIABLE rather than a literal.  That
 * precedent's asymmetry (OR needs it, AND does not) is about LITERAL masks only; with a
 * named mask both operators want the mask on the left.
 *
 * ===== STILL OPEN =====
 *
 * Our register use is r5=110 r6=63 r7=24 r8=32 r9=6 sl=14 fp=8 against the reference's
 * r5=41 r6=10 r7=8 r8=5 -- so even with count exact we hold far more live quantities
 * than the ROM, and the 291 hunks are overwhelmingly register-name substitutions.  The
 * reference's r5 is FOUR disjoint ranges (gScript_936__0200bdc4, the 0xfe mask, the
 * message base, gScript_936__0200be00, and the 0xd0 << 8 facing value), r6 is the
 * sentinel pointer and then the bit, r7 is the read-once global, r8 a zero held across
 * one call pair.  The partition levers were applied where the evidence named them; what
 * is left is the same cse1 commoning documented on OvlFunc_917_2008488, and the twenty
 * flag settings screened there apply unchanged -- do not re-screen them.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __PlayMapMusic(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __StopTask(void (*f)(void));
extern void __LoadFieldActors(unsigned char *p);
extern void __DeleteFieldActor(int slot);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_8019aa0(int a, int b, int c);
extern short __Func_8091e9c(short n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093304(int a);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

extern unsigned char *iwram_3001ebc;
extern unsigned char L4948[] __asm__(".L4948");
extern unsigned char gScript_936__0200bdc4[];
extern unsigned char gScript_936__0200be00[];
extern unsigned char gScript_936__0200bfb0[];
extern unsigned char gScript_936__0200c034[];
extern unsigned char gScript_936__0200c0cc[];
extern unsigned char gScript_936__0200c164[];
extern unsigned char gScript_936__0200c1ac[];
extern void OvlFunc_936_2009e6c(void);
extern void OvlFunc_936_2009ea4(int slot);
extern void OvlFunc_936_2009ed8(void);
extern void OvlFunc_936_2009f14(void);

void OvlFunc_936_2008590(void)
{
    unsigned char *p;
    unsigned char *s;
    unsigned char *b;
    short *w;
    int m;
    int z;
    int mask;
    int bit;
    int hd;
    int hv;
    int z0;
    int z1;
    int z2;

    p = iwram_3001ebc;
    __CutsceneStart();
    if (__GetFlag(0x91 << 4) != 0 && __GetFlag(0x911) == 0) {
        __LoadFieldActors(L4948);
        __MapActor_SetPos(0x14, 0xfc << 16, 0x88 << 17);
        __MapActor_SetPos(0x1b, 0x8e << 17, 0x84 << 17);
        __MapActor_SetPos(0x1c, 0x8e << 17, 0x8c << 17);
        __MapActor_SetPos(0x1d, 0x96 << 17, 0x84 << 17);
        __MapActor_SetPos(0x1e, 0x96 << 17, 0x8c << 17);
        __MapActor_SetPos(0x20, 0x9e << 17, 0x84 << 17);
        __MapActor_SetPos(0x1f, 0x9e << 17, 0x8c << 17);
        __MapActor_SetPos(0x21, 0xa6 << 17, 0x84 << 17);
        __MapActor_SetPos(0x22, 0xa6 << 17, 0x8c << 17);
        __MapActor_SetPos(0x15, 0xb6 << 17, 0x88 << 17);
        __PlaySound(0x11);
        __Func_8093304(0x14);
        __Func_8019aa0(0x1a91, 1, 0);
        __PlaySound(9);
        __CutsceneWait(0xa);
        __Func_80925cc(0, 2);
        if (*(short *)(p + (0xb6 << 1)) == 9) {
            __Func_80933d4(0x26666, 0x4ccc);
            __Func_8092adc(0, 0xe0 << 8, 0x14);
        } else {
            __Func_80933d4(0x13333, 0x2666);
            __Func_8092adc(0, 0, 0x14);
        }
        __MapActor_SetSpeed(0x14, 0x11999, 0x8ccc);
        __MapActor_SetSpeed(0x1b, 0x80 << 9, 0x80 << 8);
        __MapActor_SetSpeed(0x1c, 0x80 << 9, 0x80 << 8);
        __MapActor_SetSpeed(0x1d, 0xe666, 0x7333);
        __MapActor_SetSpeed(0x1e, 0xe666, 0x7333);
        __MapActor_SetSpeed(0x20, 0xcccc, 0x6666);
        __MapActor_SetSpeed(0x1f, 0xcccc, 0x6666);
        __MapActor_SetSpeed(0x21, 0xb333, 0x5999);
        __MapActor_SetSpeed(0x22, 0xb333, 0x5999);
        __MapActor_SetSpeed(0x15, 0x9999, 0x4ccc);
        s = gScript_936__0200bdc4;
        __MapActor_SetBehavior(0x14, s);
        __MapActor_SetBehavior(0x1b, s);
        __MapActor_SetBehavior(0x1c, s);
        __MapActor_SetBehavior(0x1d, s);
        __MapActor_SetBehavior(0x1e, s);
        __MapActor_SetBehavior(0x20, s);
        __MapActor_SetBehavior(0x1f, s);
        __MapActor_SetBehavior(0x21, s);
        __MapActor_SetBehavior(0x22, s);
        w = (short *)(__MapActor_GetActor(0x15) + 0x64);
        z0 = 0;
        *w = z0;
        __MapActor_SetBehavior(0x15, s);
        __Func_80933f8(0xba << 16, -1, 0x88 << 17, 1);
        __MapActor_WaitScript(0x14);
        __Func_8092adc(0x14, 0, 0);
        while (*w == 0)
            __WaitFrames(1);
        __CutsceneWait(0x28);
        __Func_80925cc(0x1b, 2);
        __Func_8092adc(0x1b, 0xa0 << 7, 0x14);
        __MessageID(0x1a92);
        __Func_8093040(0x1b, 0, 0xa);
        __Func_80925cc(0x1c, 2);
        __Func_8092adc(0x1c, 0xb0 << 8, 0xa);
        __MapActor_SetAnim(0x1c, 3);
        __Func_8093040(0x1c, 0, 0xa);
        __MapActor_Surprise(0x20, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_8093040(0x20, 0, 0xa);
        __MapActor_Emote(0x1f, 0x80 << 1, 0x28);
        __Func_8092adc(0x1f, 0xb0 << 8, 0xa);
        __Func_8093040(0x1f, 0, 0xa);
        __Func_8092adc(0x1f, 0x80 << 8, 0xa);
        __Func_80925cc(0x1f, 2);
        __MapActor_SetAnim(0x1f, 4);
        __Func_8093040(0x1f, 0, 0xa);
        __Func_8092adc(0x1f, 0xb0 << 8, 0);
        __Func_8092adc(0x20, 0xa0 << 7, 0x14);
        __MapActor_SetAnim(0x1f, 3);
        __MapActor_DoAnim(0x20, 3);
        __Func_80925cc(0x14, 2);
        __MapActor_Surprise(0x14, 0x81 << 1);
        __CutsceneWait(0x28);
        __Func_8093040(0x14, 0, 0xa);
        __Func_80925cc(0x14, 2);
        b = __MapActor_GetActor(0x14) + 0x5a;
        mask = 0xfe;
        *b = mask & *b;
        z = 0;
        __Func_80921c4(0x14, 0xac, 0x84 << 1);
        __CutsceneWait(1);
        b = __MapActor_GetActor(0x14) + 0x5a;
        bit = 1;
        *b = bit | *b;
        __Func_8092adc(0x1b, 0x80 << 8, 0);
        __Func_8092adc(0x1c, 0x80 << 8, 0);
        __Func_8092adc(0x20, 0x80 << 8, 0);
        __Func_8092adc(0x1f, 0x80 << 8, 0x14);
        __MapActor_DoAnim(0x14, 3);
        __CutsceneWait(0x14);
        b = __MapActor_GetActor(0x14) + 0x5a;
        *b = mask & *b;
        __Func_80921c4(0x14, 0xac, 0x88 << 1);
        __CutsceneWait(1);
        b = __MapActor_GetActor(0x14) + 0x5a;
        *b = bit | *b;
        __Func_80921c4(0x14, 0xb4, 0x88 << 1);
        __Func_8092adc(0x14, 0, 0);
        __Func_8093040(0x14, 0, 0xa);
        __MapActor_Emote(0x22, 0x105, 0);
        __Func_80925cc(0x22, 1);
        __MapActor_DoAnim(0x22, 3);
        __Func_8093040(0x22, 0, 0xa);
        __Func_80925cc(0x21, 1);
        __Func_8093040(0x21, 0, 0xa);
        __MapActor_SetAnim(0x21, 4);
        __Func_8093040(0x21, 0, 0xa);
        __Func_80925cc(0x15, 2);
        __MapActor_Emote(0x15, 0x81 << 1, 0);
        __Func_8093040(0x15, 0, 0xa);
        __MapActor_Jump(0x14, 2, 0x14);
        __MapActor_Jump(0x14, 4, 0x28);
        __Func_80925cc(0x14, 2);
        __Func_8093040(0x14, 0, 0xa);
        __MapActor_SetSpeed(0x15, 0x19999, 0xcccc);
        __Func_80921c4(0x15, 0x109, 0x8d << 1);
        __Func_80921c4(0x15, 0xfb, 0x8e << 1);
        __Func_80921c4(0x15, 0xf6, 0x94 << 1);
        __Func_8092adc(0x15, 0xc0 << 8, 0);
        OvlFunc_936_2009e6c();
        __CutsceneWait(0x28);
        __MapActor_SetSpeed(0x15, 0x19999, 0xcccc);
        __Func_80921c4(0x15, 0xe4, 0x94 << 1);
        __Func_8092adc(0x15, 0xc0 << 8, 0x28);
        __Func_80921c4(0x15, 0xd4, 0x94 << 1);
        __Func_8092adc(0x15, 0xc0 << 8, 0x28);
        __Func_80921c4(0x15, 0xc0, 0x94 << 1);
        __Func_8092adc(0x15, 0xc0 << 8, 0x28);
        __Func_809259c(0x15, 2);
        __MapActor_Emote(0x15, 0x80 << 1, 0x3c);
        __Func_8092adc(0x14, 0xc0 << 6, 0);
        __Func_80921c4(0x15, 0xb8, 0x8f << 1);
        __Func_8092adc(0x15, 0xb0 << 8, 0xa);
        __MapActor_Surprise(0x15, 0x81 << 1);
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0x15, 4);
        __MapActor_Emote(0x14, 0x101, 0x28);
        __MapActor_DoAnim(0x14, 3);
        __Func_80925cc(0x14, 2);
        __Func_8092adc(0x14, 0, 0);
        __Func_8092adc(0x15, 0, 0x3c);
        __Func_8092adc(0x14, 0xc0 << 6, 0);
        __Func_8092adc(0x15, 0xb0 << 8, 0xa);
        __MapActor_DoAnim(0x14, 3);
        __MapActor_DoAnim(0x15, 3);
        __Func_8092adc(0x15, 0, 0);
        __MapActor_SetSpeed(0x14, 0x19999, 0xcccc);
        __MapActor_RunScript(0x14, gScript_936__0200bfb0);
        __Func_80921c4(0x14, 0xe4, 0x94 << 1);
        __Func_8092adc(0x14, 0xc0 << 8, 0x28);
        __Func_80921c4(0x14, 0xd4, 0x94 << 1);
        __Func_8092adc(0x14, 0xc0 << 8, 0x28);
        __Func_80921c4(0x14, 0xc0, 0x94 << 1);
        __Func_8092adc(0x14, 0xc0 << 8, 0x28);
        __Func_8092adc(0x14, 0xb0 << 8, 0);
        __Func_8092adc(0x15, 0xc0 << 6, 0xa);
        __MapActor_Surprise(0x14, 0x81 << 1);
        __CutsceneWait(0x3c);
        __MapActor_DoAnim(0x14, 4);
        m = 0x1a9e;
        __MessageID(m);
        __Func_8093040(0x14, 0, 0x28);
        OvlFunc_936_2009ed8();
        __Func_80921c4(0x14, 0xb2, 0x88 << 1);
        __Func_8092adc(0x14, 0, 0);
        __CutsceneWait(0xf0);
        __MapActor_SetIdle(0x1b);
        __WaitFrames(1);
        __Func_8092adc(0x1b, 0x80 << 8, 0xa);
        __MapActor_Emote(0x1b, 0x101, 0x3c);
        __Func_8093040(0x1b, 0, 0xa);
        OvlFunc_936_2009ea4(0x1b);
        __CutsceneWait(0x50);
        __MapActor_SetIdle(0x1c);
        __WaitFrames(1);
        __Func_8092adc(0x1c, 0xd0 << 8, 0x14);
        __Func_80925cc(0x1c, 2);
        __Func_8093040(0x1c, 0, 0xa);
        OvlFunc_936_2009ea4(0x1c);
        __CutsceneWait(0xa0);
        __MapActor_SetIdle(0x20);
        __WaitFrames(1);
        __Func_8092adc(0x20, 0xa0 << 7, 0xa);
        __MapActor_Emote(0x20, 0x101, 0x3c);
        __Func_8093040(0x20, 0, 0xa);
        OvlFunc_936_2009ea4(0x20);
        __CutsceneWait(0x50);
        __MapActor_SetIdle(0x1e);
        __WaitFrames(1);
        __Func_8092adc(0x1e, 0xb0 << 8, 0xa);
        __Func_80925cc(0x1e, 1);
        __MessageID(m + 6);
        __Func_8093040(0x1e, 0, 0xa);
        __StopTask(OvlFunc_936_2009f14);
        __MapActor_SetIdle(0x14);
        __MapActor_SetIdle(0x15);
        __WaitFrames(1);
        *(short *)(__MapActor_GetActor(0x14) + 0x64) = z;
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = z;
        __MapActor_SetSpeed(0x14, 0xcccc, 0x6666);
        __MapActor_SetSpeed(0x15, 0xcccc, 0x6666);
        __MapActor_SetBehavior(0x14, gScript_936__0200c034);
        __MapActor_SetBehavior(0x15, gScript_936__0200c0cc);
        __MapActor_SetIdle(0x1d);
        __WaitFrames(1);
        __Func_8092adc(0x1d, 0xa0 << 7, 0xa);
        m += 5;
        __Func_80925cc(0x1d, 2);
        __MessageID(m);
        __Func_8093040(0x1d, 0, 0x14);
        OvlFunc_936_2009ea4(0x1d);
        OvlFunc_936_2009ea4(0x1e);
        do {
            __WaitFrames(1);
        } while (*(short *)(__MapActor_GetActor(0x14) + 0x64) == 0
              || *(short *)(__MapActor_GetActor(0x15) + 0x64) != 1);
        __MapActor_SetBehavior(0x14, gScript_936__0200c164);
        __MapActor_SetBehavior(0x15, gScript_936__0200c1ac);
        __MapActor_SetIdle(0x1f);
        __WaitFrames(1);
        __Func_8092adc(0x1f, 0xa0 << 7, 0xa);
        __Func_80925cc(0x1f, 1);
        __MapActor_DoAnim(0x1f, 4);
        m = 0x1aa2;
        __MessageID(m);
        __Func_8093040(0x1f, 0, 0xa);
        OvlFunc_936_2009ea4(0x1f);
        __MapActor_SetIdle(0x22);
        __MapActor_SetIdle(0x21);
        __WaitFrames(1);
        __MapActor_Emote(0x22, 0x105, 0x28);
        __MapActor_Emote(0x21, 0x105, 0x3c);
        __Func_8092adc(0x22, 0xb0 << 8, 0xa);
        __Func_8092adc(0x21, 0xa0 << 7, 0xa);
        m += 3;
        __MapActor_DoAnim(0x22, 4);
        __MessageID(m);
        __Func_8093040(0x22, 0, 0xa);
        __Func_80925cc(0x21, 1);
        __MapActor_SetAnim(0x21, 4);
        __Func_8093040(0x21, 0, 0xa);
        __MapActor_Emote(0x22, 0x81 << 1, 0x3c);
        __MapActor_Emote(0x14, 0x103, 0);
        __Func_80925cc(0x14, 2);
        __MessageID(0x1ab2);
        __Func_8093040(0x14, 0, 0xa);
        __MapActor_SetIdle(0x1b);
        __MapActor_SetIdle(0x1c);
        __MapActor_SetIdle(0x1d);
        __MapActor_SetIdle(0x1e);
        __MapActor_SetIdle(0x20);
        __MapActor_SetIdle(0x1f);
        __MapActor_SetIdle(0x21);
        __MapActor_SetIdle(0x22);
        __MapActor_SetIdle(0x14);
        __MapActor_SetIdle(0x15);
        __WaitFrames(1);
        __MapActor_Jump(0x1b, 2, 0);
        __MapActor_Jump(0x1c, 2, 0);
        __MapActor_Jump(0x1d, 2, 0);
        __MapActor_Jump(0x1e, 2, 0);
        __MapActor_Jump(0x20, 2, 0);
        __MapActor_Jump(0x1f, 2, 0);
        __MapActor_Jump(0x21, 2, 0);
        __MapActor_Jump(0x22, 2, 0);
        __MapActor_Jump(0x15, 2, 0x28);
        __Func_8092adc(0x1b, 0x80 << 8, 0);
        __Func_8092adc(0x1c, 0x80 << 8, 0);
        __Func_8092adc(0x1d, 0x80 << 8, 0);
        __Func_8092adc(0x1e, 0x80 << 8, 0);
        __Func_8092adc(0x20, 0x80 << 8, 0);
        __Func_8092adc(0x1f, 0x80 << 8, 0);
        __Func_8092adc(0x21, 0x80 << 8, 0);
        __Func_8092adc(0x22, 0x80 << 8, 0x28);
        __MapActor_Jump(0x15, 4, 0x28);
        __Func_8093040(0x15, 0, 0xa);
        __Func_80925cc(0x14, 1);
        __Func_8093040(0x14, 0, 0xa);
        __MapActor_DoAnim(0x15, 3);
        __Func_8093040(0x15, 0, 0xa);
        __MapActor_DoAnim(0x14, 3);
        __Func_8093040(0x14, 0, 0xa);
        __MapActor_Emote(0x1b, 0x81 << 1, 0x28);
        __Func_809259c(0x1b, 1);
        __Func_8093040(0x1b, 0, 0xa);
        __MapActor_Emote(0x1c, 0x81 << 1, 0x28);
        __Func_8093040(0x1c, 0, 0xa);
        __MapActor_DoAnim(0x15, 4);
        __CutsceneWait(0x28);
        __MapActor_DoAnim(0x15, 3);
        __Func_8093040(0x15, 0, 0x14);
        __MapActor_DoAnim(0x14, 3);
        __Func_8093040(0x14, 0, 0xa);
        __Func_8092adc(0x1b, 0xa0 << 7, 0);
        __Func_8092adc(0x1c, 0xb0 << 8, 4);
        __Func_8092adc(0x1d, 0xa0 << 7, 0);
        __Func_8092adc(0x1e, 0xb0 << 8, 4);
        __Func_8092adc(0x20, 0xa0 << 7, 0);
        __Func_8092adc(0x1f, 0xb0 << 8, 4);
        __Func_8092adc(0x21, 0xa0 << 7, 0);
        __Func_8092adc(0x22, 0xb0 << 8, 4);
        __MapActor_SetAnim(0x1b, 3);
        __MapActor_DoAnim(0x1c, 3);
        __MapActor_SetAnim(0x1d, 3);
        __MapActor_DoAnim(0x1e, 3);
        __MapActor_SetAnim(0x20, 3);
        __MapActor_DoAnim(0x1f, 3);
        __MapActor_SetAnim(0x21, 3);
        __MapActor_DoAnim(0x22, 3);
        __MapActor_Jump(0x14, 2, 0x28);
        __Func_8093040(0x14, 0, 0xa);
        __Func_8092adc(0x1b, 0x80 << 8, 0);
        __Func_8092adc(0x1c, 0x80 << 8, 4);
        __Func_8092adc(0x1d, 0x80 << 8, 0);
        __Func_8092adc(0x1e, 0x80 << 8, 4);
        __Func_8092adc(0x20, 0x80 << 8, 0);
        __Func_8092adc(0x1f, 0x80 << 8, 4);
        __Func_8092adc(0x21, 0x80 << 8, 0);
        __Func_8092adc(0x22, 0x80 << 8, 4);
        __MapActor_SetSpeed(0x14, 0x11999, 0x8ccc);
        __MapActor_SetSpeed(0x1b, 0x10ccc, 0x8666);
        __MapActor_SetSpeed(0x1c, 0x10ccc, 0x8666);
        __MapActor_SetSpeed(0x1d, 0x80 << 9, 0x80 << 8);
        __MapActor_SetSpeed(0x1e, 0x80 << 9, 0x80 << 8);
        __MapActor_SetSpeed(0x20, 0xf333, 0x7999);
        __MapActor_SetSpeed(0x1f, 0xf333, 0x7999);
        __MapActor_SetSpeed(0x21, 0xe666, 0x7333);
        __MapActor_SetSpeed(0x22, 0xe666, 0x7333);
        __MapActor_SetSpeed(0x15, 0xd999, 0x6ccc);
        __Func_8092b08(0x1b, 1);
        __Func_8092b08(0x1c, 1);
        __Func_8092b08(0x1d, 1);
        __Func_8092b08(0x1e, 1);
        __Func_8092b08(0x20, 1);
        __Func_8092b08(0x1f, 1);
        __Func_8092b08(0x21, 1);
        __Func_8092b08(0x22, 1);
        __Func_8092b08(0x14, 1);
        __Func_8092b08(0x15, 1);
        __MapActor_SetIdle(0x1b);
        __MapActor_SetIdle(0x1c);
        __MapActor_SetIdle(0x1d);
        __MapActor_SetIdle(0x1e);
        __MapActor_SetIdle(0x20);
        __MapActor_SetIdle(0x1f);
        __MapActor_SetIdle(0x21);
        __MapActor_SetIdle(0x22);
        __MapActor_SetIdle(0x14);
        __MapActor_SetIdle(0x15);
        __WaitFrames(1);
        s = gScript_936__0200be00;
        __MapActor_SetBehavior(0x14, s);
        __MapActor_SetBehavior(0x1b, s);
        __MapActor_SetBehavior(0x1c, s);
        __MapActor_SetBehavior(0x1d, s);
        __MapActor_SetBehavior(0x1e, s);
        __MapActor_SetBehavior(0x20, s);
        __MapActor_SetBehavior(0x1f, s);
        __MapActor_SetBehavior(0x21, s);
        __MapActor_SetBehavior(0x22, s);
        *(short *)(__MapActor_GetActor(0x15) + 0x64) = 0;
        __MapActor_SetBehavior(0x15, s);
        do {
            __WaitFrames(1);
        } while (*(short *)(__MapActor_GetActor(0x15) + 0x64) != 1);
        __CutsceneWait(0x50);
        __MapActor_SetPos(0xe, 0xaa << 17, 0x89 << 17);
        __WaitFrames(1);
        __MapActor_SetSpeed(0xe, 0x80 << 9, 0x80 << 8);
        __Func_80921c4(0xe, 0xe0, 0x89 << 1);
        __Func_8092adc(0xe, 0, 0x28);
        __Func_8092adc(0xe, 0x80 << 8, 0x28);
        __Func_8092adc(0xe, 0xc0 << 8, 0x28);
        __Func_8092adc(0xe, 0xa0 << 7, 0x28);
        __MapActor_Emote(0xe, 0x101, 0x3c);
        __Func_8093040(0xe, 0, 0xa);
        __Func_8092adc(0xe, 0, 0x28);
        __Func_8092adc(0xe, 0xc0 << 8, 0x28);
        __Func_8092adc(0xe, 0x80 << 8, 0x28);
        __MapActor_Surprise(0xe, 0x81 << 1);
        __MapActor_Jump(0xe, 4, 0x28);
        __Func_8093040(0xe, 0, 0x14);
        __Func_80925cc(0xe, 2);
        __Func_8093040(0xe, 0, 0xa);
        __MapActor_Jump(0xe, 4, 0x28);
        __MapActor_SetSpeed(0xe, 0x13333, 0x9999);
        *(short *)(__MapActor_GetActor(0xe) + 0x64) = 0;
        __MapActor_SetBehavior(0xe, gScript_936__0200be00);
        do {
            __WaitFrames(1);
        } while (*(short *)(__MapActor_GetActor(0xe) + 0x64) != 1);
        __MapActor_SetPos(0xe, 0x1670000, 0x9d << 17);
        hd = 0xd0 << 8;
        *(short *)(__MapActor_GetActor(0xe) + 6) = hd;
        __MapActor_SetPos(0x14, 0x1c70000, 0xd9 << 17);
        *(short *)(__MapActor_GetActor(0x14) + 6) = hd;
        __MapActor_SetPos(0x15, 0xe8 << 17, 0xd0 << 17);
        hv = 0xa0 << 7;
        *(short *)(__MapActor_GetActor(0x15) + 6) = hv;
        __DeleteFieldActor(0x1b);
        __DeleteFieldActor(0x1c);
        __DeleteFieldActor(0x1d);
        __DeleteFieldActor(0x1e);
        __DeleteFieldActor(0x1f);
        __DeleteFieldActor(0x20);
        __DeleteFieldActor(0x21);
        __DeleteFieldActor(0x22);
        __PlaySound(0x11);
        if (*(short *)(p + (0xb6 << 1)) == 9) {
            __Func_80921c4(0, 0xe0, 0xe5 << 1);
            hv = 0xc0 << 8;
            *(short *)(__MapActor_GetActor(0) + 6) = hv;
        } else {
            __Func_80921c4(0, 0x28, 0xf8);
            hv = 0x80 << 7;
            *(short *)(__MapActor_GetActor(0) + 6) = hv;
        }
        __PlayMapMusic();
        __SetFlag(0x911);
    } else {
        __PlaySound(0x7b);
        __Func_8091e9c(*(short *)(p + (0xb6 << 1)));
        __MapTransitionOut();
        __WaitMapTransition();
    }
    __CutsceneEnd();
}
