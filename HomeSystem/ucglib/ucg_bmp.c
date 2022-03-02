/*!
 *  \file    ucg_bmp.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    30-09-2020
 *  \version 4.0
 *
 *  \brief   Bitmap facilities for ucglib from Oli Kraus
 *
 *  \details This is an addition to the  
 *           <a href="https://github.com/olikraus/ucglib/">c-implementation</a> of Oli Kraus.
 *
 *           The functions ucg_DrawBmpLine(), ucg_DrawBmp(), ucg_DrawBmpRotate() print 
 *           a bitmap to the display. The bitmap is a const __memx uint8_t array in plain C. 
 *           The maximum bitmap size is 32768 kB. To show large images the image must be 
 *           split in smaller bitmaps. With __memx the 64K of the program memory can be
 *           exceeded.
 *
 *           Normally bitmap images are large. The Atmel couldn't address memory beyond
 *           64K without special actions. There are two ways to read beyond 64K: using
 *           PROGMEM with far pointers or using __memx.
 *           The first option the user must decide which part of thecis used. As long as
 *           the application you within tthe 64K 16-bits pointers otherwise you need
 *           far-pointers
 *           The charme of the latest is that the compiler does the job. The drawback
 *           is that it uses 24-pointers which is a little bit slower.
 *           Kraus uses flash for commandstrings and fonts. The libraryfunctions uses
 *           16-bits pointer (e.g. pgm_read_byte()). If you mix with large __memx array's
 *           the compiler mightnplace commandstrings and fonts beyond 64K. This reults 
 *           in unaspected behavior.
 *           The solution is to place commandstrings in RAM and use __memx with the font  
 *           functions, the commandsrings are relative small and the fonts can be large.
 *           A __memx based use_fonts.c  must be used. 
 *
 *           The function ucg_DrawBmpLine() and ucg_DrawBmp() use commandstrings that are
 *           dependent of the display type. In this case the four commandstrings for ST7735 
 *           are used, which are declared in ucg_dev_ic_st7735.c.
 *           It is better to use and problaby change display callback functions.
 *           So you can use this solution with every display.
 * 
 *           Originally these functionality was added in the Xmega Hardware Abstraction 
 *           Layer (version 2.0, 2.1 and 3.0). From vesion 4.0 this functionality is 
 *           separated from the HAL.
*/
 
//#include <avr/io.h>
//#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ucg.h"
//#include "ucglib_xmega.h"



// bitmap facilities

/*! \brief  Prints a bitmap to the display (obsolete)
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  xoffset  the x-position of the upper left corner of the bitmap
 *  \param  yoffset  the y-position of the upper left corner of the bitmap
 *  \param  width    the width of the bitmap in pixels
 *  \param  height   the height of the bitmap in pixels
 *  \param  nbytes   the number of bytes of one pixel; must always be 3 (RGB)
 *  \param  bitmap   the pointer to a const unit8_t array with the bitmap.
 *
 *                   The bitmap must be smaller than 32K (largest AVR variable)
 *                   Large bitmaps can be split in smaller parts
 *                   The bitmaps can be located in the program memory or in the
 *                   data memory:
 *                   -# 'const uint8_t array[]' will be placed the data memory.
 *                          for atXmega256a3u devices the largest array ia 16K.
 *                   -# 'const uint8_t __flash array[]' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K
 *                   -# 'const uint8_t array[] PROGMEM' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K 
 *                   -# 'const uint8_t __memx array[]' will be placed only in the program memory.
 *                          with __memx the complete program space can be used 
 *                          for atXmega256a3u devices the maximum space is 256K
 *
 *                  The drawback of using __memx is that it is slower than __flash. This is
 *                  because _memx uses a 24-bits pointer and __flash uses a 16-bit pointer.
 *
 *                  This function is renamed.
 *                  The original function calls the much faster function ucg_DrawBmp
 *  \return void
 */
