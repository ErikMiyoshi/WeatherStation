
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_TEMP
#define LV_ATTRIBUTE_TEMP
#endif

#include "temp.h"

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_TEMP
uint8_t temp_map[] = {

    0x00,0x00,0x00,0xa7,0x00,0x00,0x00,0x2b,

    0xe3,
    0xdb,
    0xdb,
    0xdb,
    0xd3,
    0xc3,
    0xc3,
    0xd3,
    0xc3,
    0x91,
    0xa5,
    0x24,
    0x42,
    0x24,
    0x99,
    0xc3,

};

const lv_image_dsc_t temp = {
  .header = {
    .magic = LV_IMAGE_HEADER_MAGIC,
    .cf = LV_COLOR_FORMAT_I1,
    .flags = 0,
    .w = 8,
    .h = 16,
    .stride = 1,
    .reserved_2 = 0,
  },
  .data_size = sizeof(temp_map),
  .data = temp_map,
  .reserved = NULL,
};

