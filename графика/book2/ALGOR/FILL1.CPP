void	PixelFill ( int x, int y, int BorderColor, int color )
{
	int	c = getpixel ( x, y );

	if ( ( c != BorderColor ) && ( c != color ) )
	{
		putpixel  ( x, y, color );
		PixelFill ( x - 1, y, BorderColor, color );
		PixelFill ( x + 1, y, BorderColor, color );
		PixelFill ( x, y - 1, BorderColor, color );
		PixelFill ( x, y + 1, BorderColor, color );
	}
}
