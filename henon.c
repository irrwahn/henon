/* Henon Mappings */

#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#include "ppmgfx.h"
#include "pal.h"

typedef
	struct hparm_struct {
		double a;			/* Only used to pass current angle to henon_1()! */
		double a0, a1, as;
		double v0, v1, vs;
		double zoom;
		unsigned iter;
		unsigned xdim, ydim;
		int xoff, yoff;
	}
	hparm_t;

enum vartype {
	INT=1,
	UINT,
	DBL,
};


static ppm_t *canvas = NULL;
static pal_t *pal = NULL;
static hparm_t parm = {
	.a0 = 0.0000,	/* from angle */
	.a1 = 6.2832,	/* to angle */
	.as = 0.0100,	/* angle increment (between frames) */
	.v0 = 0.0500,	/* from seed value */
	.v1 = 1.0000,	/* to seed value */
	.vs = 0.0500,	/* seed value increment */
	.iter = 3000,	/* iterations per seed value */
	.xdim = 640,	/* image width */
	.ydim = 480,	/* image height */
	.xoff = 0,		/* horizontal offset of origin */
	.yoff = 0,		/* vertical offset of origin */
	.zoom = 1.0,	/* zoom factor */
};

static struct {
	const char *name;
	enum vartype type;
	void *addr;
} var[] = {
	{ "a0", DBL, &parm.a0 },
	{ "a1", DBL, &parm.a1 },
	{ "as", DBL, &parm.as },
	{ "v0", DBL, &parm.v0 },
	{ "v1", DBL, &parm.v1 },
	{ "vs", DBL, &parm.vs },
	{ "iter", UINT, &parm.iter },
	{ "xdim", UINT, &parm.xdim },
	{ "ydim", UINT, &parm.ydim },
	{ "xoff", INT, &parm.xoff },
	{ "yoff", INT, &parm.yoff },
	{ "zoom", DBL, &parm.zoom },
	{ NULL, 0, NULL }
};


static void print_usage( const char *argv0 )
{
	const char *p = ( NULL == ( p = strrchr( argv0, '/' ) ) ) ? argv0 : p+1;
	fprintf( stderr, 
		"%s: Create parametrized Hénon Mappings as a series of PPM images.\n"
		"Usage: %s [-h] [-h] [-c paramfile] [-p palettefile]\n"
		"  -c : Set parameter file path; default: none.\n"
		"  -o : Set output path prefix; default: .\n"
		"  -p : Set color palette file path, first entry is background color;\n"
		"       default: none.\n"
		"  -f : Flip color palette.\n"
		"  -v : Increase verbosity, show progress.\n"
		"  -h : Display this help text and exit.\n"
		, p, p );
}

static void err_exit( const char *fmt, ... )
{
	int eno = errno;
	va_list arglist;

	va_start( arglist, fmt );
	fprintf( stderr, "henon: " );
	vfprintf( stderr, fmt, arglist );
    if ( eno )
		fprintf( stderr, ": %s\n", strerror( eno ) );
	else
		fputc( '\n', stderr );
    va_end( arglist );
	exit( EXIT_FAILURE );
}

static int readpalfile( const char *fn, pal_t *p )
{
	FILE *f;
	int r = -1;
	
	if ( !fn || !p )
		return errno = EINVAL, -1;
	if ( NULL == ( f = fopen( fn, "r" ) ) )
		return -1;
	pal_clear( p );
	r = pal_read( p, f );
	fclose( f );
	return r;
}

