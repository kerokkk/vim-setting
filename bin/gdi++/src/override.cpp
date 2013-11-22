/*******************************************************************************
*	filename	:	override.cpp
*	version		:	20060927
*	subversion	:	1005
*	
*	copyright	:	◆0x0D/0x20I
*					460	(gdi++.dllスレ)
*					168	(gdi++.dllスレ 2pt)
*					Pico◆PicoKHZGNI
*	
*	history :
*		2006/09/27	本家Ver.20060927
*		2006/09/30	168氏による改良。詳細はmemo.txt。
*		2006/10/02	このコメントの作成 by Pico◆PicoKHZGNI
*		2006/10/05	1Bitレンダリング化など多数 by Pico◆PicoKHZGNI
*******************************************************************************/

/*******************************************************************************
*	プリプロセッサ
*******************************************************************************/
#define MAX_EXCLUDES 32
//#define __MMX__	//64bitパックド整数演算を使用する。MMX-Pentium以降
//#define __SSE__	//MMX2の追加、128bitパックド実数演算。Pen3以降
//#define __SSE2__	//128bitパックド整数演算。Pen4、Athlon64以降
//#define __SSE3__	//SSE2をちょっと改良。Prescott、Venice以降
//#define __SSSE3__	//SSE3をちょっと改良。Core2以降
//#define __SSE4__	//

/*******************************************************************************
*	ヘッダファイルのインクルード - 
*******************************************************************************/
#include "override.h"

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

/*******************************************************************************
*	グローバルっぽい変数&定数の宣言
*******************************************************************************/
UINT g_Quality = 0;
UINT g_Weight = 0;
UINT g_Enhance = 0;
UINT g_UseSubPixel = 0;
UINT g_SubPixelDirection = 0;
float g_MaxHeight = 0;
UINT g_ForceAntialiasedQuality = 0;
BOOL g_IsWinXPorLater = FALSE;
int  g_Scale = 1;
CTlsData<ETOCache> g_Cache;

/*******************************************************************************
*	計算補助
*******************************************************************************/
int div_ceil(int a, int b){
	if(a % b) return (a>0)? (a/b+1): (a/b-1);
	return a / b;
}

/*******************************************************************************
*	文字列のハッシュ化
*******************************************************************************/
template <int BUFSIZE = LF_FACESIZE, bool ignoreCase = false>
struct StringHashT {
private:
	DWORD	hash;
	WCHAR	name[BUFSIZE];
public:
	StringHashT() {}
	StringHashT(LPCWSTR s) {
		Init(s);
	}
	void Init(LPCWSTR s) {
		wcsncpy(name, s, BUFSIZE);
		_UpdateHash();
	}
	bool operator ==(const StringHashT<BUFSIZE, ignoreCase>& x) const {
		if(ignoreCase)
			return !(hash != x.hash || _wcsicmp(name, x.name) != 0); // VC2005ではwcsicmpは警告が出ます。
		else
			return !(hash != x.hash || wcscmp(name, x.name) != 0);
	}
	DWORD Hash() const {
		return hash;
	}
	LPCWSTR c_str() const {
		return name;
	}
	void _UpdateHash() {
		DWORD dw = 0;
		LPWSTR p;
		for (p = name; *p; p++) {
			dw <<= 3;
			if(ignoreCase)
				dw ^= (WORD)CharLowerW((LPWSTR)*p);
			else
				dw ^= *p;
		}
		hash = dw;
	}
};

typedef StringHashT<>				StringHash;
typedef StringHashT<MAX_PATH,true>	StringHashMod;
StringHash g_ExcludeList[MAX_EXCLUDES];
StringHashMod g_ExcludeMod[MAX_EXCLUDES];

/*******************************************************************************
*	INIファイルの読み込み
*******************************************************************************/
void LoadSettings(HMODULE hModule){
	WCHAR szFileName[MAX_PATH + 1] = L"";
	// DLLのフルパスを取得
	GetModuleFileName(hModule, szFileName, MAX_PATH);
	// 拡張子を".ini"に置き換える
	lstrcpy(szFileName + lstrlen(szFileName) - 3, L"ini");
	// 各種設定読み込み
	// INIファイルの例:
	// [General]
	// Quality=1
	// Weight=0
	// Enhance=0
	// UseSubPixel=0
	// SubPixelDirection=0
	// MaxHeight=0
	// ForceAntialiasedQuality=0
	g_Quality = GetPrivateProfileInt(L"General",L"Quality",2,szFileName);
	g_Weight = GetPrivateProfileInt(L"General",L"Weight",0,szFileName);
  	g_Enhance = GetPrivateProfileInt(L"General",L"Enhance",0,szFileName);
	g_UseSubPixel = GetPrivateProfileInt(L"General",L"UseSubPixel",0,szFileName);
	g_SubPixelDirection = GetPrivateProfileInt(L"General",L"SubPixelDirection",0,szFileName);
	g_MaxHeight = (float)GetPrivateProfileInt(L"General",L"MaxHeight",0,szFileName);
	g_ForceAntialiasedQuality = GetPrivateProfileInt(L"General",L"ForceAntialiasedQuality",0,szFileName);
	if(g_UseSubPixel)
		g_Quality = 2;

	// OSのバージョンがXP以降かどうか
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	g_IsWinXPorLater = ((osvi.dwMajorVersion > 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));

	// [Exclude]セクションから除外フォントリストを読み込む
	memset(&g_ExcludeList, 0, sizeof(g_ExcludeList));

	switch(g_Quality) {
	case 15:  g_Scale = 16; break;
	case 11:  g_Scale = 12; break;
	case 7:  g_Scale = 8; break;
	case 6:  g_Scale = 7; break;
	case 5:  g_Scale = 6; break;
	case 4:  g_Scale = 5; break;
	case 3:  g_Scale = 4; break;
	case 2:  g_Scale = 3; break;
	case 1:  g_Scale = 2; break;
	default: g_Scale = 1; break;
	}

	
	const int cch = 65536;
	WCHAR*  buffer = (LPWSTR)calloc(sizeof(WCHAR), cch);
	if(!buffer)return;

	WCHAR* p = buffer;
	UINT   nb = 0;
	*p = '\0';
	GetPrivateProfileSection(L"Exclude", p, cch, szFileName);
	while(*p) {
		bool b = false;
		StringHash* exclist = g_ExcludeList;
		StringHash str(p);
		for(UINT i=0 ; i < nb ; i++) {
			if(exclist[i] == str) {
				b = true;
				break;
			}
		}
		if(!b) {
			if(nb < MAX_EXCLUDES) {
				exclist[nb++] = str;
			}
		}
		while(*p) p++;
		p++;
	}

	p = buffer;
	nb = 0;
	*p = '\0';
	GetPrivateProfileSection(L"ExcludeModule", p, cch, szFileName);
	while(*p) {
		bool b = false;
		StringHashMod* exclist = g_ExcludeMod;
		StringHashMod str(p);
		for(UINT i=0 ; i < nb ; i++) {
			if(exclist[i] == str) {
				b = true;
				break;
			}
		}
		if(!b) {
			if(nb < MAX_EXCLUDES) {
				exclist[nb++] = str;
			}
		}
		while(*p) p++;
		p++;
	}
	free(buffer);
}

