/* OvlFunc_966_20087c4  --  0x020087c4
 *   [asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a_c.s, 1st of 2 -- SPLIT REQUIRED]
 *
 * 849 instructions of straight-line cutscene script cut into regions by two
 * if/else pairs on __Func_8091c7c, one guarded tail-call, and three guarded
 * __MapActor_TravelTo blocks.  Built at the tree default -O2: no Makefile
 * pattern rule matches rom_7f148c/ovl_30_c_c_c_a_a_c, so `asm/%.o: src/%.c`
 * applies and a scratch-path screen sees the same flags as the real object
 * (objcmp prints no `(built with: ...)` line from either path).  THE MATCH DOES
 * NOT DEPEND ON A FLAG GROUP.
 *
 * READ THE PROLOGUE BY CONTENT, NOT BY WIDTH.  `push {r5, r6, lr}` with
 * `sub sp, #8` looks like the wide, register-holding shape that the pin class
 * is supposed to exclude.  It is not: r5 and r6 hold nothing but the TWO STACK
 * ARGUMENTS (4 and 0x12) of four six-argument __Func_8010788 calls, and the
 * `sub sp, #8` is their outgoing-argument area.  Nothing is held across the
 * body, so the other 246 call sites are a pure pin problem exactly as if the
 * prologue were `push {lr}`.  Width said "not a pin function"; content said it
 * is one everywhere except at four sites.
 *
 * THE LENGTH TELL FIRES LONG, AND THE DIFF TEXT NAMES THE VALUE.  Plain C is
 * 2260 bytes / 867 encodings against the ROM's 2252 / 863 -- four instructions
 * and eight bytes LONG -- with 750 of 863 differing.  Counting the diff's
 * `mov rN, r8` copies BY DESTINATION gives five, into r1, r2 (twice), r3 and
 * r5, and the extra four instructions are gcc's Thumb-1 spill of that one high
 * pseudo (`mov r5, r8` ahead of a widened `push`, and its restore).  The ROM
 * has ZERO r5-r11 copies.  cse_main had commoned one repeated multi-instruction
 * constant -- 0xc0 << 8, eleven sites -- into a pseudo that straddles `bl`.
 *
 * NOMINATING BY CALL-SITE FAMILY REPLACED THE DIAGNOSTIC LOOP.  The recorded
 * rule (docs/elevation.md, "NOMINATE PIN CANDIDATES BY CALL-SITE FAMILY") is
 * confirmed here at full strength.  The CSE rule -- sites carrying a constant
 * the ROM builds more than once -- nominates 34 sites and leaves 18 differing.
 * Adding "same callee AND same argument shape" nominates 10 more (4, 6, 13, 25,
 * 36, 52, 132, 135, 136, 181) and leaves FOUR, from one grep and no residue
 * read.  The 10 extras are the singleton members of uniform families:
 * __Func_809233c(slot, -0x20, 0, 0) alone in its shape, __MapActor_SetPos's two
 * shifted-pair sites, __MapActor_Emote(0x2, 0x84 << 1, 0x28) whose shifted byte
 * is used once.
 *
 * NEW, AND IT IS A LIMIT ON THAT RULE: A FAMILY WHOSE ARGUMENTS ARE ALL CHEAP
 * CAN STILL NEED AN ORDERING PIN, AND NEITHER RULE NOMINATES IT.  Site 117,
 * __Func_8092c40(1, 0), has two 8-bit `mov` arguments, so it carries no
 * expensive constant and its family is filtered out.  It is the whole remaining
 * residue after the family set.  Widening the nomination to EVERY multi-argument
 * call site -- 136 pins, uniform ascending -- leaves exactly the same two
 * encodings and nothing else, which is the strongest available statement that
 * THE UNIFORM ASCENDING FILL IS CORRECT RATHER THAN MERELY CHEAPER: it is
 * simultaneously right at 135 of 136 sites.  So the cheap-argument family costs
 * one residue read and no more; it does not reopen the method.
 *
 * __Func_8092c40 IS A SITE PROPERTY, AND THIS FUNCTION PROVES IT INTERNALLY.
 * The template records `__Func_8092c40` taking the DESCENDING fill.  Both of
 * this function's sites are __Func_8092c40(1, 0), identical arguments:
 *
 *   site 11   unpinned matches; pinned descending is ALSO byte-identical
 *   site 117  unpinned is 2 differing; ascending is 2 differing;
 *             descending is exact
 *
 * Same callee, same arguments, opposite requirements, 106 call sites apart.
 * The rule belongs to the site, not the callee.  Site 11 is left unpinned --
 * inert scaffolding must not ship.
 *
 * THE PIN SET IS 34 AND THREE INDEPENDENT MINIMISATIONS AGREE ON IT.  Greedy
 * drop-and-retest under objcmp, run to a fixpoint forward from the 45-pin
 * family set, backward from the same set, and forward from the 136-pin
 * all-families set, all converge on the identical 34.  Eleven of the family
 * set's 45 are inert (8, 13, 37, 46, 52, 66, 101, 115, 136, 181, 200).
 *
 * THE INERT ONES ARE NEVER THE FIRST USE IN A REGION.  Of the eleven 0xc0 << 8
 * sites, the survivors are 40-43, 98-100 and 131; the drops are 8, 101 and 200.
 * Site 8 is the EARLIEST use in the whole function and it is inert, because it
 * sits in the straight-line region before the first branch, where nothing needs
 * re-establishing; 101 and 200 are the LAST use in their runs.  "Pin the first
 * use in each region, and the earliest use in the function may not be in a
 * region that needs a pin" is exactly what the data says.
 *
 * DROPPING A REQUIRED PIN PARTITIONS CLEANLY BY THE RELOCATION LINE, AND SIZE
 * IS NOT THE DISCRIMINATOR.  Every one of the 34 was stripped individually:
 *
 *   ORDERING loss   2-4 encodings, SIZE and RELOCATIONS both SILENT   (16 pins)
 *   CSE loss        12-792 encodings, RELOCATIONS ALWAYS differ       (18 pins)
 *
 * because a lost rematerialisation moves every following `bl` and so every
 * relocation offset.  SIZE stayed silent on NINE of the 18 genuine CSE losses
 * (34, 35, 40, 85, 99, 100, 131, 138, 158) -- reading size alone would have
 * mis-classified half of them.
 *
 * THE TWO BITFIELD SITES FOUR LINES APART WANT DIFFERENT THINGS, AND THE ORR
 * SITE IS DECIDED BY DECLARATION ORDER.  The AND site takes the plain form and
 * needs nothing:
 *
 *   __MapActor_GetActor(0x16)[0x5a] &= 0xfe;      exact
 *
 * The ORR site four lines later has the ROM tying the destination to the
 * CONSTANT (`ldrb r2, [r0] / movs r3, #1 / orrs r3, r2`), where the plain form
 * gives the loaded value that role.  Five spellings were measured and only one
 * matches -- and the winner differs from a 6-differing loser ONLY in which of
 * its two locals is declared first:
 *
 *   p[0x5a] |= 1;                                         2 differing
 *   { uchar *r = G+0x5a;      uchar b = 1; *r = b | *r; }  6      <- b second
 *   { uchar b = 1; uchar *r = G+0x5a;      *r = b | *r; }  6
 *   { uchar b = 1; uchar *r = G+0x5a; b |= *r; *r = b; }   6      <- b FIRST
 *   { uchar *r = G+0x5a; uchar b = 1; b |= *r; *r = b; }   EXACT
 *   { uchar b = 1; G[0x5a] = b | G[0x5a]; }              238, +12 bytes
 *
 * The recorded reading -- the constant reaches the destination only while it
 * survives as a distinct QImode pseudo -- is right, but it is not enough on its
 * own here: the pointer local must be born BEFORE the byte local.  Declaration
 * order is recorded as "usually inert and occasionally decisive"; this is one of
 * the decisive ones, and it is worth the one screen it costs.
 *
 * THE POINTER-RETURNING CALL MUST NOT BE NAMED -- 636 differing AND EIGHT BYTES.
 * Writing `p = __MapActor_GetActor(0x16); p[0x5a] &= 0xfe;` in place of the
 * folded lvalue at the three in-place sites costs a preserved base per site and
 * takes the whole function apart.  `p` is named only at the three TravelTo
 * sites, where the result genuinely crosses a `cmp` and a branch.
 *
 * THE STACK ARGUMENTS MUST BE NAMED, AND THAT IS THE ONLY THING THAT MATTERS.
 * The ROM builds 4 and 0x12 into two registers and then stores both; literals
 * make gcc walk one register through both stores (12 differing).  Three named
 * spellings are all EXACT and byte-identical to each other: a pair scoped to
 * each of the two regions, one function-level pair reassigned per region, and
 * a fresh pair at each of the four sites.  The recorded warning "do NOT share a
 * pair between sites" is about SHARING INSTEAD OF NAMING; where the ROM itself
 * reuses the held pair at the second site of a region without rebuilding it, as
 * here, sharing and per-site tie.  The scoped pair ships because it is the
 * shortest reading of the ROM: two pseudos per region, rebuilt at the second
 * region because the scopes make them distinct.
 *
 * DO NOT TRANSCRIBE THE ROM'S EMITTED ORDER.  Filling each pinned site in the
 * order its own ROM block emits is 62 differing against 0.  sched2 produces
 * every one of the ROM's transposed orders from the single uniform ascending
 * spelling.
 *
 * AND THE FILL DIRECTION IS A POOL-ORDER LEVER, WHICH THE INSTRUCTION SCREEN
 * CANNOT SEE.  Flipping all 34 pins to descending costs only 4 encodings, but
 * two of them are the five __MapActor_SetSpeed sites emitting their pool words
 * as 0x6666/0xcccc/0x9999/0x13333 -- the ROM's four words, PAIRWISE SWAPPED.
 * tryc normalises every pool load to `=value` and would score that a match.
 * This is a live instance of the case objcmp exists for.
 *
 * `0xc0 << 8` AND `0x18000` ARE INTERCHANGEABLE.  Respelling every shifted byte
 * in the file as its whole value is byte-identical; the shifted form is kept
 * because it is what the ROM's `mov`/`lsl` pair reads as.
 *
 * ALL EIGHT POOLED VALUES ARE BARE LITERALS.  0x9ba, 0x288e, 0xcccc, 0x6666,
 * 0x9bf, 0x28a5, 0x13333 and 0x9999 -- not one is an `imm8 << k` that gcc could
 * have built with mov+lsl, so each pools unaided and the symbol tell does not
 * fire.  Nothing belongs in const.sym or message.sym for this function.  Of the
 * 253 relocations, 250 are the `bl`s and 3 are R_ARM_ABS32 iwram_3001ebc, one
 * per literal-pool dump: the function loads that symbol at five sites and the
 * assembler shares the word within each of the three `.pool_aligned` regions.
 *
 * The `iwram_3001ebc` increment appears in BOTH arms of BOTH if/elses, in the
 * ROM's two different positions relative to __ActorMessage -- after it in the
 * then arm, before it in the else arm -- and once more, unguarded, before
 * __Func_8091a58.  Written that way it needs no help.
 *
 * LANDING.  The `.s` holds TWO functions and this elevation takes the first, so
 * the file must be split before the .c can land.  `asm/overlays/rom_7f148c/
 * ovl_30_c_c_c_a_a_c.o` is 0x9f8 bytes at 0x020087c4: OvlFunc_966_20087c4 runs
 * 0x8cc (2252, the size objcmp matches) and OvlFunc_966_2009090 the remaining
 * 0x12c.  The second is CALLED by the first, so the .c keeps its `extern` and
 * the linker resolves it inside the overlay.  No suffix of the stem is taken in
 * either asm/ or src/, so the pieces are `_a` (this .c) and `_b` (the residual
 * .s).  ONE linker line names the .o by full path:
 *
 *   overlays/rom_7f148c/overlay.ld:28
 *     asm/overlays/rom_7f148c/ovl_30_c_c_c_a_a_c.o(.text)
 *
 * and it becomes two, in address order, `..._a.o(.text)` then `..._b.o(.text)`.
 * That is the ONLY edit: the overlay's `.data` output section names exactly one
 * object, `asm/overlays/rom_7f148c/ovl_30_c_c_c_c.o`, a DIFFERENT file, and this .o is
 * named in no other section.  The six further hits in
 * overlays/rom_7f148c/overlay.map -- the `.data`/`.bss`/`.ARM.attributes`
 * triplet at lines 52-55, the `.text 0x020087c4 0x9f8` placement at 132-133,
 * and `LOAD` at 223 -- are NOT edits: overlay.map is a build output
 * (`Makefile:48`, `-Map $(<:.ld=.map)`) and is not tracked by git.  It will
 * regenerate with two of each triplet, including the zero-size `.data` line for
 * a section neither piece has.  The basename `ovl_30_c_c_c_a_a_c` also exists
 * under overlays/rom_7b4558/ with several `_a_a_*` children; every match above
 * was taken on FULL PATH for that reason.
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
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_966_2009090(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_966_20087c4(void)
{
    unsigned char *p;

    __SetFlag(0x9ba);
    __CutsceneStart();
    __MessageID(0x288e);
    { PIN3; q0 = 0x0; q1 = 0x68; q2 = 0xbc << 1;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(0x0, 0x0, 0x0);
    { PIN4; q0 = 0x1; q1 = -0x20; q2 = 0x0; q3 = 0x0;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 0x3; q1 = -0x10; q2 = 0x10; q3 = 0xe0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __Func_809233c(0x2, 0x0, 0x10, 0xc0 << 8);
    __MapActor_WaitMovement(0x1);
    __CutsceneWait(0x1e);
    __Func_8092c40(0x1, 0x0);
    __CutsceneWait(0xa);
    __Func_8092adc(0x0, 0x80 << 8, 0x0);
    __CutsceneWait(0xa);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x3, 0x4);
        __CutsceneWait(0x14);
        __ActorMessage(0x3, 0x0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x3, 0x4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0x3, 0x0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x2; q1 = 0x84 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x2, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x0; q1 = -0x10; q2 = 0x0;
      __Func_8092304(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x0; q2 = -0x10;
      __Func_80922c4(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x0; q2 = -0x8;
      __Func_80922c4(q0, q1, q2); }
    __Func_8092304(0x0, 0x0, -0x10);
    __MapActor_SetAnim(0x3, 0x1);
    __MapActor_SetAnim(0x2, 0x1);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x50);
    __MapActor_SetSpeed(0x16, 0xcccc, 0x6666);
    __MapActor_GetActor(0x16)[0x55] = 2;
    __Func_8092b08(0x16, 0x2);
    { int s1 = 4, s2 = 0x12;
      __Func_8010788(0x22, 0x0, 0x1, 0x2, s1, s2);
      __PlaySound(0x9e);
      __CutsceneWait(0x14);
      __MapActor_SetPos(0x16, 0x90 << 15, 0x9c << 17);
      __CutsceneWait(0x14);
      __Func_8092304(0x16, 0x0, 0x10);
      __Func_8010788(0x20, 0x0, 0x1, 0x2, s1, s2);
    }
    __PlaySound(0x9f);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x28);
    __Func_8092304(0x16, 0x10, 0x0);
    __Func_8092adc(0x16, 0x80 << 7, 0x0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x3, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x1, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x2; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x2, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __Func_8092848(0x1, 0x0, 0x0);
    __Func_8092848(0x3, 0x2, 0x0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x3, 0xc0 << 8, 0x0);
    __CutsceneWait(0x28);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x3; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x3, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __Func_8092adc(0x1, 0xe0 << 8, 0x0);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0x0; q0 = 0x1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x16, 0x4);
        __CutsceneWait(0x14);
        __ActorMessage(0x16, 0x0);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x16, 0x4);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0x16, 0x0);
    }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x2; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x2, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x83 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x19; q1 = 0xb0 << 15; q2 = 0xa6 << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __ActorMessage(-0x1, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x3; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0x3, 0x4, 0xd);
    __MapActor_Jump(0x3, 0x4, 0x1e);
    __ActorMessage(0x3, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    if (__GetFlag(0x9bf) != 0)
        OvlFunc_966_2009090();
    __MessageID(0x28a5);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x1, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x16, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x2, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x19; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809228c(0x19, 0x0, 0x10);
    __Func_8092304(0x16, 0x0, 0x10);
    __CutsceneWait(0x1e);
    __MapActor_SetPos(0x19, 0x0, 0x0);
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __Func_8091a58(0xf2, 0x0);
    __CutsceneWait(0xa);
    __MapActor_GetActor(0x16)[0x5a] &= 0xfe;
    { PIN3; q0 = 0x16; q1 = 0x0; q2 = -0x10;
      __Func_8092304(q0, q1, q2); }
    { unsigned char *r = __MapActor_GetActor(0x16) + 0x5a;
      unsigned char b = 1;
      b |= *r; *r = b; }
    __Func_8092adc(0x16, 0x82 << 7, 0x0);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x16; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x16, 0x0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x32);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x16; q1 = -0x10; q2 = 0x0;
      __Func_8092304(q0, q1, q2); }
    __Func_8092adc(0x16, 0xc0 << 8, 0x0);
    __CutsceneWait(0x14);
    { int s1 = 4, s2 = 0x12;
      __Func_8010788(0x22, 0x0, 0x1, 0x2, s1, s2);
      __PlaySound(0x9e);
      __CutsceneWait(0xa);
      { PIN3; q0 = 0x16; q1 = 0x0; q2 = -0x10;
        __Func_8092304(q0, q1, q2); }
      __MapActor_SetPos(0x16, 0x0, 0x0);
      __CutsceneWait(0xa);
      __Func_8010788(0x20, 0x0, 0x1, 0x2, s1, s2);
    }
    __PlaySound(0x9f);
    __CutsceneWait(0x32);
    { PIN3; q0 = 0x0; q1 = 0x80 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_809280c(0x1, 0x0, 0x0);
    __Func_809280c(0x2, 0x0, 0x0);
    __CutsceneWait(0x14);
    __ActorMessage(0x1, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x3, 0x2);
    __CutsceneWait(0x14);
    __ActorMessage(0x3, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x2, 0x3);
    __CutsceneWait(0x1e);
    __ActorMessage(0x2, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x1, 0x3);
    __MapActor_SetAnim(0x2, 0x3);
    __MapActor_DoAnim(0x3, 0x3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0x1, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x1);
    __MapActor_SetPos(0x1, 0x0, 0x0);
    __MapActor_SetAnim(0x3, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x3);
    __MapActor_SetPos(0x3, 0x0, 0x0);
    __MapActor_SetAnim(0x2, 0x2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x2);
    __MapActor_SetPos(0x2, 0x0, 0x0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
