/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/interp.h"

#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#include "pico/multicore.h"

#include "ili9341_lcd.pio.h"

#define PIN_DIN 3
#define PIN_CLK 7
#define PIN_CS 1
#define PIN_DC 4
#define PIN_RESET 0
#define PIN_BL 1

//#define SERIAL_CLK_DIV 125.f
//#define SERIAL_CLK_DIV 1.f
//#define SERIAL_CLK_DIV 1.4f
#define SERIAL_CLK_DIV 1.5f
//#define SERIAL_CLK_DIV 2.f
//#define SERIAL_CLK_DIV 6.25f


// Format: cmd length (including cmd byte), post delay in units of 5 ms, then cmd payload
// Note the delays have been shortened a little
/* reference: https://github.com/adafruit/Adafruit_ILI9341.git */
static const uint8_t ili9341_init_seq[] = {
        4, 1, 0xEF, 0x03, 0x80, 0x02,
        4, 1, 0xCF, 0x00, 0xC1, 0x30,
        5, 1, 0xED, 0x64, 0x03, 0x12, 0x81,
        4, 1, 0xE8, 0x85, 0x00, 0x78,
        6, 1, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
        2, 1, 0xF7, 0x20,
        3, 1, 0xEA, 0x00, 0x00,
        2, 1, 0xC0, 0x23,
        2, 1, 0xC1, 0x10,
        3, 1, 0xC5, 0x3E, 0x28,
        3, 1, 0xC5, 0x3E, 0x28,
        2, 1, 0xC7, 0x86,
        2, 1, 0x36, 0xe0, /* rotate-0: 0x48; rotate-1: 0x28; rotate-2: 0x88; rotate-3: 0xe8 */
        2, 1, 0x3A, 0x55,
        3, 1, 0xB1, 0x00, 0x18,
        4, 1, 0xB6, 0x08, 0x82, 0x27,
        2, 1, 0xF2, 0x00,
        2, 1, 0x26, 0x01,
       16, 1, 0xE0, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
       16, 1, 0xE1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        1, 200, 0x11,
        1, 1, 0x29,
        0
};

static inline void lcd_set_dc_cs(bool dc, bool cs) {
    sleep_us(1);
    gpio_put_masked((1u << PIN_DC) | (1u << PIN_CS), !!dc << PIN_DC | !!cs << PIN_CS);
    sleep_us(1);
}

static inline void lcd_write_cmd(PIO pio, uint sm, const uint8_t *cmd, size_t count) {
    ili9341_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(0, 0);
    ili9341_lcd_put(pio, sm, *cmd++);
    if (count >= 2) {
        ili9341_lcd_wait_idle(pio, sm);
        lcd_set_dc_cs(1, 0);
        for (size_t i = 0; i < count - 1; ++i)
            ili9341_lcd_put(pio, sm, *cmd++);
    }
    ili9341_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(1, 1);
}

static inline void lcd_init(PIO pio, uint sm, const uint8_t *init_seq) {
    const uint8_t *cmd = init_seq;
    while (*cmd) {
        lcd_write_cmd(pio, sm, cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
}

static inline void ili9341_start_pixels(PIO pio, uint sm) {
    uint8_t cmd = 0x2c; // RAMWR
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);
}

/*********** TEST START *******/

/* rotate-0 */
//#define ILI9341_TFTWIDTH  240
//#define ILI9341_TFTHEIGHT 320

/* rotate-1 */
//#define ILI9341_TFTWIDTH  320
//#define ILI9341_TFTHEIGHT 240

/* rotate-2 */
//#define ILI9341_TFTWIDTH  320
//#define ILI9341_TFTHEIGHT 240

/* rotate-3 */
#define ILI9341_TFTWIDTH  320
#define ILI9341_TFTHEIGHT 240


/* RGB565 MSB-RED-GREEN-BLUE-LSB */
// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F


void tft_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    PIO pio = pio0;
    uint sm = 0;
    uint8_t cmd;

    cmd = 0x2a;
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);


    ili9341_lcd_put(pio, sm, x0 >> 8);
    ili9341_lcd_put(pio, sm, x0 & 0xff);
    ili9341_lcd_put(pio, sm, x1 >> 8);
    ili9341_lcd_put(pio, sm, x1 & 0xff);


    cmd = 0x2b;
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);

    ili9341_lcd_put(pio, sm, y0 >> 8);
    ili9341_lcd_put(pio, sm, y0 & 0xff);
    ili9341_lcd_put(pio, sm, y1 >> 8);
    ili9341_lcd_put(pio, sm, y1 & 0xff);

    cmd = 0x2c;
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);

}

void tft_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

    PIO pio = pio0;
    uint sm = 0;
    // rudimentary clipping (drawChar w/big text requires this)
    if((x >= ILI9341_TFTWIDTH) || (y >= ILI9341_TFTHEIGHT)) return;
    if((x + w - 1) >= ILI9341_TFTWIDTH)  w = ILI9341_TFTWIDTH  - x;
    if((y + h - 1) >= ILI9341_TFTHEIGHT) h = ILI9341_TFTHEIGHT - y;


    tft_set_addr_window(x, y, x+w-1, y+h-1);

    uint8_t hi = color >> 8, lo = color;

    for(y=h; y>0; y--) {
        for(x=w; x>0; x--) {
            ili9341_lcd_put(pio, sm, hi);
            ili9341_lcd_put(pio, sm, lo);
        }
    }

}

