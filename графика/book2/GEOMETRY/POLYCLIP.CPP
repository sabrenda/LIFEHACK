#include	"polygon.h"

#define	EPS		1e-7
#define MAX_VERTICES	100

inline	void addVertexToOutPolygon ( Vector2D * outPolygon,
	int& outVertices, int lastVertex, float x, float y )
{
	if ( outVertices == 0 || (fabs (x - outPolygon [outVertices-1].x) > EPS ||
	     fabs (y - outPolygon [outVertices-1].y) > EPS ) &&
	     (!lastVertex || fabs (x - outPolygon [0].x) > EPS ||
	     fabs (y - outPolygon [0].y) > EPS ) )
	{
		outPolygon [outVertices].x = x;
		outPolygon [outVertices].y = y;
		outVertices++;
	}
}

Polygon * clipPolygon ( const Polygon& poly, const Polygon& clipPoly )
{
	Vector2D   outPoly1 [MAX_VERTICES];
	Vector2D   outPoly2 [MAX_VERTICES];
	Vector2D * inPolygon   = poly.vertices;
	Vector2D * outPolygon  = outPoly1;
	int	   outVertices = 0;
	int	   inVertices  = poly.numVertices;

	for ( int edge = 0; edge < clipPoly.numVertices; edge++ )
	{
		int	lastVertex = 0;		// is this the last vertex
		int	next = edge == clipPoly.numVertices - 1 ? 0 : edge + 1;
		float	px = inPolygon [0].x;
		float	py = inPolygon [0].y;
		float	dx = clipPoly.vertices [next].x - clipPoly.vertices [edge].x;
		float	dy = clipPoly.vertices [next].y - clipPoly.vertices [edge].y;
		int	prevVertexInside = (px - clipPoly.vertices [edge].x)*dy -
					   (py - clipPoly.vertices [edge].y)*dx > 0;
		int	intersectionCount = 0;

		outVertices = 0;

		for ( int i = 1; i <= inVertices; i++ )
		{
			float	cx, cy;

			if ( i < inVertices )
			{
				cx = inPolygon [i].x;
				cy = inPolygon [i].y;
			}
			else
			{
				cx = inPolygon [0].x;
				cy = inPolygon [0].y;
				lastVertex = 1;
			}

			// if starting vertex is visible then put it
			// into the output array
			if ( prevVertexInside )
				addVertexToOutPolygon ( outPolygon, outVertices, lastVertex, px, py );

			int	curVertexInside = (cx - clipPoly.vertices [edge].x)*dy -
						  (cy - clipPoly.vertices [edge].y)*dx > 0;


			// if vertices are on different sides of the edge
			// then look where we're intersecting
			if ( prevVertexInside != curVertexInside )
			{
				double denominator = (cx-px)*dy - (cy-py)*dx;

				if ( denominator != 0.0 )
				{
					float	t = ((py-clipPoly.vertices [edge].y)*dx -
					      (px-clipPoly.vertices [edge].x)*dy) / denominator;

					float	tx, ty;	// point of intersection

					if ( t <= 0.0 )
					{
						tx = px;
						ty = py;
					}
					else
					if ( t >= 1.0 )
					{
						tx = cx;
						ty = cy;
					}
					else
					{
						tx = px + t*(cx - px);
						ty = py + t*(cy - py);
					}

					addVertexToOutPolygon ( outPolygon,
						outVertices, lastVertex, tx, ty );

				}

				if ( ++intersectionCount >= 2 )
				{
					if ( fabs (denominator) < 1 )
						intersectionCount = 0;
					else
					{	// drop out, after adding all vertices left in input polygon
						if ( curVertexInside )
						{
							memcpy ( &outPolygon [outVertices], &inPolygon [i],
								(inVertices - i)*sizeof ( Vector2D ) );

							outVertices += inVertices - i;
						}

						break;
					}
				}
			}

			px = cx;
			py = cy;
			prevVertexInside = curVertexInside;
		}

		// if polygon is wiped out, break

		if ( outVertices < 3 )
			break;

		// switch input/output polygons
		inVertices = outVertices;
		inPolygon  = outPolygon;
		if ( outPolygon == outPoly2 )
			outPolygon = outPoly1;
		else
			outPolygon = outPoly2;
	}

	if ( outVertices < 3 )
		return NULL;

	return new Polygon ( outPolygon, outVertices );
}
