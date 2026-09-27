/* Func_801eadc  --  0x0801eadc, was asm/rom_15000/rom_1de5c_c_c_a_c_c_c_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * Func_8016584 must be declared returning `int` although the result is unused.
 * A `void` call does not set r0 in RTL, so this function's own final
 * `mov r0,rN` output-depends on the argument copy `r0 = argN`, giving that copy
 * two dependents; rank_for_schedule then hoists it above equal-priority stores
 * on dependent count. With `int` the call sets r0, the edge goes, and the copy
 * sits where the ROM has it, right before the call (read from the
 * -fsched-verbose=5 dependency table: priority 2 = 2, dependents 2 vs 1).
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Node {
    unsigned int next;
    unsigned char f4;
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad[4];
    unsigned char slot;
    unsigned char fF;
    unsigned char pad2[4];
    unsigned int attr;
    unsigned int tile;
};

struct Win {
    unsigned char pad[0xc];
    unsigned short x;
    unsigned short y;
};

extern struct SpriteSlot gSpriteSlots[];
extern struct Node *Func_8015e8c(void);
extern void Func_8003f3c(int);
extern int Func_8016584(struct Win *, struct Node *);

struct Node *Func_801eadc(int slot, unsigned int flags, struct Win *win, int x, int y)
{
    struct Node *n;
    unsigned int px, py;

    n = Func_8015e8c();
    if (n == 0) {
        Func_8003f3c(slot);
        return 0;
    }
    px = x + (win->x << 3) + 8;
    py = y + (win->y << 3) + 8;
    px &= 0x1ff;
    py &= 0xff;
    n->attr = (px << 16) | py | flags;
    n->tile = gSpriteSlots[slot].vramOffset >> 5;
    n->fF = 0xff;
    n->next = 0;
    n->x = px;
    n->y = py;
    n->slot = slot;
    n->f4 = 1;
    n->f5 = 1;
    Func_8016584(win, n);
    return n;
}
