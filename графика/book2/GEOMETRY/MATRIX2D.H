#ifndef	__MATRIX2D__
#define	__MATRIX2D__

#include 	"vector2d.h"

class	Matrix2D
{
public:
	float	a [2][2];

	Matrix2D () {}
	Matrix2D ( float );
	Matrix2D ( const Matrix2D& );

	Matrix2D& operator =  ( const Matrix2D& )
	Matrix2D& operator =  ( float );
	Matrix2D& operator += ( const Matrix2D& );
	Matrix2D& operator -= ( const Matrix2D& );
	Matrix2D& operator *= ( const Matrix2D& );
	Matrix2D& operator *= ( float );
	Matrix2D& operator /= ( float );

	void	invert ();
	void	transpose ();
	float	det ()
	{
		return x [0][0] * x [1][1] - x [0][1] * x [1][0];
	}

	static Matrix2D scale   ( const Vector2D& );
	static Matrix2D rotate  ( float );
	static Matrix2D mirrorX ();
	static Matrix2D mirrorY ();

	friend Matrix2D operator + ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator - ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator * ( const Matrix2D&, const Matrix2D& );
	friend Matrix2D operator * ( const Matrix2D&, float );
	friend Matrix2D operator * ( float,           const Matrix2D& );
	friend Vector2D operator * ( const Matrix2D&, const Vector2D& );
};

inline	Vector2D operator * ( const Matrix2D& m, const Vector2D& v )
{
	return Vector2D ( m.a [0][0]*v.x + m.a [0][1]*v.y,
			  m.a [1][0]*v.x + m.a [1][1]*v.y );
}

#endif
