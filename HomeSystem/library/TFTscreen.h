#include <stdint.h>
#include <stdbool.h>

uint16_t getcoordsx();    //create two inter x posistion
uint16_t getcoordsy();    //create two inter y posistion

void screen_init();     //setup screen info

void drawPixel(uint16_t x, uint16_t y, uint16_t color);     //set single pixel

void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);      //draw line, can be diagonal. Slower because of calculations
void drawFastVLine(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);          //draw line, only vertical. Faster because of no calculations
void drawFastHLine(uint8_t x0, uint8_t y0, uint8_t length, uint16_t color);             //draw line, only horizantal. Faster because of no calculations

void drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);        //draw rectangle, only outside lines
void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);        //draw rectangle, filled in

void drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);      //draw rectangle with rounded edges, only outside lines
void fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);      //draw rectangle with rounded edges, filled in

void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);      //draw circle, only outside lines
void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);      //draw circle, filled in

void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);    //draw triangle, outside lines only
void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);    //draw triangle, filled in

void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);       //draw a Character
void setCursor(int16_t x0, int16_t y0);     //set loction to draw character(coordinates are top left of character)
void setTextColor(uint16_t color);          //set text color
void setTextSize(uint8_t size);             //set text size
void setTextWrap(bool w);                   //enable or disable screen wrapping for text

void fillScreen(uint16_t color);            //fill screen with a single color
void setRotation(uint8_t rotation);         //rotate screen, only 0 90 80 270 
