extern char *iwram_3001eec;

void Task_SpinCamera(void)
{
    char **p;
    char *st;
    char *view;
    int *mode;
    int amt;
    int v;

    p = &iwram_3001eec;
    st = *p;
    p = (char **)((char *)p - 0x6c);
    view = *p;
    mode = (int *)(st + 0x77b0);
    if (*mode == 1) {
        amt = *(int *)(st + 0x77ac);
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        *mode = 0;
    } else {
        amt = *(int *)(st + 0x77ac) / 2;
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        if (*mode != 2)
            v = 2;
        else
            v = 0;
        *mode = v;
    }
}
