//********************************************************************
// Loading BMP, TARGA, and PCX format images
//*********************************************************************

typedef unsigned char Byte;
#include <cstdio>

//=============================================================
class ImageFile {       
//=============================================================
    FILE * file;              // file
	Byte    * image;          // RGB pixels
	Byte	* imageWithAlpha; // RGBA pixels
	int		size;             // number of pixels
	void	ReadBMP(FILE * file, int& width, int& height);
	void	ReadTGA(FILE * file, int& width, int& height);
	void	ReadPCX(FILE * file, int& width, int& height);
public :
    ImageFile( const char * inputfilename, int& width, int& height );
	Byte * Load( ) { return image; }
	Byte * LoadWithAlpha( int min = 0 );
    ~ImageFile( ) { 
		if (file) fclose(file); 
		///TODO megnezni miert van baj
		//if ( image ) delete image;
		if ( imageWithAlpha ) delete imageWithAlpha;
	}
	Byte Red( int i ) { return image[i * 3]; }
	Byte Green( int i ) { return image[i * 3+1]; }
	Byte Blue( int i ) { return image[i * 3+2]; }
};
