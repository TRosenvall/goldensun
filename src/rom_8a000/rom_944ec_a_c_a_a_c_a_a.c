/* Func_8095938 (0x08095938) -- EXACT.  batch 321, brief A, target 3.
 *
 * ref: asm/rom_8a000/rom_944ec_a_c_a_a_c_a_a.s
 * install: src/rom_8a000/rom_944ec_a_c_a_a_c_a_a.c
 *
 * objcmp: OK -- 268 bytes, 129 encodings and 5 relocations identical, --func
 * AND --whole.  PINS 0.  NO shims: no register pins, no inline asm, no
 * barriers, no volatile, no .equ, default flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_944ec_a_c_a_a_c_a_a.c \
 *     asm/rom_8a000/rom_944ec_a_c_a_a_c_a_a.s --func Func_8095938
 *
 * SPLIT SHAPE: NONE.  `grep -c func_start` = 1 and tools/datacheck.py prints
 * nothing for this file (no data section, no labels), so this is a WHOLE-FILE
 * conversion: write the .c, delete the hand .s.  No exports, no linker change.
 *
 * WHAT WAS THE RESIDUE, AND WHAT CLOSED IT -------------------------------------
 *
 * Parked at 3 of 129 (src/non_matching/rom_8a000/8095938.c), diagnosed there as
 * "scheduling -- ONE store moved two instructions late" and declared "nothing
 * source-level in ten probes".  The observation was right and the verdict wrong.
 * The ROM's tail is
 *
 *     ldr r3,[r5,#0x14] / str r3,[r6]  / ldr r3,[r5,#0x18] / str r3,[r6,#8]
 *     mov r2,#0x3c      / ldrsh r0,[r5,r2]
 *
 * and gcc sank `str r3,[r6,#8]` past `mov r2,#0x3c / ldrsh`.  The two `str`s are
 * int stores into the stack vector `v`; the `ldrsh` reads `e+0x3c` through a
 * `short *`.  Those are two DISTINCT NON-ZERO alias sets, they do not conflict,
 * so sched2 has no dependence to stop it and sinks the store.
 *
 * Reading ONE of the two halfword arguments through a union member makes that
 * read ALIAS SET 0, which conflicts with everything -- the anti-dependence
 * against the `v` stores comes back and the store stays where the ROM has it.
 *
 * THIS IS NOT A NEW CONSTRUCT.  It is lever 5 of the LANDED, EXACT
 * src/rom_8a000/rom_9a44c_c_c_a_a.c (Field_Whirlwind), in this same subsystem,
 * which already carries this exact `union blob` declaration for this exact
 * reason.  The two files now read alike.
 *
 * WHICH END OF THE PAIR, AND IT IS NOT ARBITRARY.  Whirlwind's header says
 * "either end of the pair works".  HERE IT DOES NOT, and the crossed table says
 * so (tools/crossfire.py, 7 edits, depth 2):
 *
 *     0   union on the e+0x3e read                     <= this file
 *     0   union on e+0x3e + union on e+0x3c
 *     0   union on e+0x3e + union on the v[0] store
 *     3   BASE
 *     3   union on the v[0] store            exactly inert
 *     3   union on the v[2] store            exactly inert
 *     3   union on v[2] + union on v[0]      exactly inert
 *     3   union on e+0x3e + union on v[2]    <- u:v2 CANCELS the fix
 *     4   union on e+0x3e + swapped tail stores
 *     6   union on the e+0x3c read           <- TWICE the base
 *     6   swapped tail stores
 *    10   union on e+0x3c + swapped tail stores
 *   ~21   volatile on the e+0x3c read  (127 insns, RELOC COUNT MEM: not a distance)
 *
 * e+0x3c is the first vec3_translate argument AND is read-modified in three of
 * the five `k` arms; alias set 0 there drags those earlier accesses into the
 * conflict graph and costs twice the base.  e+0x3e is read only here.  And an
 * alias-set-0 STORE is inert alone and CANCELS the fix when crossed, because it
 * conflicts with everything including the load we just made conflict.  So the
 * rule is directional: PUT THE UNION ON THE ACCESS WHOSE DEPENDENCE YOU WANT,
 * NOT MERELY ON THE SAME OBJECT.  `volatile` does not substitute -- it changes
 * the instruction count -- confirming Whirlwind's "only the union is alias set 0".
 *
 * EVERYTHING THE PARK CREDITED IS KEPT: the five-way `k` cascade with its
 * cross-jumped `(*p)++` tail; `*(short *)(e + 0x38) = k;` in the k==0 arm; the
 * BLOCK-SCOPED named zeros z1/z2/z3, which keep the halfword stores on `mov` and
 * whose live ranges stay two instructions ("scope the name as tightly as the
 * ROM's live range"); the signed `/ 8` expansion; the named `gState` base; and
 * the three-int stack vector passed by address.
 */
union blob { unsigned char *pp; short hh; int ii; };
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
    vec3_translate(*(short *)(e + 0x3c) << 16, ((union blob *)(e + 0x3e))->hh << 11, v);
    *(int *)(e + 4) = v[0];
    *(int *)(e + 8) = v[2];
}
