#ifndef IMAGELIB_TTF_H
#define IMAGELIB_TTF_H

#include <stdint.h>

typedef struct
{
    uint32_t type;
    uint16_t num_tables;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
} ttf_directory;

typedef struct
{
    uint32_t version;
    uint32_t revision;
    uint32_t checksum_adjustment;
    uint32_t magic_number;
    uint16_t flags;
    uint16_t units_per_em;
    uint32_t date_modified;
    uint32_t date_created;
    float x_min;
    float y_min;
    float x_max;
    float y_max;
    uint16_t mac_style;
    uint16_t lowest_rec_ppem;
    uint16_t font_directory_hint;
    uint16_t index_to_local_format;
    uint16_t glyph_data_format;
} ttf_header;

typedef struct {
    uint8_t tag_str[4];
    uint32_t check_sum;
    uint32_t offset;
    uint32_t length;
} ttf_entry;

typedef struct {
    uint32_t fixed;
    uint16_t num_glyphs;
} ttf_maxp;

typedef struct  {
    int32_t x;
    int32_t y;
    uint8_t on_curve;
} ttf_glyph_point;

typedef struct {
    uint16_t* end_points;
    uint16_t instruction_length;
    uint8_t* instructions;
    uint8_t* flags;
    ttf_glyph_point* points;
    uint32_t points_length;
} ttf_simple_glyph;

typedef struct {
    int16_t num_contours;
    uint8_t is_simple;
    float x_min;
    float y_min;
    float x_max;
    float y_max;
    union {
        ttf_simple_glyph simple_glyph;
    };
} ttf_glyph;

typedef struct {
    union uint16_t* offset16;
    union uint16_t* offset32;
} ttf_loca;

typedef struct {
    ttf_directory* font_directory;
    ttf_entry* glyph_entry;
    ttf_entry* maxp_entry;
    ttf_entry* cmap_entry;
    ttf_entry* head_entry;
    ttf_entry* loca_entry;
    ttf_glyph* glyphs;
    ttf_maxp* maxp;
    ttf_header* header;
} ttf_file;

ttf_file* read_ttf(char* path);

#endif