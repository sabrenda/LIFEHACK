#include <math.h>
#include "Matrix.h"

Matrix :: Matrix ( float v )
{
	for ( int i = 0; i < 4; i++)
		for ( int j = 0; j < 4; j++)
			x [i][j] = (i == j) ? v : 0.0;

	x [3][3] = 1;
}

void	Matrix :: Invert ()
{
	Matrix	Out ( 1 );

	for ( int i = 0; i < 4; i++ )
	{
		float	d = x [i][i];

		if ( d != 1.0)
		{
			for ( int j = 0; j < 4; j++ )
			{
				Out.x [i][j] /= d;
				x [i][j]     /= d;
			}
		}

		for ( int j = 0; j < 4; j++ )
		{
			if ( j != i )
			{
				if ( x [j][i] != 0.0)
				{
					float	mulby = x[j][i];

					for ( int k = 0; k < 4; k++ )
					{
						x [j][k]     -= mulby * x [i][k];
						Out.x [j][k] -= mulby * Out.x [i][k];
					}
				}
			}
		}
	}

	*this = Out;
}

void	Matrix :: Transpose ()
{
	float	t;

	for ( int i = 0; i < 4; i++ )
		for ( int j = i; j < 4; j++ )
			if ( i != j )
			{
				t        = x [i][j];
				x [i][j] = x [j][i];
				x [j][i] = t;
			}
}

Matrix&	Matrix :: operator += ( const Matrix& A )
{
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			x [i][j] += A.x [i][j];

	return *this;
}

Matrix&	Matrix :: operator -= ( const Matrix& A )
{
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			x [i][j] -= A.x [i][j];

	return *this;
}

Matrix&	Matrix :: operator *= ( float v )
{
	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			x [i][j] *= v;

	return *this;
}

Matrix&	Matrix :: operator *= ( const Matrix& A )
{
	Matrix	res = *this;

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			float sum = 0;

			for ( int k = 0; k < 4; k++ )
				sum += res.x [i][k] * A.x [k][j];

			x [i][j] = sum;
		}

    return *this;
}

Matrix	operator + ( const Matrix& A, const Matrix& B )
{
	Matrix	res;

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			res.x [i][j] = A.x [i][j] + B.x [i][j];

	return res;
}

Matrix	operator - ( const Matrix& A, const Matrix& B )
{
	Matrix	res;

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			res.x [i][j] = A.x [i][j] - B.x [i][j];

	return res;
}

Matrix	operator * ( const Matrix& A, const Matrix& B )
{
	Matrix	res;

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
		{
			float sum = 0;

			for ( int k = 0; k < 4; k++ )
				sum += A.x [i][k] * B.x [k][j];

			res.x [i][j] = sum;
		}

	return res;
}

Matrix	operator * ( const Matrix& A, float v )
{
	Matrix	res;

	for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			res.x [i][j] = A.x [i][j] * v;

	return res;
}

Vector	operator * ( const Matrix& M, const Vector& v )
{
	Vector	res;

	res.x = M.x [0][0] * v.x + M.x [0][1] * v.y + M.x [0][2] * v.z + M.x [0][3];
	res.y = M.x [1][0] * v.x + M.x [1][1] * v.y + M.x [1][2] * v.z + M.x [1][3];
	res.z = M.x [2][0] * v.x + M.x [2][1] * v.y + M.x [2][2] * v.z + M.x [2][3];

	float	denom = M.x [3][0] * v.x + M.x [3][1] * v.y +  M.x [3][2] * v.z + M.x [3][3];

	if ( denom != 1.0 )
		res /= denom;

	return res;
}

//////////////////////// Derived classes /////////////////////////////

Matrix	Translate ( const Vector& Loc )
{
	Matrix	res ( 1 );

	res.x [0][3] = Loc.x;
	res.x [1][3] = Loc.y;
	res.x [2][3] = Loc.z;

	return res;
}

Matrix	Scale ( const Vector& v )
{
	Matrix	res ( 1 );

	res.x [0][0] = v.x;
	res.x [1][1] = v.y;
	res.x [2][2] = v.z;

	return res;
}

Matrix	RotateX ( float Angle )
{
	Matrix	res ( 1 );
	float  Cosine = cos ( Angle );
	float  Sine   = sin ( Angle );

	res.x [1][1] = Cosine;
	res.x [1][2] = -Sine;
	res.x [2][1] = Sine;
	res.x [2][2] = Cosine;

	return res;
}

Matrix	RotateY ( float Angle )
{
	Matrix	res ( 1 );
	float  Cosine = cos ( Angle );
	float  Sine   = sin ( Angle );

	res.x [0][0] = Cosine;
	res.x [0][2] = -Sine;
	res.x [2][0] = Sine;
	res.x [2][2] = Cosine;

	return res;
}

Matrix	RotateZ ( float Angle )
{
	Matrix	res ( 1 );
	float  Cosine = cos ( Angle );
	float  Sine   = sin ( Angle );

	res.x [0][0] = Cosine;
	res.x [0][1] = -Sine;
	res.x [1][0] = Sine;
	res.x [1][1] = Cosine;

	return res;
}

Matrix	Rotation ( const Vector& axis, float angle )
{
	Matrix	res ( 1 );
	float  Cosine = cos ( angle );
	float  Sine   = sin ( angle );

	res.x [0][0] = axis.x * axis.x + ( 1 - axis.x * axis.x ) * Cosine;
	res.x [1][0] = axis.x * axis.y * ( 1 - Cosine ) + axis.z * Sine;
	res.x [2][0] = axis.x * axis.z * ( 1 - Cosine ) - axis.y * Sine;
	res.x [3][0] = 0;

	res.x [0][1] = axis.x * axis.y * ( 1 - Cosine ) - axis.z * Sine;
	res.x [1][1] = axis.y * axis.y + ( 1 - axis.y * axis.y ) * Cosine;
	res.x [2][1] = axis.y * axis.z * ( 1 - Cosine ) + axis.x * Sine;
	res.x [3][1] = 0;

	res.x [0][2] = axis.x * axis.z * ( 1 - Cosine ) + axis.y * Sine;
	res.x [1][2] = axis.y * axis.z * ( 1 - Cosine ) - axis.x * Sine;
	res.x [2][2] = axis.z * axis.z + ( 1 - axis.z * axis.z ) * Cosine;
	res.x [3][2] = 0;

	res.x [0][3] = 0;
	res.x [1][3] = 0;
	res.x [2][3] = 0;
	res.x [3][3] = 1;

	return res;
}

Matrix	MirrorX ()
{
	Matrix	res ( 1 );

	res.x [0][0] = -1;

	return res;
}

Matrix	MirrorY ()
{
	Matrix	res ( 1 );

	res.x [1][1] = -1;

	return res;
}

Matrix	MirrorZ ()
{
	Matrix	res ( 1 );

	res.x [2][2] = -1;

	return res;
}
