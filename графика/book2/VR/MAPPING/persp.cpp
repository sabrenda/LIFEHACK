#include	<conio.h>
#include	<graphics.h>
#include	<math.h>
#include	<process.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	"vector3D.h"
#include	"matrix3D.h"
#include	"point.h"

Vector3D p [4] =
{
	Vector3D ( 0, -1, 6 ),
	Vector3D ( -1, 0, 5 ),
	Vector3D ( 0,  1, 2 ),
	Vector3D ( 1,  0, 3 )
};

Point	pr [4];

Vector3D	a;
Vector3D	e1;
Vector3D	e2;
Vector3D	n, m, l;
Matrix3D	t, prj;

void	projectVertices ()
{
	for ( int i = 0; i < 4; i++ )
	{
		pr [i].x = 500 * p [i].x / p [i].z + 320;
		pr [i].y = 500 * p [i].y / p [i].z + 200;
	}
}

void	puttexel ( int x, int y, float u, float v )
{
	int	k = (int)( 10 * u ) + (int)( 10 * v );

	putpixel ( x, y, k & 1 ? 15 : 0 );
}

void	perspective ()
{

	float	dx01 = (float) ( pr [1].x - pr [0].x ) / (float) ( pr [1].y - pr [0].y );
	float	dx12 = (float) ( pr [2].x - pr [1].x ) / (float) ( pr [2].y - pr [1].y );
	float	dx03 = (float) ( pr [3].x - pr [0].x ) / (float) ( pr [3].y - pr [0].y );
	float	dx32 = (float) ( pr [2].x - pr [3].x ) / (float) ( pr [2].y - pr [3].y );
	float	x1   = pr [0].x;
	float	x2   = pr [0].x;

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 5 );
	outtextxy ( 230, 20, "Perspective" );

	for ( int y = pr [0].y; y < pr [2].y; y++ )
	{
		for ( int x = x1; x < x2; x++ )
		{
			float	du = t.x [0][0] * x + t.x [0][1] * y + t.x [0][2];
			float	dv = t.x [1][0] * x + t.x [1][1] * y + t.x [1][2];
			float	d  = t.x [2][0] * x + t.x [2][1] * y + t.x [2][2];

			puttexel ( x, y, du/d, dv/d );
		}

		if ( y < pr [1].y )
		{
			x1 += dx01;
		}
		else
		{
			x1 += dx12;
		}

		if ( y < pr [3].y )
		{
			x2 += dx03;
		}
		else
		{
			x2 += dx32;
		}
	}
}

void	linear ()
{

	float	dx01 = (float) ( pr [1].x - pr [0].x ) / (float) ( pr [1].y - pr [0].y );
	float	dx12 = (float) ( pr [2].x - pr [1].x ) / (float) ( pr [2].y - pr [1].y );
	float	dx03 = (float) ( pr [3].x - pr [0].x ) / (float) ( pr [3].y - pr [0].y );
	float	dx32 = (float) ( pr [2].x - pr [3].x ) / (float) ( pr [2].y - pr [3].y );
	float	du01 = 1.0 / (float) ( pr [1].y - pr [0].y );
	float	dv01 = 0.0;
	float	du12 = 0.0;
	float	dv12 = 1.0 / (float) ( pr [2].y - pr [1].y );
	float	du03 = 0.0;
	float	dv03 = 1.0 / (float) ( pr [3].y - pr [0].y );
	float	du32 = 1.0 / (float) ( pr [2].y - pr [3].y );
	float	dv32 = 0.0;

	float	x1   = pr [0].x;
	float	x2   = pr [0].x;
	float	u1   = 0.0;
	float	v1   = 0.0;
	float	u2   = 0.0;
	float	v2   = 0.0;

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 5 );
	outtextxy ( 230, 20, "Linear" );

	for ( int y = pr [0].y; y < pr [2].y; y++ )
	{
		float	du = x2 > x1 ? ( u2 - u1 ) / ( x2 - x1 ) : 0.0;
		float	dv = x2 > x1 ? ( v2 - v1 ) / ( x2 - x1 ) : 0.0;
		float	u  = u1;
		float	v  = v1;

		for ( int x = x1; x < x2; x++ )
		{
			puttexel ( x, y, u, v );

			u += du;
			v += dv;
		}

		if ( y < pr [1].y )
		{
			x1 += dx01;
			u1 += du01;
			v1 += dv01;
		}
		else
		{
			x1 += dx12;
			u1 += du12;
			v1 += dv12;
		}

		if ( y < pr [3].y )
		{
			x2 += dx03;
			u2 += du03;
			v2 += dv03;
		}
		else
		{
			x2 += dx32;
			u2 += du32;
			v2 += dv32;
		}
	}
}

