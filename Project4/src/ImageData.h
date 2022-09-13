#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "Color.h"

#define BOARD_IMAGE_WIDTH       280
#define BOARD_IMAGE_HEIGHT      240

#define PIECE_IMAGE_WIDTH       32
#define PIECE_IMAGE_HEIGHT      32

extern SColor Palette[235];
extern uint8_t BoardImage[];
extern uint8_t BluePieceImage[];
extern uint8_t YellowPieceImage[];

#endif
