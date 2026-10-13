/* Func_801a66c (0x0801a66c) -- NON-MATCHING.
 *
 * NON-MATCHING, 76 of 128 encodings  (batch 347 brief C; was 112 of 128).
 *   121 instructions against the ROM's 121, 268 bytes against 268, stream 128
 *   against 128 -- so for THIS body the figure IS a true distance.  It was NOT
 *   one for the body this park carried until batch 347, and the old header was
 *   wrong twice over: it asserted "the figure IS a distance" directly below its
 *   own "COUNT DIFFERS (ref 128, ours 134)" and "SIZE ref 268 bytes, ours 288",
 *   and the 20-byte gap was FOUR EXTRA POOL WORDS (32-bit stream entries 6
 *   against 10), which no header mentioned at all.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801a66c.c \
 *     asm/rom_15000/rom_1a66c_a_a_a.s --func Func_801a66c
 *
 * ALIGNED RESIDUE (tools/aligncmp.py): 91 in 25 hunks -> 50 in 22 hunks.
 * One function in the piece, no data section, no exported non-function
 * symbols -- a landing needs NO split.  0 devices (tools/shimcount.py).
 *
 * WHAT CLOSED THE GAP.  Three levers, all of them "let gcc REUSE a value":
 *
 *   1. A HImode STORE OF A LITERAL ALWAYS POOLS THE LITERAL.  `movhi`'s Thumb
 *      branch does `force_reg (HImode, operands[1])` when the destination is a
 *      MEM (arm.md:4269-4271), and `*thumb_movhi_insn`'s alternative 1 is
 *      "l"/"mn" -- `n` matches ANY const_int and is tested BEFORE the "l"/"I"
 *      alternative 5 that would emit `mov` (arm.md:4319-4320).  So
 *      `*(short *)(p + 0x39e) = 0x80;` pools 0x80 even though `mov r3,#0x80`
 *      exists, and the same for 0x20, 0x3e7 AND the literal 0 in the loop --
 *      the four extra pool words.  Alternative 1 carries `pool_range` 64
 *      against SImode's 1020 (arm.md:4353 vs :3856), so the pool must land
 *      inside the body and `create_fix_barrier` emits a `b` over it, which the
 *      ROM has nowhere.  The cure is to store an INT: name 0x80 and 0x20, and
 *      use the existing `z` for the loop's zeros.
 *      (This is the general form of the shared brief's lever 4.)
 *
 *   2. `w = p + 0x174; *(short *)(w + 2) = z;` for the 0x176 and 0x1aa stores.
 *      The ROM holds 0xba<<1 = 0x174 and stores at `[r3,#2]` and `[r3,#2]`
 *      again off a +0x34 step; written as absolute 0x176/0x1aa gcc builds
 *      0xbb<<1 and stores at `[r3,#0]`, which costs two.  The old header had
 *      this exact spelling recorded as INERT ("128, 105").  It is not inert:
 *      alone it is worth nothing, crossed with lever 1 it is worth two
 *      instructions and four bytes.  A negative measured on another body is a
 *      measurement of that body.
 *
 *   3. THE BITFIELD ASSIGNMENT ORDER: `s->f5e = 0;` AFTER `s->f7c = 1;`.
 *      The ROM derives the ~0x3e mask from the ~0x03 mask already in r3 --
 *      `mov r3,#4 / neg r3,r3 / and r2,r3 / sub r3,#0x3b` -- one instruction
 *      where gcc otherwise spends `movs r3,#63 / negs r3,r3`.  Moving the f5e
 *      write past the two byte-7 writes puts the two masks adjacent and gcc
 *      makes the derivation itself.  f5e is in byte 5 and f7b/f7c are in byte
 *      7, so the final memory state is identical either way.
 *      Alone this lever is worth 0 instructions and takes the ALIGNED residue
 *      91 -> 74; crossed with levers 1 and 2 it is worth the last two.
 *
 * COMPOSITIONALITY, measured, because no single lever reaches it:
 *      lever        instructions (ref 121)   bytes (ref 268)   ndiff of 128
 *      none                      123              288              112
 *      1 only                    123              272              114
 *      3 only                    123              272              114
 *      1 + 2                     121              268              103
 *      1 + 3                     123              272               98
 *      1 + 2 + 3                 121              268               76   <- this
 *
 * MEASURED AND WORSE / INERT:
 *   naming 0x3e7 as well as 0x80 and 0x20     126 instructions, 276 bytes
 *   naming 0x3e7 only                         121 instructions, 280 bytes
 *   ONE reused `int n` for 0x80 and 0x20      118 instructions (-3), 264 bytes
 *   reused `n` for all three                  118 instructions, 260 bytes
 *   `z` itself used as the constant carrier   118 instructions, 260 bytes
 *   reusing n20's pseudo for 0x3e7            124 instructions, 272 bytes
 *   late definition of n80/n20 at their use   120 instructions
 *   declaration order of n80/n20              identical, 76
 *   a second zero variable after the loop     identical, 76
 *   f7b moved before f5a only                 123 instructions, 272 bytes
 *   f7c moved after f9b                       123 instructions, 272 bytes
 *   The reused-carrier rows are the informative ones: ONE pseudo for both
 *   constants lets gcc chain the whole offset walk and it goes THREE SHORT.
 *
 * FLAG AXIS CLOSED on the old body (tools/autosweep.py, batch 347): eight
 * groups, all inert or worse; -O1's 110/128 RAISES the aligned residue
 * 91 -> 102.  ANCHOR AXIS NOT CLOSED -- autosweep CAPPED at 40 of 43
 * boundaries, so boundaries 40..42 are UNTESTED, not inert.  Both axes need
 * re-running against THIS body, which has a different statement list.
 *
 * STILL TRUE FROM THE OLD HEADER: the offset chain comes free from plainly
 * written absolute offsets -- do not try to spell it; the five-iteration
 * do/while over two 0x34-stride pointers; UploadSpriteGFX reading its slot
 * argument back out of the halfword it was just stored to.
 *
 * REFUTED FROM THE OLD HEADER, which is why the park was mis-ranked:
 *   * "`*(short *)(p + 0x3b8) = 0x3e7;` gives the ROM's `add r3, #0x2f` ...
 *     when it is written as a literal."  It does not.  As a literal it is a
 *     POOLED LOAD, by lever 1 above; it is the derived form that the ROM has.
 *   * "NEXT: nothing source-level."  Three source levers were worth 2
 *     instructions, 20 bytes and 41 aligned encodings.
 *   * The register rotation the old header blamed (r7/r5/r6 against r6/r7/r5)
 *     is still there and is most of the remaining 76 -- but it was never the
 *     length gap, and the length gap was what made the figure unreadable.
 *
 * NEXT: the rotation.  This body holds p in r7 like the ROM, and spends a
 * 4-byte stack slot on the shared zero because the two named constants take
 * r5 and r6 and `-fcall-used-r4` makes r4 unable to hold it across the two
 * calls.  Get the zero into r5 and the stack slot goes.
 */
