
int	FindChipsTech ()	// check for Chips & Tech SuperVGA
{
	asm {
		mov		ax,5F00h	// Get controller info servic
		xor		bx,bx
		int		10h
	}

	if ( _AL != 0x5F )
		return 0;

	int	chipset = _BL;
	int	memory  = _BH;
					// now find chipset
	switch ( _BL >> 4 )
	{
		case 0:			// 82c451
		case 1:			// 82c452
		case 2:			// 82c455
		case 4:			// 82c456
		default:		// 82c453
	}

	switch ( memory )
	{
		case 0:
			memory = 256;
			break;

		case 1:
			memory = 512;
			break;

		case 2:
			memory = 1024;
			break;

		default:
			memory = 2048;
			break;
	}

	return chipset;
}

int	FindParadise ()			// check for Paradise SuperVGA
{
	char	tmp = ReadReg ( 0x3CE, 0x0F );
	int	res = 0;

	WriteReg ( 0x3CE, 0x0F, ReadReg ( 0x3CE, 0x0F ) & 0x17 );

	if ( !TestPort ( 0x3CE, 9, 0x7F ) )
	{
		WriteReg ( 0x3CE, 0x0F, 5 );
		if ( TestPort ( 0x3CE, 9, 0x7F ) )
		{
			tmp = ReadReg ( EGA_CRTC, 0x29 );
			WriteReg ( EGA_CRTC, 0x29, ( ReadReg ( EGA_CRTC, 0x29 ) & 0x8F ) | 0x85 );
			if ( TestPort ( EGA_CRTC, 0x2B, 0xFF ) )
				chipset = PARADISE_PVGA1A;
			else
			{
				tmp = ReadReg ( 0x3C4, 6 );
				WriteReg ( 0x3C4, 6, 0x48 );
				if ( TestPort ( 0x3C4, 7, 0xF0 ) )
					chipset = PARADISE_WD90C00;
				else
				if ( TestPort ( 0x3C4, 0x10, 0xFF ) )
					chipset = PARADISE_WD90C2X;
				else
				if ( TestPort ( 0x3C4, 0x14, 0x0F ) )
					chipset = PARADISE_WD90C1X;
				else
				if ( ReadReg ( EGA_CRTC, 0x37 ) == 0x31 )
					chipset = PARADISE_WD90C31;
				else
					chipset = PARADISE_WD90C30;
			}

			switch ( ReadReg ( 0x3CE, 0x0B ) >> 6 )
			{
				case 0:
					memory = 256;
					break;

				case 1:
					memory = 512;
					break;

				case 2:
					memory = 1024;
					break;

				default:
					memory = 2048;
					break;
			}
		}
	}

	WriteReg ( 0x3CE, 0x0F, tmp );

	return res;
}

int	FindVideo7 ()		// Detect the presence of a Video7 SuperVGA board
{
	asm {
		mov		ax,6F00h
		xor		bx,bx
		int		10h
	}

	if ( _BX != 0x5637 )		// "V7" signature
		return 0;

	asm {
		mov		ax,6F07h
		int		10h
		and		ah,7Fh
	}

	switch ( _AH )
	{
		case 0:
			memory = 256;
			break;

		case 1:
			memory = 512;
			break;

		case 2:
			memory = 1024;
			break;

		default:
			memory = 2048;
			break;
	}

	return VIDEO_7;
}

int	FindGenoa ()		// Detect the presence of a Genoa GVGA SuperVGA board
{
	int	offs = peek ( 0xC000, 0x0037 );

	if ( peekb ( 0xC000, offs ) != 0x77 || peek ( 0xC000, offs + 2 ) != 0x6699 )
		return 0;

	switch ( peekb ( 0xC000, offs + 1 ) )
	{
		case 0:
			chipset = GENOA_62_300;
			memory  = 256;
			break;

		case 0x11:
			chipset = GENOA_64_600;
			memory  = 512;
			break;

		case 0x22:
			chipset = GENOA_6100;
			memory  = 256;
			break;

		case 0x33:
			chipset = GENOA_51_5200;
			memory  = 256;
			break;

		case 0x55:
			chipset = GENOA_53_5400;
			memory  = 512;
			break;
	}

	return chipset;
}

void	FindEverex ()		// Detect the presence of an Everex SuperVGA board
{
	asm {
		mov	ax,7000h
		xor	bx,bx
		int	10h
	}

	if ( _AL != 0x70 )
		return 0;

	int	type = _DX;
	int	mem  = _CX;

	switch ( tyep >> 4 )
	{
		case 0x236:
			chipset = EVEREX_236;
			break;

		case 0x620:
			chipset = EVEREX_620;
			break;

		case 0x629:
			chipset = EVEREX_629;
			break;

		case 0x673:
			chipset = EVEREX_673;
			break;

		case 0x678:
			chipset = EVEREX_678;
			break;

		default:
			chipset = EVEREX_NR;
			break;
	}

	switch ( mem >> 14 )
	{
		case 0:
			memory = 256;
			break;

		case 1:
			memory = 512;
			break;

		case 2:
			memory = 1024;
			break;

		default:
			memory = 2048;
			break;
	}

	return chipset;
}


