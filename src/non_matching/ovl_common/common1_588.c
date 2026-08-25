/* OvlFunc_common1_588  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/common/common1_a_a_a_a_c_a.s
 * Best screen: 15 instructions in disagreeing regions, of 33 (rom 33, ours 29).
 *
 * BLOCKER CLASS: gcc if-converts the three-way constant select.
 *
 * The ROM keeps three separate blocks, each loading its message id and jumping
 * to a shared join:
 *
 *      cmp r2, r3 / bne L0 / ldr r0, =0x2076 / b L1
 *      L0: ... bne L2 / ldr r0, =0x2078 / b L1
 *      L2: ldr r0, =0x207a
 *      L1: add r0, #1 / bl __MessageID
 *
 * gcc hoists each pool load ABOVE its compare and branches straight to the
 * join, which is four instructions shorter.
 *
 * WHAT WAS TRIED
 *   1. An if/else-if chain assigning a local (kept below). 15 of 33.
 *   2. Explicit blocks with `goto done;` from each arm -- literally the ROM's
 *      block structure. BYTE-IDENTICAL.
 *
 * (2) is the informative result: this is not block PLACEMENT, which source
 * structure can sometimes reach, but a pool load scheduled before its own
 * compare. Same family as the pool-loads-first parks.
 *
 * THIS FUNCTION IS WHERE _AREA_8f AND _AREA_90 WERE IDENTIFIED. The values are
 * compared against gState+0x1C0 while the message ids passed to __MessageID are
 * 0x2076/0x2078/0x207a -- so a value-based reading would have named them
 * _FILE_8f and _FILE_90, which is what file_table.sym calls them. See the
 * batch-67 block in area.sym.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8f;
extern int _AREA_90;
extern void __Func_8019908(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(void *a, int n);

void OvlFunc_common1_588(void *a, int b)
{
    unsigned char *g;
    void *p;
    unsigned int k;
    int v;
    int m;

    p = a;
    __Func_8019908(b, 5);
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_8f))
        m = 0x2076;
    else if (v == (int)(&_AREA_90))
        m = 0x2078;
    else
        m = 0x207a;
    __MessageID(m + 1);
    __ActorMessage(p, 0);
}
