/*********************************************************************
   This is a port of the Adafruit SSD1306 driver to Nordic nRF52832.

   Currently only I2C supported.

   Original:

   https://github.com/adafruit/Adafruit_SSD1306

   electronut.in
*********************************************************************/

#include <stdlib.h>
#include <string.h>

#include "nrf_drv_twi.h"
#include "app_error.h"

#include "SSD1306.h"

// TWI handle
extern nrf_drv_twi_t m_twi_master;

static int WIDTH = SSD1306_LCDWIDTH;
static int HEIGHT = SSD1306_LCDHEIGHT;

static int8_t _i2caddr, _vccstate;
/*
   // You will need something like this in your main.c:

   const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(0);

   void twi_init (void)
   {
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_sensors_config = {
       .scl                = 11,
       .sda                = 13,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    err_code = nrf_drv_twi_init(&m_twi_master, &twi_sensors_config, NULL, NULL);        // twi in blocking mode.
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi_master);
   }
 */

// the memory buffer for the LCD
#if 0
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
        0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
#if (SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH > 96*16)
        0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
        0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
        0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
        0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
        0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
        0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
        0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
        0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
        0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
        0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
        0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
        0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
        0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SSD1306_LCDHEIGHT == 64)
        0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
        0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
        0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
        0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
        0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
        0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
        0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
        0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
        0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
        0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
        0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
        0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
        0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};
#else

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] =
{
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0x80,0x7f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0x0,0x7f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0x0,0x7f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xfe,0x0,0x3f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xfc,0x0,0x3f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xfc,0x0,0x3f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xf8,0x0,0x1f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xf0,0x0,0x1f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xe0,0x0,0x1f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xe0,0x0,0x1f,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xc0,0x0,0xf,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xc0,0x0,0xf,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0x80,0x0,0xf,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xff,0x0,0x0,0x3f,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xfe,0x0,0x0,0x7,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xfe,0x0,0x0,0x1,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xfe,0x0,0x0,0x0,0x80,0x0,0x7,0xff,0xff,0xff,0xc0,
        0xfe,0x0,0x0,0x0,0x0,0x0,0xf,0xff,0xff,0xff,0xc0,
        0xff,0x0,0x0,0x0,0x0,0x0,0xf,0xff,0xff,0xff,0xc0,
        0xff,0x80,0x0,0x0,0x0,0x60,0x18,0x3,0xff,0xff,0xc0,
        0xff,0xc0,0x0,0x0,0x0,0xe0,0x0,0x0,0x3f,0xff,0xc0,
        0xff,0xc0,0x0,0x0,0x0,0xe0,0x0,0x0,0x7,0xff,0xc0,
        0xff,0xe0,0x0,0x0,0x0,0xe0,0x0,0x0,0x0,0xff,0xc0,
        0xff,0xf0,0x0,0x0,0x0,0xe0,0x0,0x0,0x0,0x1f,0xc0,
        0xff,0xf0,0x0,0x0,0x1,0xe0,0x0,0x0,0x0,0xf,0xc0,
        0xff,0xf8,0x0,0xf,0x0,0xe0,0x0,0x0,0x0,0xf,0xc0,
        0xff,0xfc,0x0,0x7,0xc0,0xc0,0x0,0x0,0x0,0xf,0xc0,
        0xff,0xfe,0x0,0x7,0xf0,0x80,0x0,0x0,0x0,0xf,0xc0,
        0xff,0xfe,0x0,0x1,0xf0,0x1,0xf0,0x0,0x0,0x1f,0xc0,
        0xff,0xff,0x0,0x0,0xf8,0x7,0xf8,0x0,0x0,0x3f,0xc0,
        0xff,0xff,0x80,0x0,0x0,0xf,0xf0,0x0,0x0,0xff,0xc0,
        0xff,0xff,0xe0,0x0,0x0,0xf,0xc0,0x0,0x1,0xff,0xc0,
        0xff,0xff,0xf0,0x0,0x0,0x0,0x0,0x0,0x3,0xff,0xc0,
        0xff,0xff,0xfe,0x0,0x0,0x0,0x0,0x0,0xf,0xff,0xc0,
        0xff,0xff,0xff,0x0,0x1c,0x0,0x0,0x0,0x1f,0xff,0xc0,
        0xff,0xff,0xfc,0x0,0x3c,0x70,0x0,0x0,0x7f,0xff,0xc0,
        0xff,0xff,0xf8,0x0,0xf8,0x70,0x0,0x0,0xff,0xff,0xc0,
        0xff,0xff,0xf0,0x0,0xf0,0x78,0x0,0x3,0xff,0xff,0xc0,
        0xff,0xff,0xe0,0x1,0xe0,0x3c,0x2,0x1f,0xff,0xff,0xc0,
        0xff,0xff,0xe0,0x1,0xc0,0x3c,0x1,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xc0,0x1,0x80,0x1c,0x0,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xc0,0x0,0x0,0x1c,0x0,0xff,0xff,0xff,0xc0,
        0xff,0xff,0x80,0x0,0x0,0xc,0x0,0x7f,0xff,0xff,0xc0,
        0xff,0xff,0x80,0x0,0x0,0x0,0x0,0x7f,0xff,0xff,0xc0,
        0xff,0xff,0x80,0x0,0x0,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0x0,0x0,0x4,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfe,0x0,0x0,0x4,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfe,0x0,0x0,0xc,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfe,0x0,0x0,0x1e,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfc,0x0,0x0,0x3e,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfc,0x0,0x0,0x7f,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfc,0x0,0x1,0xff,0x0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xf8,0x0,0xf,0xff,0x80,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xf8,0x0,0x7f,0xff,0xc0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xf8,0x3,0xff,0xff,0xe0,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xfc,0x1f,0xff,0xff,0xf8,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xfc,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xfe,0x0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0x3f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0x7f,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0,
        0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xc0
};

