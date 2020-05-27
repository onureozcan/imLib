//
// Created by onureozcan on 26.05.2020.
//
#include <stdio.h>
#include <imlib.h>
#include <math.h>

#define PI 3.14159265

void line_test(image2d *image) {
    im_point2d start = im_point(400, 400);

    for (int y = 0; y < 7; y++) {
        image2d_draw_line(image, im_point(100, 650 + y * 20), im_point(700, 650 + y * 20), im_color(255, 255, 255, y * 25 + 15),
                          im_brush(35 - y * 3, 255, IM_BRUSH_SHAPE_CIRCULAR));
    }

    for (int i = 0; i < 360; i += 45) {
        for (int j = 2; j < 10; j++) {
            float y = (sin((i + j * 5) * (PI / 180)) * 200 + 400);
            float x = (cos((i + j * 5) * (PI / 180)) * 200 + 400);
            image2d_draw_line(image, start, im_point(x, y), im_color(x, y, i, 200),
                              im_brush(j, 255, IM_BRUSH_SHAPE_CIRCULAR));
        }
    }
}

void point_test(image2d *image) {

    for (int j = 1; j < 4; j++) {
        for (int i = 1; i < 15; i++) {
            image2d_draw_point(image, im_point(150 + (i * 30), 50 * j), im_color(255, 255, 255, 255 - j * 80),
                               im_brush(2 * i, 1, IM_BRUSH_SHAPE_CIRCULAR));
        }
    }
}

int main() {
    image2d *image = image2d_new(800, 800);
    if (!image)
        return 1;
    point_test(image);
    line_test(image);

    FILE *out = fopen("../out.ppm", "wb");
    fprintf(out, "P6\n%d %d\n255\n", image->width, image->height);
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            uint8_t *data = im_where(image, j, i);
            fwrite(data, 3, 1, out);
        }
    }
}