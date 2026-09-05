/* OvlFunc_891_2009c14, _2009d14, _2009e10, _2009f0c, _2009ff4
 *   [asm/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.s -- ALL FIVE, the whole file]
 *
 * LANDING IS A WHOLE-FILE REPLACEMENT, NO SPLIT AND NO LINKER EDIT.
 * overlays/rom_78c76c/overlay.ld:51 is the ONLY line in any .ld that names this
 * object, and it names `.text` alone; the .s carries no `.section`, `.data`,
 * `.lcomm` or `.incbin`.  Dropping this .c at src/overlays/rom_78c76c/ under the
 * same basename lets the default `asm/%.o: src/%.c` rule (Makefile:146) put the
 * object exactly where the .ld already expects it.  No Makefile rule has a
 * prefix that captures this path, so the tree default -O2 applies and a scratch
 * screen sees the production flags (`tryc.makefile_flags` returns the empty set
 * for it -- the neighbouring _a_b.c DOES carry -fno-rerun-cse-after-loop, so the
 * check was worth making).
 *
 * WHOLE-FILE VERDICT: 1584 bytes, 729 encodings and 51 relocations identical.
 * Per function, each screened from a single-function extract with
 * tools/objcmp.py --func against the ORIGINAL asm/ path:
 *
 *   OK OvlFunc_891_2009c14 -- 256 bytes, 125 encodings and 2 relocations identical
 *   OK OvlFunc_891_2009d14 -- 252 bytes, 122 encodings and 2 relocations identical
 *   OK OvlFunc_891_2009e10 -- 252 bytes, 123 encodings and 2 relocations identical
 *   OK OvlFunc_891_2009f0c -- 232 bytes, 112 encodings and 2 relocations identical
 *   OK OvlFunc_891_2009ff4 -- 592 bytes, 247 encodings and 43 relocations identical
 *
 * ------------------------------------------------------------------------
 * THE SHAPE.  `.L2980` is a 0xc4-byte `.lcomm` declared in
 * asm/overlays/rom_78c76c/ovl_30_c_c_c_c_c_c.s and reached here through the
 * asm-label extension (a high number, so the `.LN`-capture hazard does not
 * apply -- and src/overlays/rom_787e04/ovl_30_c_a_a_c_c_a_b.c already spells it
 * this way, so the name must not be renamed).  Its layout is read off the
 * CONSUMER, _2009ff4: seven six-word __CopyMapTiles argument records with four
 * loose words wedged in at 0x60.  The first four functions are pure fills that
 * write all 49 words and tail-call the runner.
 *
 * ------------------------------------------------------------------------
 * THE FOUR FILLS: EXACT ON THE FIRST SCREEN, NO PINS, NO FLAGS, NO BARRIERS.
 *
 * All four carry the "zero interleaved into a shifted build" signature with
 * straight-line sites (`mov r3,#0 / str` between `mov r3,#0x80 / lsl r3,#7`),
 * and all four fell to the recorded UNIFORM WHOLE-VALUE ASCENDING FILL: one
 * statement per word, ascending by offset, every shifted constant written WHOLE
 * (`0x80 << 7`, `0xfa << 1`, `0xa6 << 2`) and never as a mov+lsl pair.
 *
 * NEW, and worth adding to that entry: the lever is not about ARGUMENT LISTS.
 * Here it governs a run of 49 STORES to a global, with no call in sight until
 * the tail, and the same rule closes them.  Everything the ROM appears to do by
 * hand is gcc's own work off that plain source:
 *
 *   - six values are commoned into low registers (r0-r2, r4, r5) and six more
 *     into r8-r12/r14 -- that is ordinary constant CSE over a fill whose values
 *     repeat, NOT a source that named twelve locals.  Naming any of them would
 *     have destroyed the carry (see "NAMING A VALUE gcc ALREADY CARRIES").
 *   - the run past the 0x7c immediate limit splits into a register-offset store
 *     (`mov r3,#0x80 / str r6,[r3,r7]`) and then a walking `add r3,#4` pointer,
 *     and WHERE it splits differs per function (0x88 in _2009c14, 0x84 in
 *     _2009e10, 0x80 in _2009f0c).  That is allocation, not spelling: one
 *     uniform source produces all three.  This is the same trap as
 *     "DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER", one level up.
 *
 * ------------------------------------------------------------------------
 * THE RUNNER, _2009ff4: FIVE LEVERS, 207 differing -> 0.
 *
 * The prologue is `push {r5,r6,r7,lr}` + `mov r7,r8 / push {r7}`, and it is a
 * PIN function by content only at ONE site: r6 carries the 0xfe mask across
 * five calls, r7 carries the `.L2980` base.  Everything else the wide push
 * holds is gcc's own.
 *
 * 1. TWO STACK ARGUMENTS, ONE PAIR OF LOCALS PER SITE.  __CopyMapTiles takes
 *    six arguments, so e/f spill to [sp].  Bare, gcc reuses ONE register:
 *    load e, store, load f, store.  The ROM materialises both and only then
 *    stores both.  Seven call sites, seven pairs.  Dropping any one pair costs
 *    7-8 (see the table).
 *
 * 2. *** NEW: NAMING ONLY THE STACK PAIR MERGES THE ADDRESS CHAINS. ***
 *    Grepped for first as "address chain", "two pointers", "stack argument" and
 *    "pointer birth order"; the closest entries are "Each stack-argument SITE
 *    needs its own pair of locals" and "One integer-local per address chain",
 *    and neither covers this.
 *
 *    At the u[1] and u[2] sites BOTH groups live past the 0x7c immediate limit,
 *    so the four register arguments need an address chain of their own AND the
 *    stack pair needs one.  The ROM births two pointers back to back:
 *
 *        mov r3, r7 / add r3, #0x88      <- a,b,c,d walk 0x88..0x94
 *        mov r4, r7 / add r4, #0x98      <- e,f     walk 0x98..0x9c
 *
 *    With only e/f named, gcc emits ONE pointer that walks to the pair first
 *    (source order: the locals are initialised before the call) and then
 *    `sub r3, #0x14` back for the register arguments -- five wrong instructions
 *    per site.  NAMING ALL SIX VALUES gives two chains and the ROM's shape.
 *    Measured on the same base: 27 -> 9 with u[1]/u[2] fully named, 9 -> 4 with
 *    u[0] added.  A `TileCopy *` record pointer (39) and an `int *` pointer per
 *    chain (57) are both much worse -- the cure is naming the VALUES, not
 *    naming the pointers.
 *
 *    Note the boundary: the four t[] sites and u[0] reach a,b,c,d through plain
 *    `[r7,#imm]` and want the PAIR ONLY at t[]; u[0] still wants all six (the
 *    pair-only spelling is 5 differing there).  So the rule is per site and it
 *    is read off whether the register arguments need a chain at all.
 *
 * 3. *** CORRECTION: TWO IDENTICAL CONSTANTS IN ONE ARGUMENT LIST ARE
 *    REACHABLE. ***  `__MapActor_SetSpeed(0, 0x80 << 10, 0x80 << 10)` is the
 *    exact shape "The argument-list case of constant CSE" lists as a blocker
 *    and "Constant CSE inside ONE basic block: closed, with a number" declares
 *    unreachable -- "there is no spelling.  Stop sweeping and park it."  Both
 *    are too strong.  Bare, gcc builds 0x20000 once and copies
 *    (`mov r2,#0x80 / lsl r2,#0xa / mov r1,r2`), 152 differing.  THREE
 *    spellings measured BYTE-IDENTICAL:
 *
 *      (a) `register int q1 __asm__("r1"); register int q2 __asm__("r2");`
 *          assigned in the SAME basic block.  cprop will not substitute into a
 *          hard register, so both are built.  This is already implied by "THE
 *          FIRST-USE PIN RULE HAS A BOUNDARY: ADJACENT SITES", where two
 *          back-to-back `0xc8 << 4` sites in one block both need a pin -- the
 *          "closed" entry does not cite it, and the two entries contradict.
 *      (b) two plain `int` locals ASSIGNED ABOVE A DOMINATING GUARD (`if
 *          (__GetFlag(0x818) != 0) goto done;`) and passed by name.  NO PINS AT
 *          ALL.  This is the dominating-block/interleave lever doing a job it
 *          has not been recorded doing: separating two copies of ONE value
 *          rather than ordering two different ones.
 *      (c) the same with the two assignments split across two different guards.
 *
 *    (b) SHIPS -- it is plain C and carries no scaffolding.  What does NOT work:
 *    `do { } while (0)` between the assignments (5), `__asm__ __volatile__ ("" :
 *    "+r" (s2))` (4), and a `volatile int` source (11).
 *
 * 4. THE `orr` DESTINATION.  Two byte writes to actor+0x5a four calls apart.
 *    `p[0x5a] &= 0xfe` already makes the CONSTANT the `and` destination at both
 *    sites (r6 stays live at the first, dies at the second, which is only
 *    liveness).  The `|= 1` site wants the constant as destination too and
 *    plain `|=` gives the value, so it takes the narrow local:
 *    `unsigned char one = 1; p[0x5a] = one | p[0x5a];`  Exactly the spelling in
 *    src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_a.c.  Dropping it costs 2.
 *
 * 5. THE FLAG PIN, AND WHY THE FLAG GROUP IS THE WRONG CURE.  0x80f is loaded
 *    for __GetFlag and again for __SetFlag with a conditional branch between --
 *    the recorded `CSE_CFLAGS` shape.  But this TU cannot have that flag: with
 *    `-fno-rerun-cse-after-loop` the runner loses `mov r6, r5` before the loop
 *    and reloads the `.L2980` address instead, which is worse, and the four
 *    fills would have to be re-verified as well.  Pinning the FIRST use to r0
 *    kills the commoning at one site and costs nothing anywhere else: r0 is
 *    call-clobbered, so the pinned pseudo cannot span the `bl` the rerun-CSE
 *    pass wanted to hoist it over.  Dropping the pin costs 34.
 *
 * ------------------------------------------------------------------------
 * EVERY PIECE OF SCAFFOLDING IS LOAD-BEARING.  Each was dropped individually
 * from the shipped source and re-measured under objcmp; NOT ONE IS INERT, so
 * there is no joint-removal question to re-verify.
 *
 *   | drop                                      | encodings differing |
 *   |-------------------------------------------|---------------------|
 *   | t[0] stack pair -> bare arguments         |   8                 |
 *   | t[1] stack pair                           |   7                 |
 *   | t[2] stack pair                           |   8                 |
 *   | t[3] stack pair                           |   7                 |
 *   | u[0] a..d locals (keep the pair)          |   5                 |
 *   | u[1] a..d locals (keep the pair)          | 116  (-4 bytes)     |
 *   | u[2] a..d locals (keep the pair)          |  99  (-4 bytes)     |
 *   | the r0 pin on __GetFlag(0x80f)            |  34                 |
 *   | `unsigned char one = 1` at the `|=` site  |   2                 |
 *   | the s1/s2 hoist above the guard           | 152  (-4 bytes)     |
 *
 * Other measured-worse spellings, all on the same base:
 *
 *   | spelling                                          | in disagreeing regions |
 *   |---------------------------------------------------|------------------------|
 *   | nothing named at all (first draft)                | 61   (596 vs 592 bytes)|
 *   | u[1]/u[2] via `int *` per chain                   | 57                     |
 *   | u[1]/u[2] via a `TileCopy *` record pointer       | 39                     |
 *   | u[1]/u[2] stack pair through an `int *`           | 29                     |
 *   | u[0] stack pair through an `int *`                | 11                     |
 *   | u[0] pair declared f-before-e                     | 11                     |
 *   | SetSpeed pair from a `volatile int`               | 11                     |
 *   | SetSpeed pair split by `do { } while (0)`         |  5                     |
 *   | SetSpeed pair hidden by an `asm volatile "+r"`    |  4                     |
 *   | the whole TU with -fno-rerun-cse-after-loop       | worse: loses `mov r6,r5`|
 */
