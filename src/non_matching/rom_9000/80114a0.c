/* Func_80114a0 -- NON-MATCHING, 55 encodings of 97 differ (objcmp: ref 97 / ours 95,
 * ref 200 bytes / ours 192).  tryc: rom 100 lines, ours 98, 53 differ.
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/L/Func_80114a0.park.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_80114a0
 *
 * FRESH TARGET (batch 286).  The streaming twin of Func_80113e4 (same file, not yet
 * elevated either): same 2x2x2 quadrant walk, force=0, return on the first load.
 *
 * WHAT IS RIGHT: prologue, the ldmia pos read (`x = *p++; z = p[1];` -- the p[0]/p[2]
 * spelling gives two plain ldr), the two signed >>25 biases (x - 0x1000000 must be
 * written with a signed constant; 0xff000000 is unsigned and gives lsr), the
 * struct-member table read `state->quad[idx]` (gives the ROM's
 * lsl/add #0x138/ldrh [state, off] association; a u8* + 0x138 cast gives
 * (idx*2 + state) + 0x138 instead), and the z giv: writing `z + j` in BOTH the call
 * and the index makes loop.c reduce it into r6 and leave the ROM's `mov r8, r6` copy.
 *
 * BLOCKER: THE 0xF MASK CONSTANT.  ROM rebuilds `mov #0xf` in the k-loop body and at the
 * j-level (with the hoisted row), never in a register.  Ours: cse (pass 03) merges the
 * row's and the column's 15 into one pseudo; loop.c moves it with the row to the j
 * preheader, then (life 30, "halved since already moved") on to the outermost preheader
 * in r11.  That steals the high register the ROM spends on its bias copy, so bias stays
 * a plain register giv in r9 (the ROM keeps it at [sp+4] and copies it into r11 at each
 * layer), and the j counter lands in r8/r10 differently.  Writing the row at j-level instead keeps the
 * k-loop's 15 separate ("not desirable") BUT combine_movables (loop.c) merges the two
 * equal-constant movables in the j-loop scan (savings 2), moves them, and cse2 folds the
 * k-loop one into it.  So neither placement reproduces the ROM; the ROM's two 15s were
 * never unified, which no spelling tried here achieves.
 *
 * INERT / WORSE (measured): layer*0x140 inline (80 differ), bias as a user var
 * incremented (84), `zq` as its own biv (84), row local at j-level (77-84), all-goto
 * loops with explicit copies (88; copies coalesce), 0xff000000 constant (unsigned).
 *
 * ============ BATCH 334, BRIEF C: RE-MEASURED, AND THE SAME PIECE-LEVEL
 * ============ BLOCKER AS ITS PIECE-MATE
 *
 * RE-MEASURED and the claim line at the top of this file REPRODUCES: 55 of 97
 * encodings, ref 97 / ours 95, instructions ref 94 / ours 92, size ref 200 bytes
 * / ours 196.  COUNT DIFFERS, so that positional figure measures MISALIGNMENT and
 * the honest statement is WE ARE TWO 16-BIT INSTRUCTIONS SHORT.  Read the count
 * before the figure; the figure is not a distance here and the real distance is
 * unknown until the lengths agree.
 *
 * ==================== THE PIECE CANNOT BE LANDED AT ALL YET ====================
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN functions and ALL SEVEN ARE PARKED --
 * Func_8010e14, Func_8010ff0, Func_80110e0, Func_8011164, Func_80111b4,
 * Func_80113e4 and this one.  A .c is one TU, so a match here installs nothing
 * without a split.  Its piece-mate Func_8011164 is also parked two instructions
 * short and is NOT adjacent to this function, so taking both off the count needs
 * TWO byte-neutral cuts of the same .s plus a linker-script change.  Schedule the
 * two as a pair, not as standalone singles.
 *
 * ALSO: the header above names Func_80113e4 as "the streaming twin of this
 * function (same file, not yet elevated either)".  That is still true, and it is
 * a THIRD member of the same piece -- so a split that carves out this function
 * and Func_8011164 leaves the twin behind in the remainder, where it stays
 * reachable as a separate target.  The twin is the first thing to read before any
 * further work here: the brief's standing rule is that when a park names another
 * function, that function's source is read first, and in this piece every named
 * relative is still a park, which is itself the reason this one has had no
 * labelled-corpus neighbour to query.
 *
 * >> ON THE STATED BLOCKER.  The 0xF-mask story above is an observation about
 *    where two equal constants are unified, and it is NOT yet an account of the
 *    two-instruction deficit -- the header never says which two instructions are
 *    missing, only which register the ROM spends differently.  The first thing a
 *    next round should produce is the DECOMPOSITION: count the ROM's and our
 *    instructions inside each of the three nested loops and in the straight-line
 *    head and tail separately, and say which region is short.  Func_8011164's
 *    header in this same piece is the model for that, and on a body of this size
 *    tools/aligncmp.py's hunk list gives it directly.  Without it, every mask
 *    spelling measured here is being screened against a figure that cannot move
 *    until the lengths do.
 */
extern unsigned char *iwram_3001e70;
extern int Func_80108e4(int layer, int qx, int qz, int tileset, int force);

struct WorldMapState {
    int *pos;
    unsigned char pad[0x134];
    unsigned short quad[256];
};

void Func_80114a0(void)
{
    struct WorldMapState *state = (struct WorldMapState *)iwram_3001e70;
    int x = 0, z = 0;
    int *p;
    unsigned int layer, j, k;

    p = state->pos;
    if (p) {
        x = *p++;
        z = p[1];
    }
    x = (x - 0x1000000) >> 25;
    z = (z - 0x1400000) >> 25;
    for (layer = 0; layer <= 1; layer++) {
        int bias = layer * 0x140;
        for (j = 0; j <= 1; j++) {
            for (k = 0; k <= 1; k++) {
                if (Func_80108e4(layer, x + k, z + j,
                        state->quad[(((z + j) & 15) << 4) + ((x + k) & 15)] + bias, 0))
                    return;
            }
        }
    }
}