void	linear2 ()
{

	float	dx01 = (float) ( pr [1].x - pr [0].x ) / (float) ( pr [1].y - pr [0].y );
	float	dx12 = (float) ( pr [2].x - pr [1].x ) / (float) ( pr [2].y - pr [1].y );
	float	dx03 = (float) ( pr [3].x - pr [0].x ) / (float) ( pr [3].y - pr [0].y );
	float	dx32 = (float) ( pr [2].x - pr [3].x ) / (float) ( pr [2].y - pr [3].y );
	float	x1   = pr [0].x;
	float	x2   = pr [0].x;

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 5 );
	outtextxy ( 230, 20, "Linear 2" );

	for ( int y = pr [0].y; y < pr [2].y; y++ )
	{
		float	du1 = t.x [0][0] * x1 + t.x [0][1] * y + t.x [0][2];
		float	dv1 = t.x [1][0] * x1 + t.x [1][1] * y + t.x [1][2];
		float	d1  = t.x [2][0] * x1 + t.x [2][1] * y + t.x [2][2];
		float	du2 = t.x [0][0] * x2 + t.x [0][1] * y + t.x [0][2];
		float	dv2 = t.x [1][0] * x2 + t.x [1][1] * y + t.x [1][2];
		float	d2  = t.x [2][0] * x2 + t.x [2][1] * y + t.x [2][2];
		float	u1  = du1 / d1;
		float	u2  = du2 / d2;
		float	v1  = dv1 / d1;
		float	v2  = dv2 / d2;
		float	du = x2 > x1 ? ( u2 - u1 ) / ( x2 - x1 ) : 0.0;
		float	dv = x2 > x1 ? ( v2 - v1 ) / ( x2 - x1 ) : 0.0;
		float	u  = u1;
		float	v  = v1;

		for ( int x = x1; x < x2; x++ )
		{
			puttexel ( x, y, u, v );

			u += du;
			v += dv;
		}

		if ( y < pr [1].y )
			x1 += dx01;
		else
			x1 += dx12;

		if ( y < pr [3].y )
			x2 += dx03;
		else
			x2 += dx32;
	}
}

