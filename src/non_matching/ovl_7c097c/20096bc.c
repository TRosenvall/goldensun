/* OvlFunc_936_20096bc -- NON-MATCHING, 4 differing STREAM encodings of 125.
 *
 * FIGURE, re-derived with objcmp at the head of batch 339 (the authority):
 *   park body  7 differing STREAM encodings of 125 (ref 125 / ours 125);
 *              99 / 99 16-bit instructions -- IN PHASE, a true DISTANCE;
 *              relocations IDENTICAL.
 *   THIS body  4 differing STREAM encodings of 125 (ref 125 / ours 125);
 *              99 / 99 16-bit instructions; relocations IDENTICAL;
 *              first diff at index 58.  PIN-FREE, DEVICE-FREE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7c097c/20096bc.c \
 *     asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_c.s --func OvlFunc_936_20096bc
 *
 * PIECE (tools/piece.py, verbatim):
 *   asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_c.s  --  1 function(s)
 *       PARKED  OvlFunc_936_20096bc   7 of 125   src/non_matching/ovl_7c097c/20096bc.c
 *     => all 1 parked, so ONE candidate converts the whole TU.  No split needed.
 *
 * THE PARK'S DIAGNOSIS WAS RIGHT ABOUT THE CLASS AND HALF-WRONG ABOUT THE REACH.
 * It called the residue "the commutative register-role class ... the genuine
 * article" and recorded two chains, both with the mask constant and the loaded
 * byte in exchanged registers.  ONE OF THE TWO CHAINS HAS A SOURCE HANDLE.
 *
 * WHAT CLOSED, chain 1, 3 of the 7 encodings:
 *   In Thumb `and Rd, Rs` is destructive, so WHICH of the two values ends up in
 *   the destination register is decided by the tie-up of the pattern's `%0`
 *   operand -- and that tie-up is regmove's, not the allocator's.  regmove_optimize
 *   (regmove.c:1129-1215) runs find_matches, then for a COMMUTATIVE pattern
 *   checks the other operand at regmove.c:1198-1205 and keeps what it has when
 *   `operands_match_p (comm, dst)` and
 *   `replacement_quality (comm) >= replacement_quality (src)`;
 *   replacement_quality (regmove.c:341-368) returns 3 for a pseudo that was not
 *   copied from another register, 2 for one copied from a pseudo, 1 from a hard
 *   register.  Both of this chain's pseudos score 3, so the tie goes to whatever
 *   already matches the destination -- i.e. TO THE DESTINATION VARIABLE'S PSEUDO.
 *   So the lever is which C variable receives the result:
 *       park   v  = m1 & v;    ->  byte in the dest reg  (ours)
 *       here   m1 = m1 & v;    ->  MASK in the dest reg  (the ROM)
 *   `m1 = m1 & v;` is byte-exact for chain 1, all of
 *   `mov r2,#0xd / ldrb r3,[r6,#9] / neg r2,r2 / and r2,r3`.
 *   The park's negative "naming both mask constants, with the constant assigned
 *   before the load (9, no change)" varied WHERE the constant was set and never
 *   WHICH VARIABLE WAS ASSIGNED THE RESULT -- the same shape of miss as batch
 *   336's `fx >>= 16`.
 *
 * CHAIN 2 IS BOUNDED, and the bound comes with its measurement:
 *   The identical lever on chain 2 (`m2 = m2 & u; s->f05 = m2;`) measures
 *   76 of 125 -- and the reason is a LENGTH change, not a role change:
 *   99 instructions against 98, SIZE 296 against 300.  Chain 2's result is
 *   stored straight into the `unsigned char` field it came from, so once the
 *   mask variable is also the stored value combine narrows the AND to QImode
 *   and the ROM's `mov r3,#0x21 / neg r3,r3` collapses into one `mov r3,#0xdf`.
 *   THAT IS THE PARK'S OWN FINDING #3 RUNNING BACKWARDS: it needed an int
 *   intermediate to keep -0x21 in SImode, and the mask-in-dest spelling destroys
 *   exactly that intermediate.  So for chain 2 the two requirements -- SImode
 *   mask materialisation, and mask-in-destination -- are in direct conflict in
 *   every spelling measured, and no C-level handle separates them.
 *   (objcmp also prints `RELOCATIONS differ` on those variants; THAT IS NOT A
 *   SECOND FINDING.  The symbol sequence is identical and only the addresses
 *   shift, because the function is one instruction shorter.  batch 298's rule.)
 *
 * THE REMAINING 4 ARE CHAIN 2, and the AND INSTRUCTION ITSELF IS IDENTICAL:
 *      rom   mov r3,#0x4 / ldrb r1,[r6,#5] / orr r2,r3 / mov r3,#0x21 / neg r3,r3 / and r3,r1
 *      ours  mov r3,#0x4 / orr r2,r3 / mov r1,#0x21 / ldrb r3,[r6,#5] / neg r1,r1 / and r3,r1
 *   Both end `and r3, r1`; only which value is in r3 and which in r1 differs,
 *   plus the position of the `ldrb`.  Chain 1 and everything else in the
 *   function, prologue and epilogue included, is identical.
 *
 * MEASURED INERT OR WORSE on top of the chain-1 fix (differing encodings of 125):
 *   chain 2 with a THIRD variable receiving the AND (`w = m2 & u; s->f05 = w;`)
 *     and with the operands written both ways: 4;
 *   chain 2 with the byte made a COPY of another pseudo so replacement_quality
 *     drops it to 2 (`u = s->f05; u2 = u; w = m2 & u2;`): 4 -- the quality rung
 *     is reachable in principle and does not move this insn;
 *   the mask made a copy instead (`m3 = m2; w = m3 & u;`): 4;
 *   `s->f05 = m2 & u;` with no intermediate at all: 4;
 *   `w = m2; w = w & u;`: 4;  `w = m2 & u; u = w; s->f05 = u;`: 4;
 *   the mask built by explicit negation (`m2 = 0x21; m2 = -m2;`): 4;
 *   the second byte load HOISTED to 1-3 statements earlier, which is what the
 *     ROM's interleave looks like, at every position (the park reported this
 *     WORSE at 61; on this body it is 4 or 14, never better) -- 36 placements;
 *   chain 1's operands written in the reverse order (`v = v & m1;`): 7, i.e.
 *     the park's inert result reproduces and the written order is NOT the axis.
 *
 * AUTOSWEEP on the PARK body (recorded negative, do not re-screen):
 *   FLAG AXIS: -fno-gcse / -fno-strict-aliasing / -fno-strength-reduce /
 *     -fno-rerun-cse-after-loop / -fno-rerun-loop-opt all inert at 7;
 *     -fno-schedule-insns2 27, -O1 27, -ffixed-r7 121.
 *   ANCHOR AXIS: 39 top-level boundaries, all inert or worse, best 7.
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
        m1 = m1 & v;
        m1 = m1 | 4;
        m2 = -0x21;
        u = s->f05;
        u = m2 & u;
        s->f05 = u;
        m1 = m1 & 0xf;
        s->f09 = m1;
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

