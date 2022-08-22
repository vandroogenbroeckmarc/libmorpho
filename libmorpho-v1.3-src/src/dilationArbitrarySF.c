/* LIBMORPHO
 *
 * dilationArbitrarySF.c
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
 * \file dilationArbitrarySF.c
 */ 

#include "arbitraryUtil.h"

/*!
 * \fn int dilation_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin)
 * \param[in]  *imageIn Input buffer
 * \param[out]  *imageOut Output buffer
 * \param[in]  imageWidth Width of the image buffer
 * \param[in]  imageHeight Height of the image buffer
 * \param[in] *sf Buffer containing the shape of a structuring function. 
 * \param[in] sfWidth Width of the stucturing function buffer
 * \param[in] sfHeight Height of the stucturing function buffer
 * \param[in] sfHorizontalOrigin Horizontal position of the origin in the structuring function (position 0 is the first pixel on the left). sf[sfHorizontalOrigin, sfVerticalOrigin] must be !=0.
 * \param[in] sfVerticalOrigin Vertical position of the origin in the structuring function (position 0 is the first pixel on the top). sf[sfHorizontalOrigin, sfVerticalOrigin] must be !=0.
 * \return Returns MORPHO_SUCCESS upon success, MORPHO_ERROR otherwise.
 *
 * \brief Dilation by an arbitrary structuring function
 *
 * \ingroup libmorpho
 *
 * Dilation by an arbitrary structuring function.
 * For full technical details please refer to \ref detailsPage
 * or to 
 * - M. Van Droogenbroeck and H. Talbot. <b>Fast Computation of morphological operations with arbitrary structuring elements</b>. <em>Pattern Recognition Letters</em>, 17(14):1451-1460, 1996.
 * 
 * \warning All pixels of the input image should => -255 and <= 510: -255 <= imageIn[.] <= 510.
 * To avoid any computation overhead the function does not check that the input image is compliant
 *  to this rule. 

 * \author      Marc Van Droogenbroeck
 */
int dilation_arbitrary_SF(int16_t *imageIn, int16_t *imageOut, int imageWidth, int imageHeight, uint8_t *sf, int sfWidth, int sfHeight, int sfHorizontalOrigin, int sfVerticalOrigin)
{
char st[200];

int16_t	*bloc;
struct	front *l,*r,*u,*d;
struct	gfront *gl,*gr,*gu,*gd;
int	i,j,ret;
int  	blocWidth, blocHeight;
uint8_t	*sf2;
int	sf2HorizontalOrigin, sf2VerticalOrigin;

/* Test the compatibility */
if (imageWidth <= sfWidth) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring function should be larger than the image one(=%d).", "dilation_arbitrary_SF", sfWidth, imageWidth);        
	perror(st);
        return MORPHO_ERROR;
        }

if (imageHeight <= sfHeight) 
	{ 
	snprintf(st, 200, "ERROR(%s): size(=%d) of the structuring function should be larger than the image one(=%d).", "dilation_arbitrary_SF", sfHeight, imageHeight);        
	perror(st);
        return MORPHO_ERROR;
        }

/* First of all we invert the structuring function */
if ( (sf2 = (uint8_t *)malloc(sfWidth*sfHeight*sizeof(uint8_t))) == NULL)
	{ perror("Malloc"); return MORPHO_ERROR; }
invert_SE(sf, sf2, sfWidth, sfHeight);
sf2HorizontalOrigin = sfWidth-1-sfHorizontalOrigin;
sf2VerticalOrigin = sfHeight-1-sfVerticalOrigin;

/* We proceed to the analysis of the structuring function 
   and search for an origin */
l = (struct front *)malloc(sizeof(struct front));
r = (struct front *)malloc(sizeof(struct front));
d = (struct front *)malloc(sizeof(struct front));
u = (struct front *)malloc(sizeof(struct front));
gl = (struct gfront *)malloc(sizeof(struct gfront));
gr = (struct gfront *)malloc(sizeof(struct gfront));
gd = (struct gfront *)malloc(sizeof(struct gfront));
gu = (struct gfront *)malloc(sizeof(struct gfront));
if ( MORPHO_SUCCESS != analyse_b_gray(sf2,sfWidth,sfHeight, l,r,u,d, gl,gr,gu,gd, sf2HorizontalOrigin, sf2VerticalOrigin) 
   )
	{
	perror("ERROR(dilation_arbitrary_SF): analyse_b_gray did not return a valid code");
	return MORPHO_ERROR;
	}

/* Allocate a new picture with a border */
blocWidth = imageWidth+sfWidth*2;
blocHeight = imageHeight+sfHeight*2;
bloc = (int16_t *)malloc(blocWidth*blocHeight*sizeof(int16_t));
for (i=0; i<blocWidth*blocHeight; i++) bloc[i]=SMALLEST_VAL;
for (j=0;j<imageHeight;j++)
  for (i=0;i<imageWidth;i++)  
	bloc[i+sfWidth+(j+sfHeight)*blocWidth] = imageIn[i+j*imageWidth];

