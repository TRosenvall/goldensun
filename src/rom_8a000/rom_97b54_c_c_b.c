/* Cluster Func_8099d18..Func_8099d18 extracted from
 * goldensun/asm/rom_8a000/rom_97b54_c_c.s.
 *
 * Spawns one particle at a jittered offset from a source entity.  Two screens:
 * the body was right first time and the only defect was a narrow store.
 *
 * THE HALFWORD STORE NEEDS A TYPED FIELD.  `*(short *)(a + 0x5e) = 0xc` through
 * a cast compiles to `ldr r3, =0xc` -- gcc pools a literal an eight-bit `mov`
 * would build -- where the ROM has `mov r3, #0xc`.  Declaring the actor a struct
 * with a `short` field at 0x5e gives the mov form in a scratch register at no
 * cost.  2 differing -> exact.  That is the batch-150 rule, and this is the
 * third function it has decided; the byte store at 0x55 and the int store at
 * 0x48 were already right either way, so it really is specific to the halfword.
 *
 * SETTLED WITHOUT A LEVER: the three Random() calls.  The first feeds
 * `s->fc - (Random() << 4) + (0xc0 << 13)`; the second and third are the two
 * arguments of vec3_translate, and gcc splits `Random() * 48` into `(x*3) << 4`
 * across the intervening call exactly as the ROM does -- `lsl r5, r0, #1 /
 * add r5, r0` before the third call and `lsl r5, #4` after it.  Writing the
 * multiply plainly is enough; nothing needs naming.
 *
 * The local `int v[3]` has its address taken by vec3_translate, which is why
 * the ROM copies sp into a callee-saved register and addresses the slots off
 * it rather than off sp.
 *
 * The reference keeps its literal pool inside the function, so tryc.py cannot
 * see PC-relative distance; verified with make compare.
 */
struct Src {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct Actor {
    unsigned char pad00[0x48];
    int f48;
    unsigned char pad4c[0x55 - 0x4c];
    unsigned char f55;
    unsigned char pad56[0x5e - 0x56];
    short f5e;
};

extern unsigned char *iwram_3001f30;
extern unsigned char Data_9f0b0[];

extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Actor *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetScript(struct Actor *a, void *s);

void Func_8099d18(void)
{
    int v[3];
    struct Src *s;
    struct Actor *a;

    s = *(struct Src **)(iwram_3001f30 + 0x14);
    v[0] = s->f8;
    v[1] = s->fc - (Random() << 4) + (0xc0 << 13);
    v[2] = s->f10;
    vec3_translate(Random() * 48, Random(), v);
    a = CreateParticleActor(0x11d, v[0], v[1], v[2]);
    if (a != 0) {
        a->f55 = 2;
        a->f48 = 0x1999;
        _Actor_SetAnim(a, 0);
        a->f5e = 0xc;
        _Actor_SetScript(a, Data_9f0b0);
    }
}
