/* Func_8098698 -- 0x08098698 (AnimateTargetRise).  EXACT.
 * ref: asm/rom_8a000/rom_97b54_a_c_a_c_c_a_a_a.s  (ONE function, no data
 *      section -- grep -ci func_start = 1; converts WHOLE FILE, no split)
 * objcmp: OK whole file -- 432 bytes, 197 encodings and 17 relocations identical.
 *
 * batch 292, brief A, target 4.  NO SHIMS: no pins, no barriers, no volatile,
 * no do{}while(0), no .equ, default flags.
 *
 * THREE LOAD-BEARING CONSTRUCTS.  Each was dropped singly against the finished
 * file; objcmp reports the number in brackets and in every case the candidate
 * still had 197 encodings unless noted, so the numbers are true distances.
 *
 * 1. A NAMED DESTINATION POINTER FOR THE SECOND SPRITE PART.  [125 of 197,
 *    ours 193 -- lengths disagree, so that one is a shortfall, not a distance]
 *    The ROM builds the address once (`mov r0, r4 / add r0, #0xc`) and reaches
 *    every one of the five bitfield inserts through it (`[r0, #5]`, `[r0, #7]`,
 *    `[r0, #8]`, `[r0, #9]`).  Written as `q[1].field = q[0].field` gcc folds
 *    0xc into each offset instead (`strb r3, [r4, #0x11]`) and the block comes
 *    out four instructions short.  `d = q + 1;` restores it.  This is
 *    docs/elevation.md's "Name the store's DESTINATION pointer when the ROM
 *    computes the address first", and the sibling reading "a member array keeps
 *    base and offset in separate registers" from the other direction.
 *
 * 2. AN int CARRIER FOR THE BYTE ZERO AT +0x55.  [75 of 197]  Written
 *    `*(char *)(p + 0x55) = 0;` gcc serves the byte store from the register
 *    that already holds 0xc0 << 9, whose low byte is zero -- one instruction
 *    fewer and a different register.  `z = 0;` immediately before the store
 *    brings back the ROM's own `mov r3, #0 / strb r3, [r2]`.  (The mirror of
 *    "A byte-sized zero can be served by a register whose LOW BYTE is already
 *    zero": the ROM is on the other side of it here.)
 *
 * 3. THE 24-ITERATION LOOP IS WRITTEN COUNTING UP.  [2 of 197]  The ROM counts
 *    DOWN from 0x17 (`mov r0, #1 / neg r0, r0 / add r11, r0 / cmp r2, #0 /
 *    blt`), and writing that literally as `for (m = 23; m >= 0; m--)` costs the
 *    two encodings of the initialiser; `for (m = 0; m < 24; m++)` and letting
 *    check_dbra_loop reverse it is exact.  `m` appears nowhere in the body,
 *    which is the condition docs/elevation.md states for this lever.  Its twin
 *    Field_Halt (target 2 of this brief) needed the same rewrite and was worth
 *    7 -> 2 there.
 *
 * THE BITFIELD LAYOUT is read off the five insert widths and, crucially, off
 * WHICH WIDTH each insert uses:
 *    +5 bit 5, one bit      -> `mov r1,#0x20 / mov r5,#0x21 / neg r5` (QImode)
 *    +5 bits 6-7, two bits  -> merged into the SAME r3 as the insert above,
 *                              which is "several bitfield writes to one byte
 *                              MERGE -- and their order is the ROM's"
 *    +7 bits 6-7            -> QImode
 *    +8 bits 0-9, ten bits  -> HImode, `lsl #22 / lsr #22` + `ldr r2,=0xfffffc00`
 *    +8 bits 12-15          -> QImode on byte +9, because the field lies wholly
 *                              inside that byte even though its container is
 *                              the halfword
 * The 10-bit spelling is src/rom_a1000/rom_a4f08_b.c's `unsigned short tile :
 * 10` idiom, found by grepping the elevated corpus' generated asm for
 * `lsl rN, rM, #22` (two hits, both that idiom).  struct Spr must be 12 bytes
 * for `q + 1` to be +0xc, hence the trailing pad.
 *
 * MEASURED AND INERT: naming the first vec3_translate argument
 * (`r = Random() * 6 + (0x80 << 11)`) -- dropped, it is not in the shipped
 * source; `0xa0000` written as a plain literal instead of `0xa0 << 12`.
 *
 * TWO ROM SHAPES THAT ARE NOT SOURCE CONSTRUCTS, for the next reader:
 *   - the bitfield block sits BEFORE `cmp r6, #0`, so the source really does
 *     dereference p->sprite before testing p for null.  It is written that way
 *     here and it is what matches.
 *   - `blt <near> / b <loop top>` at the bottom of the loop is the Thumb
 *     long-branch expansion (the body is ~380 bytes, past a conditional
 *     branch's reach), and the two `.pool_aligned` blocks with a `b` over them
 *     are pool dumps.  Neither is written in the C.
 *
 * -- worked in scratch_elev/b292/A
 */
