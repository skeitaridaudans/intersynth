//
// Created by star on 5.3.2023.
//


#include "messages.h"
#include <memory.h>
#include <stdio.h>

int main()
{
    // Test the float conversion functions, the float value is negative pi and does include all the MSB bits
    float value = -3.14159265358979323846;
    char buffer[5];
    memset(buffer, 0, 5);
    store_float_in_buffer(buffer, value);
    printf("Original value: %f\n", value);
    for(int i=0; i<5; i++)
        if(buffer[i] >> 7)
            printf("Error: %d\n", i);
    printf("Buffer: %02x %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    float reconstructed_value = reconstruct_float_from_buffer(buffer);
    printf("Reconstructed value: %f\n", reconstructed_value);
    if(value == reconstructed_value)
        printf("Test passed\n");
    else
        printf("Test failed\n");
    return 0;
}