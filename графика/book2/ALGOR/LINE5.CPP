void	drawLine ( int xa, int ya, int xb, int yb, int color )
{
	int	dx = xb - xa;
	int	dy = yb - ya;
	int	d1 = dy << 1;
	int	d2 = ( dy - dx ) << 1;
	int	d  = d1 - dx;

	putpixel ( xa, ya, color );

	for ( int x = xa; x <= xb; x++ )
	{
		if ( d > 0 )
		{
			d += d2;
			y ++;
		}
		else
			d += d1;

		putpixel ( x, y, color );
	}
}
