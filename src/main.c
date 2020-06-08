//
// Created by onureozcan on 26.05.2020.
//
#include <stdio.h>
#include <imlib.h>
#include <math.h>

#define PI 3.14159265

void line_test(image2d *image) {

    for (int y = 1; y < 8; y++) {
        image2d_draw_line(image, im_point(100, 400 + y * 20), im_point(300, 400 + y * 20), im_color(255, 255, 255, 20),
                          im_brush(y * 2, .1, IM_BRUSH_SHAPE_CIRCULAR));
    }

    for (int y = 1; y < 8; y++) {
        image2d_draw_line(image, im_point(350, 400 + y * 20), im_point(550, 400 + y * 20), im_color(255, 255, 255, 20),
                          im_brush(y * 2, 1, IM_BRUSH_SHAPE_CIRCULAR));
    }

    float start_x = 200, start_y = 300, r = 100;
    im_point2d start = im_point(start_x, start_y);

    float hardness = .1;
    uint32_t alpha = 10;
    float size = 2;
    for (int i = 10; i < 370; i += 10) {
        hardness += .1f;
        alpha += 5;
        size += .1f;
        float y = (sin((i) * (PI / 180)) * r + start_y);
        float x = (cos((i) * (PI / 180)) * r + start_x);
        image2d_draw_line(image, start, im_point(x, y), im_color(255 - alpha, 255 + alpha, 255, 255),
                          im_brush(size, hardness, IM_BRUSH_SHAPE_CIRCULAR));
    }
}

void point_test(image2d *image) {
    for (int j = 1; j < 7; j++) {
        for (int i = 1; i < 15; i++) {
            image2d_draw_point(image, im_point(70 + (i * 33), 30 * j), im_color(255, 255, 255, 200),
                               im_brush(2 * i, j * .15, IM_BRUSH_SHAPE_CIRCULAR));
        }
    }
}

void bezier_test(image2d *image) {
    for (int i = 1; i < 10; i++) {
        image2d_draw_bezier3(image, im_point(350, 250 + i * 15), im_point(600, 150 + i * 15),
                             im_point(520, 250 + i * 15),
                             im_color(255 + i * 5, 255 * i * 25, i * 8, 250),
                             im_brush(1 + i, 0, IM_BRUSH_SHAPE_CIRCULAR));
    }
}

void ttf_test(image2d *image) {
    char *test = "i am typing now!";
    float x = 100;
    for (int i = 0; i < strlen(test); i++) {
        x = image2d_draw_char(image, im_point(x, 580), test[i], 45, im_color(50, 255, 10 + i * 80, 250),
                              im_brush(3, 0, IM_BRUSH_SHAPE_CIRCULAR));
    }
}

void filled_shape_test(image2d *image) {
    im_point2d points[] = {im_point(180, 650),
                           im_point(80, 750),
                           im_point(150, 750),
                           im_point(250, 700),
                           im_point(550, 750)};
    image2d_draw_shape(image, points, 5, im_color(255, 255, 255, 127), im_brush(3, 0, IM_BRUSH_SHAPE_CIRCULAR), 1);
}

int main() {

    imlib_init();
    image2d *image = image2d_new(800, 800);
    if (!image)
        return 1;
    point_test(image);
    line_test(image);
    bezier_test(image);
    ttf_test(image);
    filled_shape_test(image);

    FILE *out = fopen("../out.ppm", "wb");
    fprintf(out, "P6\n%d %d\n255\n", image->width, image->height);
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            uint8_t *data = im_where(image, j, i);
            fwrite(data, 3, 1, out);
        }
    }
}