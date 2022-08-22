/* LIBMORPHO
 *
 * arbitraryUtil.h
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

#include "libmorpho.h"

#ifndef __ARBITRARYUTIL__
#define __ARBITRARYUTIL__


/* Planar front */
struct	front 
	{
	int	size;
	int 	*x,*y;
	int	*pos;
	uint8_t *value;
	};

/* Grey-level front */
struct	gfront 
	{
	int	size;
	int	*x,*y;
	int	*pos;
	uint8_t	*av,*ap;
	};

/* For the erosion and the dilation */
#define	 SMALLEST_VAL		-255
#define	 LARGEST_VAL		511
#define	 SMALLEST_UINT8		0
#define	 LARGEST_UINT8		255

#define	 GREY_OFFSET		1

/* arbritraryUtil.c */
int analyse_b(uint8_t *se, int seWidth, int seHeight, struct front *lf, struct front *rf, struct front *uf, struct front *df, int ox,int oy);
int analyse_b_gray(uint8_t *se, int seWidth, int seHeight, struct front *lf, struct front *rf, struct front *uf, struct front *df, struct gfront *glf, struct gfront *grf, struct gfront *guf, struct gfront *gdf, int ox,int oy);
int transform_b(int seWidth, struct front *l, struct front *r, struct front *u, struct front *d);
int transform_b_gray(int seWidth, struct front *l, struct front *r, struct front *u, struct front *d, struct gfront *gl, struct gfront *gr, struct gfront *gu, struct gfront *gd);
int invert_SE(uint8_t *seIn, uint8_t *seOut, int width, int height);
void free_front(struct front *p);
void free_gfront(struct gfront *p);

/* erosionArbitrarySE.c */
int erosion_volume(	uint8_t *in, int blocWidth, int blocHeight,
		uint8_t *out, int imageWidth, int imageHeight,
		uint8_t *se, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		int ox,int oy);

/* dilationArbitrarySE.c */
int dilation_volume(	uint8_t *in, int blocWidth, int blocHeight,
		uint8_t *out, int imageWidth, int imageHeight,
		uint8_t *se, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		int ox,int oy);

/* erosionArbitrarySF.c */
int erosion_volume_gray( int16_t *bloc, int blocWidth, int blocHeight,
		int16_t *out, int imageWidth, int imageHeight,
		uint8_t *sf, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		struct gfront *gl,struct gfront *gr,struct gfront *gu,struct gfront *gd,
		int ox,int oy);

/* dilationArbitrarySF.c */
int dilation_volume_gray( int16_t *bloc, int blocWidth, int blocHeight,
		int16_t *out, int imageWidth, int imageHeight,
		uint8_t *sf, int bh, int bv,
		struct front *l, struct front *r, struct front *u, struct front *d,
		struct gfront *gl,struct gfront *gr,struct gfront *gu,struct gfront *gd,
		int ox,int oy);

#endif
