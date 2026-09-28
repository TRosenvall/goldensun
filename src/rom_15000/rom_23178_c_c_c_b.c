/* Debug_IconTest -- 0x08029554, first of the two functions that were in
 * asm/rom_15000/rom_23178_c_c_c.s.  Debug_FaceTest stays in assembly as `_c`,
 * together with the .rodata.
 *
 * 552 bytes, 250 encodings and 26 relocations identical.
 *
 * THIS FILE IS WHY tools/datacheck.py WAS FIXED THIS BATCH.  Its guard for "is this
 * gcc's own output?" was the substring `".gcc2_compiled." in text`, and this file
 * discusses that string in its own `@` annotation prose -- so datacheck called it
 * generated, examined no data sections and exited 0.  It carries .rodata.  Landing
 * this function on that verdict would have been the batch-267 failure again.
 *
 * FIVE NEW EXPORTS WERE NEEDED, AND datacheck LISTED NONE OF THEM.  Its EXPORTS line
 * reports the sixteen labels that are ALREADY .global; `.L37440`, `.L37448`,
 * `.L37450`, `.L37458` and `.L37460` are defined inside the same .rodata, carry no
 * .global, and are referenced ONLY from this function -- so all five had to be
 * exported on the part that keeps the data.  Debug_FaceTest references none of them.
 * The exports were gated on their own (a .global emits no bytes), then the split on
 * its own, then this file.
 *
 * The tool lists existing exports, not the exports a split will REQUIRE; the same
 * under-reporting was found independently in rom_c9000 and rom_a1000 this batch.
 * Read definitions against references across the intended boundary, in both
 * directions, and do not take the EXPORTS line as the answer.
 *
 * `volatile` on gKeyRepeat is a real qualifier, not a match dodge: the key state is
 * written by the interrupt handler and this loop re-reads it every frame.  The five
 * `__asm__` declarations are name bindings for labels that are not valid C
 * identifiers.  No pins, no barriers, no fakematch row.
 */
extern unsigned char *iwram_3001e68;
extern volatile unsigned int gKeyRepeat;

extern unsigned char L37440[] __asm__(".L37440");
extern unsigned char L37448[] __asm__(".L37448");
extern unsigned char L37450[] __asm__(".L37450");
extern unsigned char L37458[] __asm__(".L37458");
extern unsigned char L37460[] __asm__(".L37460");

extern int WaitFrames(int n);
extern void *CreateUIBox(int x, int y, int w, int h, int opts);
extern int CloseUIBox(void *box, int mode);
extern void UIDrawText(unsigned char *s, void *box, int x, int y);
extern void Func_801ea08(int a, int b, void *box, int d, int e);
extern int Func_801eadc(int slot, unsigned int m, void *box, int x, int y);
extern int LoadItemIconID(int id, int a, int *out, int *b, int c);
extern int LoadMoveIconID(int id, int a, int *out, int *b, int c);
extern int AllocSpriteSlot(void);
extern int LoadStatusIcon(int i, int a, int slot);

int Debug_IconTest(void)
{
    void *box;
    int mode;
    int idx;
    int base;
    int flag;
    int id;
    int a;
    int i;
    int row, col;
    int z;
    unsigned short *w;

    box = 0;
    idx = 0;
    mode = 0;
    flag = 1;
    *(unsigned short *)(iwram_3001e68 + 4) = flag;
    WaitFrames(1);
    while (1) {
        if (gKeyRepeat & 0x20) {
            idx--;
            flag = 1;
        }
        if (gKeyRepeat & 0x10) {
            idx++;
            flag = 1;
        }
        if (gKeyRepeat & 0x200) {
            mode--;
            flag = 1;
        }
        if (gKeyRepeat & 0x100) {
            mode++;
            flag = 1;
        }
        if (gKeyRepeat & 1)
            break;
        if (gKeyRepeat & 2)
            break;
        if (flag != 0) {
            flag = 0;
            idx = (idx + 8) % 8;
            mode = (mode + 3) % 3;
            CloseUIBox(box, 2);
            box = CreateUIBox(0xa, 0, 0x12, 0xc, 2);
            if (mode == 0)
                UIDrawText(L37440, box, 0, 0);
            else if (mode == 1)
                UIDrawText(L37448, box, 0, 0);
            else
                UIDrawText(L37450, box, 0, 0);
            UIDrawText(L37458, box, 0, 8);
            Func_801ea08(idx, 0, box, 0x28, 8);
            base = idx << 5;
            Func_801ea08(base, 3, box, 0x40, 8);
            UIDrawText(L37460, box, 0x58, 8);
            Func_801ea08(base + 0x1f, 3, box, 0x60, 8);
            i = 0;
            do {
                id = -1;
                col = i % 8 * 16;
                row = i / 8 * 16 + 0x10;
                if (mode == 0) {
                    LoadItemIconID(base + i, 1, &id, &a, 0);
                    Func_801eadc(id, 0x80 << 23, box, col, row);
                } else if (mode == 1) {
                    LoadMoveIconID(base + i, 1, &id, &a, 0);
                    Func_801eadc(id, 0x80 << 23, box, col, row);
                } else {
                    id = AllocSpriteSlot();
                    LoadStatusIcon(i, 0, id);
                    Func_801eadc(id, 0x80 << 23, box, col, row);
                }
                i++;
            } while (i <= 0x1f);
        }
        WaitFrames(1);
    }
    CloseUIBox(box, 2);
    w = (unsigned short *)(iwram_3001e68 + 4);
    z = 0;
    *w = z;
    return 0;
}
