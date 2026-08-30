/* Cluster Func_802851c..Func_802851c extracted from goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_c_a.s.
 *
 * Total .text for this TU = 88 bytes.
 * Preserves the original ROM layout when slotted before
 * asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.o in goldensun/stage1.ld.
 *
 * A UI teardown: stop the task, close the box if one is open, release each of
 * `count` entries, free the tag, wait a frame.
 *
 * THE LOOP MUST BE WRITTEN WITH A SUBSCRIPT, NOT A WALKING POINTER. Spelling
 * it as `unsigned short *p` advanced by 0x14 puts the pointer's preheader
 * (`mov r5, r7 / add r5, #0x12`) ABOVE the `bge` guard and costs a register
 * rename -- 26 differing. Written as `u + i * 0x14 + 0x12`, strength reduction
 * creates the induction variable in the preheader AFTER the guard, which is
 * where the ROM has it. Struct stride 0x14 and the 0x8e count offset are
 * confirmed against the sibling src/rom_15000/rom_23178_a_a_a_a_c_c_c_b.c.
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
    unsigned char pad7c[0x12];
    short count;
};

extern unsigned char *iwram_3001f38;

extern void Func_8028194(void);
extern void StopTask(void *task);
extern void CloseUIBox(void *box, int n);
extern void Func_8003f3c(int id);
extern void gfree(int tag);
extern void WaitFrames(int n);

void Func_802851c(void)
{
    struct Ui *u;
    int i;

    u = (struct Ui *)iwram_3001f38;
    StopTask(Func_8028194);
    if (u->box != 0)
        CloseUIBox(u->box, 2);
    for (i = 0; i < u->count; i++)
        Func_8003f3c(*(unsigned short *)((unsigned char *)u + i * 0x14 + 0x12));
    gfree(0x3a);
    WaitFrames(1);
}
