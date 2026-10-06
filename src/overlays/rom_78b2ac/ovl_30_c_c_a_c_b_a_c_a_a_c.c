/* OvlFunc_890_2008ef8, OvlFunc_890_200901c, OvlFunc_890_2009140
 * [overlays/rom_78b2ac] -- 399 encodings, 876 bytes, EXACT.
 *
 * MATCHING, 0 differing encodings of 399.  Whole TU: 876 bytes, 399 encodings
 * and 36 relocations identical; each function is 292 bytes / 133 encodings on
 * its own.  THREE FUNCTIONS IN ONE PIECE, all three parked, all three closed by
 * one body.  Pins 0, devices 0, no flag group, no split, no exports needed
 * (.L2ddc, .L2de0, .L2de8 and .L2dec are already .global in
 * asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_c.s:20-24).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_a_c.c asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_a_c.s --whole
 *
 * HOW IT CLOSED, from 9 of 133, in two independent edits.
 *
 * EDIT 1, 9 -> 3.  THE STACK-ARGUMENT MATERIALISATION LEVER, CROSSED.  Six of
 * the nine were the two arms whose stack arguments are BOTH literals:
 *
 *     rom    mov r3, #0x1 / mov r2, #0x5 / str r3, [sp] / str r2, [sp, #0x4]
 *     before mov r3, #0x1 / str r3, [sp] / mov r3, #0x5 / str r3, [sp, #0x4]
 *
 * Both literals become two separate named locals, declared in a BLOCK INSIDE
 * THE ARM and assigned AFTER the arm's preceding call.  The park had measured
 * "the first stack literal named in the entry block, 137 of 136" and "the
 * second stack literal named in the entry block, 130 of 132" and concluded
 * "no source spelling reaches it".  Two things were wrong with those rows and
 * both had to be fixed at once: ONE AT A TIME cannot work, because the point of
 * the lever is that two live values exist simultaneously; and THE ENTRY BLOCK
 * is the wrong scope, because a constant declared there is hoisted into a
 * callee-saved register and the prologue changes.  Neither row was a bound.
 *
 * EDIT 2, 3 -> 0.  `L2de8 = 2` with the VALUE given a name:
 *
 *     rom    ldr r2, =.L2de8 / mov r3, #0x2 / str r3, [r2, #0x0]
 *     before ldr r3, =.L2de8 / mov r2, #0x2 / str r2, [r3, #0x0]
 *
 * `int v; v = 2; L2de8 = v;` in its own block is exact.  The park had tried
 * `p = &L2de8; *p = 2;` and recorded it inert, which REPRODUCES: naming the
 * ADDRESS does nothing.  And naming BOTH -- pointer and value together -- is
 * also 3, so the named pointer actively CANCELS the win.  The store's address
 * has to stay a SYMBOL_REF; only the stored value may become a pseudo.
 *
 * THE FAMILY, and the park's description of it corrected.  The park said the
 * other two functions are "byte-identical to this one except for the state
 * variable they read".  They are not -- they differ in six parameters -- but
 * they are the same shape instruction for instruction, so one body with six
 * substitutions covers all three:
 *
 *     function   state    sparse case   arg0   arg2   arg3 base   threshold
 *     2008ef8    .L2ddc   0x5a          0x2f   0x2a   0x21        0x64
 *     200901c    .L2de0   0x5f          0x30   0x1f   0x24        0x69
 *     2009140    .L2dec   0x55          0x2e   0x29   0x24        0x5f
 *
 * arg1 is 0x3b in the five dense arms and 0x31 in the sparse arm in all three;
 * arg3 runs base .. base+4; .L2de8 is shared by all three.
 *
 * WHERE THE LEVER ALREADY WAS.  The installed file-mate
 * src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_a_b.c states it in its own
 * header -- "the switch arms each need their OWN block-scoped stack-argument
 * locals.  Bare literals are 6, one shared pair across the arms is 8, per-arm
 * is exact" -- one piece away in the same directory, for the same callee, with
 * the same arm structure.  This park never cited it.  That is the third recorded
 * instance of the class "the mechanism existed, indexed under another name".
 *
 * KEPT FROM THE PARK, all still load-bearing: the signedness split (`s` is
 * `int` and __Random() returns `unsigned int`, so the switch compares signed
 * and the final comparison is promoted unsigned); `s` reassigned to 1 inside
 * cases 2, 3 and 4; `s` passed as a stack argument in cases 1-4; the
 * comparison-tree dispatch left to gcc; the pooled zero for the final halfword
 * store; and the asm-label externs for the state words.
 */
