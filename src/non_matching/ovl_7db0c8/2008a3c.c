/* ===== BATCH 341, BRIEF C: 14 -> 11 ENCODINGS OF 367.  READ THIS FIRST. =====
 *
 * OvlFunc_954_2008a3c -- NON-MATCHING, **11 ENCODINGS OF 367 DIFFER**.
 * Instruction counts AGREE, 286 each (the old header's "348 instructions" is
 * wrong -- objcmp excludes pool words and pads, and that line predates the
 * metric).  Size exact.  tools/shimcount.py prints NO ROWS: zero pins.
 * RELOCATIONS DIFFER BY EXACTLY ONE ROW and it is the known `_FILE_e4` one --
 * ours carries `00000374 R_ARM_ABS32 _FILE_e4` where the reference has the
 * literal.  See the `_FILE_e4` block below; this is NOT a new defect.
 *
 * Verify with (the INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7db0c8/2008a3c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a_a.s --func OvlFunc_954_2008a3c
 *   -> 11 of 367, instructions 286/286, first at index 61
 * ONE function, tools/datacheck.py prints nothing -- CONVERTS WHOLE when it lands.
 *
 * WHAT MOVED IT: ONE DEVICE-FREE ANCHOR.  `do { } while (0);` immediately
 * after `a[0x23] = two;`.  Found by tools/autosweep.py's anchor axis, which
 * this park had never been run through.  It removes three of the thirteen
 * encodings at [68..71] outright and leaves the window a clean permutation.
 *
 * THE RESIDUE IS 10 INSTRUCTIONS PLUS THE ONE POOL WORD, AND THE TEN ARE ONE
 * THING: THREE RELOAD SCRATCH REGISTERS, PICKED ONE SLOT ROUND THE CURSOR.
 *
 *       ref                      ours
 * [61]  movs r1,#0               str  r3,[r5,#8]
 * [62]  str  r3,[r5,#8]          movs r3,#0
 * [63]  mov  r8,r1               mov  r8,r3
 * [66]  mov  r2,r8               mov  r1,r8
 * [67]  strb r2,[r3,#0]          strb r1,[r3,#0]
 * [71]  movs r3,#12              movs r2,#12
 * [72]  str  r3,[sp,#4]          str  r2,[sp,#4]
 * [73]  movs r2,#1               movs r3,#1
 * [76]  mov  sl,r3               mov  sl,r2
 * [77]  movs r3,#1               movs r2,#1
 *
 * All three are LO scratch registers a Thumb HI-register access needs
 * (a const cannot be moved into a HI reg, and a HI reg cannot be the source of
 * a `strb`).  THE ROM TAKES r1, r2, r3 IN THAT ORDER; WE TAKE r3, r1, r2.
 * That is the SAME SEQUENCE ONE POSITION FURTHER ROUND
 * `allocate_reload_reg`'s cursor (reload1.c:4962, `i = last_spill_reg` at
 * :5003, advanced only on success in `set_reload_reg` at :4937, reset to -1
 * once per function at :821).  Derivation, checkable: with r0 unavailable at
 * the second site, a cursor sitting on r2's slot yields r3 then r1 then r2
 * (ours) and one sitting on r0's yields r1 then r2 then r3 (the ROM).
 *
 * THE DUMP EVIDENCE, and it is the decisive part -- `-da`, `.19.flow2` and
 * `.18.greg` of this body:
 *
 *   .19.flow2  (insn 845 ... (set (reg:SI 3 r3) (const_int 0)))      <- ours
 *              (insn 129 ... (set (reg/v:SI 8 r8) (reg:SI 3 r3)))
 *              (insn 851 ... (set (reg:QI 1 r1) (reg:QI 8 r8)))      <- ours
 *              (insn 137 ... (set (mem:QI (reg:SI 3 r3) 0) (reg:QI 1 r1)))
 *   .18.greg   Spilling for insn 129. / Using reg 3 for reload 0
 *              Spilling for insn 137. / Using reg 2 for reload 2
 *
 * > **FOR INSN 137 `find_reg` PRINTED r2 -- WHICH IS THE ROM'S REGISTER -- AND
 * > THE EMITTED REGISTER IS r1.**  So at that site layer 1 already agrees with
 * > the ROM and the round-robin overrides it.  This is docs/elevation.md's
 * > brief-E observable reproduced on a second function
 * > (`find_reg` reserves at reload1.c:1588, prints at :1664, tie-breaks on
 * > `inv_reg_alloc_order` at :1645-1662; the cursor then picks inside the set
 * > `finish_spills` grew at :3609-3627).  **Never read the register off
 * > `.18.greg`.**
 *
 * SO THE PARK'S CLASSIFICATION ("a reload-stage register choice, which the
 * brief's alias-set dependent-count lever provably cannot reach") IS RIGHT,
 * and it is now pinned to one quantity: the cursor's position on entry to
 * insn 129.  The next door is the SAME DOOR as
 * src/non_matching/ovl_7aa430/2009a3c.c's: make an earlier reload that
 * currently emits nothing into an ALLOCATED one, so the cursor advances once
 * more before insn 129.  `find_dummy_reload` (reload.c:1404, reached only when
 * `in != 0 && out != 0`) and its `REG_DEAD` requirement are the handle that
 * park names.  TWO PARKS, ONE MECHANISM -- work them together.
 *
 * MEASURED THIS BATCH, DO NOT RE-RUN:
 *   ANCHOR AXIS, ALL 94 BOUNDARIES (not the 40 autosweep's default reaches):
 *     best 11 (after `a[0x23] = two;`), second 13 (after `a[0x55] = z;`),
 *     everything else 14 or worse.
 *   ANCHOR AXIS CROSSED WITH ITSELF: all 95 second-anchor placements on top of
 *     the 11 base -- NONE better than 11.  Closest 12.
 *   FLAG AXIS: every per-file group inert at 14 except -O1 (91),
 *     -fno-schedule-insns2 (91) and -ffixed-r7 (355).
 */