/*******************************************************************************
*	Is～～関数
*******************************************************************************/
// フォントがアウトラインフォントかをチェックする
BOOL IsOutlineFont(HDC hdc)
{
	OUTLINETEXTMETRIC otm = { sizeof(OUTLINETEXTMETRIC) };
	return GetOutlineTextMetrics(hdc, otm.otmSize, &otm);
}

// フォントが除外リストに入っているかをチェックする
BOOL IsFontExcluded(const LPCWSTR lpszFaceName){
	StringHash* exclist = g_ExcludeList;
	StringHash str(lpszFaceName);
	for(UINT i=0 ; i < MAX_EXCLUDES && exclist[i].Hash() ; i++) {
		if(exclist[i] == str)
			return TRUE;
	}
	return FALSE;
}
// 非ユニコード版
BOOL IsFontExcluded(const LPCSTR lpszFaceName){
	int bufferLength = MultiByteToWideChar(CP_ACP, 0, lpszFaceName, LF_FACESIZE, 0, 0);
	LPWSTR lpszUnicode = (LPWSTR)malloc(bufferLength * 2);
	MultiByteToWideChar(CP_ACP, 0, lpszFaceName, LF_FACESIZE, lpszUnicode, bufferLength);
	BOOL b = IsFontExcluded(lpszUnicode);
	free(lpszUnicode);
	return b;
}

// プロセスが除外リストに入っているかをチェックする
BOOL IsProcessExcluded()
{
	//この辺参照→http://www.catch22.net/tuts/undoc01.asp
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	GetStartupInfo(&si);
	if(si.cbReserved2 == sizeof(int) + sizeof(FOURCC)) {
		FOURCC* pMagic = (FOURCC*)((BYTE*)si.lpReserved2 + sizeof(int));
		if (*pMagic == CREATEPROCESS_MAGIC)
			return FALSE;
	}

	StringHashMod* exclist = g_ExcludeMod;
	for(UINT i=0 ; i < MAX_EXCLUDES && exclist[i].Hash() ; i++) {
		if(GetModuleHandleW(exclist[i].c_str()))
			return TRUE;
	}
	return FALSE;
}

// 有効なDCかどうかをチェックする
BOOL IsValidDC(const HDC hdc){
	if (GetDeviceCaps(hdc, TECHNOLOGY) != DT_RASDISPLAY) return FALSE;
	if (!IsOutlineFont(hdc)) return FALSE;	// ビットマップフォントを弾く
	// ここでフォントチェックも行う
	WCHAR szFaceName[LF_FACESIZE] = L"";
	GetTextFaceW(hdc, LF_FACESIZE, szFaceName);
	if (IsFontExcluded(szFaceName)) return FALSE;
	return TRUE;
}


