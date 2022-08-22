/* LIBMORPHO
 *
 * util.c
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
 * \file util.c
 */ 

#include "libmorpho.h"

/*! 
 * \fn int imageTranspose(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image input buffer
 * \param[in]  imageHeight Height of the image input buffer
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * \brief Transposes an image
 * \ingroup libmorpho
 *
 * This function is used for internal purposes only. It tranposes an image 
 * such that \code imageOut[i][j]=imageIn[j][i]\endcode 
 * It is recommended that you 
 * do not use this functions as is. Remember that, after 
 * transposition, the width of the output
 * buffer is given by the height of the input buffer, and vice versa. 
 *
 */ 
int imageTranspose(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight)
{
  int x,y;
  uint8_t	*in, *out;

  in = imageIn;

  for (y=0; y<imageHeight; y++)
    {
      out = imageOut+y;
      for (x=0; x<imageWidth; x++)
 	{
	  *out = *in;
	  in++;
	  out += imageHeight;
 	}
    }
  return MORPHO_SUCCESS;
}

/*! 
 * \fn int is_size_valid_1D(int size, int imageSize, char *func, int odd)
 * \param[in]  size Size to be checked
 * \param[in]  imageSize Dimension of the image input buffer
 * \param[in]  *func The name of the calling function
 * \param[in]  odd Tells if the size should be odd
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * \brief Various checks on the sizes. 
 * \ingroup libmorpho
 * 
 * This function is used for internal purposes only. It checks the size 
 * of the structuring element and compares it to the image dimensions.
 * For full technical details related to sizes please refer to \ref sectionBorder.
 *
 */
int is_size_valid_1D(int size, int imageSize, char *func, int odd)
{
  char st[200];

  if (DEBUG) { 
	printf ("Running %s ... (8 bits/grayscale only) ", func); 
  	fflush(stdout);
  	}

  if (size<2) {
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be >=2.", func, size);
	perror(st);
	return MORPHO_ERROR; 
  	}

  if (size>=imageSize) {
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements is not < to the image dimension(=%d).", func, size, imageSize);
	perror(st);
	return MORPHO_ERROR; 
  	}
  
  if ( (1==odd) && (1!=(size%2)) ) {
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be odd.", func, size);
	perror(st);
	return MORPHO_ERROR; 
  	}

  return MORPHO_SUCCESS;
}
