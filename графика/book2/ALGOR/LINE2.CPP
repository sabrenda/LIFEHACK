
void	drawLine ( int xa, int ya, int xb, int yb, int color )
{
	double	k = ((double)(yb - ya))/(xb - xa);
	double	b = ya - k*xa;
	double	y = ya;

	for ( int x = xa; x <= xb; x++, y += k )
		putpixel ( x, (int) y, color );
}
