struct S {
    unsigned int p;
    unsigned short a;
    unsigned short b;
    unsigned short c;
    unsigned short d;
    unsigned char e;
    unsigned char f;
};

/* InitListRecord -- exported
 * r0 = list record, r1 = visible rows, r2 = cursor, r3 = flag.
 * Initialises the controller from the list at [r0]: copies the total count from
 * [list]+6 and the row height from [list]+8, stores the caller's visible count,
 * cursor and flag, and clears +0x0C.
 */
void Func_80b09fc(struct S *arg0, unsigned short arg1, unsigned short arg2, unsigned char arg3)
{
    unsigned short *src;

    src = (unsigned short *)arg0->p;
    arg0->a = src[3];
    arg0->b = src[4];
    arg0->c = arg1;
    arg0->d = arg2;
    arg0->f = arg3;
    arg0->e = 0;
}
