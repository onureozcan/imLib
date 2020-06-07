#include <ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ON_CURVE 1
#define X_SHORT 1
#define Y_SHORT 2
#define REPEAT 8
#define X_SAME 4
#define Y_SAME 5

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static uint8_t read_uint8(FILE *fp) {
    uint8_t data = 0;
    fread(&data, 1, sizeof(uint8_t), fp);
    return data;
}

static uint16_t read_uint16(FILE *fp) {
    return (read_uint8(fp) << 8) | (read_uint8(fp));
}

static int16_t read_int16(FILE *fp) {
    int16_t result = read_uint16(fp);
    if (result & 0x8000) {
        result -= (1 << 16);
    }
    return result;
}

static uint32_t read_uint32(FILE *fp) {
    return ((read_uint8(fp) << 24) |
            (read_uint8(fp) << 16) |
            (read_uint8(fp) << 8) |
            (read_uint8(fp)));
}

static int tagcmp(char *tag, char tag_str[4]) {
    for (int i = 0; i < 4; i++) {
        if (tag[i] != tag_str[i]) {
            return 1;
        }
    }
    return 0;
}

static void read_header(ttf_file *ttf, FILE *file) {
    ttf_header *header = (ttf_header *) malloc(sizeof(ttf_header));
    if (!header) {
        return;
    }
    fseek(file, ttf->head_entry->offset, SEEK_SET);
    header->version = read_uint32(file);
    header->revision = read_uint32(file);
    header->checksum_adjustment = read_uint32(file);
    header->magic_number = read_uint32(file);
    assert(header->magic_number == 0x5f0f3cf5);
    header->flags = read_uint16(file);
    header->units_per_em = read_uint16(file);
    header->date_created = read_uint32(file) | (read_uint32(file) >> 4);
    header->date_modified = read_uint32(file) | (read_uint32(file) >> 4);
    header->x_min = read_uint16(file);
    header->y_min = read_uint16(file);
    header->x_max = read_uint16(file);
    header->y_max = read_uint16(file);
    header->mac_style = read_uint16(file);
    header->lowest_rec_ppem = read_uint16(file);
    header->font_directory_hint = read_uint16(file);
    header->index_to_local_format = read_uint16(file);
    header->glyph_data_format = read_uint16(file);
    ttf->header = header;
}

static void read_maxp(ttf_file *ttf, FILE *file) {
    ttf_maxp *maxp = (ttf_maxp *) malloc(sizeof(ttf_maxp));
    if (!maxp) {
        return;
    }
    fseek(file, ttf->maxp_entry->offset, SEEK_SET);
    maxp->fixed = read_uint32(file);
    maxp->num_glyphs = read_uint16(file);
    ttf->maxp = maxp;
}

static uint32_t get_glyph_offset(int index, ttf_file *ttf, FILE *file) {
    uint32_t offset;
    if (ttf->header->index_to_local_format == 1) {
        fseek(file, ttf->loca_entry->offset + index * 4, SEEK_SET);
        offset = read_uint32(file);
    } else {
        fseek(file, ttf->loca_entry->offset + index * 2, SEEK_SET);
        offset = read_uint16(file) * 2;
    }
    return ttf->glyph_entry->offset + offset;
}

