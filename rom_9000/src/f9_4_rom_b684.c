// #ifndef NULL
// #define NULL ((void *)0)
// #endif

// typedef struct Entry
// {
//     unsigned char unk0[5];
//     unsigned char state; // offset 5
// } Entry;

// typedef struct Parent
// {
//     unsigned char unk0[0x25];

//     unsigned char flag25; // 0x25
//     unsigned char unk26;
//     unsigned char count;  // 0x27

//     Entry *entries[];     // 0x28
// } Parent;

// void Func_b684_C(Parent *parent, unsigned char value)
// {
//     if (parent == NULL)
//         return;

//     for (unsigned i = 0; i < parent->count; i++)
//     {
//         Entry *entry = parent->entries[i];

//         if (entry->state != 0x0F)
//         {
//             entry->state = value;
//         }
//     }

//     parent->flag25 = 1;
// }