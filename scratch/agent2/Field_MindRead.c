#include "dma.h"

extern void *galloc_ewram(int tag, int size);
extern void Func_8097384(void);
extern unsigned int iwram_3001e40;
extern void Func_80978c4(void);
extern int Func_8091200(unsigned int a, unsigned int b);
extern void Func_8091254(unsigned int a);
extern void Func_8097a7c(void);
extern void Task_08097644(void);
extern void StartTask(void (*fn)(void), int prio);

void Field_MindRead(int id, int style)
{
    char *s;
    unsigned int t;

    s = (char *)galloc_ewram(0x16, 0xa6 << 2);
    Func_8097384();
    DMA3_CLEAR(s, 0xa6 << 2);
    t = iwram_3001e40;
    t <<= 1;
    *(unsigned short *)(s + 0x28e) = t % (0xb4 << 1);
    Func_80978c4();
    Func_8091200((*(signed char *)(s + 0x28d) << 10)
                 | (*(signed char *)(s + 0x28c) << 5)
                 | *(signed char *)(s + 0x28b)
                 | (0x80 << 14), 1);
    Func_8091254(8);
    *(unsigned short *)(s + (0xa4 << 2)) = id;
    *(unsigned short *)(s + (0xa4 << 2) + 2) = style;
    s += 0xa5 << 2;
    *(unsigned char *)s = 8;
    Func_8097a7c();
    StartTask(Task_08097644, 0xc8 << 4);
}