int	FindTrident ()		// Detect the presence of the Trident SuperVGA boards
{
	asm {
		mov	dx,3C4h
		mov	al,0Bh
		out	dx,al
		inc	dl
		xor	al,al
		out	dx,al
		in	al,dx
		and	al,0Fh
	}

	if ( _AL < 2 )
		return 0;
	else
	if ( _AL == 2 )
	{
		chipset = TRIDENT_88;

		if ( ReadReg ( EGA_CRTC, 0x1F ) & 2 )
			memory = 512;
		else
			memory = 256;
	}
	else
	{
		chipset = TRIDENT_89;
		switch ( ReadReg ( EGA_CRTC, 0x1F ) & 3 )
		{
			case 0:
				memory = 256;
				break;

			case 1:
				memory = 512;
				break;

			case 2:
				memory = 1024;
				break;

			default:
				memory = 2048;
		}
	}

	return chipset;
}

int	FindATI ()		// Detect the presence of an ATI Technologies SuperVGA board
{
	if ( _fstrncmp ( MK_FP ( 0xC000, 0x40 ), "13", 2 ) )
		return 0;

	if ( _fstrncmp ( MK_FP ( 0xC000, 0x31 ), "761295520", 9 ) )
		return 0;

	char	ver = peekb ( 0xC000, 0x43 );

	if ( ver >= '3' )
	{
		chipset = ATI_28800_2;
		memory  = 256;

		if ( ReadReg ( 0x1CE, 0xB0 ) & 0x10 )
			memory = 512;

		if ( ReadReg ( 0x1CE, 0xB0 ) & 0x08 )
			memory = 1024;

		if ( ver == '4' )
			chipset = ATI_2800_4;
		else
		if ( ver >= '5' )
		{
			chipset = ATI_28800_5;
		}
	}
	else
	if ( ver == '1' )

		chipset = ATI_18800;
	}
}

int	FindAhead ()		// Detect the presence of a Ahead A or B SuperVGA board
{
	int	tmp = ReadReg ( 0x3CE, 0x0F );

	WriteReg ( 0x3CE, 0x0F, 0 );
	if ( TestReg ( 0x3CE, 0x0D, 0xFF ) )
	{
		WriteReg ( 0x3CE, 0x0F, 0x20 );
		if ( !TestReg ( 0x3CE, 0x0D, 0xFF ) )
		{
			if ( ReadRef ( 0x3CE, 0x0F ) & 0x0F )
			{
				chipset = AHEAD_B;
			}
			else
			{
				chipset = AHEAD_A;
			}
		}
	}

	WriteReg ( 0x3CE, 0x0F, tmp );
	return chipset;
}

int	FindNCR ()		// Detect the presence of the NCR 77C22E SuperVGA boards
{
	int tmp = ReadReg ( 0x3C4, 5 );


	if ( !TestReg ( 0x3C4, 5, 5 ) )
	{
		WriteReg ( 0x3C4, 5, 0 );

		wrinx	3C4h, 5, 0			; Disable extended registers
		tstinx	3C4h, 10h, 0FFh		; Test Cursor location y register
		jz		@@NoNCR				; Reg was writeable, so test failed

		wrinx	3C4h, 5, 1			; Enable extended registers
		tstinx	3C4h, 10h, 0FFh		; Test Cursor location y register
		jnz		@@NoNCR				; Reg was not writeable, so not NCR

; Determine the chip revision

		rdinx	3C4h, 8				; Get chip version number
		shr		al,4
		cmp		al,2
		jae		@@NCR77C22E

; We have an NCR77C20/21 chip. These do not support certain modes.

		ClearMode	grSVGA_1280x1024x16, NCRModes
		ClearMode	grSVGA_1024x768x256, NCRModes
		ClearMode	grSVGA_1280x1024x256, NCRModes

@@NCR77C22E:
		mov		ax,4096				; Maximum of 4Mb memory
		mov		bx,offset NCRSwitch	; Offset of bank switch routine
		mov		cx,5Eh				; Values to set 640x400x256
		xor		dx,dx
		call	CheckMem			; Manually check amount of memory

		mov		[PageFlip],true		; Card supports extended page flipping
		mov		[WORD VideoModes],offset NCRModes
		mov		ax,grSVGA_NCR
		stc
		ret

@@NoNCR:
		wrinx	3C4h, 5, [old]		; Restore register lock
		clc
		ret

ENDP	FindNCR

int	FindTseng ()		// Detect the presence of the ET3000 and ET4000 SuperVGA boards
{
	outportb ( 0x3BF, 3 );
	outportb ( 0x3D8, 0xA0 ); 	// Enable ET4000 extensions

	if ( !TestPort ( 0x3CD, 0x3F ) )
		return 0;

	if ( TestReg ( EGA_CRTC, 0x33, 0x0F ) )
	{
		chipset = ET4000;
		int mem = ReadReg ( EGA_CRTC, 0x37 ) & 0x0B;

		if ( mem < 0x0A )
			memory = 256;
		else
		if ( mem < 0x0B )
			memory = 512;
		else
			memory = 1024;
	}
	else
	{
		chipset = ET3000;
		// ????
	}

	return chipset;
}

int	FindCirrus ()			// detect Cirrus Logic 54xx SuperVGA
{
	char	tmp = ReadReg ( 0x3C4, 6 );
	int	res = 0;

	WriteReg ( 0x3C4, 6, 0x12 );

	if ( ReadReg ( 0x3C4, 6 ) != 0x12 )
		if ( TestPort ( 0x3C4, 0x1E, 0x3F ) &&
		     TestPort ( EGA_CRTC, 0x1B, 0xFF ) )
		     {
			switch ( ( ReadReg ( 0x3C4, 0x0F ) >> 3 ) & 3 )
			{
				case 0:
					memory = 256;
					break;

				case 1:
					memory = 512;
					break;

				case 2:
					memory = 1024;
					break;

				default:
					memory = 2048;
			}

			res = CIRRUS_54XX;
		     }

	WriteReg ( 0x3C4, 6, tmp );

	return res;
}