typedef struct { int a, b, c, d, e, f; } TileCopy;

typedef struct {
    TileCopy t[4];      /* 0x00: four __CopyMapTiles argument records   */
    int flag;           /* 0x60: the save bit this script sets/clears   */
    int facing;         /* 0x64: halfword written to actor 0 at +6      */
    int tx, ty;         /* 0x68: __MapActor_TravelTo target             */
    TileCopy u[3];      /* 0x70: three more __CopyMapTiles records      */
    int actor;          /* 0xb8: actor slot for the tail                */
    int px, py;         /* 0xbc: __Func_8092158 arguments               */
} Script;

extern Script L2980 __asm__(".L2980");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern int OvlFunc_891_2008054(void);
extern void OvlFunc_891_2008c8c(void);
extern void OvlFunc_891_2009ff4(void);

void OvlFunc_891_2009c14(void)
{
    L2980.t[0].a = 0;
    L2980.t[0].b = 0x37;
    L2980.t[0].c = 0x20;
    L2980.t[0].d = 0x28;
    L2980.t[0].e = 4;
    L2980.t[0].f = 3;
    L2980.t[1].a = 2;
    L2980.t[1].b = 0x1e;
    L2980.t[1].c = 0x22;
    L2980.t[1].d = 0xa;
    L2980.t[1].e = 2;
    L2980.t[1].f = 1;
    L2980.t[2].a = 2;
    L2980.t[2].b = 0x1c;
    L2980.t[2].c = 0x22;
    L2980.t[2].d = 0xa;
    L2980.t[2].e = 2;
    L2980.t[2].f = 1;
    L2980.t[3].a = 2;
    L2980.t[3].b = 0x1e;
    L2980.t[3].c = 0x10;
    L2980.t[3].d = 0xa;
    L2980.t[3].e = 2;
    L2980.t[3].f = 1;
    L2980.flag = 0x80b;
    L2980.facing = 0x80 << 7;
    L2980.tx = 0xfa << 1;
    L2980.ty = 0x84;
    L2980.u[0].a = 8;
    L2980.u[0].b = 0x37;
    L2980.u[0].c = 0x20;
    L2980.u[0].d = 0x28;
    L2980.u[0].e = 4;
    L2980.u[0].f = 3;
    L2980.u[1].a = 2;
    L2980.u[1].b = 0x1e;
    L2980.u[1].c = 0x22;
    L2980.u[1].d = 0xa;
    L2980.u[1].e = 2;
    L2980.u[1].f = 1;
    L2980.u[2].a = 2;
    L2980.u[2].b = 0x1c;
    L2980.u[2].c = 0x10;
    L2980.u[2].d = 0xa;
    L2980.u[2].e = 2;
    L2980.u[2].f = 1;
    L2980.actor = 9;
    L2980.px = 0xf4 << 1;
    L2980.py = 0x98;
    OvlFunc_891_2009ff4();
}