void	quadratic ()
{

	float	dx01 = (float) ( pr [1].x - pr [0].x ) / (float) ( pr [1].y - pr [0].y );
	float	dx12 = (float) ( pr [2].x - pr [1].x ) / (float) ( pr [2].y - pr [1].y );
	float	dx03 = (float) ( pr [3].x - pr [0].x ) / (float) ( pr [3].y - pr [0].y );
	float	dx32 = (float) ( pr [2].x - pr [3].x ) / (float) ( pr [2].y - pr [3].y );
	float	x1   = pr [0].x;
	float	x2   = pr [0].x;

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 5 );
	outtextxy ( 230, 20, "Quadratic" );

	for ( int y = pr [0].y; y < pr [2].y; y++ )
	{
		float	du1 = t.x [0][0] * x1 + t.x [0][1] * y + t.x [0][2];
		float	dv1 = t.x [1][0] * x1 + t.x [1][1] * y + t.x [1][2];
		float	d1  = t.x [2][0] * x1 + t.x [2][1] * y + t.x [2][2];
		float	du2 = t.x [0][0] * x2 + t.x [0][1] * y + t.x [0][2];
		float	dv2 = t.x [1][0] * x2 + t.x [1][1] * y + t.x [1][2];
		float	d2  = t.x [2][0] * x2 + t.x [2][1] * y + t.x [2][2];
		float	u1  = du1 / d1;
		float	u2  = du2 / d2;
		float	v1  = dv1 / d1;
		float	v2  = dv2 / d2;
		float	um  = ( du1 + du2 ) / ( d1 + d2 );
		float	vm  = ( dv1 + dv2 ) / ( d1 + d2 );

		float	invDx = x2 > x1 ? 1.0 / ( x2 - x1 ) : 0.0;
		float	a0 = u1;
		float	a1 = (-3*u1 + 4*um - u2 ) * invDx;
		float	a2 = 2*( u1 - 2*um + u2 ) * invDx * invDx;

		float	b0 = v1;
		float	b1 = (-3*v1 + 4*vm - v2 ) * invDx;
		float	b2 = 2*( v1 - 2*vm + v2 ) * invDx * invDx;

		float	deltaU1 = a1 + a2;
		float	deltaU2 = 2 * a2;
		float	deltaV1 = b1 + b2;
		float	deltaV2 = 2 * b2;

		float	u = u1;
		float	v = v1;

		for ( int x = x1; x < x2; x++ )
		{
			puttexel ( x, y, u, v );

			u       += deltaU1;
			v       += deltaV1;
			deltaU1 += deltaU2;
			deltaV1 += deltaV2;
		}

		if ( y < pr [1].y )
			x1 += dx01;
		else
			x1 += dx12;

		if ( y < pr [3].y )
			x2 += dx03;
		else
			x2 += dx32;
	}
}

void	cubic ()
{

	float	dx01 = (float) ( pr [1].x - pr [0].x ) / (float) ( pr [1].y - pr [0].y );
	float	dx12 = (float) ( pr [2].x - pr [1].x ) / (float) ( pr [2].y - pr [1].y );
	float	dx03 = (float) ( pr [3].x - pr [0].x ) / (float) ( pr [3].y - pr [0].y );
	float	dx32 = (float) ( pr [2].x - pr [3].x ) / (float) ( pr [2].y - pr [3].y );
	float	x1   = pr [0].x;
	float	x2   = pr [0].x;

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 5 );
	outtextxy ( 230, 20, "Cubic" );

	for ( int y = pr [0].y; y < pr [2].y; y++ )
	{
		float	du1 = t.x [0][0] * x1 + t.x [0][1] * y + t.x [0][2];
		float	dv1 = t.x [1][0] * x1 + t.x [1][1] * y + t.x [1][2];
		float	d1  = t.x [2][0] * x1 + t.x [2][1] * y + t.x [2][2];
		float	du2 = t.x [0][0] * x2 + t.x [0][1] * y + t.x [0][2];
		float	dv2 = t.x [1][0] * x2 + t.x [1][1] * y + t.x [1][2];
		float	d2  = t.x [2][0] * x2 + t.x [2][1] * y + t.x [2][2];
		float	invDx = x2 > x1 ? 1.0 / ( x2 - x1 ) : 0.0;
		float	tt1 = ( du2 * d1 - du1 * d2 ) * invDx;
		float	tt2 = ( dv2 * d1 - dv1 * d2 ) * invDx;
		float	tt3 = tt1 / ( d2 * d2 );
		float	tt4 = tt2 / ( d2 * d2 );
		float	du  = tt1 / ( d1 * d2 );
		float	dv  = tt2 / ( d1 * d2 );
		float	a0  = du1 / d1;
		float	a1  = tt1 / ( d1 * d1 );
		float	a2  = ( 3 * du - 2 * a1 - tt3 ) * invDx;
		float	a3  = ( -2 * du + a1 + tt3 ) * invDx * invDx;
		float	b0  = dv1 / d1;
		float	b1  = tt2 / ( d1 * d1 );
		float	b2  = ( 3 * dv - 2 * b1 - tt4 ) * invDx;
		float	b3  = ( -2 * dv + b1 + tt4 ) * invDx * invDx;

		float	deltaU1 = a1 + a2 + a3;
		float	deltaU2 = 6 * a3 + 2 * a2;
		float	deltaU3 = 6 * a3;
		float	deltaV1 = b1 + b2 + b3;
		float	deltaV2 = 6 * b3 + 2 * b2;
		float	deltaV3 = 6 * b3;
		float	u       = a0;
		float	v       = b0;

		for ( int x = x1; x < x2; x++ )
		{
			puttexel ( x, y, u, v );

			u       += deltaU1;
			v       += deltaV1;
			deltaU1 += deltaU2;
			deltaV1 += deltaV2;
			deltaU2 += deltaU3;
			deltaV2 += deltaV3;
		}

		if ( y < pr [1].y )
			x1 += dx01;
		else
			x1 += dx12;

		if ( y < pr [3].y )
			x2 += dx03;
		else
			x2 += dx32;
	}
}

