extern int iwram_3001ebc;
extern unsigned char gDebugMode;
extern volatile int gKeyPress;

void Task_Cutscene(void)
{
    char *p;

    p = (char *)iwram_3001ebc;
    if (gDebugMode != 0) {
        if (gKeyPress & (0x80 << 2))
            *(int *)(p + 0xe6 * 2) = 0;
        if (gKeyPress & (0x80 << 1))
            *(int *)(p + 0xe6 * 2) = -1;
    }
}
