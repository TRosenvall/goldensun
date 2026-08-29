/* ActorCmd_Loop -- PARKED, 3 of 40 aligned (1 real instruction + 2 label shift).
   Ref: asm/rom_9000/rom_d654_a_c_a_a_a_c.s
   Residue: the ROM has `mov r0, r5 / add r0, #0x5d`; gcc emits only
   `add r0, #0x5d`, because reload knows the incoming parameter register r0
   still holds `e`.  Blocker class: elided pointer copy / register allocation.
   Tried (all byte-identical to each other): derived initialiser
   `c = e + 0x5d`, `(unsigned char *)((int)e + 0x5d)`, a two-step walk
   `c = e; c += 0x5d;`, a struct with the counter as a member at 0x5d, and
   hoisting the assignment above the `if` (worse: 28 differ).  */
extern int Actor_FindScriptMarker(unsigned char *e, int label);

int ActorCmd_Loop(unsigned char *e)
{
    int *p;
    int count;
    int label;
    unsigned char *c;
    int n;

    p = *(int **)e + *(short *)(e + 4) + 1;
    count = *p++;
    label = *p;
    if (count == 0xffff)
        goto jump;
    c = e + 0x5d;
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
