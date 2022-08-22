/* LIBMORPHO
 *
 * dilationArbitrarySE.c
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
 * \file dilationArbitrarySE.c
 */ 

#include "arbitraryUtil.h"

/*!
 * \fn int dilation_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
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
 * \brief Dilation by an arbitrary structuring element
 *
 * \ingroup libmorpho
 *
 * Dilation by an arbitrary structuring element.
 * For full technical details please refer to \ref detailsPage
 * or to 
 * - M. Van Droogenbroeck and H. Talbot. <b>Fast Computation of morphological operations with arbitrary structuring elements</b>. <em>Pattern Recognition Letters</em>, 17(14):1451-1460, 1996.
 * 
 * \author      Marc Van Droogenbroeck
 */
int dilation_arbitrary_SE(uint8_t *imageIn, uint8_t *imageOut, int imageWidth, int imageHeight, uint8_t *se, int seWidth, int seHeight, int seHorizontalOrigin, int seVerticalOrigin)
{
char st[200];

uint8_t	*bloc;
struct	front *l,*r,*u,*d;
int	i,j,ret;
int  	blocWidth, blocHeight;
uint8_t	*se2;
int	se2HorizontalOrigin, se2VerticalOrigin;

/* Test the compatibility */
if (imageWidth <= seWidth) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "dilation_arbitrary_SE", seWidth, imageWidth);        
	perror(st);
        return MORPHO_ERROR;
        }

if (imageHeight <= seHeight) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring elements should be larger than the image one(=%d).", "dilation_arbitrary_SE", seHeight, imageHeight);        
	perror(st);
        return MORPHO_ERROR;
        }

/* First of all we invert the structuring function */
if ( (se2 = (uint8_t *)malloc(seWidth*seHeight*sizeof(uint8_t))) == NULL)
	{ perror("Malloc"); return MORPHO_ERROR; }
invert_SE(se, se2, seWidth, seHeight);
se2HorizontalOrigin = seWidth-1-seHorizontalOrigin;
se2VerticalOrigin = seHeight-1-seVerticalOrigin;

/* We proceed to the analysis of the structuring element 
   and search for an origin */
l = (struct front *)malloc(sizeof(struct front));
r = (struct front *)malloc(sizeof(struct front));
u = (struct front *)malloc(sizeof(struct front));
d = (struct front *)malloc(sizeof(struct front));
if ( MORPHO_SUCCESS != analyse_b(se2,seWidth,seHeight, l,r,u,d, se2HorizontalOrigin, se2VerticalOrigin) 
   )
	{
	perror("ERROR(dilation_arbitrary_SE): analyse_b did not return a valid code");
	return MORPHO_ERROR;
	}

/* Allocate a new picture with a border */
blocWidth = imageWidth+seWidth*2;
blocHeight = imageHeight+seHeight*2;
bloc = (uint8_t *)malloc(blocWidth*blocHeight*sizeof(uint8_t));
for (i=0; i<blocWidth*blocHeight; i++) bloc[i]=SMALLEST_UINT8;
for (j=0;j<imageHeight;j++)
  for (i=0;i<imageWidth;i++)  
	bloc[i+seWidth+(j+seHeight)*blocWidth] = imageIn[i+j*imageWidth];

/* Transforms the information contained in the front structures */
if ( MORPHO_SUCCESS != transform_b(blocWidth,l,r,u,d) )
	{
	perror("ERROR(dilation_arbitrary_SE): transform_b did not return a valid code");
	return MORPHO_ERROR;
	}

/* Proceed to the dilation; 
   ATTENTION: sizeof(im_inter->f...) != sizeof(im_out->f...) */
ret = dilation_volume(bloc,blocWidth,blocHeight, imageOut,imageWidth,imageHeight, se,seWidth,seHeight, l,r,u,d, se2HorizontalOrigin,se2VerticalOrigin);

if ( MORPHO_SUCCESS != ret)
	{
	perror("ERROR(dilation_arbitrary_SE): dilation_volume did not return a valid code");
	return MORPHO_ERROR;
	}

/* Free the data */
free(bloc); 
free(se2);
free_front(l); free_front(r); free_front(u); free_front(d);
free(l); free(r); free(u); free(d);
return MORPHO_SUCCESS;
}

/*************************************************************/
/* Dilation procedure		                             */
/* ATTENTION: sizeof(in) != sizeof(out) 		     */
/*************************************************************/
int dilation_volume(	uint8_t *bloc, int blocWidth, int blocHeight,
		uint8_t *out, int imageWidth, int imageHeight,
		uint8_t *se, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		int ox,int oy)
{
uint8_t	max,*corner,val;
int	i,space[256],*histo,col,*pos,line,lx,n;


/* Build the first histogram */
for (i=0;i<=255;i++)  space[i]=0;
histo = space;
for (i=0;i<bh*bv;i++)
   if (se[i] != 0)	
	histo[0]++;
max=SMALLEST_UINT8;

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
		if (val > max) max=val;
		pos++; 
		}
	/* 3. Removing "flat" pixels */
 	   pos=l->pos; 
   	   for (n=0;n<l->size;n++)   
		{ histo[*(corner+*pos)]--; pos++; }
	/* Recomputes the maximum if necessary */
	while (histo[max] == 0) max--;
	/* Puts the value in the picture */
	if ( (col+1+ox>=bh) && (col+1-bh+ox<imageWidth) && 
	        (line-bv+oy>=0) && (line-bv+oy<imageHeight) )
			out[col+1-bh+ox+(line-bv+oy)*imageWidth] = max;
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
		if (val > max) max=val;
		pos++; 
		}
	/* 3. Removing "flat" pixels */
 	   pos=r->pos; 
   	   for (n=0;n<r->size;n++)   
		{ histo[*(corner+*pos)]--; pos++;  }
	/* Recomputes the maximum if necessary */
	while (histo[max] == 0) max--;
	/* Put the value in the picture */
	if ( (col-1+ox>=bh) && (col-1-bh+ox<imageWidth) && 
	        (line-bv+oy>=0) && (line-bv+oy<imageHeight) )
			out[col-1-bh+ox+(line-bv+oy)*imageWidth] = max;
	}
    }

return MORPHO_SUCCESS;
}