/*******************************************************************************
*	DIBの縮小
*******************************************************************************/
void  ReductDIB(ETOCache* pCache, BYTE* lpBufferPixels, BYTE* lpCanvasPixels, int CanvasWidth, int CanvasHeight,COLORREF FontColor)
{
	#define BufferPixelSize 4
	int BufferWidth = CanvasWidth /g_Scale; //単位はピクセル
	int BufferHeight = CanvasHeight /g_Scale; //単位はピクセル
	int BufferLineSize = pCache->Size(NUM_ETO_BUFFER).cx; //単位はピクセル
	int CanvasLineSize = pCache->Size(NUM_ETO_CANVAS).cx; //単位はピクセル
	int BufferLineSizeByte = BufferLineSize * BufferPixelSize; //単位はバイト
	int CanvasLineSizeByte = CanvasLineSize /8; //単位はバイト

	BYTE fcB = (BYTE)( (FontColor & 0x00FF0000) >> 16);
	BYTE fcG = (BYTE)( (FontColor & 0x0000FF00) >> 8 );
	BYTE fcR = (BYTE)( (FontColor & 0x000000FF)      );

	DWORD TargetBits;
	float Alpha;

	for(int i_y = 0;i_y<BufferHeight;i_y++){
		for(int i_x = 0;i_x<BufferWidth;i_x++){
			int BufferCurPos =		(i_x + i_y * BufferLineSize) * BufferPixelSize;	//Bufferの対象アドレス
			int CanvasCurPosBit =	(i_x + i_y * CanvasLineSize) * g_Scale;
			int CanvasCurPos =		CanvasCurPosBit /8;								//Canvasの対象アドレス
			int CanvasCurPosDiffBit = CanvasCurPosBit & 0x7;						//ビットのずれ
			
			Alpha = 0.0f;
			for(int j = 0;j < g_Scale; j++){
				TargetBits =	(lpCanvasPixels[CanvasCurPos + j*CanvasLineSizeByte    ] << 24) |
								(lpCanvasPixels[CanvasCurPos + j*CanvasLineSizeByte + 1] << 16) |
								(lpCanvasPixels[CanvasCurPos + j*CanvasLineSizeByte + 2] << 8 );
				TargetBits = TargetBits << CanvasCurPosDiffBit;
				for(int k = 0;k < g_Scale; k++){
						if(TargetBits & 0x80000000)	Alpha += 1.0f;
						TargetBits = TargetBits << 1;
					}
				}
			Alpha = Alpha/(g_Scale*g_Scale);

			lpBufferPixels[BufferCurPos+0] = (BYTE)(lpBufferPixels[BufferCurPos+0] * (1.0f-Alpha) + fcB*Alpha);
			lpBufferPixels[BufferCurPos+1] = (BYTE)(lpBufferPixels[BufferCurPos+1] * (1.0f-Alpha) + fcG*Alpha);
			lpBufferPixels[BufferCurPos+2] = (BYTE)(lpBufferPixels[BufferCurPos+2] * (1.0f-Alpha) + fcR*Alpha);
//			lpBufferPixels[BufferCurPos+3] = 0;
			
		}
	}

}

/*******************************************************************************
*	ReductDIBの最適化バージョン読み込み
*******************************************************************************/
#if defined(__MMX__)
	#include "ReductDIB_Q8_mmx.cpp"	
#elif defined(__SSE2__)
	#include "ReductDIB_Q8_sse2.cpp"
#endif

/*******************************************************************************
*	Bufferを塗りつぶす
*******************************************************************************/
void  FillBuffer(ETOCache* pCache, BYTE* lpBufferPixels, int BufferWidth, int BufferHeight,COLORREF FontColor)
{
	DWORD* lpdwBufferPixels = (DWORD*)lpBufferPixels;
	BYTE FontColorB = (BYTE)( (FontColor & 0x00FF0000) >> 16);
	BYTE FontColorG = (BYTE)( (FontColor & 0x0000FF00) >> 8 );
	BYTE FontColorR = (BYTE)( (FontColor & 0x000000FF)      );
	DWORD dwColor = (FontColorR<<16) | (FontColorG<<8) | FontColorB;
	int BufferLineSize = pCache->Size(NUM_ETO_BUFFER).cx;

	for(int i_y = 0;i_y<BufferHeight;i_y++){
		for(int i_x = 0;i_x<BufferWidth;i_x++){
			lpdwBufferPixels[i_x + i_y * BufferLineSize] = dwColor;
		}
	}
	
}
/*******************************************************************************
*	Canvasをゼロクリア
*******************************************************************************/
void  ZeroCanvas(ETOCache* pCache, BYTE* lpCanvasPixels, int CanvasWidth, int CanvasHeight)
{
	DWORD* lpdwCanvasPixels = (DWORD*)lpCanvasPixels;
	int CanvasLineSize = pCache->Size(NUM_ETO_CANVAS).cx;
//	if(CanvasLineSize % 32)CanvasLineSize += 32 - (CanvasLineSize % 32);
	
	for(int i_y = 0;i_y<CanvasHeight;i_y++){
		for(int i_x = 0;i_x<CanvasWidth / 32;i_x++){
			lpdwCanvasPixels[i_x + i_y * CanvasLineSize/32] = 0;
		}
	}
	
}


/*******************************************************************************
*	ETOCache
*******************************************************************************/
HDC ETOCache::CreateDC(int num)
{
	if(!hdc[num])
		hdc[num] = CreateCompatibleDC(NULL);
	return hdc[num];
}

HBITMAP ETOCache::CreateDIB(int num, int width, int height, int BitCount, BYTE** lplpPixels){

	int size_alignment = 128/BitCount; 
	if(width % size_alignment)width += size_alignment - (width % size_alignment);//128bitアラインメント
	width += 128/BitCount;//128bitマージンを取る

	if(dibSize[num].cx < width || dibSize[num].cy < height) {
		if(dibSize[num].cx > width)  width = dibSize[num].cx;
		if(dibSize[num].cy > height) height = dibSize[num].cy;
		BITMAPINFOHEADER bmiHeader = {sizeof(bmiHeader), width, -height, 1, BitCount, BI_RGB,};

		HBITMAP hbmpNew = CreateDIBSection(CreateDC(num), (CONST BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS, (LPVOID*)lplpPixels, NULL, 0);
		if(!hbmpNew)return NULL;

		//メモリ不足等でhbmpNew==NULLの場合を想定し、成功したときのみキャッシュを更新
		if(hbmp[num])DeleteObject(hbmp[num]);

		DWORD address = (DWORD)*lplpPixels;
		if(address % 16)address += 16 - (address % 16);//128bitアラインメント
		*lplpPixels = (BYTE*)address;

		hbmp[num] = hbmpNew;
		dibSize[num].cx = width;
		dibSize[num].cy = height;
		lpPixels[num] = *lplpPixels;
	}
	*lplpPixels = lpPixels[num];
	return hbmp[num];
}

void ETOCache::FillSolidRect(int num, COLORREF rgb, const RECT* lprc)
{
	rgb = RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
	DWORD* p = (DWORD*)lpPixels[num];
	DWORD* pend = p + (dibSize[num].cx * dibSize[num].cy);
	while(p < pend)
		*p++ = rgb;
}

/*******************************************************************************
*	APIフックのためのプリプロセッサ
*******************************************************************************/
BOOL WINAPI _RealGetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize, const TEXTMETRICA* lptm = NULL, INT* lpDx = NULL);
BOOL WINAPI _RealGetTextExtentPoint32W(HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize, const TEXTMETRICW* lptm = NULL, INT* lpDx = NULL);

