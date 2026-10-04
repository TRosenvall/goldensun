/* Func_80936a0 (ScrollCameraBy) -- 0x080936a0.  LANDS, 0 of 48, PIN-FREE.
 * ref: asm/rom_8a000/rom_93304_a_c_a_a_a_c.s  (ONE function, no data section;
 *      whole-file conversion, no split -- tools/datacheck.py prints nothing)
 * install at: src/rom_8a000/rom_93304_a_c_a_a_a_c.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_93304_a_c_a_a_a_c.c \
 *     asm/rom_8a000/rom_93304_a_c_a_a_a_c.s --whole
 *   -> OK whole file -- 112 bytes, 48 encodings and 6 relocations identical
 *
 * Pins: 0.  Shims: 0.  No fakematch row.  split_s.py: not needed.
 *
 * ---------------------------------------------------------------- WHAT MOVED --
 * The park (src/non_matching/rom_8a000/80936a0.c, 16 of 48) closed with
 * "NEXT: nothing source-level outstanding", and listed under
 * "WHAT IS RIGHT AND SHOULD BE KEPT" the one thing that was wrong:
 *
 *     the two DERIVED offsets.  The ROM builds 0xd4 << 2 and then `add r3, #4`
 *     for the next field ... written as `off = 0xd4 << 2; q1 = g + off;
 *     off += 4; q2 = g + off;` both chains come out with the ROM's arithmetic.
 *
 * THE ROM'S `add r3, #4` AND `add r1, #2` ARE NOT SOURCE ARITHMETIC.  They are
 * `reload_cse_move2add` (reload1.c:8840) deriving the second constant from the
 * first, which it can do because local-alloc had already put both in ONE hard
 * register.  There is no offset variable in this function at all: the source is
 * four plain constant-offset accesses off `g`.
 *
 * Why the park's offset variables cost 16 encodings, read out of the compiler:
 * `off` is a block-local pseudo that DIES at `q2 = g + off`, and local-alloc's
 * tying loop (local-alloc.c:1090-1178) ties operand 0 of any `=`-without-`&`
 * insn to an operand that dies in it.  So `combine_regs(off, q2)` succeeds and
 * q2 inherits off's register -- `add r3, r5, r3`.  The ROM instead keeps the
 * offset in r3 and gives q2 a fresh r2, which is what `add r2, r5, r3` is.  Once
 * the offset is a constant rather than a pseudo there is nothing for q2 to be
 * tied to, and every register downstream falls into place.
 *
 * THE BANK'S OWN LANDED SIBLING SAYS SO.  src/rom_8a000/rom_93304_a_c_a_a.c
 * (Func_8093710) is in the SAME .s family, opens with the same
 * `g = iwram_3001e70; galloc_ewram(0x1b, 0xccc);` and the same
 * `(0xcf << 1)` short test, and spells its field access
 * `*(short *)(pFVar1 + (0xd6 << 2))` -- a constant offset, no variable.  That
 * is the spelling used below.
 *
 * FIVE spellings of the constant-offset form all measure 0 (so the landing is
 * not balanced on one lucky phrasing): `(0xd4 << 2)` inline; plain `0x350`;
 * named `q1..q4` pointers from constants, either spelling; and
 * `((int *)g)[0xd4]` subscripts.  The sibling's `<< 2` spelling is shipped.
 *
 * MEASURED, and each of these three IS load-bearing (remove one and it breaks):
 *   `int (*f)(int,int); f = Func_80008ac; r = f(...)`  -> the ROM's
 *       `ldr r3, =Func_80008ac / bl _call_via_r3`.  Calling Func_80008ac
 *       directly: 34 of 48 at 45 insns (RELOC).            [park was right]
 *   the named `r` for the call result: inlining it is 53 at 55 insns (RELOC).
 *   the named `z` for the stored zero: `= 0` inline is 22 at 50 insns (RELOC).
 *
 * Also measured on the park's own body, pin-free, before the above was found:
 * moving `*(int *)q2 = r;` up to just after the field copy is worth 2 (16 -> 14),
 * and moving `z = 0;` earlier is worth 1 (16 -> 15); a 144-body sweep over the
 * legal statement orders of the park's shape bottoms out at 14.  Pins on the
 * park's shape bottom out at 3 of 48 with four pins.  Both are superseded.
 */
extern unsigned char *iwram_3001e70;
extern unsigned char *galloc_ewram(int a, int b);
extern int Func_80008ac(int a, int b);
extern void Func_80935d4(void);
extern void StartTask(void *f, int a);

void Func_80936a0(int a, int b)
{
    unsigned char *g;
    unsigned char *p;
    int (*f)(int, int);
    int r;
    int z;

    g = iwram_3001e70;
    p = galloc_ewram(0x1b, 0xccc);
    if (*(short *)(p + (0xcf << 1)) == 3) {
        f = Func_80008ac;
        r = f(a, 0x80 << 9);
        *(int *)(g + (0xd4 << 2)) = *(int *)(g + (0xd4 << 2) + 4);
        *(int *)(g + (0xd4 << 2) + 4) = r;
        *(short *)(g + (0xd6 << 2)) = b;
        z = 0;
        *(short *)(g + (0xd6 << 2) + 2) = z;
        StartTask(Func_80935d4, 0xc94);
    }
}
