/* OvlFunc_935_20089c0 -- 0x020089c0, asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c_a.s.
 *
 * LANDING: 224 bytes, 93 encodings and 16 relocations identical.
 * PINS: 0 (tools/shimcount.py) -- the park it replaces carried SEVEN.
 * PER-FILE FLAG GROUP: CSE_CFLAGS. See the reason below.
 *
 * Verify with (inside the build container):
 *   python3 tools/objcmp.py src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c_a.c \
 *     asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c_a.s --whole
 *
 * SPLIT SHAPE: none. The .s holds this function alone and tools/datacheck.py
 * reports no data, so "split": null and "exports": [].
 *
 * A per-frame handler: bump two counters held in `.lcomm` cells, compare one
 * against a table, then sweep four actors resetting any that have gone out of
 * range. Three cells reached with the tree's asm-renamed extern idiom.
 *
 * ============ THE PARK'S SEVEN PINS HAD ALIASED TWO LIVE VALUES ============
 *
 * The park read fifty-seven of ninety-three and named its blocker "a register
 * the ROM spends and ours does not": the ROM pushes r5, r6 and r7 and keeps the
 * fetched actor in r5, and the park's body pushed only r6 and r7. It then
 * recorded that pinning the actor to r5 makes things WORSE, and left the
 * question as "how to keep the `mov r5, r0` copy that the pin destroys".
 *
 * THE QUESTION WAS THE WRONG WAY ROUND. The park spelled two one-register-two-
 * roles pairs as `register` declarations naming r6 and r7, and believed them
 * correct -- "they are not the residue". They were. The zero stored through the
 * swept actors was pinned to r7 while the unpinned actor pointer was ALSO given
 * r7, and the output reads `str r7, [r7, #0x48]`: the body was storing the
 * POINTER into the actor instead of zero. One `mov r7, #0` was missing from the
 * stream because the value it would have built had been aliased away, and that
 * -- not a register the compiler refused to spend -- is the whole length gap.
 *
 * DROPPING EVERY PIN FIXES THE ROLES WITH NO PIN AT ALL. Left alone, gcc puts
 * the actor in r5, the actor byte and then the loop counter in r6, and the cell
 * pointer and then the zero in r7, which is the ROM's assignment exactly, and
 * the prologue widens to `push {r5, r6, r7, lr}` on its own.
 *
 * GENERAL: a pin cannot be assumed inert because its register looks free. Two
 * pins on one register in sibling scopes, with an unpinned local in the same
 * live range, will silently take that register and the symptom shows up as a
 * MISSING instruction somewhere else entirely.
 *
 * TWO SMALL THINGS BESIDES.
 *
 * A. THE TABLE ACCESS WANTS BASE AND INDEX AS SEPARATE NAMED LOCALS. The ROM's
 *    `ldr r2,=.L2214 / lsl r3,r0,#2 / ldr r2,[r2,r3]` is a register+register
 *    load, so the two halves must be live at once; the park had this right and
 *    reached it with r2/r3 pins. Ordinary named locals do the same job --
 *    pin-free it is worth one aligned position.
 *
 * B. A SAVE-FLAG ID TESTED AND THEN SET, WHICH IS THE CSE_CFLAGS CONSTRUCT. The
 *    ROM loads the same pool word twice, `ldr r0,=0x207` for the test and again
 *    for the set. gcc's second common-subexpression pass hoists it into a
 *    callee-saved register and pays two `adds r0, r5, #0` plus the load for the
 *    privilege.
 *
 *    NO SPELLING OF THE SOURCE AVOIDS IT, and that is measured, not assumed:
 *    two DISTINCT locals, each assigned the id immediately before its own call,
 *    compile to the identical hoist. There is no cse-block boundary between the
 *    two uses to hide behind -- cse ends a block only at a CODE_LABEL
 *    (cse.c:6572) and the conditional jump between them branches FORWARD, so
 *    both uses sit in one block.
 *
 * FLAG REASON, stated for the owner. The test-then-set shape of a save-flag id
 * is the construct CSE_CFLAGS was created for and the group already carries
 * members in several overlays for it. At production flags this body is eleven
 * aligned positions away with a stream two entries long; under the group it is
 * byte-identical. The alternative is a `register int __asm__("r0")` assigned at
 * each of the two sites, which lands at production flags but reintroduces a pin
 * the depinning pass would have to take back off, and the pin policy prefers
 * the pin-free body.
 *
 * WHAT CARRIED OVER FROM THE PARK, unchanged and right: the guard sense and its
 * long branch over the whole body, the two-step counter bump through a named
 * cell pointer, re-reading the first cell for the table compare rather than
 * reusing the bumped value, the `k + 0xb` and `i + 0xb` actor slots, the order
 * of the four stores through a swept actor, and the do/while shape of the sweep.
 */
extern int L2224 __asm__(".L2224");
extern int L2228 __asm__(".L2228");
extern int L2214 __asm__(".L2214");
extern int OvlFunc_935_2008944(int a);
extern void OvlFunc_935_2008b8c(void);

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_935_20089c0(void)
{
    unsigned char *s;
    int *c;
    int *d;
    int f;
    int t;
    int k;
    int i;
    int z;

    f = __MapActor_GetActor(0xa)[0x5b];
    if (f == 0) {
        c = &L2224;
        t = *c;
        t += 1;
        *c = t;
        if (t > 0xbe)
            *c = f;
        d = &L2228;
        k = *d;
        {
            unsigned char *tb;
            int ix;

            tb = (unsigned char *)&L2214;
            ix = k << 2;
            if (*(int *)(tb + ix) == *c) {
                s = __MapActor_GetActor(k + 0xb);
                *(int *)(s + 0x48) = 0xa3d;
                t = *d;
                t += 1;
                *d = t;
                if (t > 3)
                    *d = f;
            }
        }
        i = 0;
        z = 0;
        do {
            s = __MapActor_GetActor(i + 0xb);
            if (*(int *)(s + 0x28) >= 0 && *(int *)(s + 0xc) <= 0xffff) {
                OvlFunc_935_2008b8c();
                *(int *)(s + 0xc) = 0xff << 16;
                *(int *)(s + 0x48) = z;
                *(int *)(s + 0x28) = z;
                s[0x5b] = z;
                __PlaySound(0x6a);
            }
            i++;
        } while (i <= 3);
        if (OvlFunc_935_2008944(0xa) != 0) {
            __MapActor_SetAnim(0xa, 1);
            if (__GetFlag(0x207) == 0) {
                __SetFlag(0x207);
                __PlaySound(0xcc);
            } else {
                __PlaySound(0x6a);
            }
        }
        if (OvlFunc_935_2008944(9) != 0)
            __PlaySound(0x6a);
    }
}
