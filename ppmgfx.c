
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ppmgfx.h"

struct ppm_t_struct {
	size_t w, h;
	ppc_t *b;
};


int ppm_create( ppm_t **pp, size_t w, size_t h )
{
	ppm_t *p;
	size_t sz = w * h * sizeof *p->b;
		
 	if ( !pp )
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

int ppm_fill( ppm_t *p, ppc_t col )
{
	size_t sz = p->w * p->h;

	if ( !p )
		return errno = EINVAL, -1;
	while ( sz-- )
		p->b[sz] = col;
	return 0;
}

int ppm_drawdot( ppm_t *p, size_t x, size_t y, ppc_t col )
{
	if ( !p || x >= p->w || y >= p->h )
		return errno = EINVAL, -1;
	p->b[ y * p->w + x ] = col;
	return 0;
}

int ppm_write( ppm_t *p, FILE *f )
{
	size_t sz = p->w * p->h;
	
	if ( !f || !p )
		return errno = EINVAL, -1;
	fprintf( f, "P6\n# CREATOR: HENON\n%zu %zu\n255\n", p->w, p->h );
	for ( size_t i = 0; i < sz; ++i )
	{
		fputc( ( p->b[i] >> 16 ) & 0xff, f );
		fputc( ( p->b[i] >> 8  ) & 0xff, f );
		fputc( ( p->b[i]       ) & 0xff, f );
	}
	return 0;
}

/* EOF */
