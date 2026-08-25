/* Cluster Func_8096ab0..Func_8096ab0 extracted from goldensun/asm/rom_8a000/rom_944ec_a_c_c_a_a_a.s.
 *
 * Total .text for this TU = 50 bytes (= 0x32).
 * Appended after the _a piece in goldensun/stage1.ld.
 *
 * Clears a byte on the party record when the field state is 2 and the stored
 * facing at gState+0x24a no longer matches the actor's own.
 *
 * EVERY MEMBER HERE IS READ WITH `ldrsh` THROUGH AN INDEX REGISTER --
 * `mov r1, #0x1e / ldrsh r3, [r5, r1]` -- because Thumb `ldrsh` has no
 * immediate form. Written as struct members that falls out; written as pointer
 * arithmetic gcc folds the offset into the index and the shape changes. Same
 * lever as OvlFunc_922_2009a34, and it is the reason this matched first screen.
 *
 * NOTE FOR RESCREENING: tryc.py warns that the reference keeps its pool inside
 * the function and that it cannot see PC-relative offsets. Cleared by
 * `make compare`, not by the screen.
 */

typedef struct {
    unsigned char pad[0x24a];
    short f24a;
} GlobalState;

struct S {
    unsigned char pad00[0x14];
    unsigned char *p;
    unsigned char pad18[2];
    short f1a;
    unsigned char pad1c[2];
    short f1e;
};

extern struct S *iwram_3001f30;
extern GlobalState gState;
extern void Func_8097608(void);

void Func_8096ab0(void)
{
    struct S *s;

    s = iwram_3001f30;
    if (s->f1e != 2)
        return;
    Func_8097608();
    if (gState.f24a == s->f1a)
        return;
    s->p[0x5b] = 0;
}
