//
// Created by onureozcan on 26.05.2020.
//

#ifndef IMAGELIB_IMLIB_H
#define IMAGELIB_IMLIB_H

#define IM_DEPTH_RGBA_32 sizeof(uint32_t)

#define im_where(image,x,y) (uint8_t*)(((image)->data) + ((y)*(image)->width+(x)))
#define im_alpha_where(image,x,y) (((image)->alpha_buffer) + ((y)*(image)->width+(x)))

#define im_color(r,g,b,a) ((im_color4){(r),(g),(b),(a)})
#define im_brush(thickness,hardness) ((im_brush2d){(thickness),(hardness)})
#define im_point(x,y) ((im_point2d){(x),(y)})

#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
} im_point2d;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} im_color4;

typedef struct {
    float size;
    uint32_t hardness;
} im_brush2d;

typedef struct {
    uint32_t* data;
    uint32_t* alpha_buffer; // prevent overlapping alpha operations
    uint32_t width;
    uint32_t height;
    uint32_t z_value;
    uint32_t depth; // always 4 bytes (4 channels) for simplicity
} image2d;

image2d* image2d_new(uint32_t width, uint32_t height);

void image2d_draw_point(image2d* image, im_point2d start, im_color4 color, im_brush2d brush);

void image2d_draw_line(image2d* image, im_point2d start, im_point2d end, im_color4 color, im_brush2d brush);

#endif //IMAGELIB_IMLIB_H
