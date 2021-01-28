#include	<windows.h>
#include	<gl\gl.h>
#include	<gl\glu.h>

LONG WINAPI wndProc ( HWND, UINT, WPARAM, LPARAM );
void	    setDCPixelFormat ( HDC );
void	    initializeRC ();
void	    drawBox ( GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat );
void	    drawScene ( HDC, int );

int	WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow )
{
	static char appName [] = "Robot Arm";
	WNDCLASS    wc;
	HWND	    hWnd;
	MSG	    msg;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC) wndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon ( NULL, IDI_APPLICATION );
	wc.hCursor       = LoadCursor ( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = appName;

	RegisterClass ( &wc );

	hWnd = CreateWindow ( appName, appName,
			      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			      CW_USEDEFAULT, HWND_DESKTOP, NULL, hInstance, NULL );

	ShowWindow ( hWnd, cmdShow );
	UpdateWindow ( hWnd );

	while ( GetMessage ( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage ( &msg );
		DispatchMessage  ( &msg );
	}

	return msg.wParam;
}

LONG WINAPI wndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	static HDC   hdc;
	static HGLRC hrc;
	PAINTSTRUCT  ps;
	GLdouble     aspect;
	GLsizei      width, height;
	static int   angle = 0;
	static BOOL  up = TRUE;
	static UINT  timer;

	switch ( msg )
	{
		case WM_CREATE:
			hdc = GetDC ( hWnd );
			setDCPixelFormat ( hdc );
			hrc = wglCreateContext ( hdc );
			wglMakeCurrent ( hdc, hrc );
			initializeRC ();
			timer = SetTimer ( hWnd, 1, 50, NULL );
			 
			return 0;

		case WM_SIZE:
			width  = (GLsizei) LOWORD ( lParam );
			height = (GLsizei) HIWORD ( lParam );
			aspect = (GLdouble) width / (GLdouble) height;

			glMatrixMode ( GL_PROJECTION );
			glLoadIdentity ();
			gluPerspective ( 30.0, aspect, 1.0, 100.0 );

			glViewport ( 0, 0, width, height );

			return 0;

		case WM_PAINT:
			BeginPaint ( hWnd, &ps );
			drawScene  ( hdc, angle );
			EndPaint   ( hWnd, &ps );

			return 0;

		case WM_TIMER:
			if ( up )
			{
				angle += 2;
				if ( angle == 90 )
					up = FALSE;
			}
			else
			{
				angle -= 2;
				if ( angle == 0 )
					up = TRUE;
			}

			InvalidateRect ( hWnd, NULL, FALSE );

			return 0;
		case WM_DESTROY:
			wglMakeCurrent ( NULL, NULL );
			wglDeleteContext ( hrc );

			KillTimer ( hWnd, timer );
			PostQuitMessage ( 0 );

			return 0;


	}

	return DefWindowProc ( hWnd, msg, wParam, lParam );
}

void	setDCPixelFormat ( HDC hdc )
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof ( PIXELFORMATDESCRIPTOR ),			// sizeof this structure
		1,											// version number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,	// flags
		PFD_TYPE_RGBA,								// RGBA pixel values
		24,											// 24-bit color
		0, 0, 0, 0, 0, 0,							// don't care about these
		0, 0, 										// no alpha buffer
		0, 0, 0, 0, 0,								// no accumulation buffer
		32,											// 32-bit depth buffer
		0, 											// no stencil buffer
		0,											// no auxiliary buffers
		PFD_MAIN_PLANE,								// layer type
		0,											// reserved (must be 0)
		0, 0, 0										// no layer masks
	};
	int	pixelFormat;

	pixelFormat = ChoosePixelFormat ( hdc, &pfd );
	SetPixelFormat ( hdc, pixelFormat, &pfd );
	DescribePixelFormat ( hdc, pixelFormat,
			      sizeof (PIXELFORMATDESCRIPTOR), &pfd );

	if ( pfd.dwFlags & PFD_NEED_PALETTE )
		MessageBox ( NULL, "Wrong video mode", "Attention", MB_OK );
}

void	initializeRC ()
{
	GLfloat	lightAmbient  [] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat lightDiffuse  [] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat	lightSpecular [] = { 0.0, 0.0, 0.0, 1.0 };

	glFrontFace  ( GL_CCW );
	glCullFace   ( GL_BACK );
	glEnable     ( GL_CULL_FACE );

	glDepthFunc  ( GL_LEQUAL );
	glEnable     ( GL_DEPTH_TEST );
	glClearColor ( 0.0, 0.0, 0.0, 0.0 );

	glLightfv ( GL_LIGHT0, GL_AMBIENT, lightAmbient );
	glLightfv ( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
	glLightfv ( GL_LIGHT0, GL_SPECULAR, lightSpecular );
	glEnable  ( GL_LIGHTING );
	glEnable  ( GL_LIGHT0 );
}

void	drawScene ( HDC hDC, int angle )
{
	GLfloat blue [] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat yellow [] = { 1.0, 1.0, 0.0, 1.0 };

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();
	glTranslatef ( 0.0, 0.0, -64.0 );
	glRotatef ( 30.0, 1.0, 0.0, 0.0 );
	glRotatef ( 30.0, 0.0, 1.0, 0.0 );
					// draw the block anchoring the arm
	glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow );
	drawBox ( 1.0, 3.0, -2.0, 2.0, -2.0, 2.0 );
	drawBox ( -3.0, -1.0, -2.0, 2.0, -2.0, 2.0 );

					// rotate the coordinate system and
					// draw the arm's base member
	glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue );
	glRotatef ( (GLfloat) angle, 1.0, 0.0, 0.0 );
	drawBox ( -1.0, 1.0, -1.0, 1.0, -5.0, 5.0 );

					// translate the coordinate system to
					// the end of base member, rotate it,
					// and draw the second member
	glTranslatef ( 0.0, 0.0, -5.0 );
	glRotatef ( -(GLfloat) angle / 2.0, 1.0, 0.0, 0.0 );
	drawBox ( -1.0, 1.0, -1.0, 1.0, -10.0, 0.0 );

					// translate and rotate coordinate
					// system again and draw arm's
					// third member
	glTranslatef ( 0.0, 0.0, -5.0 );
	glRotatef ( -(GLfloat) angle / 2.0, 1.0, 0.0, 0.0 );
	drawBox ( -1.0, 1.0, -1.0, 1.0, -10.0, 0.0 );

	glFlush ();			// render the scene to pixel buffer
}
