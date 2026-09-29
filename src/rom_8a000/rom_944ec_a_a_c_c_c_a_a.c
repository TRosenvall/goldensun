/* Func_809537c  --  0x0809537c, asm/rom_8a000/rom_944ec_a_a_c_c_c_a_a.s
 * SINGLE function in the file: a WHOLE-FILE replacement, no split needed.
 *
 * EXACT.  objcmp:
 *     OK Func_809537c -- 564 bytes, 244 encodings and 38 relocations identical
 *   SIZE 564 bytes.  INSTRUCTION COUNT 244 (objcmp's figure, pool words included).
 *   tools/shimcount.py: 0 shims -- PIN-FREE.
 *   `python3 tools/datacheck.py asm/rom_8a000/rom_944ec_a_a_c_c_c_a_a.s` prints
 *   NOTHING: no data sections, no required .global data exports.
 *   The .o is named on ONE linker line:
 *       stage1.ld:1019   asm/rom_8a000/rom_944ec_a_a_c_c_c_a_a.o(.text)
 *   which keeps naming asm/rom_8a000/rom_944ec_a_a_c_c_c_a_a.o -- Makefile:146
 *   builds it from src/rom_8a000/rom_944ec_a_a_c_c_c_a_a.c.  No park to repoint.
 *
 * NOTE ON THE .s PROSE: the reference's `@` comment calls this
 * RestorePlayerControl with "r0=player id, r1=mode" and says it restores field
 * control.  The id is right; there is NO second parameter (r1 is never read), and
 * the body is a cutscene SCRIPT -- CutsceneStart, a fixed sequence of
 * WaitFrames/_PlaySound/jumps, eight spawned clone actors chained through +0x68,
 * then CutsceneEnd.  Signature shipped as `void Func_809537c(int id)`.
 *
 * SECOND CANDIDATE WAS EXACT.  The first was 201 of 244 aligned (2 instructions
 * short); TWO already-recorded levers closed the whole gap, and nothing else in
 * the reconstruction needed a decision.  What made it cheap was reading the shape
 * off the reference literally -- it is a straight-line script with no loop except
 * the eight-clone one, so every call argument and every store order is visible.
 *
 * ============ THE TWO LEVERS ============
 *
 * 1. THE gState OFFSET MUST BE BUILT, NOT FOLDED.  docs/elevation.md line 3413,
 *    and its named example is THIS EXACT OFFSET:
 *        rom   ldr r3, =gState / mov r2, #0xfa / lsl r2, #1 / add r3, r2 / ldr r0, [r3]
 *        ours  ldr r3, =gState+500 / ldr r0, [r3]
 *    Written `*(int *)(gState + (0xfa << 1))` gcc folds symbol and offset into one
 *    pool entry, which cost 3 instructions AND changed the relocation addend
 *    (ref's pool word is 0x00000000, ours was 0x000001f4 -- the addend is the tell
 *    and objcmp reports it as a relocation difference).  Assigning gState to a
 *    local `unsigned char *g` first blocks the fold; the offset then has to be
 *    real arithmetic, and 0x1f4 is `mov #0xfa / lsl #1`.
 *      Worth noting against line 7361, which reads as if this is a PARK ("above
 *      124 the lever is not a candidate and the difference is a park"): that entry
 *      is about a DISPLACEMENT not fitting, and the local-base lever is unrelated
 *      to the displacement -- it works here at 0x1f4 on the first try.  The two
 *      entries do not contradict once you read 7361 as being about `[rN, #imm]`
 *      and 3413 as being about the POOL FOLD.
 *
 * 2. A BYTE MASK WRITTEN AS A BITFIELD GIVES THE WRONG COMPLEMENT.       -1 insn
 *    The +0x5a byte is cleared with `mov r3, #0xfe / and r3, r2` and set with
 *    `mov r2, #1 / orr r3, r2`.  Declared as `unsigned char f5a0 : 1;` and
 *    assigned 0, gcc computes the complement in SImode and emits
 *    `mov r3, #2 / neg r3, r3` (0xfffffffe) -- one instruction too many.
 *    `unsigned char f5a; ... f5a &= 0xfe;` keeps the mask in QImode, where 0xfe is
 *    a plain `mov #imm8`.
 *      THE SAME FUNCTION WANTS THE OTHER SPELLING 20 LINES AWAY: the 2-bit field
 *      at bit 2 of byte +9 of the +0x50 sub-object IS a bitfield, and its ROM mask
 *      is `mov r3, #0xd / neg r3, r3` -- the SImode ~0xc that the bitfield insert
 *      produces.  So within one function, `~0xc` is a bitfield and `0xfe` is not.
 *      THE TELL IS THE COMPLEMENT'S FORM, NOT ITS WIDTH: a mask that arrives as a
 *      bare `mov #imm8` is explicit masking; a mask that arrives as `mov / neg` is
 *      a bitfield insert.  Both fit imm8 here (0xfe and 0xf3), so the imm8 test
 *      the docs suggest at line 690 does NOT separate them -- the neg does.
 *
 * ============ WHAT THE REFERENCE ESTABLISHES ============
 *
 *  - `bl CutsceneStart` with r0 still holding MapActor_GetActor's return proves
 *    CutsceneStart takes no arguments; same for CutsceneEnd at the tail.
 *  - `_CreateActor` is called with FOUR arguments here (r2, r3, r1, r0 -- r0 LAST,
 *    the recorded "callee returns a value" order), matching the four-parameter
 *    declaration four other elevated files in this bank already use.  Contrast
 *    Task_08097644 in rom_97384, which calls it with TWO.
 *  - `sub r2, #0x32` after `add r2, #0x55` is ONE base with two byte offsets:
 *    0x55 - 0x32 = 0x23.  Both exceed `strb`'s imm5 (31) so both addresses must be
 *    materialised, and reload_cse_move2add derives the second from the first.
 *    Nothing in the source provokes it; writing `n->f55 = 0; n->f23 = 2;` is enough.
 *  - `str r2, [sp, #4]` spilling `a + 0x55` across ~40 instructions and 12 calls is
 *    cse keeping the address available for the late `a->f55 = 0;`.  It needs NO
 *    pointer variable -- two plain `a->f55 =` statements produce it.
 *  - FRAME AND DECLARATION ORDER: sp is 0x30.  `void *arr[8]` occupies sp+0x10..0x2f,
 *    then sp+0xc (the Func_8096c48 accumulator), sp+8 (the colorswap id), sp+4
 *    (the cse'd &a->f55) and sp+0 (gcc's own copy of &arr[0]).  Declaring
 *    arr, then c, then cs -- highest offset first -- is what puts them there.
 *  - `cmp r7, #7 / ble` is SIGNED, so `i` is `int` and the loop is `i <= 7`.
 *  - The three identical Func_8096bec / _PlaySound(0x98) / MapActor_Jump(id,4,0) /
 *    _Actor_WaitMovement(a) groups are written out three times; 0x100000 is
 *    materialised once (`mov r5,#0x80 / lsl r5,#13`) and shared by cse across all
 *    three, so the literal can be repeated in the source.
 *  - The tail's store order is f24, f2c, f28 then f38, f40, f3c, and the clone
 *    block's is f1c before f18; both are shipped exactly as the reference emits
 *    them.  Whether address order would also match was NOT measured -- the first
 *    spelling was exact, so the reordered variants were never run.
 */
