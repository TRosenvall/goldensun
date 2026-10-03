/* Func_801c244 -- NOT MATCHING
 *
 * NON-MATCHING, 31 of 55 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 55, ours 51) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS ALSO DIFFER -- this figure is NOT a distance. ***
 *   SIZE ref 140 bytes, ours 132.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801c244.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_c.s --func Func_801c244
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_c.s
 * Best screen: 59 instructions against the ROM's 62.
 *
 * BLOCKER CLASS: the -1 rematerialisation, third member of the family in
 * src/non_matching/overlays/constant_reuse.c.
 *
 * Three of the switch arms compare a return value against -1, and the ROM
 * builds it FRESH in each of them:
 *
 *     mov r3, #1 / neg r3, r3 / cmp r0, r3
 *
 * gcc builds it once and shares it, which is why we are three instructions
 * short. That is the same disposition catalogued for OvlFunc_965_2009158,
 * where three -1 arguments to one call are materialised independently in the
 * ROM and shared by gcc.
 *
 * FLAGS RULED OUT: -fno-rerun-cse-after-loop, -fno-gcse and
 * -fno-cse-follow-jumps all give byte-identical output. That matches the
 * eleven-flag sweep already recorded for the class.
 *
 * The rest reads clean: the loop is a `for (;;)` with the switch's arms either
 * returning or falling out to the next iteration, and the jump table's five
 * slots are cases 0..4 with the default returning.
 */
extern char *iwram_3001ebc;
extern void Func_801c2d0(void);
extern void Func_801c2e4(void);
extern int Func_8028920(int n);
extern int _Func_808ce74(void);
extern int _Func_80a5b94(void);
extern int _Func_80aa56c(void);
extern int _Func_80a24d0(void);
extern int _Func_80a7478(void);

void Func_801c244(void)
{
    char *p;
    int r;
    int v;

    p = iwram_3001ebc;
    r = 0;
    for (;;) {
        Func_801c2d0();
        r = Func_8028920(r);
        Func_801c2e4();
        switch (r) {
        case 0:
            v = _Func_808ce74();
            if (v == 0)
                v = 0xff;
            *(unsigned short *)(p + (0xbd << 1)) = v;
            return;
        case 1:
            if (_Func_80a5b94() != -1)
                return;
            break;
        case 2:
            if (_Func_80aa56c() == 0)
                return;
            break;
        case 3:
            if (_Func_80a24d0() != -1)
                return;
            break;
        case 4:
            if (_Func_80a7478() != -1)
                return;
            break;
        default:
            return;
        }
    }
}
