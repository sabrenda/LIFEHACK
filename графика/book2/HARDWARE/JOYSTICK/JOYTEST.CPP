#include	<bios.h>
#include	<stdio.h>
#include	"joystick.h"

main ()
{
	if ( !joystickPresent ( 1 ) )
	{
		printf ( "\nJoystick 1 not found." );
		return 0;
	}
	else
		printf ( "\nJoystick 1 found." );

				// now calibrate joystick
	joy1MinX = 0xFFFF;
	joy1MaxX = 0;
	joy1MinY = 0xFFFF;
	joy1MaxY = 0;

	while ( ! joystickButtons ( BUTTON_1_A | BUTTON_1_B ) )
	{
		unsigned x = joystickValueBIOS ( JOYSTICK_1_X );
		unsigned y = joystickValueBIOS ( JOYSTICK_1_Y );

		if ( x < joy1MinX )
			joy1MinX = x;

		if ( x > joy1MaxX )
			joy1MaxX = x;

		if ( y < joy1MinY )
			joy1MinY = y;

		if ( y > joy1MaxY )
			joy1MaxY = y;
	}

	printf ( "\nCalibration complete.\nxMin = %u. xMax = %u. yMin = %u. yMax = %u",
		 joy1MinX, joy1MaxX, joy1MinY, joy1MaxY );
}
