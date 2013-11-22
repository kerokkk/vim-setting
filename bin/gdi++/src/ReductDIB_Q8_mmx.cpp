/*******************************************************************************
*	filename	:	ReductDIB_Q8_mmx.cpp
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
void  ReductDIB_Q8_mmx(ETOCache* pCache, BYTE* lpBufferPixels, BYTE* lpCanvasPixels, int CanvasWidth, int CanvasHeight,COLORREF FontColor)
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

	DWORD c_01110111b[2] = { 0x77777777,0x77777777 };
	DWORD c_00001111b[2] = { 0x0f0f0f0f,0x0f0f0f0f };
	WORD c_fontcolor[4] = { fcB,fcG,fcR,0 };

	for(int i_y = 0;i_y<BufferHeight;i_y++){
		int i_y_buf = i_y * BufferLineSizeByte;
		int i_y_can = i_y * CanvasLineSizeByte *8;
		int bwd8 = BufferWidth/8;
		if(BufferWidth % 8)bwd8++;
		for(int i_x = 0;i_x<bwd8;i_x++){
			DWORD BufferCurPos = (DWORD)lpBufferPixels + (i_x << 5) + i_y_buf ;
			DWORD CanvasCurPos = (DWORD)lpCanvasPixels + (i_x << 3) + i_y_can;
			__asm
			{
				//***************************************canvasの8*8のビットを数える
				movq	mm7 , c_01110111b
				movq	mm6 , c_00001111b
				movq	mm0 , c_fontcolor

				mov		eax , BufferCurPos
				mov		ebx , CanvasCurPos
				mov		edx , CanvasLineSizeByte

				pxor	mm1 , mm1

				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx
				//***************************line 0
				movq	mm3 , [ebx]
				movq	mm2 , mm3 //
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				psrlw	mm3 , 1
				pand	mm3 , mm7
				psubb	mm2 , mm3
				movq	mm3 , mm2 
				psrlw	mm3 , 4
				paddb	mm2 , mm3
				pand	mm2 , mm6
				paddusb	mm1 , mm2
				add		ebx , edx

				//***************************************アルファブレンド
				pxor		mm7 , mm7

				//***************************line 0
				movq		mm2 , mm1
				punpcklbw	mm2 , mm2
				punpcklwd	mm2 , mm2
				movq		mm3 , mm2
				punpckhbw	mm2 , mm7 //alpha[0,1]
				punpcklbw	mm3 , mm7 //alpha[2,3]
				movq		mm4 , [eax]
				movq		mm5 , mm4
				punpckhbw	mm4 , mm7 //dest[0,1]
				punpcklbw	mm5 , mm7 //dest[2,3]
				movq		mm6 , mm4	
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm2 //(符号有り)
				psraw		mm6 , 6 
				psubsw		mm4 , mm6 //dest - (dest - src)*alpha
				movq		mm6 , mm5
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm3 
				psraw		mm6 , 6 
				psubsw		mm5 , mm6 //dest - (dest - src)*alpha
				packuswb	mm5 , mm4
				movq		[eax] , mm5
				add			eax , 8
				psrlq		mm1 , 16 
				//***************************line 0
				movq		mm2 , mm1
				punpcklbw	mm2 , mm2
				punpcklwd	mm2 , mm2
				movq		mm3 , mm2
				punpckhbw	mm2 , mm7 //alpha[0,1]
				punpcklbw	mm3 , mm7 //alpha[2,3]
				movq		mm4 , [eax]
				movq		mm5 , mm4
				punpckhbw	mm4 , mm7 //dest[0,1]
				punpcklbw	mm5 , mm7 //dest[2,3]
				movq		mm6 , mm4	
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm2 //(符号有り)
				psraw		mm6 , 6 
				psubsw		mm4 , mm6 //dest - (dest - src)*alpha
				movq		mm6 , mm5
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm3 
				psraw		mm6 , 6 
				psubsw		mm5 , mm6 //dest - (dest - src)*alpha
				packuswb	mm5 , mm4
				movq		[eax] , mm5
				add			eax , 8
				psrlq		mm1 , 16 
				//***************************line 0
				movq		mm2 , mm1
				punpcklbw	mm2 , mm2
				punpcklwd	mm2 , mm2
				movq		mm3 , mm2
				punpckhbw	mm2 , mm7 //alpha[0,1]
				punpcklbw	mm3 , mm7 //alpha[2,3]
				movq		mm4 , [eax]
				movq		mm5 , mm4
				punpckhbw	mm4 , mm7 //dest[0,1]
				punpcklbw	mm5 , mm7 //dest[2,3]
				movq		mm6 , mm4	
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm2 //(符号有り)
				psraw		mm6 , 6 
				psubsw		mm4 , mm6 //dest - (dest - src)*alpha
				movq		mm6 , mm5
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm3 
				psraw		mm6 , 6 
				psubsw		mm5 , mm6 //dest - (dest - src)*alpha
				packuswb	mm5 , mm4
				movq		[eax] , mm5
				add			eax , 8
				psrlq		mm1 , 16 
				//***************************line 0
				movq		mm2 , mm1
				punpcklbw	mm2 , mm2
				punpcklwd	mm2 , mm2
				movq		mm3 , mm2
				punpckhbw	mm2 , mm7 //alpha[0,1]
				punpcklbw	mm3 , mm7 //alpha[2,3]
				movq		mm4 , [eax]
				movq		mm5 , mm4
				punpckhbw	mm4 , mm7 //dest[0,1]
				punpcklbw	mm5 , mm7 //dest[2,3]
				movq		mm6 , mm4	
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm2 //(符号有り)
				psraw		mm6 , 6 
				psubsw		mm4 , mm6 //dest - (dest - src)*alpha
				movq		mm6 , mm5
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm3 
				psraw		mm6 , 6 
				psubsw		mm5 , mm6 //dest - (dest - src)*alpha
				packuswb	mm5 , mm4
				movq		[eax] , mm5
				add			eax , 8
				psrlq		mm1 , 16 
				//***************************line 0
				movq		mm2 , mm1
				punpcklbw	mm2 , mm2
				punpcklwd	mm2 , mm2
				movq		mm3 , mm2
				punpckhbw	mm2 , mm7 //alpha[0,1]
				punpcklbw	mm3 , mm7 //alpha[2,3]
				movq		mm4 , [eax]
				movq		mm5 , mm4
				punpckhbw	mm4 , mm7 //dest[0,1]
				punpcklbw	mm5 , mm7 //dest[2,3]
				movq		mm6 , mm4	
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm2 //(符号有り)
				psraw		mm6 , 6 
				psubsw		mm4 , mm6 //dest - (dest - src)*alpha
				movq		mm6 , mm5
				psubsw		mm6 , mm0 
				pmullw		mm6 , mm3 
				psraw		mm6 , 6 
				psubsw		mm5 , mm6 //dest - (dest - src)*alpha
				packuswb	mm5 , mm4
				movq		[eax] , mm5
				add			eax , 8
				psrlq		mm1 , 16 
				
				emms
			}
		}
	}
}
