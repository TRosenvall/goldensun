extern int L5b70[] __asm__(".L5b70");
extern int OvlFunc_943_200b150(int i);
extern int OvlFunc_943_200b464(int i);

void OvlFunc_943_200b3b8(void)
{
    int *p;
    unsigned int i;

    p = L5b70;
    for (i = 0; i <= 3; i++) {
        if (OvlFunc_943_200b150(i))
            *p = OvlFunc_943_200b464(i);
        else
            *p = 3;
        p++;
    }
    if (OvlFunc_943_200b150(0))
        L5b70[0] = OvlFunc_943_200b464(0);
    else
        L5b70[0] = 3;
    if (OvlFunc_943_200b150(2))
        L5b70[1] = OvlFunc_943_200b464(2);
    else
        L5b70[1] = 3;
    L5b70[2] = 3;
    L5b70[3] = 3;
    if (OvlFunc_943_200b150(1))
        L5b70[4] = OvlFunc_943_200b464(1);
    else
        L5b70[4] = 3;
    if (OvlFunc_943_200b150(3))
        L5b70[5] = OvlFunc_943_200b464(3);
    else
        L5b70[5] = 3;
    L5b70[6] = 3;
    L5b70[7] = 3;
}
