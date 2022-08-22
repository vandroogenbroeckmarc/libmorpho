/* LIBMORPHO
 *
 * openingByAnchor.c
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
 * \file openingByAnchor.c
 */ 

#include "libmorpho.h"

/*!
 * \fn int openingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 * 
 * \brief Opening with an horizontal linear segment
 * 
 * \ingroup libmorpho
 *
 * Opening with an horizontal linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int openingByAnchor_1D_horizontal(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
  uint8_t *in,*out,*aux,*end;
  uint8_t *outLeft,*outRight,*current,*sentinel; 
  uint8_t min;
  int 	j,imageWidthMinus1,sizeMinus1;
  int 	*histo,nbrBytes;

  /* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageWidth, "openingByAnchor_1D_horizontal", 0) ) return MORPHO_ERROR;

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
      while ( (outLeft < outRight) && (*outLeft >= *(outLeft+1)) )
	{ outLeft++; }

      /* Right side */
      while ( (outLeft < outRight) && (*(outRight-1) <= *outRight) )
	{ outRight--; }

      /* Enters in the loop */
    startLine:
      min = *outLeft;
      current = outLeft+1;
      while ((current<outRight) && (*current<=min))
	{ min=*current; outLeft++; current++; }
      sentinel = outLeft+size;
      if (sentinel>outRight) { goto finishLine; }
		
      /* We ran "size" pixels ahead */ 
      current++; 
      while (current<sentinel)
	{
	  if (*current<=min) /* We have found a new minimum */
	    {
	      end = current;
	      outLeft++; 
	      while (outLeft < end) { *outLeft=min; outLeft++; }
	      outLeft = current; 
	      goto startLine; 
	    }
	  current++; 
	}

      /* We did not find a smaller value in the segment in reach
       * of outLeft; current is the first position outside the 
       * reach of outLeft 
       */
      if (*current<=min)
	{
	  end = current;
	  outLeft++; 
	  while (outLeft < end) { *outLeft=min; outLeft++; }
	  outLeft = current;
	  goto startLine; 
	}
      else	/* We can not avoid computing the histogram */
	{
	  memset(histo, 0, nbrBytes);
	  outLeft++; 
	  for (aux=outLeft; aux<=current; aux++) { histo[*aux]++; }
	  min++; while (histo[min]<=0) { min++; }
	  histo[*outLeft]--;
	  *outLeft = min;
	  histo[min]++;
	}
		
      /* We just follow the pixels, update the histogram and look for
       * the minimum */
      while (current < outRight)  
	{ 
	  current++; 
	  if (*current <= min)
	    {
	      /* We have found a new mimum */
	      end = current;
	      outLeft++; 
	      while (outLeft < end) { *outLeft=min; outLeft++; }
	      outLeft = current; 
	      goto startLine; 
	    }
	  else 
	    {
	      /* Update the histogram */
	      histo[*current]++;
	      histo[*outLeft]--;
	      /* Recompute the minimum */
	      while (histo[min]<=0) { min++; }
	      outLeft++; 
	      histo[*outLeft]--;
	      *outLeft=min; 
	      histo[min]++;
	    }
	}
   
      /* We have to finish the line */
      while (outLeft < outRight)
	{
	  histo[*outLeft]--;
	  while (histo[min]<=0) { min++; }
	  outLeft++; 
	  histo[*outLeft]--;
	  *outLeft=min; 
	  histo[min]++;
	}

    finishLine:
      while (outLeft < outRight)
	{
	  if (*outLeft<=*outRight)
	    {
	      min=*outRight; outRight--; 
	      if (*outRight>min) 	{ *outRight=min; }
	    }
	  else
	    {
	      min=*outLeft; outLeft++; 
	      if (*outLeft>min) 	{ *outLeft=min; }
	    }
	}
     }

  /* Free memory */
  free(histo);
  if(DEBUG) printf(" finished.\n");
  return MORPHO_SUCCESS;
}

/*!
 * \fn int openingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  size (= width in tems of pixels) of the linear structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Opening with a vertical linear segment
 * 
 * \ingroup libmorpho
 *
 * Opening with a vertical linear segment whose size is given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int openingByAnchor_1D_vertical(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int size)
{
uint8_t *in,*out,*aux,*end;
uint8_t *outUp,*outDown,*current,*sentinel; 
uint8_t min;
int 	j,imageJump,sizeJump,sizeMinus1;
int 	*histo,nbrBytes;

/* Tests */
  if ( MORPHO_ERROR == is_size_valid_1D(size, imageHeight, "openingByAnchor_1D_vertical", 0) ) return MORPHO_ERROR;

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
        /* Left side */
        while ( (outUp < outDown) && (*outUp >= *(outUp+imageWidth)) )
                { outUp+=imageWidth; }

        /* Right side */
        while ( (outUp < outDown) && (*(outDown-imageWidth) <= *outDown) )
                { outDown-=imageWidth; }

   /* Enters in the loop */
