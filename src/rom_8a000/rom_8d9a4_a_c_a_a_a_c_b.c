/* Func_808e5d8 (SortMapObjects) -- MATCHES on the default flags (and unchanged
 * under -fno-rerun-cse-after-loop).  ref: asm/rom_8a000/rom_8d9a4_a_c_a_a_a_c.s
 * tryc.py: OK (64 lines).  Byte-verified: 168 bytes of .text identical
 * (scratch/agent1/bytecheck.sh).
 *
 * The function DOES take an argument (r0) and DOES return a value -- the ROM's
 * `pop {r1} / bx r1` names the `return 0;`.
 * Three levers:
 *  - `gs = gState; gs += 0xfa << 1;` as two statements.  Written
 *    `gs = gState + (0xfa<<1)` gcc folds it to a single `ldr =gState+500`
 *    pool entry where the ROM has mov/lsl/add.
 *  - `unsigned int n` for the shifted field, or the shift comes out `asr`
 *    where the ROM has `lsr`.
 *  - Func_8096b28 declared to return `int`, which emits r0 after the
 *    `ldr r2,[sp]` at both of its call sites.  Declared void: 8 of 64.
 */
extern unsigned char gState[];
extern unsigned char *_GetMoveInfo(int id);
extern int  GetFieldActor(int a);
extern int  Func_808e4b4(int a, int b, int *v);
extern void Func_8096fb0(int a, int b);
extern void Func_80970f8(int a, int b);
extern int Func_8096b28(int a, int b, int c);
extern void Func_8096af0(void);
extern void Func_8097174(void);
extern void Func_8097194(void);

int Func_808e5d8(int arg)
{
    unsigned char *gs;
    int v;
    int id;
    unsigned int n;
    int k;
    int a;
    int b;

    id = 0x3ff & arg;
    n = arg;
    n >>= 10;
    n &= 0xf;
    k = _GetMoveInfo(id)[0xc];
    gs = gState;
    gs += 0xfa << 1;
    GetFieldActor(*(int *)gs);
    a = Func_808e4b4(0x30000005, k, &v);
    b = Func_808e4b4(0x20000005, k, &v);
    Func_8096fb0(id, 0);
    Func_80970f8(*(int *)gs, v);
    Func_8096b28(a, n, v);
    Func_8096af0();
    Func_8097174();
    Func_8096b28(b, n, v);
    Func_8097194();
    return 0;
}
