/* OvlFunc_949_2008170  --  0x02008170, cut from
 * goldensun/asm/overlays/rom_7d4af4/ovl_30_a_a_c_c.s.
 *
 * Hands an actor to the interaction handler twice: once with kind 0x20, and if
 * that comes back zero, again with a kind and flag chosen from map state. One
 * of THREE members found by tools/find_shape.py, differing only in the two slot
 * ids and which handler they call.
 *
 * TWO THINGS DECIDED IT, and both are about how a two-way choice is spelled.
 *
 *   THE CALL GOES IN BOTH ARMS. `__MapActor_GetActor(c ? 0xf : 0xe)` and the
 *   equivalent if/else-into-a-variable both come out BRANCHLESS -- gcc notices
 *   the two values differ by one and emits the `neg / orr / lsr #31 / add` chain
 *   from docs/elevation.md, three instructions shorter than the ROM. Writing
 *   the call inside each arm gives the ROM's `mov r0, #0xf / b / mov r0, #0xe /
 *   bl`, because gcc cross-jumps the call itself and keeps the branch for the
 *   argument. 58 differing lines to 4.
 *
 *   THE TWO INITIALISERS ARE ASSIGNED IN THE OPPOSITE ORDER TO THE ROM'S
 *   EMISSION. The ROM sets kind (0x12) into r10 first and flag (0) into r9
 *   second; writing `kind = 0x12; flag = 0;` emits them the other way round.
 *   `flag = 0; kind = 0x12;` produces the ROM's order. Declaration order does
 *   not do it -- only the assignment order. Those were the last 4 lines.
 *
 * The `||` is short-circuit and stays two tests; written with `|` it would fuse.
 */
extern unsigned char iwram_3001e8c[];
extern void *__MapActor_GetActor(int slot);
extern int OvlFunc_949_200807c(void *a, void *b, int c, int d);

int OvlFunc_949_2008170(void *e)
{
    unsigned char *base;
    char *tbl;
    unsigned short *p;
    int kind;
    int flag;
    void *a;
    void *a1;

    base = *(unsigned char **)iwram_3001e8c;
    tbl = *(char **)(iwram_3001e8c + 0x30);
    p = (unsigned short *)((char *)e + 0x64);
    flag = 0;
    kind = 0x12;
    if (*p & 1)
        a1 = __MapActor_GetActor(0x11);
    else
        a1 = __MapActor_GetActor(0x10);
    if (OvlFunc_949_200807c(e, a1, 0x20, 0) == 0) {
        a = __MapActor_GetActor(0);
        if (*(short *)(tbl + (0xbc << 1)) != 0 || base[0xea4] != 0) {
            kind = 0x1a;
            if (*p & 2)
                flag = 1;
        }
        OvlFunc_949_200807c(e, a, kind, flag);
    }
    return 0;
}
