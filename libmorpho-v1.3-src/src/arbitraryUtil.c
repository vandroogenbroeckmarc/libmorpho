/* LIBMORPHO
 *
 * arbitraryUtil.c
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

#include "arbitraryUtil.h"

/*******************************************************************/
/* Analyse the fronts of the structuring element 
   and compute the origin 	*/
int analyse_b( 	uint8_t *se, int seWidth, int seHeight, 
		struct front *lf, struct front *rf, struct front *uf, struct front *df,
		int ox, int oy)
{
int	i,j,n;
int	*x,*y;
uint8_t *b;

b = se;

/* Check if the origin is correct */
if (DEBUG) printf("Origin is (%d,%d)\n",ox,oy); 

if ( (ox<0) || (oy<0) || (ox>seWidth) || (oy>seHeight) )
	{
	perror("ERROR(analyse_b): the origin of the structuring element must be included in the structuring element.");
	return MORPHO_ERROR;
	}
if ( se[ox+oy*seWidth]==0) 
	{
	perror("ERROR(analyse_b): for this function you need an origin of the structuring element that is not null.");
	return MORPHO_ERROR;
	}

/* Horizontal front to the left */ 
n = 0; 
   	/* Counts the number of valid points */
	/* First column */
	for (j=0;j<seHeight;j++)
		if (b[j*seWidth] != 0)  n++;
	/* Other columns */
	for (i=1;i<seWidth;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] == 0)) 	n++;
	/* Parameter entry and memory allocation */
	lf->size = n;  
	lf->pos = (int *)NULL;  
	lf->x = (int *)malloc(n*sizeof(int));		x=lf->x;
	lf->y = (int *)malloc(n*sizeof(int));		y=lf->y;
	lf->value = NULL;
	/* Coordinate entry */
	n = 0;
	   /* First column */
	   for (j=0;j<seHeight;j++)
		if (b[j*seWidth] != 0)  
			{
			x[n]=(int)0; y[n]=(int)j; 
			n++;
			}
	   /* Other column */
	   for (i=1;i<seWidth;i++)
	      for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] == 0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			n++;
			}

/* Horizontal front to the right */ 
n = 0; 
   	/* Counts the number of valid points */
	/* Last column */
	for (j=0;j<seHeight;j++)
		if (b[seWidth-1+j*seWidth] != 0)  n++;
	/* Other columns */
	for (i=0;i<seWidth-1;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] == 0)) 	n++;
	/* Parameter entry and memory allocation */
	rf->size = n;  
	rf->pos = (int *)NULL;  
	rf->x = (int *)malloc(n*sizeof(int));		x=rf->x;
	rf->y = (int *)malloc(n*sizeof(int));		y=rf->y;
	rf->value = NULL;
	/* Coordinate entry */
	n = 0;
	   /* Last column */
	   for (j=0;j<seHeight;j++)
		if (b[seWidth-1+j*seWidth] != 0)  
			{
			x[n]=(int)seWidth-1; y[n]=(int)j; 
			n++;
			}
	   /* Other columns */
	   for (i=0;i<seWidth-1;i++)
	      for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] == 0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			n++;
			}

/* Upwards vertical front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* First raw */
	for (i=0;i<seWidth;i++)
		if (b[i] != 0)  n++;
	/* Other raws */
	for (j=1;j<seHeight;j++)
	   for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] ==0)) 	n++;
	/* Parameter entry and memory allocation */
	uf->size = n;  
	uf->pos = (int *)NULL;  
	uf->x = (int *)malloc(n*sizeof(int));		x=uf->x;
	uf->y = (int *)malloc(n*sizeof(int));		y=uf->y;
	uf->value = NULL;
	/* Coordinate entry */
	n = 0;
	   /* First raw */
	   for (i=0;i<seWidth;i++)
		if (b[i] != 0)  
			{
			x[n]=(int)i; y[n]=(int)0; 
			n++;
			}
	   /* Other raws */
	   for (j=1;j<seHeight;j++)
	     for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] == 0))	
			{
			x[n]=(int)i; y[n]=(int)j; 
			n++;
			}

