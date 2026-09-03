/* Func_8098294  --  0x08098294
 *
 * Cut from the middle of goldensun/asm/rom_8a000/rom_97b54_a_c_a_a_a_c_c.s;
 * the function ahead of it stays in _a.s and the two after it in _c.s. No data
 * in the file, so the split is a pure text cut and was verified byte-neutral
 * before this landed.
 *
 * Sweeps all 64 slots of the actor table at stride 0x70 and, for every live
 * slot whose sub-record carries the marker value, writes the caller's byte into
 * the record and sets the slot's own flag.
 *
 * MATCHED ON THE FIRST CANDIDATE, and it is worth saying why rather than
 * treating that as luck: this function only became visible after the selection
 * filter was recalibrated. The old rule rejected anything making fewer than
 * eight calls, and this makes NONE -- yet 85% of everything this project has
 * already matched would have been rejected on the same test. It is 33
 * instructions with no calls and no repeated constant, which is the profile the
 * corpus says converges, and it did so immediately.
 *
 * Two details that fell out of the plain reading rather than a lever:
 *
 * THE STORED FLAG IS THE VARIABLE, NOT THE LITERAL. The ROM stores the same
 * register it compared against 1 (`strb r4, [r3]` where r4 is the byte just
 * tested), so the source writes the tested variable back rather than a fresh
 * `1`. Writing the literal would have materialised a second constant.
 *
 * THE ldrsh INDEX REGISTER IS THE PATTERN'S OWN CLOBBER. The `mov r6, #0 /
 * ldrsh r3, [r2, r6]` pair is the sign-extending load's mandatory index, chosen
 * by reload -- not a user pseudo, and not reachable by naming an offset. That
 * is the third independent confirmation this round that the [offset] question
 * does not apply to ldrsh at all.
 *
 * The base pointer lands in r12 and the tested byte in r4 with no push for
 * either, which is free here because the function contains no calls.
 */
extern unsigned char *iwram_3001e64;

void Func_8098294(int v)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    int i;
    int k;

    p = iwram_3001e64;
    i = 0x3f;
    do {
        if (*(int *)p != 0) {
            k = p[0x54];
            if (k == 1) {
                q = *(unsigned char **)(p + 0x50);
                r = *(unsigned char **)(q + 0x28);
                if (*(short *)r == 0xc8) {
                    r[5] = v;
                    q[0x25] = k;
                }
            }
        }
        i--;
        p += 0x70;
    } while (i >= 0);
}