extern unsigned short L2ddc __asm__(".L2ddc");
extern unsigned short L2de0 __asm__(".L2de0");
extern unsigned short L2dec __asm__(".L2dec");
extern int L2de8 __asm__(".L2de8");
extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_2008ef8(void)
{
    int s;

    if ((__Random() & 3) == 0)
        return;
    s = L2ddc;
    switch (s) {
    case 0:
        {
            int p;
            int q;
            __PlaySound(0xbb);
            p = 1;
            q = 5;
            __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x21, p, q);
        }
        break;
    case 1:
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x21, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x22, s, 5);
        break;
    case 2:
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x22, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x23, s, 5);
        break;
    case 3:
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x23, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x24, s, 5);
        break;
    case 4:
        {
            int v;
            v = 2;
            L2de8 = v;
        }
        s = 1;
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x24, s, s);
        __CopyMapTiles(0x2f, 0x3b, 0x2a, 0x25, s, 5);
        break;
    case 0x5a:
        {
            int p;
            int q;
            p = 1;
            q = 0xa;
            __CopyMapTiles(0x2f, 0x31, 0x2a, 0x21, p, q);
        }
        break;
    }
    L2ddc = L2ddc + 1;
    s = L2ddc;
    if (s > ((__Random() * 40) >> 16) + 0x64)
        L2ddc = 0;
}

void OvlFunc_890_200901c(void)
{
    int s;

    if ((__Random() & 3) == 0)
        return;
    s = L2de0;
    switch (s) {
    case 0:
        {
            int p;
            int q;
            __PlaySound(0xbb);
            p = 1;
            q = 5;
            __CopyMapTiles(0x30, 0x3b, 0x1f, 0x24, p, q);
        }
        break;
    case 1:
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x24, s, s);
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x25, s, 5);
        break;
    case 2:
        s = 1;
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x25, s, s);
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x26, s, 5);
        break;
    case 3:
        s = 1;
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x26, s, s);
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x27, s, 5);
        break;
    case 4:
        {
            int v;
            v = 2;
            L2de8 = v;
        }
        s = 1;
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x27, s, s);
        __CopyMapTiles(0x30, 0x3b, 0x1f, 0x28, s, 5);
        break;
    case 0x5f:
        {
            int p;
            int q;
            p = 1;
            q = 0xa;
            __CopyMapTiles(0x30, 0x31, 0x1f, 0x24, p, q);
        }
        break;
    }
    L2de0 = L2de0 + 1;
    s = L2de0;
    if (s > ((__Random() * 40) >> 16) + 0x69)
        L2de0 = 0;
}

void OvlFunc_890_2009140(void)
{
    int s;

    if ((__Random() & 3) == 0)
        return;
    s = L2dec;
    switch (s) {
    case 0:
        {
            int p;
            int q;
            __PlaySound(0xbb);
            p = 1;
            q = 5;
            __CopyMapTiles(0x2e, 0x3b, 0x29, 0x24, p, q);
        }
        break;
    case 1:
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x24, s, s);
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x25, s, 5);
        break;
    case 2:
        s = 1;
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x25, s, s);
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x26, s, 5);
        break;
    case 3:
        s = 1;
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x26, s, s);
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x27, s, 5);
        break;
    case 4:
        {
            int v;
            v = 2;
            L2de8 = v;
        }
        s = 1;
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x27, s, s);
        __CopyMapTiles(0x2e, 0x3b, 0x29, 0x28, s, 5);
        break;
    case 0x55:
        {
            int p;
            int q;
            p = 1;
            q = 0xa;
            __CopyMapTiles(0x2e, 0x31, 0x29, 0x24, p, q);
        }
        break;
    }
    L2dec = L2dec + 1;
    s = L2dec;
    if (s > ((__Random() * 40) >> 16) + 0x5f)
        L2dec = 0;
}
