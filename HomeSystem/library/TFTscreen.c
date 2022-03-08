#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#include "TFTscreen.h"
#include "Defines.h"

#define BSCALE_VAL  -7
#define BSEL_VAL   11

#define USART_TIMEOUT 500


void set_usartctrl (USART_t *usart, uint8_t bscale, uint16_t bsel);     //send band rate settings

void init_uart_bscale_bsel(USART_t *usart, int8_t bscale, int16_t bsel)
{
    switch (screen_port)
    {
        case 'D':
        PORTD.DIRSET = screen_TX_pin;             //set directory for TX pin
        PORTD.DIRSET = screen_RX_pin;             //set directory for RX pin
        usart->CTRLA = 0;                               //no interupts
        usart->CTRLB = USART_TXEN_bm | USART_RXEN_bm;   //enable both transmitter and receiver
        usart->CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
        set_usartctrl(usart, bscale, bsel);
        break;

        case 'E':
        PORTE.DIRSET = screen_TX_pin;             //set directory for TX pin
        PORTE.DIRSET = screen_RX_pin;             //set directory for RX pin
        usart->CTRLA = 0;                               //no interupts
        usart->CTRLB = USART_TXEN_bm | USART_RXEN_bm;   //enable both transmitter and receiver
        usart->CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;
        set_usartctrl(usart, bscale, bsel);
        break;
    }
}

void screen_init()      //initialze screen
{
    switch (screen_port)
    {
        case 'D':
        init_uart_bscale_bsel(&USARTD0, BSCALE_VAL, BSEL_VAL);
        break;

        case 'E':
        init_uart_bscale_bsel(&USARTE0, BSCALE_VAL, BSEL_VAL);
        break;
    }
}

uint16_t getcoordsx() //get x coord from usart
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 20;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        uint16_t xpos = USARTD0.DATA;

        return xpos;
        break;

        case 'E':
        uint16_t data = 20;
        
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        uint16_t xpos = USARTE0.DATA;

        return xpos;
        break;
    }
}

uint16_t getcoordsy() //get y coord from usart
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 21;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        uint16_t xpos = USARTD0.DATA;

        return xpos;
        break;

        case 'E':
        uint16_t data = 21;
        
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        uint16_t xpos = USARTE0.DATA;

        return xpos;
        break;
    }
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color)     //1 set single pixel
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 1;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 3)
        {
            break;
        }
        
        data = x;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 1;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 3)
        {
            break;
        }

        data = x;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = y;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)      //2 draw line, can be diagonal. Slower because of calculations
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 2;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 5)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = x1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 2;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 5)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = x1;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawFastVLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)          //3 draw line, only vertical. Faster because of no calculations
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 3;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 4)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = length;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 3;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 4)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = length;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawFastHLine(uint8_t x0, uint8_t y0, uint8_t length, uint16_t color)             //4 draw line, only horizantal. Faster because of no calculations
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 4;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 4)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = length;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 4;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 4)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = length;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color)        //5 draw rectangle, only outside lines
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 5;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 5)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 5;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 5)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color)        //6 draw rectangle, filled in
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 6;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 5)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 6;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 5)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color)      //7 draw rectangle with rounded edges, only outside lines
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 7;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 6)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = radius;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 7;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 6)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = radius;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color)      //8 draw rectangle with rounded edges, filled in
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 8;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 6)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = radius;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 8;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 6)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = h;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = radius;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)      //9 draw circle, only outside lines
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 9;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 4)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = r;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 9;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 4)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = r;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)      //10 draw circle, filled in
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 10;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 4)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = r;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 10;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 4)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = r;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)    //11 draw triangle, outside lines only
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 11;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 7)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 11;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 7)
        {
            break;
        }

        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;
    }
}

void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)    //12 draw triangle, filled in
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 12;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 7)
        {
            break;
        }
        
        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 12;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 7)
        {
            break;
        }

        data = x0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y1;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = x2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = y2;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;
    }
}

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)       //13 draw a Character
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 13;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 6)
        {
            break;
        }
        
        data = x;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);
        
        data = y;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = c;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = bg;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        _delay_ms(10);

        data = size;
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 7;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 6)
        {
            break;
        }

        data = x;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = c;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = bg;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = size;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void setCursor(int16_t x0, int16_t y0)     //14 set loction to draw character(coordinates are top left of character)
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 14;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 2)
        {
            break;
        }
        
        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);
        
        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        break;

        case 'E':
        uint16_t data = 14;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 2)
        {
            break;
        }

        data = x0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        _delay_ms(10);

        data = y0;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void setTextColor(uint16_t color)          //15 set text color
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 15;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 1)
        {
            break;
        }
        
        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 15;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 1)
        {
            break;
        }

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void setTextSize(uint8_t size)             //16 set text size
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 16;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 1)
        {
            break;
        }
        
        data = size;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 16;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 1)
        {
            break;
        }

        data = size;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void setTextWrap(bool w)                   //17 enable or disable screen wrapping for text
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 17;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 1)
        {
            break;
        }
        
        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 17;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 1)
        {
            break;
        }

        data = w;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void fillScreen(uint16_t color)            //18 fill screen with a single color
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 18;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 1)
        {
            break;
        }
        
        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 18;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 1)
        {
            break;
        }

        data = color;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}

void setRotation(uint8_t rotation)         //19 rotate screen, only 0 90 80 270   
{
    switch (screen_port)
    {
        case 'D':
        uint16_t data = 19;
        
        while (!(USARTD0.STATUS & USART_DREIF_bm)){}
        USARTD0.DATA = data;
        
        uint16_t timing = 0;
        while (!(USARTD0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTD0.DATA;

        if (data != 1)
        {
            break;
        }
        
        data = rotation;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;

        case 'E':
        uint16_t data = 19;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;

        uint16_t timing = 0;
        while (!(USARTE0.STATUS & USART_RXCIF_bm) && timing <= USART_TIMEOUT)
        {
            _delay_ms(1);
            timing++;
        }

        if(timing >= USART_TIMEOUT-1)
        {
            break;
        } 

        data = USARTE0.DATA;

        if (data != 1)
        {
            break;
        }

        data = rotation;
        while (!(USARTE0.STATUS & USART_DREIF_bm)){}
        USARTE0.DATA = data;
        
        break;
    }
}
