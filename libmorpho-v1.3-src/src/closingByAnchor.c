/* LIBMORPHO
 *
 * closingByAnchor.c
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
 * \file closingByAnchor.c
 */ 


#include "libmorpho.h"

/*!
 * \fn int closingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Closing with an horizontal linear segment
 *
 * \ingroup libmorpho
 *
 * Closing with an horizontal linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int closingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
  uint8_t *in,*out,*aux,*end;
  uint8_t *outLeft,*outRight,*current,*sentinel; 
  uint8_t max;
  int 	j,imageWidthMinus1,sizeMinus1;
  int 	*histo,nbrBytes;

  /* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageWidth, "closingByAnchor_1D_horizontal", 0) ) return MORPHO_ERROR;

  in =  imageIn;
  out = imageOut;
  imageWidthMinus1 = imageWidth-1;
  sizeMinus1 = size-1;

  /* Copy the input into the output */
  memcpy(out, in, imageWidth*imageHeight*sizeof(uint8_t));

  /* Initialisation of the histogram */
  nbrBytes = 256*sizeof(int);
  histo = (int *)malloc(nbrBytes);

  /* Computation */
  out = imageOut;

  /* Row by row */
  for (j=0; j<imageHeight; j++)
    {
      /* Initialisation of both extremities of a line */
      outLeft = out+(j*imageWidth);
      outRight = outLeft+imageWidthMinus1;

      /* Handling of both sides */
      /* Left side */
      while ( (outLeft < outRight) && (*outLeft <= *(outLeft+1)) )
	{ outLeft++; }

      /* Right side */
      while ( (outLeft < outRight) && (*(outRight-1) >= *outRight) )
	{ outRight--; }

      /* Enters in the loop */
    startLine:
      max = *outLeft;
      current = outLeft+1;
      while ((current<outRight) && (*current>=max))
	{ max=*current; outLeft++; current++; }
      sentinel = outLeft+size;
      if (sentinel>outRight) { goto finishLine; }
		
      /* We ran "size" pixels ahead */ 
      current++; 
      while (current<sentinel)
	{
	  if (*current>=max) /* We have found a new maximum */
	    {
	      end = current;
	      outLeft++; 
	      while (outLeft < end) { *outLeft=max; outLeft++; }
	      outLeft = current; 
	      goto startLine; 
	    }
	  current++; 
	}

      /* We did not find a larger value in the segment in reach
       * of outLeft; current is the first position outside the 
       * reach of outLeft 
       */
      if (*current>=max)
	{
	  end = current;
	  outLeft++; 
	  while (outLeft < end) { *outLeft=max; outLeft++; }
	  outLeft = current;
	  goto startLine; 
	}
      else	/* We can not avoid computing the histogram */
	{
	  memset(histo, 0, nbrBytes);
	  outLeft++; 
	  for (aux=outLeft; aux<=current; aux++) { histo[*aux]++; }
	  max--; while (histo[max]<=0) { max--; }
	  histo[*outLeft]--;
	  *outLeft = max;
	  histo[max]++;
	}
		
      /* We just follow the pixels, update the histogram and look for
       * the maximum */
      while (current < outRight)  
	{ 
	  current++; 
	  if (*current >= max)
	    {
	      /* We have found a new maximum */
	      end = current;
	      outLeft++; 
	      while (outLeft < end) { *outLeft=max; outLeft++; }
	      outLeft = current; 
	      goto startLine; 
	    }
	  else 
	    {
	      /* Update the histogram */
	      histo[*current]++;
	      histo[*outLeft]--;
	      /* Recompute the minimum */
	      while (histo[max]<=0) { max--; }
	      outLeft++; 
	      histo[*outLeft]--;
	      *outLeft=max; 
	      histo[max]++;
	    }
	}
   
      /* We have to finish the line */
      while (outLeft < outRight)
	{
	  histo[*outLeft]--;
	  while (histo[max]<=0) { max--; }
	  outLeft++; 
	  histo[*outLeft]--;
	  *outLeft=max; 
	  histo[max]++;
	}

    finishLine:
      while (outLeft < outRight)
	{
	  if (*outLeft<=*outRight)
	    {
	      max=*outRight; outRight--; 
	      if (*outRight<max) 	{ *outRight=max; }
	    }
	  else
	    {
	      max=*outLeft; outLeft++; 
	      if (*outLeft<max) 	{ *outLeft=max; }
	    }
	}
    }

  /* Free memory */
  free(histo);
  if(DEBUG)
    printf(" finished.\n");
  return MORPHO_SUCCESS;
}

