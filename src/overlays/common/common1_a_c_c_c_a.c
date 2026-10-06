/* Cluster OvlFunc_common1_15b8..OvlFunc_common1_15b8 extracted from
 * goldensun/asm/overlays/common/common1_a_c_c_c.s.
 *
 * LANDING, batch 329 brief J. Parked at 6 of 34; byte-identical now.
 *   OK OvlFunc_common1_15b8 -- 80 bytes, 34 encodings and 6 relocations identical
 *   --whole agrees: ok OvlFunc_common1_15b8 34 encodings / OK whole file.
 * No pins, no devices, no volatile, no flag change -- ordinary C.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/common/common1_a_c_c_c_a.c asm/overlays/common/common1_a_c_c_c_a.s --whole
 *
 * FLAGS, CHECKED NOT ASSUMED: `common1` matches no Makefile override (the
 * COMMON2_CFLAGS rule patterns on common2_%.o), so this builds under the generic
 * rule with -mthumb-interwork AND -fcall-used-r4, which is what the measurement
 * used.  Same conclusion the landed sibling common1_a_c_c_c_b.c records.
 *
 * ===== THE EDIT: NAME THE ZERO, AND PLACE ITS ASSIGNMENT. =====
 *
 * The park sat at 6 of 34 with the right instructions in the wrong order:
 *
 *   rom   asr r3,#1 / str r3,[r5,#0x34] / mov r3,r5 / mov r2,#0
 *         / add r3,#0x5b / strb r2,[r3]
 *   park  mov r2,r5 / asr r3,#1 / add r2,#0x5b / str r3,[r5,#0x34]
 *         / mov r3,#0 / strb r3,[r2]
 *
 * Six instructions, permuted, with the roles of r2 and r3 exchanged.  The park
 * read the register exchange as a second fact; it is not.  The ROM's byte
 * pointer is r3 -- the register `v` has just died out of -- and it can only be
 * r3 if the pointer is born AFTER the `str`.  Order decides the registers.
 *
 * What fixes the order is a named `int zero` whose ASSIGNMENT sits between the
 * shift and the second store.  That is the single change from the park body.
 *
 * WHY THE PARK MISSED IT.  Its one recorded attempt at the ordering named the
 * POINTER and the zero together (`p = a; z = 0; p += 0x5b; *p = z;`) and read 11
 * of 34 with the stream a line short, because naming the pointer lets gcc
 * reassociate the whole prologue.  Naming only the zero leaves the pointer
 * arithmetic alone.  The unvaried dimension was not WHICH value to name, it was
 * naming ONE of them and placing it.
 *
 * KEPT FROM THE PARK, and it is load-bearing: the second constant is DERIVED
 * from the first by a shift in the source (`v >>= 1`).  Two literals give a
 * fresh `mov r3,#0xa0 / lsl r3,#8` and 8 of 34.
 *
 * MEASURED INERT -- all at 6 of 34 and byte-identical to the park body:
 *   - `*(a + 0x5b) = 0;` for `a[0x5b] = 0;`
 *   - `*(char *)(a + 0x5b) = 0;`  (signed-char lvalue, different alias set)
 *   - a full COMPONENT_REF rewrite: a `struct Actor` with fc/f30/f34/f5b fields
 *     and every access a field reference.  So on this function the
 *     alias-set/dependent-count rung of rank_for_schedule
 *     (haifa-sched.c:4096-4107) bounds nothing: the byte store's address
 *     arithmetic is hoisted by exactly the same amount whether the reference is
 *     a COMPONENT_REF or a cast byte index.  Worth knowing, because that lever
 *     closed a function two batches ago.
 */
extern void *__GetFieldActor(int id);
extern void __Actor_Stop(void *a);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);

void OvlFunc_common1_15b8(int id, int x, int z)
{
    unsigned char *a;
    int v;
    int zero;

    a = (unsigned char *)__GetFieldActor(id);
    if (a == 0)
        return;
    v = 0xa0 << 9;
    *(int *)(a + 0x30) = v;
    v >>= 1;
    zero = 0;
    *(int *)(a + 0x34) = v;
    a[0x5b] = zero;
    __Actor_Stop(a);
    __Actor_SetAnim(a, 5);
    __Actor_TravelTo(a, x << 16, *(int *)(a + 0xc), z << 16);
    __Actor_WaitMovement(a);
    __Actor_SetAnim(a, 1);
}
