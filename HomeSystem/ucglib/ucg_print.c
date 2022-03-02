/*!
 *  \file    ucg_print.c
 *  \author  Wim Dolman (<a href="mailto:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    30-09-2020
 *  \version 4.0
 *
 *  \brief   Print facilities for ucglib from Oli Kraus
 *
 *  \details This is an addition to the  
 *           <a href="https://github.com/olikraus/ucglib/">c-implementation</a> of Oli Kraus.
 *
 *           The Arduino/C++ implementation of ucglib contains extra printing facilities.
 *           This file contains a bunch of functions that implements the same facilities.
 *           So you can use <code>ucg_SetPrintPos()</code>, <code>ucg_SetPrintDir()</code> 
 *           and in stead of <code>print</code> and <code>println</code> 
 *           you can use <code>ucg_Print()</code> which prints a formatstring.
 *
 *           This print facility uses the user pointer in the struct ucg_t. To use this
 *           pointer you need to remove the comment (at line 100 in ucg.h), before the 
 *           macrodefinition of  WITH_USER_PTR
 *  \verbatim
             / * Define this for an additional user pointer inside the ucg data struct * /
             #define WITH_USER_PTR\endverbatim
 *             
 *           Originally these functionality was added in the Xmega Hardware Abstraction 
 *           Layer (version 2.0, 2.1 and 3.0). From vesion 4.0 this functionality is 
 *           separated from the HAL.
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ucg.h"

//!< Struct for compatibility printing facilities with Arduino/C++ version of library 
typedef struct {      
  FILE      *fp;      //!< file pointer for printing 
  ucg_int_t  tx;      //!< current x coordinate of the position
  ucg_int_t  ty;      //!< current y coordinate of the position
  uint8_t    tdir;    //!< current printing direction
} ucg_print_t;           

static int   _ucg_putc(char c, FILE *stream);

static ucg_t  *_curr_ucg;   //!< pointer necessary for the printing facilities

/*! \brief  Initializes the printing facilities compatible with Arduino/C++ version of library
 *
 *  \param  ucg      pointer to struct for the display
 *
 *  \return void
 */
void ucg_PrintInit(ucg_t *ucg)
{
  if  (ucg_GetUserPtr(ucg) != NULL) return;
  
  ucg_print_t *p = (ucg_print_t *) malloc(sizeof(ucg_print_t));
  
  if (p == NULL) return;
  
  p->fp    = fdevopen(_ucg_putc, NULL); 
  p->tx    = 0;
  p->ty    = 0;
  p->tdir  = 0;
  
  ucg_SetUserPtr(ucg, p);
}
/*! \brief  Gets the current position of the 'print cursor'
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  x        pointer to a variable for the x-coordinate of the current position
 *  \param  y        pointer to a variable for the y-coordinate of the current position
 *
 *  \return void
 */
void ucg_GetPrintPos(ucg_t *ucg, ucg_int_t *x, ucg_int_t *y)
{
  *x = ((ucg_print_t *)ucg_GetUserPtr(ucg))->tx;
  *y = ((ucg_print_t *)ucg_GetUserPtr(ucg))->ty;
}


/*! \brief  Sets the position for next "print" command.
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  x        x-coordinate of the position
 *  \param  y        y-coordinate of the position
 *
 *  \return void
 */
void ucg_SetPrintPos(ucg_t *ucg, ucg_int_t x, ucg_int_t y)
{
  ((ucg_print_t *)ucg->user_ptr)->tx = x;
  ((ucg_print_t *)ucg->user_ptr)->ty = y;
}


/*! \brief  Sets the direction for next "print" command.
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  dir      the direction
 *
 *  \return void
 */
void ucg_SetPrintDir(ucg_t *ucg, uint8_t dir)
{
  ((ucg_print_t *)ucg_GetUserPtr(ucg))->tdir = dir;
}


/*! \brief  Put a formatted string to the display at the
 *          current position and in the current direction.
 *
 *          This replaces print and println from the Arduino implementation of ucg_lib
 *
 *          The Arduino style:
 * \verbatim
 *          ucg.print("text ");
 *          ucg.print(x);     // x is an int
 *          ucg.print(" more text ");
 *          ucg.print(y);     // y is a float
 *          ucg.println(";"); \endverbatim
 *              
 *          The replacement in Xmega style:
 *\verbatim
 *          ucg_Print(&ucg, "text %d more text %f;\n", x, y); \endverbatim
 *
 *  \param  ucg      pointer to struct for the display
 *  \param  fmt      formatstring with escape sequences
 *  \param  ...      variables that are printed
 *
 *  \return number of printed characters
 */
int ucg_Print(ucg_t *ucg, char *fmt, ...)
{
  va_list  vl;

  _curr_ucg = ucg;
  va_start(vl, fmt);
  int n = vfprintf( ((ucg_print_t *) (ucg_GetUserPtr(ucg)))->fp, fmt, vl);
  va_end(vl);

  return n;
}



// local print facilities


/*  brief  Put a character to the display at the
 *         current position and in the current direction.
 *
 *  param  ucg      pointer to struct for the display
 *  param  dir      the direction
 *
 *  return 0  (always OK, one character printed)
 */
static int _ucg_putc(char c, FILE *stream)
{
  ucg_int_t delta;
  ucg_print_t *p = ((ucg_print_t *) ucg_GetUserPtr(_curr_ucg));

  delta = ucg_DrawGlyph(_curr_ucg, p->tx, p->ty, p->tdir, c);

  switch(p->tdir) {
    case          0: p->tx += delta; break;
    case          1: p->ty += delta; break;
    case          2: p->tx -= delta; break;
    default: case 3: p->ty -= delta; break;
  }

  return 0;
}