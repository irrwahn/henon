/*
 * This file is part of the henon project.
 *
 * Copyright 2016 Urban Wallasch <irrwahn35@freenet.de>
 * See LICENSE file for more details.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "pal.h"

int pal_create( pal_t **pp )
{
	pal_t *p;

 	if ( !pp )
		return errno = EINVAL, -1;
	if ( NULL == ( p = malloc( sizeof *p ) ) )
		return -1;
	p->max = p->ent = 0;
	p->col = NULL;
	*pp = p;
	return 0;
}

int pal_destroy( pal_t **pp )
{
	if ( !pp )
		return errno = EINVAL, -1;
	free( (*pp)->col );
	free( *pp );
	*pp = NULL;
	return 0;
}

int pal_clear( pal_t *p )
{
	if ( !p )
		return errno = EINVAL, -1;
	free( p->col );
	p->max = p->ent = 0;
	p->col = NULL;
	return 0;
}

int pal_resize( pal_t *p, size_t nmax )
{
	ppc_t *t;

	if ( !p )
		return errno = EINVAL, -1;
	if ( nmax == p->max )
		return 0;
	if ( !nmax )
	{
		free( p->col );
		p->col = NULL;
		p->ent = p->max = 0;
		return 0;
	}
	if ( NULL == ( t = realloc( p->col, nmax * sizeof *p->col ) ) )
		return -1;
	p->col = t;
	p->max = nmax;
	if ( p->ent > p->max )
		p->ent = p->max;
	return 0;
}

int pal_add( pal_t *p, ppc_t col )
{
	if ( !p )
		return errno = EINVAL, -1;
	if ( p->ent >= p->max && 0 != pal_resize( p, p->ent + 32 ) )
		return -1;
	p->col[p->ent++] = col;
	return 0;
}

int pal_read( pal_t *p, FILE *f )
{
	char s[256];
	char *sp, *ep;
	ppc_t col;

	if ( !f || !p )
		return errno = EINVAL, -1;
	while ( NULL != fgets( s, sizeof s, f ) )
	{
		/* Accept '#' as valid prefix for hexadecimal constants. */
		for ( sp = s; isspace( (unsigned char)sp[0] ); ++sp )
			;
		if ( '#' == sp[0] && isxdigit( (unsigned char)sp[1] ) )
			++sp;
		col = strtoul( sp, &ep, 16 );
		if ( ep != sp && 0 != pal_add( p, col ) )
			return -1;
	}
	return 0;
}

int pal_flip( pal_t *p )
{
	int a, b;
	ppc_t c;
	if ( !p )
		return errno = EINVAL, -1;
	for ( a = 0, b = p->ent - 1; a < b; ++a, --b )
		c = p->col[a], p->col[a] = p->col[b], p->col[b] = c;
	return 0;
}


/* EOF */
