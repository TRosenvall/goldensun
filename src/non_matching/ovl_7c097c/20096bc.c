/* OvlFunc_936_20096bc -- NON-MATCHING, 7 differing STREAM encodings of 125.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Q96bc_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 125 / ours 125 stream encodings; 99/99 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7c097c/20096bc.c \
 *     asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_c.s --func OvlFunc_936_20096bc
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
struct Sprite {
    unsigned char pad0[5];
    unsigned char f05;
    unsigned char pad1[3];
    unsigned char f09;
    unsigned char pad2[0x12];
    unsigned char f1c;
    unsigned char pad3[0xa];
    unsigned char f27;
};

struct Actor {
    unsigned char pad0[8];
    int f08;
    int f0c;
    int f10;
    unsigned char pad1[0x1c];
    int f30;
    unsigned char pad2[4];
    int f38;
    int f3c;
    int f40;
    unsigned char pad3[0xc];
    struct Sprite *sprite;
    unsigned char pad4[1];
    unsigned char f55;
    unsigned char pad5[6];
    unsigned char f5c;
    unsigned char pad6[4];
    unsigned char f61;
};

extern unsigned char gState[];

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092950(int a, int b);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int n);
extern void __StartTask(void (*fn)(void), int n);
extern void OvlFunc_936_200b90c(void);

void OvlFunc_936_20096bc(void)
{
    struct Actor *a;
    struct Sprite *s;
    unsigned char *p;
    int f;
    int off;
    int v;
    int x, y;
    int u;
    int m1, m2;

    if (__GetFlag(0x941)) {
        __SetFlag(0x321);
        __SetFlag(0x913);
        __SetFlag(0x912);
        __SetFlag(0x915);
    }
    if (__GetFlag(0x94 << 4))
        __SetFlag(0x321);
    off = 0xe1 << 1;
    x = 0xd4 << 17;
    y = 0xb0 << 15;
    if (*(short *)(gState + off) == 0xe)
        __MapActor_SetPos(0x19, x, y);
    __Func_8092950(0x15, 2);
    f = __GetFlag(0x916);
    if (f) {
        __MapActor_SetPos(0x1a, 0, 0);
    } else {
        a = __MapActor_GetActor(0x1a);
        s = a->sprite;
        m1 = -13;
        v = s->f09;
        v = m1 & v;
        v = v | 4;
        m2 = -0x21;
        u = s->f05;
        u = m2 & u;
        s->f05 = u;
        v = v & 0xf;
        s->f09 = v;
        s->f27 = f;
        a->f5c = 1;
        a->f55 = f;
        a->f0c = 0xa0 << 12;
        a->f61 = 1;
        p = __galloc_iwram(0x11, 0xc1 << 3);
        __LoadItemIcon(0xb5);
        __UploadSpriteGFX(s->f1c, 0x80, p + (0x80 << 3));
        __gfree(0x11);
        a->f30 = f;
        a->f38 = a->f08;
        a->f3c = a->f0c;
        a->f40 = a->f10;
        __StartTask(OvlFunc_936_200b90c, 0xc8 << 4);
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_936_20096bc -- 0x020096bc, asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_c.s
 *
 * 121 of 121 lines, SEVEN differing, down from 65.
 * Candidate: scratch/Q96bc_best.c.
 *
 * FOUR THINGS CARRIED IT AND ARE WORTH REUSING:
 *
 *   1. BOTH SIDES TYPED.  The actor and the sprite it points at (actor+0x50) are
 *      declared as structs with named fields rather than reached by pointer
 *      arithmetic.  Every one of the eleven field accesses reproduces.
 *   2. The guarded __MapActor_SetPos interleave, fixed by naming its two split
 *      builds in the dominating block.  First difference moved 34 -> 57.
 *   3. AN INT INTERMEDIATE FOR THE SECOND NEGATED MASK, worth 53 lines by
 *      itself (62 -> 9).  `s->f05 = s->f05 & -0x21` truncates to a byte and gcc
 *      emits one `mov r3, #0xdf`; the ROM builds -0x21 in SImode with
 *      `mov r3,#0x21 / neg r3,r3`.  I had applied this to the OTHER mask chain
 *      in the same function and missed this one -- the tell is a single mov
 *      where the ROM has mov+neg, and it also costs a line.
 *   4. Dropping __UploadSpriteGFX's prototype, which pushes its r0 argument to
 *      the end where the ROM has it.  9 -> 7.
 *
 * BLOCKER: the two mask chains.  Register roles are exchanged --
 *      rom   mov r2,#0xd / ldrb r3,[r6,#9] / neg r2,r2
 *      ours  mov r3,#0xd / ldrb r2,[r6,#9] / neg r3,r3
 * -- and the ROM INTERLEAVES the second chain's load (`ldrb r1,[r6,#5]`) into
 * the middle of the first chain, where ours keeps the two chains sequential.
 *
 * TRIED: naming both mask constants, with the constant assigned before the load
 * (9, no change); reordering the source so the second read happens early, which
 * is what the ROM's interleave looks like -- WORSE, 120 lines and 61 differing.
 *
 * The residue is the commutative register-role class, which batch 136 showed can
 * be a typing problem in disguise.  Here the typing is already done, so this is
 * the genuine article rather than a mislabelled one.
 *)

*/
#endif