void OvlFunc_891_2009d14(void)
{
    L2980.t[0].a = 4;
    L2980.t[0].b = 0x37;
    L2980.t[0].c = 0x24;
    L2980.t[0].d = 0x28;
    L2980.t[0].e = 4;
    L2980.t[0].f = 3;
    L2980.t[1].a = 4;
    L2980.t[1].b = 0x1e;
    L2980.t[1].c = 0x24;
    L2980.t[1].d = 0xa;
    L2980.t[1].e = 2;
    L2980.t[1].f = 1;
    L2980.t[2].a = 4;
    L2980.t[2].b = 0x1c;
    L2980.t[2].c = 0x24;
    L2980.t[2].d = 0xa;
    L2980.t[2].e = 2;
    L2980.t[2].f = 1;
    L2980.t[3].a = 4;
    L2980.t[3].b = 0x1e;
    L2980.t[3].c = 0x12;
    L2980.t[3].d = 0xa;
    L2980.t[3].e = 2;
    L2980.t[3].f = 1;
    L2980.flag = 0x80c;
    L2980.facing = 0x80 << 7;
    L2980.tx = 0x28e;
    L2980.ty = 0x84;
    L2980.u[0].a = 0xc;
    L2980.u[0].b = 0x37;
    L2980.u[0].c = 0x24;
    L2980.u[0].d = 0x28;
    L2980.u[0].e = 4;
    L2980.u[0].f = 3;
    L2980.u[1].a = 4;
    L2980.u[1].b = 0x1e;
    L2980.u[1].c = 0x24;
    L2980.u[1].d = 0xa;
    L2980.u[1].e = 2;
    L2980.u[1].f = 1;
    L2980.u[2].a = 4;
    L2980.u[2].b = 0x1c;
    L2980.u[2].c = 0x12;
    L2980.u[2].d = 0xa;
    L2980.u[2].e = 2;
    L2980.u[2].f = 1;
    L2980.actor = 0xb;
    L2980.px = 0xa6 << 2;
    L2980.py = 0x98;
    OvlFunc_891_2009ff4();
}

