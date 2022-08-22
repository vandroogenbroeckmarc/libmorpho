/* LIBMORPHO
 *
 * dilationByAnchor.c
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
 * \file dilationByAnchor.c
 */ 

#include "libmorpho.h"

/*!
 * \fn int dilationByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * 
 * \brief Dilation with an horizontal linear segment
 *
 * \ingroup libmorpho
 *
 * Dilation with an horizontal linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 * 
 * \author      Marc Van Droogenbroeck
 */
int dilationByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
  uint8_t *in,*out,*aux;
  uint8_t *inLeft,*inRight,*outLeft,*outRight,*current,*sentinel; 
  uint8_t max;
  int 	i,j,imageWidthMinus1,sizeMinus1;
  int 	*histo,nbrBytes;
  int	middle;

  /* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageWidth, "dilationByAnchor_1D_horizontal", 1) ) return MORPHO_ERROR;

  in =  (uint8_t *)imageIn;
  out = (uint8_t *)imageOut;
  imageWidthMinus1 = imageWidth-1;
  sizeMinus1 = size-1;
  middle = size/2;

  /* Initialisation of the histogram */
  nbrBytes = 256*sizeof(int);
  histo = (int *)malloc(nbrBytes);

  /* Computation */
  /* Row by row */
  for (j=0; j<imageHeight; j++)
    {
      /* Initialisation of both extremities of a line */
      inLeft = in+(j*imageWidth);
      outLeft = out+(j*imageWidth);
      inRight = inLeft+imageWidthMinus1;
      outRight = outLeft+imageWidthMinus1;

      /* Handles the left border */ 
      /* First half of the structuring element */
      memset(histo, 0, nbrBytes);
      max = *inLeft; histo[max]++;
      for (i=0; i<middle; i++) 
   	{
	  inLeft++; 
	  histo[*inLeft]++;
	  if (*inLeft > max) { max = *inLeft; }
	}
      *outLeft = max;

      /* Second half of the structuring element */
      for (i=0; i<size-middle-1; i++) 
   	{
	  inLeft++; outLeft++;
	  histo[*inLeft]++;
	  if (*inLeft > max) { max = *inLeft; }
	  *outLeft = max;
	}

      /* Use the histogram as long as we have not found a new maximum */
      while ( (inLeft<inRight) && (max>=*(inLeft+1)))
   	{
	  inLeft++; outLeft++;
	  histo[*(inLeft-size)]--;
	  histo[*inLeft]++;
	  while (histo[max]<=0) { max--; }
	  *outLeft = max;
	}

      /* Enters in the loop */
      max = *outLeft;

    startLine:
      current = inLeft+1;
      while ((current<inRight) && (*current>=max))
	{ max=*current; outLeft++; *outLeft=max; current++; }
      inLeft = current-1;
      sentinel = inLeft+size;
      if (sentinel>inRight) { goto finishLine; }
      outLeft++;
      *outLeft = max;
		
      /* We ran "size" pixels ahead */ 
      current++; 
      while (current<sentinel)
	{
	  if (*current>=max) /* We have found a new maximum */
	    {
	      max = *current;
	      outLeft++; 
	      *outLeft = max;
	      inLeft = current;
	      goto startLine; 
	    }
	  current++; 
	  outLeft++; 
	  *outLeft = max;
	}

      /* We did not find a smaller value in the segment in reach
       * of inLeft; current is the first position outside the 
       * reach of inLeft 
       */
      if (*current>=max)
	{
	  max = *current;
	  outLeft++; 
	  *outLeft = max;
	  inLeft = current;
	  goto startLine; 
	}
      else	/* We can not avoid computing the histogram */
	{
	  memset(histo, 0, nbrBytes);
	  inLeft++; outLeft++; 
	  for (aux=inLeft; aux<=current; aux++) { histo[*aux]++; }
	  max--; while (histo[max]<=0) { max--; }
	  *outLeft = max;
	}
		
      /* We just follow the pixels, update the histogram and look for
       * the maximum */
      while (current < inRight)  
	{ 
	  current++; 
	  if (*current >= max)
	    {
	      /* We have found a new mimum */
	      max = *current;
	      outLeft++; 
	      *outLeft = max;
	      inLeft = current;
	      goto startLine; 
	    }
	  else 
	    {
	      /* Update the histogram */
	      histo[*current]++;
	      histo[*inLeft]--;
	      /* Recompute the maximum */
	      while (histo[max]<=0) { max--; }
	      inLeft++; outLeft++; 
	      *outLeft=max; 
	    }
	}

    finishLine:
      /* Handles the right border */ 
      /* First half of the structuring element */
      memset(histo, 0, nbrBytes);
      max = *inRight; histo[max]++;
      for (i=0; i<middle; i++) 
	{
	  inRight--; 
	  histo[*inRight]++;
	  if (*inRight > max) { max = *inRight; }
	}
      *outRight = max;
   
      /* Second half of the structuring element */
      for (i=0; (i<size-middle-1) && (outLeft<outRight); i++) 
	{
	  inRight--; outRight--;
	  histo[*inRight]++;
	  if (*inRight > max) { max = *inRight; }
	  *outRight = max;
	}

      /* Use the histogram as long as we have not found a new maximum */
      while ( outLeft<outRight )
	{
	  inRight--; outRight--;
	  histo[*(inRight+size)]--;
	  histo[*inRight]++;
	  if (*inRight > max) { max = *inRight; }
	  while (histo[max]<=0) { max--; }
	  *outRight = max;
	}
    }

  /* Free memory */
  free(histo);
  if(DEBUG)
    printf(" finished.\n");
  return MORPHO_SUCCESS;
}


