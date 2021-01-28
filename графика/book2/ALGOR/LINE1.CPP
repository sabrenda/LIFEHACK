
void	drawLine ( int xa, int ya, int xb, int yb, int color )
{
	double	k = ((double)(yb - ya))/(xb - xa);
	double	b = ya - k*xa;

	for ( int x = xa; x <= xb; x++ )
		putpixel ( x, (int)( k*x +b ), color );
}
