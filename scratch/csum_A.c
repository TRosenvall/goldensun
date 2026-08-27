extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Anim_Ramses(int *desc);
extern void Anim_Nereid(int *desc);
extern void Anim_Kirin(int *desc);
extern void Anim_Atalanta(int *desc);
extern void Anim_Cybele(int *desc);
extern void Anim_Neptune(int *desc);
extern void Anim_Tiamat(int *desc);
extern void Anim_Procne(int *desc);
extern void Anim_Judgment(int *desc);
extern void Anim_Boreas(int *desc);
extern void Anim_Meteor(int *desc);
extern void Anim_Thor(int *desc);

void Anim_Summon(int *desc)
{
    int c;

    galloc_iwram(0x29, 0x302);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    c = desc[0];
    if (c == 0) {
        Anim_Meteor(desc);
        return;
    }
    switch (c) {
    case 1:
        Anim_Ramses(desc);
        break;
    case 2:
        Anim_Nereid(desc);
        break;
    case 3:
        Anim_Kirin(desc);
        break;
    case 4:
        Anim_Atalanta(desc);
        break;
    case 5:
        Anim_Cybele(desc);
        break;
    case 6:
        Anim_Neptune(desc);
        break;
    case 7:
        Anim_Tiamat(desc);
        break;
    case 8:
        Anim_Procne(desc);
        break;
    case 9:
        Anim_Judgment(desc);
        break;
    case 10:
        Anim_Boreas(desc);
        break;
    case 11:
        Anim_Meteor(desc);
        break;
    case 12:
        Anim_Thor(desc);
        break;
    }
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}
