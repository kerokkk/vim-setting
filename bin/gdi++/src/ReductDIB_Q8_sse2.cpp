/*******************************************************************************
*	filename	:	ReductDIB_Q8_sse2.cpp
*	version		:	20060927
*	subversion	:	1005
*	
*	copyright	:	◆0x0D/0x20I
*					460	(gdi++.dllスレ)
*					168	(gdi++.dllスレ 2pt)
*					Pico◆PicoKHZGNI
*	
*	history :
*		2006/10/05	override.cppより分離 by Pico◆PicoKHZGNI
*******************************************************************************/
void  ReductDIB_Q8_sse2(ETOCache* pCache, BYTE* lpBufferPixels, BYTE* lpCanvasPixels, int CanvasWidth, int CanvasHeight,COLORREF FontColor)
{
	
	int BufferWidth = CanvasWidth /8; //単位はピクセル
	int BufferHeight = CanvasHeight /8; //単位はピクセル
	int BufferLineSize = pCache->Size(NUM_ETO_BUFFER).cx; //単位はピクセル
	int CanvasLineSize = pCache->Size(NUM_ETO_CANVAS).cx; //単位はピクセル
//	if(CanvasLineSize % 32)CanvasLineSize += 32 - (CanvasLineSize % 32);
	int BufferLineSizeByte = BufferLineSize *4; //単位はバイト
	int CanvasLineSizeByte = CanvasLineSize /8; //単位はバイト

	BYTE fcB = (BYTE)( (FontColor & 0x00FF0000) >> 16);
	BYTE fcG = (BYTE)( (FontColor & 0x0000FF00) >> 8 );
	BYTE fcR = (BYTE)( (FontColor & 0x000000FF)      );

	DWORD c_01110111b[4] = { 0x77777777,0x77777777,0x77777777,0x77777777 };
	DWORD c_00001111b[4] = { 0x0f0f0f0f,0x0f0f0f0f,0x0f0f0f0f,0x0f0f0f0f };
	WORD c_fontcolor[8] = { fcB,fcG,fcR,0, fcB,fcG,fcR,0 };
	for(int i_y = 0;i_y<BufferHeight;i_y++){
		int i_y_buf = i_y * BufferLineSizeByte;
		int i_y_can = i_y * CanvasLineSizeByte *8;
		int bwd16 = BufferWidth/16;
		if(BufferWidth % 16)bwd16++;
		for(int i_x = 0;i_x<bwd16;i_x++){
			DWORD BufferCurPos = (DWORD)lpBufferPixels + (i_x << 6) + i_y_buf ;
			DWORD CanvasCurPos = (DWORD)lpCanvasPixels + (i_x << 4) + i_y_can;
			__asm
			{
				//***************************************canvasの8*8のビットを数える
				movdqu	xmm7 , c_01110111b
				movdqu	xmm6 , c_00001111b
				movdqu	xmm0 , c_fontcolor

				mov		eax , BufferCurPos
				mov		ebx , CanvasCurPos
				mov		edx , CanvasLineSizeByte

				pxor	xmm1 , xmm1

				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
				add		ebx , edx
				//***************************line 0
				movdqa	xmm3 , [ebx]
				movdqu	xmm2 , xmm3 //
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				psrlw	xmm3 , 1
				pand	xmm3 , xmm7
				psubb	xmm2 , xmm3
				movdqu	xmm3 , xmm2 
				psrlw	xmm3 , 4
				paddb	xmm2 , xmm3
				pand	xmm2 , xmm6
				paddusb	xmm1 , xmm2
//				add		ebx , edx

				//***************************************アルファブレンド
				pxor		xmm7 , xmm7

				//***************************line 0
				movdqu		xmm2 , xmm1
				punpcklbw	xmm2 , xmm2
				punpcklwd	xmm2 , xmm2
				movdqu		xmm3 , xmm2
				punpckhbw	xmm2 , xmm7 //alpha[0,1]
				punpcklbw	xmm3 , xmm7 //alpha[2,3]
				movdqa		xmm4 , [eax]
				movdqu		xmm5 , xmm4
				punpckhbw	xmm4 , xmm7 //dest[0,1]
				punpcklbw	xmm5 , xmm7 //dest[2,3]
				movdqu		xmm6 , xmm4	
				psubsw		xmm6 , xmm0 //(符号有り)
				pmullw		xmm6 , xmm2 
				psraw		xmm6 , 6 
				psubsw		xmm4 , xmm6 //dest - (dest - src)*alpha
				movdqu		xmm6 , xmm5
				psubsw		xmm6 , xmm0 
				pmullw		xmm6 , xmm3 
				psraw		xmm6 , 6 
				psubsw		xmm5 , xmm6 //dest - (dest - src)*alpha
				packuswb	xmm5 , xmm4
				movdqa		[eax] , xmm5
				add			eax , 16
				psrldq		xmm1 , 4 //単位はバイト
				//***************************line 0
				movdqu		xmm2 , xmm1
				punpcklbw	xmm2 , xmm2
				punpcklwd	xmm2 , xmm2
				movdqu		xmm3 , xmm2
				punpckhbw	xmm2 , xmm7 //alpha[0,1]
				punpcklbw	xmm3 , xmm7 //alpha[2,3]
				movdqa		xmm4 , [eax]
				movdqu		xmm5 , xmm4
				punpckhbw	xmm4 , xmm7 //dest[0,1]
				punpcklbw	xmm5 , xmm7 //dest[2,3]
				movdqu		xmm6 , xmm4	
				psubsw		xmm6 , xmm0 //(符号有り)
				pmullw		xmm6 , xmm2 
				psraw		xmm6 , 6 
				psubsw		xmm4 , xmm6 //dest - (dest - src)*alpha
				movdqu		xmm6 , xmm5
				psubsw		xmm6 , xmm0 
				pmullw		xmm6 , xmm3 
				psraw		xmm6 , 6 
				psubsw		xmm5 , xmm6 //dest - (dest - src)*alpha
				packuswb	xmm5 , xmm4
				movdqa		[eax] , xmm5
				add			eax , 16
				psrldq		xmm1 , 4 //単位はバイト
				//***************************line 0
				movdqu		xmm2 , xmm1
				punpcklbw	xmm2 , xmm2
				punpcklwd	xmm2 , xmm2
				movdqu		xmm3 , xmm2
				punpckhbw	xmm2 , xmm7 //alpha[0,1]
				punpcklbw	xmm3 , xmm7 //alpha[2,3]
				movdqa		xmm4 , [eax]
				movdqu		xmm5 , xmm4
				punpckhbw	xmm4 , xmm7 //dest[0,1]
				punpcklbw	xmm5 , xmm7 //dest[2,3]
				movdqu		xmm6 , xmm4	
				psubsw		xmm6 , xmm0 //(符号有り)
				pmullw		xmm6 , xmm2 
				psraw		xmm6 , 6 
				psubsw		xmm4 , xmm6 //dest - (dest - src)*alpha
				movdqu		xmm6 , xmm5
				psubsw		xmm6 , xmm0 
				pmullw		xmm6 , xmm3 
				psraw		xmm6 , 6 
				psubsw		xmm5 , xmm6 //dest - (dest - src)*alpha
				packuswb	xmm5 , xmm4
				movdqa		[eax] , xmm5
				add			eax , 16
				psrldq		xmm1 , 4 //単位はバイト
				//***************************line 0
				movdqu		xmm2 , xmm1
				punpcklbw	xmm2 , xmm2
				punpcklwd	xmm2 , xmm2
				movdqu		xmm3 , xmm2
				punpckhbw	xmm2 , xmm7 //alpha[0,1]
				punpcklbw	xmm3 , xmm7 //alpha[2,3]
				movdqa		xmm4 , [eax]
				movdqu		xmm5 , xmm4
				punpckhbw	xmm4 , xmm7 //dest[0,1]
				punpcklbw	xmm5 , xmm7 //dest[2,3]
				movdqu		xmm6 , xmm4	
				psubsw		xmm6 , xmm0 //(符号有り)
				pmullw		xmm6 , xmm2 
				psraw		xmm6 , 6 
				psubsw		xmm4 , xmm6 //dest - (dest - src)*alpha
				movdqu		xmm6 , xmm5
				psubsw		xmm6 , xmm0 
				pmullw		xmm6 , xmm3 
				psraw		xmm6 , 6 
				psubsw		xmm5 , xmm6 //dest - (dest - src)*alpha
				packuswb	xmm5 , xmm4
				movdqa		[eax] , xmm5
				add			eax , 16
				psrldq		xmm1 , 4 //単位はバイト
				

			}
		}
	}

}
