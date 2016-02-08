
#ifndef PAL_H_INCLUDED
#define PAL_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "ppmgfx.h"

typedef 
	struct pal_t_struct 
	pal_t;
	
struct pal_t_struct {
	size_t ent, max;
	ppc_t *col;
};

int pal_create( pal_t **pp );
int pal_destroy( pal_t **pp );

int pal_clear( pal_t *p );
int pal_resize( pal_t *p, size_t nmax );
int pal_add( pal_t *p, ppc_t col );
int pal_read( pal_t *p, FILE *f );
int pal_flip( pal_t *p );

#endif	//ndef PAL_H_INCLUDED
/* EOF */
