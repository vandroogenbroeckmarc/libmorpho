/* LIBMORPHO
 *
 * morpho.c
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

#include "../src/libmorpho.h"

#define VERBOSE 1

#ifndef FALSE
#define FALSE (0)
#define TRUE (1)
#endif

#define MAGIC_PGM	"P5\n"
#define MAGIC_PPM	"P6\n"

#define O_BINARY 0
#define IO_LEN	(1<<30)	

/*-----------------------------------------------------------------------------------*/
void usage(int argc, char *argv[])
{
  printf("\n"
	 "usage: %s <image_in> <image_out> <operation> <code> <size>\n\n"
	 " <image_in> and <image_out> should be pgm images\n"
	 " <operation> is the operation to perform:\n"
	 "   1 = erosion (default)\n"
	 "   2 = dilation\n"
	 "   3 = opening\n"
	 "   4 = closing\n"
	 " <code> tells if we should use the 1D or 2D version:\n"
	 "   1 = 1D (default)\n"
	 "   2 = 2D\n"
	 " <size> is the size of the structuring element (default is 4)\n\n", argv[0]);
}

/*-----------------------------------------------------------------------------------*/
int parseCmdLine(int argc, char *argv[], char *fileIn, char *fileOut, int *op, int *code, int *size)
{
  if(argc<3) {
    printf("\nWrong arguments \n(minimum = %s image_in image_out)\n",argv[0]);
    return -1;
  } else if (argc>6) {
      printf("\nWrong arguments \n(maximum = %s image_in image_out <operation> <code> <size>\n",argv[0]);  
    return -1;
  } else {
    strcpy(fileIn, argv[1]);
    strcpy(fileOut, argv[2]);
    if(argc==4 || argc==5 || argc==6) {
      printf("\n argv[3] = %s",argv[3]);
      *op=atoi(argv[3]); 
      if(*op < 1 || *op > 4) {
	printf("\nWrong arguments \n(<operation> = %d)\n",*op);
	return -1;
      }
      printf("\n argv[4] = %s",argv[4]);
      *code=atoi(argv[4]);
      printf("\n 1 %d",*code != 1);
      printf("\n 2 %d",*code != 2);
      if(*code != 1 &&  *code != 2) {
	printf("\nWrong arguments \n(<code> = %d)\n",*code);
	return -1;
      }

      printf("\n argv[5] = %s\n",argv[5]);
      *size=atoi(argv[5]);
     if(*size < 1) {
	printf("\nWrong arguments \n(<size> = %d)\n",*size);
	return -1;
      }
    }
    return 0;
  }
}

/*-----------------------------------------------------------------------------------*/
static int open_read(char *filename)
{
  int fd;

  if( (fd = open(filename, O_BINARY|O_RDONLY)) < 0 )
    fprintf(stderr, " ERROR : can't reset file `%s': %s\n", filename, strerror(errno));
  return fd;
}

/*-----------------------------------------------------------------------------------*/
static int match_key(int fd, char *key)
{
  char buf[80];

  read(fd, buf, strlen(key));
  if( strncmp(buf, key, strlen(key)) != 0 )
    return FALSE;
  else
    return TRUE;
}

/*-----------------------------------------------------------------------------------*/
static void skip_comment(int fd, char code, char *c)
{
  while( *c == code ) {
    while( (read(fd, c, 1) == 1 ) && (*c != '\n') ) ;
    read(fd, c, 1);
  }
}

/*-----------------------------------------------------------------------------------*/
static void read_header_line(int fd, char *buf)
{
  int i;

  i = 1;
  while( (read(fd, &buf[i], 1) == 1 ) && (buf[i] != '\n') && (buf[i] != '\r') && (i<79) )
    i++;
  buf[i] = 0;
}

/*-----------------------------------------------------------------------------------*/
static int get_pgm_header(int fd, char *magic, int *width, int *height)
{
  char buf[80];

  if( !match_key(fd, magic) )
    return FALSE;
  read(fd, buf, 1);
  skip_comment(fd, '#', buf);
  read_header_line(fd, buf);
  sscanf(buf, "%d %d", width, height);
  read(fd, buf, 1);
  skip_comment(fd, '#', buf);
  read_header_line(fd, buf);
  return TRUE;
}

/*-----------------------------------------------------------------------------------*/
static int open_read_pixmap(char *filename, char *magic, int *width, int *height)
{
  int fd;

  if( (fd = open_read(filename)) < 0)
    return fd;
  if( !get_pgm_header(fd, magic, width, height) ) {
    fprintf(stderr, " ERROR : can't read header of %s\n", filename);
    return -1;
  }
  return fd;
}