/* Downwards vertical front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* Last raw */
	for (i=0;i<seWidth;i++)
		if (b[i+(seHeight-1)*seWidth] != 0)  n++;
	/* Other raws */
	for (j=0;j<seHeight-1;j++)
	   for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] ==0)) 	n++;
	/* Parameter entry and memory allocation */
	df->size = n;  
	df->pos = (int *)NULL;  
	df->x = (int *)malloc(n*sizeof(int));		x=df->x;
	df->y = (int *)malloc(n*sizeof(int));		y=df->y;
	df->value = NULL;
	/* Coordinate entry */
	n = 0;
	   /* Last raw */
	   for (i=0;i<seWidth;i++)
		if (b[i+(seHeight-1)*seWidth] != 0)  
			{
			x[n]=(int)i; y[n]=(int)seHeight-1; 
			n++;
			}
	   /* Other raws */
	   for (j=0;j<seHeight-1;j++)
	      for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] ==0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			n++;
			}

return MORPHO_SUCCESS;
}

/*******************************************************************/
/* Analyse the fronts of the structuring element 
   and compute the origin 	*/
int analyse_b_gray( 	uint8_t *se, int seWidth, int seHeight, 
		struct front *lf, struct front *rf, struct front *uf, struct front *df,
		struct gfront *glf, struct gfront *grf, struct gfront *guf, struct gfront *gdf,
		int ox, int oy)
{
int	i,j,n;
int	*x,*y;
uint8_t *value,*b,*av,*ap;

b = se;

/* Check if the origin is correct */
if (DEBUG) printf("Origin is (%d,%d)\n",ox,oy); 

if ( (ox<0) || (oy<0) || (ox>seWidth) || (oy>seHeight) )
	{
	perror("ERROR(analyse_b_gray): the origin of the structuring element must be included in the structuring element.");
	return MORPHO_ERROR;
	}
if ( se[ox+oy*seWidth]==0) 
	{
	perror("ERROR(analyse_b_gray): for this function you need an origin of the structuring element that is not null.");
	return MORPHO_ERROR;
	}

/* Horizontal front to the left */ 
n = 0; 
   	/* Counts the number of valid points */
	/* First column */
	for (j=0;j<seHeight;j++)
		if (b[j*seWidth] != 0)  n++;
	/* Other columns */
	for (i=1;i<seWidth;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] == 0)) 	n++;
	/* Parameter entry and memory allocation */
	lf->size = n;  
	lf->pos = (int *)NULL;  
	lf->x = (int *)malloc(n*sizeof(int));		x=lf->x;
	lf->y = (int *)malloc(n*sizeof(int));		y=lf->y;
	lf->value = (uint8_t *)malloc(n*sizeof(uint8_t));	value=lf->value;
	/* Coordinate entry */
	n = 0;
	   /* First column */
	   for (j=0;j<seHeight;j++)
		if (b[j*seWidth] != 0)  
			{
			x[n]=(int)0; y[n]=(int)j; 
			value[n]=(uint8_t)b[j*seWidth]-GREY_OFFSET; n++;
			}
	   /* Other column */
	   for (i=1;i<seWidth;i++)
	      for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] == 0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			value[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; n++;
			}

