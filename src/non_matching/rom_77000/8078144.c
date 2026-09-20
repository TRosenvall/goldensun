/* Func_8078144 -- 0x08078144, asm/rom_77000/rom_77320_a_c_c.s (3 functions).
 *
 * NOT MATCHING: 4 differing of 103 encodings, SIZE EXACT -- plus one BLOCKER that is a
 * build-input decision. Candidate below.
 *
 * ================ BLOCKER: A label.sym ENTRY ================
 *
 * The ROM has `ldr r3, =.L7a828`. `.L7a828` is an assembler-local label defined at
 * asm/rom_77000/rom_77320_c_c_c_b.s:13 and is unreachable from C. It needs
 *
 *     _TBL_7a828 = .L7a828;
 *
 * in label.sym, spelled `extern unsigned char L7a828[] __asm__("_TBL_7a828");`.
 *
 * VERIFIED, so nobody has to re-check: `.L7a828` IS already `.global` in that .s, which is
 * label.sym's stated bar for adding an entry. So the entry would link.
 *
 * AND A TRAP WORTH NAMING: aliases.txt:537 already has
 * `BYTE_ARRAY_0807a828 = 0x0807a828` -- but aliases.txt is NOT INCLUDEd by stage1.ld, so it
 * does not link and is not a substitute. Do not reach for it.
 *
 * NOT ADDED HERE because the entry would buy a NON-MATCH: four encodings still differ with it.
 * That is the same reasoning that kept _MSG_b24 out in batch 272 -- a build input is worth
 * adding when it COMPLETES a function, not when it improves one.
 *
 * ================ THE OTHER RESIDUE, AND IT TRADES ================
 *
 * Four encodings: `mov r2, #0x38` / `mov r3, #0x34` against ours reversed -- the two `ldrsh`
 * scratch/offset registers.
 *
 * THE TWO RESIDUES TRADE AGAINST EACH OTHER, which is the result worth recording. With the
 * loads in the ROM's source order the scratch registers are RIGHT but the
 * `strh [r5, #0x3a]` sinks below the first `ldrsh` -- 6 differing. With them swapped the store
 * stays put and the scratch registers swap -- 4 differing. From -fsched-verbose=6: the store
 * (priority 34) is only ordered by an ANTI-dependence on whichever `ldrsh`'s reloaded offset
 * register holds the store's value (r3); the other `ldrsh` (priority 36) has no edge at all and
 * wins on priority.
 *
 * So no source order can satisfy both, and the two orderings are 4 and 6.
 *
 * MEASURED: inline index 29 (named index 42); swapped ldrsh order 27; offset locals 29; a
 * signed-short store 75; a one-statement copy 29; a separate store pointer 29; the store between
 * the loads 29; the store after both loads 28; offset locals in either order 27; --no-sched2 27.
 */
extern int GetPartySize(void);
extern void *GetUnit(int unit);
extern int GetFlag(int id);
extern unsigned char gState[];
extern unsigned char L7a828[] __asm__("_TBL_7a828");

void Func_8078144(void)
{
    void *r5;
    int r0;
    int r1;
    int r3;
    int i;
    int n;
    int k;
    int id;
    int t;
    int ok;

    n = GetPartySize();
    for (i = 0; i < n; i++) {
        id = gState[(0xfc << 1) + i];
        t = L7a828[id];
        ok = 0;
        if (t == 0) {
            if (GetFlag(0x88 << 1) != 0) {
                ok = 1;
            } else if (GetFlag(0x89 << 1) != 0) {
                ok = 1;
            }
        } else {
            if (GetFlag(0x111) != 0) {
                ok = 1;
            } else if (GetFlag(0x113) != 0) {
                ok = 1;
            }
        }
        if (ok != 0) {
            r5 = GetUnit(id);
            r3 = *(unsigned short *)((char *)r5 + 0x36);
            *(unsigned short *)((char *)r5 + 0x3a) = r3;
            r1 = *(short *)((char *)r5 + 0x34);
            r0 = *(short *)((char *)r5 + 0x38);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x14) = r3;
            if ((r3 << 16) != 0) {
                goto label_0x3a;
            }
            r3 = *(short *)((char *)r5 + 0x38);
            if (r3 == 0) {
                goto label_0x3a;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x14) = r3;
        label_0x3a:
            r0 = *(short *)((char *)r5 + 0x3a);
            r1 = *(short *)((char *)r5 + 0x36);
            r0 <<= 14;
            r0 /= r1;
            r3 = 0x80;
            r3 <<= 7;
            if (r0 > r3) {
                r3 = 0x80 << 7;
            } else {
                if (r0 < 0) {
                    r3 = 0;
                } else {
                    r3 = r0;
                }
            }
            *(short *)((char *)r5 + 0x16) = r3;
            if ((r3 << 16) != 0) {
                goto label_next;
            }
            r3 = *(short *)((char *)r5 + 0x3a);
            if (r3 == 0) {
                goto label_next;
            }
            r3 = 1;
            *(short *)((char *)r5 + 0x16) = r3;
        }
    label_next:
        ;
    }
}
