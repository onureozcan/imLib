//
// Created by onureozcan on 26.05.2020.
//

#include <imlib.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static im_color4 current_color;
static int commit_count = 0;

static float f_abs(float f) {
    return f < 0 ? -f : f;
}

static void put_pixel(const image2d *image, im_color4 color, uint32_t x, uint32_t y, uint8_t a) {
    uint8_t *pixel = im_where(image, x, y);
    pixel[0] = (color.r * a + pixel[0] * (255 - a)) / 255;
    pixel[1] = (color.g * a + pixel[1] * (255 - a)) / 255;
    pixel[2] = (color.b * a + pixel[2] * (255 - a)) / 255;
    pixel[3] = a;
}

static void put_pixel_alpha_buff(const image2d *image, uint32_t x, uint32_t y, uint8_t a) {
    uint32_t *alpha_buffer = im_alpha_where(image, x, y);
    uint32_t old_alpha = *alpha_buffer;
    uint32_t new_alpha = old_alpha + a;
    if (new_alpha > 255){
        new_alpha = 255;
    }
    *alpha_buffer = new_alpha;
}

static void put_pixel_aa(const image2d *image, im_color4 color, float xf, float yf, uint8_t a) {

    int floor_x = floor(xf);
    int ceil_x = ceil(xf);
    int floor_y = floor(yf);
    int ceil_y = ceil(yf);

    for (int x = floor_x; x <= ceil_x; x++) {
        for (int y = floor_y; y <= ceil_y; y++) {
            float percent_x = 1 - f_abs(x - xf);
            float percent_y = 1 - f_abs(y - yf);
            float percent = percent_x * percent_y;
            put_pixel_alpha_buff(image, x, y, round(percent * a));
        }
    }

}

static void put_point(const image2d *image, im_color4 color, float xf, float yf, im_brush2d brush) {

    float half_brush_size = brush.size / 2;
    float floor_x = (xf - half_brush_size);
    float ceil_x = (xf + half_brush_size);
    float floor_y = (yf - half_brush_size);
    float ceil_y = (yf + half_brush_size);
    float r = half_brush_size;

    int steps_x = abs((int) (floor_x - ceil_x)) + 1;
    for (float x = floor_x; steps_x > 0; steps_x--, x++) {
        int steps_y = abs((int) (floor_y - ceil_y)) + 1;
        for (float y = floor_y; steps_y > 0; steps_y--, y++) {
            float x_dist = (xf - x) / r;
            float y_dist = (yf - y) / r;
            float dist = x_dist * x_dist + y_dist * y_dist;
            float alpha = (2 - dist) / 2;
            alpha *= alpha;
            alpha *= alpha;
            put_pixel_aa(image, color, x, y, (int8_t) (alpha * 255));
        }
    }
}

void begin_drawing(image2d *image, im_color4 color) {
    memset(image->alpha_buffer, 0,
           image->width * image->height *
           sizeof(uint32_t));
    current_color = color;
}

void commit(image2d *image) {
    for (int x = 0; x < image->width; x++) {
        for (int y = 0; y < image->height; y++) {
            uint32_t *alpha_buffer = im_alpha_where(image, x, y);
            uint8_t a = *alpha_buffer;
            put_pixel(image, current_color, x, y,a);
        }
    }
    /*char* name = malloc(500);
    sprintf(name,"../out_alpha_%d.ppm",commit_count++);
    FILE *out = fopen(name, "wb");
    fprintf(out, "P6\n%d %d\n255\n", image->width, image->height);
    for (int i = 0; i< image->height; i++) {
        for (int j = 0; j< image->width; j++) {
            uint8_t* data = im_alpha_where(image,j,i);
            fwrite(data, 3,1,out);
        }
    }*/
}

image2d *image2d_new(uint32_t width, uint32_t height) {
    image2d *ret = (image2d *) malloc(sizeof(image2d) + width * height * (IM_DEPTH_RGBA_32 + sizeof(uint32_t)));
    if (!ret) {
        return NULL;
    }
    ret->data = (uint32_t *) ((uint8_t *) ret + sizeof(image2d));
    ret->width = width;
    ret->height = height;
    ret->depth = IM_DEPTH_RGBA_32;
    ret->alpha_buffer = ret->data + width * height;

    return ret;
}

void image2d_draw_line(image2d *image, im_point2d start, im_point2d end, im_color4 color, im_brush2d brush) {

    begin_drawing(image, color);

    // DDA line drawing algorithm
    float x, y, x1 = start.x, y1 = start.y, x2 = end.x, y2 = end.y, dx, dy, step;
    int i;

    dx = (x2 - x1);
    dy = (y2 - y1);

    if (abs(dx) > abs(dy))
        step = dx;
    else
        step = dy;

    if (step < 0)
        step = -step;

    dx = dx / step;
    dy = dy / step;

    x = x1;
    y = y1;

    i = 1;
    while (i <= step) {
        put_point(image, color, x, y, brush);
        x = x + dx;
        y = y + dy;
        i = i + 1;
    }
    commit(image);
}

void image2d_draw_point(image2d *image, im_point2d start, im_color4 color, im_brush2d brush) {
    begin_drawing(image, color);
    put_point(image, color, start.x, start.y, brush);
    commit(image);
}