struct Obj {
    unsigned char pad0[5];
    unsigned char f5a : 2, f5b : 2, f5c : 1, f5d : 1, f5e : 2;
    unsigned char pad6;
    unsigned char f7a : 1, f7b : 5, f7c : 2;
    unsigned char pad8;
    unsigned char f9a : 2, f9b : 2, f9c : 4;
};

extern unsigned char *galloc_ewram(int tag, int size);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern unsigned char Data_346f8[];

void Func_801a66c(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    struct Obj *s;
    void *gfx;
    int i;
    int z;
    unsigned char *w;
    int n80 = 0x80;
    int n20 = 0x20;

    p = galloc_ewram(0x12, 0xf9 << 2);
    z = 0;
    *(int *)(p + 0x348) = z;
    *(int *)(p + 0x34c) = z;
    *(int *)(p + 0x350) = z;
    *(short *)(p + 0x39a) = z;
    *(short *)(p + 0x39c) = z;
    *(short *)(p + 0x39e) = n80;
    *(short *)(p + 0x3a0) = n20;
    *(short *)(p + 0x394) = z;
    *(short *)(p + 0x3b8) = 0x3e7;
    q = p + 0x1de;
    r = p + 0x72;
    i = 0;
    do {
        i++;
        *(short *)r = z;
        *(short *)q = z;
        r += 0x34;
        q += 0x34;
    } while (i != 5);
    z = 0;
    w = p + 0x174;
    *(short *)(w + 2) = z;
    *(short *)(w + 0x36) = z;
    *(short *)(p + 0xa) = z;
    *(short *)(p + 0x3e) = z;
    *(short *)(p + 0x12) = z;
    *(short *)(p + 0x46) = z;
    gfx = Data_346f8;
    *(short *)(p + 0x2e4) = AllocSpriteSlot();
    *(short *)(p + 0x2e6) = UploadSpriteGFX(*(unsigned short *)(p + 0x2e4), 0x80 << 1, gfx);
    *(short *)(p + 0x2e2) = z;
    *(short *)(p + 0x2fa) = z;
    *(short *)(p + 0x316) = z;
    s = (struct Obj *)(p + 0x300);
    s->f5b = 0;
    s->f5c = 0;
    s->f5d = 1;
    s->f5a = 0;
    s->f7b = 0;
    s->f7c = 1;
    s->f5e = 0;
    s->f9b = 0;
}
