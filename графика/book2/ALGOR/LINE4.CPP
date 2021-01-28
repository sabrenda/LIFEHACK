void	drawLine ( int xa, int ya, int xb, int yb, int color )
{
	double	k = ((double)(yb - ya))/(xb - xa);
	double	d = 2*k - 1;
	int	y = ya;

	putpixel ( xa, ya, color );

	for ( int x = xa + 1; x <= xb; x++ )
	{
		if ( d > 0 )
		{
			d += 2*k - 2;
			y++;
		}
		else
			d += 2*k;

		putpixel ( x, y, color );
	}
}
