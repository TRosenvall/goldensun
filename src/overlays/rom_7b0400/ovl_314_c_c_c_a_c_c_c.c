// fakematch
/* ovl_314_c_c_c_a_c_c_c -- ALL FIVE functions of the original
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.s, RECOMBINED.
 *
 * OvlFunc_925_200af18, _200b060 were elevated in batch 237 into
 * src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_a.c; _200b1c0, _200b208 and
 * _200b324 are elevated here.  With all five solved the split is no longer
 * needed: this ONE file reproduces the whole original .o.
 *
 * VERDICTS (objcmp, in-container, against the ORIGINAL asm/ path
 *  asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_b.s):
 *   OK OvlFunc_925_200b1c0 --  72 bytes,  35 encodings and  1 relocations identical
 *   OK OvlFunc_925_200b208 -- 284 bytes, 131 encodings and  9 relocations identical
 *   OK OvlFunc_925_200b324 -- 276 bytes, 125 encodings and 10 relocations identical
 * and for the WHOLE recombined TU, against the pre-split five-function .s
 * recovered with `git show f4d733ad^:asm/.../ovl_314_c_c_c_a_c_c_c.s`:
 *   size ref 1312 ours 1312 / OK encodings identical (585) / relocs OK 60 60
 *
 * The two batch-237 functions are UNCHANGED except that `__MapActor_GetActor`
 * now returns `struct Actor *` instead of `int *` (one TU, one return type),
 * so `a[2]/a[3]/a[4]` became `a->f8/a->fc/a->f10`.  Re-screened after the
 * retype: both still exact.
 *
 * ------------------------------------------------------------------
 * NEW: A `char` LVALUE PINS A BYTE STORE *TOO EARLY* -- THE INVERSE FACE OF
 * "Strict aliasing can SINK a store, and a `char *` lvalue pins it"
 * ------------------------------------------------------------------
 * Grepped by concept first -- "alias set", "strict aliasing", "alias set 0",
 * "spill slot", "byte store", "sink a store" -- and the governing sections are
 * "Strict aliasing can SINK a store, and a `char *` lvalue pins it" and
 * "gcse hashes the MEMORY ALIAS SET".  Both run one way: the store moved TOO
 * FAR and a `char` lvalue is the brake.  This is the other face.
 *
 * _200b208 and _200b324 each hide their actors with a byte store inside a loop
 * whose counter is SPILLED across the call.  The ROM issues the spill reload
 * FIRST and the store three slots later:
 *
 *   rom   bl __MapActor_GetActor / ldr r2,[sp] / add r0,#0x55 / mov r1,#1
 *         / add r2,#1 / strb r6,[r0] / add r9,r1
 *   ours  bl __MapActor_GetActor / add r0,#0x55 / strb r6,[r0] / ldr r2,[sp]
 *         / mov r1,#1 / add r2,#1 / add r9,r1
 *
 * Everything is in the same relative order; only the `strb` sits four slots
 * early.  Written `__MapActor_GetActor(arr[i])[0x55] = 0`, the store goes
 * through an `unsigned char *`, which is ALIAS SET 0 and conflicts with every
 * memory reference in the function -- including the loop counter's own spill
 * slot.  sched2 therefore may not hoist `ldr r2,[sp]` above it.  Give the byte
 * a STRUCT MEMBER (`struct Actor { ...; unsigned char f55; }`) and the store
 * gets that member's alias set, the reload is free to move, and sched2 puts it
 * where the ROM has it.
 *
 * MECHANISM CONTROLS, both measured on the finished _200b208 with everything
 * else held fixed:
 *
 *   only the byte store changed back to `((unsigned char *)a)[0x55] = 0`,
 *     struct kept for f10/f40 ......................... 9 differing
 *   the finished file compiled `-fno-strict-aliasing` ... 9 differing
 *   the finished file compiled `--no-sched2` .......... 124 differing (137 lines)
 *
 * The first isolates the lvalue TYPE as the whole lever; the second proves the
 * mechanism is alias sets rather than the struct tag as such; the third proves
 * the reorder is post-reload scheduling.  Read together with "gcse hashes the
 * MEMORY ALIAS SET", the operational consequence is the same and it is worth
 * stating: this TU MUST NOT fall under an `ALIAS_CFLAGS` / per-file
 * `-fno-strict-aliasing` rule.  It does not today -- the Makefile has no rule
 * naming rom_7b0400 -- and adding one would break both functions.
 *
 * Stated as a rule for the notebook:
 *
 *   > When a BYTE store sits EARLIER than the ROM has it and the thing the ROM
 *   > moved above it is a stack SPILL RELOAD, the `char *` spelling is the
 *   > cause, not the cure.  Reach the byte through a struct member.
 *
 * The same edit is worth 5 differing on _200b324, whose hide loop is identical.
 *
 * ------------------------------------------------------------------
 * _200b208 / _200b324: the rest of the levers
 * ------------------------------------------------------------------
 * INDEX, DO NOT WALK.  The first candidate carried an explicit `int *q = arr;`
 * consumed as `*q++` in the two actor loops.  That is 32 differing of 136
 * lines, and the tell is structural, not a register rename: the ROM puts
 * `mov r6, r8` (the walking pointer's birth) AFTER the loop guard
 * `cmp r7,r9 / bcs`, i.e. in the loop PREHEADER, and a source-level `q = arr;`
 * is emitted BEFORE the guard.  Writing `arr[j]` lets loop strength reduction
 * synthesise the pointer itself, and LSR builds its bivs in the preheader --
 * which is where the ROM has them.  It also produces the ROM's `ldmia r6!,{r0}`
 * for the third read of the same element for free.  32 -> 9.
 * (This is "Consume the pointer, do not index it" read in the OTHER direction;
 * that section's exemplar has no loop guard between the two positions, which is
 * what makes the two placements distinguishable here.)
 *
 * The three `__MapActor_GetActor(arr[j])` calls in the inner body are the
 * ROM's three `bl`s with the same argument; only the SECOND is named (`a`),
 * because the ROM's `mov r5, r0` is the only result it keeps.
 *
 * MEASURED (_200b208, tryc `--quiet`, default flag group, ROM 138 lines)
 *   `int *q` walked with `*q++`, two loops ......... 32  (136 lines)
 *   ... + both loops indexed `arr[i]` ..............  9
 *   ... + destination pointer named `d = a + 0x55` ..  9  (INERT -- not shipped)
 *   ... + a named `z = 0` for the stored zero ....... 23
 *   ... + `struct Actor` byte member ............. ** 0 **
 *
 * _200b324 needed none of that sweep -- written directly in the shape
 * _200b208 ended in, its FIRST candidate was exact.  Its only real reading
 * choices were `if ((i & 3) == 3 && i > 0x4b)` for the ROM's two stacked
 * `bne`s, and `if (v < 0xccc) v = 0xccc;` for `ldr r1,=0xccb / cmp / bgt`
 * (gcc canonicalises `< K` to `<= K-1`, which is why the bound and the
 * assigned value differ by one).
 *
 * ------------------------------------------------------------------
 * _200b1c0: THE LEAF -- MERGE THE SHIFT INTO THE SUBTRACT, AND PIN IT
 * ------------------------------------------------------------------
 * 34 instructions, no calls, so the residue can only be allocation and
 * scheduling.  The prologue is the whole function's difficulty:
 *
 *   rom   ldr r3,=iwram_3001ebc / mov r2,r1 / asr r2,#20 / ldr r1,[r3]
 *         / mov r3,#0x40 / sub r2,r3,r2 / mov r6,r2 / mov r5,r2
 *
 * Three separate things had to be true, and each was measured on its own:
 *
 * 1. `i = 0;` AS ITS OWN STATEMENT, WRITTEN FIRST.  With the counter's zero in
 *    the `for`-init, `p` gets r4 and `i` gets r1 -- the ROM has them the other
 *    way round.  Six statement orders x two access forms were swept; the whole
 *    grid is in the table below.  This is "Register allocation follows
 *    ASSIGNMENT position, not declaration order" and "Counter initialisation
 *    wants to come first", and both read exactly as written.
 *
 * 2. `p[i]`, NOT `*p++`.  Same preheader argument as _200b208: the ROM's
 *    `add r1,#0x14` is the LSR biv, not a source pointer.  With the pin in
 *    place, `*p++` is 35 differing of 37 lines -- it costs a whole extra
 *    instruction, so this is not a rename either.
 *
 * 3. THE SHIFT AND THE SUBTRACT ARE ONE VARIABLE, PINNED TO r2.  This is the
 *    part that is not reachable any other way here.  gcc coalesces
 *    `v >> 20` with the incoming parameter and emits `asr r1,#0x14` in place,
 *    which leaves r1 occupied when the global load wants it; the ROM's
 *    `mov r2,r1 / asr r2,#20` is that coalesce NOT happening.  A bare
 *    `register int s __asm__("r2")` for the shift result buys the ROM's copy
 *    and the direct `ldr r1,[r3]` -- 3 differing.  The last 3 are
 *    `sub r2,r3,r2` against `sub r3,r2`: the subtract's DESTINATION.  Writing
 *    the subtract back into the SAME variable -- `s = v >> 20; s = 0x40 - s;`
 *    -- puts the result in the shift's register, which is the pinned one.
 *    That is "The merge lever CHAINS" applied to a two-role value, and it is
 *    the pin that makes the merge worth anything: unpinned, the merged form is
 *    10-17 differing in every one of the six statement orders.
 *
 * The pin makes this file a FAKEMATCH.  Eleven unpinned spellings were
 * measured before reaching for it (table below); none is better than 7.
 *
 * MEASURED (_200b1c0, tryc `--quiet`, default flag group, ROM 36 lines)
 *   order of the three prologue statements: I = `i = 0;`, P = `p = ...;`,
 *   T = the shift/subtract group.  "idx" = `p[i]`, "inc" = `*p++`.
 *
 *   T,P,I  inc  16   |  T,P,I idx 16   |  T,I,P inc 16  |  T,I,P idx 16
 *   P,T,I  inc  12   |  P,T,I idx 15   |  I,T,P inc  8  |  I,T,P idx  8
 *   I,P,T  inc  35 (37 lines)          |  I,P,T idx  7
 *   P,I,T  inc  35 (37 lines)          |  P,I,T idx  7
 *   do/while with the tests inlined ......................... 20
 *   `int i` with the bound cast unsigned .................... 16
 *   `lo`/`hi` each from their own copy of the expression .... 33 (34 lines)
 *   `s` and `t` as separate unpinned locals .................  7
 *   `t = 0x40; t -= v >> 20;` ...............................  7
 *   merged `s = v>>20; s = 0x40-s;` UNPINNED, six orders ..... 10,11,11,16,16,17
 *   `s = v; s >>= 20; s = 0x40 - s;` unpinned, three orders .. 16,16,17
 *   pin on the SUBTRACT's result (`t`) instead, r2 ...........  7
 *   pin on the SUBTRACT's result, r3 ........................  7
 *   pin `p` to r1 ........................................... 29 (37 lines)
 *   pin `s` to r2, `t` kept separate .........................  3
 *   pin `s` to r2, MERGED, but `for (i = 0; ...)` ............  3
 *   pin `s` to r2, MERGED, but `*p++` ....................... 35 (37 lines)
 *   pin `s` to r0 ...........................................  9
 *   pin `s` to r3 ...........................................  8
 *   --no-sched2 (on the best unpinned form) ................. 13
 *   --O1 (on the best unpinned form) ........................ 33 (37 lines)
 *   --no-rerun-cse-after-loop (on the best unpinned form) ....  7
 *   pin `s` to r2, MERGED, as landed ..................... ** 0 **
 *   ... + `p = ...` written before `i = 0;` .............. ** 0 **  (INERT)
 *   ... + the pin declared FIRST among the locals ........ ** 0 **  (INERT)
 *   ... + `register unsigned int s` ...................... ** 0 **  (INERT)
 *
 * The last three are inert and none of them is in the file.  Note the second:
 * once the pin is present the I/P order stops mattering, which is the same
 * shape as batch 237's "with UNINITIALISED pins, DECLARATION ORDER STOPS
 * MATTERING" -- the pin removes the pseudo the ordering was steering.
 *
 * ------------------------------------------------------------------
 * LANDING
 * ------------------------------------------------------------------
 * `overlays/rom_7b0400/overlay.ld` currently carries TWO lines,
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_a.o(.text)
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c_b.o(.text)
 * which collapse back to the ONE pre-split line
 *     \t\tasm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_c.o(.text)
 * (objects built from `src/**.c` land in `asm/` in this tree, so the path in
 * the .ld does not change when a piece is elevated).  The `_b.s` is deleted
 * and `src/.../ovl_314_c_c_c_a_c_c_c_a.c` is renamed to
 * `src/.../ovl_314_c_c_c_a_c_c_c.c`.
 *
 * The same-named `asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_c.o(.text)` in
 * overlays/rom_793768/overlay.ld is a DIFFERENT full path and must not be
 * touched.
 *
 * FLAG GROUP: default (tryc/objcmp's own `-fcall-used-r4` group).  _200b1c0
 * uses r4 without pushing it, which confirms the group independently.
 *
 * FAKEMATCH: two functions here carry register pins --
 *     OvlFunc_925_200af18  (batch 237, `r0`/`r1` argument pins)
 *     OvlFunc_925_200b1c0  (this batch, an `r2` pin on the merged shift)
 * Neither is in fakematch.txt yet; batch 237's landing note asked for the
 * first and it did not get added.  Both need an entry against whatever the
 * file is finally called.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x40 - 0x14];
    int f40;
    unsigned char pad44[0x55 - 0x44];
    unsigned char f55;
};

extern unsigned char *iwram_3001ebc;
extern char *iwram_3001e70;
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Func_800fe9c(void);
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[8];
    short f18;
    unsigned char pad1a[0xe];
};

extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *actor, int f);
extern void __Func_8092950(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct P *p);

void OvlFunc_925_200af18(void)
{
    struct P p;
    struct Actor *a;
    struct Actor *b;
    unsigned int i;
    int t;
    int x, y;
    int x2, y2;

    a = __MapActor_GetActor(0x16);
    b = __MapActor_GetActor(0x18);
    __PlaySound(0xbe);
    {
        register int p1 __asm__("r1");
        register int p0 __asm__("r0");
        p1 = 0x80;
        p0 = 0x16;
        p1 <<= 1;
        __Func_8092950(p0, p1);
    }
    __Func_8092950(0x18, 0x80 << 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t != 0) {
            x = a->f8 + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a->fc + ((__Random() << 5) >> 16 << 16);
            y += 0xfff00000;
            OvlFunc_common0_10c(x, y, a->f10, 0, 0x80 << 11, 0, 0xd8 << 13, &p);
        } else {
            x2 = b->f8 + ((__Random() * 24) >> 16 << 16);
            x2 += 0xfff40000;
            y2 = b->fc + ((__Random() << 5) >> 16 << 16);
            y2 += 0xfff00000;
            OvlFunc_common0_10c(x2, y2, b->f10, 0, 0x80 << 11, 0, 0xd8 << 13, &p);
        }
        i++;
    } while (i <= 0x1f);
    __MapActor_SetPos(0x16, 0, 0);
    __MapActor_SetPos(0x18, 0, 0);
}

void OvlFunc_925_200b060(void)
{
    struct P p;
    struct Actor *a;
    struct Actor *b;
    unsigned int i;
    int t;
    int x, y;
    int x2, y2;
    int k;

    a = __MapActor_GetActor(0x16);
    b = __MapActor_GetActor(0x18);
    __PlaySound(0xbe);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8e << 1;
    p.f8 = 0x6666;
    p.fc = 0xc0 << 10;
    k = 0x80 << 1;
    i = 0;
    do {
        __CutsceneWait(1);
        t = 1 & i;
        if (t != 0) {
            x = a->f8 + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a->fc + ((__Random() << 5) >> 16 << 16);
            y += 0x80 << 14;
            OvlFunc_common0_10c(x, y, a->f10, 0, 0xfffc0000, 0, 0xd8 << 13, &p);
        } else {
            x2 = b->f8 + ((__Random() * 24) >> 16 << 16);
            x2 += 0xfff40000;
            y2 = b->fc + ((__Random() << 5) >> 16 << 16);
            y2 += 0x80 << 14;
            OvlFunc_common0_10c(x2, y2, b->f10, 0, 0xfffc0000, 0, 0xd8 << 13, &p);
        }
        if (i == 0x14) {
            __Func_8092950(0x16, k);
            __Func_8092950(0x18, k);
        }
        i++;
    } while (i <= 0x1f);
    __Func_8092950(0x16, 0);
    __Func_8092950(0x18, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 1);
}

void OvlFunc_925_200b1c0(int *out, int v)
{
    unsigned char **p;
    unsigned char *e;
    unsigned int i;
    int lo, hi;
    unsigned int a;
    int y;
    register int s __asm__("r2");

    i = 0;
    p = (unsigned char **)(iwram_3001ebc + 0x14);
    s = v >> 20;
    s = 0x40 - s;
    lo = s + 8;
    hi = s + 0xb;
    for (; i <= 0x41; i++) {
        e = p[i];
        if (e != 0) {
            a = (*(int *)(e + 8) >> 20) - 4;
            y = *(int *)(e + 0x10) >> 20;
            if (a <= 4 && lo <= y && y < hi)
                *out++ = i;
        }
    }
}

void OvlFunc_925_200b208(void)
{
    int arr[5];
    char *p;
    struct Actor *a;
    unsigned int i;
    unsigned int j;
    unsigned int n;
    int v;

    p = iwram_3001e70 + (0xb2 << 1);
    v = 0x1999;
    n = 0;
    for (i = 0; i <= 4; i++)
        arr[i] = 0x42;
    OvlFunc_925_200b1c0(arr, *(int *)(p + 0xc));
    for (i = 0; i <= 4; i++) {
        if (arr[i] == 0x42)
            break;
        __MapActor_GetActor(arr[i])->f55 = 0;
        n++;
    }
    __PlaySound(0xdf);
    for (i = 0; i <= 0xe3; i++) {
        *(int *)(p + 0xc) -= v;
        for (j = 0; j < n; j++) {
            __MapActor_GetActor(arr[j])->f10 += v;
            a = __MapActor_GetActor(arr[j]);
            a->f40 = __MapActor_GetActor(arr[j])->f10;
        }
        if ((i & 3) == 3)
            v += 0x1999;
        if (v > 0x17fff)
            v = 0xc0 << 9;
        __WaitFrames(1);
    }
    for (i = 0; i < n; i++)
        __MapActor_GetActor(arr[i])->f55 = 0;
}

void OvlFunc_925_200b324(void)
{
    int arr[5];
    char *p;
    struct Actor *a;
    unsigned int i;
    unsigned int j;
    unsigned int n;
    int v;

    p = iwram_3001e70 + (0xb2 << 1);
    v = 0xc0 << 9;
    n = 0;
    for (i = 0; i <= 4; i++)
        arr[i] = 0x42;
    OvlFunc_925_200b1c0(arr, *(int *)(p + 0xc));
    for (i = 0; i <= 4; i++) {
        if (arr[i] == 0x42)
            break;
        __MapActor_GetActor(arr[i])->f55 = 0;
        n++;
    }
    __PlaySound(0xdf);
    for (i = 0; i <= 0x55; i++) {
        *(int *)(p + 0xc) += v;
        for (j = 0; j < n; j++) {
            __MapActor_GetActor(arr[j])->f10 -= v;
            a = __MapActor_GetActor(arr[j]);
            a->f40 = __MapActor_GetActor(arr[j])->f10;
        }
        if ((i & 3) == 3 && i > 0x4b)
            v -= 0x3333;
        if (v < 0xccc)
            v = 0xccc;
        __WaitFrames(1);
    }
    *(int *)(p + 0xc) = 0x80 << 19;
    __Func_800fe9c();
    __WaitFrames(2);
}
