//
// Created by onureozcan on 26.05.2020.
//

#include <imlib.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <ttf.h>

static im_color4 current_color;
static im_brush2d current_brush;
static ttf_file *font;

// y = ax*x + bx + c
// c = 1
// a = -b - 1
static float get_hardness_b_by_pd(float d) {
    return (d + d * d - 1) / (d - d * d);
}

static float f_abs(float f) {
    return f < 0 ? -f : f;
}

static uint8_t blend(uint8_t a, uint8_t b, uint8_t alpha) {
    return (a * alpha + b * (255 - alpha)) / 255;
}

static void put_pixel(const image2d *image, im_color4 color, uint32_t x, uint32_t y, uint8_t a) {
    uint8_t *pixel = im_where(image, x, y);
    a = (color.a * a) / 255;
    pixel[0] = blend(color.r, pixel[0], a);
    pixel[1] = blend(color.g, pixel[1], a);
    pixel[2] = blend(color.b, pixel[2], a);
    pixel[3] = a;
}

#ifdef USE_ALPHA_BUFF

static void put_pixel_alpha_buff(const image2d *image, uint32_t x, uint32_t y, uint8_t a) {
    uint32_t *alpha_buffer = im_alpha_where(image, x, y);
    uint32_t old_alpha = *alpha_buffer;
    uint32_t new_alpha = old_alpha + a;
    if (new_alpha > 255) {
        new_alpha = 255;
    }
    *alpha_buffer = new_alpha;
}

#endif

static void put_pixel_aa(const image2d *image, float xf, float yf, uint8_t a) {

    int floor_x = floor(xf);
    int ceil_x = ceil(xf);
    int floor_y = floor(yf);
    int ceil_y = ceil(yf);

    for (int x = floor_x; x <= ceil_x; x++) {
        for (int y = floor_y; y <= ceil_y; y++) {
            float percent_x = 1 - f_abs(x - xf);
            float percent_y = 1 - f_abs(y - yf);
            float percent = percent_x * percent_y;
#ifndef USE_ALPHA_BUFF
            put_pixel(image, current_color, x, y, round(percent * a));
#else
            put_pixel_alpha_buff(image, x, y, round(percent * a));
#endif
        }
    }
}

static void put_point(const image2d *image, float xf, float yf) {

    im_brush2d brush = current_brush;
    float half_brush_size = brush.size / 2;
    float floor_x = (xf - half_brush_size);
    float ceil_x = (xf + half_brush_size);
    float floor_y = (yf - half_brush_size);
    float ceil_y = (yf + half_brush_size);
    float r = half_brush_size;

    float hardness_b = get_hardness_b_by_pd(current_brush.hardness);
    float hardness_a = -hardness_b - 1;

    int steps_x = abs((int) (floor_x - ceil_x)) + 1;
    for (float x = floor_x; steps_x > 0; steps_x--, x++) {
        int steps_y = abs((int) (floor_y - ceil_y)) + 1;
        for (float y = floor_y; steps_y > 0; steps_y--, y++) {
            float x_dist = (xf - x) / r;
            float y_dist = (yf - y) / r;
            float dist = sqrt(x_dist * x_dist + y_dist * y_dist);
            float alpha;
            if (dist < r) {
                alpha = (hardness_a * dist * dist) + hardness_b * dist + 1;
                if (alpha < 0) {
                    alpha = 0;
                }
                if (alpha > 1) {
                    alpha = 1;
                }
                put_pixel_aa(image, x, y, (int8_t) (alpha * 255));
            }
        }
    }
}

static void begin_drawing(image2d *image, im_color4 color, im_brush2d brush) {
#ifdef USE_ALPHA_BUFF
    memset(image->alpha_buffer, 0,
           image->width * image->height *
           sizeof(uint32_t));
#endif
    current_color = color;
    current_brush = brush;
    if (current_brush.hardness >= 1) {
        current_brush.hardness = .99f;
    }
    if (current_brush.hardness <= 0) {
        current_brush.hardness = .01f;
    }
    current_brush.hardness = .5f + current_brush.hardness / 2;
}

void commit(image2d *image) {
#ifndef USE_ALPHA_BUFF
#else

    for (int x = 0; x < image->width; x++) {
        for (int y = 0; y < image->height; y++) {
            uint32_t *alpha_buffer = im_alpha_where(image, x, y);
            uint8_t a = *alpha_buffer;
            put_pixel(image, current_color, x, y, a);
        }
    }
#endif

}

/// ------------------ api methods

