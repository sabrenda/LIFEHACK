#include	"matrix2d.h"

Matrix2D :: Matrix2D ( float v )
{
	a [0][1] = a [1][0] = 0.0;
	a [0][0] = a [1][1] = v;
}

Matrix2D :: Matrix2D ( const Matrix2D& m )
{
	a [0][0] = m.a [0][0];
	a [0][1] = m.a [0][1];
	a [1][0] = m.a [1][0];
	a [1][1] = m.a [1][1];
}

Matrix2D& Matrix2D :: operator = ( const Matrix2D& m )
{
	a [0][0] = m.a [0][0];
	a [0][1] = m.a [0][1];
	a [1][0] = m.a [1][0];
	a [1][1] = m.a [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator = ( float x )
{
	a [0][1] = a [1][0] = 0.0;
	a [0][0] = a [1][1] = x;

	return *this;
}

Matrix2D& Matrix2D :: operator += ( const Matrix2D& m )
{
	a [0][0] += m.a [0][0];
	a [0][1] += m.a [0][1];
	a [1][0] += m.a [1][0];
	a [1][1] += m.a [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator -= ( const Matrix2D& m )
{
	a [0][0] -= m.a [0][0];
	a [0][1] -= m.a [0][1];
	a [1][0] -= m.a [1][0];
	a [1][1] -= m.a [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator *= ( const Matrix2D& m )
{
	Matrix2D c ( *this );

	a [0][0] = c.a [0][0]*m.a [0][0] + c.a [0][1]*m.a [1][0];
	a [0][1] = c.a [0][0]*m.a [0][1] + c.a [0][1]*m.a [1][1];
	a [1][0] = c.a [1][0]*m.a [0][0] + c.a [1][1]*m.a [1][0];
	a [1][1] = c.a [1][0]*m.a [0][1] + c.a [1][1]*m.a [1][1];

	return *this;
}

Matrix2D& Matrix2D :: operator *= ( float f )
{
	a [0][0] *= f;
	a [0][1] *= f;
	a [1][0] *= f;
	a [1][1] *= f;
	return *this;
}

Matrix2D& Matrix2D :: operator /= ( float f )
{
	a [0][0] /= f;
	a [0][1] /= f;
	a [1][0] /= f;
	a [1][1] /= f;

	return *this;
};

void Matrix2D :: invert ()
{
	float det = a [0][0]*a [1][1] - a [0][1]*a [1][0];
	Matrix2D m;

	m.a [0][0] =  a [1][1] / det;
	m.a [0][1] = -a [0][1] / det;
	m.a [1][0] = -a [1][0] / det;
	m.a [1][1] =  a [0][0] / det;

	*this = m;
}

void Matrix2D :: transpose ()
{
	Matrix2D m;

	m.a [0][0] = a [0][0];
	m.a [0][1] = a [1][0];
	m.a [1][0] = a [0][1];
	m.a [1][1] = a [1][1];

	*this = m;
}

Matrix2D Matrix2D :: scale ( const Vector2D& v )
{
	Matrix2D m;

	m.a [0][0] = v.x;
	m.a [1][1] = v.y;
	m.a [0][1] =
	m.a [1][0] = 0.0;

	return m;
}

Matrix2D Matrix2D :: rotate ( float angle )
{
	float    cosine,sine;
	Matrix2D m ( 1.0 );

	cosine     = cos ( angle );
	sine       = sin ( angle );
	m.a [0][0] = cosine;
	m.a [0][1] = sine;
	m.a [1][0] = -sine;
	m.a [1][1] = cosine;

	return m;
}

Matrix2D Matrix2D :: mirrorX ()
{
	Matrix2D m ( 1.0 );

	m.a [0][0] = -1.0;

	return m;
}

Matrix2D Matrix2D :: mirrorY ()
{
	Matrix2D m ( 1.0 );

	m.a [1][1] = -1.0;

	return m;
}

Matrix2D operator + ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x[0][0]=a.x[0][0]+b.x[0][0];
	c.x[0][1]=a.x[0][1]+b.x[0][1];
	c.x[1][0]=a.x[1][0]+b.x[1][0];
	c.x[1][1]=a.x[1][1]+b.x[1][1];

	return c;
}

Matrix2D operator - ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x[0][0]=a.x[0][0]-b.x[0][0];
	c.x[0][1]=a.x[0][1]-b.x[0][1];
	c.x[1][0]=a.x[1][0]-b.x[1][0];
	c.x[1][1]=a.x[1][1]-b.x[1][1];

	return c;
}

Matrix2D operator * ( const Matrix2D& a, const Matrix2D& b )
{
	Matrix2D c;

	c.x[0][0]=a.x[0][0]*b.x[0][0]+a.x[0][1]*b.x[1][0];
	c.x[0][1]=a.x[0][0]*b.x[0][1]+a.x[0][1]*b.x[1][1];
	c.x[1][0]=a.x[1][0]*b.x[0][0]+a.x[1][1]*b.x[1][0];
	c.x[1][1]=a.x[1][0]*b.x[0][1]+a.x[1][1]*b.x[1][1];

	return c;
}

Matrix2D operator * ( const Matrix2D& a, float b )
{
	Matrix2D c;

	c.x[0][0]=a.x[0][0]*b;
	c.x[0][1]=a.x[0][1]*b;
	c.x[1][0]=a.x[1][0]*b;
	c.x[1][1]=a.x[1][1]*b;

	return c;
}

Matrix2D operator * ( float b, const Matrix2D& a )
{
	Matrix2D c;

	c.x[0][0]=a.x[0][0]*b;
	c.x[0][1]=a.x[0][1]*b;
	c.x[1][0]=a.x[1][0]*b;
	c.x[1][1]=a.x[1][1]*b;

	return c;
}
