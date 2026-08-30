/* Cluster OvlFunc_900_20081e4..OvlFunc_900_20081e4 split out of goldensun/asm/overlays/rom_797740/ovl_30_c_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * TWENTY-FIVE SPELLINGS. The plain `|=` body is 24 differing: gcc cross-jumps
 * the `strb r3, [r0]` tail of the `e == 0xa` arm into the else arm's third
 * store, which the ROM does not do. The ROM prevents it by making the CONSTANT
 * the destination in two of the four ORs while the else arm's first two keep
 * the VALUE as destination.
 *
 * Three measured facts behind that:
 *   - a narrow (`unsigned char`) local as the OR constant flips the
 *     destination; an `int` local never does;
 *   - a narrow local also costs a `mov r3, r5` copy on every `|=` that keeps
 *     it live;
 *   - two separate constants land in two registers and cost a `pop {r5, r6}`.
 *
 * The resolution routes the two non-flipped stores through an explicit `int`
 * temp, so the ior's first operand is unambiguously the loaded value. That
 * keeps one QImode constant in r5 for all three, drops the copies, and leaves
 * the last store's `orr r5, r3` intact so no cross-jump forms.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_900_20081e4(void)
{
    char *p;
    unsigned char *g;
    unsigned char *q;
    unsigned char m;
    unsigned char n;
    int t;
    int e;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    if (e == 2) {
        __ClearFlag(0x12f);
    } else if (e == 0xa) {
        q = __MapActor_GetActor(8) + 0x59;
        n = 0x14;
        *q = n | *q;
    } else {
        m = 0x14;
        q = __MapActor_GetActor(8) + 0x59;
        t = *q;
        t |= m;
        *q = t;
        q = __MapActor_GetActor(9) + 0x59;
        t = *q;
        t |= m;
        *q = t;
        q = __MapActor_GetActor(0xa) + 0x59;
        *q = m | *q;
    }
    return 0;
}