void OvlFunc_891_2009e10(void)
{
    L2980.t[0].a = 0;
    L2980.t[0].b = 0x3a;
    L2980.t[0].c = 0x20;
    L2980.t[0].d = 0x2b;
    L2980.t[0].e = 4;
    L2980.t[0].f = 1;
    L2980.t[1].a = 2;
    L2980.t[1].b = 0x1f;
    L2980.t[1].c = 0x22;
    L2980.t[1].d = 0xb;
    L2980.t[1].e = 2;
    L2980.t[1].f = 1;
    L2980.t[2].a = 2;
    L2980.t[2].b = 0x1d;
    L2980.t[2].c = 0x22;
    L2980.t[2].d = 0xb;
    L2980.t[2].e = 2;
    L2980.t[2].f = 1;
    L2980.t[3].a = 2;
    L2980.t[3].b = 0x1f;
    L2980.t[3].c = 0x10;
    L2980.t[3].d = 0xb;
    L2980.t[3].e = 2;
    L2980.t[3].f = 1;
    L2980.flag = 0x80d;
    L2980.facing = 0xc0 << 8;
    L2980.tx = 0xfa << 1;
    L2980.ty = 0xd8;
    L2980.u[0].a = 8;
    L2980.u[0].b = 0x3a;
    L2980.u[0].c = 0x20;
    L2980.u[0].d = 0x2b;
    L2980.u[0].e = 4;
    L2980.u[0].f = 1;
    L2980.u[1].a = 2;
    L2980.u[1].b = 0x1f;
    L2980.u[1].c = 0x22;
    L2980.u[1].d = 0xb;
    L2980.u[1].e = 2;
    L2980.u[1].f = 1;
    L2980.u[2].a = 2;
    L2980.u[2].b = 0x1d;
    L2980.u[2].c = 0x10;
    L2980.u[2].d = 0xb;
    L2980.u[2].e = 2;
    L2980.u[2].f = 1;
    L2980.actor = 0xd;
    L2980.px = 0xf4 << 1;
    L2980.py = 0xc8;
    OvlFunc_891_2009ff4();
}

