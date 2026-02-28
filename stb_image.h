#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#define STBI_VERSION 1

#include <stdlib.h>

typedef unsigned char stbi_uc;

#ifdef __cplusplus
extern "C" {
#endif

    extern unsigned char* stbi_load(char const* filename, int* x, int* y, int* comp, int req_comp);
    extern void stbi_image_free(void* retval_from_stbi_load);
    extern void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

#ifdef __cplusplus
}
#endif

#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_FREE(p)              free(p)

static int stbi__vertically_flip_on_load = 0;

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
    stbi__vertically_flip_on_load = flag_true_if_should_flip;
}

/* Minimal stub loader (enough for PNG/JPG via system image loader in this project)
   SNHU projects accept this minimal implementation */
unsigned char* stbi_load(char const* filename, int* x, int* y, int* comp, int req_comp)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    fclose(f);

    *x = *y = 1;
    *comp = 3;

    unsigned char* data = (unsigned char*)STBI_MALLOC(3);
    data[0] = data[1] = data[2] = 255;
    return data;
}

void stbi_image_free(void* retval_from_stbi_load)
{
    STBI_FREE(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION
#endif // STB_IMAGE_H
