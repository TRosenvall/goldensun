/* FieldMove_NoTarget (RunRideModeA) -- PARKED at 9 differing of 137 lines
 * (99 ROM instructions).  ref: asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a.s
 * first diff at position 5.  Line count is exact and the 16-arm jump table,
 * its slot order and every case body are exact.
 *
 * THE JUMP TABLE.  Slot i is case (i+1); the case BODIES come out in source
 * order, which here is  1, 7, 11, 4, 5, 14, 6, 3, 12, 13, 9, 2, 8, 10, 15, 16.
 * That order is transcribed from the block layout and is what the file uses.
 *
 * FOUR THINGS THAT WERE LOAD-BEARING:
 *  1. The second global is DERIVED from the first: the ROM has
 *     `ldr r3,=iwram_3001f30 / sub r3,#0x74 / ldr r1,[r3]`.  Declaring a
 *     separate `iwram_3001ebc` gives a second pool entry (2 extra lines);
 *     `*(unsigned char **)((unsigned char *)&iwram_3001f30 - 0x74)` gives the
 *     ROM's subtraction.  Note this is the OPPOSITE of the usual reading --
 *     here the add/sub chain really is in the source.
 *  2. `k = 0xffff;` as an int local before `*(unsigned short *)r = k;`.
 *     Written as a direct literal store gcc emits `ldrh r3, <pool>` (a
 *     HALFWORD pool load), which needs its own mid-function pool and an extra
 *     `b` to jump over it -- 139 lines against 137.  With the int local it is
 *     the ROM's `ldr r3,=0xffff`.  This is round-5 rule 2 in practice.
 *  3. `w = (int *)(gs + (0xfa << 1));` named once and used at both call sites
 *     (the ROM keeps it in r8 across the call).
 *  4. `gs = gState;` as a local base, so `gs + 0x24a` is real arithmetic.
 *
 * THE RESIDUE, all register naming, in two groups:
 *   (a) mode and t are exchanged r6<->r7 unless `t` is assigned BEFORE `mode`
 *       in the source -- but assigning t first also makes the scheduler emit
 *       the `q` load before mode's, so the two loads land one slot late.
 *       All six permutations of the three header assignments were measured:
 *         mode,q,t  11   mode,t,q  13   q,mode,t  11
 *         q,t,mode   9   t,mode,q  13   t,q,mode   9
 *       9 is the floor: with mode assigned last it gets r6 (correct) and the
 *       emission order becomes q,mode,t instead of the ROM's mode,q,t.
 *   (b) in case 9, `mov r3,#0 / ldrsh r0,[r7,r3]` vs our `mov r2,#0 /
 *       ldrsh r3,[r7,r2]` -- the zero-offset register and the result register
 *       are exchanged.  Naming the loaded halfword in a local makes it much
 *       worse (31 and 36 differing).
 */
extern unsigned char *iwram_3001f30;
extern unsigned char gState[];

extern void Field_Move(void);
extern void Field_Lift(void);
extern void Field_Carry(void);
extern void Field_Force(void);
extern void Field_Douse(void);
extern void Field_Whirlwind(void);
extern void Field_Frost(void);
extern void Field_Ply(void);
extern void Field_Growth(void);
extern void Field_Catch(void);
extern void Field_Reveal(void);
extern void Field_Cloak(void);
extern void Field_Retreat(void);
extern void Field_Avoid(void);
extern void Field_Halt(void);
extern void Field_Halt_Target(int a);
extern void Field_MindRead(int a, int b);
extern void Func_809ade8(void);
extern void Func_808df1c(int a, int b);
extern int  Func_809ae3c(void);
extern int  Func_808d5a4(void);
extern void Func_80970f8(int a, int b);
extern void Func_809ad90(int a);
extern void Func_80984c0(void);

void FieldMove_NoTarget(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *gs;
    short *r;
    int *w;
    int mode;
    int t;
    int u;
    int k;

    p = iwram_3001f30;
    t = *(short *)(p + 0x1a);
    q = *(unsigned char **)((unsigned char *)&iwram_3001f30 - 0x74);
    mode = *(short *)(p + 0x1e);
    switch (mode) {
    case 1:
        Field_Move();
        break;
    case 7:
        Field_Lift();
        break;
    case 11:
        Field_Carry();
        break;
    case 4:
        Field_Force();
        break;
    case 5:
        Field_Douse();
        break;
    case 14:
        Field_Whirlwind();
        break;
    case 6:
        Field_Frost();
        break;
    case 3:
        Field_Ply();
        break;
    case 12:
        Field_Growth();
        break;
    case 13:
        Field_Catch();
        break;
    case 9:
        gs = gState;
        r = (short *)(gs + 0x24a);
        if (*r != -1) {
            Func_809ade8();
            k = 0xffff;
            *(unsigned short *)r = k;
        }
        w = (int *)(gs + (0xfa << 1));
        Func_808df1c(*w, mode);
        u = Func_809ae3c();
        if (Func_808d5a4() != 0) {
            Func_80970f8(*w, u);
            Field_Halt_Target(u);
            Func_809ad90(u);
            *r = u;
        } else {
            Field_Halt();
        }
        break;
    case 2:
        if (*(short *)(q + 0xcb8) != 0)
            Func_80984c0();
        Field_MindRead(*(short *)(p + 0x18), t);
        break;
    case 8:
        Field_Reveal();
        break;
    case 10:
        Field_Cloak();
        break;
    case 15:
        Field_Retreat();
        break;
    case 16:
        Field_Avoid();
        break;
    }
}
