/* OvlFunc_957_200ba30 -- 0x0200ba30
 *
 * A three-state effect step: on entry seed the start and end points and pick a
 * randomised travel vector, then on the following states poll the mover and
 * either back the state counter off or hand over to the finisher.
 *
 * A BYTE-WIDE `-1` IS A MODE ARTEFACT, NOT A SPELLING RESIDUE. This is the new
 * row and it is the whole reason the function did not land. With a `char *`,
 * `*p = *p - 1;` compiles the -1 in QImode, and gcc canonicalises that to
 * `add r3, #0xff` where the ROM has `sub r3, #0x1`. FOUR syntactically distinct
 * decrements -- `*p = *p - 1`, `(*p)--`, `*p -= 1`, `*p = *p + -1` -- all lower
 * to the same QImode `plus -1` and all give the same wrong instruction.
 *
 * Introducing an `int`-typed name for the loaded byte forces the arithmetic
 * into SImode and yields `sub`:
 *
 *     int n = *p;
 *     *p = n - 1;
 *
 * The temp is scoped inside the arm, matching the ROM's live range. Note the
 * ASYMMETRY: the INCREMENT needs no such treatment -- `+1` survives QImode
 * intact. So this is not "narrow arithmetic needs a name"; it is specifically
 * that a negative addend gets canonicalised and a positive one does not.
 *
 * The other three levers are all already on file:
 *  - STATEMENT ORDER IS STORE ORDER, twice (four of the seven initial diffs).
 *    Writing the stores in the ROM's emission order fixes both, and gcc's CSE
 *    folds the duplicated reads back into single loads by itself.
 *  - STORE THE SWITCH VARIABLE, NOT A LITERAL ZERO. The ROM reuses the
 *    already-zero state value rather than materialising a fresh zero, and that
 *    reuse is also what keeps it live across both calls and so in a
 *    callee-saved register.
 *  - Nothing needed pinning. The pointer reaches a high register on its own
 *    because it is live across calls in two arms -- statement structure alone,
 *    no pin and no barrier.
 *
 * On the division helpers: this overlay's linker script does carry the
 * `__divsi3`/`__modsi3`/`__udivsi3` aliases, so it is in the ALIAS camp -- a
 * plain `/` or `%` would be correct here and a direct call to a `_RAM` name
 * would show up as a relocation-name disagreement. Moot for this function,
 * which divides nothing, but worth recording for its siblings.
 *
 * Verified with tools/objcmp.py: 164 bytes, 75 encodings and 7 relocations
 * identical.
 */
extern unsigned int iwram_3001800;
extern int __Random(void);
extern void __PlaySound(int id);
extern int __Func_809ba34(char *a);
extern void __Func_809bb34(char *a);
extern void __vec3_translate(int x, int y, int *v);

void OvlFunc_957_200ba30(char *e)
{
    char *p;
    int v[3];
    int k;

    p = e + 0x40;
    k = *(signed char *)p;
    if (k == 0) {
        *(int *)(e + 4) = *(int *)(e + 0x14);
        *(int *)(e + 8) = *(int *)(e + 0x18);
        v[0] = *(int *)(e + 0x14);
        v[2] = *(int *)(e + 0x18);
        __vec3_translate(0xf0 << 15, __Random(), v);
        *(int *)(e + 0xc) = v[0];
        *(int *)(e + 0x10) = v[2];
        *(int *)(e + 0x24) = 0xa0 << 11;
        *(int *)(e + 0x20) = 0xa0 << 11;
        *(char *)(e + 0x42) = k;
        *p = *p + 1;
        if ((iwram_3001800 & 3) == 0) {
            __PlaySound(0x86);
        }
    } else if (k == 1) {
        if (__Func_809ba34(e) == 0) {
            int n = *p;
            *p = n - 1;
        }
    } else if (k == 2) {
        if (__Func_809ba34(e) == 0) {
            __Func_809bb34(e);
        }
    }
}
