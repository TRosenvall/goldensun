/* Cluster Func_80a6a98..Func_80a6a98 extracted from goldensun/asm/rom_a1000/rom_a5534_c_c_c_a_c.s.
 *
 * Total .text for this TU = 204 bytes (= 0xcc). Never attempted before batch 276.
 * No pins, no flags. Requires _MSG_53a; see message.sym for why, and for the
 * correction to my batch-275 reasoning that came with it.
 *
 * A POOLED CONSTANT IN THE WRONG REGISTER IS EVIDENCE FOR A SYMBOL, INDEPENDENTLY OF
 * SHIFTABILITY. 0x53a is unshiftable, so gcc pools it either way and the pool load
 * proves nothing -- but the REGISTER does. A CONST_INT is materialised by reload
 * immediately before its user, taking whatever register is free (here r2, just freed
 * by `and r0, r2`), and sched2 then cannot hoist the load above the `and`. A
 * SYMBOL_REF is a real pseudo allocated at greg time, gets r3, and the ROM's
 * `ldr r0,=0x1ff / ldr r3,=0x53a / and / add r0,r3 / ...` falls out whole.
 *
 * 8 differing with the literal, 1 with the symbol -- and four literal spellings tie
 * at 8 (both operand orders, a named `int base` before the `if`, and the callee
 * redeclared to return int), so no spelling reaches it. That is a second, weaker
 * tell than the `byte << n` impossibility argument, and it is worth having because
 * it applies to the unshiftable majority that the strong argument cannot touch.
 *
 * The structure came wholesale from the near-twin Func_80a5614
 * (src/rom_a1000/rom_a5534_a_c_c.c, batch 275), which needed _MSG_75 on the same
 * footing into the same _Func_801e7c0.
 */
extern unsigned char *iwram_3001f2c;
extern int _GetFlag(int id);
extern int _MSG_53a;
extern void _ClearFlag(int id);
extern void _Func_8016498(unsigned int win);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void Func_80a2268(unsigned int win, int a, int b, int c, int d, int e);

int Func_80a6a98(int a0, int a1, int *d)
{
    unsigned char *state;
    int ofs;
    int i;

    state = iwram_3001f2c;
    d[6] = d[2] * 5 + d[4];
    if (_GetFlag(0x151) == 0) {
        _Func_8016498(*(unsigned int *)(state + 0x2c));
        WaitFrames(1);
        ofs = d[6] * 2 + 0xe4 * 2;
        if (*(unsigned short *)((int)state + ofs) != 0)
            _Func_801e7c0((*(unsigned short *)((int)state + ofs) & 0x1ff) + (int)&_MSG_53a,
                          *(unsigned int *)(state + 0x2c), 0, 0);
    } else {
        _ClearFlag(0x2ff);
    }
    for (i = 0; i <= 4; i++) {
        if (i == d[4])
            Func_80a2268(*(unsigned int *)(state + 0x20), 0, i * 2 + 1, 0xf, 1, 0xe);
        else
            Func_80a2268(*(unsigned int *)(state + 0x20), 0, i * 2 + 1, 0xf, 1, 0xf);
    }
    WaitFrames(1);
    return 1;
}
