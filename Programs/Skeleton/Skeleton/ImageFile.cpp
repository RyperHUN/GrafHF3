//********************************************************************
//  BMP, TARGA, PCX image file input
//********************************************************************

#include "ImageFile.h"
#include <string.h>

//-----------------------------------------------------------------------
ImageFile :: ImageFile( const char * inputfilename, int& width, int& height ) {
//-----------------------------------------------------------------------
	file = NULL;
	image = imageWithAlpha = NULL;
	width = height = 0;

	if ( !inputfilename ) return;

	char path[160];
	sprintf(path, "images\\%s", inputfilename);	// Files are in images subdirectory

	file = fopen(path, "rb");
	if ( !file ) return;
	printf("\n %s is loaded ...", path);
	for(int i = 0; path[i] != '\0'; i++) {
		if (path[i] == '.') {
			if ( strcmp(&path[i+1], "bmp") == 0 ||
				 strcmp(&path[i+1], "BMP") == 0) ReadBMP(file, width, height);
			if ( strcmp(&path[i+1], "tga") == 0 ||
				 strcmp(&path[i+1], "TGA") == 0) ReadTGA(file, width, height);
			if ( strcmp(&path[i+1], "pcx") == 0 ||
				 strcmp(&path[i+1], "PCX") == 0) ReadPCX(file, width, height);
			break;
		}
	}
}

/* BITMAPFILEHEADER
0.        unsigned short    bfType;
1.        unsigned long     bfSize;
3.        unsigned short    bfReserved1;
4.        unsigned short    bfReserved2;
5.        unsigned long     bfOffBits;
7.        unsigned long     biSize;
9.        long			    biWidth;
11.       long              biHeight;
13.       unsigned short    biPlanes;
14.       unsigned short    biBitCount;
15.       unsigned long     biCompression;
17.       unsigned long     biSizeImage;
19.       long			    biXPelsPerMeter;
21.       long              biYPelsPerMeter;
23.       unsigned long     biClrUsed;
25.       unsigned long     biClrImportant;
*/

//-----------------------------------------------------------------------
void ImageFile :: ReadBMP( FILE * file, int& width, int& height ) {
//-----------------------------------------------------------------------
	unsigned short bitmapFileHeader[27];					// bitmap header
	fread(&bitmapFileHeader, 27, 2, file);
	if (bitmapFileHeader[0] != 0x4D42 ) {   // magic number
		return;
	}

	if (bitmapFileHeader[14] != 24) {
		printf("only true color bmp files are supported\n");
		return;
	}
	width = bitmapFileHeader[9];
	height = bitmapFileHeader[11];
	size = (unsigned long)bitmapFileHeader[17] + (unsigned long)bitmapFileHeader[18] * 65536;

	fseek(file, 54, SEEK_SET);

	image = new Byte[ size ];

	// read the pixels
	fread(image, 1, size, file);

	// Swap R and B since in BMP, the order is BGR
	for (int imageIdx = 0; imageIdx < size; imageIdx += 3) {
		Byte tempRGB = image[imageIdx];
		image[imageIdx] = image[imageIdx + 2];
		image[imageIdx + 2] = tempRGB;
	}
}

//-----------------------------------------------------------------------
void ImageFile :: ReadTGA( FILE * file, int& width, int& height ) {
//-----------------------------------------------------------------------
	for(int i = 0;i < 12; i++) fgetc(file);
	width = fgetc(file) + fgetc(file) * 256L;
	height = fgetc(file) + fgetc(file) * 256L;
	size = width * height * 3;
	fgetc(file); fgetc(file);

	image = new Byte[ size ];

	// read pixels
	fread(image, 1, size, file);

	// Swap R and B since in TGA, the order is BGR
	for (int imageIdx = 0; imageIdx < size; imageIdx += 3) {
		Byte tempRGB = image[imageIdx];
		image[imageIdx] = image[imageIdx + 2];
		image[imageIdx + 2] = tempRGB;
	}
}

//-----------------------------------------------------------------------
void ImageFile :: ReadPCX( FILE * file, int& width, int& height ) {
//-----------------------------------------------------------------------
     // the first character must be 10
     if (fgetc(file) != 10) return;

     // the second character must be 5
     if (fgetc(file) != 5) return;

     // skip two characteors
     fgetc(file);
     fgetc(file);

     // minimal X
     int xMin = fgetc(file);   // lower byte
     xMin |= fgetc(file) << 8; // upper byte

     int yMin = fgetc(file);
     yMin |= fgetc(file) << 8;

     int xMax = fgetc(file);
     xMax |= fgetc(file) << 8;

     int yMax = fgetc(file);
     yMax |= fgetc(file) << 8;

     width = xMax - xMin + 1;
     height = yMax - yMin + 1;

     // Look up table for indexed color mode
     Byte * LUT = new Byte[768];

     // LUT is the last 769 bytes in a PCX file
     fseek(file, -769, SEEK_END);

     // LUT check
	 if (fgetc(file) != 12) {
		 printf("Only indexed color is supported in PCX");
		 return;
	 }

     // LUT input
	 fread(LUT, 1, 768, file);

	 // memory allocation for pixels
	 size = width * height * 3;
     image = new Byte[size];

     // pixels start at location 128
	 fseek(file, 128, SEEK_SET);

     // Run length encoding
     int pixel = 0;
	 while( pixel < width * height ) {
		int c = fgetc(file);
        if (c > 0xbf) { // counting
			int numRepeat = 0x3f & c;
            c = fgetc(file);
			for (int i = 0; i < numRepeat; i++) {
				image[3 * pixel]     = LUT[3 * c+0];
				image[3 * pixel + 1] = LUT[3 * c+1];
				image[3 * pixel + 2] = LUT[3 * c+2];
				pixel++;
			}
		} else {
			image[3 * pixel]     = LUT[3 * c+0];
			image[3 * pixel + 1] = LUT[3 * c+1];
			image[3 * pixel + 2] = LUT[3 * c+2];
			pixel++;
		}
     }
}

//-----------------------------------------------------------------------
Byte * ImageFile :: LoadWithAlpha( int nmin ) {
//-----------------------------------------------------------------------
	if ( image == NULL ) return NULL;

	Byte * imageWithAlpha = new Byte[ size * 4 / 3 ];

	for (int src = 0, dst = 0; src < size; src += 3, dst += 4) {
		// pixel copy
		imageWithAlpha[dst]   = image[src];
		imageWithAlpha[dst+1] = image[src+1];
		imageWithAlpha[dst+2] = image[src+2];
		// if a pixel is black, then alpha = 0, otherwise alpha = 255.
		if (image[src] < nmin && image[src+1] < nmin && image[src+2]  < nmin)
				imageWithAlpha[dst+3] = 0;
		else	imageWithAlpha[dst+3] = 0xFF;
	}
	return imageWithAlpha;
}

