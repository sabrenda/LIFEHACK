#ifdef  __WATCOMC__
#include        <i86.h>
#endif

#include        <bios.h>
#include        <dos.h>
#include        <malloc.h>
#include        <math.h>
#include        <stdio.h>
#include        <time.h>

#include        "wad.h"
#include        "texdict.h"

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200
#define V_MIN           20
#define HSCALE          160l
#define VSCALE          160l

#define FLOOR_COLOR             8
#define CEILING_COLOR           7
#define WALL_COLOR              1
#define LOWER_WALL_COLOR        2
#define UPPER_WALL_COLOR        3

#define       ESC     0x011b
#define        UP     0x4800
#define      DOWN     0x5000
#define      LEFT     0x4b00
#define     RIGHT     0x4d00
#define     CtrlLeft  0x7300
#define     CtrlRight 0x7400

struct  VertexCache
{
        int     u;
        int     v;
        int     c;
        int     frame;
};

struct DPMIDosPtr
{
        unsigned short segment;
        unsigned short selector;
};

//////////////////////// Static Data /////////////////////////////
struct
{
        unsigned long   edi;
        unsigned long   esi;
        unsigned long   ebp;
        unsigned long   esp;
        unsigned long   ebx;
        unsigned long   edx;
        unsigned long   ecx;
        unsigned long   eax;
        unsigned short  flags;
        unsigned short  es, ds, fs, gs, ip, cs, sp, ss;
} rmRegs;

REGS      regs;
SREGS     sregs;

int       locX;                         // viewer's location
int       locY;
int       locZ;                         // viewer height
Angle     angle;                        // viewing direction
SSector * curSector;                    // ssector, viewer located in
long      totalFrames = 0;

int       topLine [SCREEN_WIDTH];       // top horizon line
int       bottomLine [SCREEN_WIDTH];    // bottom horizon line
int       colsLeft;                     // # on unfinished columns
char      colFilled [SCREEN_WIDTH];     // whether column is already filled

VertexCache     * vertexCache;
WadFile           wad ( "E:\\GAMES\\DOOM.ULT\\DOOM.WAD" );
TextureDictionary texDict ( &wad );

//////////////////////////////////////////////////////////////////

void    drawView       ();
void    drawNode       ( unsigned node );
void    drawSubSector  ( int s );
void    drawSeg        ( Segment * seg );
void    drawSimpleWall ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed t1, Fixed t2, int v1, int v2 );
void    drawLowerWall  ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed t1, Fixed t2, int v1, int v2 );
void    drawUpperWall  ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed t1, Fixed t2, int v1, int v2 );
void    setVideoMode   ( int mode );
void    drawVertLine   ( int col, int top, int bottom, int color );

///////////////////////////////////////////////////////////////////

Fixed   fixedDist ( int u1, int v1, int u2, int v2, Angle angle )
{
        int     du = u2 - u1;
        int     dv = v2 - v1;
        Fixed   dist;

        if ( abs ( du ) > abs ( dv ) )
                dist = du * invCosine ( angle );
        else
                dist = dv * invSine ( angle );

        return dist;
}

inline  int     viewerOnRight ( int node )
{
        Node    * n = &nodes [node];

        return n->dy * (long)( locX - n->x ) >=
               n->dx * (long)( locY - n->y );
}

inline  int     frontFacing ( int from, int to )
{
        return (long)( vertices [from].x - vertices [to].x ) * (long)( locY - vertices [to].y ) >=
               (long)( vertices [from].y - vertices [to].y ) * (long)( locX - vertices [to].x );
}

void    RMVideoInt ()                   // execute real-mode video interrupt
{
        segread ( &sregs );

        regs.w.ax  = 0x0300;
        regs.w.bx  = 0x0010;
        regs.w.cx  = 0;
        regs.x.edi = FP_OFF ( &rmRegs );
        sregs.es   = FP_SEG ( &rmRegs );

        int386x ( 0x31, &regs, &regs, &sregs );
}

void    DPMIAllocDosMem ( DPMIDosPtr& ptr, int paras )
{
        regs.w.ax = 0x0100;             // allocate DOS memory
        regs.w.bx = paras;              // # of memory in paragraphs

        int386 ( 0x31, &regs, &regs );

        ptr.segment  = regs.w.ax;       // real-mode segment of the block
        ptr.selector = regs.w.dx;       // selector for the allocated block
}

void    DPMIFreeDosMem ( DPMIDosPtr& ptr )
{
        regs.w.ax = 0x0101;             // free DOS memory
        regs.w.dx = ptr.selector;

        int386 ( 0x31, &regs, &regs );
}

