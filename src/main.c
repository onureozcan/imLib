//
// Created by onureozcan on 26.05.2020.
//
#include <stdio.h>
#include <imlib.h>


void line_test(image2d *image) {
    im_brush2d brush = im_brush(2, 255);
    image2d_draw_line(image, im_point(200, 200), im_point(22, 500), im_color(255, 255, 255, 255), brush);

    image2d_draw_line(image, im_point(400, 400), im_point(450, 500), im_color(255, 0, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(400, 500), im_color(0, 0, 255, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(500, 500), im_color(0, 255, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(500, 450), im_color(0, 0, 255, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(500, 400), im_color(255, 0, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(500, 350), im_color(0, 255, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(500, 300), im_color(0, 0, 255, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(450, 300), im_color(255, 0, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(400, 300), im_color(0, 255, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(350, 300), im_color(0, 0, 255, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(300, 300), im_color(255, 0, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(300, 350), im_color(0, 255, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(300, 400), im_color(0, 0, 255, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(300, 450), im_color(255, 0, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(300, 500), im_color(0, 255, 0, 255), brush);
    image2d_draw_line(image, im_point(400, 400), im_point(350, 500), im_color(255, 0, 0, 255), brush);
}

void point_test(image2d *image) {
    image2d_draw_point(image, im_point(250, 100), im_color(255, 255, 255, 255), im_brush(2, 1));
    image2d_draw_point(image, im_point(200, 100), im_color(255, 255, 255, 255), im_brush(5, 1));
    image2d_draw_point(image, im_point(150, 100), im_color(255, 255, 255, 255), im_brush(7, 1));
    image2d_draw_point(image, im_point(100, 100), im_color(255, 255, 255, 255), im_brush(10, 1));
    image2d_draw_point(image, im_point(50, 100), im_color(255, 255, 255, 255), im_brush(40, 1));
}

int main() {
    image2d *image = image2d_new(800, 800);
    if (!image)
        return 1;
    point_test(image);
    line_test(image);

    FILE *out = fopen("../out.ppm", "wb");
    fprintf(out, "P6\n%d %d\n255\n", image->width, image->height);
    for (int i = 0; i< image->height; i++) {
        for (int j = 0; j< image->width; j++) {
            uint8_t* data = im_where(image,j,i);
            fwrite(data, 3,1,out);
        }
    }
}