static int readparm( FILE *fp, hparm_t *pp )
{
	char *p;
	char s[1000];
	char *name;
	char *val;
	
	if ( !fp || !pp )
		return errno = EINVAL, -1;
	while ( NULL != fgets( s, sizeof s, fp ) )
	{
		p = s;
		while ( *p && isspace( (unsigned char)*p ) )
			++p;
		if ( !*p || !isalpha( (unsigned char)*p ) )
			continue;
		name = p;
		while ( *p && isalnum( (unsigned char)*p ) )
			++p;
		while ( isspace( (unsigned char)*p ) )
			*p++ = '\0';
		if ( '=' != *p )
			continue;
		*p++ = '\0';
		if ( !*name )
			continue;
		while ( isspace( (unsigned char)*p ) )
			p++;
		val = p;
		while ( *p && '\r' != *p && '\n' != *p )
			++p;
		*p = '\0';
		
		for ( int i = 0; var[i].name; ++i )
		{
			if ( 0 == strcmp( name, var[i].name ) )
			{
				switch ( var[i].type )
				{
				case DBL:  
					*(double *)var[i].addr = strtod( val, NULL ); 
					break;
				case INT:
					*(int *)var[i].addr = strtol( val, NULL, 10 ); 
					break;
				case UINT: 
				default:
					*(unsigned *)var[i].addr = strtoul( val, NULL, 10 ); 
					break;
				}
			}
		}
	}
	return 0;
}

static int readparmfile( const char *fn, hparm_t *pp )
{
	int r = -1;
	FILE *fp;
	
	if ( !fn || !pp )
		return errno = EINVAL, -1;
	if ( NULL == ( fp = fopen( fn, "r" ) ) )
		return -1;
	r = readparm( fp, pp );
	fclose( fp );
	return r;
}

static int dumpparm( FILE *fp )
{
	if ( !fp )
		return errno = EINVAL, -1;

	for ( int i = 0; var[i].name; ++i )
	{
		fprintf( fp, "%s=", var[i].name );
		switch ( var[i].type )
		{
		case DBL:  
			fprintf( fp, "%f", *(double *)var[i].addr );
			break;
		case INT:
			fprintf( fp, "%d", *(int *)var[i].addr ); 
			break;
		case UINT: 
		default:
			fprintf( fp, "%u", *(unsigned *)var[i].addr ); 
			break;
		}
		fputc( '\n', fp );
	}
	return 0;
}


/*
 * Draw single frame by sweeping over start value interval with 
 * a constant angle.
 */

/* Interface macros to keep henon_1() clean */
#define SCALE(A,B,Z)	((double)(((A)<(B))?(A):(B))/2*(Z))
#define NUMCOL			(pal->ent)
#define COLOR(N)		(pal->col[(N)])
#define DRAWDOT(X,Y,C)	ppm_drawdot(canvas,(X),(Y),(C))

static int henon_1( hparm_t *pp )
{
	int colidx = 0;
	int hx = pp->xdim / 2 + pp->xoff * pp->zoom;
	int hy = pp->ydim / 2 + pp->yoff * pp->zoom;
	unsigned hz = SCALE( pp->xdim, pp->ydim, pp->zoom );
	double sin_a = sin( pp->a );
	double cos_a = cos( pp->a );
	double x, y, v, h;

	if ( pp->v0 > pp->v1 )
		h = pp->v0, pp->v0 = pp->v1, pp->v1 = h;
	if ( pp->vs < 0.0 )
		pp->vs = -pp->vs;
	for ( v = pp->v0; v <= pp->v1; v += pp->vs )
	{
		colidx = colidx % (NUMCOL-1) + 1;
		x = y = v;
		for ( int i = pp->iter; i--; )
		{
			h = y - x * x;
			y = x * sin_a + h * cos_a;
			x = x * cos_a - h * sin_a;
			DRAWDOT( x * hz + hx, y * hz + hy, COLOR(colidx) );
		}
	}
	return 0;
}
#undef SCALE
#undef NUMCOL
#undef COLOR
#undef DRAWDOT

/*
 * Create series of frames by sweeping over [a0;a1], calling henon_1() 
 * for each increment.
 */