void    setVideoMode ( int mode )
{
        _asm {
                mov     ax, word ptr mode
                int     10h
        }
}

void    setPalette ( RGB * palette )
{
        DPMIDosPtr     palPool;

        DPMIAllocDosMem ( palPool, 3*256 / 16 );

        RGB * tmpPal = (RGB *) (palPool.segment * 16);

        for ( int i = 0; i < 256; i++ )         // convert from 8-bit to 6-bit values
        {
                tmpPal [i].red   = palette [i].red   >> 2;
                tmpPal [i].green = palette [i].green >> 2;
                tmpPal [i].blue  = palette [i].blue  >> 2;
        }

        rmRegs.eax = 0x1012;                    // set palette
        rmRegs.ebx = 0;                         // starting index
        rmRegs.ecx = 256;                       // # of pal entries
        rmRegs.es  = palPool.segment;
        rmRegs.edx = 0;

        RMVideoInt ();                          // execute video interrupt

        DPMIFreeDosMem ( palPool );             // free memory
}

void    drawVertLine ( int col, int top, int bottom, int color )
{
        char * vptr = col + 320 * top + (char *)0xA0000l;

        while ( top <= bottom )
        {
                * vptr = color;
                vptr  += 320;
                top++;
        }
}

void    drawTexturedLine ( int col, int top, int bottom, Texture * tex, int textureOffset, Fixed texY, Fixed dTexY )
{
        char  * vptr = col + 320 * top + (char *)0xA0000l;
        Fixed   h    = int2Fixed ( tex -> height );

        while ( texY < 0 )
                texY += h;

        while ( top <= bottom )
        {
                if ( texY >= h )
                        texY -= h;

                * vptr = tex -> data [textureOffset + tex -> width * (fixed2Int (texY) & 127)];
                vptr  += SCREEN_WIDTH;
                texY  += dTexY;
                top++;
        }
}

main ()
{
        int     done = 0;

        initFixMath      ();
        wad.loadLevel    ( 1, 1 );
        wad.loadPlayPal  ();
        wad.loadColorMap ();

        vertexCache = new VertexCache [numVertices];

                                        // set viewer to start of level
        for ( int i = 0; i < numThings; i++ )
                if ( things [i].type == 1 )
                {
                        locX  = things [i].x;
                        locY  = things [i].y;
                        locZ  = 0;
                        angle = (Angle) ( ( 0x4000l * things [i].angle ) / 90 );

			break;
                }
                                        // load all textures
        for ( i = 0; i < numSides; i++ )
        {
                texDict.getTexture ( sides [i].main_tx );
                texDict.getTexture ( sides [i].lower_tx );
                texDict.getTexture ( sides [i].upper_tx );
        }

        setVideoMode ( 0x13 );
        setPalette   ( playPal [0] );

        clock_t start = clock ();

        while ( !done )
        {
                drawView ();
                if ( _bios_keybrd ( _KEYBRD_READY ) )
                        switch ( _bios_keybrd ( _KEYBRD_READ ) )
			{
                                case UP:
                                        locX += fixed2Int ( 8l * cosine ( angle ) );
                                        locY += fixed2Int ( 8l * sine   ( angle ) );
                                        break;

                                case DOWN:
                                        locX -= fixed2Int ( 8l * cosine ( angle ) );
                                        locY -= fixed2Int ( 8l * sine   ( angle ) );
                                        break;

                                case LEFT:
                                        angle += 1024;
                                        break;

                                case RIGHT:
                                        angle -= 1024;
                                        break;

                                case CtrlLeft:
					locX += fixed2Int ( 8l * cosine (angle+ANGLE_90) );
                                        locY += fixed2Int ( 8l * sine   (angle+ANGLE_90) );
                                        break;

                                case CtrlRight:
                                        locX += fixed2Int ( 8l * cosine (angle-ANGLE_90) );
                                        locY += fixed2Int ( 8l * sine   (angle-ANGLE_90) );
                                        break;

                                case ESC:
                                        done = 1;
                                        break;
                        }
        }

        float   totalTime = ( clock () - start ) / CLK_TCK;

        setVideoMode ( 0x03 );

        printf ( "\nFrames rendered    : %7ld",  totalFrames );
	printf ( "\nTotal time ( sec ) : %7.2f", totalTime );
	printf ( "\nFPS                : %7.2f", totalFrames / totalTime );
}