startLine:
   	min = *outUp;
   	current = outUp+imageWidth;
	while ((current<outDown) && (*current<=min))
               	{ min=*current; outUp+=imageWidth; current+=imageWidth; }
   	sentinel = outUp+sizeJump;
   	if (sentinel>outDown) { goto finishLine; }
		
	/* We ran "size" pixels ahead */ 
	current+=imageWidth; 
	while (current<sentinel)
		{
		if (*current<=min) /* We have found a new minimum */
			{
			end = current;
			outUp+=imageWidth; 
			while (outUp < end) { *outUp=min; outUp+=imageWidth; }
			outUp = current; 
			goto startLine; 
			}
		current+=imageWidth; 
		}

   	/* We did not find a smaller value in the segment in reach
	 * of outUp; current is the first position outside the 
	 * reach of outUp 
 	 */
	if (*current<=min)
		{
		end = current;
		outUp+=imageWidth; 
		while (outUp < end) { *outUp=min; outUp+=imageWidth; }
		outUp = current;
		goto startLine; 
		}
	else	/* We can not avoid computing the histogram */
		{
		memset(histo, 0, nbrBytes);
		outUp+=imageWidth; 
		for (aux=outUp; aux<=current; aux+=imageWidth) { histo[*aux]++; }
		min++; while (histo[min]<=0) { min++; }
		histo[*outUp]--;
		*outUp = min;
		histo[min]++;
		}
		
	/* We just follow the pixels, update the histogram and look for
	 * the minimum */
   	while (current < outDown)  
		{ 
		current+=imageWidth; 
		if (*current <= min)
			{
			/* We have found a new mimum */
			end = current;
			outUp+=imageWidth; 
			while (outUp < end) { *outUp=min; outUp+=imageWidth; }
			outUp = current; 
			goto startLine; 
			}
		else 
			{
			/* Update the histogram */
			histo[*current]++;
			histo[*outUp]--;
			/* Recompute the minimum */
			while (histo[min]<=0) { min++; }
			outUp+=imageWidth; 
			histo[*outUp]--;
			*outUp=min; 
			histo[min]++;
			}
		}
   
	/* We have to finish the line */
	while (outUp < outDown)
		{
		histo[*outUp]--;
		while (histo[min]<=0) { min++; }
		outUp+=imageWidth; 
		histo[*outUp]--;
		*outUp=min; 
		histo[min]++;
		}

finishLine:
		while (outUp < outDown)
		{
		if (*outUp<=*outDown)
			{
			min=*outDown; outDown-=imageWidth; 
			if (*outDown>min) 	{ *outDown=min; }
			}
		else
			{
			min=*outUp; outUp+=imageWidth; 
			if (*outUp>min) 	{ *outUp=min; }
			}
		}
   }

/* Free memory */
free(histo);
if(DEBUG) printf(" finished.\n");
 return MORPHO_SUCCESS;
}


/*!
 * \fn int openingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in]  seWidth (= width in tems of pixels) of the rectangular structuring element
 * \param[in]  seHeight (= height in tems of pixels) of the rectangular structuring element
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Opening with a seWidth * seHeight rectangle  structuring element
 * 
 * \ingroup libmorpho
 *
 * Two-dimensional opening with a rectangle. Both horizontal and vertical 
 * sizes are given in pixels.
 * For full technical details please refer to \ref detailsPage
 * or to
 * - M. Van Droogenbroeck and M. Buckley. <b>Morphological erosions and openings: 
fast algorithms based on anchors</b>. <em>Journal of Mathematical Imaging and Vision</em>, Special Issue on Mathematical Morphology after 40 Years, 22(2-3):121-142, May 2005.
 *
 * \author      Marc Van Droogenbroeck
 */
int openingByAnchor_2D(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, int seWidth, int seHeight)
{
  uint8_t	*bloc=NULL;
  int err1, err2, err3;

  if ((bloc=(uint8_t*)malloc(imageWidth*imageHeight*sizeof(uint8_t))) == NULL) {
    perror("Malloc");
    return MORPHO_ERROR;
  }

  err1 = erosionByAnchor_1D_horizontal(imageIn, bloc, imageWidth, imageHeight, seWidth);
  err2 = openingByAnchor_1D_vertical(bloc, bloc, imageWidth, imageHeight, seHeight);
  err3 = dilationByAnchor_1D_horizontal(bloc, imageOut, imageWidth, imageHeight, seWidth);

  free(bloc);

   if ( (MORPHO_SUCCESS == err1) && (MORPHO_SUCCESS == err2) && (MORPHO_SUCCESS == err3) )
        return MORPHO_SUCCESS;
  else  return MORPHO_ERROR;
}