#define NES_DISP_WIDTH  256
#define NES_DISP_HEIGHT 240
extern unsigned short *WorkFrame;
/* fb from nes, 256 x 240 */

//unsigned short framebuffer[256 * 240];

void __tft_flush()
{
    uint16_t x, y;
    uint16_t i = 0;
    uint8_t hi, lo;
    uint16_t black_color = ILI9341_BLACK;

    uint16_t *fb = (unsigned short *)&WorkFrame;
    if (fb == NULL) {
        return;
    }

    //uint16_t *fb = framebuffer;

    //memcpy((uint8_t *)framebuffer, (uint8_t *)&WorkFrame, sizeof(framebuffer));

    PIO pio = pio0;
    uint sm = 0;


#if 0
    tft_set_addr_window(0, 0, ILI9341_TFTWIDTH-1, ILI9341_TFTHEIGHT-1);

    for(y = 0; y < ILI9341_TFTHEIGHT; y++) {

        for(x = 0; x < NES_DISP_WIDTH; x++) {
            hi = fb[i] >> 8, lo = fb[i];
            ili9341_lcd_put(pio, sm, hi);
            ili9341_lcd_put(pio, sm, lo);
            i++;
        }

        for(x = 0; x < (ILI9341_TFTWIDTH - NES_DISP_WIDTH); x++) {
            hi = black_color >> 8, lo = black_color;
            ili9341_lcd_put(pio, sm, hi);
            ili9341_lcd_put(pio, sm, lo);
        }

    }
#else

    //tft_set_addr_window(0, 0, NES_DISP_WIDTH-1, NES_DISP_HEIGHT-1);

    for(y = 0; y < NES_DISP_HEIGHT; y++) {

        for(x = 0; x < NES_DISP_WIDTH; x++) {
            hi = fb[i] >> 8, lo = fb[i];
            ili9341_lcd_put(pio, sm, hi);
            ili9341_lcd_put(pio, sm, lo);
            i++;
        }

    }

#endif

}

void tft_flush()
{
#if 1
    static uint32_t frame_index = 0;
    multicore_fifo_push_blocking(frame_index++);
#else
    __tft_flush();
#endif
}

int tft_fill_screen(uint16_t color)
{
    tft_fill_rect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTWIDTH, color);
}

/*********** TEST END  *******/

extern int InfoNES_Load( const char *pszFileName );
extern void InfoNES_Main();

void core1_entry() 
{
    uint32_t m = 0;
    printf("core1 start0\n");

    while (1) {
        m = multicore_fifo_pop_blocking();
        //printf("c1-m [%d]\r\n", m);
        __tft_flush();
    }

}

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);

    // Can't measure clk_ref / xosc as it is the ref
}

int main() {
    stdio_init_all();

    printf("infones start\n");
    set_sys_clock_khz(200000, true);    /* work with spi clkdiv 1.5f */
    //set_sys_clock_khz(250000, true);  /* work with spi clkdiv 2.f */

    measure_freqs();

    multicore_launch_core1(core1_entry);

    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ili9341_lcd_program);
    ili9341_lcd_program_init(pio, sm, offset, PIN_DIN, PIN_CLK, SERIAL_CLK_DIV);

    gpio_init(PIN_CS);
    gpio_init(PIN_DC);
    gpio_init(PIN_RESET);
    gpio_init(PIN_BL);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_set_dir(PIN_BL, GPIO_OUT);

    gpio_put(PIN_CS, 1);
    gpio_put(PIN_RESET, 1);
    //lcd_init(pio, sm, ili9341_init_seq);
    lcd_init(pio, sm, ili9341_init_seq);
    gpio_put(PIN_BL, 1);

#if 0
    while (1) {
        //tft_fill_screen(ILI9341_GREEN);
        tft_fill_screen(ILI9341_RED);
        sleep_ms(1000);
    }
#endif
    //tft_set_addr_window(0, 0, ILI9341_TFTWIDTH - 1, ILI9341_TFTHEIGHT - 1);
    //tft_fill_screen(ILI9341_RED);

    tft_set_addr_window(0, 0, ILI9341_TFTWIDTH - 1, ILI9341_TFTHEIGHT - 1);
    tft_fill_screen(ILI9341_BLACK);

    tft_set_addr_window(0, 0, NES_DISP_WIDTH-1, NES_DISP_HEIGHT-1);

    //tft_set_addr_window(0, 0, ILI9341_TFTWIDTH - 1, ILI9341_TFTHEIGHT - 1);
    //tft_fill_screen(ILI9341_BLUE);
    //tft_fill_screen(ILI9341_BLACK);

    while (1) {
        if(InfoNES_Load(NULL) == 0) {
            InfoNES_Main();
        }
    }

    while (1);

}
