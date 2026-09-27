/* Func_80a9c18  --  0x080a9c18, was asm/rom_a1000/rom_a8604_c_c_a_c_a_a.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * `list[i]` read THREE TIMES with no local reproduces the ROM's pooled
 * `ldr =0x200` and the `ldrh r2 / mov r3,r2 / cmp r3` copy. Every local form
 * (u16, s16, int, u32, a pointer walk, (short) masks) stayed 17-75 off, and the
 * u16/s16 locals come out as `ldrsh`. Observed; mechanism not traced.
 */
struct Node {
    unsigned char pad00[6];
    short x;
    short y;
};

extern unsigned char *iwram_3001f2c;
extern void Func_80a9cbc(void);
extern unsigned char *_GetItemInfo(int item);
extern void Func_80a17c4(struct Node *node);

void Func_80a9c18(unsigned short *list)
{
    struct Node **slot;
    struct Node *n;
    int i;
    int x;
    unsigned char *st;

    st = iwram_3001f2c;
    Func_80a9cbc();
    slot = (struct Node **)(st + 0x48);
    x = 0xd8;
    for (i = 0; i < 15; i++) {
        if (list[i] != 0 && (list[i] & 0x200) && (n = slot[i]) != 0) {
            switch (_GetItemInfo(list[i] & 0x1ff)[2]) {
            case 1:
                n->x = x;
                n->y = 0x20;
                break;
            case 2:
                n->x = x;
                n->y = 0x50;
                break;
            case 3:
                n->x = x;
                n->y = 0x40;
                break;
            case 4:
                n->x = x;
                n->y = 0x30;
                break;
            }
            Func_80a17c4(n);
        }
    }
}
