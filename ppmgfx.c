
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ppmgfx.h"

#ifdef USE_PNG
	#include "pngwrite.h"
#endif

struct ppm_t_struct {
	size_t w, h;
	ppc_t *b;
};


int ppm_create( ppm_t **pp, size_t w, size_t h )
{
	ppm_t *p;
	size_t sz = w * h * sizeof *p->b;

 	if ( !pp || !w || !h )
		return errno = EINVAL, -1;
	if ( NULL == ( p = malloc( sizeof *p ) ) )
		return -1;
	if ( NULL == ( p->b = malloc( sz ) ) )
		return free( p ), -1;
	p->w = w;
	p->h = h;
	*pp = p;
	return 0;
}

int ppm_destroy( ppm_t **pp )
{
	if ( !pp )
		return errno = EINVAL, -1;
	free( (*pp)->b );
	free( *pp );
	*pp = NULL;
	return 0;
}

int ppm_clear( ppm_t *p )
{
	if ( !p )
		return errno = EINVAL, -1;
	memset( p->b, 0, p->w * p->h * sizeof *p->b );
	return 0;
}


#define SETPIXEL(P,C) \
			do{ ((uint8_t *)(P))[0] = (C) >> 16 & 0xff; /* R */ \
				((uint8_t *)(P))[1] = (C) >>  8 & 0xff; /* G */ \
				((uint8_t *)(P))[2] = (C)       & 0xff; /* B */ \
				((uint8_t *)(P))[3] =             0xff; /* A */ \
			}while(0);

int ppm_fill( ppm_t *p, ppc_t col )
{
	size_t sz = p->w * p->h;

	if ( !p )
		return errno = EINVAL, -1;
	while ( sz-- )
		SETPIXEL( &p->b[sz], col );
	return 0;
}

static inline int ppm_drawdot_unsafe( ppm_t *p, size_t x, size_t y, ppc_t col )
{
	SETPIXEL( &p->b[ y * p->w + x ], col );
	return 0;
}

#undef SETPIXEL

int ppm_drawdot( ppm_t *p, size_t x, size_t y, ppc_t col )
{
	if ( !p || x >= p->w || y >= p->h )
		return errno = EINVAL, -1;
	return ppm_drawdot_unsafe( p, x, y, col );
}

int ppm_write( ppm_t *p, FILE *f, enum ppm_fmt fmt )
{
	int r = -1;
	int errsav;
	size_t sz = p->w * p->h;
	uint8_t *b = (uint8_t *)p->b;

	if ( !f || !p )
		return errno = EINVAL, -1;
	switch ( fmt )
	{
	case PPM_RAW:
		errsav = errno;
		errno = 0;
		fprintf( f, "P6\n# Creator: %s\n%zu %zu\n255\n", __FILE__, p->w, p->h );
		while ( sz-- )
		{
			fputc( *b++, f ); /* R */
			fputc( *b++, f ); /* G */
			fputc( *b++, f ); /* B */
			b++; /* A */
		}
		if ( 0 == errno )
		{
			r = 0;
			errno = errsav;
		}
		break;
#ifdef USE_PNG
	case PPM_PNG:
		r = png_write( f, (uint8_t *)p->b, p->w, p->h );
		break;
#endif
	default:
		errno = EINVAL;
		break;
	}
	return r;
}

/* EOF */
