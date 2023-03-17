//
// Created by star on 13.2.2023.
//

#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>
#include "floating_fix.h"
void fragment_floating(float f, unsigned char* fragmented)
{
    //Fragments the float into 5 bytes in the char array.
    memcpy(fragmented, (const void *) &f, sizeof(f));
    unsigned char x = ((fragmented[0] >> 7) & 1);
    unsigned char y = ((fragmented[1] >> 7) & 1);
    unsigned char z = ((fragmented[2] >> 7) & 1);
    unsigned char w = ((fragmented[3] >> 7) & 1);
    fragmented[4] |= x | (y << 1) | (z << 2) | (w << 3);

    //Taking the end bit of every byte
    for(int i=0; i<5;i++)
    {
        fragmented[i] &= 0x7F;
    }
}
float defragment_floating(const char *fragmented)
{
    union
    {
        float f;
        uint32_t u;
    }tmp;

    tmp.u = fragmented[0] | ((uint32_t)fragmented[1] << 8) | ((uint32_t)fragmented[2] << 15) | ((uint32_t)fragmented[3] << 24);
    // Now retrive the endbits
    uint32_t x = fragmented[4] & 1;
    uint32_t y = (fragmented[4] >> 1) & 1;
    uint32_t z = (fragmented[4] >> 2) & 1;
    uint32_t w = (fragmented[4] >> 3) & 1;

    // Now set them per byte, X to the first endbit in the first endbyte and so on.
    tmp.u |= (w << 31) | (z << 23) | (y << 15) | (x << 7);
    return tmp.f;
}