struct Spr {
    unsigned char pad_00[5];
    unsigned char b0 : 5;
    unsigned char sel : 1;
    unsigned char pri : 2;
    unsigned char pad_06;
    unsigned char c0 : 6;
    unsigned char c6 : 2;
    unsigned short e : 10;
    unsigned short f : 2;
    unsigned short g : 4;
    unsigned char pad_0a[2];
};

extern int *iwram_3001f30;
extern void Func_8097384(void);
extern void vec3_translate(int a, int b, int *v);
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void _Actor_SetSpriteFlags(unsigned char *a, int n);
extern void _Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern unsigned int Random(void);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern unsigned char L9f0b4[] __asm__(".L9f0b4");

void Func_8098698(void)
{
    int v[3];
    int *o;
    int *t;
    unsigned char *p;
    struct Spr *q;
    int m;
    int z;
    struct Spr *d;

    o = iwram_3001f30;
    t = (int *)o[4];
    Func_8097384();
    for (m = 0; m < 24; m++) {
        if (o[0] == (0x80 << 7)) {
            v[0] = t[2];
            v[1] = t[3] + (0xa0 << 12);
            v[2] = t[4];
        } else if (o[0] == (0xc0 << 8)) {
            v[0] = t[2];
            v[1] = t[3] + (0xc0 << 13);
            v[2] = t[4];
        } else {
            v[0] = t[2];
            v[1] = t[3] + (0xa0 << 12);
            v[2] = t[4];
            vec3_translate(0xa0 << 12, o[0], v);
        }
        p = CreateParticleActor(0x8e << 1, v[0], v[1], v[2]);
        q = *(struct Spr **)(p + 0x50);
        d = q + 1;
        d->sel = q->sel;
        d->pri = q->pri;
        d->c6 = q->c6;
        d->e = q->e;
        d->g = q->g;
        if (p != 0) {
            *(int *)(p + 0x1c) = 0xb333;
            *(int *)(p + 0x18) = 0xb333;
            *(int *)(p + 0x34) = 0xc0 << 9;
            *(int *)(p + 0x30) = 0xc0 << 9;
            z = 0;
            *(char *)(p + 0x55) = z;
            _Actor_SetColorswap(p, 0xb);
            _Actor_SetAnim(p, 7);
            _Actor_SetScript(p, L9f0b4);
            _Actor_SetSpriteFlags(p, 1);
            v[0] = o[1];
            v[1] = o[2];
            v[2] = o[3];
            if (o[0] == (0xc0 << 8))
                vec3_translate(0xe0 << 12, o[0], v);
            vec3_translate(Random() * 6 + (0x80 << 11), Random(), v);
            _Actor_TravelTo(p, v[0], v[1], v[2]);
        }
        _PlaySound(0x83);
        WaitFrames(2);
    }
    WaitFrames(8);
}
