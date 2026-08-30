struct SpriteSlot { unsigned short size; unsigned short vramOffset; };
extern struct SpriteSlot gSpriteSlots[];
extern unsigned char gSpriteAllocTable[];

int Func_8003e58(unsigned int slot, unsigned int size)
{
    unsigned int n;
    int i;
    int r;
    unsigned int end;
    unsigned int j;
    unsigned char *p;
    unsigned char *t;
    unsigned char *s;

    n = size >> 6;
    if (slot > 0x5f)
        return -1;
    t = gSpriteAllocTable;
    i = 0;
    s = (unsigned char *)gSpriteSlots;
loop:
    r = -1;
    if (i >= 0x200)
        goto done;
    if (*(t + i) == 0xff) {
        r = i;
        end = n + r;
        if (r < end) {
            p = r + t;
            do {
                if (*p++ != 0xff)
                    goto skip;
                i++;
            } while (i < end);
        }
        j = 0;
        if (j >= n)
            goto shift;
    fill:
        *(gSpriteAllocTable + (r + j)) = slot;
        j++;
        if (j < n)
            goto fill;
    shift:
        r = r << 6;
        goto done;
    }
skip:
    i += *(unsigned short *)(s + (*(t + i) << 2)) >> 6;
    goto loop;
done:
    return r;
}
