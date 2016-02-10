
#ifndef PPMGFX_H_INCLUDED
#define PPMGFX_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

typedef
	uint32_t
	ppc_t;

typedef
	struct ppm_t_struct
	ppm_t;

enum ppm_fmt {
	PPM_RAW = 0,
	PPM_PNG,
};

int ppm_create( ppm_t **pp, size_t w, size_t h );
int ppm_destroy( ppm_t **pp );

int ppm_clear( ppm_t *p );
int ppm_fill( ppm_t *p, ppc_t col );
int ppm_drawdot( ppm_t *pp, size_t x, size_t y, ppc_t col );

int ppm_write( ppm_t *p, FILE *f, enum ppm_fmt fmt );

#endif	//ndef PPMGFX_H_INCLUDED

/* EOF */
