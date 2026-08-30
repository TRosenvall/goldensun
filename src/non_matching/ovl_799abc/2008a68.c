/*
 * OvlFunc_905_2008a68 -- asm/overlays/rom_799abc/ovl_30_a_a_c.s
 *
 * BLOCKER: scheduling of two independent chains. 110 lines against 110, 12
 * differing, first difference at 77. Everything to instruction 76 is exact.
 *
 *      rom   ldrb r1,[r7,#9] / mov r3,#0xd / neg r3,r3 / and r3,r1 / orr r3,r2
 *      ours  mov r1,#0xd / ldrb r3,[r7,#9] / neg r1,r1 / and r1,r3 / orr r1,r2
 *
 * The ROM loads the byte first and builds the mask second; we do the reverse.
 * The register numbering follows -- whichever chain is emitted second gets r3 --
 * and the same five instructions differ again in the other switch arm.
 *
 * TRIED AND REJECTED, all measured: a named mask (12); the mask assigned once
 * before the switch (35); at function top (101); reversed `|` operands (35);
 * `mask & q[9]` rather than `q[9] & mask` (12); an int temp for the load (36);
 * an unsigned char temp (36); splitting the read-modify-write (13 -- this stops
 * the narrowing but not the ordering); the mask inline as -13 (35, and it
 * narrows to `mov r3,#0xf3` exactly as documented); an unsigned char for the OR
 * operand (26); mask declared first (12); a struct for both sides (62), for the
 * read only (12), for read and store (12); volatile on the read (12) and on the
 * pointer (12). Flags: --no-sched2 34, -fno-schedule-insns 12, --no-rerun-cse 12.
 *
 * SETTLED, and each was worth a lot: the `cmp #0 / beq / cmp #3 / bhi` tree is a
 * SWITCH with `case 0:` and `case 1: case 2: case 3:` -- written as
 * `if (h == 0) ... else if (h <= 3)` gcc lays the blocks out the other way round
 * (44 to 38 differing). The register-offset load needs the base-first pointer
 * form `*(int *)(vp + ((f & 0xf) << 2))`, not `vp[f & 0xf]`. And 38 to 12 came
 * from writing the other `|` operand as its own statement first in each arm,
 * which stops gcc cross-jumping the `orr` into the join.
 */
extern unsigned char L160c[] __asm__(".L160c");

extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __Actor_SetScript(unsigned char *e, int s);
extern void __Func_80929d8(unsigned char *e, int n);
extern void OvlFunc_905_2008a00(void);

struct S3 { int a; int b; int c; };

void OvlFunc_905_2008a68(int p0, int p1, int p2, int p3,
                         int e, unsigned int f, unsigned int g)
{
    int v[3];
    char *vp;
    unsigned char *a0;
    unsigned char *ent;
    unsigned char *q;
    unsigned int h;
    int mask;
    int u;

    a0 = __MapActor_GetActor(0);
    vp = (char *)v;
    *(struct S3 *)vp = *(struct S3 *)L160c;
    ent = __CreateActor(0xde, p0, p1, p2);
    if (ent != 0) {
        q = *(unsigned char **)(ent + 0x50);
        __Actor_SetAnim(ent, (f + 1) & 0xf);
        __Actor_SetScript(ent, *(int *)(vp + ((f & 0xf) << 2)));
        __Func_80929d8(ent, (f >> 16) & 0xf);
        ent[0x55] = 0;
        q[0x26] = 0;
        *(void **)(ent + 0x6c) = (void *)OvlFunc_905_2008a00;
        *(int *)(ent + 0x30) = p3;
        *(int *)(ent + 0x34) = e;
        *(short *)(ent + 0x66) = (short)g;
        h = g >> 16;
        switch (h) {
        case 0:
            u = (*(unsigned char **)(a0 + 0x50))[9] & 0xc;
            mask = -13;
            q[9] = (q[9] & mask) | u;
            break;
        case 1:
        case 2:
        case 3:
            ent[0x23] &= 0xfe;
            h &= 3;
            mask = -13;
            q[9] = (q[9] & mask) | (h << 2);
            break;
        }
    }
}
