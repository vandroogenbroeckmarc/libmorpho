/* LIBMORPHO
 *
 * closingArbitrarySF.c
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
 * \file closingArbitrarySF.c
 */ 

#include "arbitraryUtil.h"

/*!
 * \fn int closing_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf1, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in] *sf1 Buffer containing the shape of a structuring function
 * \param[in] sfWidth Width of the stucturing buffer
 * \param[in] sfHeight Height of the stucturing function buffer
 * \param[in] sfHorizontalOrigin Horizontal position of the origin in the structuring function (position 0 is the first pixel on the left). sf[sfHorizontalOrigin, sfVerticalOrigin] must be !=0.
 * \param[in] sfVerticalOrigin Vertical position of the origin in the structuring function (position 0 is the first pixel on the top). sf[sfHorizontalOrigin, sfVerticalOrigin] must be !=0.
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Closing by an arbitrary structuring function
 *
 * \ingroup libmorpho
 *
 * Closing by an arbitrary structuring function.
 * For full technical details please refer to \ref detailsPage
 * or to 
 * - M. Van Droogenbroeck and H. Talbot. <b>Fast Computation of morphological operations with arbitrary structuring elements</b>. <em>Pattern Recognition Letters</em>, 17(14):1451-1460, 1996.
 * 
 * \warning All pixels of the input image should => -255 and <= 510: -255 <= imageIn[.] <= 510. 
 * To avoid any computation overhead the function does not check that the input image 
 * is compliant to this rule. 
 *
 * \author      Marc Van Droogenbroeck
 */
int closing_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf1, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin)
{
int16_t	*bloc;

if (DEBUG) printf("Running closing_arbitrary_SF\n");

/* Allocates a new picture */
if ( (bloc = (int16_t *)malloc(imageWidth*imageHeight*sizeof(int16_t))) == NULL)
	{
	perror("Malloc");
	return MORPHO_ERROR;
	}

/* Steps include: erosion, invert SE, and dilation */
if (MORPHO_ERROR == dilation_arbitrary_SF( imageIn, bloc, imageWidth, imageHeight, sf1, sfWidth, sfHeight, sfHorizontalOrigin, sfVerticalOrigin) ) return MORPHO_ERROR;

if (MORPHO_ERROR == erosion_arbitrary_SF(bloc, imageOut, imageWidth, imageHeight, sf1, sfWidth, sfHeight, sfHorizontalOrigin, sfVerticalOrigin) ) return MORPHO_ERROR;

/* Free the data */
free(bloc); 

return MORPHO_SUCCESS;
}