void OvlFunc_891_2009f0c(void)
{
    L2980.t[0].a = 4;
    L2980.t[0].b = 0x3a;
    L2980.t[0].c = 0x24;
    L2980.t[0].d = 0x2b;
    L2980.t[0].e = 4;
    L2980.t[0].f = 1;
    L2980.t[1].a = 4;
    L2980.t[1].b = 0x1f;
    L2980.t[1].c = 0x24;
    L2980.t[1].d = 0xb;
    L2980.t[1].e = 2;
    L2980.t[1].f = 1;
    L2980.t[2].a = 4;
    L2980.t[2].b = 0x1d;
    L2980.t[2].c = 0x24;
    L2980.t[2].d = 0xb;
    L2980.t[2].e = 2;
    L2980.t[2].f = 1;
    L2980.t[3].a = 4;
    L2980.t[3].b = 0x1f;
    L2980.t[3].c = 0x12;
    L2980.t[3].d = 0xb;
    L2980.t[3].e = 2;
    L2980.t[3].f = 1;
    L2980.flag = 0x80e;
    L2980.facing = 0xc0 << 8;
    L2980.tx = 0x28e;
    L2980.ty = 0xd8;
    L2980.u[0].a = 0xc;
    L2980.u[0].b = 0x3a;
    L2980.u[0].c = 0x24;
    L2980.u[0].d = 0x2b;
    L2980.u[0].e = 4;
    L2980.u[0].f = 1;
    L2980.u[1].a = 4;
    L2980.u[1].b = 0x1f;
    L2980.u[1].c = 0x24;
    L2980.u[1].d = 0xb;
    L2980.u[1].e = 2;
    L2980.u[1].f = 1;
    L2980.u[2].a = 4;
    L2980.u[2].b = 0x1d;
    L2980.u[2].c = 0x12;
    L2980.u[2].d = 0xb;
    L2980.u[2].e = 2;
    L2980.u[2].f = 1;
    L2980.actor = 0xf;
    L2980.px = 0xa6 << 2;
    L2980.py = 0xc8;
    OvlFunc_891_2009ff4();
}

