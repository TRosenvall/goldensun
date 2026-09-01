extern unsigned char *GetFieldActor(int i);
extern void _Actor_SetAnimSpeed(void *a, int s);
extern short ewram_200048a;

void Func_8096cdc(void *self, int flag, int speed)
{
    unsigned char *a;
    short *pw;
    int i;

    i = 0;
    pw = &ewram_200048a;
    do {
        a = GetFieldActor(i);
        if (i != *pw && a != 0 && a != self) {
            a[0x5b] = flag;
            _Actor_SetAnimSpeed(a, speed);
        }
        i++;
    } while (i <= 0x42);
}
