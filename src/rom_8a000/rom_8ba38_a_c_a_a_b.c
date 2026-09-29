/* Func_808ce74 (0x0808ce74) -- 121 encodings, 260 bytes.
 *
 * objcmp reports ONE differing word against the reference and a relocation
 * mismatch, and both are the same artefact of comparing UNLINKED objects: the
 * reference leaves 0 at 0xfc with an R_ARM_ABS32 against `ewram_2020000`
 * (wram.sym:137 = 0x02020000), where we place the value directly.  The REL
 * addend is 0, so the linker writes exactly 0x02020000 there and the LINKED
 * bytes are identical -- which is what `make compare` gates on, and it is green.
 *
 * The literal spelling is not cosmetic.  With the `ewram_2020000` symbol the
 * load is a local-alloc pseudo and consumes no step of reload's spill-register
 * round robin (reload1.c:5003 starts at last_spill_reg, 4937 advances it); with
 * the literal, reload materialises the constant, the rotation advances one, and
 * the else-arm's `/ 0x100000` bias wraps back to r0 -- the ROM's `ldr r0 /
 * add r3,r0` instead of `ldr r2 / add r3,r2`.  Worth 2 -> 1.  Same spelling as
 * landed src/rom_9000/rom_1219c_a_a_b.c:3-4.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
/* ewram_2020000 as a LITERAL, not the wram.sym symbol: see the header.  The
 * symbol form is a local-alloc pseudo and costs the else-arm's reload register.
 * Same spelling as landed src/rom_9000/rom_1219c_a_a_b.c:3-4. */
#define ewram_2020000 ((unsigned char *)0x02020000)
extern void vec3_translate(int a, int b, int *v);
extern struct A *GetFieldActor(int id);
extern int _Func_8011f54(int a, int x, int z);
extern int FindMapActorEvent(int a, int b);

struct A {
    unsigned char pad00[6];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
};

int Func_808ce74(void)
{
    int v[3];
    int *bp;
    unsigned char *g;
    unsigned char *p;
    unsigned char *q;
    struct A *e;
    unsigned char *t;
    int code;
    int result;
    int h;

    g = gState;
    result = 0;
    e = GetFieldActor(*(int *)(g + (0xfa << 1)));
    p = iwram_3001ebc;
    q = *(unsigned char **)((char *)&iwram_3001ebc - 0x4c);
    if (e != 0) {
        bp = v;
        bp[0] = e->f08;
        bp[1] = e->f0c;
        bp[2] = e->f10;
        vec3_translate(0x80 << 13, e->f06, bp);
        if (*(short *)(p + (0xcf << 1)) == 3) {
            t = ewram_2020000 + (((bp[0] / 0x200000) & 0x1f) + (((bp[2] / 0x200000) & 0x1f) << 5)) * 4;
        } else {
            t = *(unsigned char **)(q + (0x98 << 1));
            t += ((bp[0] / 0x100000) + ((bp[2] / 0x100000) << 7)) * 4;
        }
        code = t[2];
        if ((unsigned int)(code - 0xf2) <= 5) {
            h = _Func_8011f54(e->f22, v[0], v[2]);
            if (h >= e->f0c && h <= e->f0c + 0x400000)
                result = code;
        } else if (FindMapActorEvent(3, code) != 0) {
            result = code;
        }
    }
    return result;
}