struct Sub {
    unsigned char pad00[9];
    unsigned char b09a : 2;
    unsigned char f09 : 2;
    unsigned char b09b : 4;
    unsigned char pad0a[0x28 - 10];
    short *f28;
};

struct Actor {
    unsigned char pad00[6];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    int f24;
    int f28;
    int f2c;
    unsigned char pad30[8];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x50 - 0x44];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[4];
    unsigned char f5a;
    unsigned char pad5b[0x68 - 0x5b];
    struct Actor *f68;
    void *f6c;
};

extern unsigned char gState[];
extern struct Actor *MapActor_GetActor(int id);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_80925cc(int id, int a);
extern void Func_8092adc(int id, int a, int b);
extern void Func_8096bec(struct Actor *a, int b, int c);
extern void MapActor_Jump(int id, int a, int b);
extern void _Actor_WaitMovement(struct Actor *a);
extern struct Actor *_CreateActor(int kind, int x, int y, int z);
extern void _Actor_SetColorswap(struct Actor *a, int c);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetSpriteFlags(struct Actor *a, int n);
extern int Func_8096c48(struct Sub *s, int v);
extern void Func_8095348(void);

void Func_809537c(int id)
{
    void *arr[8];
    int c;
    int cs;
    struct Actor *a;
    struct Actor *o;
    struct Actor *prev;
    struct Actor *n;
    int dir;
    int ang;
    int kind;
    int i;
    unsigned char *g;

    a = MapActor_GetActor(id);
    g = gState;
    o = MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    dir = (o->f06 + 0x2000) & 0xc000;
    CutsceneStart();
    WaitFrames(0xa);
    _PlaySound(0xad);
    Func_80925cc(id, 1);
    _PlaySound(0xaf);
    Func_80925cc(id, 1);
    ang = 0x8000 + dir;
    WaitFrames(0x14);
    Func_8092adc(id, ang, 0);
    WaitFrames(0xa);
    a->f50->f09 = 0;
    a->f06 = ang;
    MapActor_GetActor(id)->f5a &= 0xfe;
    a->f55 = 2;
    Func_8096bec(a, 0x100000, dir);
    _PlaySound(0x98);
    MapActor_Jump(id, 4, 0);
    _Actor_WaitMovement(a);
    Func_8096bec(a, 0x100000, dir);
    _PlaySound(0x98);
    MapActor_Jump(id, 4, 0);
    _Actor_WaitMovement(a);
    Func_8096bec(a, 0x100000, dir);
    _PlaySound(0x98);
    MapActor_Jump(id, 4, 0);
    _Actor_WaitMovement(a);
    WaitFrames(0x14);
    kind = *a->f50->f28;
    cs = 9;
    if (kind == 0x5a)
        cs = 2;
    if (kind == 0x5c)
        cs = 0xa;
    if (kind == 0x5b)
        cs = 9;
    c = 0;
    prev = a;
    for (i = 0; i <= 7; i++) {
        n = _CreateActor(kind, a->f08, a->f0c, a->f10);
        arr[i] = n;
        if (n != 0) {
            n->f1c = 0xf000;
            n->f18 = 0xf000;
            n->f55 = 0;
            n->f23 = 2;
            n->f5a |= 1;
            n->f6c = Func_8095348;
            n->f06 = a->f06;
            n->f50->f09 = 0;
            _Actor_SetColorswap(n, cs);
            _Actor_SetAnim(n, 0);
            _Actor_SetSpriteFlags(n, 0);
            c = Func_8096c48(n->f50, c);
            n->f68 = prev;
            prev = n;
        }
    }
    Func_8096bec(a, 0x400000, 0x8000 + dir);
    _PlaySound(0x88);
    MapActor_Jump(id, 0xc, 0);
    WaitFrames(0x18);
    a->f55 = 0;
    a->f24 = 0;
    a->f2c = 0;
    a->f28 = 0;
    a->f38 = 0x80000000;
    a->f40 = 0x80000000;
    a->f3c = 0x80000000;
    _Actor_SetAnim(a, 0);
    a->f50->f09 = 2;
    CutsceneEnd();
}
