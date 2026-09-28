/* Func_8012b2c (0x08012b2c) -- NON-MATCHING: 220 encodings of 239 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/8012b2c.c asm/rom_9000/rom_1219c_a_c_c_c.s --func Func_8012b2c
 *
 * SIZE 480 vs the ROM's 500 and 228 instructions vs 239, so 220 is NOT a
 * distance: ELEVEN INSTRUCTIONS ARE MISSING, and twelve of them are accounted
 * for below.  tryc --full --align reports 153 lines dirty of 250, which is the
 * honest figure.  RELOCATIONS ARE IDENTICAL IN SET AND ORDER: all six
 * vec3_translate calls and the three data words land in the ROM's order, so the
 * control flow and the switch dispatch are right.
 *
 * The .s holds this function alone with no data, so it converts whole.
 *
 * ================== THE BLOCKER: THREE EMPTY LOOPS gcc-2.96 DELETES ==========
 * Three of the five switch bodies end with a loop that does NOTHING but count:
 *
 *   case 3       mov r4,#6  / .L: add r4,#1 / cmp r4,#9 / ble .L
 *   case 5,8,..  mov r4,#8  / .L: add r4,#1 / cmp r4,#9 / ble .L
 *   default      mov r4,#5  / .L: sub r4,#1 / cmp r4,#0 / bge .L
 *
 * 12 instructions, and OUR gcc-2.96 removes every one of them.  Probed
 * separately in scratch_elev/b292/F/emptyloop.c, five spellings at the
 * production flags, and none survives:
 *
 *   i = 6; do { i++; } while (i <= 9);           deleted outright
 *   for (i = 7; i <= 9; i++) ;                   deleted outright
 *   ... while (i <= 9); f(i);   (counter LIVE)   folded to `mov r0,#10`
 *   volatile int i                               kept, but as `str #10,[sp]`,
 *                                                one store, no loop
 *   i++ plus `__asm__ ("")` in the body          loop UNROLLED to 4 empty asms
 *
 * So the loop bodies were NOT empty in the original source -- they compiled to
 * nothing while leaving the loop, which is what a store to a DEAD LOCAL does
 * before dead-store elimination and after gcc-2.96 has given up on removing the
 * loop around it.  This function's frame is only `sub sp, #4` (the counter
 * spill), so that dead local is not in ITS frame -- but note that this batch's
 * other two rom_9000 targets BOTH carry unexplained frame slack for locals
 * nothing reads (Func_800c880 24 bytes, ActorCmd_Player_Climb 68), so the three
 * belong together as one question about this bank's source.
 *
 * Blocker class: SOURCE SHAPE, not codegen.  Pass: gcc-2.96's own loop/flow dead
 * code removal, which the original build did not reach because the body was not
 * yet empty when it ran.
 *
 * ================== WHAT IS SOLVED (keep all of it) ==================
 * 1. SWITCH BODY ORDER, by the ROM's LABEL ADDRESSES: .L12b8a (3), .L12bca
 *    (5/8/0x2c/0x58), .L12c0c (4/6), .L12c44 (0x14), .L12cae (default).  Written
 *    numerically the tree comes out wrong.
 * 2. CASE 5 IS REAL.  `cmp r4,#4 / bhi .L12bca` fires only for values above 4
 *    and below 6, and 6 was already taken -- so 5 has its own label, grouped
 *    with 8 / 0x2c / 0x58.  Omitting it gives a different tree.
 * 3. BLOCK-SCOPING X, Z, mag and zero INSIDE each case is the one real lever
 *    found here: 244 -> 228 instructions, 229 -> 220, and it is what makes the
 *    whole seven-way dispatch tree come out exactly (including the ROM's
 *    UNSIGNED `bhi`, which `unsigned int kind` alone does NOT fix -- measured
 *    inert, 229 either way).  As five per-case allocnos each pair has a short
 *    live range instead of one range spanning the switch.
 * 4. The loops are do/while with the bump at the END and the counter spilled to
 *    [sp] across vec3_translate; the angle is `(unsigned short)ang` at the call
 *    (`lsl #16 / lsr #16`) and `ang = (short)(ang + K)` after it
 *    (`add / lsl #16 / asr #16`).
 * 5. `ang = 0` then `t = iwram_3001800 << 24; ang = t >> 16;` in the guarded
 *    block gives the ROM's `lsl r3,#24 / asr r5,r3,#16` with no cast.
 * 6. The 0x14 case really does keep TWO pointers over the same memory (q for the
 *    stores, p for the two call arguments), both stepping 0x20, plus a
 *    loop-invariant zero in a callee-saved register for the `y` stores.
 *
 * ================== THE RESIDUE BEYOND THE EMPTY LOOPS ==================
 * `ang` lands in r8 where the ROM has r5, which costs 2 instructions at its
 * initialisation and 1 at each of its ~7 updates; X and Z then rotate from the
 * ROM's r8/r6 to our r6/r5.  Read out of the .18.greg dump: the allocation order
 * is `37 36 38 41 39 ...` and `ang` is pseudo 39, FIFTH, so 37 and 36 take r6/r7
 * ahead of it; the ROM's order has ang first (r5), Z second (r6), p third (r7),
 * X fourth (r8).  Measured and inert or worse, all single changes:
 *   short ang (220, inert) / ternary initialiser (227 of 230) /
 *   ang declared first (compile order only, inert) /
 *   ang assigned before the kind read (223) / `unsigned int kind` (inert).
 *
 * NEXT: the empty loops first -- they are 12 of the 11 missing instructions and
 * nothing else can be scored honestly until they are there.  Then price ang
 * against pseudos 36 and 37 with allocno_compare (floor_log2(n_refs)*n_refs /
 * live_length, global.c).
 *
 * NO SHIMS in this draft: no pins, no barriers, no volatile, no .equ, no .sym.
 */
