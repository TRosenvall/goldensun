extern int Actor_FindScriptMarker(unsigned char *e, int label);

int ActorCmd_Loop(unsigned char *e)
{
    int *p;
    int count;
    int label;
    unsigned char *c;
    int n;

    c = e + 0x5d;
    p = *(int **)e + *(short *)(e + 4) + 1;
    count = *p++;
    label = *p;
    if (count == 0xffff)
        goto jump;
    n = *c + 1;
    *c = n;
    if ((int)(unsigned char)n >= (int)(short)count)
        goto reset;
jump:
    *(short *)(e + 4) = Actor_FindScriptMarker(e, label);
    return 1;
reset:
    *c = 0;
    *(unsigned short *)(e + 4) += 3;
    return 1;
}