/* Horizontal front to the right */ 
n = 0; 
   	/* Counts the number of valid points */
	/* Last column */
	for (j=0;j<seHeight;j++)
		if (b[seWidth-1+j*seWidth] != 0)  n++;
	/* Other columns */
	for (i=0;i<seWidth-1;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] == 0)) 	n++;
	/* Parameter entry and memory allocation */
	rf->size = n;  
	rf->pos = (int *)NULL;  
	rf->x = (int *)malloc(n*sizeof(int));		x=rf->x;
	rf->y = (int *)malloc(n*sizeof(int));		y=rf->y;
	rf->value = (uint8_t *)malloc(n*sizeof(uint8_t));	value=rf->value;
	/* Coordinate entry */
	n = 0;
	   /* Last column */
	   for (j=0;j<seHeight;j++)
		if (b[seWidth-1+j*seWidth] != 0)  
			{
			x[n]=(int)seWidth-1; y[n]=(int)j; 
			value[n]=(uint8_t)b[seWidth-1+j*seWidth]-GREY_OFFSET; 
			n++;
			}
	   /* Other columns */
	   for (i=0;i<seWidth-1;i++)
	      for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] == 0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			value[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; n++;
			}

/* Upwards vertical front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* First raw */
	for (i=0;i<seWidth;i++)
		if (b[i] != 0)  n++;
	/* Other raws */
	for (j=1;j<seHeight;j++)
	   for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] ==0)) 	n++;
	/* Parameter entry and memory allocation */
	uf->size = n;  
	uf->pos = (int *)NULL;  
	uf->x = (int *)malloc(n*sizeof(int));		x=uf->x;
	uf->y = (int *)malloc(n*sizeof(int));		y=uf->y;
	uf->value = (uint8_t *)malloc(n*sizeof(uint8_t));	value=uf->value;
	/* Coordinate entry */
	n = 0;
	   /* First raw */
	   for (i=0;i<seWidth;i++)
		if (b[i] != 0)  
			{
			x[n]=(int)i; y[n]=(int)0; 
			value[n]=(uint8_t)b[i]-GREY_OFFSET; n++;
			}
	   /* Other raws */
	   for (j=1;j<seHeight;j++)
	     for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] == 0))	
			{
			x[n]=(int)i; y[n]=(int)j; 
			value[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; n++;
			}

/* Downwards vertical front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* Last raw */
	for (i=0;i<seWidth;i++)
		if (b[i+(seHeight-1)*seWidth] != 0)  n++;
	/* Other raws */
	for (j=0;j<seHeight-1;j++)
	   for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] ==0)) 	n++;
	/* Parameter entry and memory allocation */
	df->size = n;  
	df->pos = (int *)NULL;  
	df->x = (int *)malloc(n*sizeof(int));		x=df->x;
	df->y = (int *)malloc(n*sizeof(int));		y=df->y;
	df->value = (uint8_t *)malloc(n*sizeof(uint8_t));	value=df->value;
	/* Coordinate entry */
	n = 0;
	   /* Last raw */
	   for (i=0;i<seWidth;i++)
		if (b[i+(seHeight-1)*seWidth] != 0)  
			{
			x[n]=(int)i; y[n]=(int)seHeight-1; 
			value[n]=(uint8_t)b[i+(seHeight-1)*seWidth]-GREY_OFFSET; 
			n++;
			}
	   /* Other raws */
	   for (j=0;j<seHeight-1;j++)
	      for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] ==0)) 	
			{
			x[n]=(int)i; y[n]=(int)j; 
			value[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; n++;
			}

/* Grey-level analysis */
/* Horizontal front to the left */ 
n = 0; 
   	/* Counts the number of valid points */
	/* All columns except for the last one */
	for (i=0;i<seWidth-1;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+1+j*seWidth])) n++;
	/* Parameter entry and memory allocation */
	glf->size = n;  
	glf->pos = (int *)NULL;  
	glf->x = (int *)malloc(n*sizeof(int));	x=glf->x;
	glf->y = (int *)malloc(n*sizeof(int));	y=glf->y;
	glf->av = (uint8_t *)malloc(n*sizeof(uint8_t));	av=glf->av;
	glf->ap = (uint8_t *)malloc(n*sizeof(uint8_t));	ap=glf->ap;
	/* Coordinate entry */
	n = 0;
	/* All columns without the first one */
	for (i=0;i<seWidth-1;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i+1+j*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+1+j*seWidth]))
			{
			x[n]=(int)i; y[n]=(int)j; 
			av[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; 
			ap[n]=(uint8_t)b[i+1+j*seWidth]-GREY_OFFSET; 
			n++;
			}

/* Horizontal front to the right */ 
n = 0; 
   	/* Counts the number of valid points */
	/* All columns without the first one */
	for (i=1;i<seWidth;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i-1+j*seWidth])) n++;
	/* Parameter entry and memory allocation */
	grf->size = n;  
	grf->pos = (int *)NULL;  
	grf->x = (int *)malloc(n*sizeof(int));	x=grf->x;
	grf->y = (int *)malloc(n*sizeof(int));	y=grf->y;
	grf->av = (uint8_t *)malloc(n*sizeof(uint8_t));	av=grf->av;
	grf->ap = (uint8_t *)malloc(n*sizeof(uint8_t));	ap=grf->ap;
	/* Coordinate entry */
	n = 0;
	/* All columns without the first one */
	for (i=1;i<seWidth;i++)
	    for (j=0;j<seHeight;j++)
		if ((b[i+j*seWidth] != 0) && (b[i-1+j*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i-1+j*seWidth]))
			{
			x[n]=(int)i; y[n]=(int)j; 
			av[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; 
			ap[n]=(uint8_t)b[i-1+j*seWidth]-GREY_OFFSET; 
			n++;
			}

/* Upwards front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* All raws except for the last one */
	for (j=0;j<seHeight-1;j++)
	    for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+(j+1)*seWidth])) n++;
	/* Parameter entry and memory allocation */
	guf->size = n;  
	guf->pos = (int *)NULL;  
	guf->x = (int *)malloc(n*sizeof(int));	x=guf->x;
	guf->y = (int *)malloc(n*sizeof(int));	y=guf->y;
	guf->av = (uint8_t *)malloc(n*sizeof(uint8_t));	av=guf->av;
	guf->ap = (uint8_t *)malloc(n*sizeof(uint8_t));	ap=guf->ap;
	/* Coordinate entry */
	n = 0;
	/* All raws without the last one */
	for (j=0;j<seHeight-1;j++)
	    for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j+1)*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+(j+1)*seWidth]))
			{
			x[n]=(int)i; y[n]=(int)j; 
			av[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; 
			ap[n]=(uint8_t)b[i+(j+1)*seWidth]-GREY_OFFSET; 
			n++;
			}

/* Downwards front */ 
n = 0; 
   	/* Counts the number of valid points */
	/* All raws but the first one */
	for (j=1;j<seHeight;j++)
	    for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+(j-1)*seWidth])) n++;
	/* Parameter entry and memory allocation */
	gdf->size = n;  
	gdf->pos = (int *)NULL;  
	gdf->x = (int *)malloc(n*sizeof(int));	x=gdf->x;
	gdf->y = (int *)malloc(n*sizeof(int));	y=gdf->y;
	gdf->av = (uint8_t *)malloc(n*sizeof(uint8_t));	av=gdf->av;
	gdf->ap = (uint8_t *)malloc(n*sizeof(uint8_t));	ap=gdf->ap;
	/* Coordinate entry */
	n = 0;
	/* All raws without the first one */
	for (j=1;j<seHeight;j++)
	    for (i=0;i<seWidth;i++)
		if ((b[i+j*seWidth] != 0) && (b[i+(j-1)*seWidth] != 0) 
			&& (b[i+j*seWidth] != b[i+(j-1)*seWidth]))
			{
			x[n]=(int)i; y[n]=(int)j; 
			av[n]=(uint8_t)b[i+j*seWidth]-GREY_OFFSET; 
			ap[n]=(uint8_t)b[i+(j-1)*seWidth]-GREY_OFFSET; 
			n++;
			}

