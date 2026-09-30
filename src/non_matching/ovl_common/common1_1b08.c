/* OvlFunc_common1_1b08  --  0x0200?b08 in each of three overlays   [PARK DRAFT]
 *
 * NON-MATCHING, 442 of 454 encodings differ.  Reference
 * asm/overlays/common/common1_c_a_c_c_a_c_c.s, 2nd of THREE functions
 * (OvlFunc_common1_1928, this one, OvlFunc_common1_1ecc -- the other two are
 * already parked in src/non_matching/ovl_common/).  tools/datacheck.py prints
 * nothing: no data section in this .s, so no TEXT/DATA split and NO new exports.
 * common1 is a SHARED object named by EXACTLY THREE overlay.ld rows
 * (overlays/rom_7db0c8, rom_7ddb88, rom_7e0928) -- `grep -rln common1 overlays/`
 * hits 10 files, but only 3 of them are overlay.ld.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_common/common1_1b08.c \
 *       asm/overlays/common/common1_c_a_c_c_a_c_c.s --func OvlFunc_common1_1b08
 *
 * NOT A DISTANCE: size 864 against 964 AND count 401 against 454, so objcmp's
 * 442 is saturated.  tools/aligncmp.py reads aligned-equal 152 of 454 (33.5%),
 * 413 differing/ins/del in 71 hunks.  SHIMS: 0 pins, 0 barriers
 * (tools/shimcount.py prints nothing).
 *
 * ============================================================
 * BLOCKER 1 -- AND IT IS A LANDING BLOCKER, NOT A MATCHING ONE:
 * THE `.L4` / `.L5` CAPTURE HAZARD IS REAL HERE, AND MEASURED.
 *
 * This function needs two asm-label externs, `.L4` (the DMA source) and `.L5`
 * (the LZ-compressed sprite), both `.global` in
 * asm/overlays/common/common1_c_c_b_c.s.  They are ONE DIGIT, and gcc names its
 * own branch targets the same way.  The generated assembly for the candidate
 * below DEFINES BOTH:
 *
 *     $ ... gens | grep -oE '^\.L[0-9]+:' | sort -u
 *     .L10: .L14: .L19: .L22: .L24: .L25: .L26: .L27: .L28: .L29:
 *     .L2: .L3: .L4: .L5:
 *
 * So `extern unsigned char L4[] __asm__(".L4");` would bind to gcc's own local
 * label inside the same object -- a silently wrong program, not a build error.
 * This is the hazard src/non_matching/ovl_common/common1_1928.c records for its
 * two-digit `.L15` and could not trigger; at one digit it triggers every time.
 * NOTHING IN THE SOURCE CAN AVOID IT: the label numbers follow gcc's internal
 * counter, and this function has ~29 of them.  The remedy is the one that park
 * names -- RENAME AND EXPORT the two data labels in
 * asm/overlays/common/common1_c_c_b_c.s (and fix the 3 `ldr rN, =.L4/.L5`
 * references in asm/overlays/common/, one of which belongs to
 * OvlFunc_common1_1ecc) -- AND THAT IS AN OWNER DECISION, because it edits a
 * shared data file that three overlays link.
 *
 * Landing it also needs common1_c_a_c_c_a_c_c.s SPLIT THREE WAYS (or all three
 * of its functions solved together), with the three overlay.ld rows updated.
 *
 * ============================================================
 * BLOCKER 2 -- THE OUTPUT CURSOR IS IN MEMORY AND WE KEEP IT IN A REGISTER.
 * This is the whole of the 53-instruction shortfall.
 *
 * The ROM's frame is 0x14 (five words: 0 = a caller-save temp, 4 = n<<4,
 * 8 = the tile number, 0xc = the write cursor, 0x10 = the struct base); ours is
 * 0x10.  Around EVERY one of the ~21 record words the ROM reads the cursor back
 * and writes it out:
 *
 *     ldr  r2, [sp, #0xc]
 *     stmia r2!, {r3}
 *     mov  r1, r2
 *     str  r1, [sp, #0xc]
 *
 * -- the value stays in r2 across the three words of a record (no reload
 * between them) but is STORED BACK after each increment.  That is the signature
 * of a pointer that lives in memory with CSE satisfying the reads, and it is
 * two extra instructions per word: 21 words x ~2.5 = the 53 encodings we are
 * short.  We emit a plain register `*w++`.
 *
 * MEASURED AND DISPROVED -- the obvious hypothesis is wrong.  Wrapping the three
 * words in a `static inline void Emit(unsigned int **wp, unsigned char *e, ...)`
 * so that `&w` is taken (the family's own idiom, as in
 * src/overlays/common/common1_a_a_a_a_c_c_a_c_b.c) does NOT put the cursor in
 * memory: gcc-2.96 sees through `*wp` after inlining and the figure gets WORSE,
 * 33.5% -> 23.6% and 401 -> 395 encodings.  So TREE_ADDRESSABLE via an inlined
 * by-reference parameter is not the construct.  What is needed is something that
 * forces the pointer to the stack while leaving CSE free to keep it in r2 within
 * a record -- and that is the open question for this function.
 *
 * The same shortfall explains the register roles: the ROM has five values live
 * across the whole body (cursor, base, tile, n<<4, record pointer) plus r9/r10
 * carrying the per-block OAM constants, and spends r8-r11 on them; with the
 * cursor in a register we have one to spare and nothing matches.
 *
 * ============================================================
 * WHAT IS ESTABLISHED.
 *
 * 1. IT RETURNS void AND HAS NO ARGUMENTS; the task state is
 *    `*(T **)iwram_3001f3c`, copied into THREE separate variables at the top
 *    (`adds r1,r3,#0 / mov r8,r1 / str r3,[sp,#0xc] / str r3,[sp,#0x10]`): the
 *    record pointer handed to __Func_8003dec, the write cursor, and the struct
 *    base used for field reads.  They are one value and three variables.
 *
 * 2. gSpriteSlots IS AN ARRAY OF 4-BYTE ENTRIES AND THE FIELD IS THE SECOND
 *    HALFWORD: `lsl r3,#2 / add r3,r2 / ldrh r3,[r3,#2]`.  A `unsigned short []`
 *    with index `slot*2+1` gives `adds r3,#2 / ldrh r3,[r2,r3]` instead -- a
 *    two-member struct is what produces the immediate offset.
 *
 * 3. THE HALFWORD COUNTER AT +0xda IS READ SIGNED FOR THE TEST AND UNSIGNED FOR
 *    THE ARITHMETIC, and the arithmetic must promote to int.  Written as
 *    `*(unsigned short *)dp -= 1` the constant folds to the HImode 0xffff and
 *    gcc POOLS it (`ldr r1,[pc] / adds r3,r2,r1`); through an `int` temp it is
 *    `subs r3,r2,#1` as in the ROM.  This is the same HImode-signedness
 *    mechanism that decided OvlFunc_891_200905c's range test, met twice more
 *    here: the `== 1` test after the increment is also a HImode compare
 *    (`lsl r3,#16 / cmp r3, 0x10000`), so it is `(short)u == 1`.
 *
 * 4. THE DMA PUSH IS A THREE-WORD BLOCK STORE to REG_DMA3SAD
 *    (`stmia r3!, {r0, r1, r2}` with 0x50003c0 and 0x80000010), and the
 *    `sub r3, #0xc` after it is DEAD -- a gcc artefact of the post-increment,
 *    not a source statement.
 *
 * 5. THE TWO n-ITERATION LOOPS ARE NOT SYMMETRIC IN THE ROM.  The first
 *    computes its y field per iteration (`lsl r2,r6,#4 / mov r3,#0x60 /
 *    sub r4,r3,r2 / lsl r3,r4,#16`); the second is STRENGTH-REDUCED into an
 *    accumulator in r10 starting at 0x800000 and stepped by 0x100000
 *    (`add r10, r1` with r1 = 0x80<<13).  Same shape of expression, two
 *    different treatments, so the source forms differ -- probably
 *    `(0x80 << 16) + (i << 20)` against `(0x60 - (i << 4)) << 16`.
 *
 * 6. THE OAM ATTRIBUTE WORD IS ACCUMULATED IN PLACE (`orr r7, r3` three times
 *    on the same register) at the sixth record, and REBUILT from the actor
 *    division at the last two (`sub r7, r0, #4 / and r7, r3` with 0xff).
 *
 * 7. THE TWO ACTOR BLOCKS ARE IDENTICAL except for the field they read
 *    (+0xe0 then +0xde) and the tile offset (+0xc then +8), and the LAST record
 *    word is a plain `str` rather than a post-increment because the cursor dies
 *    -- so the source is the same sequence twice, not a loop.
 *
 * 8. THE DIVISOR IS 0xe0 << 12 (`mov r5,#0xe0 / lsl r5,#12`) and the calls are
 *    `_divsi3_RAM`, i.e. plain signed `/` in C; the overlay .ld supplies
 *    `__divsi3 = _divsi3_RAM`.
 *
 * ============================================================
 * WHAT IT DOES.  The per-frame task of the "party follows you" sprite train.
 * It fades a counter at +0xda in or out depending on save bit 0x106, uploads the
 * train's sprite sheet on the frame the counter first reaches 1, and then, if
 * the counter is non-zero, emits a display list through __Func_8003dec: a head
 * record, `n` body records going up the screen, two tail records, `n` more body
 * records going down, a final record, and -- only when the low nibble of
 * *iwram_3001e40 exceeds 4 -- one record for each of the two field actors named
 * at +0xe0 and +0xde, positioned by dividing their world coordinates by
 * 0xe0000.  When the counter is zero it calls __Func_8003f78 with the sprite
 * slot and returns.
 */
