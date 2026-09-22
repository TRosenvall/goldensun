/* Cluster Func_97f80..Func_97f80 extracted from goldensun/asm/rom_8a000/rom_97b54_a_c_a_a_a_c.s.
 *
 * Total .text for this TU = 172 bytes (= 0xac). Never attempted before batch 279, and it could not
 * have been picked before batch 278: THIS FUNCTION IS DECLARED `.thumb_Func_start` WITH A CAPITAL
 * F, a disassembler typo GAS accepts, and it was invisible to census.py until that scan was made
 * case-insensitive. Its whole size band was being reported as empty.
 *
 * FOR THE RECORD, SINCE IT AFFECTS FUTURE PICKS: tryc.py, objcmp.py and split_s.py all match the
 * directive case-insensitively and handled it without trouble. Still case-sensitive and therefore
 * blind to it: asmfacts.py, blocked_cse.py, find_bb_lever.py, find_twins.py, find_jumptables.py,
 * match_shapes.py, not_c.py, pool_candidates.py, find_shape.py, pool.py, pick_candidates.py,
 * pickable.py, remaining.py, split_asm.py. None is wired into the build or the commit gate --
 * verified -- so this is a TARGETING blind spot, not a correctness risk. The other capital-F
 * survivor is Func_a1f74.
 *
 * No pins, no volatile, no flags. Three levers:
 *
 * 1. THE `goto` LOOP. A structured `while (1)` hoisted `mov r5, sp` and the `e + 0x42` address into
 *    the prologue; the goto form suppresses that. 79 differing to 31 -- the two-sign LICM rule on
 *    its suppress side.
 * 2. A NAMED `int` LOCAL FOR THE HALFWORD CONSTANT. Inline gives the ROM's `ldr r3, =0x400`;
 *    without it gcc synthesises `mov r3, #0x80 / lsl r3, #3`.
 * 3. `unsigned short r = Random();` IN A BLOCK SCOPE. The sched2 order of `lsr r1, #16` against
 *    `lsl r0, #13` is decided by that spelling: a `(unsigned short)` CAST and an `int` temp both
 *    leave 2 differing, and the block-scoped `unsigned short` temp is exact. So the truncation has
 *    to be a variable's TYPE rather than a cast, and the scope is what places it.
 */
extern int Random(void);
extern int Func_809ba34(char *a);
extern void Func_809bb34(char *a);
extern void vec3_translate(int x, int y, int *v);

void Func_97f80(char *e)
{
    char *p;
    int v[3];
    int k;

    p = e + 0x40;
top:
    k = *(signed char *)p;
    if (k == 0) {
        v[0] = *(int *)(e + 0x14);
        v[2] = *(int *)(e + 0x18);
        { unsigned short r = Random(); vec3_translate(0xf0 << 13, r, v); }
        *(int *)(e + 0xc) = v[0];
        *(int *)(e + 0x10) = v[2];
        *(int *)(e + 0x24) = 0x80 << 11;
        *(int *)(e + 0x20) = 0x80 << 11;
        *(char *)(e + 0x42) = k;
        *p = *p + 1;
    } else if (k == 1) {
        if (Func_809ba34(e) == 0) {
            *p = *p + 1;
            goto top;
        }
    } else if (k == 2) {
        *(int *)(e + 0xc) = *(int *)(e + 0x14);
        *(int *)(e + 0x10) = *(int *)(e + 0x18);
        { int c = 0x80 << 3; *(short *)(e + 0x32) = c; }
        *(char *)(e + 0x42) = 1;
        *p = *p + 1;
    } else if (k == 3) {
        if (Func_809ba34(e) == 0) {
            Func_809bb34(e);
        }
    }
}