/*!
  \fn writePPM(uint8_t *image, int x, int y, int increment, int offset, char* path)
  \param  *image Input buffer
  \param  x Width of the image buffer
  \param  y Height of the image buffer
  \param  increment useful for loop image writing 
  \param  offset useful for loop image writing
  \param  path prefix name of the image (fullname = path.offset+increment)
  \brief Write PPM images
 
  This function allows to write RGB PPM images with the magic number P6. To know more
  about the portable pixmap file format, you can take a look,at 'man PPM'.
  
  The size of the input buffer should be at least x*y*3
  
  
  \ingroup    	morphoLibAnchor 
  \author      Renaud Dardenne
 */
/*-----------------------------------------------------------------------------------*/
int  writePPM(uint8_t *image, int x, int y, int increment, int offset, char* path)
{
  FILE * outfile=NULL;    
  char filename[31]="";
  /* Checks */
  if (image == NULL) {
    fprintf(stderr, "\n ERROR : empty image buffer");
    return -1;
  }
  if (strlen(path)==0) {
    fprintf(stderr, "\n ERROR : wrong filename");
    return -1;
  }
  snprintf(filename, 30, "%s-%1d.ppm",path,offset+increment);
  if (VERBOSE)
    printf("Writing %s\n", filename);
  if ((outfile = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "\n ERROR : can't open %s", filename);
    return -1;
  }
  /* Writing  magic number, image size and dynamic  */  
  fprintf(outfile, "P6\n%d %d\n255\n", x, y);  /* P6 ou P3 */
  /* Wrinting image */
  fwrite(image, sizeof(uint8_t), x*y*3,  outfile);
  fclose(outfile);
  return 0;
}


/**
  \fn int  readPPM(uint8_t *image, char* filename, int *width, int *height)
  \param *image ouput buffer
  \param *filename file name of the image 
  \param  width Width of the image buffer
  \param  height Height of the image buffer
  \brief read PPM images
 
  This function allows to read RGB PPM images with the magic number P6. To know more
  about the portable pixmap file format, you can take a look,at 'man PPM'.
  
  The output buffer should be allocated
  width and height are varible addresses to allow the calling function to get back the size of the image
  
  
  \ingroup    	morphoLibAnchor 
  \author      Renaud Dardenne
 */
/*-----------------------------------------------------------------------------------*/
int  readPPM(uint8_t *image, char* filename, int *width, int *height)
{
  int fd;
  int x=0,y=0;
  /* Checks */
  if(image==NULL) {
    fprintf(stderr, "\n ERROR : Output buffer not allocated");
    return -1;
  }
  if (strlen(filename)==0) {
    fprintf(stderr, "\n ERROR : wrong filename");
    return -1;
  }
  /* Reading magic number, image size */
  if( (fd = open_read_pixmap(filename, MAGIC_PPM, width, height)) < 0)
    return -1;
  if (VERBOSE)
    printf("Reading PPM image %s: [%d]x[%d]\n",filename, *width, *height);
  /* Reading image */
  x=*width;
  y=*height;
  read(fd, image, x*y*3*sizeof(uint8_t));  /* ??? */   
  close(fd);
  return 0;
}

/*!
  \fn writePGM(uint8_t *image, int x, int y, int increment, int offset, char* path)
  \param *image Input buffer
  \param x Width of the image buffer
  \param y Height of the image buffer
  \param increment useful for loop image writing 
  \param offset useful for loop image writing
  \param *path prefix name of the image (fullname = path.offset+increment)
  \brief Write PGM images
 
  This function allows to write grayscale PGM images with the magic number P5. To know more
  about the portable graymap file format, you can take a look,at 'man PGM'.
  
  The size of the input buffer should be at least x*y
  
  \ingroup    	morphoLibAnchor 
  \author      Renaud Dardenne
 */
/*-----------------------------------------------------------------------------------*/
int  writePGM(uint8_t *image, int x, int y, char* path)
{
  FILE * outfile=NULL;    
  char filename[31]="";
  /* Checks */
  if (image == NULL) {
    fprintf(stderr, "\n ERROR : empty image buffer");
    return -1;
  }
  if (strlen(path)==0) {
    fprintf(stderr, "\n ERROR : wrong filename");
    return -1;
  }
  snprintf(filename, 30, "%s",path);
  if (VERBOSE)
    printf("Writing %s\n", filename);
  if ((outfile = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "\n ERROR : can't open %s", filename);
    return -1;
  }
  /* Writing  magic number, image size and dynamic  */  
  fprintf(outfile, "P5\n%d %d\n255\n", x, y);  /* P5 */
  /* Wrinting image */
  fwrite(image, sizeof(uint8_t), x*y,  outfile);
  fclose(outfile);
  return 0;
  return 0;
}

