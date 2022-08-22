/* LIBMORPHO
 *
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
  snprintf(filename, 30, "%s.ppm",path);
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
int  writePGM(uint8_t *image, int x, int y, int increment, int offset, char* path)
{
  FILE * outfile=NULL;    
  char filename[81]="";
  /* Checks */
  if (image == NULL) {
    fprintf(stderr, "\n ERROR : empty image buffer");
    return -1;
  }
  if (strlen(path)==0) {
    fprintf(stderr, "\n ERROR : wrong filename");
    return -1;
  }
  snprintf(filename, 80, "%s.pgm",path);
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
  uint8_t *imageIn=NULL, *imageOut=NULL, *se=NULL;
  int16_t *imageIn2=NULL, *imageOut2=NULL;
  int x, y;
  int sizeX=20, sizeY=20; 
  int posX, posY;
  int i;
  
  if((imageIn=(uint8_t*)malloc(1024*768*sizeof(uint8_t))) == NULL)
    perror("Malloc");

  readPGM(imageIn,"img/pinguin.pgm", &x, &y);

  if((imageIn2=(int16_t*)malloc(x*y*sizeof(int16_t))) == NULL) perror("Malloc");
  if((imageOut=(uint8_t*)malloc(x*y*sizeof(uint8_t))) == NULL) perror("Malloc");
  if((imageOut2=(int16_t*)malloc(x*y*sizeof(int16_t))) == NULL) perror("Malloc");
  
  if((se=(uint8_t*)malloc(50*50*sizeof(uint8_t))) == NULL) perror("Malloc");

  /* 1D Operation by Anchor */
  sizeX = 25;
  erosionByAnchor_1D_horizontal(imageIn, imageOut, x, y, sizeX); 
  writePGM(imageOut, x, y, 0, 0, "erosionByAnchor_1D_horizontal");
  dilationByAnchor_1D_horizontal(imageIn, imageOut, x, y, sizeX); 
  writePGM(imageOut, x, y, 0, 0, "dilationByAnchor_1D_horizontal");
  openingByAnchor_1D_horizontal(imageIn, imageOut, x, y, sizeX); 
  writePGM(imageOut, x, y, 0, 0, "openingByAnchor_1D_horizontal");
  closingByAnchor_1D_horizontal(imageIn, imageOut, x, y, sizeX); 
  writePGM(imageOut, x, y, 0, 0, "closingByAnchor_1D_horizontal");

  /* 1D Operation by Anchor */
  sizeY = 25;
  erosionByAnchor_1D_vertical(imageIn, imageOut, x, y, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "erosionByAnchor_1D_vertical");
  dilationByAnchor_1D_vertical(imageIn, imageOut, x, y, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "dilationByAnchor_1D_vertical");
  openingByAnchor_1D_vertical(imageIn, imageOut, x, y, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "openingByAnchor_1D_vertical");
  closingByAnchor_1D_vertical(imageIn, imageOut, x, y, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "closingByAnchor_1D_vertical");

  /* 2D Operation by Anchor */
  sizeX = 25;
  sizeY = 25;
  erosionByAnchor_2D(imageIn, imageOut, x, y, sizeX, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "erosionByAnchor_2D");
  dilationByAnchor_2D(imageIn, imageOut, x, y, sizeX, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "dilationByAnchor_2D");
  openingByAnchor_2D(imageIn, imageOut, x, y, sizeX, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "openingByAnchor_2D");
  closingByAnchor_2D(imageIn, imageOut, x, y, sizeX, sizeY); 
  writePGM(imageOut, x, y, 0, 0, "closingByAnchor_2D");

  /* Arbitrary shaped flat structuring element */
  readPGM(se,"img/U.pgm", &sizeX, &sizeY);
  for (i=0; i<sizeX*sizeY; i++) if (0!=se[i]) se[i]=1;
  posX = 13;
  posY = 12;
  erosion_arbitrary_SE(imageIn, imageOut, x, y, se, sizeX, sizeY, posX, posY);
  writePGM(imageOut, x, y, 0, 0, "erosion_arbitrary_SE");
  dilation_arbitrary_SE(imageIn, imageOut, x, y, se, sizeX, sizeY, posX, posY);
  writePGM(imageOut, x, y, 0, 0, "dilation_arbitrary_SE");
  opening_arbitrary_SE(imageIn, imageOut, x, y, se, sizeX, sizeY, posX, posY);
  writePGM(imageOut, x, y, 0, 0, "opening_arbitrary_SE");
  closing_arbitrary_SE(imageIn, imageOut, x, y, se, sizeX, sizeY, posX, posY);
  writePGM(imageOut, x, y, 0, 0, "closing_arbitrary_SE");
  
  
  /* Arbitrary shaped flat structuring function */
  readPGM(se,"img/ball.pgm", &sizeX, &sizeY);
  posX = sizeX/2;
  posY = sizeY/2;
  for (i=0; i<x*y; i++) imageIn2[i]=imageIn[i];

  erosion_arbitrary_SF(imageIn2, imageOut2, x, y, se, sizeX, sizeY, posX, posY);
  for (i=0; i<x*y; i++) { if (imageOut2[i]<0) imageOut2[i]=0; if (imageOut2[i]>255) imageOut2[i]=255; imageOut[i]=imageOut2[i]; }
  writePGM(imageOut, x, y, 0, 0, "erosion_arbitrary_SF");
  dilation_arbitrary_SF(imageIn2, imageOut2, x, y, se, sizeX, sizeY, posX, posY);
  for (i=0; i<x*y; i++) { if (imageOut2[i]<0) imageOut2[i]=0; if (imageOut2[i]>255) imageOut2[i]=255; imageOut[i]=imageOut2[i]; }
  writePGM(imageOut, x, y, 0, 0, "dilation_arbitrary_SF");
  opening_arbitrary_SF(imageIn2, imageOut2, x, y, se, sizeX, sizeY, posX, posY);
  for (i=0; i<x*y; i++) { if (imageOut2[i]<0) imageOut2[i]=0; if (imageOut2[i]>255) imageOut2[i]=255; imageOut[i]=imageOut2[i]; }
  writePGM(imageOut, x, y, 0, 0, "opening_arbitrary_SF");
  closing_arbitrary_SF(imageIn2, imageOut2, x, y, se, sizeX, sizeY, posX, posY);
  for (i=0; i<x*y; i++) { if (imageOut2[i]<0) imageOut2[i]=0; if (imageOut2[i]>255) imageOut2[i]=255; imageOut[i]=imageOut2[i]; }
  writePGM(imageOut, x, y, 0, 0, "closing_arbitrary_SF");
  
  free(imageIn);
  free(imageOut);

  return 0;
}
