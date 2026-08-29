extern char *__MapActor_GetActor(int slot);

void OvlFunc_945_200b7b4(void)
{
    int i;
    int bit;
    unsigned char *p;

    i = 0x1c;
    bit = 8;
    do {
        p = (unsigned char *)(__MapActor_GetActor(i) + 0x59);
        *p |= bit;
        i++;
    } while (i <= 0x23);
}
