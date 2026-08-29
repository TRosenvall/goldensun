struct Actor {
    int *script;
    unsigned short cursor;
    unsigned char pad6[0x57];
    unsigned char iter;
};

extern int Actor_FindScriptMarker(struct Actor *e, int label);

int ActorCmd_Loop(struct Actor *e)
{
    int *p;
    int count;
    int label;
    int n;

    p = e->script + *(short *)&e->cursor + 1;
    count = *p++;
    label = *p;
    if (count == 0xffff)
        goto jump;
    n = e->iter + 1;
    e->iter = n;
    if ((int)(unsigned char)n >= (int)(short)count)
        goto reset;
jump:
    e->cursor = Actor_FindScriptMarker(e, label);
    return 1;
reset:
    e->iter = 0;
    e->cursor += 3;
    return 1;
}
