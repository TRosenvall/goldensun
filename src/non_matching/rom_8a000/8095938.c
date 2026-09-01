/* Func_8095938 (0x08095938) -- NON-MATCHING.
 * Blocker class: scheduling -- ONE store moved two instructions late.
 *
 * 137 lines against the ROM's 136, and the extra line is a DUPLICATE LABEL
 * (`.L20: .L21:` where the ROM has one), which emits no bytes. The instruction
 * counts are equal. The only byte difference is this, in the tail:
 *
 *     rom    ldr r3, [r5, #0x18] / str r3, [r6, #0x8] / mov r2, #0x3c
 *            ldrsh r0, [r5, r2]
 *     ours   ldr r3, [r5, #0x18] / mov r2, #0x3c / ldrsh r0, [r5, r2]
 *            str r3, [r6, #0x8]
 *
 * gcc sinks the second vector store past the first argument's load.
 *
 * A WARNING ABOUT READING tryc's LINE COUNT. This screened at "137 against 136,
 * 23 differing", which reads as a one-instruction deficit plus a shifted tail.
 * It is not: the deficit is a free label and the 23 are one transposed store
 * plus the shift it causes. `make compare` was the only way to find that out --
 * the .c was placed, built, and FAILED, and the actual generated .s had to be
 * read side by side with the reference to see it. **When a screen is within a
 * line or two and the diff looks like a pure shift, check for a duplicate label
 * before believing the count**, and read the generated asm rather than the
 * normalised diff.
 *
 * (That test also re-confirmed the batch-172 stale-object trap from the other
 * side: after removing the .c and restoring the .s, `make compare` still failed
 * until `asm/.../rom_944ec_a_c_a_a_c_a_a.o` was deleted by hand. The object is
 * built from whichever source existed last and make will not notice the swap.)
 *
 * MEASURED (rom 136 lines):
 *   the pointer set up as `p = e + 0x40;` before the call    138, 48
 *   the same AFTER the call                                  138, 46
 *   `*(short *)(e + 0x3c) -= 1;` instead of `unsigned short`
 *     (the -1 becomes +0xffff on an unsigned halfword)       137, 28
 *   BLOCK-SCOPED named zeros in the k==1, k==2 and k==3 arms
 *     so the halfword stores use `mov` not a pooled zero     137, 25
 *   the vector component named before the field it updates
 *     (`d = v[0]; t = e->0x14; ... (d - t) / 8`)             137, 23  <- best
 *   a FUNCTION-SCOPED named zero instead of block-scoped     143, 142 (the
 *                          local has to live across the whole body and gcc
 *                          spills -- the same bound the basic-block lever hits)
 *   the two shifted call arguments named before the call     137, 23 (inert)
 *   -fno-schedule-insns                                      137, 23 (inert)
 *   -fno-schedule-insns2                                     137, 41 (worse)
 *
 * THE BLOCK-SCOPED-ZERO RESULT IS THE REUSABLE ONE. Naming a stored value to
 * keep it out of the constant pool is recorded (batch 176, and twice more in
 * ovl_7fa4ec/2008da4.c), but naming it at FUNCTION scope costs six lines here
 * and 119 more differing, because the local outlives every call. Declared
 * inside the arm that uses it, the live range is two instructions and the lever
 * is free. **Scope the name as tightly as the ROM's live range.**
 *
 * WHAT IS RIGHT: the five-way `k` cascade with its cross-jumped `(*p)++` tail;
 * `*(short *)(e + 0x38) = k;` in the k==0 arm, where the ROM stores the switch
 * variable itself because it is already zero there; the signed `/ 8` expansion;
 * the named `gState` base; and the three-int stack vector with `Func_80974d8`
 * taking its address.
 *
 * NEXT: nothing source-level in ten probes.
 */
extern unsigned char gState[];
extern char *MapActor_GetActor(int slot);
extern void Func_80974d8(int *v);
extern void Func_809bb34(char *a);
extern void vec3_translate(int x, int y, int *v);

void Func_8095938(char *e)
{
    unsigned char *g;
    char *a;
    char *p;
    int v[3];
    int k;
    int t;
    int d;

    g = gState;
    a = MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    p = e + 0x40;
    k = *(signed char *)p;
    if (k == 0) {
        *(unsigned short *)(e + 0x3c) += 1;
        *(unsigned short *)(e + 0x3e) += 1;
        if (*(short *)(e + 0x38) == 0x3c) {
            *(short *)(e + 0x38) = k;
            *p = *p + 1;
        }
    } else if (k == 1) {
        int z1 = 0;
        *(unsigned short *)(e + 0x3e) += 1;
        if (*(short *)(e + 0x38) == 0x28) {
            *(short *)(e + 0x38) = z1;
            *p = *p + 1;
        }
    } else if (k == 2) {
        int z2 = 0;
        *(unsigned short *)(e + 0x3e) += 1;
        v[0] = *(int *)(a + 8);
        v[1] = *(int *)(a + 0xc) + (0xa0 << 13);
        v[2] = *(int *)(a + 0x10);
        Func_80974d8(v);
        d = v[0];
        t = *(int *)(e + 0x14);
        *(int *)(e + 0x14) = t + (d - t) / 8;
        d = v[2];
        t = *(int *)(e + 0x18);
        *(int *)(e + 0x18) = t + (d - t) / 8;
        if (*(short *)(e + 0x38) == 0x28) {
            *(short *)(e + 0x38) = z2;
            *p = *p + 1;
        }
    } else if (k == 3) {
        int z3 = 0;
        *(short *)(e + 0x3c) -= 1;
        *(unsigned short *)(e + 0x3e) += 1;
        if (*(short *)(e + 0x38) == 0x3c) {
            *(short *)(e + 0x38) = z3;
            *p = *p + 1;
        }
    } else if (k == 4) {
        Func_809bb34(e);
    }
    v[0] = *(int *)(e + 0x14);
    v[2] = *(int *)(e + 0x18);
    vec3_translate(*(short *)(e + 0x3c) << 16, *(short *)(e + 0x3e) << 11, v);
    *(int *)(e + 4) = v[0];
    *(int *)(e + 8) = v[2];
}
