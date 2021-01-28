#ifndef	__JOYSTICK__
#define	__JOYSTICK__

#define	JOYPORT	0x201		// joystick port

#define	BUTTON_1_A	0x10	// joystick A, button 1
#define	BUTTON_2_A	0x20	// joystick A, button 2
#define	BUTTON_1_B	0x40	// joystick B, button 1
#define	BUTTON_2_B	0x80	// joystick B, button 2
#define	JOYSTICK_1_X	0x01	// joystick A, X axis
#define	JOYSTICK_1_Y	0x02	// joystick A, Y axis
#define	JOYSTICK_2_X	0x04	// joystick B, X axis
#define	JOYSTICK_2_Y	0x08	// joystick B, Y axis

				// global values
extern	unsigned joy1MinX, joy1MaxX, joy1MinY, joy1MaxY;
extern	unsigned joy2MinX, joy2MaxX, joy2MinY, joy2MaxY;

unsigned joystickButtons   ( char button );
unsigned joystickValue     ( char stick  );
unsigned joystickValueBIOS ( char stick  );
int	 joystickPresent   ( char stick );
#endif
