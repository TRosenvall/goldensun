typedef unsigned char u8;
typedef unsigned short u16;

struct MsgRec {
    u8 pad00[0x12];
    u16 pending;
    u16 unk14;
    u16 flags;
    int busy;
};

struct MsgSlot {
    struct MsgRec *rec;
    u16 unk04;
    u16 unk06;
    u8 pad08[0x0a];
    u16 unk12;
    u16 unk14;
    u16 unk16;
    u16 unk18;
    u16 unk1a;
    u8 pad1c[0x0c];
};

extern u8 *iwram_3001e8c;
extern void Func_8019854(struct MsgSlot *slot);
extern int AdvanceMsgText(struct MsgSlot *slot);
extern void CloseUIBox(struct MsgRec *rec, u16 flag);

void Func_8016868(void)
{
    struct MsgSlot *slot;
    struct MsgRec *rec;
    int i;
    u16 pending;
    int r;

    slot = (struct MsgSlot *)(iwram_3001e8c + 0x620);
    for (i = 0; i != 3; i++, slot++) {
        rec = slot->rec;
        if (rec == 0)
            continue;
        if (rec->busy != 0)
            continue;
        if (rec->flags == 0) {
            slot->rec = 0;
            continue;
        }
        pending = rec->pending;
        if (pending != 0) {
            Func_8019854(slot);
            continue;
        }
        r = AdvanceMsgText(slot);
        switch (r) {
        case 8:
            slot->rec->unk14 = 1;
            break;
        case 9:
            CloseUIBox(slot->rec, slot->rec->flags & 2);
            slot->unk04 = pending;
            slot->unk06 = pending;
            slot->unk12 = pending;
            slot->unk14 = pending;
            slot->unk16 = pending;
            slot->unk18 = pending;
            slot->unk1a = pending;
            slot->rec->unk14 = 1;
            break;
        }
    }
}
