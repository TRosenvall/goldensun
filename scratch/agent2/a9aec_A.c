/* Func_80a9aec -- PARKED, 25 of 80 aligned.
   Ref: asm/rom_a1000/rom_a8604_c_c_a_a.s
   NEEDS A message.sym ADDITION:  _MSG_182 = 0x182;
   (screened with scratch/agent2/msg2.sym bind-mounted over message.sym).
   Without the symbol gcc builds 0x182 as mov #0xc1 / lsl #1 at all four call
   sites; the ROM pools it once into r8, which is the symbol tell.
   Two other things confirmed here: the loop must be a `goto` loop (the ROM
   rebuilds both pool masks inside the body), and the four kinds are a
   `switch` (the repeated `cmp #2` decision tree).
   Residue: the ROM has list in r7 and the 0x182 base in r8; ours has them the
   other way round, which costs `mov r3, r8` -> `add r0, r5, r7` at four sites
   and turns `add r7, #2` into `mov r1,#2 / add r8, r1`.  Blocker class: the
   "parameter pointer one register too low" allocation residue.
   Tried: assigning base after the counter, copying the parameter into a local
   (both byte-identical at 25), and inlining the symbol at the four sites
   instead of naming it (worse -- gcc then does not carry it at all).  */
extern int _MSG_182;

extern unsigned char *_GetItemInfo(int id);
extern void _Func_801e7c0(int msg, int win, int x, int y);

void Func_80a9aec(int win, unsigned short *list)
{
    int base;
    int i;
    int v;
    int id;
    int m;
    unsigned char *info;

    base = (int)&_MSG_182;
    i = 0xe;
loop:
    v = *list;
    m = 0x200;
    m &= v;
    list++;
    if (m != 0) {
        id = 0x1ff;
        id &= v;
        info = _GetItemInfo(id);
        switch (info[2]) {
        case 1:
            _Func_801e7c0(id + base, win, 8, 8);
            break;
        case 2:
            _Func_801e7c0(id + base, win, 8, 0x38);
            break;
        case 3:
            _Func_801e7c0(id + base, win, 8, 0x28);
            break;
        case 4:
            _Func_801e7c0(id + base, win, 8, 0x18);
            break;
        }
    }
    i--;
    if (i >= 0)
        goto loop;
}
