/* Func_80b6f44  --  0x080b6f44, from asm/rom_b5000/rom_b6f44.s.
 *
 * NON-MATCHING, 422 of 440 encodings differ.
 *
 * SPLIT: tools/datacheck.py says this file needs a TEXT/DATA SPLIT --
 *   asm/rom_b5000/rom_b6f44.s
 *       data sections : .rodata
 *       Func_80b6f44 reads .Lc5938
 *       *** SPLIT MUST EXPORT: .global .Lc5938
 * so the four rodata bytes (`.Lc5938: .incrom 0xc5938, 0xc593c`) must stay in
 * their own object carrying `.global .Lc5938`, and the .c reaches them through
 *     extern unsigned char Lc5938[] __asm__(".Lc5938");
 * EXPORTS: .Lc5938 only.  Func_80b6f44 is the file's only function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b6f44.c \
 *     asm/rom_b5000/rom_b6f44.s --func Func_80b6f44
 *   -> XX SIZE  ref 1228 bytes, ours 1232
 *      XX ENCODINGS differ in 422 place(s) (ref 440, ours 442)
 *      XX RELOCATIONS differ          <- offsets only
 *
 * Shims: NONE (tools/shimcount.py reports 0).
 *
 * THE RELOCATION SEQUENCE IS EXACT, all 158 entries: five named calls, then the
 * 148 `.text` entries of the two jump tables in the ROM's order, then
 * GetWeaponSpriteID / _Func_8078870 / atan2 / _Actor_SetScript / .Lc5938.  That
 * is the load-bearing readout here -- it says BOTH switch tables, every case
 * grouping and the data export are right, and that the residue is all register
 * allocation.  Only 100 of 483 instructions now sit in disagreeing regions.
 *
 * WHAT CLOSED IT, by pass:
 *
 * 1. THE FIRST SWITCH NEEDS `case 0:` AND `case 4:` WRITTEN OUT.  With only the
 *    four cases 1/2/3/5 plus `default`, gcc has 4 case labels, which is below
 *    CASE_VALUES_THRESHOLD, and emits a COMPARISON TREE (`cmp #2 / beq / bgt /
 *    sub r1, #1 / ...`).  The ROM has a six-entry table whose slots for 0 and 4
 *    point at the default block, so those two labels are in the source, grouped
 *    with `default`.  Writing them fixed the entire rom[57..390] region -- the
 *    whole 140-entry second table came into agreement at the same time.
 *
 * 2. `if (unit > 7)` IS UNSIGNED.  The ROM has `cmp r3, #7 / bls`, so the
 *    parameter is `unsigned int`, not `int`.
 *
 * 3. THE FIVE ZERO STORES AND THE 0x1fe STORE NEED INT-TYPED LOCALS.  A literal
 *    `0` or `0x1fe` through a `short *` gives `ldr r3, =0x0` / `ldr r3, =0x1fe`
 *    -- gcc-2.96 has no immediate alternative for an HImode constant
 *    (docs/elevation.md).  The ROM has ONE `mov r5, #0` covering
 *    a+8/a+0x20/a+0x24/a+0x28/a+0x2a and `mov r3, #0xff / lsl r3, #1` for a+0xa,
 *    so one int local for the zero and one for 0x1fe.  Same instrument for the
 *    0xf0/0xf1 arms: `z2 = 0xf0; *(short *)(a + 4) = z2 << 1;` gives the ROM's
 *    `mov r1, #0xf0` + shared `lsl r1, #1`, where a literal `0xf0 << 1` gives
 *    `ldr r3, =0x1e0`.
 *
 * 4. THE atan2 BIAS MUST BE ADDED THROUGH AN INT LOCAL.  `*(short *)(act + 6) =
 *    atan2(...) + (0x80 << 8)` folds the addend to `ldr r3, =0xffff8000` because
 *    the result is truncated to HImode and -0x8000 == +0x8000 there.  Two
 *    statements (`e = atan2(...); e += 0x80 << 8;`) give the ROM's
 *    `mov r3, #0x80 / lsl r3, #8 / add r0, r3`.
 *
 * 5. The two `if/else` arms assigning 0xf0 and 0xf1 must be written as two FULL
 *    blocks, each with its own store to a+4 and a+6.  Assigning a shared
 *    variable lets jump.c turn the pair into `0xf0 + (x != 0)`
 *    (`neg / orr / lsr #31 / add`), which the ROM does not have; the duplicated
 *    form is tail-merged back into the ROM's shared `lsl r1, #1`.
 *
 * BLOCKER, by pass: global.c register allocation, TWO PAIRWISE SWAPS.
 *
 *   ROM   r5 = &u[0x128]   r6 = a    r7 = w   r8 = act   r10 = u
 *   ours  r5 = a           r6 = 0/&u[0x128]   r7 = w/&u[0x128]   r8 = u   r10 = act
 *
 * REG_ALLOC_ORDER is {...,4,5,6,7,8,10,9,11}, so this says the ROM's priority
 * order is  &u[0x128] > a > w > act > u  where ours is  a > ... > u > act.  Both
 * swaps are pure encoding -- `mov r8, r0` and `mov r10, r0` are the same size --
 * which is why the count is within 2 while 422 encodings differ.  Two residues
 * feed the live lengths that decide it:
 *   - the ROM rematerialises its zero TWICE (`mov r5, #0` for the batch, a fresh
 *     `mov r3, #0` for the a+6 store) and recomputes `&u[0x128]` a second time;
 *     ours keeps one of each alive in a register.
 *   - `if (u[0x129] == 0) f = 0x14ccc; else f = 0x80 << 9;` -- ours hoists the
 *     pool load above the branch and inverts it (4 instructions); the ROM keeps
 *     `bne / ldr =0x14ccc / b / mov / lsl` (5).
 *
 * This is the closest of the three and the only residue is the register map.
 */