/*!
 * \fn int dilationByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * 
 * \brief Dilation with a vertical linear segment
 * 
 * \ingroup libmorpho
 *
 * Dilation with a vertical linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 * 
 * \author      Marc Van Droogenbroeck
 */
int dilationByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
  uint8_t *in,*out,*aux;
  uint8_t *inUp,*inDown,*outUp,*outDown,*current,*sentinel; 
  uint8_t max;
  int 	i,j,imageJump,sizeJump,sizeMinus1;
  int 	*histo,nbrBytes;
  int	middle;

  /* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageHeight, "dilationByAnchor_1D_vertical", 1) ) return MORPHO_ERROR;

  in =  (uint8_t *)imageIn;
  out = (uint8_t *)imageOut;
  imageJump = imageWidth*(imageHeight-1);
  sizeMinus1 = size-1;
  sizeJump = size*imageWidth;
  middle = size/2;

  /* Initialisation of the histogram */
  nbrBytes = 256*sizeof(int);
  histo = (int *)malloc(nbrBytes);

  /* Computation */
  /* Row by row */
  for (j=0; j<imageWidth; j++)
    {
      /* Initialisation of both extremities of a column */
      inUp = in+j;
      outUp = out+j;
      inDown  = inUp+imageJump;
      outDown = outUp+imageJump;

      /* Handles the upper border */ 
      /* First half of the structuring element */
      memset(histo, 0, nbrBytes);
      max = *inUp; histo[max]++;
      for (i=0; i<middle; i++) 
   	{
	  inUp+=imageWidth; 
	  histo[*inUp]++;
	  if (*inUp > max) { max = *inUp; }
	}
      *outUp = max;

      /* Second half of the structuring element */
      for (i=0; i<size-middle-1; i++) 
   	{
	  inUp+=imageWidth; outUp+=imageWidth;
	  histo[*inUp]++;
	  if (*inUp > max) { max = *inUp; }
	  *outUp = max;
	}

      /* Uses the histogram as long as we have not found a new maximum */
      while ( (inUp<inDown) && (max>=*(inUp+imageWidth)))
   	{
	  inUp+=imageWidth; outUp+=imageWidth;
	  histo[*(inUp-sizeJump)]--;
	  histo[*inUp]++;
	  while (histo[max]<=0) { max--; }
	  *outUp = max;
	}

      /* Enters in the loop */
      max = *outUp;

    startLine:
      current = inUp+imageWidth;
      while ((current<inDown) && (*current>=max))
	{ max=*current; outUp+=imageWidth; *outUp=max; current+=imageWidth; }
      inUp = current-imageWidth;
      sentinel = inUp+sizeJump;
      if (sentinel>inDown) { goto finishLine; }
      outUp+=imageWidth;
      *outUp = max;
		
      /* We ran "size" pixels ahead */ 
      current+=imageWidth; 
      while (current<sentinel)
	{
	  if (*current>=max) /* We have found a new maximum */
	    {
	      max = *current;
	      outUp+=imageWidth; 
	      *outUp = max;
	      inUp = current;
	      goto startLine; 
	    }
	  current+=imageWidth; 
	  outUp+=imageWidth; 
	  *outUp = max;
	}

      /* We did not find a smaller value in the segment in reach
       * of inUp; current is the first position outside the 
       * reach of inUp 
       */
      if (*current>=max)
	{
	  max = *current;
	  outUp+=imageWidth; 
	  *outUp = max;
	  inUp = current;
	  goto startLine; 
	}
      else	/* We can not avoid computing the histogram */
	{
	  memset(histo, 0, nbrBytes);
	  inUp+=imageWidth; outUp+=imageWidth; 
	  for (aux=inUp; aux<=current; aux+=imageWidth) { histo[*aux]++; }
	  max--; while (histo[max]<=0) { max--; }
	  *outUp = max;
	}
		
      /* We just follow the pixels, update the histogram and look for
       * the maximum */
      while (current < inDown)  
	{ 
	  current+=imageWidth; 
	  if (*current >= max)
	    {
	      /* We have found a new mimum */
	      max = *current;
	      outUp+=imageWidth; 
	      *outUp = max;
	      inUp = current;
	      goto startLine; 
	    }
	  else 
	    {
	      /* Update the histogram */
	      histo[*current]++;
	      histo[*inUp]--;
	      /* Recompute the maximum */
	      while (histo[max]<=0) { max--; }
	      inUp+=imageWidth; outUp+=imageWidth; 
	      *outUp=max; 
	    }
	}

    finishLine:
      /* Handles the bottom border */ 
      /* First half of the structuring element */
      memset(histo, 0, nbrBytes);
      max = *inDown; histo[max]++;
      for (i=0; i<middle; i++) 
	{
	  inDown-=imageWidth; 
	  histo[*inDown]++;
	  if (*inDown > max) { max = *inDown; }
	}
      *outDown = max;
   
      /* Second half of the structuring element */
      for (i=0; (i<size-middle-1) && (outUp<outDown); i++) 
	{
	  inDown-=imageWidth; outDown-=imageWidth;
	  histo[*inDown]++;
	  if (*inDown > max) { max = *inDown; }
	  *outDown = max;
	}

      /* Use the histogram as long as we have not found a new maximum */
      while ( outUp<outDown )
	{
	  inDown-=imageWidth; outDown-=imageWidth;
	  histo[*(inDown+sizeJump)]--;
	  histo[*inDown]++;
	  if (*inDown > max) { max = *inDown; }
	  while (histo[max]<=0) { max--; }
	  *outDown = max;
	}
    }

  /* Free memory */
  free(histo);
  if(DEBUG) printf(" finished.\n");
  return MORPHO_SUCCESS;
}


/*!
 * \fn int dilationByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  seWidth (= width in tems of pixels) of the rectangular structuring element
 * \param[in]  seHeight (= height in tems of pixels) of the rectangular structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Dilation with a seWidth * seHeight rectangle structuring element
 * 
 * \ingroup libmorpho
 *
 * Two-dimensional dilation with a rectangle. Both horizontal and vertical 
 * sizes are given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 * 
 * \author      Marc Van Droogenbroeck
 */
int dilationByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
{
  uint8_t	*bloc=NULL;
  int err1, err2;

  if ((bloc=(uint8_t*)malloc(imageWidth*imageHeight*sizeof(uint8_t))) == NULL){
    perror("Malloc");
    return MORPHO_ERROR;
  }

  err1 = dilationByAnchor_1D_horizontal(imageIn, bloc, imageWidth, imageHeight, seWidth);
  err2 = dilationByAnchor_1D_vertical(bloc, imageOut, imageWidth, imageHeight, seHeight);

  free(bloc);

  if ( (MORPHO_SUCCESS == err1) && (MORPHO_SUCCESS == err2) )
        return MORPHO_SUCCESS;
  else  return MORPHO_ERROR;
}

