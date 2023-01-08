/*
 * fonts.h
 *
 *  Created on: Jan 8, 2023
 *      Author: marzdz
 */
#ifndef FONTS_H_
#define FONTS_H_

#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


extern FontDef Font_11x18;


#endif /* FONTS_H_ */
