// fakematch
/* OvlFunc_959_2009150 -- EXACT under the tree's PRODUCTION flags, no Makefile row.
 * 400 bytes, 175 encodings and 21 relocations identical; --whole also OK.
 *
 * Supersedes the park's "173 encodings of 175 / byte-exact only under
 * -fno-rerun-cse-after-loop".  ONE shim replaces the flag: a hard-register pin on
 * the FIRST (dominating) use of the flag id 0x214.  Its one-actor twin
 * OvlFunc_959_2009528 takes the identical shim and is also exact under production
 * flags, so the proposed CSE_CFLAGS pair for overlay rom_7e7574 is no longer needed.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.s --whole
 *
 * SHIMS: one `register ... __asm__` declaration (`register int k1 __asm__("r0")`).
 * Zero `__asm__(".equ ...)` lines.  Needs a fakematch.txt row:
 *   OvlFunc_959_2009150  src/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.c
 *
 * THE MECHANISM, corrected.  The park (and the brief) said cse2 commons the id
 * because `after_loop = 1` turns on path extension through flag_cse_follow_jumps /
 * flag_cse_skip_blocks, and that cse1 stops at the conditional branch between the
 * two uses.  Both halves are wrong, and both were measured:
 *
 *   -fno-cse-follow-jumps                            173 of 175  (inert)
 *   -fno-cse-skip-blocks                             173 of 175  (inert)
 *   both together                                    173 of 175  (inert)
 *
 * cse_end_of_basic_block scans `while (p && GET_CODE (p) != CODE_LABEL)`, so a cse
 * basic block runs THROUGH a conditional branch along the fall-through and ends only
 * at a CODE_LABEL.  There is no CODE_LABEL between the two uses -- the ROM's own
 * output proves it, `.L11c8` is before the first and `.L120a` after the second.  So
 * no path extension is involved, and cse1 commons the pair too.  The real chain, read
 * off the -da dumps (count of `(set (reg) (const_int 532))` in the insn stream):
 *
 *   .00.rtl   5   one per use (argument precompute, calls.c:855, cost 6 > 2)
 *   .03.cse   4   CSE1 ALREADY COMMONS uses 1 and 2
 *   .07.gcse  5   GCSE'S CPROP RESTORES THE LITERAL -- this is the ROM's form
 *   .08.loop  5   unchanged
 *   .09.cse2  4   cse2 re-commons it; the whole 40-instruction diff is this one
 *                 substitution (the cse2 dump diff is exactly one line)
 *
 * So -fno-rerun-cse-after-loop works by letting gcse's cprop result stand, not by
 * suppressing path extension.  That also explains why per-use named locals are
 * exactly inert: they all expand to the same five sets.
 *
 * WHY NO PURE-SOURCE ROUTE EXISTS.  cse2 substitutes because the class holding 532
 * contains a live pseudo (COST 0) and the constant costs 6.  Only three things stop
 * it and none is spellable here:
 *   1. a CODE_LABEL between the uses -- the ROM has none, and manufacturing one needs
 *      a jump the ROM does not have;
 *   2. the pseudo being re-SET between the uses -- gcc never re-sets a pseudo;
 *   3. the value living in a CALL-CLOBBERED HARD REG, so invalidate_for_call drops it.
 * (3) is the shim, and it is the minimum: `register int k1 __asm__("r0")` means no
 * pseudo is ever formed, cse's table holds only r0, and the `bl __GetFlag` between
 * the two uses invalidates it.  Four spellings all reach exact, so the pin is the
 * smallest of them:
 *   register int k1 __asm__("r0") = 0x214;                       EXACT  (this file)
 *   int k1 = 0x214; __asm__ ("" : "+r" (k1));                     EXACT
 *   int k1 = 0x214; __asm__ __volatile__ ("" : "+r" (k1));        EXACT
 *   pin + __asm__ __volatile__ ("" : : "r" (k1));                 EXACT
 * The bare pin with no barrier is what docs/elevation.md's "try the bare pin before
 * the barrier" rule predicts for a mov+lsl (non-pooled) constant, and it holds.
 *
 * Everything else in the function is the park's work, unchanged: the three-arm gState
 * chain with a per-arm `g`, the shared pointer / per-site value locals for the
 * cross-jumped `strh`, and the soft-float trio.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
};

struct S {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    int f20;
    int f24;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern volatile int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern int OvlFunc_959_2009108(void);
extern int OvlFunc_959_2009918(int slot);
extern unsigned int OvlFunc_959_20098e4(unsigned int slot);
extern long long OvlFunc_common2_304(int x);
extern long long OvlFunc_common2_28c(long long a, long long b);
extern int OvlFunc_common2_380(long long v);

void OvlFunc_959_2009150(void)
{
    struct Actor *a;
    struct Actor *c;
    struct S *s;
    unsigned char *b;
    unsigned char *g;
    unsigned short *p;

    a = __MapActor_GetActor(9);
    c = __MapActor_GetActor(0xa);
    s = (struct S *)(iwram_3001e70[0] + (0xb2 << 1));
    b = iwram_3001e70[0x13];
    if (iwram_3001e40 & 1) {
        s->f18 = 1;
        s->f1c = 1;
    } else {
        s->f18 = -1;
        s->f1c = -1;
    }
    if (__GetFlag(0x83 << 1) != 0 || *(short *)(b + (0xbf << 1)) != 0
        || *(short *)(b + (0xc0 << 1)) != 0) {
        a->f5b = 1;
        c->f5b = 1;
        return;
    }
    {
        register int k1 __asm__("r0") = 0x85 << 2;
        if (__GetFlag(k1) != 0)
            return;
    }
    a->f5b = 0;
    c->f5b = 0;
    if (__GetFlag(0x85 << 2) == 0 && a->f5b == 0)
        s->f20 = OvlFunc_common2_380(OvlFunc_common2_28c(0x41610000, OvlFunc_common2_304(a->f8)));
    if (OvlFunc_959_2009108() != 0)
        return;
    g = gState;
    if (*(short *)(g + (0x93 << 2)) != 0) {
        if (OvlFunc_959_2009918(9) != 0 && *(short *)(g + (0x93 << 2)) != 0) {
            int va;
            p = (unsigned short *)(b + (0xbf << 1));
            va = 0x2092;
            *p = va;
            return;
        }
        if (OvlFunc_959_2009918(0xa) != 0) {
            unsigned char *g2 = gState;
            int vb;
            if (*(short *)(g2 + (0x93 << 2)) == 0)
                goto rest;
            p = (unsigned short *)(b + (0xbf << 1));
            vb = 0x2092;
            *p = vb;
            return;
        }
        {
            unsigned char *g3 = gState;
            if (*(short *)(g3 + (0x93 << 2)) != 0)
                goto check;
        }
    }
rest:
    if (OvlFunc_959_20098e4(9) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
    if (OvlFunc_959_20098e4(0xa) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
check:
    if (__GetFlag(0x85 << 2) != 0) {
        int vc;
        p = (unsigned short *)(b + (0xc1 << 1));
        vc = 0x5b;
        *p = vc;
    }
}