#endif

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

static int width() {
        return SSD1306_LCDWIDTH;
}
static int height() {
        return SSD1306_LCDHEIGHT;
}
static int getRotation() {
        return 0;
}


// the most basic function, set a single pixel
void SSD1306_drawPixel(int16_t x, int16_t y, uint16_t color) 
{
        if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
                return;

        // check rotation, move pixel around if necessary
        switch (getRotation()) {
        case 1:
                ssd1306_swap(x, y);
                x = WIDTH - x - 1;
                break;
        case 2:
                x = WIDTH - x - 1;
                y = HEIGHT - y - 1;
                break;
        case 3:
                ssd1306_swap(x, y);
                y = HEIGHT - y - 1;
                break;
        }

        // x is which column
        int index = x+ (y/8)*SSD1306_LCDWIDTH;
        //printf("index = %d\n", index);

        switch (color)
        {
        case WHITE:   buffer[index] |=  (1 << (y&7)); break;
        case BLACK:   buffer[index] &= ~(1 << (y&7)); break;
        case INVERSE: buffer[index] ^=  (1 << (y&7)); break;
        }

}


void SSD1306_begin(uint8_t vccstate, uint8_t i2caddr, bool reset) {

        _vccstate = vccstate;
        _i2caddr = i2caddr;

        // Init sequence
        SSD1306_command(SSD1306_DISPLAYOFF);              // 0xAE
        SSD1306_command(SSD1306_SETDISPLAYCLOCKDIV);      // 0xD5
        SSD1306_command(0x80);                            // the suggested ratio 0x80

        SSD1306_command(SSD1306_SETMULTIPLEX);            // 0xA8
        SSD1306_command(SSD1306_LCDHEIGHT - 1);

        SSD1306_command(SSD1306_SETDISPLAYOFFSET);        // 0xD3
        SSD1306_command(0x0);                             // no offset
        SSD1306_command(SSD1306_SETSTARTLINE | 0x0);      // line #0
        SSD1306_command(SSD1306_CHARGEPUMP);              // 0x8D
        if (vccstate == SSD1306_EXTERNALVCC)
        { SSD1306_command(0x10); }
        else
        { SSD1306_command(0x14); }
        SSD1306_command(SSD1306_MEMORYMODE);              // 0x20
        SSD1306_command(0x00);                            // 0x0 act like ks0108
        SSD1306_command(SSD1306_SEGREMAP | 0x1);
        SSD1306_command(SSD1306_COMSCANDEC);

 #if defined SSD1306_128_32
        SSD1306_command(SSD1306_SETCOMPINS);              // 0xDA
        SSD1306_command(0x02);
        SSD1306_command(SSD1306_SETCONTRAST);             // 0x81
        SSD1306_command(0x8F);

#elif defined SSD1306_128_64
        SSD1306_command(SSD1306_SETCOMPINS);              // 0xDA
        SSD1306_command(0x12);
        SSD1306_command(SSD1306_SETCONTRAST);             // 0x81
        if (vccstate == SSD1306_EXTERNALVCC)
        { SSD1306_command(0x9F); }
        else
        { SSD1306_command(0xCF); }

#elif defined SSD1306_96_16
        SSD1306_command(SSD1306_SETCOMPINS);              // 0xDA
        SSD1306_command(0x2); //ada x12
        SSD1306_command(SSD1306_SETCONTRAST);             // 0x81
        if (vccstate == SSD1306_EXTERNALVCC)
        { SSD1306_command(0x10); }
        else
        { SSD1306_command(0xAF); }

#endif

        SSD1306_command(SSD1306_SETPRECHARGE);            // 0xd9
        if (vccstate == SSD1306_EXTERNALVCC)
        { SSD1306_command(0x22); }
        else
        { SSD1306_command(0xF1); }
        SSD1306_command(SSD1306_SETVCOMDETECT);           // 0xDB
        SSD1306_command(0x40);
        SSD1306_command(SSD1306_DISPLAYALLON_RESUME);     // 0xA4
        SSD1306_command(SSD1306_NORMALDISPLAY);           // 0xA6

        SSD1306_command(SSD1306_DEACTIVATE_SCROLL);

        SSD1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void SSD1306_invertDisplay(uint8_t i) {
        if (i) {
                SSD1306_command(SSD1306_INVERTDISPLAY);
        } else {
                SSD1306_command(SSD1306_NORMALDISPLAY);
        }
}

void SSD1306_command(uint8_t c)
{
        ret_code_t ret;
        uint8_t data[] = {0x00, c};
        ret = nrf_drv_twi_tx(&m_twi_master, _i2caddr, data, 2, false);
        APP_ERROR_CHECK(ret);
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SSD1306_startscrollright(uint8_t start, uint8_t stop){
        SSD1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
        SSD1306_command(0X00);
        SSD1306_command(start);
        SSD1306_command(0X00);
        SSD1306_command(stop);
        SSD1306_command(0X00);
        SSD1306_command(0XFF);
        SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SSD1306_startscrollleft(uint8_t start, uint8_t stop){
        SSD1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
        SSD1306_command(0X00);
        SSD1306_command(start);
        SSD1306_command(0X00);
        SSD1306_command(stop);
        SSD1306_command(0X00);
        SSD1306_command(0XFF);
        SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SSD1306_startscrolldiagright(uint8_t start, uint8_t stop)
{
        SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
        SSD1306_command(0X00);
        SSD1306_command(SSD1306_LCDHEIGHT);
        SSD1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
        SSD1306_command(0X00);
        SSD1306_command(start);
        SSD1306_command(0X00);
        SSD1306_command(stop);
        SSD1306_command(0X01);
        SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void SSD1306_startscrolldiagleft(uint8_t start, uint8_t stop)
{
        SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
        SSD1306_command(0X00);
        SSD1306_command(SSD1306_LCDHEIGHT);
        SSD1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
        SSD1306_command(0X00);
        SSD1306_command(start);
        SSD1306_command(0X00);
        SSD1306_command(stop);
        SSD1306_command(0X01);
        SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_stopscroll(void){
        SSD1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void SSD1306_dim(bool dim) {
        uint8_t contrast;

        if (dim) {
                contrast = 0; // Dimmed display
        } else {
                if (_vccstate == SSD1306_EXTERNALVCC) {
                        contrast = 0x9F;
                } else {
                        contrast = 0xCF;
                }
        }
        // the range of contrast to too small to be really useful
        // it is useful to dim the display
        SSD1306_command(SSD1306_SETCONTRAST);
        SSD1306_command(contrast);
}

void SSD1306_display(void)
{
        SSD1306_command(SSD1306_COLUMNADDR);
        SSD1306_command(0); // Column start address (0 = reset)
        SSD1306_command(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

        SSD1306_command(SSD1306_PAGEADDR);
        SSD1306_command(0); // Page start address (0 = reset)
  #if SSD1306_LCDHEIGHT == 64
        SSD1306_command(7); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 32
        SSD1306_command(3); // Page end address
  #endif
  #if SSD1306_LCDHEIGHT == 16
        SSD1306_command(1); // Page end address
  #endif


        // save I2C bitrate
#ifdef TWBR
        uint8_t twbrbackup = TWBR;
        TWBR = 12; // upgrade to 400KHz!
#endif


        // I2C
        for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
                uint8_t tmpBuf[17];
                // SSD1306_SETSTARTLINE
                tmpBuf[0] = 0x40;
                // data
                for (uint8_t j = 0; j < 16; j++) {
                        tmpBuf[j+1] = buffer[i];
                        i++;
                }
                i--;

                ret_code_t ret;
                ret = nrf_drv_twi_tx(&m_twi_master, _i2caddr, tmpBuf, sizeof(tmpBuf), false);
                APP_ERROR_CHECK(ret);
        }

#ifdef TWBR
        TWBR = twbrbackup;
#endif

}

// clear everything
void SSD1306_clearDisplay(void)
{
        memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}


void SSD1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        bool bSwap = false;
        switch(getRotation())
        {
        case 0:
                // 0 degree rotation, do nothing
                break;
        case 1:
                // 90 degree rotation, swap x & y for rotation, then invert x
                bSwap = true;
                ssd1306_swap(x, y);
                x = WIDTH - x - 1;
                break;
        case 2:
                // 180 degree rotation, invert x and y - then shift y around for height.
                x = WIDTH - x - 1;
                y = HEIGHT - y - 1;
                x -= (w-1);
                break;
        case 3:
                // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
                bSwap = true;
                ssd1306_swap(x, y);
                y = HEIGHT - y - 1;
                y -= (w-1);
                break;
        }

        if(bSwap) {
                SSD1306_drawFastVLineInternal(x, y, w, color);
        } else {
                SSD1306_drawFastHLineInternal(x, y, w, color);
        }
}

void SSD1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
        // Do bounds/limit checks
        if(y < 0 || y >= HEIGHT) { return; }

        // make sure we don't try to draw below 0
        if(x < 0) {
                w += x;
                x = 0;
        }

        // make sure we don't go off the edge of the display
        if( (x + w) > WIDTH) {
                w = (WIDTH - x);
        }

        // if our width is now negative, punt
        if(w <= 0) { return; }

        // set up the pointer for  movement through the buffer
        register uint8_t *pBuf = buffer;
        // adjust the buffer pointer for the current row
        pBuf += ((y/8) * SSD1306_LCDWIDTH);
        // and offset x columns in
        pBuf += x;

        register uint8_t mask = 1 << (y&7);

        switch (color)
        {
        case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
        case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
        case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
        }
}

void SSD1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        bool bSwap = false;
        switch(getRotation()) {
        case 0:
                break;
        case 1:
                // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
                bSwap = true;
                ssd1306_swap(x, y);
                x = WIDTH - x - 1;
                x -= (h-1);
                break;
        case 2:
                // 180 degree rotation, invert x and y - then shift y around for height.
                x = WIDTH - x - 1;
                y = HEIGHT - y - 1;
                y -= (h-1);
                break;
        case 3:
                // 270 degree rotation, swap x & y for rotation, then invert y
                bSwap = true;
                ssd1306_swap(x, y);
                y = HEIGHT - y - 1;
                break;
        }

        if(bSwap) {
                SSD1306_drawFastHLineInternal(x, y, h, color);
        } else {
                SSD1306_drawFastVLineInternal(x, y, h, color);
        }
}


void SSD1306_drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

        // do nothing if we're off the left or right side of the screen
        if(x < 0 || x >= WIDTH) { return; }

        // make sure we don't try to draw below 0
        if(__y < 0) {
                // __y is negative, this will subtract enough from __h to account for __y being 0
                __h += __y;
                __y = 0;

        }

        // make sure we don't go past the height of the display
        if( (__y + __h) > HEIGHT) {
                __h = (HEIGHT - __y);
        }

        // if our height is now negative, punt
        if(__h <= 0) {
                return;
        }

        // this display doesn't need ints for coordinates, use local byte registers for faster juggling
        register uint8_t y = __y;
        register uint8_t h = __h;


        // set up the pointer for fast movement through the buffer
        register uint8_t *pBuf = buffer;
        // adjust the buffer pointer for the current row
        pBuf += ((y/8) * SSD1306_LCDWIDTH);
        // and offset x columns in
        pBuf += x;

        // do the first partial byte, if necessary - this requires some masking
        register uint8_t mod = (y&7);
        if(mod) {
                // mask off the high n bits we want to set
                mod = 8-mod;

                // note - lookup table results in a nearly 10% performance improvement in fill* functions
                // register uint8_t mask = ~(0xFF >> (mod));
                static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
                register uint8_t mask = premask[mod];

                // adjust the mask if we're not going to reach the end of this byte
                if( h < mod) {
                        mask &= (0XFF >> (mod-h));
                }

                switch (color)
                {
                case WHITE:   *pBuf |=  mask;  break;
                case BLACK:   *pBuf &= ~mask;  break;
                case INVERSE: *pBuf ^=  mask;  break;
                }

                // fast exit if we're done here!
                if(h<mod) { return; }

                h -= mod;

                pBuf += SSD1306_LCDWIDTH;
        }


        // write solid bytes while we can - effectively doing 8 rows at a time
        if(h >= 8) {
                if (color == INVERSE)  {// separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
                        do  {
                                *pBuf=~(*pBuf);

                                // adjust the buffer forward 8 rows worth of data
                                pBuf += SSD1306_LCDWIDTH;

                                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                                h -= 8;
                        } while(h >= 8);
                }
                else {
                        // store a local value to work with
                        register uint8_t val = (color == WHITE) ? 255 : 0;

                        do  {
                                // write our value in
                                *pBuf = val;

                                // adjust the buffer forward 8 rows worth of data
                                pBuf += SSD1306_LCDWIDTH;

                                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                                h -= 8;
                        } while(h >= 8);
                }
        }

        // now do the final partial byte, if necessary
        if(h) {
                mod = h & 7;
                // this time we want to mask the low bits of the byte, vs the high bits we did above
                // register uint8_t mask = (1 << mod) - 1;
                // note - lookup table results in a nearly 10% performance improvement in fill* functions
                static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
                register uint8_t mask = postmask[mod];
                switch (color)
                {
                case WHITE:   *pBuf |=  mask;  break;
                case BLACK:   *pBuf &= ~mask;  break;
                case INVERSE: *pBuf ^=  mask;  break;
                }
        }
}