void  ucg_BitmapPrint(ucg_t *ucg, ucg_int_t xoffset,  ucg_int_t yoffset,
                                  ucg_int_t width, ucg_int_t height,
                                  uint8_t nbytes, const __memx uint8_t *bitmap)
{
  ucg_int_t  x = 0;
  ucg_int_t  y = 0;
  ucg_color_t oldColor;

  const __memx uint8_t *endBitmap = bitmap + width*height*nbytes;

  // remember original color
  memcpy(oldColor.color, ucg->arg.rgb[0].color, 3);

  while ( bitmap < endBitmap ) {
    ucg_SetColor(ucg, 0, *bitmap, *(bitmap+1), *(bitmap+2));
    bitmap += nbytes;
    ucg_DrawPixel(ucg, x+xoffset, y+yoffset);
    x++;
    if ( x >= width ) {
      x = 0;
      y++;
      if ( y >= height) break;
    }
  }

  // reset original color
  memcpy(ucg->arg.rgb[0].color, oldColor.color, 3);
}

/*!
 *    \defgroup group-1 extern consts
 *    \remark about the improved bitmap rendering
 *
 *    The function ucg_BitmapPrint prints the bitmap pixel by pixel
 *    The ucg_DrawBmpLine, ucg_DrawBmp, ucg_DrawBmpRotate and the new function
 *    ucg_BitmapPrint prints the bitmap line by line.
 *    The idea of the improved bitmap rendering is borrowed from:
 *       https://p3dt.net/post/2018/12/27/esp8266-ili9846-improved.html
 *    The new functions should actually be added to devices files
 *    ucg_dev_ic_XXXX.c.
 *    In this case the functions for ST7735 are added to ucg_bmp.c. These
 *    functions need four constants, which are declared in ucg_dev_ic_st7735.c.
 *   
 */
 /*!
 *   \addtogroup group-1
 * @{
 */
extern const ucg_pgm_uint8_t ucg_st7735_set_pos_dir0_seq[]; //!< extern const from ucg_dev_ic_st7735.c for printing left->right 
extern const ucg_pgm_uint8_t ucg_st7735_set_pos_dir1_seq[]; //!< extern const from ucg_dev_ic_st7735.c for printing top->bottom
extern const ucg_pgm_uint8_t ucg_st7735_set_pos_dir2_seq[]; //!< extern const from ucg_dev_ic_st7735.c for printing right->left
extern const ucg_pgm_uint8_t ucg_st7735_set_pos_dir3_seq[]; //!< extern const from ucg_dev_ic_st7735.c for printing bottom->top
/*! @} */

//  Development remark about new bitmap functions 
//  
//  It seems yhat with __memx (24-bit pointers) that pointer arithmetic 
//  is only possible with constants.  If bitmap is a __memx pointer the
//  assignment bitmap++; is ok, but the aasignment bitmap += bmpLineLength; 
//  is not OK. So the bitmap are always scanned byte by byte even if 
//  a part of the bitmap is outside the display.

/*! \brief  Draws a bitmap line to the display 
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  xoffset  the x-position of the upper left corner of the bitmap line
 *  \param  yoffset  the y-position of the upper left corner of the bitmap line
 *  \param  dir      the direction of the bitmap
 *                   0 (normal), 1 (+90*), 2 (180*) or 3 (270*)
 *  \param  nbytes   the number of bytes to be sent
 *  \param  bitLine  the pointer to a const unit8_t array with the bitmap line.
 *
 *  \return void
 */
void ucg_DrawBmpLine(ucg_t *ucg, ucg_int_t xoffset, ucg_int_t yoffset, ucg_int_t dir, 
                                 ucg_int_t nbytes, uint8_t *bitLine)
{
  ucg->arg.pixel.pos.x = xoffset;
  ucg->arg.pixel.pos.y = yoffset;
  ucg->arg.dir = dir;

  switch(ucg->arg.dir) {
    case 0: ucg_com_SendCmdSeq(ucg, ucg_st7735_set_pos_dir0_seq); break;
    case 1: ucg_com_SendCmdSeq(ucg, ucg_st7735_set_pos_dir1_seq); break;
    case 2: ucg_com_SendCmdSeq(ucg, ucg_st7735_set_pos_dir2_seq); break;
    case 3: ucg_com_SendCmdSeq(ucg, ucg_st7735_set_pos_dir3_seq); break;
  }
  ucg_com_SendString(ucg, nbytes, bitLine);
  ucg_com_SetCSLineStatus(ucg, 1);		/* disable chip */
}

