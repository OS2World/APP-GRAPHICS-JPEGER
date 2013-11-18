#ifndef JPEGER_H
#define JPEGER_H

/*
 * HISTORY
 *
 * v0.0  initial code to display compression/decompression windows, SC, 1998June
 * v0.1  first version to read & display a JPEG file, SC, 1998July1
 * v0.2  added support for BMP, GIF, PCX, TIF, and other formats, SC, 1998August
 * v0.3  started coding "resize" dialog, SC, 1998August27-28
 * v0.4  first set of source files uploaded to Hobbes, 1998September15
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * JPEGER is based in part on the work of the Independent
 * JPEG Group at ftp://ftp.uu.net/graphics/jpeg
 *
 */

#define JPEGER_TITLE                   "JPEGER - v0.4 [beta]"
#define JPEGER_NAME                    "Stephane Charette"
#define JPEGER_EMAIL                   "charette@writeme.com"


#define BACK_COLOUR                    IColor(128,192,160)


#define WND_MAIN                       100
#define WND_BANNER                     101
#define WND_DECOMPRESSION              102
#define WND_COMPRESSION                103
#define WND_MOODIES                    104
#define WND_RESIZE                     105

#define BM_BANNER                      1100
#define EF_BYTES_1                     1101
#define EF_COLOURS_1                   1102
#define EF_SIZE_1                      1103
#define EF_BYTES_2                     1104
#define EF_COLOURS_2                   1105
#define EF_SIZE_2                      1106
#define MI_HORIZONTAL                  1107
#define MI_VERTICAL                    1108
#define MI_SOURCE                      1109
#define MI_DESTINATION                 1110
#define MI_ROTATE_90                   1111
#define MI_FLIP_HORIZONTAL             1112
#define MI_FLIP_VERTICAL               1113
#define MI_TRANSPOSE                   1114
#define MI_COMPRESS_OPTIONS            1115
#define MI_DECOMPRESS_OPTIONS          1116
#define MI_CROP                        1117
#define MI_OPEN                        1118
#define MI_SAVE                        1119
#define MI_SAVE_AS                     1120
#define MI_ABOUT                       1121
#define MI_CLOSE                       1122
#define MI_RESIZE                      1123

#define RB_USE_24_BIT_COLOUR           1200
#define RB_QUANTIZE_COLOURS            1201
#define SL_NUMBER_OF_COLOURS           1202
#define EF_NUMBER_OF_COLOURS           1203
#define RB_USE_1_PASS_QUANTIZATION     1204
#define RB_USE_2_PASS_QUANTIZATION     1205
#define RB_NO_DITHERING                1206
#define RB_ORDERED_DITHERING           1207
#define RB_FLOYD_STEINBERG_DITHERING   1208
#define CK_FANCY_UPSAMPLING            1209
#define CK_BLOCK_SMOOTHING             1210
#define TXT_NUMBER_OF_COLOURS          1211

#define RB_COMPRESS_SLOW_INTEGER       1300
#define RB_COMPRESS_FAST_INTEGER       1301
#define RB_COMPRESS_FLOAT              1302
#define CK_HUFFMAN_OPTIMIZATION        1303
#define SL_QUALITY                     1304
#define EF_QUALITY                     1305
#define SL_SMOOTHING                   1306
#define EF_SMOOTHING                   1307
#define CK_FORCE_BASELINE              1308
#define CK_PROGRESSIVE_JPEG            1309

#define PB_RATIO                       1500
#define EF_RATIO_1                     1501
#define EF_RATIO_2                     1502
#define RB_CUSTOM_SIZE                 1503
#define RB_PREVIOUS_SIZE               1504
#define SB_SIZE_X                      1505
#define SB_SIZE_Y                      1506
#define CK_KEEP_PROPORTIONAL           1507
#define RB_SIZE_1                      1508
#define RB_SIZE_2                      1509
#define RB_SIZE_3                      1510

#endif

