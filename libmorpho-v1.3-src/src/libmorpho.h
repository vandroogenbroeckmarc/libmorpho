/* LIBMORPHO
 *
 * libmorpho.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*!
 * \file libmorpho.h
 */ 


/*!
 * \ingroup libmorpho
 */

#ifndef __LIBMORPHO__
#define __LIBMORPHO__

/* Include other headers files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

/*! 
 * \typedef uint8_t 
 * \brief This type designates an unsigned 8 bits long integer. 
 * 
 * This type is the reference type for all image input and output buffers. 
 * You should not change it as the algorithm uses an histogram whose size 
 * matches the range of possible input/output values. 
 */ 
typedef unsigned char uint8_t;

/*! 
 * \typedef int16_t 
 * \brief This type designates a signed 16 bits long integer. 
 * 
 * This type is used by functions that treat structuring functions. 
 * Keep in ming that with structuring functions the range can be enlarged depending 
 * on the relation bewteen 
 * the original function and the structuring function.
 */ 
# ifndef __int8_t_defined
typedef short int int16_t;
#endif

/* Verbosity for the sources */
/*!
  \def  DEBUG
  \brief Verbosity level for the sources code of the morphologic functions
*/
#define DEBUG 0

/* Enumeration of return codes */
/*!
 * \def  MORPHO_ERROR
  * Error code returned by the functions of the library.
  * Erro code (return -1) 
*/
#define  MORPHO_ERROR  -1

/*!
 * \def  MORPHO_SUCCESS
 * Success code returned by the functions of the library
*/
#define  MORPHO_SUCCESS 0

/* util.c */
int imageTranspose(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight);
int is_size_valid_1D(int size, int imageWidth, char *func, int odd);

/* erosionByAnchor.c */
int erosionByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int erosionByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int erosionByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight);

/* src/dilationByAnchor.c */
int dilationByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int dilationByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int dilationByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight);

/* openingByAnchor.c */
int openingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int openingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int openingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight);

/* closingByAnchor.c */
int closingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int closingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size);
int closingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight);

/* erosionArbitrarySE.c */
int erosion_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin);

/* dilationArbitrarySE.c */
int dilation_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin);

/* openingArbitrarySE.c */
int opening_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se1, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin);

/* closingArbitrarySE.c */
int closing_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se1, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin);

/* erosionArbitrarySF.c */
int erosion_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin);

/* dilationArbitrarySF.c */
int dilation_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin);

/* openingArbitrarySF.c */
int opening_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf1, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin);

/* closingArbitrarySF.c */
int closing_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf1, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin);

#endif

