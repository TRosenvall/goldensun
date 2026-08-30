/* Cluster Func_80b8b48..Func_80b8b48 extracted from
 * goldensun/asm/rom_b5000/rom_b8228_c_a_c_a_c_c.s.
 *
 * Matched on the FIRST screen, and it is worth saying which reads of the
 * assembly made that happen, because none of them was a lever:
 *
 *   THE EPILOGUE NAMES THE RETURN TYPE.  `pop {r5, r6} / pop {r1} / bx r1`
 *   pops the return address into r1, not r0, which gcc only does when r0 is
 *   carrying a value.  So this is `int`, and the `mov r0, #1 / neg r0` and
 *   `mov r0, #0` at the two exits are `return -1` and `return 0`.
 *
 *   `mov r5, sp` FOLLOWED BY `[r5, #N]` IS A LOCAL STRUCT WHOSE ADDRESS IS
 *   TAKEN.  thumb can address the frame off sp directly, so gcc only puts the
 *   frame base in a register when something needs it as a value -- here
 *   `_Anim_Attack(&s)`.  The 0x54-byte frame and the five offsets it touches
 *   (4, 8, 0x14, 0x1c, 0x24) are that struct's fields.
 *
 *   THE FIELD AT 0x24 IS `short`, READ ONE WAY UNSIGNED.  Two reads are
 *   `ldrsh` off a register offset -- thumb has no immediate form, so that is
 *   just what a signed halfword field costs -- and the third is `ldrh` with an
 *   immediate feeding `cmp #7 / bhi`, an UNSIGNED compare.  A `short` field
 *   with `(unsigned short)` on the one comparison gives all three; making the
 *   field unsigned would need casts on the other two instead.
 *
 *   THE STORE IS WRITTEN IN BOTH ARMS.  `*q = 0x80 << 6` appears in the ROM
 *   before each WaitFrames, not once before the branch.  Hoisting it would emit
 *   one store; the ROM has two.
 */
struct Cmd {
    short f0;
    unsigned char pad02[0xa - 0x2];
    unsigned short fa;
};

struct Anim {
    int f0;
    int f4;
    int f8;
    unsigned char pad0c[0x14 - 0xc];
    int f14;
    unsigned char pad18[0x1c - 0x18];
    int f1c;
    unsigned char pad20[0x24 - 0x20];
    short f24;
    unsigned char pad26[0x54 - 0x26];
};

extern int *iwram_3001f00;

extern void WaitFrames(int n);
extern int Func_80b8808(int a);
extern void _GetUnit(int id);
extern void Random(void);
extern void _Func_8019908(int a, int b);
extern void _Func_80175a0(int a);
extern void Func_80b82c4(int a, int b, int c, int d);
extern int *GetBattleActor(int a);
extern void _Actor_SetAnimSpeed(int a, int b);
extern void _Anim_Attack(struct Anim *s);
extern void Func_80b8000(int a);

int Func_80b8b48(struct Cmd *c)
{
    struct Anim s;
    int *q;

    q = iwram_3001f00;
    if (*q == (0x80 << 6)) {
        *q = 0x80 << 6;
        WaitFrames(0xa);
    } else {
        *q = 0x80 << 6;
        WaitFrames(0x1e);
    }
    s.f8 = c->f0;
    if (Func_80b8808(s.f8) < 0)
        return -1;
    s.f24 = c->fa;
    if (Func_80b8808(s.f24) < 0)
        return -1;
    _GetUnit(s.f8);
    _GetUnit(s.f24);
    Random();
    _Func_8019908(s.f8, 1);
    _Func_80175a0(0x814);
    Func_80b82c4(s.f8, s.f24, 0xd, 0);
    _Actor_SetAnimSpeed(*GetBattleActor(s.f8), 0x10);
    GetBattleActor(s.f24);
    s.f14 = 1;
    if ((unsigned short)s.f24 <= 7)
        s.f4 = 1;
    else
        s.f4 = 0;
    s.f1c = 0;
    WaitFrames(4);
    _Anim_Attack(&s);
    Func_80b8000(s.f24);
    Func_80b8000(s.f8);
    return 0;
}
