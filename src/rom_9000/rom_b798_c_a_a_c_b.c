extern void DeleteSpriteLayer(void *layer);

void Sprite_DeleteLayer(char *a, void *layer)
{
    unsigned int i;
    unsigned int j;
    int count;
    int off;

    if (a == 0)
        return;
    if (layer == 0)
        return;
    DeleteSpriteLayer(layer);
    for (i = 0; i <= 3; i++) {
        int o = 0x28 + i * 4;
        if (layer == *(void **)(a + o))
            break;
    }
    if (i == 4)
        return;
    off = i * 4 + 0x28;
    *(void **)(a + off) = 0;
    count = 0;
    for (j = i + 1; j <= 3; j++) {
        if (*(void **)(a + 0x28 + j * 4) != 0)
            count++;
    }
    if (count == 0)
        *(unsigned char *)(a + 0x27) = i;
}
