extern int iwram_3001ebc;
extern unsigned char gState[];
extern int GetEncounterGroup(int encounterID, int group);
extern int GetFieldActor(int actorID);
extern void Func_808adf0(int a);
extern void Func_808b320(int a, int b);

void Func_8091eb0(int a, int b)
{
    unsigned char *g;
    int e;
    int v;

    v = 0x21;
    e = iwram_3001ebc;
    *(short *)(e + (0xbe << 1)) = GetEncounterGroup(a, b);
    if (a == 0x62 && b == 0) {
        g = gState;
        *(short *)(g + 0x1d6) = v;
    }
    if (*(short *)(e + (0xcf << 1)) == 3) {
        g = gState;
        Func_808adf0(GetFieldActor(*(int *)(g + 0x1f4)) + 8);
    }
    Func_808b320(a, b);
}