#if 0//#ifdef _DEBUG
void dbg_GetTextExtentPoint32W(LPCSTR name, LPCWSTR psz, int cch, LPSIZE lpSize)
{
	LPWSTR pTmp = new WCHAR[cch+1];
	memcpy(pTmp, psz, cch * sizeof(WCHAR));
	pTmp[cch] = 0;
	if(lpSize) {
		TRACE(_T("%hs(\"%ls\", {%d,%d})\n"), name, pTmp, lpSize->cx, lpSize->cy);
	} else {
		TRACE(_T("%hs(\"%ls\", <null>)\n"), name, pTmp);
	}
	delete[] pTmp;
}
void dbg_GetTextExtentPoint32A(LPCSTR name, LPCSTR psz, int cch, LPSIZE lpSize)
{
	LPWSTR pTmp = new WCHAR[cch * 2 + 1];
	cch = MultiByteToWideChar(CP_ACP, 0, psz, cch, pTmp, cch * 2 + 1);
	dbg_GetTextExtentPoint32W(name, pTmp, cch, lpSize);
	delete[] pTmp;
}
#else
#define dbg_GetTextExtentPoint32A(a,b,c,d)
#define dbg_GetTextExtentPoint32W(a,b,c,d)
#endif

#define HOOK_DEFINE(dll, rettype, name, argtype, vars) \
	rettype WINAPI IMPL_##name argtype \
	{ \
	if ((g_Quality == 0) || (!IsValidDC(hdc))) return ORIG_##name vars; \
	int grmode = GetGraphicsMode(hdc); \
	if(grmode != GM_ADVANCED) \
		SetGraphicsMode(hdc, GM_ADVANCED); \
	XFORM xformOrig; \
	GetWorldTransform(hdc, &xformOrig); \
	rettype b = FALSE; \
	if (xformOrig.eM12 < -0.1f || 0.1f < xformOrig.eM12 || \
	    xformOrig.eM21 < -0.1f || 0.1f < xformOrig.eM21) { \
		b = _Real##name vars; /* ORIG_##name vars; */\
	} else { \
		XFORM xformScale = { \
			xformOrig.eM11 * g_Scale, xformOrig.eM12 * g_Scale, \
			xformOrig.eM21 * g_Scale, xformOrig.eM22 * g_Scale, \
			xformOrig.eDx  * g_Scale, xformOrig.eDy  * g_Scale  \
		}; \
		SetWorldTransform(hdc, &xformScale); \
		b = _Real##name vars; /*ORIG_##name vars; */\
		SetWorldTransform(hdc, &xformOrig); \
	} \
	if(grmode != GM_ADVANCED) \
		SetGraphicsMode(hdc, grmode); \
	dbg_##name(#name, lpString, cbString, lpSize); \
	return b; \
	}

/*
HOOK_DEFINE(user32.dll, DWORD, GetTabbedTextExtentA, (HDC hdc, LPCSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions), (hdc, lpString, nCount, nTabPositions, lpnTabStopPositions))
HOOK_DEFINE(user32.dll, DWORD, GetTabbedTextExtentW, (HDC hdc, LPCWSTR lpString, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions), (hdc, lpString, nCount, nTabPositions, lpnTabStopPositions))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointA, (HDC hdc, LPCSTR lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, lpszStr, cchString, nMaxExtent, lpnFit, lpDx, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointW, (HDC hdc, LPCWSTR lpszStr, int cchString, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, lpszStr, cchString, nMaxExtent, lpnFit, lpDx, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentExPointI, (HDC hdc, LPWORD pgiIn, int cgi, int nMaxExtent, LPINT lpnFit, LPINT lpDx, LPSIZE lpSize), (hdc, pgiIn, cgi, nMaxExtent, lpnFit, lpDx, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointA, (HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointW, (HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPointI, (HDC hdc, LPWORD pgiIn, int cgi, LPSIZE lpSize), (hdc, pgiIn, cgi, lpSize))
*/