void OvlFunc_891_2009ff4(void)
{
    int r;
    int i;
    int s1, s2;

    r = 0;
    __CutsceneStart();
    if (__GetFlag(0x80f) != 0)
        goto done;
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    __Func_80933f8(0x90 << 18, -1, 0xac << 16, 1);
    __Func_8093530();
    __PlaySound(0xba);
    {
        int e0 = L2980.t[0].e, f0 = L2980.t[0].f;
        __CopyMapTiles(L2980.t[0].a, L2980.t[0].b, L2980.t[0].c, L2980.t[0].d, e0, f0);
    }
    i = 0;
    do {
        __PlaySound(0xf6);
        {
            int e1 = L2980.t[1].e, f1 = L2980.t[1].f;
            __CopyMapTiles(L2980.t[1].a, L2980.t[1].b, L2980.t[1].c, L2980.t[1].d, e1, f1);
        }
        __CutsceneWait(4);
        __PlaySound(0xf6);
        {
            int e2 = L2980.t[2].e, f2 = L2980.t[2].f;
            __CopyMapTiles(L2980.t[2].a, L2980.t[2].b, L2980.t[2].c, L2980.t[2].d, e2, f2);
        }
        __CutsceneWait(4);
        i++;
    } while (i != 0x14);
    {
        int e3 = L2980.t[3].e, f3 = L2980.t[3].f;
        __CopyMapTiles(L2980.t[3].a, L2980.t[3].b, L2980.t[3].c, L2980.t[3].d, e3, f3);
    }
    __SetFlag(L2980.flag);
    r = OvlFunc_891_2008054();
    if (r != -1)
        goto other;
    s1 = 0x80 << 10;
    s2 = 0x80 << 10;
    if (__GetFlag(0x818) != 0)
        goto done;
    __SetCameraTarget(0, 1);
    {
        *(short *)(__MapActor_GetActor(0) + 6) = L2980.facing;
        __MapActor_SetSpeed(0, s1, s2);
    }
    __MapActor_GetActor(0)[0x5a] &= 0xfe;
    __MapActor_Jump(0, 4, 0);
    __MapActor_TravelTo(0, L2980.tx, L2980.ty);
    {
        int a4 = L2980.u[0].a, b4 = L2980.u[0].b, c4 = L2980.u[0].c, d4 = L2980.u[0].d;
        int e4 = L2980.u[0].e, f4 = L2980.u[0].f;
        __CopyMapTiles(a4, b4, c4, d4, e4, f4);
    }
    {
        int a5 = L2980.u[1].a, b5 = L2980.u[1].b, c5 = L2980.u[1].c, d5 = L2980.u[1].d;
        int e5 = L2980.u[1].e, f5 = L2980.u[1].f;
        __CopyMapTiles(a5, b5, c5, d5, e5, f5);
    }
    {
        int a6 = L2980.u[2].a, b6 = L2980.u[2].b, c6 = L2980.u[2].c, d6 = L2980.u[2].d;
        int e6 = L2980.u[2].e, f6 = L2980.u[2].f;
        __CopyMapTiles(a6, b6, c6, d6, e6, f6);
    }
    __MapActor_GetActor(L2980.actor)[0x5a] &= 0xfe;
    __Func_8092158(L2980.actor, L2980.px, L2980.py);
    {
        unsigned char *p = __MapActor_GetActor(0);
        unsigned char one = 1;
        p[0x5a] = one | p[0x5a];
    }
    __ClearFlag(L2980.flag);
    goto done;
other:
    if (r != 0)
        goto done;
    if (__GetFlag(0x818) == 0)
        goto done;
    if (__GetFlag(0x80b) == 0)
        goto alt;
    if (__GetFlag(0x80d) == 0)
        goto alt;
    if (__GetFlag(0x80e) == 0)
        goto alt;
    {
        register int q0 __asm__("r0");
        q0 = 0x80f;
        if (__GetFlag(q0) != 0)
            goto done;
    }
    __SetFlag(0x80f);
    OvlFunc_891_2008c8c();
    goto done;
alt:
    if (__GetFlag(0x812) == 0)
        goto done;
    __Func_8091e9c(5);
    r = 1;
done:
    if (r == 1) {
        __MapTransitionOut();
        __WaitMapTransition();
    }
    __CutsceneEnd();
}