void    drawView ()
{
        curSector = NULL;                               // now unknown
        colsLeft  = SCREEN_WIDTH;

        for ( int i = 0; i < SCREEN_WIDTH; i++ )        // reset horizon lines
        {
                colFilled  [i] = 0;
                topLine    [i] = 0;
                bottomLine [i] = SCREEN_HEIGHT - 1;
        }

        drawNode ( numNodes - 1 );                      // start drawing with root
        totalFrames++;
}

// check whether the bounding box is in the viewing frustrum
int     boxInVCone ( BBox& box )
{
        Angle   leftAngle = angle + 0x2000;

        if ( leftAngle < ANGLE_90 )
        {
                if ( box.x2 > locX )
                        return 1;
        }
        else
        if ( leftAngle < ANGLE_180 )
        {
                if ( box.y2 > locY )
                        return 1;
        }
        else
        if ( leftAngle < ANGLE_270 )
        {
                if ( box.x1 < locX )
                        return 1;
        }
        else
        {
                if ( box.y1 < locY )
                        return 1;
        }

        return 0;
}

void    drawNode ( unsigned node )
{
        if ( node & 0x8000 )            // if it's a ssector => draw it
                drawSubSector ( node & 0x7FFF );
        else
        if ( viewerOnRight ( node ) )   // otherwise draw tree in front-to-back order
        {
                if ( colsLeft > 0 && boxInVCone ( nodes [node].rightBox ) )
                        drawNode ( nodes [node].rightNode );

                if ( colsLeft > 0 && boxInVCone ( nodes [node].leftBox ) )
                        drawNode ( nodes [node].leftNode );
        }
        else
        {
                if ( colsLeft > 0 && boxInVCone ( nodes [node].leftBox ) )
                        drawNode ( nodes [node].leftNode );

                if ( colsLeft > 0 && boxInVCone ( nodes [node].rightBox ) )
                        drawNode ( nodes [node].rightNode );
        }
}

void    drawSubSector ( int s )
{
        int       firstSeg = subSectors [s].firstSeg;   // get segments, forming ssector
        int       numSegs  = subSectors [s].numSegs;
        Segment * seg      = &segs [firstSeg];          // pointer to current segment

        if ( curSector == NULL )                        // 1st ssector to be drawn using
        {                                               // front-to-back will contain viewer
                SideDef * side = &sides [lines [seg->lineDef].sideDefs [seg->lineSide]];

                curSector = &subSectors [s];            // so remember it & adjust height
                locZ = 40 + sectors [side -> sector].floorHeight;
        }

        for ( int i = 0; i < numSegs; i++, seg++ )      // draw all front-facing segments
                if ( frontFacing ( seg -> from, seg -> to ) )
                        drawSeg ( seg );
}