/*! \brief  Draws a bitmap to the display 
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  xoffset  the x-position of the upper left corner of the bitmap
 *  \param  yoffset  the y-position of the upper left corner of the bitmap
 *  \param  delay    a delay after each line drawn to get a wipe style
 *  \param  width    the width of the bitmap in pixels
 *  \param  height   the height of the bitmap in pixels
 *  \param  nbytes   the number of bytes of one pixel; must always be 3 (RGB)
 *  \param  bitmap   the pointer to a const unit8_t array with the bitmap.
 *
 *                   The bitmap must be smaller than 32K (largest AVR variable)
 *                   Large bitmaps can be split in smaller parts
 *                   The bitmaps can be located in the program memory or in the
 *                   data memory:
 *                   -# 'const uint8_t array[]' will be placed the data memory.
 *                          for atXmega256a3u devices the largest array ia 16K.
 *                   -# 'const uint8_t __flash array[]' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K
 *                   -# 'const uint8_t array[] PROGMEM' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K 
 *                   -# 'const uint8_t __memx array[]' will be placed only in the program memory.
 *                          with __memx the complete program space can be used 
 *                          for atXmega256a3u devices the maximum space is 256K
 *
 *                  The drawback of using __memx is that it is a little bit slower (< 2%) than 
 *                  __flash. This is because _memx uses a 24-bits pointer and __flash 
 *                  uses a 16-bit pointer.
 *
 *                  This function replaces the earlier function ucg_BitmapPrint
 *  \return void
 */
void  ucg_DrawBmp(ucg_t *ucg, ucg_int_t xoffset, ucg_int_t yoffset,
                  ucg_int_t delay,
                  ucg_int_t width, ucg_int_t height,
                  uint8_t nbytes, const __memx uint8_t *bitmap)
{
  ucg_int_t  bmpLine = yoffset;
  ucg_int_t  bLen = 0;
  ucg_int_t  bmpLastLine = 0;
  ucg_int_t  bmpLineLength = nbytes * width;
  uint8_t    bmp[bmpLineLength];
 
  // clip the printed part of bitmap to the display (first point must be in window)
  bLen         = ((ucg_GetWidth(ucg) - xoffset) > width ) ?  bmpLineLength : nbytes*(ucg_GetWidth(ucg) - xoffset);
  bmpLastLine  = ((ucg_GetHeight(ucg) - yoffset) > height ) ? height : (ucg_GetHeight(ucg) - yoffset);
  bmpLastLine += yoffset;
 
  ucg->arg.pixel.pos.x = xoffset;
  ucg->arg.len = bLen;
  ucg->arg.dir = 0;

  const __memx uint8_t *endBitmap = bitmap + bmpLineLength*height;
  int i = 0;
  while ( bitmap < endBitmap ) {
    if (i < bLen ) {
      bmp[i] = *bitmap;
    }
    i++;
    bitmap++;   
    if (i == bLen) {
      ucg->arg.pixel.pos.y = bmpLine;
      ucg_com_SendCmdSeq(ucg, ucg_st7735_set_pos_dir0_seq);
      ucg_com_SendString(ucg, bLen, bmp);
      ucg_com_SetCSLineStatus(ucg, 1);
      bmpLine++;
      if ( bmpLine > bmpLastLine) break;
      ucg_com_DelayMicroseconds(ucg, delay);
    } 
    if ( i == bmpLineLength ) { 
      i = 0;    
    }
  }
}