/* OvlFunc_954_2008a3c -- NON-MATCHING, **14 ENCODINGS OF 367**, DOWN FROM 17.
 * SIZE AND RELOCATION COUNT EXACT, 348 instructions, instruction count exact.
 * Production flags, no per-file Makefile adjustment (checked).
 * tools/shimcount.py: ZERO register pins.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7db0c8/2008a3c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a_a.s
 * ONE function, no data section (tools/datacheck.py prints nothing) --
 * CONVERTS WHOLE when it lands.
 *
 * ========== BATCH 316: 17 -> 14, AND THE 17th WAS NEVER AN INSTRUCTION ==========
 *
 * 1. THE PARK'S 17 WAS 16 INSTRUCTIONS PLUS ONE POOL WORD.  Encoding index
 *    [365] is `.word 0x000000e4` in the reference against `.word 0x00000000`
 *    in ours: the undefined `_FILE_e4`.  It is a POOL WORD, not an insn.
 *
 * 2. **A NON-VOID RETURN TYPE ON OvlFunc_common1_1ecc IS WORTH 3.**
 *    `extern void OvlFunc_common1_1ecc(...)` -> `extern int ...` takes the
 *    figure from 17 to 14 and removes the whole cluster at [257..259]:
 *        ref   movs r1,#8 / movs r2,#4 / movs r0,#0
 *        ours  movs r0,#0 / movs r1,#8 / movs r2,#4
 *    the three constant arguments of
 *    `OvlFunc_common1_1ecc(0, 8, 4, 0xa3 << 19, 0xc0 << 16, 0x18, 0x19)`.
 *    MECHANISM, and it is the brief's lever 2 exactly: a `void` call only
 *    CLOBBERS r0, and a CLOBBER never becomes a last setter, so `movs r0,#0`
 *    has no output dependence on the call and sched2 falls through the whole
 *    ladder to INSN_LUID, which is the argument emission order r0,r1,r2.
 *    Declaring a return value makes the call SET r0, the output dependence
 *    appears, and r0's load sinks to last -- the ROM's order.
 *    IT IS THE NON-VOIDNESS, NOT THE TYPE: int, short, char, long,
 *    unsigned int and `unsigned char *` all give 14 (measured).  This is a
 *    DECLARATION-ONLY change; the call's value is unused, so the C is
 *    unchanged in meaning and nothing is fabricated.
 *
 * 3. THE SWEEP THAT FOUND IT, and what it rules out.  All 26 return-type
 *    changes available in this file were measured; only 1ecc moves anything.
 *    Re-swept AGAINST THE NEW 14 BASELINE (31 variants): 17 EXACTLY INERT,
 *    none better.  Worse: __Func_8010704 27, __MapActor_SetExtra 18,
 *    __Actor_SetAnim 16, __Actor_SetSpriteFlags 16, __StartTask -> void 16.
 *
 * ================== THE REMAINING 13 ARE ONE CLUSTER ==================
 *
 * [61..77], around
 *     a = __MapActor_GetActor(0xa);  hi = 0x80 << 12;
 *     *(int *)(a + 8) = (n << 20) + hi;
 *     z = 0; a[0x55] = z; two = 2; a[0x23] = two; twelve = 0xc;
 *     __Func_8010704(0xe, 0xd, 1, 1, n, twelve);
 * Two reload copies out of HI registers pick different LO staging registers:
 * `z` (which lives in r8) is staged in r1 by the ROM and r3 by us, and
 * `twelve` (which lives in sl) in r3 by the ROM and r2 by us; the one-slot
 * schedule shifts at [68..74] follow from that.  SAME CLASS AS
 * src/non_matching/ovl_787e04/2008578.c and ovl_7ac2d8/200cfcc.c: a
 * reload-stage register choice, which the brief's alias-set dependent-count
 * lever provably cannot reach.
 *
 * MEASURED INERT AT 14 (16 variants): moving `twelve = 0xc` before `two = 2`;
 * `two` first; two-step computed forms for `twelve` (0xc0>>4), `z` (2>>2) and
 * `two` (8>>2); `twelve = two * 6`; a named temp for the (n<<20)+hi value;
 * `register int twelve __asm__("r10")` (inert -- twelve is ALREADY in sl);
 * `register int z __asm__("r8")` (inert -- z is already in r8); both local
 * declaration orders; `hi = 0x80; hi <<= 12;`; `a[0x55]` through a named
 * pointer.  WORSE: `z = 0` hoisted above the (a+8) store 18 (first moves to
 * 57); `twelve` first 183; `z` last 183.
 * FLAG SWEEP, 29 flags: EVERY ONE inert at the park figure except
 * -fno-schedule-insns2 (91) and -fno-omit-frame-pointer (336).
 *
 * ============== `_FILE_e4` IS NOW DECISION-CRITICAL, NOT COSMETIC ==============
 *
 * The park reported this symbol and withheld it because it "does not complete
 * the function".  That calculus has changed: the function is now 13
 * instructions plus this one pool word.  `_FILE_e4` is NOT in file_table.sym
 * (it is only mentioned in a comment there, line 159); `_FILE_e6`, `_FILE_e7`
 * and `_FILE_e8` are.  With the symbol the figure is 13 and the relocation set
 * is exact; without it, 14 plus a relocation mismatch.  If the [61..77] cluster
 * falls, THIS WORD IS THE LAST THING BETWEEN THIS FUNCTION AND ZERO, so the
 * owner's decision on it now gates a landing rather than one cosmetic encoding.
 * The evidence is unchanged and is stronger than _MSG_d27's, which went in:
 * this function builds TWENTY-THREE distinct values under 0x100 with
 * `mov r0, #N` and pools EXACTLY ONE, `ldr r0, =0xe4`, and that value is the
 * argument to OvlFunc_common1_1fb4, whose landed C
 * (src/overlays/common/common1_c_a_c_c_b.c) passes it straight to
 * __GetFile(arg)
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int _FILE_e4;

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetExtra(int slot, int v);
extern int __Func_8011f54(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __DeleteFieldActor(int slot);
extern void __PlaySound(int id);
extern int __StartTask(void (*fn)(void), int n);
extern short __Func_8091e9c(short n);
extern void OvlFunc_954_200804c(void);
extern void OvlFunc_954_2008a10(int a);
extern void OvlFunc_954_2008974(int a);
extern void OvlFunc_954_2008db8(int a);
extern void OvlFunc_common1_0(void);
extern void OvlFunc_common1_78(int a);
extern void OvlFunc_common1_148(void);
extern void OvlFunc_common1_488(void);
extern void OvlFunc_common1_ea0(int a);
extern void OvlFunc_common1_1608(int a, int b);
extern int OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g);
extern void OvlFunc_common1_1fb4(int file);

int OvlFunc_954_2008a3c(void)
{
    unsigned char *a;
    unsigned char *s;
    int t;
    int n;
    int hi;
    int z;
    int two;
    int twelve;
    int v;
    int e22;
    int e7;
    int v1;
    int v2;
    int iv2;
    int sa;

    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0;
    __SetFlag(0xa2 << 1);
    a = __MapActor_GetActor(9);
    t = __Func_8011f54(0, *(int *)(a + 8), *(int *)(a + 0x10));
    if (*(int *)(a + 0xc) == 0 && t == 0) {
        a[0x23] = 2;
        a[0x55] = t;
        v1 = *(int *)(a + 8) >> 20;
        v2 = *(int *)(a + 0x10) >> 20;
        __Func_8010704(0xe, 0xd, 1, 1, v1, v2);
    }
    n = __GetFlagByte(0xc4 << 2);
    if (n == 0)
        n = 0x19;
    a = __MapActor_GetActor(0xa);
    hi = 0x80 << 12;
    *(int *)(a + 8) = (n << 20) + hi;
    z = 0;
    a[0x55] = z;
    two = 2;
    a[0x23] = two;
    do { } while (0);
    twelve = 0xc;
    __Func_8010704(0xe, 0xd, 1, 1, n, twelve);
    __StartTask(OvlFunc_954_200804c, 0xc8 << 4);
    a = __MapActor_GetActor(0xf);
    a[0x22] = 1;
    if (__GetFlag(0x303) != 0) {
        __Actor_SetAnim(a, 4);
        __Actor_SetSpriteFlags(a, 0);
        a[0x59] = z;
        a[0x23] = 3;
        __Func_8010704(0x2f, 0x18, 1, 1, 0x2f, twelve);
    }
    a = __MapActor_GetActor(0x11);
    v = *(int *)(a + 0x10) >> 20;
    a[0x55] = z;
    a[0x23] = two;
    __Func_8010704(0x40, 0x18, 3, 1, 0x40, v);
    a = __MapActor_GetActor(0x12);
    v = *(int *)(a + 8) >> 20;
    a[0x55] = z;
    a[0x23] = two;
    __Func_8010704(0x3f, 0x19, 1, 3, v, 9);
    if (__GetFlag(0x302) != 0) {
        e22 = 0x22;
        e7 = 7;
        __Func_8010704(0x25, 7, 1, 4, e22, e7);
        __Func_8010704(0x24, 7, 1, 4, 0x25, e7);
        __Func_80105d4(0x64, 0x1d, 1, 3, e22, 0x26);
    }
    a = __MapActor_GetActor(0xd);
    if (__GetFlag(0x301) != 0) {
        __Func_8010704(0x2b, 0xc, 1, 1, 0x29, twelve);
        a[0x55] = z;
        *(int *)(a + 0x34) = 0x6666;
        *(int *)(a + 0x30) = 0xcccc;
        *(int *)(a + 0xc) = hi;
        __Actor_SetAnim(a, 3);
    } else {
        __Actor_SetAnim(a, 2);
    }
    __MapActor_GetActor(0xe)[0x23] = 2;
    OvlFunc_common1_1608(0x18, 0x78);
    OvlFunc_common1_1608(0x19, 0x7f);
    s = gState;
    switch (*(short *)(s + (0xe1 << 1))) {
    case 1:
        OvlFunc_common1_1ecc(0, 8, 4, 0xa3 << 19, 0xc0 << 16, 0x18, 0x19);
        sa = 0x13;
        iv2 = 2;
        __Func_8010788(0x7f, 0, 1, 2, sa, iv2);
        __DeleteFieldActor(0x13);
        __DeleteFieldActor(0x14);
        __DeleteFieldActor(0x15);
        __DeleteFieldActor(0x16);
        __DeleteFieldActor(0x17);
        if (__GetFlag(0x109) == 0) {
            __PlaySound(0x11);
            OvlFunc_common1_78(0);
            OvlFunc_common1_0();
            OvlFunc_954_2008a10(1);
            OvlFunc_954_2008a10(2);
            OvlFunc_954_2008a10(3);
            OvlFunc_common1_ea0(1);
        }
        __MapActor_SetExtra(1, 0);
        __MapActor_SetExtra(2, 0);
        __MapActor_SetExtra(3, 0);
        OvlFunc_common1_1fb4((int)(&_FILE_e4));
        break;
    case 2:
        __StartTask(OvlFunc_common1_148, 0xc8 << 4);
        __DeleteFieldActor(0x18);
        __DeleteFieldActor(0x19);
        if (__GetFlag(0x109) == 0) {
            OvlFunc_common1_0();
            OvlFunc_common1_78(1);
            OvlFunc_common1_ea0(0);
        }
        break;
    case 3:
        if (__GetFlag(0x109) == 0) {
            OvlFunc_954_2008db8(0x13);
            OvlFunc_common1_488();
        }
        break;
    case 4:
        OvlFunc_954_2008974(1);
        __Func_8091e9c(4);
        break;
    case 5:
        OvlFunc_954_2008974(-1);
        __Func_8091e9c(5);
        break;
    }
    return 0;
}