void    drawSeg ( Segment * seg )
{
        if ( vertexCache [seg -> from].frame != totalFrames )   // vertex has not been projected this frame
        {
                int           x1 = vertices [seg->from].x - locX;
                int           y1 = vertices [seg->from].y - locY;
                VertexCache * ptr = &vertexCache [seg -> from];

                ptr -> frame = totalFrames;
                ptr -> u     = fixed2Int ( -x1*sine (angle)   + y1*cosine (angle) );
                ptr -> v     = fixed2Int (  x1*cosine (angle) + y1*sine (angle) );

                if ( ptr -> v >= V_MIN )
                        ptr -> c = (int) (160l - (ptr -> u * HSCALE) / ptr -> v);
        }

        if ( vertexCache [seg -> to].frame != totalFrames )
        {
                int           x2  = vertices [seg->to].x - locX;
                int           y2  = vertices [seg->to].y - locY;
                VertexCache * ptr = &vertexCache [seg -> to];

                ptr -> frame = totalFrames;
                ptr -> u     = fixed2Int ( -x2*sine (angle)   + y2*cosine (angle) );
                ptr -> v     = fixed2Int (  x2*cosine (angle) + y2*sine (angle) );

                if ( ptr -> v >= V_MIN )
                        ptr -> c = (int) (160l - (ptr -> u * HSCALE) / ptr -> v);
        }
                                                        // convert to local coords
        int     v1 = vertexCache [seg -> from].v;
        int     v2 = vertexCache [seg -> to].v;

        if ( v1 < V_MIN && v2 < V_MIN )                 // segment behind the image plane
                return;

	int     u1 = vertexCache [seg -> from].u;
	int     u2 = vertexCache [seg -> to].u;
	Fixed   t1 = int2Fixed ( seg -> lineOffset );
	Fixed   t2 = t1 + fixedDist ( u1, v1, u2, v2, ANGLE_90 - seg -> angle + angle );

	if ( v1 < V_MIN )                               // clip 1st point
	{
		int oldU = u1;
		int oldV = v1;

		u1 += (int)( ( (long)( V_MIN - v1 ) * (long)( u2 - u1 ) ) / ( v2 - v1 ) );
		v1  = V_MIN;
		t1  = int2Fixed ( seg -> lineOffset ) + fixedDist ( oldU, oldV, u1, v1, ANGLE_90 - seg->angle + angle);
	}

	if ( v2 < V_MIN )                               // clip 2nd point
	{
		u2 += (int)( ( (long)( V_MIN - v2 ) * (long)( u2 - u1 ) ) / ( v2 - v1 ) );
		v2  = V_MIN;
                t2  = t1 + fixedDist ( u1, v1, u2, v2, ANGLE_90 - seg -> angle + angle );
	}
                                                        // project vertices to screen
        int     c1 = (int) ( 160l - ( u1 * HSCALE ) / v1 );
        int     c2 = (int) ( 160l - ( u2 * HSCALE ) / v2 );
							// reject invisible segs
	if ( c1 >= SCREEN_WIDTH || c2 < 0 || c2 <= c1 )
                return;

        int     canExit = 1;                            // check whether any
							// column in range [c1, c2]
        for ( register int i = c1; i < c2; i++ )                        // remains unfilled
                if ( !colFilled [i] )
                {
                        canExit = 0;
                        break;
                }

        if ( canExit )
                return;

	SideDef * side = &sides [ lines [seg -> lineDef].sideDefs [seg -> lineSide]];

        int     floorHeight   = sectors [side->sector].floorHeight;
	int     ceilingHeight = sectors [side->sector].ceilingHeight;

	if ( side -> main_tx [0] != '-' )               // simple wall from floor to ceiling
							// no lower or upper
		drawSimpleWall ( texDict.getTexture ( side -> main_tx ), side, c1, c2, locZ - floorHeight, locZ - ceilingHeight, t1, t2, v1, v2 );
	else
	{                                               // otherwise we'll need adjacent sector heights
		SideDef * otherSide = &sides [lines [seg -> lineDef].sideDefs [seg -> lineSide ^ 1]];

							// draw lower part
		if ( side -> lower_tx [0] != '-' )
			drawLowerWall ( texDict.getTexture ( side -> lower_tx ), side, c1, c2, locZ - floorHeight, locZ - sectors [otherSide -> sector].floorHeight, t1, t2, v1, v2 );
		else
			drawLowerWall ( NULL, side, c1, c2, locZ - floorHeight, locZ - floorHeight, t1, t2, v1, v2 );

							// draw upper part
		if ( side -> upper_tx [0] != '-' )
			drawUpperWall ( texDict.getTexture ( side -> upper_tx ), side, c1, c2, sectors [otherSide->sector].ceilingHeight - locZ, ceilingHeight - locZ, t1, t2, v1, v2 );
		else
			drawUpperWall ( NULL, side, c1, c2, ceilingHeight - locZ, ceilingHeight - locZ, t1, t2, v1, v2 );

        }
}

// Draw walls using horizon lines - only parts between horizon lines are drawn

