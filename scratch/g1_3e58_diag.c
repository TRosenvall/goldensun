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
    if (slot <= 0x5f) {
        t = gSpriteAllocTable;
        i = 0;
        s = (unsigned char *)gSpriteSlots;
    loop:
        if (i >= 0x200)
            return -1;
        if (*(t + i) == 0xff) {
            start = i;
            end = n + start;
            if (start < end) {
                p = start + t;
                do {
                    if (*p++ != 0xff)
                        goto skip;
                    i++;
                } while (i < end);
            }
            j = 0;
            if (j >= n)
                goto done;
        fill:
            *(t + (start + j)) = slot;
            j++;
            if (j < n)
                goto fill;
        done:
            return start << 6;
        }
    skip:
        i += *(unsigned short *)(s + (*(t + i) << 2)) >> 6;
        goto loop;
    } else {
        return -2;
    }
}