void imlib_init() {
    font = read_ttf("../FreeSans.ttf");
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
#ifdef USE_ALPHA_BUFF
    ret->alpha_buffer = ret->data + width * height;
#endif
    return ret;
}

static void dda(const image2d *image, im_point2d *start, im_point2d *end) {
    // DDA line drawing algorithm
    float x, y, x1 = (*start).x, y1 = (*start).y, x2 = (*end).x, y2 = (*end).y, dx, dy, step;
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

    i = 0;
    while (i <= step) {
        put_point(image, x, y);
        x = x + dx;
        y = y + dy;
        i = i + 1;
    }
}

void image2d_draw_line(image2d *image, im_point2d start, im_point2d end, im_color4 color, im_brush2d brush) {

    begin_drawing(image, color, brush);
    dda(image, &start, &end);
    commit(image);
}

void image2d_draw_point(image2d *image, im_point2d center, im_color4 color, im_brush2d brush) {
    begin_drawing(image, color, brush);
    put_point(image, center.x, center.y);
    commit(image);
}

static im_point2d get_interpolated_point(im_point2d p1, im_point2d p2, float t) {
    im_point2d p;
    p.x = (p1.x + t * (p2.x - p1.x));
    p.y = (p1.y + t * (p2.y - p1.y));
    return p;
}

static im_point2d get_bezier_point(im_point2d *points, int n, float t) {
    if (n == 2) {
        return get_interpolated_point(points[0], points[1], t);
    } else {
        im_point2d *points_derived = (im_point2d *) malloc(sizeof(im_point2d) * (n));
        memcpy(points_derived, points, sizeof(im_point2d) * (n));
        while (n > 2) {
            n--;
            for (int i = 0; i < n; i++) {
                points_derived[i] = get_interpolated_point(points_derived[i], points_derived[i + 1], t);
            }
        }
        im_point2d ret = get_interpolated_point(points_derived[0], points_derived[1], t);
        free(points_derived);
        return ret;
    }
}

void image2d_draw_bezier_n(image2d *image, im_point2d *points, int n, im_color4 color, im_brush2d brush) {
    begin_drawing(image, color, brush);
    im_point2d prev = points[0];
    im_point2d current;
    int t_limit = 500;
    for (int i = 1; i <= t_limit; i++) {
        float t = (float) i / t_limit;
        current = get_bezier_point(points, n, t);
        image2d_draw_line(image, prev, current, color, brush);
        prev = current;
    }
}

void image2d_draw_bezier3(image2d *image, im_point2d start, im_point2d control, im_point2d end, im_color4 color,
                          im_brush2d brush) {
    image2d_draw_bezier_n(image,((im_point2d[3]){ start, control, end }), 3, color, brush);
}

void
image2d_draw_char(image2d *image, im_point2d start, char character, uint16_t size, im_color4 color, im_brush2d brush) {

    int index = (int)character - 29;
    ttf_glyph glyph = font->glyphs[index];
    if (!glyph.is_simple || !glyph.num_contours) {
        return;
    }
    int c = 0, first = 1;

    ttf_glyph_point current_point;

    im_point2d start_point;
    im_point2d bezier_points[50];

    int bezier_points_size = 0;

    for (int i = 0; i < glyph.simple_glyph.points_length; i++) {

        current_point = glyph.simple_glyph.points[i];
        im_point2d current = im_point(current_point.x * .5 + start.x, (glyph.y_max - current_point.y) * .5 + start.y);
        bezier_points[bezier_points_size++] = current;

        if (first) {
            first = 0;
            start_point = current;
            bezier_points_size = 0;
        }

        if (current_point.on_curve) {
            if (bezier_points_size > 1)
                image2d_draw_bezier_n(image, bezier_points, bezier_points_size, color, brush);
            bezier_points_size = 0;
            image2d_draw_point(image, current, im_color(255,0,0,255), im_brush(5,.5,IM_BRUSH_SHAPE_CIRCULAR));
            bezier_points[bezier_points_size++] = current;
        } else {
            image2d_draw_point(image, current, im_color(0,255,0,255), im_brush(5,.5,IM_BRUSH_SHAPE_CIRCULAR));
        }

        if (i == glyph.simple_glyph.end_points[c]) {
            c += 1;
            first = 1;
            bezier_points[bezier_points_size++] = start_point;
            if (bezier_points_size > 1){
                image2d_draw_bezier_n(image, bezier_points, bezier_points_size, im_color(255,255,0,255), brush);
            }
        }
    }
}
