struct SpriteSlot { unsigned short size; unsigned short vramOffset; };
extern struct SpriteSlot gSpriteSlots[];
extern unsigned char gSpriteAllocTable[];

int Func_8003e58(unsigned int slot, unsigned int size)
{
    unsigned int n;
    int i;
    int start;
    unsigned int end;
    unsigned int j;
    unsigned char *p;
    unsigned char *t;
    unsigned char *s;

    n = size >> 6;
    if (slot > 0x5f)
        return -1;
    t = gSpriteAllocTable;
    s = (unsigned char *)gSpriteSlots;
    i = 0;
loop:
    if (i >= 0x200)
        return -1;
    if (*(t + i) == 0xff) {
        start = i;
        end = n + start;
        p = start + t;
        while (i < end) {
            if (*p++ != 0xff)
                goto skip;
            i++;
        }
        for (j = 0; j < n; j++)
            *(t + (start + j)) = slot;
        return start << 6;
    }
skip:
    i += *(unsigned short *)(s + (*(t + i) << 2)) >> 6;
    goto loop;
}