/*!
  \fn int  readPGM(uint8_t *image, char* filename, int *width, int *height)
  \param *image ouput buffer
  \param *filename file name of the image 
  \param width Width of the image buffer
  \param height Height of the image buffer
  \brief read PGM images
 
  This function allows to read RGB PPM images with the magic number P6. To know more
  about the portable graymap file format, you can take a look,at 'man PPM'.
  
  The output buffer should be allocated
  width and height are varible addresses to allow the calling function to get back the size of the image
  
  \ingroup    	morphoLibAnchor 
  \author      Renaud Dardenne
 */
/*-----------------------------------------------------------------------------------*/
int  readPGM(uint8_t *image, char* filename, int *width, int *height)
{
  int fd;
  int x=0,y=0;
  /* Checks */
  if(image==NULL) {
    fprintf(stderr, "\n ERROR : Output buffer not allocated");
    return -1;
  }
  if (strlen(filename)==0) {
    fprintf(stderr, "\n ERROR : wrong filename");
    return -1;
  }
  /* Reading magic number, image size */
  if( (fd = open_read_pixmap(filename, MAGIC_PGM, width, height)) < 0)
    return -1;
  if (VERBOSE)
    printf("Reading PGM image %s: [%d]x[%d]\n",filename, *width, *height);
  /* Reading image */
  x=*width;
  y=*height;
  read(fd, image, x*y*sizeof(uint8_t));  /* ??? */   
  close(fd);
  return 0;
}

/*-----------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  uint8_t *imageIn=NULL, *imageOut=NULL;
  int x, y;
  int fd;
  char filenameIn[50]="";
  char filenameOut[50]="";
  int op=1, code=1, size=4;
  /* Show the usage */ 
  usage(argc, argv);
  
  /* Parsing the command line*/ 
  if(parseCmdLine(argc, argv, filenameIn, filenameOut, &op, &code, &size)==-1)
    return -1;
    
  /* Reading magic number, image size */
  if( (fd = open_read_pixmap(filenameIn, MAGIC_PGM, &x, &y)) < 0)
    return -1;
  close(fd);
  
  /* Allocating images*/ 
  if((imageIn  = (uint8_t*)malloc(x*y*sizeof(uint8_t))) == NULL)
    perror("Malloc ImageIn");
   
  if((imageOut = (uint8_t*)malloc(x*y*sizeof(uint8_t))) == NULL)
    perror("Malloc ImageOut");
  
  /* Reading input image*/ 
  readPGM(imageIn,filenameIn, &x, &y);

  printf("\n OP = %d\n CO = %d\n SI = %d\n",op, code , size);

  /* Operation*/ 
  switch(op) {
  case 1:
    printf("Running erosionByAnchor");
    if(code==1) {
      printf("_1D_horizontal ... (8 bits/grayscale only!)");    
      erosionByAnchor_1D_horizontal(imageIn, imageOut, x, y, size);
      printf(" finished.\n");
    } else if(code==2) {
      printf("_2D ... (8 bits/grayscale only!)");    
      erosionByAnchor_2D(imageIn, imageOut, x, y, size, size);      
      printf(" finished.\n");
    } else {
      printf("Unknown code %d\n",code);
    }
    break;
  case 2:
    printf("Running dilationByAnchor");
    if(code==1) {
      printf("_1D_horizontal ... (8 bits/grayscale only!)");    
      dilationByAnchor_1D_horizontal(imageIn, imageOut, x, y, size);
      printf(" finished.\n");
    } else if(code==2) {
      printf("_2D ... (8 bits/grayscale only!)");    
      dilationByAnchor_2D(imageIn, imageOut, x, y, size, size);      
      printf(" finished.\n");
    } else {
      printf("Unknown code %d\n",code);
    }
    break;
  case 3:
    printf("Running openingByAnchor");
    if(code==1) {
      printf("_1D_horizontal ... (8 bits/grayscale only!)");    
      openingByAnchor_1D_horizontal(imageIn, imageOut, x, y, size);
      printf(" finished.\n");
    } else if(code==2) {
      printf("_2D ... (8 bits/grayscale only!)");    
      openingByAnchor_2D(imageIn, imageOut, x, y, size, size);      
      printf(" finished.\n");
    } else {
      printf("Unknown code %d\n",code);
    }
    break;
  case 4:
    printf("Running closingByAnchor");
    if(code==1) {
      printf("_1D_horizontal ... (8 bits/grayscale only!)");    
      closingByAnchor_1D_horizontal(imageIn, imageOut, x, y, size);
      printf(" finished.\n");
    } else if(code==2) {
      printf("_2D ... (8 bits/grayscale only!)");    
      closingByAnchor_2D(imageIn, imageOut, x, y, size, size);      
      printf(" finished.\n");
    } else {
      printf("Unknown code %d\n",code);
    }
    break;
  default:
    printf("Unknown operation %d\n",op);
  }

  /* Wrinting output image*/ 
  writePGM(imageOut, x, y, filenameOut);
  
  /* Free Images*/ 
  free(imageIn);
  free(imageOut);
  return 0;
}
