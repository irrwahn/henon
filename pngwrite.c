#ifdef USE_PNG

#include <stdlib.h>
#include <errno.h>
#include <png.h>

#include "pngwrite.h"

int png_write( FILE *fp, uint8_t *b, unsigned w, unsigned h )
{
	png_structp png = NULL;
	png_infop info = NULL;
	png_bytep *row = NULL;
	volatile int r = -1;

	if ( !fp || !b || !w || !h )
		return errno = EINVAL, -1;
	/* Allocate and populate row pointer array. */
	if ( NULL == ( row = malloc( h * sizeof *row ) ) )
		return -1;
	for ( unsigned i = 0; i < h; ++i )
		row[i] = b + i * w * 4;		/* 4 bytes per pixel. */

	/* Boilerplate libpng code follows. */
	png = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( !png )
		goto CLEANUP;
	info = png_create_info_struct( png );
	if ( !info )
		goto CLEANUP;
	/* Set longjump() target for libpng "error handling". */
	if ( setjmp( png_jmpbuf( png ) ) )
		goto CLEANUP;
	png_init_io( png, fp );
	png_set_IHDR( png, info, w, h, 8, /* 8 bits per component. */
				  PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
				  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );
	png_write_info( png, info );
	png_write_image( png, row );
	png_write_end( png, NULL );
	r = 0;
CLEANUP:
	png_free_data( png, info, PNG_FREE_ALL, -1 );
	png_destroy_write_struct( png?&png:NULL, info?&info:NULL );
	free( row );
	if ( r )
		errno = EIO; /* FIXME: Stopgap hack! */
	return r;
}

#endif //def USE_PNG

/* EOF */