return MORPHO_SUCCESS;
}

/****************************************************************/
/* Modify the front structures to speed up the access operations 
   and look for an origin */ 
int transform_b(int seWidth, struct front *l, struct front *r, struct front *u, struct front *d)
{
int 	i,size,*pos;
int	*x,*y;

/* Left front */
size=(int)l->size; x=(int *)l->x; y=(int *)l->y;
l->pos=(int *)malloc(size*sizeof(int)); pos=(int *)l->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Right front */
size=(int)r->size; x=(int *)r->x; y=(int *)r->y;
r->pos=(int *)malloc(size*sizeof(int)); pos=(int *)r->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Upper front */
size=(int)u->size; x=(int *)u->x; y=(int *)u->y;
u->pos=(int *)malloc(size*sizeof(int)); pos=(int *)u->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Down front */
size=(int)d->size; x=(int *)d->x; y=(int *)d->y;
d->pos=(int *)malloc(size*sizeof(int)); pos=(int *)d->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

return MORPHO_SUCCESS;
}

/****************************************************************/
/* Modify the front structures to speed up the access operations 
   and look for an origin */ 
int transform_b_gray(int seWidth,
		struct front *l, struct front *r, struct front *u, struct front *d,
		struct gfront *gl, struct gfront *gr, struct gfront *gu, struct gfront *gd)
{
int 	i,size,*pos;
int	*x,*y;

/* Left front */
size=(int)l->size; x=(int *)l->x; y=(int *)l->y;
l->pos=(int *)malloc(size*sizeof(int)); pos=(int *)l->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Right front */
size=(int)r->size; x=(int *)r->x; y=(int *)r->y;
r->pos=(int *)malloc(size*sizeof(int)); pos=(int *)r->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Upper front */
size=(int)u->size; x=(int *)u->x; y=(int *)u->y;
u->pos=(int *)malloc(size*sizeof(int)); pos=(int *)u->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Down front */
size=(int)d->size; x=(int *)d->x; y=(int *)d->y;
d->pos=(int *)malloc(size*sizeof(int)); pos=(int *)d->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Grey-level left front */
size=(int)gl->size; x=(int *)gl->x; y=(int *)gl->y;
gl->pos=(int *)malloc(size*sizeof(int)); pos=(int *)gl->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Grey-level right front */
size=(int)gr->size; x=(int *)gr->x; y=(int *)gr->y;
gr->pos=(int *)malloc(size*sizeof(int)); pos=(int *)gr->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Grey-level upper front */
size=(int)gu->size; x=(int *)gu->x; y=(int *)gu->y;
gu->pos=(int *)malloc(size*sizeof(int)); pos=(int *)gu->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

/* Grey-level down front */
size=(int)gd->size; x=(int *)gd->x; y=(int *)gd->y;
gd->pos=(int *)malloc(size*sizeof(int)); pos=(int *)gd->pos;
for (i=size-1; i>=0; i--)
	pos[i] = (int)(x[i]+y[i]*seWidth);

return MORPHO_SUCCESS;
}

/****************************************************************/
int invert_SE(uint8_t *seIn, uint8_t *seOut, int width, int height)
{
int 	i,j;

for (j=0; j<height; j++)
  for (i=0; i<width; i++)
	  seOut[i+j*width] = seIn[width-i-1+(height-j-1)*width];

return MORPHO_SUCCESS;
}

/****************************************************************/
void free_front(struct front *p)
{
if (p != NULL) 
	{ 
	if (p->x != NULL) free(p->x); 
	if (p->y != NULL) free(p->y); 
	if (p->pos != NULL) free(p->pos); 
	if (p->value != NULL) free(p->value); 
	}
}

void free_gfront(struct gfront *p)
{
if (p != NULL) 
	{ 
	if (p->x != NULL) free(p->x); 
	if (p->y != NULL) free(p->y); 
	if (p->pos != NULL) free(p->pos); 
	if (p->av != NULL) free(p->av); 
	if (p->ap != NULL) free(p->ap); 
	}
}

