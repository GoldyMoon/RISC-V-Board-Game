#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct{
    uint32_t DBlue : 8;
    uint32_t DGreen : 8;
    uint32_t DRed : 8;
    uint32_t DAlpha : 8;
} SColor, *SColorRef;

#endif