/* Transforms the information contained in the front structures */
if ( MORPHO_SUCCESS != transform_b_gray(blocWidth, l,r,u,d, gl,gr,gu,gd) )
	{
	perror("ERROR(dilation_arbitrary_SF): transform_b_gray did not return a valid code");
	return MORPHO_ERROR;
	}

/* Proceed to the dilation; 
   ATTENTION: sizeof(im_inter->f...) != sizeof(im_out->f...) */
ret = dilation_volume_gray(bloc,blocWidth,blocHeight,imageOut,imageWidth,imageHeight,sf2,(int)sfWidth,(int)sfHeight, l,r,u,d, gl,gr,gu,gd, sf2HorizontalOrigin, sf2VerticalOrigin);

if ( MORPHO_SUCCESS != ret)
	{
	perror("ERROR(dilation_arbitrary_SF): dilation_volume_gray did not return a valid code");
	return MORPHO_ERROR;
	}

/* Free the data */
free(bloc); 
free(sf2);
free_front(l); free_front(r); free_front(u); free_front(d);
free(l); free(r); free(u); free(d);
free_gfront(gl); free_gfront(gr); free_gfront(gu); free_gfront(gd);
free(gl); free(gr); free(gu); free(gd);
return MORPHO_SUCCESS;
}

/*************************************************************/
/* Dilation procedure		                             */
/* ATTENTION: sizeof(in) != sizeof(out) 		     */
/*************************************************************/
int dilation_volume_gray( int16_t *bloc, int blocWidth, int blocHeight,
		int16_t *out, int imageWidth, int imageHeight,
		uint8_t *sf, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		struct gfront *gl,struct gfront *gr,struct gfront *gu,struct gfront *gd,
		int ox,int oy)
{
int16_t	max,*corner,val;
uint8_t *value,*av,*ap;
int	i,space[LARGEST_VAL+1-SMALLEST_VAL],*histo,col,*pos,line,lx,n;


/* Build the first histogram */
for (i=0;i<=LARGEST_VAL-SMALLEST_VAL;i++)  space[i]=0;
histo = space-SMALLEST_VAL;
for (i=0;i<bh*bv;i++)
   if (sf[i] != 0)	
	histo[SMALLEST_VAL+sf[i]-GREY_OFFSET]++;
max=LARGEST_VAL; 	while(histo[max] == 0) max--;

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
 	   pos=r->pos; value=r->value;
   	   for (n=0;n<r->size;n++)
		{
		val = *(corner+((*pos)+1)) + *value;
		histo[val]++;
		if (val > max) max=val;
		pos++; value++;
		}
	/* 2. Adding and removing "grey" pixels */
	   /* Adding */
	   pos=gr->pos; ap=gr->ap;	   
   	   for (n=0;n<gr->size;n++)
		{
		val = *(corner+*(pos)) + *ap;
		histo[val]++;
		if (val > max) max=val;
		pos++; ap++;
		}
 	   /* Removing */
	   pos=gr->pos; av=gr->av;
   	   for (n=0;n<gr->size;n++)
		{ histo[*(corner+*pos) + *av]--; pos++; av++; }
	/* 3. Removing "flat" pixels */
 	   pos=l->pos; value=l->value;
   	   for (n=0;n<l->size;n++)   
		{ histo[*(corner+*pos) + *value]--; pos++; value++; }
	/* Recomputes the minimum if necessary */
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
 	   pos=l->pos; value=l->value;
   	   for (n=0;n<l->size;n++)
		{
		val = *(corner+((*pos)-1)) + *value;
		histo[val]++;
		if (val > max) max=val;
		pos++; value++;
		}
	/* 2. Adding and removing "grey" pixels */
	   /* Adding */
	   pos=gl->pos; ap=gl->ap;	   
   	   for (n=0;n<gl->size;n++)
		{
		val = *(corner+(*pos)) + *ap;
		histo[val]++;
		if (val > max) max=val;
		pos++; ap++;
		}
 	   /* Removing */
	   pos=gl->pos; av=gl->av;
   	   for (n=0;n<gl->size;n++)
		{ histo[*(corner+*pos) + *av]--; pos++; av++; }
	/* 3. Removing "flat" pixels */
 	   pos=r->pos; value=r->value;
   	   for (n=0;n<r->size;n++)   
		{ histo[*(corner+*pos) + *value]--; pos++; value++; }
	/* Recomputes the minimum if necessary */
	while (histo[max] == 0) max--;
	/* Put the value in the picture */
	if ( (col-1+ox>=bh) && (col-1-bh+ox<imageWidth) && 
	        (line-bv+oy>=0) && (line-bv+oy<imageHeight) )
			out[col-1-bh+ox+(line-bv+oy)*imageWidth] = max;
	}
    }

return MORPHO_SUCCESS;
}
