/* OvlFunc_965_2009238  --  0x02009238
 *   [asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a.s, 1st of 2 -- SPLIT NEEDED]
 *
 * 853 instructions of straight-line cutscene script: 251 calls, three
 * if/else diamonds on __Func_8091c7c and three null-guarded
 * __MapActor_GetActor blocks.  Same shape and the same levers as
 * OvlFunc_962_2008240 (src/overlays/rom_7ec19c/ovl_30_c_c_a.c), which was the
 * template.  VERDICT:
 *
 *   OK OvlFunc_965_2009238 -- 2264 bytes, 866 encodings and 254 relocations identical
 *
 * THE FLAGS ARE THE FIRST THING TO GET RIGHT, AND THE MAKEFILE SAYS THE WRONG
 * ONE.  `asm/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c%.o` applies O1_CFLAGS and
 * its stem captures this TU, so both tryc and objcmp screen it at -O1 when the
 * reference is given by its real asm/ path.  Measured: 286 differing at -O1
 * against BYTE-IDENTICAL at -O2, on exactly the same source.  This is the
 * THIRD file in this one directory the wildcard is wrong for -- the Makefile
 * already carries explicit -O2 overrides for ovl_30_a_c_c_c_c_c_c_a_b and
 * ovl_30_a_c_c_c_c_c_c_c_c_c_b for the same reason.  LANDING THIS FUNCTION
 * REQUIRES A FOURTH SUCH OVERRIDE for whatever the split names the .c piece;
 * without it the screen is green and the build is red.  All measurements below
 * were taken through a scratch copy of the single function (scratch paths match
 * no Makefile rule, so they get the tree default -O2).
 *
 * A PIN FUNCTION, AND THE PROLOGUE SAYS SO.  `push {lr}` alone: nothing is kept
 * across a call anywhere, so named locals cannot be what the source had and
 * only pins work.  Plain C is 2268 bytes against 2264 and 726 of 866 differ,
 * opening at `push {r5,r6,r7,lr}` (b5e0) against the ROM's `push {lr}` (b500)
 * -- cse_main commons the repeated multi-instruction constants into pseudos
 * that straddle `bl` and gcc spills them into r5-r7.  Here the recorded
 * "LONGER proves constant-CSE" length tell DOES fire (+4 bytes), unlike the
 * template where the widened prologue exactly paid for the removed
 * rematerialisations.
 *
 * THE THIRTEEN REPEATED EXPENSIVE CONSTANTS, by use count:
 * 0x80 << 8 (14 -- 6 as __MapActor_SetSpeed's r2 and 8 as __Func_8092adc's r1),
 * 0xc0 << 8 (9), 0x80 << 7 (7), 0x80 << 9 (6), 0x81 << 1 (4), 0x101 (3, pool),
 * 0x13333 (3), 0x9999 (3), 0xa4 << 1 (3), 0x107 (2), 0x14ccc (2), 0xa666 (2),
 * 0xac << 1 (2), 0x9c << 1 (2), 0x8c << 17 (2).  Every 8-bit `mov` constant is
 * rematerialised for free and needs nothing.
 *
 * FIFTY-SIX PIN CANDIDATES, FORTY-FIVE REQUIRED.  The candidate set is the 50
 * sites carrying one of those values, plus six ORDERING-ONLY sites found in the
 * first residue (see below).  Every one of the 56 was then stripped
 * individually under objcmp, greedily, re-testing after each drop and running
 * to a fixpoint -- and the sweep was run from BOTH ends of the list, which
 * agreed on the same surviving 45.  The eleven that fall are sites 27, 62, 74,
 * 77, 104, 110, 122, 173, 190, 220 and 230 (numbering is the ordinal of the
 * `bl` in the reference; see scratch_elev/b231/f2009238/calls.txt).  Ten of the
 * eleven are a LATER use of a value whose earlier use is pinned, which is the
 * recorded "one pin at the first use covers the later ones".  The eleventh is
 * not, and it is the interesting one.
 *
 * 0x107 REVERSES THE POLARITY OF THE FIRST-USE RULE -- a SECOND instance of the
 * case recorded under "WHAT A PIN IS FOR, NOT WHERE THE BLOCKS ARE"
 * (OvlFunc_952_2008674).  Its two uses are __MapActor_Emote sites 62 and 166,
 * and they straddle two whole if/else diamonds, so gcc never commons them at
 * all.  Neither pin has any CSE work to do; the pin at 166 is buying pure
 * ARGUMENT ORDERING, because the ROM emits that site as
 * `mov r2,#0x32 / mov r0,#0xc / ldr r1,=0x107` and gcc's own order is
 * `mov r2 / ldr r1 / mov r0`.  So:
 *
 *   pin 166 only  -- exact                 (this file)
 *   pin 62 only   -- 2 encodings, an adjacent transposition at index 538
 *   pin neither   -- the SAME 2 encodings, at the same index
 *   pin both      -- exact, and 62 is therefore inert scaffolding: dropped
 *
 * The residue's shape is the diagnostic that separates the two jobs: the pool
 * load is PRESENT in both streams and merely transposed, so this is ordering,
 * not a missing rematerialisation.  "Pin the first use" is a rule about the CSE
 * job; it says nothing about the ordering job, and a minimiser that assumes it
 * would have kept the inert pin and dropped the required one.
 *
 * THE SIX ORDERING-ONLY PINS.  With all 50 constant sites pinned the residue was
 * 13 encodings at six sites, none of them a CSE victim: site 7
 * (__Func_80921c4, r0 wanted before the two `lsl`s), site 88
 * (__MapActor_Emote(0xe, 0x105, 0x3c), r0 before the pool load), site 141
 * (__MapActor_Emote(0xe, 0x80 << 1, 0x28), r0 between the `mov` and the `lsl`)
 * and the three __Func_8092c40 sites.  All four constants there are
 * SINGLE-USE, so no pin of theirs can be about CSE.  Adding them uniform
 * ascending (descending at 8092c40) took 13 to 0.
 *
 * __Func_8092c40 WANTS THE DESCENDING FILL -- the seventh function, and the
 * first with MORE THAN ONE such site: all three of sites 101, 142 and 179 want
 * `q1 = 0; q0 = N;`.  Writing them ascending costs exactly 6 encodings, two per
 * site, and the recorded "binary, not graded" tell holds: ascending here is
 * byte-identical to leaving all three unpinned.
 *
 * THE SHIFTED-BYTE SPELLING IS INERT ON THIS FUNCTION.  Every `mov #n / lsl #k`
 * build is written the ROM's way (`0x80 << 9`, `0xc0 << 8`, `0x8c << 17`, ...)
 * for documentary value, but respelling all 44 of them as whole values
 * (`0x10000`, `0xc000`, `0x1180000`) is BYTE-IDENTICAL.  That is consistent
 * with the recorded scope of the exception -- the spelling matters when the ROM
 * puts the `lsl` BETWEEN two movs and the source statement has to sit there
 * too.  Every fill here has its shift last, so sched2 lands it unaided, and it
 * reproduces all four of the ROM's transposed emitted orders from the one
 * uniform ascending spelling.
 *
 * ALL EIGHT POOLED VALUES ARE BARE LITERALS.  0x988, 0x98a, 0x2702, 0x101,
 * 0x105, 0x107, 0x13333, 0x9999, 0x14ccc and 0xa666 need no symbol: none is a
 * shifted byte gcc could build with mov+lsl, so each pools unaided.  objcmp
 * reports 254 relocations identical; the only non-`bl` relocations are the
 * three R_ARM_ABS32 iwram_3001ebc.  Nothing belongs in const.sym or message.sym
 * for this function.  Note 0x988 and 0x98a are two DISTINCT values, so the
 * "two symbols of equal value" caveat does not arise.
 *
 * THE THREE IF/ELSE DIAMONDS are the template's shape verbatim: the
 * `iwram_3001ebc` counter bump appears in BOTH arms, in the ROM's two different
 * positions relative to __ActorMessage, and written that way plain C emits the
 * pointer-then-0xec<<1 build with no help.  `bne` past the block means the
 * fallthrough is the `if` body, so the guard is `== 0`.  The three
 * __MapActor_GetActor blocks are likewise the template's
 * `p = __MapActor_GetActor(0); if (p != 0) __MapActor_TravelTo(...)`.
 *
 * LANDING.  The .s holds TWO functions -- this one and OvlFunc_965_2009b10, a
 * 914-instruction cutscene of the same family that is NOT solved here -- so a
 * whole-file .c replacement is not available and the file must be split.  The
 * single linker-script line naming the .o is
 * overlays/rom_7ef4f4/overlay.ld:48.  After the split BOTH pieces still match
 * the O1 wildcard, so the .c piece needs the explicit -O2 rule described above.
 *
 * Reproduce: scratch_elev/b231/f2009238/gen.py emits this file from a pin set
 * given as reference call-site numbers; minimise.py runs the greedy sweep and
 * variants.py the measured-worse table, each inside ONE container invocation.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int slot, int x, int y);
extern void __Func_80921c4(int slot, int x, int y);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_965_2009238(void)
{
    unsigned char *p;

    __SetFlag(0x988);
    __SetFlag(0x98a);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x2702);
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x94 << 1; q2 = 0xb0 << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN4; q0 = 0xa; q1 = 0x10; q2 = 0; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 1; q1 = -8; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 2; q1 = 8; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0x18; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(3);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    { PIN4; q0 = 0x8c << 17; q1 = -1; q2 = 0x90 << 17; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0xb, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    __Func_809259c(0xd, 2);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 0xc; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xd, 0, 0);
    __CutsceneWait(0x19);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xd, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xb, 0);
    __CutsceneWait(0x14);
    __MapActor_Emote(0xd, 0x107, 0x28);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x101; q2 = 0x4b;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xb; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __Func_80933f8(0x8c << 17, -1, 0xa0 << 17, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0xa, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xa, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x105; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 8, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xa, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __Func_8092adc(0xa, 0x80 << 8, 0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xa, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xe, 0, 0x10);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0xc0 << 8, 0);
    __CutsceneWait(0x23);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xe; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0xe;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xe, 4);
        __CutsceneWait(0x14);
        __ActorMessage(0xe, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xe, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xe, 0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xd, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xd; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xd, 0, 0x10);
    __CutsceneWait(0x14);
    __ActorMessage(0xd, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xc; q1 = 0x14ccc; q2 = 0xa666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0xc, 0, 0x10);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0xc; q1 = 0x107; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0xc, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xa; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8092adc(0xa, 0x80 << 8, 0);
    __CutsceneWait(0x19);
    __ActorMessage(0xa, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xa, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 0xa;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(0xa, 0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0xa, 4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0xa, 0);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(0xa, 0xc0 << 8, 0);
    __CutsceneWait(0x23);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x1e);
    __Func_8092adc(0xe, 0xb0 << 8, 0);
    __CutsceneWait(0x28);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_8092848(0xc, 0xd, 0x32);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0xc, 3);
    __MapActor_DoAnim(0xd, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xc; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80922c4(0xc, 0x20, 0);
    __Func_8092304(0xd, 0x20, 0);
    __Func_80922c4(0xc, 0, 0x10);
    __Func_8092304(0xd, 0x10, 0);
    { PIN3; q0 = 0xd; q1 = 0xac << 1; q2 = 0x9c << 1;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xac << 1; q2 = 0xa8 << 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0xd, 1);
    { PIN3; q0 = 0xc; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092adc(0xe, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0xe; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xa4 << 1; q2 = 0x9c << 1;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0xb; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xa4 << 1; q2 = 0xa4 << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0xb, 0x80 << 8, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