//firefox向け
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32A, (HDC hdc, LPCSTR  lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32W, (HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
#undef HOOK_DEFINE

/*******************************************************************************
*	APIフック
*******************************************************************************/
BOOL WINAPI IMPL_GetTextExtentPointA(HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize)
{
	return IMPL_GetTextExtentPoint32A(hdc, lpString, cbString, lpSize);
}

BOOL WINAPI IMPL_GetTextExtentPointW(HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize)
{
	return IMPL_GetTextExtentPoint32W(hdc, lpString, cbString, lpSize);
}

HFONT WINAPI IMPL_CreateFontA(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCSTR  lpszFace)
{
	if ((g_Quality > 0) && g_ForceAntialiasedQuality && (!IsFontExcluded(lpszFace)))
		fdwQuality = ANTIALIASED_QUALITY;
	return ORIG_CreateFontA(nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace);
}

HFONT WINAPI IMPL_CreateFontW(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCWSTR lpszFace)
{
	if ((g_Quality > 0) && g_ForceAntialiasedQuality && (!IsFontExcluded(lpszFace)))
		fdwQuality = ANTIALIASED_QUALITY;
	return ORIG_CreateFontW(nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace);
}

HFONT WINAPI IMPL_CreateFontIndirectA(CONST LOGFONTA *lplf)
{
	LOGFONTA lf;
	if ((g_Quality > 0) && g_ForceAntialiasedQuality && !IsFontExcluded(lplf->lfFaceName)) {
		lf = *lplf;
		lf.lfQuality = ANTIALIASED_QUALITY;
		return ORIG_CreateFontIndirectA(&lf);
		//lplf = &lf;
	}
	return ORIG_CreateFontIndirectA(lplf);
}

HFONT WINAPI IMPL_CreateFontIndirectW(CONST LOGFONTW *lplf)
{
	LOGFONTW lf;
	if ((g_Quality > 0) && g_ForceAntialiasedQuality && !IsFontExcluded(lplf->lfFaceName)) {
		lf = *lplf;
		lf.lfQuality = ANTIALIASED_QUALITY;
		return ORIG_CreateFontIndirectW(&lf);
		//lplf = &lf;
	}
	return ORIG_CreateFontIndirectW(lplf);
}


// Windows 2000用
BOOL WINAPI IMPL_TextOutA(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, UINT cbString)
{
	if (g_IsWinXPorLater) {
		return ORIG_TextOutA(hdc, nXStart, nYStart, lpString, cbString);
	} else {
		return IMPL_ExtTextOutA(hdc, nXStart, nYStart, NULL, NULL, lpString, cbString, NULL);
	}
}

BOOL WINAPI IMPL_TextOutW(HDC hdc, int nXStart, int nYStart, LPCWSTR lpString, UINT cbString)
{
	if (g_IsWinXPorLater) {
		return ORIG_TextOutW(hdc, nXStart, nYStart, lpString, cbString);
	} else {
		return IMPL_ExtTextOutW(hdc, nXStart, nYStart, NULL, NULL, lpString, cbString, NULL);
	}
}

//gdi32.dll/win2kを逆汗して書き起こした関数
void ConvertDxArray(UINT CodePage, LPCSTR lpString, const int* lpDxA, int cbString, int* lpDxW, int)
{
	LPCSTR lpEnd = lpString + cbString;
	while(lpString < lpEnd) {
		*lpDxW = *lpDxA++;
		if(IsDBCSLeadByteEx(CodePage, *lpString)) {
			*lpDxW += *lpDxA++;
			lpString++;
		}
		lpDxW++;
		lpString++;
	}
}

// ANSI->Unicodeに変換してExtTextOutWに投げるExtTextOutA（Windows 2000用）
BOOL WINAPI IMPL_ExtTextOutA(HDC hdc, int nXStart, int nYStart, UINT fuOptions, CONST RECT *lprc, LPCSTR lpString, UINT cbString, CONST INT *lpDx)
{
#if 0//#ifdef _DEBUG
{
	LPSTR pTmp = new char[cbString+1];
	memcpy(pTmp, lpString, cbString);
	pTmp[cbString] = 0;
	if(lprc)
		TRACE(_T("IMPL_ExtTextOutA(%d, %d, %x, {%d,%d,%d,%d}, \"%hs\")\n"), nXStart, nYStart, fuOptions,
				lprc->left,lprc->top,lprc->right,lprc->bottom, pTmp);
	else
		TRACE(_T("IMPL_ExtTextOutA(%d, %d, %x, <null>, \"%hs\")\n"), nXStart, nYStart, fuOptions, pTmp);
	delete[] pTmp;
}
#endif

	if (g_IsWinXPorLater) {
		return ORIG_ExtTextOutA(hdc, nXStart, nYStart, fuOptions, lprc, lpString, cbString, lpDx);
	} else {
		LPWSTR lpszUnicode;
		int bufferLength;
		BOOL result;
		bufferLength = MultiByteToWideChar(CP_ACP, 0, lpString, cbString, 0, 0);
	//	lpszUnicode = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferLength * 2);
		lpszUnicode = (LPWSTR)malloc(bufferLength * 2);
		MultiByteToWideChar(CP_ACP, 0, lpString, cbString, lpszUnicode, bufferLength);

		UINT cp = _getmbcp();
		int* lpDxW = NULL;
		if(lpDx && cbString && cp) {
			lpDxW = (int*)calloc(sizeof(int), cbString);
			ConvertDxArray(cp, lpString, lpDx, cbString, lpDxW, 0);
			lpDx = lpDxW;
		}

		result = IMPL_ExtTextOutW(hdc, nXStart, nYStart, fuOptions, lprc, (LPCWSTR)lpszUnicode, bufferLength, lpDx);
	//	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpszUnicode);
		free(lpszUnicode);
		free(lpDxW);	// free(NULL)は問題無い事が保証されていたはず
		return result;
	}
}

BOOL WINAPI _RealGetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize, const TEXTMETRICA* lptm, INT* lpDx)
{
	//ANSI版ではlptmとlpDxは利用しない
//	Assert(lptm || lpDx);
	int cchStringW = MultiByteToWideChar(CP_ACP, 0, lpString, cbString, NULL, 0);
	LPWSTR lpStringW = new WCHAR[cchStringW + 1];
	if(!lpStringW) {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, 0, lpString, cbString, lpStringW, cchStringW + 1);
	BOOL ret = _RealGetTextExtentPoint32W(hdc, lpStringW, cchStringW, lpSize, NULL, NULL);
	delete[] lpStringW;
	return ret;
}

//正確なテキストのサイズを返す
//さらに正確な描画位置を計算してlpDxに返す
//(このlpDxをExtTextOutに渡して描画するとズレが無くなる)
BOOL WINAPI _RealGetTextExtentPoint32W(HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize, const TEXTMETRICW* lptm, INT* lpDx)
{
	if(!hdc || !lpString || !cbString || !lpSize) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	switch(GetObjectType(hdc)) {
	case OBJ_DC:
	case OBJ_MEMDC:
	case OBJ_METADC:
	case OBJ_ENHMETADC:
		break;
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	TEXTMETRICW _tm;
	//IMPL_ExtTextOutWからの呼び出しには既に取得済みのTEXTMETRICを使える
	if(!lptm) {
		GetTextMetrics(hdc, &_tm);
		lptm = &_tm;
	}

	lpSize->cx = 0;
	lpSize->cy = lptm->tmHeight;
	LPCWSTR lpEnd = lpString + cbString;
	for(; lpString < lpEnd; lpString++) {
		ABCFLOAT abcWidth = { 0 };
		GetCharABCWidthsFloatW(hdc, *lpString, *lpString, &abcWidth);
		const float f = abcWidth.abcfA
						+ abcWidth.abcfB
						+ abcWidth.abcfC
						+ 0.01f;
		lpSize->cx += (LONG)f;
		if(lpDx) {
			*lpDx++ = (int)f;
		}
	}
	return TRUE;
}


/*******************************************************************************
*	キレイなExtTextOut（当社比）の実装
*	BOOL ExtTextOut(
*	  HDC hdc,          // デバイスコンテキストのハンドル
*	  int X,            // 開始位置（基準点）の x 座標
*	  int Y,            // 開始位置（基準点）の y 座標
*	  UINT fuOptions,   // 長方形領域の使い方のオプション
*	  CONST RECT *lprc, // 長方形領域の入った構造体へのポインタ
*	  LPCTSTR lpString, // 文字列
*	  UINT cbCount,     // 文字数
*	  CONST INT *lpDx   // 文字間隔の入った配列
*	  );
*******************************************************************************/

typedef enum {
	ETOE_OK				= 0,
	ETOE_CREATEDC		= 1,
	ETOE_SETFONT		= 2,
	ETOE_CREATEDIB		= 3,
	ETOE_INVALIDARG		= 11,
	ETOE_ROTATION		= 12,
	ETOE_LARGESIZE		= 13,
	ETOE_INVALIDHDC		= 14,
	ETOE_ROTATEFONT		= 15,
	ETOE_NOAREA			= 16,
	ETOE_GETTEXTEXTENT	= 17,
	ETOE_CACHE			= 21,
} ExtTextOut_ErrorCode;

//例外モドキ
#define _TRY()
#define _THROW(code)	error = (code); goto ExtTextOutW_THRU
#define _CATCH()		ExtTextOutW_THRU:

BOOL WINAPI IMPL_ExtTextOutW(HDC hdc, int nXStart, int nYStart, UINT fuOptions, CONST RECT *lprc, LPCWSTR lpString, UINT cbString, CONST INT *lpDx)
{
_TRY();


	ExtTextOut_ErrorCode error = ETOE_OK;
	POINT	curPos = { nXStart, nYStart };
	POINT	destPos;
	SIZE	destSize;
	POINT	canvasPos;
	HFONT	hCurFont  = NULL;
	HGDIOBJ	hPrevFont = NULL;
	ETOCache* pCache = g_Cache.GetPtr();

	HDC		hCanvasDC = NULL;//拡大された1bit
	SIZE	canvasSize;
	BYTE*	lpCanvasPixels  = NULL;
	HBITMAP	hBmp1      = NULL;
	HGDIOBJ	hPrevBmp1  = NULL;
	
	HDC		hBufferDC = NULL;//元のサイズの32bit
	BYTE*	lpBufferPixels  = NULL;
	SIZE	bufferSize;
	HBITMAP	hBmp32      = NULL;
	HGDIOBJ	hPrevBmp32  = NULL;

	XFORM	xformNoTrns;
	XFORM	xformScale = {1.0f,0.0f,  0.0f,1.0f,  0.0f,0.0f};
	UINT	align;
	SIZE	textSize;
	TEXTMETRIC	tm = { 0 };
	LOGFONT	lf = { 0 };
	RECT	rc;
	int*	lpDxTemp = NULL;

	if(!pCache) {_THROW(ETOE_CACHE);}

//***************************** 引数のチェック ******************************
	if(lpString == NULL || cbString == 0 || g_Quality==0) { _THROW(ETOE_INVALIDARG);}
	if(!IsValidDC(hdc)) { _THROW(ETOE_INVALIDHDC);}
	
//***************************** HDC作成 ******************************
	hCanvasDC = pCache->CreateDC(NUM_ETO_CANVAS);
	if(!hCanvasDC) {_THROW(ETOE_CREATEDC);}
	hBufferDC = pCache->CreateDC(NUM_ETO_BUFFER);
	if(!hBufferDC) {_THROW(ETOE_CREATEDC);}

//***************************** 描画先位置と座標変換 ******************************
	align = GetTextAlign(hdc);
	if(align & TA_UPDATECP) {GetCurrentPositionEx(hdc, &curPos);}

	GetWorldTransform(hdc, &xformNoTrns);
	if(abs(xformNoTrns.eM12) > 0.1f || abs(xformNoTrns.eM21) > 0.1f) {
		_THROW(ETOE_ROTATION);// 回転変換はサポートできない
	}
	xformNoTrns.eDx = 0.0f; // 移動変換も無視する
	xformNoTrns.eDy = 0.0f;
	xformScale.eM11 = xformNoTrns.eM11 * g_Scale;//scaling
	xformScale.eM22 = xformNoTrns.eM22 * g_Scale;
	SetGraphicsMode(hCanvasDC, GM_ADVANCED);
	SetWorldTransform(hCanvasDC, &xformScale);
//	SetGraphicsMode(hBufferDC, GM_ADVANCED);
	SetWorldTransform(hBufferDC, &xformNoTrns);

//***************************** 現在のフォントを取得 ******************************
	//copy font
	//フォント周りはコストが大きく、何度もGetCurrentObjectやGetTextMetricsするのは
	//効率が悪いので1度だけ情報を取得して使い回す
	hCurFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
	if (!hCurFont) {
		// NULLの場合はSystemを設定しておく
		hCurFont = (HFONT)GetStockObject(SYSTEM_FONT);
		if (!hCurFont) {_THROW(ETOE_SETFONT);}// SystemすらNULLの場合はエラーを返す
	}
	hPrevFont = SelectObject(hCanvasDC, hCurFont);
	GetTextMetrics(hdc, &tm);
	GetObject(hCurFont,sizeof(LOGFONT), &lf);

//***************************** 文字間隔の設定 ******************************
	//GetTextExtentPoint系はSetTextCharacterExtraを必要とするらしい
	SetTextCharacterExtra(hCanvasDC, GetTextCharacterExtra(hdc));
	//text size
	if(fuOptions & ETO_GLYPH_INDEX) {
		if(!GetTextExtentPointI(hCanvasDC, (LPWORD)lpString, (int)cbString, &textSize)) {
			_THROW(ETOE_GETTEXTEXTENT);
		}
	} else {
		//GetTextExtentPointWはTeraPad on win2kで正しく描画されない
		//→ビットマップフォントのTransformに対応してない？
		if(!lpDx) {
			lpDxTemp = (int*)calloc(sizeof(int), cbString);
			if(!lpDxTemp) {_THROW(ETOE_GETTEXTEXTENT);}
		}
		if(!_RealGetTextExtentPoint32W(hCanvasDC, lpString, (int)cbString, &textSize, &tm, lpDxTemp)) {
			_THROW(ETOE_GETTEXTEXTENT);
		}
	}
//***************************** 字間行間の設定 ******************************
	if(lpDx) {
		textSize.cx = 0;
		if(fuOptions & ETO_PDY) {
			for(UINT i=0; i < cbString; textSize.cx += lpDx[i],i+=2);
		} else {
			for(UINT i=0; i < cbString; textSize.cx += lpDx[i++]);
		}
	}

//***************************** スラント幅 ******************************
	// イタリック時のスラント幅を足す(構造体の名前がABC･･･
	// 参考: http://bugzilla.mozilla.gr.jp/show_bug.cgi?id=3253
	if (tm.tmItalic) {
		ABC abcWidth={0, 0, 0};
		GetCharABCWidthsW(hdc, lpString[cbString-1], lpString[cbString-1], &abcWidth);
		textSize.cx += tm.tmOverhang - abcWidth.abcC;
	}

//***************************** 最終的な文字のサイズチェック ******************************
	if (g_MaxHeight > 0) {
		//max 26pixel
		if(textSize.cy * xformNoTrns.eM22 >= g_MaxHeight) {
			_THROW(ETOE_LARGESIZE);//throw large size
		}
	}

//***************************** 縦書き時の処理 ******************************
// まだ実装してないので、暫定的に使えるようにエラーを投げる
	if (lf.lfEscapement != 0) {_THROW(ETOE_ROTATEFONT);}

//***************************** 最終的な矩形の計算 ******************************
	//rectangle
	//trace(L"OrigTextSize=%d %d\n", textSize.cx, textSize.cy);
	//trace(L"OrigCursor=%d %d\n", curPos.x, curPos.y);
	{
		UINT horiz;
		UINT vert;
		horiz = align & (TA_LEFT|TA_RIGHT|TA_CENTER);
		vert  = align & (TA_BASELINE|TA_TOP);//TA_BOTTOM

		if(horiz == TA_CENTER) {
			rc.left  = curPos.x - div_ceil(textSize.cx, 2);
			rc.right = curPos.x + div_ceil(textSize.cx, 2);
			//no move
		}
		else if(horiz == TA_RIGHT) {
			rc.left  = curPos.x - textSize.cx;
			rc.right = curPos.x;
			curPos.x -= textSize.cx;//move pos
		}
		else {
			rc.left  = curPos.x;
			rc.right = curPos.x + textSize.cx;
			curPos.x += textSize.cx;//move pos
		}
		if(vert == TA_BASELINE) {
			TEXTMETRIC& metric = tm;
			rc.top = curPos.y - metric.tmAscent;
			rc.bottom = curPos.y + metric.tmDescent;
			//trace(L"ascent=%d descent=%d\n", metric.tmAscent, metric.tmDescent);
		}
		else {
			rc.top = curPos.y;
			rc.bottom = curPos.y + textSize.cy;
		}
		canvasPos.x = canvasPos.y = 0;

//***************************** クリッピングの計算 ******************************
		if(lprc && (fuOptions & ETO_CLIPPED)) {
			if(rc.left < lprc->left) {
				canvasPos.x -= div_ceil( (int)((lprc->left - rc.left) * xformScale.eM11), g_Scale);
				rc.left = lprc->left;
			}
			if(rc.right > lprc->right) {
				if(horiz == TA_RIGHT)
					canvasPos.x -= div_ceil( (int)((rc.right - lprc->right) * xformScale.eM11), g_Scale);
				rc.right = lprc->right;
			}
			if(rc.top < lprc->top) {
				canvasPos.y -= div_ceil( (int)((lprc->top - rc.top) * xformScale.eM22), g_Scale);
				rc.top = lprc->top;
			}
			if(rc.bottom > lprc->bottom) {
				rc.bottom = lprc->bottom;
			}
			//trace(L"ClipRect=%d %d %d %d\n", lprc->left, lprc->top, lprc->right, lprc->bottom);
		}
	}

//***************************** 矩形からDIBを作成 ******************************
	destPos.x = rc.left;
	destPos.y = rc.top;
	destSize.cx = rc.right - rc.left;
	destSize.cy = rc.bottom - rc.top;
	canvasSize.cx = (int)(destSize.cx * xformScale.eM11);
	canvasSize.cy = (int)(destSize.cy * xformScale.eM22);
	bufferSize.cx = (int)(destSize.cx * xformNoTrns.eM11);
	bufferSize.cy = (int)(destSize.cy * xformNoTrns.eM22);

	//trace(L"MovedCursor=%d %d\n", curPos.x, curPos.y);
	//trace(L"TargetRect=%d %d %d %d\n", rc.left, rc.top, rc.right, rc.bottom);
	//trace(L"DestPos=%dx%d Size=%dx%d\n", destPos.x, destPos.y, destSize.cx, destSize.cy);
	//trace(L"CanvasPos=%dx%d Size=%dx%d\n", canvasPos.x, canvasPos.y, canvasSize.cx, canvasSize.cy);

	if(destSize.cx < 1 || destSize.cy < 1) {_THROW(ETOE_NOAREA); }//幅か高さが0ですやん

	hBmp1 = pCache->CreateDIB(NUM_ETO_CANVAS,canvasSize.cx, canvasSize.cy, 1, &lpCanvasPixels);
	if(!hBmp1) {_THROW(ETOE_CREATEDIB);}
	hPrevBmp1 = SelectObject(hCanvasDC, hBmp1);
	hBmp32 = pCache->CreateDIB(NUM_ETO_BUFFER,bufferSize.cx, bufferSize.cy, 32, &lpBufferPixels);
	if(!hBmp32) {_THROW(ETOE_CREATEDIB);}
	hPrevBmp32 = SelectObject(hBufferDC, hBmp32);

//***************************** 背景の塗りつぶし/BitBlt ******************************
	{
		const BOOL fillrect = (lprc && (fuOptions & ETO_OPAQUE));//背景を塗りつぶすかどうか

		//clear bitmap
		if(fillrect || GetBkMode(hdc) == OPAQUE) {
			FillBuffer(pCache, lpBufferPixels,bufferSize.cx,bufferSize.cy, GetBkColor(hdc));
			if(fillrect) {ORIG_ExtTextOutW(hdc, 0,0, ETO_OPAQUE, lprc, NULL, 0, NULL);}
		}
		else {
			SetBkMode(hBufferDC, TRANSPARENT);
			BitBlt(hBufferDC,0,0,destSize.cx,destSize.cy, hdc,destPos.x,destPos.y, SRCCOPY);
		}
	}

//***************************** 文字色/アラインの設定 ******************************
//	SetTextColor(hCanvasDC, GetTextColor(hdc));
	SetTextAlign(hCanvasDC, TA_LEFT | TA_TOP);

//***************************** 文字描画 ******************************
	//textout
	ZeroCanvas(pCache, lpCanvasPixels, canvasSize.cx,canvasSize.cy);

//	for (UINT w=0; w<=g_Weight; w++) {
	ORIG_ExtTextOutW(hCanvasDC, canvasPos.x, canvasPos.y, fuOptions, NULL, lpString, cbString, lpDx ? lpDx:lpDxTemp);
//	}

//***************************** 縮小/エッジ強調 ******************************
	//SaveDIB(L"a0.bmp", canvasSize.cx, canvasSize.cy, lpPixels);

	#if defined(__MMX__)
		switch(g_Scale) {
		case 8:
			ReductDIB_Q8_mmx(pCache, lpBufferPixels, lpCanvasPixels, canvasSize.cx, canvasSize.cy, GetTextColor(hdc));
			break;
		default:
			ReductDIB(pCache, lpBufferPixels, lpCanvasPixels, canvasSize.cx, canvasSize.cy, GetTextColor(hdc));
			break;
		}	
	#elif defined(__SSE2__)
		switch(g_Scale) {
		case 8:
			ReductDIB_Q8_sse2(pCache, lpBufferPixels, lpCanvasPixels, canvasSize.cx, canvasSize.cy, GetTextColor(hdc));
			break;
		default:
			ReductDIB(pCache, lpBufferPixels, lpCanvasPixels, canvasSize.cx, canvasSize.cy, GetTextColor(hdc));
			break;
		}	
	#else
		ReductDIB(pCache, lpBufferPixels, lpCanvasPixels, canvasSize.cx, canvasSize.cy, GetTextColor(hdc));
	#endif
	//EnhanceEdges(pCache, lpPixels, canvasSize.cx, canvasSize.cy);

	//SaveDIB(L"a1.bmp", canvasSize.cx, canvasSize.cy, lpPixels);

//***************************** 描画先にbitblt ******************************
//	SetWorldTransform(hBufferDC, &xformNoTrns);
	BitBlt(hdc, destPos.x, destPos.y, destSize.cx, destSize.cy, hBufferDC, 0,0, SRCCOPY);

//***************************** カーソル移動 ******************************
	if(align & TA_UPDATECP) {MoveToEx(hdc, curPos.x, curPos.y, NULL);}

//***************************** エラー処理 ******************************
_CATCH();
	free(lpDxTemp);
	if(hPrevBmp1) SelectObject(hCanvasDC, hPrevBmp1);
	if(hPrevBmp32) SelectObject(hBufferDC, hPrevBmp32);
	if(hPrevFont) SelectObject(hCanvasDC, hPrevFont);
	if(error == ETOE_OK) return TRUE;
	
	return ORIG_ExtTextOutW(hdc, nXStart, nYStart, fuOptions, lprc, lpString, cbString, lpDx);
}

//EOF
