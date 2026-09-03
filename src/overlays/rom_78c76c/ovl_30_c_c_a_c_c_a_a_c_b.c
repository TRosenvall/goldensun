/* OvlFunc_891_20094b8  --  0x020094b8
 *
 * The tail of goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c.s; the
 * function ahead of it stays in _a.s and its pool with it. No data anywhere in
 * the file, so the split is a pure text cut, verified byte-neutral first.
 *
 * Decompresses one sprite sheet and uploads it as three separate sprite slots,
 * writing nine three-word OAM-ish entries per slot through a running pointer.
 *
 * THE EXTRA REGISTER WAS A MISSING RETURN VALUE, NOT AN ALLOCATION PROBLEM --
 * and my own first diagnosis of this function was wrong. I read the third pushed
 * register as the loop's zero wanting a caller-saved home. It was the sprite
 * slot id. The ROM does `bl AllocSpriteSlot / ... / bl UploadSpriteGFX / ... /
 * orr r0, r3`, ORing INTO THE ARGUMENT REGISTER AFTER the call. Written as
 * `o = AllocSpriteSlot(); UploadSpriteGFX(o, ...); o |= K;` the value is live
 * ACROSS the second call, so it must take a call-saved register -- a third
 * push, a copy per block, and everything else rotated. Written as
 * `o = UploadSpriteGFX(AllocSpriteSlot(), ...); o |= K;` it is the callee's
 * RETURN and stays in r0 for free. That one change went 43 differing to 17 and
 * fixed the push list, the rotation and the length together.
 *
 * THE TELL: an `orr`/`and`/`add rN, ...` on the ARGUMENT register immediately
 * after a `bl` says the operand is that call's return value, not something the
 * caller kept alive. Any candidate pushing one register too many at a call site
 * with this shape should be re-read that way before any allocation lever.
 *
 * A CONSTANT THE ROM RE-MATERIALISES PER BLOCK NEEDS A DISTINCT VARIABLE PER
 * BLOCK -- which scopes a note recorded earlier. That note says two locals
 * holding the same constant fold back to one const_int before allocation, so
 * splitting cannot separate them. Read from the dumps, that is true of the GCSE
 * hoist and false of the fold that was actually blocking here: cse2 rewrites the
 * second and third blocks' `mov rN, #0` into a COPY of the loop counter's zero
 * and leaves the first block alone -- path-dependent, not uniform. Distinct
 * `z1/z2/z3` stop it and match at plain -O2. So "same constant, distinct
 * variables" DOES separate a cse2 copy and does NOT separate a gcse hoist; the
 * existing note wants scoping to gcse.
 *
 * The flag route existed and was not taken: -fno-gcse and
 * -fno-rerun-cse-after-loop each make the single-variable spelling exact, while
 * four other cse flags do nothing. The per-block variable is the better answer
 * because it needs no Makefile rule.
 *
 * `bls` MEANT AN UNSIGNED COUNTER, and that alone denied the loop reversal. A
 * signed `int i` gives gcc's reversed countdown; `unsigned int i` with `i <= 8`
 * blocks the dbra rewrite and produces the ROM's increment-and-compare. 17 to 8.
 * READ THE BRANCH CONDITION CODE AS A SIGNEDNESS STATEMENT about the counter
 * before reaching for a goto-written loop.
 *
 * RETURN TYPE, NOT PROTOTYPE, IS THE DECLARATION LEVER HERE. With the prototype
 * held constant, `void` against `int` on the task-start callee flips the
 * argument fill order by itself; and the undeclared form the sibling family uses
 * is unnecessary -- a full `extern int` prototype is exact. That is a cleaner
 * separation of the two declaration levers than the corpus records: it is not
 * "drop the declaration", it is "the callee returns int".
 *
 * The OR's statement position is a one-slot-at-a-time lever, and sweeping it is
 * what found the landing spot.
 */
extern int L2a50[] __asm__(".L2a50");
extern unsigned char L256c[] __asm__(".L256c");

extern void *__galloc_ewram(int tag, int n);
extern void __DecompressLZ1(unsigned char *src, void *dst);
extern int __AllocSpriteSlot(void);
extern int __UploadSpriteGFX(int slot, int size, void *src);
extern void __gfree(int tag);
extern int __StartTask(void (*fn)(void), int prio);
extern void OvlFunc_891_2008eb0(void);

void OvlFunc_891_20094b8(void)
{
    int *p;
    unsigned char *buf;
    int o;
    unsigned int i;
    int *q;
    int z1, z2, z3;

    p = L2a50;
    buf = __galloc_ewram(0xe, 0x80 << 3);
    __DecompressLZ1(L256c, buf);
    o = __UploadSpriteGFX(__AllocSpriteSlot(), 0x80, buf);
    i = 0;
    z1 = 0;
    o |= 0xac << 8;
    do {
        q = p;
        *q++ = z1;
        *q++ = 0x40004000;
        i++;
        p += 3;
        *q = o;
    } while (i <= 8);
    o = __UploadSpriteGFX(__AllocSpriteSlot(), 0x80, buf + 0x80);
    i = 0;
    z2 = 0;
    o |= 0xdc << 8;
    do {
        q = p;
        *q++ = z2;
        *q++ = 0x40004000;
        i++;
        p += 3;
        *q = o;
    } while (i <= 8);
    o = __UploadSpriteGFX(__AllocSpriteSlot(), 0x80, buf + (0x80 << 1));
    i = 0;
    z3 = 0;
    o |= 0xc0 << 4;
    do {
        q = p;
        *q++ = z3;
        *q++ = 0x40004000;
        i++;
        p += 3;
        *q = o;
    } while (i <= 8);
    __gfree(0xe);
    __StartTask(OvlFunc_891_2008eb0, 0xc8 << 4);
}
