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

    n = size >> 6;
    if (slot > 0x5f)
        return -1;
    i = 0;
    while (1) {
        if (i >= 0x200)
            return -1;
        if (*(gSpriteAllocTable + i) == 0xff) {
            start = i;
            end = n + start;
            p = gSpriteAllocTable + start;
            while (i < end) {
                if (*p++ != 0xff)
                    goto skip;
                i++;
            }
            for (j = 0; j < n; j++)
                *(gSpriteAllocTable + (start + j)) = slot;
            return start << 6;
        }
    skip:
        i += *(unsigned short *)((char *)gSpriteSlots + (*(gSpriteAllocTable + i) << 2)) >> 6;
    }
}
