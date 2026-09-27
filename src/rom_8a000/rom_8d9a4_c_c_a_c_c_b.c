/* ScreenTransitionOut  --  0x080901c0, split out of asm/rom_8a000/rom_8d9a4_c_c_a_c_c.s;
 * ScreenTransitionIn stays in _a.s. Matched from scratch.
 *
 * - STRUCT FIELD STORES LET cse REUSE HALFWORD CONSTANTS FOR LATER BYTE STORES
 *   (81 -> 8): a u16 store through a typed field expands via store_bit_field as a
 *   read-modify-write with (reg:HI) 0 and 0x20 pseudos, which cse reuses for later
 *   byte stores; the zero is REG_EQUIV and pooled, the 0x20 reloaded from the
 *   pool at the tail. TELL: pooled 0 or small constants feeding strb after calls,
 *   plus mid-function pool islands. It needs each case's shared tail written out
 *   (one EBB per case); jump2's cross-jumping merges them back into the ROM's one
 *   `b`, where a `goto common` tail loses the reuse.
 * - `p` declared per switch case makes those pseudos local-alloc candidates, and
 *   the allocation falls into the ROM's p=r5, lo=r6, b=r7 (8 -> 2); StartTask
 *   declared `int` closed the last 2.
 */
struct TW {
    unsigned char pad0[0x528];
    unsigned short f528;
    unsigned short f52a;
    unsigned char pad1[8];
    unsigned short f534;
    unsigned short f536;
    unsigned char pad2[2];
    unsigned char f53a;
    unsigned char f53b;
    unsigned char f53c;
    unsigned char f53d;
};

extern void Func_8003bb4(int);
extern void Func_8003b70(int);
extern void Func_8091200(int a, int b);
extern void Func_8091254(int);
extern struct TW *AllocGlobal1F(void);
extern int StartTask(void *fn, int pri);
extern void WaitFrames(int n);
extern void Func_80907b0(int);
extern void SetIntrHandler(int a, int b, void (*f)(void));
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);
extern void Task_Transition300(void);
extern void Func_80903bc(void);
extern void Func_8090488(void);
extern void Func_8090584(void);

void ScreenTransitionOut(int a, int b)
{
    int lo;
    int k;

    k = (a >> 8) & 0xff;
    lo = a & 0xff;
    switch (k) {
    case 0:
        Func_8003bb4(0);
        Func_8003b70(b);
        break;
    case 1:
        Func_8091200(0x8000, 0);
        Func_8091254(b);
        break;
    case 2: {
        struct TW *p = AllocGlobal1F();
        p->f528 = lo;
        p->f52a = 0x20;
        p->f534 = 0x3f;
        p->f536 = 1;
        StartTask(Task_ScreenWindowTransition, 0xc80);
        StartTask(Func_808f498, 0x480);
        WaitFrames(1);
        p->f53a = 0x20;
        p->f53b = 0x40;
        p->f53c = b;
        p->f53d = 0;
        break;
    }
    case 3: {
        struct TW *p = AllocGlobal1F();
        p->f528 = lo;
        p->f52a = 0x20;
        Func_80907b0(0);
        WaitFrames(1);
        StartTask(Task_Transition300, 0xc80);
        p->f53a = 0x20;
        p->f53b = 0x40;
        p->f53c = b;
        p->f53d = 0;
        break;
    }
    case 4: {
        struct TW *p = AllocGlobal1F();
        if (lo == 0) {
            StartTask(Func_80903bc, 0xc80);
            SetIntrHandler(1, 0, Func_8090584);
            p->f53a = 0;
            p->f53b = 0x50;
            p->f53c = b;
            p->f53d = 0;
        } else {
            StartTask(Func_8090488, 0xc80);
            SetIntrHandler(1, 0, Func_8090584);
            p->f53a = 0;
            p->f53b = 0x50;
            p->f53c = b;
            p->f53d = 0;
        }
        break;
    }
    }
}
