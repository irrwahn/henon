#ifdef USE_PNG

#ifndef PNGWRITE_H_INCLUDED
#define PNGWRITE_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

/*
 * Compress w*h*4 bytes of image data (8 bits per component) from
 * buffer b into default RGBA PNG format and write to file fp.
 */
int png_write( FILE *fp, uint8_t *b, unsigned w, unsigned h );

#endif	//ndef PNGWRITE_H_INCLUDED

#endif	//def USE_PNG

/* EOF */
