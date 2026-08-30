/*
 * Func_80930bc -- asm/rom_8a000/rom_92950_c_c_a.s
 *
 * BLOCKER: register allocation priority. 80 lines against 81.
 *
 * NOTE FIRST: the ROM function READS TWO UNINITIALISED LOCALS. r6 and r7 are
 * read at `mov r2,r7 / mov r1,r6` and never written; r7's incoming value is
 * destroyed by the prologue's `mov r7,r8 / push {r7}`, and the preceding
 * function does not fall through. This is a genuine bug in the original, not a
 * transcription error, and the C below reproduces it.
 *
 * The residue is a three-way permutation of which values get r6/r7/r8:
 *
 *      value                       rom   ours
 *      *iwram_3001ebc               r8    r6
 *      x, clamped to [8, 0x138]     r6    r7
 *      y, the 0x77 / [0x14,0xdc]    r7    r8
 *
 * gcc always hands the iwram pointer the first free callee-saved register
 * because it has the most references; the ROM puts it in a HIGH register, which
 * is the only reason the ROM pays `mov r7,r8 / push {r7}` at all.
 *
 * TRIED AND REJECTED, all measured against the 81-line reference: clamps applied
 * directly to x/y (79 lines, 80 differing -- x gets spilled); copy to xx/yy then
 * clamp (80, 76 -- best); plus a pointer local for the tail (76); ternaries for
 * the +/-0x20 (79 lines, 76); x/y declared first (76); the two copies in either
 * order (identical, 76); the pointer loaded after the call (80); no pointer
 * local at all (80); three extra unused parameters (80); explicit
 * `register ... __asm__("r6")` bindings (gcc gives the pointer r7 anyway and the
 * binding breaks); the pointer reduced to three references via intermediate
 * locals (65, but drags in r10); one nested ternary for both x clamps (76 lines,
 * 80 differing).
 *
 * The closest was `register unsigned char *s __asm__("r8")` with direct clamps:
 * 83 lines, 46 differing, first difference at 21 -- the entire prologue and head
 * exact. It is not a legitimate spelling and the tail costs two.
 *
 * DIAGNOSTIC worth keeping: artificially adding references to x and y moves the
 * pointer from r6 to r7 and puts x in r6, so gcc-2.96's allocation here is
 * priority-driven and the pointer IS beatable -- but no spelling was found that
 * gets both coordinates ahead of it while keeping the single `mov r1, r6` copy.
 */
extern unsigned char *iwram_3001ebc;
extern void *GetFieldActor(int slot);
extern int _Func_8017658(int msg, int x, int y, int flag);
extern int _Func_8017394(int handle);
extern void WaitFrames(int n);

void Func_80930bc(int packed)
{
    unsigned char *s;
    int slot;
    int x;
    int y;
    int handle;
    int xx;
    int yy;

    s = iwram_3001ebc;
    slot = packed & 0xfff;
    GetFieldActor(slot);
    *(int *)(s + (0xfa << 1)) = slot;
    if (*(int *)(s + (0xe6 << 1)) == 0) {
        yy = y;
        xx = x;
        if (yy > 0x77)
            yy += 0x20;
        else
            yy -= 0x20;
        xx = xx < 8 ? 8 : xx;
        if (xx > (0x9c << 1))
            xx = 0x9c << 1;
        yy = yy < 0x14 ? 0x14 : yy;
        if (yy > 0xdc)
            yy = 0xdc;

        handle = _Func_8017658(*(short *)(s + (0xec << 1)), xx, yy, 1);
        *(int *)(s + (0xfc << 1)) = handle;
        while (_Func_8017394(handle) == 0)
            WaitFrames(1);
    }
    *(unsigned short *)(s + (0xec << 1)) += 1;
}