extern unsigned char *iwram_3001e60;
extern unsigned int gKeyHeld;
extern int iwram_3001800;
extern void vec3_translate(int mag, int ang, int *v);

void Func_8012b2c(int x, int z, int *r2)
{
    unsigned char *rec;
    int *p;
    int *q;
    unsigned int kind;
    int ang;
    int t;
    int i;

    p = r2;
    rec = *(unsigned char **)(iwram_3001e60 + 0x28);
    kind = rec[4];
    ang = 0;
    if ((gKeyHeld & 2) != 0) {
        t = iwram_3001800 << 24;
        ang = t >> 16;
    }
    switch (kind) {
    case 3:
      {
        int X = x << 16;
        int Z = z << 16;
        i = 5;
        do {
            p[0] = X;
            p[1] = 0;
            p[2] = Z;
            vec3_translate(0xe0 << 14, (unsigned short)ang, p);
            ang = (short)(ang + 0x2aaa);
            i--;
            p += 4;
        } while (i >= 0);
        i = 6;
        do {
            i++;
        } while (i <= 9);
        break;
      }
    case 5:
    case 8:
    case 0x2c:
    case 0x58:
      {
        int X = x << 16;
        int Z = z << 16;
        i = 7;
        do {
            p[0] = X;
            p[1] = 0;
            p[2] = Z;
            vec3_translate(0xe0 << 14, (unsigned short)ang, p);
            ang = (short)(ang + (0x80 << 6));
            i--;
            p += 4;
        } while (i >= 0);
        i = 8;
        do {
            i++;
        } while (i <= 9);
        break;
      }
    case 4:
    case 6:
      {
        int X = x << 16;
        int Z = z << 16;
        i = 0;
        do {
            p[0] = X;
            p[1] = 0;
            p[2] = Z;
            vec3_translate(0xe0 << 14, (unsigned short)ang, p);
            ang = (short)(ang + 0x1999);
            i++;
            p += 4;
        } while (i <= 9);
        break;
      }
    case 0x14:
        ang = (short)(ang + (0x80 << 7));
      {
        int X = x << 16;
        int Z = z << 16;
        int mag = 0xa0 << 14;
        int zero = 0;
        q = p;
        i = 0;
        do {
            q[0] = X;
            q[1] = zero;
            q[2] = Z;
            vec3_translate(mag, (unsigned short)ang, p);
            q[4] = X;
            q[5] = zero;
            q[6] = Z;
            vec3_translate(mag, (unsigned short)ang, p + 4);
            ang = (short)(ang + (0x80 << 8));
            i++;
            q += 8;
            p += 8;
        } while (i <= 1);
        break;
      }
    default:
      {
        int X;
        int Z;
        ang = (short)(ang + (0x80 << 6));
        X = x << 16;
        Z = z << 16;
        i = 3;
        do {
            p[0] = X;
            p[1] = 0;
            p[2] = Z;
            vec3_translate(0xe0 << 14, (unsigned short)ang, p);
            ang = (short)(ang + (0x80 << 7));
            i--;
            p += 4;
        } while (i >= 0);
        i = 5;
        do {
            i--;
        } while (i >= 0);
        break;
      }
    }
}