static int henon( hparm_t *pp, const char *dir, int verbose )
{
	size_t frame_no = 0;
	char fn[1024];
	FILE *fp;

	errno = 0;
	if (   ( pp->as > 0.0 && pp->a0 > pp->a1 )
		|| ( pp->as < 0.0 && pp->a0 < pp->a1 ) )
		err_exit( "Sign of 'as' must match [a0;a1] interval orientation!" );
	if ( 0.0 == pp->as )
		err_exit( "Increment 'as' must not be zero!" );
	if (   ( pp->vs > 0.0 && pp->v0 > pp->v1 )
		|| ( pp->vs < 0.0 && pp->v0 < pp->v1 ) )
		err_exit( "Sign of 'vs' must match [v0;v1] interval orientation!" );
	if ( 0.0 == pp->vs )
		err_exit( "Increment 'vs' must not be zero!" );
	if ( 0.0 >= pp->zoom )
		err_exit( "Zoom must be positive!" );

	if ( !dir || !*dir )
		dir = ".";
	if ( verbose )
		printf( "Generating %d frames ...\n", (int)((pp->a1-pp->a0)/pp->as) + 1 );
		
	for ( pp->a = pp->a0; 
		  pp->as > 0.0 ? pp->a <= pp->a1 : pp->a >= pp->a1;
		  pp->a += pp->as 
		)
	{
		snprintf( fn, sizeof fn, "%s/%05zu.ppm", dir, frame_no );
		if ( verbose && 0 == frame_no % 25 )
			printf( "\r%s [a=%.04f]", fn, pp->a ), fflush( stdout );
		if ( NULL == ( fp = fopen( fn, "w" ) ) )
			err_exit( "fopen '%s' for writing", fn );
		
		ppm_fill( canvas, pal->col[0] );
		henon_1( pp );
		
		if ( 0 != ppm_write( canvas, fp ) )
			err_exit( "ppm_write to '%s'" );
		fclose( fp );
		++frame_no;
	}
	if ( verbose )
		printf( "\r%s [a=%.04f]\n", fn, pp->a );
	return 0;
}

int main( int argc, char *argv[] )
{
	int verbose = 0;
	int palflip = 0;
	int opt;
	const char *optstr = ":fhvc:o:p:";
	const char *parmfile = NULL;
	const char *palfile = NULL;
	const char *outpfx = NULL;

	opterr = 0;
	errno = 0;
	while ( -1 != ( opt = getopt( argc, argv, optstr ) ) )
	{
		switch ( opt )
		{
		case 'c':
			parmfile = optarg;
			break;
		case 'o':
			outpfx = optarg;
			break;
		case 'p':
			palfile = optarg;
			break;
		case 'f':
			palflip = 1;
			break;
		case 'v':
			++verbose;
			break;
		case '?':
			print_usage( argv[0] );
			err_exit( "Unrecognized option '-%c'.", optopt );
			break;
		case ':':
			print_usage( argv[0] );
			err_exit( "Missing argument for option '-%c'.", optopt );
			break;
		case 'h':
			print_usage( argv[0] );
			exit( EXIT_SUCCESS );
		default:
			print_usage( argv[0] );
			exit( EXIT_FAILURE );
			break;
		}
	}
	if ( optind < argc )
	{
		print_usage( argv[0] );
		err_exit( "Excess non-option command line argument '%s'.", argv[optind] );
	}

	if ( 0 != pal_create( &pal ) )
		err_exit( "pal_create" );
	if ( palfile )
	{
		if ( 0 != readpalfile( palfile, pal ) )
			err_exit( "Reading palette file '%s'", palfile );
	}
	else
	{	/* trivial KRYGCBM fallback palette :) */
		pal_add( pal, 0x000000 );
		pal_add( pal, 0xFF0000 ); pal_add( pal, 0xFFFF00 );
		pal_add( pal, 0x00FF00 ); pal_add( pal, 0x00FFFF );
		pal_add( pal, 0x0000FF ); pal_add( pal, 0xFF00FF );
	}
	if ( pal->ent < 2 )
		errno = 0, err_exit( "At least 2 palette entries required!" );
	if ( palflip )
		pal_flip( pal );
	if ( parmfile && 0 != readparmfile( parmfile, &parm ) )
		err_exit( "Reading parameter file '%s'", parmfile );
	if ( verbose )
	{
		printf("Have %zu palette entries.\n", pal->ent );
		dumpparm( stdout );
	}
	if ( 0 != ppm_create( &canvas, parm.xdim, parm.ydim ) )
		err_exit( "ppm_create" );
	henon( &parm, outpfx, verbose );
	ppm_destroy( &canvas );
	exit( EXIT_SUCCESS );
}

/* EOF */