void    drawSimpleWall ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed tOffs1, Fixed tOffs2, int v1, int v2 )
{
	Fixed   b1     = int2Fixed ( 100l + ( bottomHeight * VSCALE ) / v1 );
	Fixed   b2     = int2Fixed ( 100l + ( bottomHeight * VSCALE ) / v2 );
	Fixed   t1     = int2Fixed ( 100l + ( topHeight * VSCALE ) / v1 );
	Fixed   t2     = int2Fixed ( 100l + ( topHeight * VSCALE ) / v2 );
	Fixed   db     = ( b2 - b1 ) / ( col2 - col1 );
	Fixed   dt     = ( t2 - t1 ) / ( col2 - col1 );
	Fixed   invV1  = (ONE<<5) / v1;
	Fixed   invV2  = (ONE<<5) / v2;
	Fixed   tInvV1 = (tOffs1<<5) / v1;
	Fixed   tInvV2 = (tOffs2<<5) / v2;
        Fixed   dInvV  = (invV2 - invV1) / (col2 - col1);
        Fixed   dtInvV = (tInvV2 - tInvV1 ) / (col2 - col1);
        Fixed   invV   = invV1;
        Fixed   tInvV  = tInvV1;
	Fixed   texY;
	Fixed   dTexY;
	int     top, bottom;
	int     newTop;
	int     tOffset;

	if ( col1 < 0 )         // clip invisible part
	{
		b1    -= col1 * db;
		t1    -= col1 * dt;
		invV  -= col1 * dInvV;
		tInvV -= col1 * dtInvV;
		col1   = 0;
	}

	if ( col2 > 320 )
		col2 = 320;

	for ( int col = col1; col < col2; col++ )
	{
		top     = fixed2Int ( t1 );
		bottom  = fixed2Int ( b1 );
		newTop  = top;
		dTexY   = int2Fixed (bottomHeight - topHeight + 1) / ( bottom - top + 1 );
		texY    = int2Fixed (side -> yOffset) - (bottom - top + 1) * dTexY;
		tOffset = tInvV / invV;
		t1      += dt;
		b1      += db;
		invV    += dInvV;
		tInvV   += dtInvV;

		if ( tex != NULL )
			tOffset &= tex -> width - 1;

		if ( topLine [col] > bottomLine [col] ) // nothing to draw here
			continue;

                if ( top > bottomLine [col] )
                        top = bottomLine [col];

                if ( bottom < topLine [col] )
                        bottom = topLine [col];

                if ( top >= topLine [col] )             // draw ceiling
		{
                        drawVertLine ( col, topLine [col], top, CEILING_COLOR );
                        topLine [col] = ++top;
                }
                else                                    // otherwise correct to
                        top = topLine [col];            // draw only visible part

		if ( bottom <= bottomLine [col] )       // draw floor
                {
                        drawVertLine ( col, bottom, bottomLine [col], FLOOR_COLOR );
                        bottomLine [col] = --bottom;
		}
                else
                        bottom = bottomLine [col];

                topLine [col]    = bottom + 1;
                bottomLine [col] = top - 1;
                                                        // now draw visible part of wall
		texY -= (newTop-top)*dTexY;

		if ( tex != NULL )
                        drawTexturedLine ( col, top, bottom, tex, tOffset, texY, dTexY );
                else
                        drawVertLine ( col, top, bottom, WALL_COLOR );

                colFilled [col] = 0xFF;
                colsLeft--;
	}
}

void    drawLowerWall ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed tOffs1, Fixed tOffs2, int v1, int v2 )
{
        Fixed   b1     = int2Fixed ( 100l + ( bottomHeight * VSCALE ) / v1 );
        Fixed   b2     = int2Fixed ( 100l + ( bottomHeight * VSCALE ) / v2 );
        Fixed   t1     = int2Fixed ( 100l + ( topHeight * VSCALE ) / v1 );
        Fixed   t2     = int2Fixed ( 100l + ( topHeight * VSCALE ) / v2 );
        Fixed   db     = ( b2 - b1 ) / ( col2 - col1 );
        Fixed   dt     = ( t2 - t1 ) / ( col2 - col1 );
	Fixed   invV1  = (ONE<<5) / v1;
	Fixed   invV2  = (ONE<<5) / v2;
	Fixed   tInvV1 = (tOffs1<<5) / v1;
	Fixed   tInvV2 = (tOffs2<<5) / v2;
        Fixed   dInvV  = (invV2 - invV1) / (col2 - col1);
        Fixed   dtInvV = (tInvV2 - tInvV1 ) / (col2 - col1);
        Fixed   invV   = invV1;
        Fixed   tInvV  = tInvV1;
        Fixed   texY;
        Fixed   dTexY;
        int     top, bottom;
        int     newTop;
        int     tOffset;

        if ( col1 < 0 )         // clip invisible part
        {
                b1    -= col1 * db;
                t1    -= col1 * dt;
                invV  -= col1 * dInvV;
                tInvV -= col1 * dtInvV;
                col1 = 0;
        }

        if ( col2 > 320 )
                col2 = 320;

        for ( int col = col1; col < col2; col++ )
        {
                top    = fixed2Int ( t1 );
                bottom = fixed2Int ( b1 );
                newTop  = top;
                dTexY   = int2Fixed (bottomHeight - topHeight + 1) / ( bottom - top + 1 );
                texY    = int2Fixed (side -> yOffset) - (bottom - top + 1) * dTexY;
                tOffset = tInvV / invV;
                t1      += dt;
                b1      += db;
                invV    += dInvV;
                tInvV   += dtInvV;

                if ( tex != NULL )
                        tOffset &= tex -> width - 1;

		if ( topLine [col] > bottomLine [col] )
                        continue;

                if ( top < topLine [col] )
                        top = topLine [col];            // adjust top

                if ( bottom < topLine [col] )
                        bottom = topLine [col];

                if ( bottom <= bottomLine [col] )       // draw floor
                        drawVertLine ( col, bottom, bottomLine [col]++, FLOOR_COLOR );
                else
                        bottom = bottomLine [col];

                if ( top < bottom )                     // now draw visible part of wall
                {
                        texY -= (newTop-top)*dTexY;

                        if ( tex != NULL )
                                drawTexturedLine ( col, top, bottom, tex, tOffset, texY, dTexY );
			else
                                drawVertLine ( col, top, bottom, LOWER_WALL_COLOR );

                        if ( top < bottomLine [col] )
                                bottomLine [col] = top - 1;
                }
                else
                        if ( bottom < bottomLine [col] )
                                bottomLine [col] = bottom - 1;

                if ( topLine [col] > bottomLine [col] )
                {
                        colsLeft--;
                        colFilled [col] = 0xFF;
                }
        }
}