main ()
{
	int	driver = DETECT;
	int	mode;
	int	res;

	projectVertices ();

	a  = p [0];
	e1 = p [1] - p [0];
	e2 = p [3] - p [0];
					// compute "magic" vectors
	n = e1 ^ e2;
	m = e2 ^ a;
	l = a  ^ e1;
					// prepare texture mapping matrix
	t.x [0][0] = m.x;
	t.x [0][1] = m.y;
	t.x [0][2] = m.z;
	t.x [1][0] = l.x;
	t.x [1][1] = l.y;
	t.x [1][2] = l.z;
	t.x [2][0] = n.x;
	t.x [2][1] = n.y;
	t.x [2][2] = n.z;
					// prepare projection matrix
	prj [0][0] = 500.0;
	prj [0][1] = 0.0;
	prj [0][2] = 320.0;
	prj [1][0] = 0.0;
	prj [1][1] = 500.0;
	prj [1][2] = 200.0;
	prj [2][0] = 0.0;
	prj [2][1] = 0.0;
	prj [2][2] = 1.0;

	prj.invert ();

	t *= prj;

	initgraph ( &driver, &mode, "c:\\borlandc\\bgi" );
	if ( ( res = graphresult () ) != grOk )
	{
		printf("\nGraphics error: %s\n", grapherrormsg ( res) );
		exit ( 1 );
	}

	setfillstyle ( SOLID_FILL, WHITE );
	bar ( 0, 0, getmaxx (), getmaxy () );
	setcolor ( BLACK );

	linear ();

	moveto ( pr [0].x, pr [0].y );
	for ( int i = 0; i < 4; i++ )
		lineto ( pr [(i+1)&3].x, pr [(i+1)&3].y );


	getch ();

	setfillstyle ( SOLID_FILL, WHITE );
	bar ( 0, 0, getmaxx (), getmaxy () );
	setcolor ( BLACK );

	linear2 ();

	getch ();

	setfillstyle ( SOLID_FILL, WHITE );
	bar ( 0, 0, getmaxx (), getmaxy () );
	setcolor ( BLACK );

	quadratic ();

	getch ();

	setfillstyle ( SOLID_FILL, WHITE );
	bar ( 0, 0, getmaxx (), getmaxy () );
	setcolor ( BLACK );

	cubic ();

	getch ();

	setfillstyle ( SOLID_FILL, WHITE );
	bar ( 0, 0, getmaxx (), getmaxy () );
	setcolor ( BLACK );

	perspective ();

	getch ();
	closegraph ();
}
