#include "gba/types.h"

struct View {
    vec3_t v0;
    vec3_t v1;
    vec3_t v2;
    unsigned char pad24[0x10];
    short pitch;
    short yaw;
};

extern void *iwram_3001e80;
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixYaw(int angle);
extern void MatrixPitch(int angle);
void Func_80009c0(vec3_t *a, vec3_t *b);

void Func_80b7f9c(void)
{
    struct View *s;
    vec3_t t;
    int z;
    void (*transform)(vec3_t *, vec3_t *);

    s = (struct View *)iwram_3001e80;
    s->yaw = 0xc0 << 6;
    s->pitch = 0xfe << 8;
    z = 0;
    s->v2.z = 0xff << 17;
    s->v1.x = z;
    s->v1.y = z;
    s->v1.z = z;
    s->v2.y = z;
    s->v2.x = z;
    InitMatrixStack();
    MatrixTranslatev(&s->v1);
    MatrixYaw(s->yaw);
    MatrixPitch(s->pitch);
    t.x = z;
    t.y = z;
    t.z = s->v2.z;
    transform = Func_80009c0;
    transform(&t, &s->v0);
}
