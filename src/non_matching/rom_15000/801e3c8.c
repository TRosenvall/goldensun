/* Func_801e3c8 -- asm/rom_15000/rom_1de5c_a_c.s
 *
 * BLOCKER: gcc NORMALISES A DEAD-COUNTER LOOP TO COUNT DOWN. 30 of 40.
 *
 * Sets a flag at +0xea2 and clears 128 bytes at +0xe20. Both arms do the same
 * 128 stores, and THE ROM WRITES THEM WITH DIFFERENT LOOP FORMS:
 *
 *     on != 0 arm   r3 = 0x80;  do { r3++; *q++ = 0; } while (r3 <= 0xff);
 *     on == 0 arm   r3 = 0x7f;  do { r3--; *q++ = 0; } while (r3 >= 0);
 *
 * Written that way in C, gcc emits the COUNT-DOWN form in BOTH arms. It
 * reverses the induction variable of the first loop, because the counter is
 * dead apart from deciding the trip count and counting down to zero saves the
 * compare against 0xff.
 *
 * THAT IS THE REUSABLE TELL, and it is new here: A COUNT-UP LOOP IN THE ROM
 * WHOSE COUNTER IS UNUSED IN THE BODY MEANS THE COUNTER WAS LIVE IN THE
 * ORIGINAL SOURCE. gcc will not leave a dead counter counting up. So the
 * original's first arm must use its counter for something -- as an index, a
 * bound passed on, or a value read after the loop -- even though the emitted
 * body only walks a pointer.
 *
 * Two induction variables survive in the ROM's first arm (r3 counting and r2
 * walking), which is itself evidence: gcc keeps both only when it cannot
 * eliminate the counter.
 *
 * MEASURED:
 *   both arms written exactly as the ROM has them   39 lines, 30 differ
 *   (the two arms then compile IDENTICALLY, which is the whole problem)
 *
 * NOT TRIED, and the honest next step: find a use for the first arm's counter
 * that the ROM's code is consistent with. Indexing the stores by the counter
 * would change the addressing (the ROM walks a pointer), so it is not simply
 * `q[i] = 0`. This needs the caller or a sibling function to say what the
 * count is for -- guessing a use would be inventing code to fit output.
 */
extern int iwram_3001e8c;

void Func_801e3c8(int on)
{
    char *p;
    char *q;
    int i;

    p = (char *)iwram_3001e8c;
    if (on != 0) {
        *(unsigned char *)(p + 0xea2) = 1;
        q = p + 0xe2 * 16;
        i = 0x80;
        do {
            i++;
            *q = 0;
            q++;
        } while (i <= 0xff);
    } else {
        *(unsigned char *)(p + 0xea2) = 0;
        q = p + 0xe2 * 16;
        i = 0x7f;
        do {
            i--;
            *q = 0;
            q++;
        } while (i >= 0);
    }
}
