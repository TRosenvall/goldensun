/* Cluster Func_801ec6c..Func_801ed40 extracted from
 * goldensun/asm/rom_15000/rom_1de5c_c_c_c_a_a_c_c.s.
 *
 * Total .text for this TU = 220 bytes (0x50 + 0x8c, plus each function's pool).
 * Preserves the original ROM layout when slotted where
 * asm/rom_15000/rom_1de5c_c_c_c_a_a_c_c.o sits in goldensun/stage1.ld.
 */

/* The display node Func_801eadc hands back. Only the two bytes these two
 * functions touch are named: a state byte at 0x04 and a byte at 0x19 whose
 * high nibble carries the portrait slot.
 */
struct MenuNode {
    unsigned char unk0[4];
    unsigned char unk4;
    unsigned char unk5[0x14];
    unsigned char lo4 : 4;
    unsigned char hi4 : 4;
};

extern unsigned char *iwram_3001e8c;
extern int _GetFlag(int);
extern int GetPortrait(int b);
extern void LoadPortrait(int id, int c, int *v, int *t, int e, int f);
extern struct MenuNode *Func_801eadc(int v, int mask, int d, int e, int f);

/* PopulateMenuWindow
 * r0 = menu id, r1.. = placement. Resolves the menu with GetPortrait, loads its
 * graphics with LoadPortrait, reads character data through _Func_79338, and
 * attaches each entry's sprite with Func_1eadc. Called by Func_19da8.
 *
 * The two halfwords at +0x12ec/+0x12ee are the portrait ids held by the two
 * slots, 0x3e7 meaning empty; +0x12f0/+0x12f2 hold each slot's loaded handle.
 */
struct MenuNode *Func_801ec6c(int b, int c, unsigned int a, int d, int e, int f)
{
    unsigned char *p;
    struct MenuNode *node;
    int id;
    int t;
    int v;
    int slot;
    int i;
    int k1;
    int k2;

    p = iwram_3001e8c;
    if (_GetFlag(0x20) != 0) {
        if (b == 0)
            b = 0x12;
        if (b == 1)
            b = 0x13;
    }
    id = GetPortrait(b);
    if (id == -1)
        return 0;
    if (a > 1) {
        if (*(unsigned short *)(p + 0x12ee) == 0x3e7)
            a = 1;
        else if (*(unsigned short *)(p + 0x12ec) == 0x3e7)
            a = 0;
        else
            return 0;
    }
    slot = a + 0xe;
    LoadPortrait(id, c, &v, &t, slot, 0);
    node = Func_801eadc(v, 0x80000000, d, e, f);
    if (node != 0) {
        node->hi4 = slot;
        node->unk4 = 2;
    }
    i = a * 2;
    k1 = i + 0x12ec;
    *(unsigned short *)(p + k1) = id;
    k2 = i + 0x12f0;
    *(unsigned short *)(p + k2) = v;
    return node;
}

/* PopulateMenuEntries
 * r0 = menu id, r1.. = placement. As Func_1ec6c without the node attachment --
 * used when the entries are drawn rather than sprited.
 *
 * The slot search here looks for the slot that ALREADY holds this portrait,
 * where Func_801ec6c looks for a free one.
 */
void Func_801ed40(unsigned int a, int b, int c)
{
    unsigned char *p;
    int id;
    int t;
    int v;
    int k;

    p = iwram_3001e8c;
    if (_GetFlag(0x20) != 0) {
        if (b == 0)
            b = 0x12;
        if (b == 1)
            b = 0x13;
    }
    id = GetPortrait(b);
    if (id == -1)
        return;
    if (a > 1) {
        if (*(unsigned short *)(p + 0x12ee) == id)
            a = 1;
        else if (*(unsigned short *)(p + 0x12ec) == id)
            a = 0;
        else
            return;
    }
    k = a * 2 + 0x12f0;
    v = *(unsigned short *)(p + k);
    LoadPortrait(id, c, &v, &t, a + 0xe, 1);
}
