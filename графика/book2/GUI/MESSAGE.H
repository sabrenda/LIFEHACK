//
// Basic message definitions & class MessageQueue defintion
// Author: Alex V. Boreskoff
//
#ifndef	__MESSAGE__
#define	__MESSAGE__

#include	<malloc.h>
#include	"mouse.h"

				// window messages
#define	WM_IDLE		0	// idle message, nothing happened
#define	WM_KEYDOWN	1
#define	WM_KEYUP	2
#define	WM_LBUTTONDOWN	3	// mouse messages, if m.wnd != NULL
#define	WM_RBUTTONDOWN	4	// it's captured by m.wnd message
#define	WM_LBUTTONUP	5
#define	WM_RBUTTONUP	6
#define	WM_DBLCLICK	7
#define	WM_TRIPLECLICK	8
#define	WM_MOUSEMOVE	9
#define	WM_COMMAND	10	// notification from one of the subviews or menu
#define	WM_RECEIVEFOCUS	11	// recieve focus notification
#define	WM_LOOSEFOCUS	12	// loose focus notification
#define	WM_CLOSE	13
#define	WM_HSCROLL	14	// perform horizontal scrolling (sent by scrollBar)
#define	WM_VSCROLL	15	// perform vertical scrolling (sent by scrollBar)
#define	WM_TIMER	16	// notifications from timer

class	View;

struct	Message
{
	View	 * view;
	int	   code;
	int        subCode;
	int	   param;
	MouseState mouse;
	int	   time;		// time in timer ticks
	int	   kbdFlags;		// keyboard flags (shift, alt, ctrl)
	void	 * data;		// pointer to some additional data

	int 	isMouseMessage () const
	{
		return code >= WM_LBUTTONDOWN && code <= WM_MOUSEMOVE;
	}

	int	isKeyboardMessage () const
	{
		return code == WM_KEYDOWN || code == WM_KEYUP;
	}
};

class	MessageQueue
{
	Message * queue;
	int	  putPtr;
	int	  getPtr;
	int	  size;

public:
	MessageQueue ( int queueSize )
	{
		queue  = new Message [size = queueSize];
		putPtr = 0;
		getPtr = 0;
	}
	~MessageQueue ()
	{
		delete queue;
	}

	void	postMessage ( View * view, int code, int subCode = 0, int param = 0, void * data = NULL );
	int	getMessage  ( Message& m );

	int	isEmpty () const
	{
		return putPtr == getPtr;
	}
};

int	sendMessage   ( View * view, int code, int subCode = 0, int param = 0, void * data = NULL );
void	messageBeep   ();
int	getTimerTicks ();
int	getKbdFlags   ();
void	log           ( char *, ... );

extern	MessageQueue	messageQueue;

#endif
