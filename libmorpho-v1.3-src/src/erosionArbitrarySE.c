/* LIBMORPHO
 *
 * erosionArbitrarySE.c
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
 * \file erosionArbitrarySE.c
 */ 

#include "arbitraryUtil.h"

/*!
 * \fn int erosion_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
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
 * \brief Erosion by an arbitrary structuring element
 *
 * \ingroup libmorpho
 *
 * Erosion by an arbitrary structuring element.
 * For full technical details please refer to \ref detailsPage 
 * or to 
 * - M. Van Droogenbroeck and H. Talbot. <b>Fast Computation of morphological operations with arbitrary structuring elements</b>. <em>Pattern Recognition Letters</em>, 17(14):1451-1460, 1996.
 * 
 * \author      Marc Van Droogenbroeck
 */
int erosion_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
{
char st[200];

uint8_t	*bloc;
struct	front *l,*r,*u,*d;
int	i,j,ret;
int  	blocWidth, blocHeight;

/* Test the compatibility */
if (imageWidth <= seWidth) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "erosion_arbitrary_SE", seWidth, imageWidth);        
	perror(st);
        return MORPHO_ERROR;
        }

if (imageHeight <= seHeight) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "erosion_arbitrary_SE", seHeight, imageHeight);        
	perror(st);
        return MORPHO_ERROR;
        }

/* First, we proceed to the analysis of the structuring element 
   and search for an origin */
l = (struct front *)malloc(sizeof(struct front));
r = (struct front *)malloc(sizeof(struct front));
u = (struct front *)malloc(sizeof(struct front));
d = (struct front *)malloc(sizeof(struct front));
if ( MORPHO_SUCCESS != analyse_b(se,seWidth,seHeight, l,r,u,d, seHorizontalOrigin, seVerticalOrigin) 
   )
	{
	perror("ERROR(erosion_arbitrary_SE): analyse_b did not return a valid code");
	return MORPHO_ERROR;
	}

/* Allocate a new picture with a border */
blocWidth = imageWidth+seWidth*2;
blocHeight = imageHeight+seHeight*2;
bloc = (uint8_t *)malloc(blocWidth*blocHeight*sizeof(uint8_t));
for (i=0; i<blocWidth*blocHeight; i++) bloc[i]=LARGEST_UINT8;
for (j=0;j<imageHeight;j++)
  for (i=0;i<imageWidth;i++)  
	bloc[i+seWidth+(j+seHeight)*blocWidth] = imageIn[i+j*imageWidth];

/* Transforms the information contained in the front structures */
if ( MORPHO_SUCCESS != transform_b(blocWidth,l,r,u,d) )
	{
	perror("ERROR(erosion_arbitrary_SE): transform_b did not return a valid code");
	return MORPHO_ERROR;
	}

/* Proceed to the erosion; 
   ATTENTION: sizeof(im_inter->f...) != sizeof(im_out->f...) */
ret = erosion_volume(bloc,blocWidth,blocHeight, imageOut,imageWidth,imageHeight, se,seWidth,seHeight,
		l,r,u,d, seHorizontalOrigin,seVerticalOrigin);

if ( MORPHO_SUCCESS != ret)
	{
	perror("ERROR(erosion_arbitrary_SE): erosion_volume did not return a valid code");
	return MORPHO_ERROR;
	}

/* Free the data */
free(bloc); 
free_front(l); free_front(r); free_front(u); free_front(d);
free(l); free(r); free(u); free(d);
return MORPHO_SUCCESS;
}

/*************************************************************/
/* Erosion procedure		                             */
/* ATTENTION: sizeof(in) != sizeof(out) 		     */
/*************************************************************/
int erosion_volume(	uint8_t *bloc, int blocWidth, int blocHeight,
		uint8_t *out, int imageWidth, int imageHeight,
		uint8_t *se, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		int ox,int oy)
{
uint8_t	min,*corner,val;
int	i,space[256],*histo,col,*pos,line,lx,n;


/* Build the first histogram */
for (i=0;i<=255;i++)  space[i]=0;
histo = space;
for (i=0;i<bh*bv;i++)
   if (se[i] != 0)	
	histo[LARGEST_UINT8]++;
min=LARGEST_UINT8;

/* Each line is scanned through */
for (line=0;line<imageHeight+bv;line++)
  if (line%2 == 0)
    {
    lx = line*blocWidth;
    /* From left to right */
    for (col=0;col<imageWidth+bh;col++)
	{
	corner = &bloc[col+lx];
	/* Updates the histogram */
	/* 1. Adding "flat" pixels */
 	   pos=r->pos; 
   	   for (n=0;n<r->size;n++)
		{
		val = *(corner+((*pos)+1));
		histo[val]++;
		if (val < min) min=val;
		pos++; 
		}
	/* 3. Removing "flat" pixels */
 	   pos=l->pos; 
   	   for (n=0;n<l->size;n++)   
		{ histo[*(corner+*pos)]--; pos++; }
	/* Recomputes the minimum if necessary */
	while (histo[min] == 0) min++;
	/* Puts the value in the picture */
	if ( (col+1+ox>=bh) && (col+1-bh+ox<imageWidth) && 
	        (line-bv+oy>=0) && (line-bv+oy<imageHeight) )
			out[col+1-bh+ox+(line-bv+oy)*imageWidth] = min;
	}
    }
  else 
    {
    lx = line*blocWidth;
    /* From right to left */
    for (col=imageWidth+bh;col>0;col--)
	{
	corner = &bloc[col+lx];
	/* Updates the histogram */
	/* 1. Adding "flat" pixels */
 	   pos=l->pos; 
   	   for (n=0;n<l->size;n++)
		{
		val = *(corner+((*pos)-1));
		histo[val]++;
		if (val < min) min=val;
		pos++; 
		}
	/* 3. Removing "flat" pixels */
 	   pos=r->pos; 
   	   for (n=0;n<r->size;n++)   
		{ histo[*(corner+*pos)]--; pos++;  }
	/* Recomputes the minimum if necessary */
	while (histo[min] == 0) min++;
	/* Put the value in the picture */
	if ( (col-1+ox>=bh) && (col-1-bh+ox<imageWidth) && 
	        (line-bv+oy>=0) && (line-bv+oy<imageHeight) )
			out[col-1-bh+ox+(line-bv+oy)*imageWidth] = min;
	}
    }

return MORPHO_SUCCESS;
}
