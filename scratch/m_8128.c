extern unsigned char L1940[] __asm__(".L1940");
extern int CHAR_ARRAY_ARRAY_971__02009928[];
extern unsigned char ewram_2002224[];

void OvlFunc_971_2008128(int i)
{
    int off;
    int k;

    off = i * 4;
    k = L1940[i] << 2;
    *(int *)(k + (int)ewram_2002224) = *(int *)(off + (int)CHAR_ARRAY_ARRAY_971__02009928);
}