void    drawUpperWall ( Texture * tex, SideDef * side, int col1, int col2, int bottomHeight, int topHeight, Fixed tOffs1, Fixed tOffs2, int v1, int v2 )
{
	Fixed   b1     = int2Fixed ( 100l - ( bottomHeight * VSCALE ) / v1 );
	Fixed   b2     = int2Fixed ( 100l - ( bottomHeight * VSCALE ) / v2 );
        Fixed   t1     = int2Fixed ( 100l - ( topHeight * VSCALE ) / v1 );
        Fixed   t2     = int2Fixed ( 100l - ( topHeight * VSCALE ) / v2 );
        Fixed   db     = ( b2 - b1 ) / ( col2 - col1 );
	Fixed   dt     = ( t2 - t1 ) / ( col2 - col1 );
	Fixed   invV1  = (ONE<<5) / v1;
	Fixed   invV2  = (ONE<<5) / v2;
	Fixed   tInvV1 = (tOffs1<<5) / v1;
	Fixed   tInvV2 = (tOffs2<<5) / v2;
        Fixed   dInvV  = (invV2 - invV1) / (col2 - col1);
	Fixed   dtInvV = (tInvV2 - tInvV1) / (col2 - col1);
        Fixed   invV   = invV1;
        Fixed   tInvV  = tInvV1;
        Fixed   texY;
        Fixed   dTexY;
        int     top, bottom;
        int     newTop;
	int     tOffset;

	if ( col1 < 0 )         // clip invisible part
        {
                b1    -= col1 * db;
                t1    -= col1 * dt;
                invV  -= col1 * dInvV;
                tInvV -= col1 * dtInvV;
		col1 = 0;
        }

        if ( col2 > 320 )
                col2 = 320;

	for ( int col = col1; col < col2; col++ )
	{
		top     = fixed2Int ( t1 );
		bottom  = fixed2Int ( b1 );
		newTop  = top;
		dTexY   = int2Fixed (bottomHeight - topHeight + 1) / ( bottom - top + 1 );
		texY    = int2Fixed (side -> yOffset) - (bottom - top + 1) * dTexY;
		tOffset = tInvV / invV;
		t1      += dt;
		b1      += db;
		invV    += dInvV;
		tInvV   += dtInvV;

		if ( tex != NULL )
			tOffset &= tex -> width - 1;

		if ( topLine [col] > bottomLine [col] )
			continue;

		if ( top > bottomLine [col] )
			top = bottomLine [col];

		if ( top >= topLine [col] )             // draw ceiling
                        drawVertLine ( col, topLine [col]++, top, CEILING_COLOR );
                else                                    // otherwise correct to
                        top = topLine [col];            // draw only visible part

                if ( bottom > bottomLine [col] )
                        bottom = bottomLine [col];

                if ( top < bottom )                     // now draw visible part of wall
                {
                        texY -= (newTop-top)*dTexY;

                        if ( tex != NULL )
                                drawTexturedLine ( col, top, bottom, tex, tOffset, texY, dTexY );
                        else
                                drawVertLine ( col, top, bottom, UPPER_WALL_COLOR );

                        if ( bottom > topLine [col] )
                                topLine [col] = bottom + 1;
                }
                else
                        if ( top > topLine [col] )
                                topLine [col] = top + 1;

                if ( topLine [col] > bottomLine [col] )
                {
                        colsLeft--;
                        colFilled [col] = 0xFF;
                }
        }
}
