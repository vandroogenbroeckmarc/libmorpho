/* LIBMORPHO
 *
 * closingArbitrarySE.c
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
 * \file closingArbitrarySE.c
 */ 

#include "arbitraryUtil.h"

/*!
 * \fn int closing_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in] *se Buffer containing the shape of a structuring element. 
 * \param[in] seWidth Width of the stucturing element buffer
 * \param[in] seHeight Height of the stucturing element buffer
 * \param[in] seHorizontalOrigin Horizontal position of the origin in the structuring element (position 0 is the first pixel on the left). se[seHorizontalOrigin, seVerticalOrigin] must be !=0.
 * \param[in] seVerticalOrigin Vertical position of the origin in the structuring element (position 0 is the first pixel on the top). se[seHorizontalOrigin, seVerticalOrigin] must be !=0.
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Closing by an arbitrary structuring element
 *
 * \ingroup libmorpho
 *
 * Closing by an arbitrary structuring element.
 * For full technical details please refer to \ref detailsPage 
 * or to 
 * - M. Van Droogenbroeck and H. Talbot. <b>Fast Computation of morphological operations with arbitrary structuring elements</b>. <em>Pattern Recognition Letters</em>, 17(14):1451-1460, 1996.
 * 
 * 
 * \author      Marc Van Droogenbroeck
 */
int closing_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
{
uint8_t	*bloc;

if (DEBUG) printf("Running closing_arbitrary_SE\n");

/* Allocates a new picture */
if ( (bloc = (uint8_t *)malloc(imageWidth*imageHeight*sizeof(uint8_t))) == NULL)
	{
	perror("Malloc");
	return MORPHO_ERROR;
	}


/* Steps include: erosion, invert SE, and dilation */
if (MORPHO_ERROR == dilation_arbitrary_SE( imageIn, bloc, imageWidth, imageHeight, se, seWidth, seHeight, seHorizontalOrigin, seVerticalOrigin) ) return MORPHO_ERROR;

if (MORPHO_ERROR == erosion_arbitrary_SE(bloc, imageOut, imageWidth, imageHeight, se, seWidth, seHeight, seHorizontalOrigin, seVerticalOrigin) ) return MORPHO_ERROR;

/* Free the data */
free(bloc); 

return MORPHO_SUCCESS;
}
