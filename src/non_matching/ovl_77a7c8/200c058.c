/* OvlFunc_881_200c058  [overlays/rom_77a7c8]
 *
 * NON-MATCHING, 21 of 133 encodings  (batch 347 brief C; was 111 of 133).
 *   *** THE FIGURE IS NOT A DISTANCE, AND IT IS SHORT, NOT LONG. ***
 *   122 instructions against the ROM's 123, with the STREAM equal at 133 and
 *   the SIZE equal -- a pad absorbs the missing instruction, which is exactly
 *   the shape that made the old header wrong.  That header said "COUNT DIFFERS
 *   (ref 133, ours 137) ... the figure IS a distance" and "SIZE ref 284 bytes,
 *   ours 292" in the same breath; the old body was 125 instructions, +2.
 *   objcmp prints its own NOTE about the pad.  Believe the NOTE.
 *   RELOCATIONS: same seven symbols; offsets now match the ROM's.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200c058.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s --func OvlFunc_881_200c058
 *
 * ALIGNED RESIDUE (tools/aligncmp.py): 33 in 15 hunks -> 22 in 10 hunks.
 * 0 devices (tools/shimcount.py).
 *
 * *** SPLIT SHAPE -- THIS PIECE EXPORTS DATA, SO A MATCH CANNOT BE CONVERTED
 * WHOLE. *** tools/piece.py reports
 *   !! EXPORTS 43 NON-FUNCTION SYMBOL(S): .L44ac, .L47a6, .L5b84, .L604c,
 *      .L6154, .L61e4 ...
 * on asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s, whose only function is this
 * one.  Converting the piece deletes those 43 labels; a gate went RED on
 * exactly this in batch 345.  A landing must go through tools/split_s.py to cut
 * code from data (run its --dry-run and read the shape -- with 43 symbols the
 * export list is the part to check).  Recorded here, not acted on: the manifest
 * `split` field is for landings only and install_batch.py refuses one on a park.
 *
 * WHAT CLOSED THE +2.  It was TWO SITES OF +1, not one site of +2, and only
 * per-region counting separated them.  Both are failures to REUSE a pointer.
 *
 *   SITE 1, in the loop: `a->f55 = 0; a->f64 = 0;`.  The ROM walks ONE
 *   register -- `mov r3,r0 / add r3,#0x55 / strb r2,[r3] / add r3,#0xf /
 *   strh r2,[r3]`, three address instructions for two stores.  Written as two
 *   plain member stores gcc builds TWO addresses off `a` (four).  Spelling the
 *   two fields as members of one sub-struct BASED AT a+0x54, so that both
 *   stores fold into displacements off a single pointer, costs TWO address
 *   instructions -- which overshoots by one but is the shape that produced
 *   nearly everything else.  THE OFFSETS ARE THE WHOLE POINT: base a+0x54 is
 *   EVEN, so `f1` at +1 is a+0x55 and `f10` at +0x10 is a+0x64.  Basing at
 *   a+0x55 the way the ROM's register does is NOT expressible -- a `short` at
 *   an odd struct offset is padded to the next even one, and the first version
 *   of this (base a+0x55, halfword at a nominal +0xf) silently wrote a+0x65.
 *   It compiled, it scored 21, and it was A WRONG PROGRAM.  That is the
 *   sweep-score caveat from the shared brief, caught by hand.
 *
 *   SITE 2, in the tail: `a->f23 = 2;` as the last use of the second actor.
 *   The ROM does `add r1,#0x23 / strb r3,[r1]` -- it converts the pointer
 *   register in place because the pointer dies there.  The fix is TWO SEPARATE
 *   LOCALS for arr[0] and arr[1] instead of one reused `a`: with one shared
 *   local the pseudo spans both blocks and regmove will not tie the `plus` to
 *   it (Thumb `add rd,#imm8` is two-address for imm > 7, so an untied operand
 *   costs a `mov`).  This is the shared brief's "one shared local across two
 *   sites makes both global -- the cure is TWO spellings at once"
 *   (local-alloc.c:1684-1687, :1344-1348, :2010-2016), and the ROM confirms it
 *   from the other end: it holds arr[0] in r2 and arr[1] in r1, two registers.
 *
 * WHAT IS LEFT: ONE INSTRUCTION, and it is known.  Site 1 wants the ROM's
 * three address instructions and this body emits two.  The only spelling that
 * reproduces the ROM's `add r3,#0xf` exactly is an explicit walking pointer
 *     t = (unsigned char *)a + 0x55;  *t = 0;  t += 0xf;
 *     *(unsigned short *)t = 0;
 * -- measured, and it DOES emit `adds r2,#0x55 / strb / adds r2,#15 / strh`.
 * It costs one elsewhere: the extra two-address local flips the loop counter
 * into r8 and the pooled zero into r7, inverting the ROM's roles, which is
 * `movs r3,#1 / add r8,r3 / mov r1,r8 / cmp r1,#1` against the ROM's
 * `add r7,#1 / cmp r7,#1`.  Net 124 against 123, aligned residue far worse
 * (ndiff 121).  So the remaining work is to keep the walk and keep i in a low
 * register.
 *
 * MEASURED (instructions / objcmp stream ndiff of 133):
 *   two tail locals only                             124 / 109
 *   + sub-struct at a+0x54 (this body)               122 /  21
 *   + sub-struct reached as &a->pad54                122 /  21 (identical)
 *   + halfword via an odd cast off the sub-struct     122 /  21 (identical)
 *   two tail locals + walking `t` with `t += 0xf`     124 / 121
 *   two tail locals + `*(u16 *)(t + 0xf) = 0`        125 / 121  (cse re-derives
 *                   the second address from `a`, so the chain does not survive)
 *   two tail locals + `t = &a->f55` walk             124 / 121
 *   two tail locals + a one-byte struct, odd cast    125 / 121
 *   tail locals + `s = aN->f50` hoisted in the tail  124 / 109
 *   walking `t` crossed with: t declared first, last, i declared first,
 *     a named zero, s loaded before the f14 store, do/while loop
 *                                                ALL 124 / 121, no movement
 *
 * BOTH CHEAP AXES WERE CLOSED ON THE OLD BODY (tools/autosweep.py, batch 347):
 *   flag axis -- eight groups, all inert or worse; -O1 108/133 and
 *   -fno-schedule-insns2 103/133 both RAISE the aligned residue (33 -> 96,
 *   33 -> 64).  anchor axis -- 26 of 26, nothing better than base.
 *   Both need re-running against THIS body.
 *
 * STILL TRUE FROM THE OLD HEADER, re-checked: the two-iteration loop, the
 * stack `struct Actor *arr[2]` addressed as `arr[i]`, all seven bitfield
 * writes, and THE ROM'S POOLED ZERO, which gcc emits unaided -- no const.sym
 * entry needed.  dupfuncs' twin OvlFunc_883_200dd68 (ovl_780898/200dd68.c) is
 * now the WORSE body of the pair; port in this direction, not that one.
 * The reference keeps two literal pools inside the function body, so even at
 * OK this needs `make compare` as the authority rather than a tryc screen.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

/* The ROM reaches a->f55 and a->f64 through ONE pointer.  Based at a+0x54 so
 * that f1 is a+0x55 and f10 is a+0x64; see SITE 1 above for why the base
 * cannot be the odd a+0x55 that the ROM's register actually holds. */
struct Pair {
    unsigned char p0;
    unsigned char f1;
    unsigned char pad2[0xe];
    unsigned short f10;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Cfg {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
};

struct Actor {
    unsigned char pad0[0x14];
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Cfg *f68;
    void *f6c;
};

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void OvlFunc_881_200c004(void);
extern void OvlFunc_881_200bfb4(void);

void OvlFunc_881_200c058(struct Cfg *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Actor *a0;
    struct Actor *a1;
    struct Spr *s;
    unsigned char *w;
    struct Pair *t;
    int i;

    w = iwram_3001f30;
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            t = (struct Pair *)((unsigned char *)a + 0x54);
            t->f1 = 0;
            t->f10 = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    a0 = arr[0];
    a0->f6c = OvlFunc_881_200c004;
    a0->f50->f8_b = 1;
    a1 = arr[1];
    a1->f6c = OvlFunc_881_200bfb4;
    a1->f50->f8_b = 1;
    a1->f23 = 2;
}