/*!
 * \fn int closingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * 
 * \brief Closing with a vertical linear segment
 * 
 * \ingroup libmorpho
 *
 * Closing with a vertical linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int closingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
  uint8_t *in,*out,*aux,*end;
  uint8_t *outUp,*outDown,*current,*sentinel; 
  uint8_t max;
  int 	j,imageJump,sizeJump,sizeMinus1;
  int 	*histo,nbrBytes;

  /* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageHeight, "closingByAnchor_1D_vertical", 0) ) return MORPHO_ERROR;

  in =  imageIn;
  out = imageOut;
  imageJump = imageWidth*(imageHeight-1);
  sizeMinus1 = size-1;
  sizeJump = size*imageWidth;

  /* Copy the input into the output */
  memcpy(out, in, imageWidth*imageHeight*sizeof(uint8_t));

  /* Initialisation of the histogram */
  nbrBytes = 256*sizeof(int);
  histo = (int *)malloc(nbrBytes);

  /* Computation */
  out = imageOut;

  /* Row by row */
  for (j=0; j<imageWidth; j++)
    {
      /* Initialisation of both extremities of a line */
      outUp = out+j;
      outDown = outUp+imageJump;

      /* Handling of both sides */
      /* Up side */
      while ( (outUp < outDown) && (*outUp <= *(outUp+imageWidth)) )
	{ outUp+=imageWidth; }

      /* Down side */
      while ( (outUp < outDown) && (*(outDown-imageWidth) >= *outDown) )
	{ outDown-=imageWidth; }

      /* Enters in the loop */
    startLine:
      max = *outUp;
      current = outUp+imageWidth;
      while ((current<outDown) && (*current>=max))
	{ max=*current; outUp+=imageWidth; current+=imageWidth; }
      sentinel = outUp+sizeJump;
      if (sentinel>outDown) { goto finishLine; }
		
      /* We ran "size" pixels ahead */ 
      current+=imageWidth; 
      while (current<sentinel)
	{
	  if (*current>=max) /* We have found a new maximum */
	    {
	      end = current;
	      outUp+=imageWidth; 
	      while (outUp < end) { *outUp=max; outUp+=imageWidth; }
	      outUp = current; 
	      goto startLine; 
	    }
	  current+=imageWidth; 
	}

      /* We did not found a larger value in the segment in reach
       * of outUp; current is the first position outside the 
       * reach of outUp 
       */
      if (*current>=max)
	{
	  end = current;
	  outUp+=imageWidth; 
	  while (outUp < end) { *outUp=max; outUp+=imageWidth; }
	  outUp = current;
	  goto startLine; 
	}
      else	/* We can not avoid computing the histogram */
	{
	  memset(histo, 0, nbrBytes);
	  outUp+=imageWidth; 
	  for (aux=outUp; aux<=current; aux+=imageWidth) { histo[*aux]++; }
	  max--; while (histo[max]<=0) { max--; }
	  histo[*outUp]--;
	  *outUp = max;
	  histo[max]++;
	}
		
      /* We just follow the pixels, update the histogram and look for
       * the maximum */
      while (current < outDown)  
	{ 
	  current+=imageWidth; 
	  if (*current >= max)
	    {
	      /* We have found a new maximum */
	      end = current;
	      outUp+=imageWidth; 
	      while (outUp < end) { *outUp=max; outUp+=imageWidth; }
	      outUp = current; 
	      goto startLine; 
	    }
	  else 
	    {
	      /* Update the histogram */
	      histo[*current]++;
	      histo[*outUp]--;
	      /* Recompute the minimum */
	      while (histo[max]<=0) { max--; }
	      outUp+=imageWidth; 
	      histo[*outUp]--;
	      *outUp=max; 
	      histo[max]++;
	    }
	}
   
      /* We have to finish the line */
      while (outUp < outDown)
	{
	  histo[*outUp]--;
	  while (histo[max]<=0) { max--; }
	  outUp+=imageWidth; 
	  histo[*outUp]--;
	  *outUp=max; 
	  histo[max]++;
	}

    finishLine:
      while (outUp < outDown)
	{
	  if (*outUp<=*outDown)
	    {
	      max=*outDown; outDown-=imageWidth; 
	      if (*outDown<max) 	{ *outDown=max; }
	    }
	  else
	    {
	      max=*outUp; outUp+=imageWidth; 
	      if (*outUp<max) 	{ *outUp=max; }
	    }
	}
    }

  /* Free memory */
  free(histo);
  if(DEBUG)
    printf(" finished.\n");
  return MORPHO_SUCCESS;
}

/*!
 * \fn int closingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  seWidth (= width in tems of pixels) of the rectangular structuring element
 * \param[in]  seHeight (= height in tems of pixels) of the rectangular structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Closing with a seWidth * seHeight rectangle  structuring element
 * 
 * \ingroup libmorpho
 *
 * Two-dimensional closing with a rectangle. Both horizontal and vertical
 * sizes are given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int closingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
{
  uint8_t	*bloc=NULL;
  int err1, err2, err3;  

  if ((bloc=(uint8_t*)malloc(imageWidth*imageHeight*sizeof(uint8_t))) == NULL){
    perror("Malloc");
    return MORPHO_ERROR;
  }

  err1 = dilationByAnchor_1D_horizontal(imageIn, bloc, imageWidth, imageHeight, seWidth);  
  err2 = closingByAnchor_1D_vertical(bloc, bloc, imageWidth, imageHeight, seHeight);
  err3 = erosionByAnchor_1D_horizontal(bloc, imageOut, imageWidth, imageHeight, seWidth);
  
  free(bloc);

  if ( (MORPHO_SUCCESS == err1) && (MORPHO_SUCCESS == err2) && (MORPHO_SUCCESS == err3) )
        return MORPHO_SUCCESS;
  else  return MORPHO_ERROR;

}

