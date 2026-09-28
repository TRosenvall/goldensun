/* CreateSprite @ 0x0800bc70  --  EXACT.
 *
 * objcmp:  ok CreateSprite  163 encodings
 *          OK whole file -- 356 bytes, 163 encodings and 6 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_9000/rom_b798_c_c_c_a_a.s --whole
 *
 * asm/rom_9000/rom_b798_c_c_c_a_a.s HOLDS ONLY THIS FUNCTION AND NO DATA
 * SECTION (datacheck.py), so it CONVERTS WHOLE ON ITS OWN -- no split, pure text.
 *
 * FAKEMATCH: needs one row.  The ROM ends with a DEAD -1 after the
 * Sprite_AddLayer call:
 *
 *     bl Sprite_AddLayer / mov r2, #0x1 / neg r2, r2 / mov r0, r8
 *
 * r2 is never read again (checked to the `bx r1`).  gcc deletes any honest dead
 * assignment, so the two instructions need a barrier.  The WEAKEST form that
 * works is booked here: a plain `int` set to -1 and an empty `asm volatile("")`
 * with it as an input -- NO register pin.  gcc picks r2 by itself.  The counter
 * `i` is reused as the carrier so the file gains no local.
 *   with the barrier   163 of 163 encodings identical
 *   without it         35 of 163 differ, 161 instructions (two short)
 *   with an r2 pin as well   also exact, so the pin is unnecessary
 *
 * LOAD-BEARING SPELLINGS (drop ladder, all measured):
 *
 *   1. THE SEARCH LOOP MUST BE A COUNTED `for` WITH THE POINTER TEST AS A
 *      `break`, not a `while` on the pointer test with a counter bail-out:
 *          for (i = 0; i <= 0x3f; i++) {
 *              if (p[0x20] == 0) { sel = p; break; }
 *              p += 0x38;
 *          }
 *      55 of 163 -> 35 of 163.  Two things change together.  The `while` form
 *      lets strength reduction build a SECOND walking pointer for the 0x20
 *      address (`add r2,#0x38` beside `add r6,#0x38`), where the ROM recomputes
 *      `mov r3,r5 / add r3,#0x20 / ldrb` every iteration; and it peels the exit
 *      test (`cmp r3,#0 / beq END`), where the ROM enters the loop with a bare
 *      `b` to the shared bottom test.  The `for` form gives both at once.
 *      NOTE the offset matters: the sibling park src/non_matching/rom_9000/
 *      800bbc0.c gets the ROM's shape from a plain `while` because ITS field is
 *      at +4, which fits the `ldrb` immediate and is never a giv.
 *   2. `unsigned int dim` for the switch selector -- the ROM's comparisons are
 *      `bhi`, not `bgt`.  `int dim`: 3 of 163 differ.
 *   3. A NAMED `int z = 0` for the halfword store at +0x1e.  Written as a bare
 *      `= 0` gcc emits `ldr r3, =0x0` from the literal pool (the HImode constant
 *      store that reload_cse_move2add cannot chain) and the whole allocation
 *      cascades: 137 of 163 differ at 171 instructions.
 *   4. The six attribute words written through a POST-INCREMENT pointer
 *      (`*q++ = ...`), which is what produces the ROM's six single-register
 *      `stmia r2!, {rX}`.  Indexed as `q[0..5]`: 155 of 163 differ at 153.
 *
 * INERT: `unsigned int attr` vs `int attr` (byte-identical).  `*(p + 0x20)`,
 * `p[32]`, `p = p + 0x38`, `++i` in the condition, an explicit
 * `((unsigned char *)p)[0x20]` cast, a `for (;;)` with two breaks, and reading
 * the field into a local before the test -- all 55, i.e. all the same object as
 * the plain `while`.
 *
 * WORSE: the search loop as a hand-written `goto test` loop (149 of 163); moving
 * `p += 0x38` before the counter bail-out (65).
 *
 * Symbols used: iwram_3001e60 and gSpriteSlots (wram.sym), both already defined.
 * gSpriteSlots is read as `gSpriteSlots[0xbb]` on an `unsigned short` extern,
 * which is what gives the ROM's `mov r1,#0xbb / lsl r1,#1 / add r3,r1 / ldrh`
 * -- 0x176 is outside the `ldrh` immediate range, so reload splits it.
 */
extern unsigned char *_GetSpriteInfo(int id);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int a, int b, int c);
extern void Sprite_AddLayer(unsigned char *h, int id);
extern unsigned char *iwram_3001e60;
extern unsigned short gSpriteSlots[];

void *CreateSprite(int id)
{
    unsigned char *info;
    unsigned char *p;
    unsigned char *sel;
    unsigned int *q;
    int size;
    int tile;
    int i;
    unsigned int dim;
    unsigned int attr;
    int z;

    sel = 0;
    info = _GetSpriteInfo(id);
    size = AllocSpriteSlot();
    p = iwram_3001e60;
    if (info[0] == 0)
        return 0;
    for (i = 0; i <= 0x3f; i++) {
        if (p[0x20] == 0) {
            sel = p;
            break;
        }
        p += 0x38;
    }
none:
    if (sel == 0)
        return 0;
    if (size == 0x60)
        return 0;
    tile = UploadSpriteGFX(size, 0, 0);
    if (tile == 0)
        return 0;
    z = 0;
    sel[0x1c] = size;
    *(unsigned short *)(sel + 0x1e) = z;
    sel[0x26] = 1;
    dim = (info[0] << 8) + info[1];
    switch (dim) {
    case 0x808:
        attr = 0;
        break;
    case 0x810:
        attr = 0x8000;
        break;
    case 0x1008:
        attr = 0x4000;
        break;
    case 0x1010:
        attr = 0x40000000;
        break;
    case 0x1020:
        attr = 0x80008000;
        break;
    case 0x2010:
        attr = 0x80004000;
        break;
    case 0x2020:
        attr = 0x80000000;
        break;
    case 0x2040:
        attr = 0xc0008000;
        break;
    case 0x4020:
        attr = 0xc0004000;
        break;
    case 0x4040:
        attr = 0xc0000000;
        break;
    default:
        attr = 0;
        break;
    }
    q = (unsigned int *)sel;
    *q++ = 0;
    *q++ = attr | 0x2000;
    *q++ = tile | 0x800;
    *q++ = 0;
    *q++ = 0x6000;
    *q = (gSpriteSlots[0xbb] >> 5) | 0x800;
    Sprite_AddLayer(p, id);
    i = -1;
    __asm__ volatile("" : : "r"(i));
    return sel;
}
