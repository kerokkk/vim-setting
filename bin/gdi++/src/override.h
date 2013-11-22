/*******************************************************************************
*	filename	:	override.h
*	version		:	20060927
*	subversion	:	
*	
*	copyright	:	◆0x0D/0x20I
*		460	(gdi++.dllスレ)
*		168	(gdi++.dllスレ 2pt)
*		Pico◆PicoKHZGNI
*	
*	history :
*		2006/09/27	本家Ver.20060927
*		2006/09/30	168氏による改良。詳細はmemo.txt。
*		2006/10/02	このコメントの作成 by Pico◆PicoKHZGNI
*******************************************************************************/

#pragma once
#define _CRT_SECURE_NO_DEPRECATE 1
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN 1
#define UNICODE  1
#define _UNICODE 1
#include <Windows.h>
#define for if(0);else for	//VC2005向け

//#define USE_NATIVE_API
#include "mem.h"
#include <tchar.h>
#include <malloc.h>		// _alloca
#include <mbctype.h>	// _getmbcp
#include <math.h>

//Intelコンパイラ向け
#if defined(__SSE3__)
	#include <pmmintrin.h>
#elif defined(__SSE2__)
	#include <emmintrin.h> 
#endif

/*******************************************************************************
*	デバッグ関連
*******************************************************************************/
#ifdef _DEBUG	//デバッグビルド時
	#define TRACE	_Trace
	#include <stdarg.h>	//va_list
	#include <stdio.h>	//_vsnwprintf
	static void _Trace(LPCTSTR pszFormat, ...)
	{
		va_list argptr;
		va_start(argptr, pszFormat);
		//w(v)sprintfは1024文字以上返してこない
		TCHAR szBuffer[1024];
		wvsprintf(szBuffer, pszFormat, argptr);
		OutputDebugString(szBuffer);
	}
#else	//デバッグビルドでない時
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
*	ファンクションの定義
*******************************************************************************/
void LoadSettings(HMODULE hModule);
BOOL IsOSXPorLater();
BOOL IsProcessExcluded();

/*******************************************************************************
*	ExtTextOutWで使用されるキャッシュのクラス
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

	//本体はoverride.cpp
	HDC CreateDC(int num);
	HBITMAP CreateDIB(int num, int width, int height, int BitCount, BYTE** lplpPixels);
	void FillSolidRect(int num, COLORREF rgb, const RECT* lprc);
};

/*******************************************************************************
*	スレッド関連
*******************************************************************************/
#include "tlsdata.h"
extern CTlsData<ETOCache> g_Cache;

#include <mmsystem.h>	//mmioFOURCC
#define CREATEPROCESS_MAGIC		mmioFOURCC('G', 'D', 'I', '+')

/*******************************************************************************
*	フックするAPIの定義
*******************************************************************************/
#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	rettype WINAPI ORIG_##name argtype; \
	rettype WINAPI IMPL_##name argtype;
#include "hooklist.h"
#undef HOOK_DEFINE

//EOF
