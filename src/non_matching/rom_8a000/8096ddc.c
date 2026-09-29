/* Func_8096ddc -- NON-MATCHING, 11 encodings of 146 (objcmp: ENCODINGS differ in 11
 * place(s), ref 146 / ours 146).  LENGTH EXACT.  The .s (asm/rom_8a000/rom_96cdc_a_a_c_c.s)
 * holds only this function and tools/datacheck.py is silent on it -- whole-file, no split,
 * no exports beyond Func_8096ddc.  Two mid-function pools (a HImode 0 and a HImode
 * 0xfffffc00) both reproduce.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8096ddc.c asm/rom_8a000/rom_96cdc_a_a_c_c.s --func Func_8096ddc
 *
 * ===== BATCH 295: THE BLOCKER IS NOW FULLY LOCATED, AND IT IS CLOSED =====
 *
 * The residue is the r2/r3 swap between the walk pointer `q` and the shared SImode
 * zero, plus two sched2 ties that follow it.  Read out of .17.lreg / .18.greg:
 *
 *   reg 40  q          8 refs / 7 insns  in block 2, set 2 times, DIES IN 2 PLACES,
 *                      pref STACK_REG   -> GLOBAL allocno, gets r2
 *   reg 52  dead QI 0  2 refs / 2 insns, REG_UNUSED, pref LO_REGS -> LOCAL, gets r3
 *   reg 54  SImode 0   6 refs / 10 insns, pref LO_REGS            -> LOCAL, gets r3
 *
 * TWO GATES, BOTH AT local-alloc.c:362-368.  A pseudo is eligible for local-alloc
 * only if REG_BASIC_BLOCK >= 0 AND REG_N_DEATHS == 1 AND (reg_alternate_class ==
 * NO_REGS OR ! CLASS_LIKELY_SPILLED_P (reg_preferred_class)).  `q` fails BOTH the
 * death test (the `q += 0xf` chain sets it twice) and the class test.
 *
 * NEW AND REUSABLE -- THE STACK_REG GATE.  An address pseudo set by
 * `(set (reg) (plus (reg) (const_int N)))` and used as a memory base gets
 * `pref STACK_REG` when N is NOT a valid `add rd, sp, #imm` operand, and
 * `pref BASE_REGS` when it is.  Measured on a four-line isolate: o+0x55 STACK_REG,
 * o+0x54 BASE_REGS, o+0x64 BASE_REGS, o+0x65 STACK_REG -- it is the CONSTANT, not the
 * mode of the store (both a QImode and a HImode base show the same split).
 * reg_class_size[STACK_REG] == 1, so the default CLASS_LIKELY_SPILLED_P is TRUE and
 * local-alloc refuses the pseudo.  The ROM's offset is 0x55, so the walk pointer is
 * global by construction.  Controls: a one-death walk pointer at offset 0x55 is still
 * global (`pref STACK_REG`, 17 differ); the same spelling at offset 0x54 is LOCAL
 * (`pref BASE_REGS`) -- that one is a diagnostic only, it stores to the wrong byte.
 *
 * WHY THE SWAP IS UNREACHABLE FROM SOURCE.  For `q` (global) to get r3, no LOCAL
 * quantity may hold r3 anywhere in q's live range [q's set .. the strh].  But the
 * shared SImode zero is born INSIDE that range and is local, so it takes r3 unless
 * some other local already holds r3 there -- and any local holding r3 there conflicts
 * with q as well.  The two escapes are (a) make q local -- closed by the STACK_REG
 * gate above, or (b) make the SImode zero non-local.  (b) needs the zero to span two
 * blocks or die twice, and every source route to that also stops the zero being SHARED
 * between the strb and the strh, which is the thing that produces the ROM's single
 * `mov r2, #0`.  Measured: a plain `int z = 0` in the walk gives the strh its own
 * pooled HImode zero (22 differ, and loop.c hoists a loop-invariant int zero -- 148
 * encodings when declared in the loop); a BLKmode zero store after the `s == 0` branch
 * does NOT get cse'd together with the walk's zero (reg 54 stays block-local, 11).
 *
 * ISOLATING THE REST.  With `register unsigned char *q __asm__("r3")` -- a class-1
 * shim, recorded here as a DIAGNOSTIC, not a proposed landing -- the count is
 * 11 -> 6 at the same 146 encodings, and the whole remaining residue is two sched2
 * ties: (1) `ldr r5,[r0,#0x50]` belongs between `mov r2,#0` and the strb, ours puts it
 * after `ldr r1,=0x0` (4 encodings), and (2) `mov r1,#0x21 / neg r1,r1` belongs before
 * `strh r3,[r5,#8]`, ours puts it after (2 encodings).  So the register swap is worth 5
 * and the ties 6.  All SEVEN positions of `s = o->f50;` in the guarded block read 6
 * under the pin -- source order does not reach tie (1).
 *
 * NO FLAG ROUTE.  -fno-expensive-optimizations is the only production-flag candidate
 * that touches this (it controls regclass.c:1161's two-pass prefclass/altclass
 * computation, which is what could make reg_alternate_class NO_REGS and satisfy the
 * local-alloc gate).  Measured: --align 53 -> 94, i.e. much worse; -fno-schedule-insns2
 * 53 -> 97; the pair fails to compile.  Nothing reaches local-alloc's eligibility test.
 *
 * DELTA to the inert list (all at 146 encodings unless noted): a second address local
 * instead of the chain, `q2 = q + 0xf`, and `((struct H1 *)(q + 0xf))` / `&q[0xf]` all
 * 17 -- in every one of them the zero DOES move to r2 and the f64 address DOES get r3,
 * and only the f55 address is left in r1, which is the cleanest statement of the
 * blocker.  Also inert at 11: `struct B1 { unsigned int v : 8; }`, `{ unsigned char v : 8; }`,
 * `struct H1 { unsigned int v : 16; }`, `{ unsigned short v : 16; }`, all four
 * combinations, a `struct B1 *` walk pointer, and BLK-u8 + PLAIN-u16 (the plain
 * `*(unsigned short *)q = 0` still shares the SImode zero here -- same as the
 * 200a440 park's note).  Worse: plain-u8 + BLK-u16 23, `((struct H1 *)q)->v = zero` 13,
 * `((struct B1/H1 *)q)->v = zero` for both 144 encodings, the HI store written first
 * with `q -= 0xf` 86, a function-scope `int z` 24, BLKmode stores of 0 for BOTH
 * `s->f26` and `s->f28[0x16]` 12.
 */
struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06[1];
    unsigned char d0 : 6;
    unsigned char d6 : 2;
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Actor {
    void *f00;
    unsigned char pad04[2];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

struct B1 { unsigned char v; unsigned char pad[11]; };
struct H1 { unsigned short v; unsigned char pad[10]; };
struct SpriteSlot {
    unsigned short f0;
    unsigned short f2;
};

extern unsigned char *iwram_3001f30;
extern struct SpriteSlot gSpriteSlots[];
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Sprite_SetAnim(struct Sprite *s, int n);
extern void Func_8003f3c(int n);
extern void Func_8096d84(void);
extern void Func_8096d2c(void);

void Func_8096ddc(struct Actor *e)
{
    struct Actor *arr[2];
    struct Actor *c;
    struct Actor *o;
    struct Sprite *s;
    unsigned char *m;
    unsigned short zero;
    int i;
    unsigned char *q;

    m = iwram_3001f30;
    c = *(struct Actor **)(m + 0x10);
    for (i = 0; i <= 1; i++) {
        o = _CreateActor(0x1a, e->f08, e->f0c, e->f10);
        arr[i] = o;
        if (o == 0)
            continue;
        o->f14 = e->f14;
        s = o->f50;
        q = (unsigned char *)o + 0x55;
        ((struct B1 *)q)->v = 0;
        q += 0xf;
        ((struct H1 *)q)->v = 0;
        o->f68 = e;
        o->f1c = 0x1999;
        zero = 0;
        o->f18 = 0x1999;
        if (s == 0)
            continue;
        _Sprite_SetAnim(s, 0);
        s->f26 = zero;
        Func_8003f3c(s->f1c);
        s->f1c = *(unsigned short *)(m + 0x46);
        s->f1d |= 1;
        s->f08 = gSpriteSlots[s->f1c].f2 >> 5;
        s->c5 = 0;
        s->c6 = 1;
        s->d6 = 2;
        s->f28[0x16] = zero;
    }
    arr[0]->f6c = Func_8096d84;
    arr[0]->f50->b10 = 0;
    arr[1]->f6c = Func_8096d2c;
    arr[1]->f50->b10 = c->f50->b10;
    arr[1]->f23 = 2;
}