/*! \brief  Draws a (rotated) bitmap to the display 
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  xoffset  the x-position of the upper left corner of the bitmap
 *  \param  yoffset  the y-position of the upper left corner of the bitmap
 *  \param  dir      the direction of the bitmap
 *                   0 (normal), 1 (+90*), 2 (180*) or 3 (270*)
 *  \param  delay    a delay (in us) after each line drawn to get a wipe style                   
 *  \param  width    the width of the bitmap in pixels
 *  \param  height   the height of the bitmap in pixels
 *  \param  nbytes   the number of bytes of one pixel; must always be 3 (RGB)
 *  \param  bitmap   the pointer to a const unit8_t array with the bitmap.
 *
 *                   The bitmap must be smaller than 32K (largest AVR variable)
 *                   Large bitmaps can be split in smaller parts
 *                   The bitmaps can be located in the program memory or in the
 *                   data memory:
 *                   -# 'const uint8_t array[]' will be placed the data memory.
 *                          for atXmega256a3u devices the largest array ia 16K.
 *                   -# 'const uint8_t __flash array[]' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K
 *                   -# 'const uint8_t array[] PROGMEM' will be placed only in the program memory.
 *                          for atXmega256a3u devices the maximum space is 64K 
 *                   -# 'const uint8_t __memx array[]' will be placed only in the program memory.
 *                          with __memx the complete program space can be used 
 *                          for atXmega256a3u devices the maximum space is 256K
 *
 *                  The drawback of using __memx is that it is a little bit slower than 
 *                  __flash. This is because _memx uses a 24-bits pointer and __flash 
 *                  uses a 16-bit pointer.
 *
 *                  This function is a little slower (< 10%) than ucg_DrawBmp
 *  \return void
 */
void  ucg_DrawBmpRotate(ucg_t *ucg, ucg_int_t xoffset,  ucg_int_t yoffset, ucg_int_t  dir,
                        ucg_int_t delay,
                        ucg_int_t width, ucg_int_t height,
                        uint8_t nbytes, const __memx uint8_t *bitmap)
{
  ucg_int_t   bmpLine = 0;
  ucg_int_t   bLen = 0;
  ucg_int_t   bmpLastLine = 0;
  ucg_int_t   bmpLineLength = nbytes * width;
  uint8_t     bmp[bmpLineLength];
 
  // clip the printed part of bitmap to the display
  switch(dir) {
    case 0:
    bLen = ((ucg_GetWidth(ucg) - xoffset) > width ) ?  bmpLineLength : nbytes*(ucg_GetWidth(ucg) - xoffset) ;
    bmpLastLine = ((ucg_GetHeight(ucg) - yoffset) > height ) ? height : (ucg_GetHeight(ucg) - yoffset);
    break;
    case 1:
    bLen = (yoffset > width ) ?  bmpLineLength : nbytes*yoffset ;
    bmpLastLine = ((ucg_GetWidth(ucg) - xoffset) > height) ? height : (ucg_GetWidth(ucg) - xoffset) ;
    break;
    case 2:
    bLen = (xoffset > width ) ? bmpLineLength : nbytes*xoffset ;
    bmpLastLine = (yoffset > height ) ? height : yoffset ;
    break;
    case 3:
    default:
    bLen = ((ucg_GetHeight(ucg) - yoffset) > width ) ?  bmpLineLength : nbytes*(ucg_GetHeight(ucg) - yoffset) ;
    bmpLastLine = (xoffset > height) ? height : xoffset;
    break;
  }
  
  // print the bitmap
  const __memx uint8_t *endBitmap = bitmap + width*height*nbytes;
  int i = 0;
  while ( bitmap < endBitmap ) {
    if (i < bLen ) {
      bmp[i] = *bitmap;
    }
    i++;
    bitmap++;   
    if (i == bLen) {
      switch(dir) {
      case 0:
        ucg_DrawBmpLine(ucg, xoffset, yoffset+bmpLine, 0, bLen, bmp);
        break;
      case 1:
        ucg_DrawBmpLine(ucg, xoffset+bmpLine, ucg_GetHeight(ucg)-yoffset, 3, bLen, bmp);
        break;
      case 2:
        ucg_DrawBmpLine(ucg, ucg_GetWidth(ucg)-xoffset, yoffset-bmpLine-1, 2, bLen, bmp);
        break;
      case 3:
        ucg_DrawBmpLine(ucg, xoffset-bmpLine, yoffset, 1, bLen, bmp);
        break;
      }
      bmpLine++;
      if ( bmpLine >= bmpLastLine) break;     
    }  
    if (i == bmpLineLength) {
      i = 0;    
    }
    ucg_com_DelayMicroseconds(ucg, delay);
  }
}


