/* Cluster OvlFunc_944_20090a0..OvlFunc_944_20090a0 extracted from goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c.s.
 *
 * Total .text for this TU = 144 bytes (= 0x90).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_c_a.o and asm/overlays/rom_7ca63c/ovl_30_c_c_b.o in
 * goldensun/overlays/rom_7ca63c/overlay.ld.
 *
 * Never attempted before batch 272. No pins, no flags.
 *
 * A camera shake: advance two phase accumulators by random increments and add
 * cos/sin of them to the camera offset. The four `.L` cells are 4-byte .lcomm BSS
 * slots already `.global`-ised in asm/overlays/rom_7ca63c/ovl_30_c_c_c_b.s:69-83, so
 * no asm change was needed.
 *
 * THE LEVER IS A NAMED MASK LOCAL PLACED BEFORE THE STORE IT COMPETES WITH, and it
 * works by shortening a live range at the TOP rather than the bottom.
 *
 * The residue was the last eleven instructions. The ROM keeps 0xffff in r1 and the
 * truncated L1940 in r2, with the `ldrh` and its `str` four instructions apart.
 * Every natural spelling gave those two values the SAME register or the registers
 * SWAPPED, because the two pseudos either never overlapped or overlapped the wrong
 * way round.
 *
 * ARM's REG_ALLOC_ORDER hands out r3, then r2, then r1, so the FIRST-allocated of
 * two tied 2-reference quantities takes r2 and the second takes r1; and local-alloc's
 * priority with n_refs == 2 for both reduces to SHORTER LIVE RANGE WINS. Written
 * naturally, 0xffff is born after the ldrh/str pair (a constant load is emitted with
 * its `and`), so the two never overlap and both get r2. Naming the mask as its own
 * statement BEFORE the ldrh statement puts its `mov` first in the RTL, so 0xffff is
 * born earlier and dies later, the ldrh quantity becomes the shorter one, is
 * allocated first and takes r2, and 0xffff falls to r1.
 *
 * Both `m` and `v` are load-bearing: with `m` as a literal it is 7 differing, with
 * `v` folded away it is 8.
 *
 * MEASURED AND INERT, all at 8: a union on L1940, a union on both cells,
 * -fno-schedule-insns, -fno-strict-aliasing, -fno-rerun-cse-after-loop, -fno-gcse,
 * and a `register int w __asm__("r2")` pin on the truncated value -- the pin does NOT
 * help, which is worth recording. -fno-schedule-insns2 is WORSE at 24: sched2
 * produces the correct part here.
 *
 * WORTH KNOWING: -fno-schedule-insns produced no .16 dump at all, so sched1 does not
 * run for Thumb in this configuration and pre-sched2 RTL order IS expansion order.
 * That is what makes a source-statement-order lever deterministic here rather than a
 * guess.
 */
extern int **iwram_3001e70;
extern int L1920 __asm__(".L1920");
extern int L1924 __asm__(".L1924");
extern int L1928 __asm__(".L1928");
extern int L1940 __asm__(".L1940");
extern int __cos(int a);
extern int __sin(int a);
extern unsigned int __Random(void);
void OvlFunc_944_20090a0(void)
{
    int *p;
    int c;
    int s;
    int v;
    int m;
    p = *iwram_3001e70;
    c = __cos(L1940);
    s = __sin(L1928);
    *p += c;
    p++;
    *p += s * 4;
    L1924 += c;
    L1920 += s * 4;
    L1940 += (unsigned short)(__Random() * 3 >> 9);
    v = L1928 + (unsigned short)(__Random() >> 7);
    m = 0xffff;
    L1940 = *(unsigned short *)&L1940;
    L1928 = v & m;
}