static void read_simple_glyph(ttf_file *ttf, FILE *file, ttf_glyph *glyph) {
    ttf_simple_glyph simple = glyph->simple_glyph;
    simple.end_points = malloc(glyph->num_contours * sizeof(uint16_t));
    if (simple.end_points == NULL) {
        return;
    }
    int num_points = 0;
    for (int i = 0; i < glyph->num_contours; i++) {
        simple.end_points[i] = read_uint16(file);
        if (num_points < simple.end_points[i]) {
            num_points = simple.end_points[i];
        }
    }
    num_points++;

    simple.points_length = num_points;

    simple.flags = malloc(sizeof(uint8_t) *  num_points);
    simple.points = malloc(sizeof(ttf_glyph_point) * num_points);

    if (!simple.flags || !simple.points) {
        return;
    }

    fseek(file,read_uint16(file) + ftell(file), SEEK_SET);

    for(int i = 0; i < num_points; i++ ) {
        uint8_t flag = read_uint8(file);
        simple.flags[i] = flag;
        if (flag & REPEAT) {
            uint8_t repeatCount = read_uint8(file);
            i += repeatCount;
            while( repeatCount-- ) {
                simple.flags[i - repeatCount] = flag;
            }
        }
    }

    uint32_t value_x = 0;
    uint32_t value_y = 0;

    for(int i = 0; i < num_points; i++ ) {
        uint8_t flag = simple.flags[i];
        if ( CHECK_BIT(flag, X_SHORT)) {
            if (CHECK_BIT(flag, X_SAME)) {
                value_x += read_uint8(file);
            } else {
                value_x -= read_uint8(file);
            }
        } else if (CHECK_BIT(~flag, X_SAME)) {
            value_x += read_int16(file);
        } else {
            // value is unchanged.
        }
        simple.points[i].x = value_x;
        simple.points[i].on_curve = (flag & ON_CURVE) > 0;
    }
    for(int i = 0; i < num_points; i++ ) {
        uint8_t flag = simple.flags[i];
        if ( CHECK_BIT(flag, Y_SHORT)) {
            if (CHECK_BIT(flag, Y_SAME)) {
                value_y += read_uint8(file);
            } else {
                value_y -= read_uint8(file);
            }
        } else if (CHECK_BIT(~flag, Y_SAME)) {
            value_y += read_int16(file);
        } else {
            // value is unchanged.
        }
        simple.points[i].y = value_y;
    }
    glyph->simple_glyph = simple;
}

static void read_glyph(int index, ttf_file *ttf, FILE *file) {
    uint32_t offset = get_glyph_offset(index, ttf, file);
    fseek(file, offset, SEEK_SET);

    ttf_glyph *glyph = &ttf->glyphs[index];
    glyph->num_contours = read_int16(file);
    glyph->x_min = read_int16(file);
    glyph->y_min = read_int16(file);
    glyph->x_max = read_int16(file);
    glyph->y_max = read_int16(file);

    if (glyph->num_contours < 0) {
        // compound glpy
        glyph->is_simple = 0;
    } else {
        glyph->is_simple = 1;
        read_simple_glyph(ttf, file, glyph);
    }
}

static void read_glyphs(ttf_file *ttf, FILE *file) {
    ttf_glyph* glyphs = (ttf_glyph*) malloc(sizeof(ttf_glyph) * ttf->maxp->num_glyphs);
    if (!glyphs) {
        return;
    }
    ttf->glyphs = glyphs;
    fseek(file, ttf->glyph_entry->offset, SEEK_SET);
    for (int i = 0; i < ttf->maxp->num_glyphs; i++) {
        read_glyph(i, ttf, file);
    }
}

ttf_file *read_ttf(char *path) {
    FILE *file = fopen(path, "rb");
    ttf_file *ttf = (ttf_file *) malloc(sizeof(ttf_file));
    ttf_directory *directory = (ttf_directory *) malloc(sizeof(ttf_directory));
    if (!file || !directory || !ttf) {
        return NULL;
    }
    directory->type = read_uint32(file);
    directory->num_tables = read_uint16(file);
    directory->search_range = read_uint16(file);
    directory->entry_selector = read_uint16(file);
    directory->range_shift = read_uint16(file);

    ttf->font_directory = directory;
    ttf_entry *entries = (ttf_entry *) malloc(sizeof(ttf_entry) * directory->num_tables);
    if (!entries) {
        return NULL;
    }
    ttf_entry *entry = entries;
    for (int i = 0; i < directory->num_tables; i++) {

        entry->tag_str[0] = (char) read_uint8(file);
        entry->tag_str[1] = (char) read_uint8(file);
        entry->tag_str[2] = (char) read_uint8(file);
        entry->tag_str[3] = (char) read_uint8(file);
        entry->check_sum = read_uint32(file);
        entry->offset = read_uint32(file);
        entry->length = read_uint32(file);

        if (tagcmp("glyf", entry->tag_str) == 0) {
            ttf->glyph_entry = entry;
        } else if (tagcmp("maxp", entry->tag_str) == 0) {
            ttf->maxp_entry = entry;
        } else if (tagcmp("cmap", entry->tag_str) == 0) {
            ttf->cmap_entry = entry;
        } else if (tagcmp("head", entry->tag_str) == 0) {
            ttf->head_entry = entry;
        } else if (tagcmp("loca", entry->tag_str) == 0) {
            ttf->loca_entry = entry;
        }
        entry++;
    }

    read_header(ttf, file);
    read_maxp(ttf, file);
    read_glyphs(ttf, file);

    return ttf;
}