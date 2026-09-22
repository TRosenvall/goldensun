/* Cluster Menu_Save..Menu_Save extracted from goldensun/asm/rom_15000/rom_20198_a.s.
 *
 * Total .text for this TU = 288 bytes (= 0x120). Never attempted before batch 279.
 * No pins, no volatile, no flags. Three levers, as a ladder:
 *
 * 1. A NAMED INDEX for the `(slot << 6) + 0x105c` access. Without it gcc splits base-plus-shift
 *    and then adds the constant, losing the register-offset `ldrb`. 82 differing to 15.
 * 2. A SEPARATE POINTER VARIABLE for the ewram base rather than reusing the iwram one -- that
 *    alone fixed the whole r5/r6 permutation. 15 to 4.
 * 3. `goto` INTO A `do/while` for both `while (Func_8017364() == 0) WaitFrames(1);` loops. 4 to 0.
 *
 * LEVER 3 IS WORTH RECORDING PRECISELY, because it is a scheduling effect and not a loop-shape
 * one: the loop's `NOTE_INSN_LOOP_BEG` rides on the `b` into the test and acts as a SCHEDULING
 * BARRIER, which flips `mov r1, #0xd` ahead of `ldr r0, =<msg>` at exactly the two sites FOLLOWED
 * BY THAT LOOP. The three `Func_801776c` sites not followed by a loop were correct from the start.
 * `for(;;)` + `break`, `while (!f())`, and a one-line body all stayed at 4.
 *
 * So the recorded `goto`-into-a-do/while entry is about more than reproducing a branch shape: the
 * loop note it creates is a barrier that reorders the code BEFORE it.
 */
extern int Func_80056cc(void);
extern int Func_801776c(int, int);
extern void Func_8005c68(void);
extern int Func_8020244(int, int);
extern int Func_8017364(void);
extern void WaitFrames(int);
extern int YesNoMenu(int, int, int, int);
extern void Func_8019a54(void);
extern void _PlaySound(int);
extern void PrepareSaveHeader(void);
extern void _Func_808ba38(void);
extern int SomethingSaveHeader(int, int);
extern void Func_8005cf8(void);
extern unsigned char *iwram_3001f1c;
extern short ewram_2002004;
extern unsigned char ewram_2000000[];
extern int _MSG_0a;
extern int _MSG_0b;
extern int _MSG_14;
extern int _MSG_17;
extern int _MSG_1a;

int Menu_Save(void) {
    int r6;
    unsigned char *r5;
    unsigned char *q;
    int r7;
    int r8;
    int i;

    r8 = 0;
    r6 = Func_80056cc();
    if (r6 != 0) {
        Func_801776c((int)&_MSG_0a, 1);
        r8 = -9;
    } else {
        Func_8005c68();
        r5 = iwram_3001f1c;
        r7 = Func_8020244(ewram_2002004, 0);
        if (r7 == -1) {
            r8 = r7;
        } else {
            i = (r7 << 6) + 0x105c;
            if (r5[i] != 0) {
                Func_801776c((int)&_MSG_14, 0xd);
                goto chk1;
                do {
                    WaitFrames(1);
                chk1:
                    ;
                } while (Func_8017364() == 0);
                if (YesNoMenu(1, 0, 0, 1) != 0) {
                    Func_8019a54();
                    goto done;
                }
                Func_8019a54();
            }
            ewram_2002004 = r7;
            _PlaySound(0x55);
            Func_801776c((int)&_MSG_1a, 0xd);
            goto chk2;
            do {
                WaitFrames(1);
            chk2:
                ;
            } while (Func_8017364() == 0);
            PrepareSaveHeader();
            _Func_808ba38();
            q = ewram_2000000;
            r6 = SomethingSaveHeader(r7, (int)q);
            q = q + 0x1000;
            r6 |= SomethingSaveHeader(r7 + 3, (int)q);
            Func_8019a54();
            if (r6 != 0) {
                Func_801776c((int)&_MSG_0b, 1);
                r8 = -3;
            } else {
                Func_801776c((int)&_MSG_17, 9);
            }
        }
    }
done:
    Func_8005cf8();
    return r8;
}
