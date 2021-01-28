#ifndef	__SPRITE__
#define	__SPRITE__

#define	MAX_STAGES	20
#define	MAX_HEIGHT	100
#define	TRANSP_COLOR	0xFF

class	Sprite
{
public:
	int	x, y;		// location of upper-left corner
	int	width, height;	// size of single image
	int	stageCount;	// number of stages
	int	curStage;	// current stage
	char  * underImage;	// place to store image under the sprite
	char  * lineStart [MAX_STAGES*MAX_HEIGHT];

	Sprite ( int, int, char *, ... );
	~Sprite ()
	{
		free ( underImage );
	}

	void	set ( int ax, int ay )
	{
		x = ax;
		y = ay;
	}

	void	draw         ();
	void	storeUnder   ();
	void	restoreUnder ();
};

inline	int	min ( int x, int y )
{
	return x < y ? x : y;
}

inline	int	max ( int x, int y )
{
	return x > y ? x : y;
}

extern	char far * videoAddr;
extern	int        screenWidth;
extern	int	   screenHeight;
extern	int	   orgX;
extern	int	   orgY;

#endif