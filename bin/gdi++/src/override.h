/*******************************************************************************
*	filename	:	override.h
*	version		:	20060927
*	subversion	:	
*	
*	copyright	:	��0x0D/0x20I
*		460	(gdi++.dll�X��)
*		168	(gdi++.dll�X�� 2pt)
*		Pico��PicoKHZGNI
*	
*	history :
*		2006/09/27	�{��Ver.20060927
*		2006/09/30	168���ɂ����ǁB�ڍׂ�memo.txt�B
*		2006/10/02	���̃R�����g�̍쐬 by Pico��PicoKHZGNI
*******************************************************************************/

#pragma once
#define _CRT_SECURE_NO_DEPRECATE 1
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN 1
#define UNICODE  1
#define _UNICODE 1
#include <Windows.h>
#define for if(0);else for	//VC2005����

//#define USE_NATIVE_API
#include "mem.h"
#include <tchar.h>
#include <malloc.h>		// _alloca
#include <mbctype.h>	// _getmbcp
#include <math.h>

//Intel�R���p�C������
#if defined(__SSE3__)
	#include <pmmintrin.h>
#elif defined(__SSE2__)
	#include <emmintrin.h> 
#endif

/*******************************************************************************
*	�f�o�b�O�֘A
*******************************************************************************/
#ifdef _DEBUG	//�f�o�b�O�r���h��
	#define TRACE	_Trace
	#include <stdarg.h>	//va_list
	#include <stdio.h>	//_vsnwprintf
	static void _Trace(LPCTSTR pszFormat, ...)
	{
		va_list argptr;
		va_start(argptr, pszFormat);
		//w(v)sprintf��1024�����ȏ�Ԃ��Ă��Ȃ�
		TCHAR szBuffer[1024];
		wvsprintf(szBuffer, pszFormat, argptr);
		OutputDebugString(szBuffer);
	}
#else	//�f�o�b�O�r���h�łȂ���
	#define TRACE	NOP_FUNCTION
	//<winnt.h>
	//#ifndef NOP_FUNCTION
	//#if (_MSC_VER >= 1210)
	//#define NOP_FUNCTION __noop
	//#else
	//#define NOP_FUNCTION (void)0
	//#endif
	//#endif
#endif	//_DEBUG

/*******************************************************************************
*	�t�@���N�V�����̒�`
*******************************************************************************/
void LoadSettings(HMODULE hModule);
BOOL IsOSXPorLater();
BOOL IsProcessExcluded();

/*******************************************************************************
*	ExtTextOutW�Ŏg�p�����L���b�V���̃N���X
*******************************************************************************/
	#define NUM_ETO_CANVAS 0
	#define NUM_ETO_BUFFER 1
	#define SIZE_ETO 2
class ETOCache {
private:
	HDC		hdc[SIZE_ETO];
	HBITMAP	hbmp[SIZE_ETO];
	BYTE*	lpPixels[SIZE_ETO];
	SIZE	dibSize[SIZE_ETO];

public:
	ETOCache(){
		for(int i=0 ; i<SIZE_ETO ; i++){
			hdc[i] = NULL;
			hbmp[i] = NULL;
			lpPixels[i] = NULL;
			dibSize[i].cx = dibSize[i].cy = 0;
		}
	}
	~ETOCache(){
		for(int i=0 ; i<SIZE_ETO ; i++){
		if(hdc[i])	DeleteDC(hdc[i]);
		if(hbmp[i])	DeleteObject(hbmp[i]);
		}
	}

	const SIZE& Size(int num) const{return dibSize[num];}
	BYTE* GetPixel(int num){return lpPixels[num];}

	//�{�̂�override.cpp
	HDC CreateDC(int num);
	HBITMAP CreateDIB(int num, int width, int height, int BitCount, BYTE** lplpPixels);
	void FillSolidRect(int num, COLORREF rgb, const RECT* lprc);
};

/*******************************************************************************
*	�X���b�h�֘A
*******************************************************************************/
#include "tlsdata.h"
extern CTlsData<ETOCache> g_Cache;

#include <mmsystem.h>	//mmioFOURCC
#define CREATEPROCESS_MAGIC		mmioFOURCC('G', 'D', 'I', '+')

/*******************************************************************************
*	�t�b�N����API�̒�`
*******************************************************************************/
#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	rettype WINAPI ORIG_##name argtype; \
	rettype WINAPI IMPL_##name argtype;
#include "hooklist.h"
#undef HOOK_DEFINE

//EOF