extern unsigned char *_CreateActor(int a, int x, int y, int z);
extern unsigned char *_GetUnit(int n);
extern int Func_80b6d30(int n);
extern int Func_80c2384(int n);
extern int Func_80c23a0(int n);
extern int GetWeaponSpriteID(int n);
extern int _Func_8078870(unsigned char *u, int n);
extern int atan2(int a, int b);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern unsigned char Lc5938[] __asm__(".Lc5938");

void Func_80b6f44(unsigned char *a, unsigned int unit, int x, int z)
{
    unsigned char *act;
    unsigned char *u;
    int w;
    int spr;
    int flag;
    int k;
    int c;
    int d;
    int e;
    int f;
    int z0;
    int z1;
    int z2;
    int z3;
    int z4;
    int z5;

    act = _CreateActor(0xf0 << 8, x << 16, 0, z << 16);
    u = _GetUnit(unit);
    flag = 0;
    w = Func_80b6d30(unit);
    if (u[0x129] == 0) {
        spr = Func_80c2384(u[0x128]);
        if (w != 0)
            spr = w;
        else
            flag = Func_80c23a0(u[0x128]);
    } else {
        switch (u[0x128]) {
        case 1:
            spr = 0x12d;
            break;
        case 3:
            spr = 0x12f;
            break;
        case 2:
            spr = 0x97 * 2;
            break;
        case 5:
            spr = 0x131;
            break;
        case 0:
        case 4:
        default:
            spr = 0x96 * 2;
            break;
        }
        if (unit > 7)
            flag = 1;
    }
    *(int *)(a + 0x18) = 0x80 << 9;
    switch (u[0x128]) {
    case 0x4e:
        *(int *)(a + 0x18) = 0x19999;
        break;
    case 0x59:
        *(int *)(a + 0x18) = 0x18ccc;
        break;
    case 0x82:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x83:
        *(int *)(a + 0x18) = 0x19999;
        break;
    case 0x8a:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x93:
        *(int *)(a + 0x18) = 0x1cccc;
        break;
    case 0x95:
        *(int *)(a + 0x18) = 0x1cccc;
        break;
    case 0x1d:
        *(int *)(a + 0x18) = 0x80 << 9;
        break;
    case 0x79:
        *(int *)(a + 0x18) = 0x1b333;
        break;
    case 0x94:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x96:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x97:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x98:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x99:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x9a:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x9b:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x9c:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x9d:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x2f:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x30:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x31:
        *(int *)(a + 0x18) = 0x16666;
        break;
    case 0x54:
        *(int *)(a + 0x18) = 0x80 << 9;
        break;
    case 0x55:
        *(int *)(a + 0x18) = 0xa0 << 9;
        break;
    case 0x80:
        *(int *)(a + 0x18) = 0x16666;
        break;
    case 0x81:
        *(int *)(a + 0x18) = 0x16666;
        break;
    case 0x5e:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x62:
        *(int *)(a + 0x18) = 0x14ccc;
        break;
    case 0x6e:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x84:
        *(int *)(a + 0x18) = 0x10ccc;
        break;
    case 0x85:
        *(int *)(a + 0x18) = 0x10ccc;
        break;
    case 0x86:
        *(int *)(a + 0x18) = 0x11999;
        break;
    case 0x87:
        *(int *)(a + 0x18) = 0x11999;
        break;
    case 0x88:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x89:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x8d:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x90:
        *(int *)(a + 0x18) = 0x13333;
        break;
    case 0x91:
        *(int *)(a + 0x18) = 0xc0 << 9;
        break;
    case 0x92:
        *(int *)(a + 0x18) = 0x18ccc;
        break;
    case 0x34:
        *(int *)(a + 0x18) = 0xa0 << 9;
        break;
    case 0x69:
        *(int *)(a + 0x18) = 0xa0 << 9;
        break;
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x1e:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x5c:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
        *(int *)(a + 0x18) = 0xe666;
        break;
    }
    *(unsigned char **)a = act;
    *(int *)(a + 0xc) = x << 16;
    *(int *)(a + 0x10) = z << 16;
    *(int *)(a + 0x14) = flag;
    *(short *)(a + 4) = spr;
    k = GetWeaponSpriteID(unit);
    z0 = 0;
    z1 = 0xff << 1;
    *(short *)(a + 8) = z0;
    *(int *)(a + 0x20) = z0;
    *(int *)(a + 0x24) = z0;
    *(short *)(a + 0x28) = z0;
    *(short *)(a + 0x2a) = z0;
    *(short *)(a + 0xa) = z1;
    *(short *)(a + 6) = k;
    if (u[0x128] <= 1 && _Func_8078870(u, 1) == 0xf) {
        if (u[0x128] == 0) {
            z2 = 0xf0;
            *(short *)(a + 4) = z2 << 1;
            z4 = 0;
            *(short *)(a + 6) = z4;
        } else {
            z3 = 0xf1;
            *(short *)(a + 4) = z3 << 1;
            z5 = 0;
            *(short *)(a + 6) = z5;
        }
    }
    d = z;
    if (d < 0)
        d += 7;
    e = atan2(d >> 3, x);
    e += 0x80 << 8;
    *(short *)(act + 6) = e;
    act[0x59] = 3;
    act[0x55] = 2;
    if (u[0x129] == 0)
        f = 0x14ccc;
    else
        f = 0x80 << 9;
    *(int *)(act + 0x18) = f;
    *(int *)(act + 0x1c) = f;
    _Actor_SetScript(act, Lc5938);
}