extern unsigned char *iwram_3001f3c;
extern unsigned char iwram_3001e40[];
struct SpriteSlot {
    unsigned short a;
    unsigned short b;
};
extern struct SpriteSlot gSpriteSlots[];
extern unsigned char L4[] __asm__(".L4");
extern unsigned char L5[] __asm__(".L5");

extern int __GetFlag(int id);
extern void *__Func_8004970(int size);
extern void __DecompressLZ(const void *src, void *dst);
extern void __UploadSpriteGFX(int slot, int size, const void *src);
extern void __free(void *p);
extern void __Func_8003f78(int slot);
extern void __Func_8003dec(unsigned char *p, int n);
extern unsigned char *__GetFieldActor(int id);

#define REG_DMA3SAD (*(volatile unsigned int (*)[3])0x40000D4)

void OvlFunc_common1_1b08(void)
{
    unsigned char *b;
    unsigned char *e;
    unsigned int *w;
    short *slotp;
    int u;
    short *dp;
    unsigned char *act;
    void *gfx;
    unsigned int tile;
    unsigned int attr;
    unsigned int n;
    unsigned int sh;
    int k;
    int x, y;
    unsigned int i;
    int t;

    b = iwram_3001f3c;
    w = (unsigned int *)b;
    e = b;
    slotp = (short *)(e + 0xd8);
    tile = gSpriteSlots[*slotp].b >> 5;
    n = *(short *)(e + 0xe6);
    t = *(short *)(e + 0xdc);
    if (t != 0) {
        dp = (short *)(e + 0xda);
        *dp = 2;
    } else if (__GetFlag(0x83 << 1) != 0) {
        dp = (short *)(e + 0xda);
        u = *(unsigned short *)dp;
        if (*dp > 0)
            *(unsigned short *)dp = u - 1;
    } else {
        dp = (short *)(e + 0xda);
        u = *(unsigned short *)dp;
        if (*dp <= 1) {
            u = u + 1;
            *(unsigned short *)dp = u;
            if ((short)u == 1) {
                REG_DMA3SAD[0] = (unsigned int)L4;
                REG_DMA3SAD[1] = 0x50003c0;
                REG_DMA3SAD[2] = 0x80000010;
                gfx = __Func_8004970(0x80 << 2);
                __DecompressLZ(L5, gfx);
                __UploadSpriteGFX(*slotp, 0x80 << 2, gfx);
                __free(gfx);
            }
        }
    }
    k = *dp;
    if (k == 0) {
        __Func_8003f78(*(short *)(b + 0xd8));
        return;
    }
    attr = (k * 6 - 8) & 0xff;
    sh = n << 4;
    *w++ = 0;
    *w++ = ((0x68 - sh) << 16) | attr | (0x80 << 8);
    *w++ = tile | (0xe4 << 8);
    __Func_8003dec(e, 0xff);
    e += 0xc;
    for (i = 0; i < n; i++) {
        *w++ = 0;
        *w++ = ((0x60 - (i << 4)) << 16) | attr | (0x80 << 23);
        *w++ = (tile + 2) | (0xe4 << 8);
        __Func_8003dec(e, 0xff);
        e += 0xc;
    }
    *w++ = 0;
    *w++ = (0xe0 << 15) | attr | (0x80 << 8);
    *w++ = (tile + 6) | (0xe4 << 8);
    __Func_8003dec(e, 0xff);
    e += 0xc;
    *w++ = 0;
    *w++ = (0xf0 << 15) | attr | (0x80 << 8) | (0x80 << 21);
    *w++ = (tile + 6) | (0xe4 << 8);
    __Func_8003dec(e, 0xff);
    e += 0xc;
    for (i = 0; i < n; i++) {
        *w++ = 0;
        *w++ = ((0x80 + (i << 4)) << 16) | attr | (0x80 << 23) | (0x80 << 21);
        *w++ = (tile + 2) | (0xe4 << 8);
        __Func_8003dec(e, 0xff);
        e += 0xc;
    }
    *w++ = 0;
    attr |= ((sh + 0x80) << 16) | (0x80 << 8) | (0x80 << 21);
    *w++ = attr;
    *w++ = tile | (0xe4 << 8);
    __Func_8003dec(e, 0xff);
    e += 0xc;
    if ((*(int *)iwram_3001e40 & 0xf) <= 4)
        return;
    act = __GetFieldActor(*(short *)(b + 0xe0));
    if (act != 0) {
        x = (*(int *)(act + 8) - *(int *)(b + 0xe8)) / (0xe0 << 12);
        x += 0x70;
        y = (*(int *)(act + 0x10) - *(int *)(b + 0xec)) / (0xe0 << 12);
        y += *(short *)(b + 0xda) * 6;
        attr = (y - 4) & 0xff;
        *w++ = 0;
        attr |= (x << 16) | (0x80 << 23);
        *w++ = attr;
        *w++ = (tile + 0xc) | (0xe4 << 8);
        __Func_8003dec(e, 0xff);
        e += 0xc;
    }
    act = __GetFieldActor(*(short *)(b + 0xde));
    if (act != 0) {
        x = (*(int *)(act + 8) - *(int *)(b + 0xe8)) / (0xe0 << 12);
        x += 0x70;
        y = (*(int *)(act + 0x10) - *(int *)(b + 0xec)) / (0xe0 << 12);
        y += *(short *)(b + 0xda) * 6;
        attr = (y - 4) & 0xff;
        *w++ = 0;
        attr |= (x << 16) | (0x80 << 23);
        *w++ = attr;
        *w = (tile + 8) | (0xe4 << 8);
        __Func_8003dec(e, 0xff);
    }
}
