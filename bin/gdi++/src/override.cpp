/*******************************************************************************
*	filename	:	override.cpp
*	version		:	20060927
*	subversion	:	1005
*	
*	copyright	:	��0x0D/0x20I
*					460	(gdi++.dll�X��)
*					168	(gdi++.dll�X�� 2pt)
*					Pico��PicoKHZGNI
*	
*	history :
*		2006/09/27	�{��Ver.20060927
*		2006/09/30	168���ɂ����ǁB�ڍׂ�memo.txt�B
*		2006/10/02	���̃R�����g�̍쐬 by Pico��PicoKHZGNI
*		2006/10/05	1Bit�����_�����O���ȂǑ��� by Pico��PicoKHZGNI
*******************************************************************************/

/*******************************************************************************
*	�v���v���Z�b�T
*******************************************************************************/
#define MAX_EXCLUDES 32
//#define __MMX__	//64bit�p�b�N�h�������Z���g�p����BMMX-Pentium�ȍ~
//#define __SSE__	//MMX2�̒ǉ��A128bit�p�b�N�h�������Z�BPen3�ȍ~
//#define __SSE2__	//128bit�p�b�N�h�������Z�BPen4�AAthlon64�ȍ~
//#define __SSE3__	//SSE2��������Ɖ��ǁBPrescott�AVenice�ȍ~
//#define __SSSE3__	//SSE3��������Ɖ��ǁBCore2�ȍ~
//#define __SSE4__	//

/*******************************************************************************
*	�w�b�_�t�@�C���̃C���N���[�h - 
*******************************************************************************/
#include "override.h"

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

/*******************************************************************************
*	�O���[�o�����ۂ��ϐ�&�萔�̐錾
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
*	�v�Z�⏕
*******************************************************************************/
int div_ceil(int a, int b){
	if(a % b) return (a>0)? (a/b+1): (a/b-1);
	return a / b;
}

/*******************************************************************************
*	������̃n�b�V����
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
			return !(hash != x.hash || _wcsicmp(name, x.name) != 0); // VC2005�ł�wcsicmp�͌x�����o�܂��B
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
*	INI�t�@�C���̓ǂݍ���
*******************************************************************************/
void LoadSettings(HMODULE hModule){
	WCHAR szFileName[MAX_PATH + 1] = L"";
	// DLL�̃t���p�X���擾
	GetModuleFileName(hModule, szFileName, MAX_PATH);
	// �g���q��".ini"�ɒu��������
	lstrcpy(szFileName + lstrlen(szFileName) - 3, L"ini");
	// �e��ݒ�ǂݍ���
	// INI�t�@�C���̗�:
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

	// OS�̃o�[�W������XP�ȍ~���ǂ���
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	g_IsWinXPorLater = ((osvi.dwMajorVersion > 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));

	// [Exclude]�Z�N�V�������珜�O�t�H���g���X�g��ǂݍ���
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
*	Is�`�`�֐�
*******************************************************************************/
// �t�H���g���A�E�g���C���t�H���g�����`�F�b�N����
BOOL IsOutlineFont(HDC hdc)
{
	OUTLINETEXTMETRIC otm = { sizeof(OUTLINETEXTMETRIC) };
	return GetOutlineTextMetrics(hdc, otm.otmSize, &otm);
}

// �t�H���g�����O���X�g�ɓ����Ă��邩���`�F�b�N����
BOOL IsFontExcluded(const LPCWSTR lpszFaceName){
	StringHash* exclist = g_ExcludeList;
	StringHash str(lpszFaceName);
	for(UINT i=0 ; i < MAX_EXCLUDES && exclist[i].Hash() ; i++) {
		if(exclist[i] == str)
			return TRUE;
	}
	return FALSE;
}
// �񃆃j�R�[�h��
BOOL IsFontExcluded(const LPCSTR lpszFaceName){
	int bufferLength = MultiByteToWideChar(CP_ACP, 0, lpszFaceName, LF_FACESIZE, 0, 0);
	LPWSTR lpszUnicode = (LPWSTR)malloc(bufferLength * 2);
	MultiByteToWideChar(CP_ACP, 0, lpszFaceName, LF_FACESIZE, lpszUnicode, bufferLength);
	BOOL b = IsFontExcluded(lpszUnicode);
	free(lpszUnicode);
	return b;
}

// �v���Z�X�����O���X�g�ɓ����Ă��邩���`�F�b�N����
BOOL IsProcessExcluded()
{
	//���̕ӎQ�Ɓ�http://www.catch22.net/tuts/undoc01.asp
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

// �L����DC���ǂ������`�F�b�N����
BOOL IsValidDC(const HDC hdc){
	if (GetDeviceCaps(hdc, TECHNOLOGY) != DT_RASDISPLAY) return FALSE;
	if (!IsOutlineFont(hdc)) return FALSE;	// �r�b�g�}�b�v�t�H���g��e��
	// �����Ńt�H���g�`�F�b�N���s��
	WCHAR szFaceName[LF_FACESIZE] = L"";
	GetTextFaceW(hdc, LF_FACESIZE, szFaceName);
	if (IsFontExcluded(szFaceName)) return FALSE;
	return TRUE;
}


/*******************************************************************************
*	DIB�̏k��
*******************************************************************************/
void  ReductDIB(ETOCache* pCache, BYTE* lpBufferPixels, BYTE* lpCanvasPixels, int CanvasWidth, int CanvasHeight,COLORREF FontColor)
{
	#define BufferPixelSize 4
	int BufferWidth = CanvasWidth /g_Scale; //�P�ʂ̓s�N�Z��
	int BufferHeight = CanvasHeight /g_Scale; //�P�ʂ̓s�N�Z��
	int BufferLineSize = pCache->Size(NUM_ETO_BUFFER).cx; //�P�ʂ̓s�N�Z��
	int CanvasLineSize = pCache->Size(NUM_ETO_CANVAS).cx; //�P�ʂ̓s�N�Z��
	int BufferLineSizeByte = BufferLineSize * BufferPixelSize; //�P�ʂ̓o�C�g
	int CanvasLineSizeByte = CanvasLineSize /8; //�P�ʂ̓o�C�g

	BYTE fcB = (BYTE)( (FontColor & 0x00FF0000) >> 16);
	BYTE fcG = (BYTE)( (FontColor & 0x0000FF00) >> 8 );
	BYTE fcR = (BYTE)( (FontColor & 0x000000FF)      );

	DWORD TargetBits;
	float Alpha;

	for(int i_y = 0;i_y<BufferHeight;i_y++){
		for(int i_x = 0;i_x<BufferWidth;i_x++){
			int BufferCurPos =		(i_x + i_y * BufferLineSize) * BufferPixelSize;	//Buffer�̑ΏۃA�h���X
			int CanvasCurPosBit =	(i_x + i_y * CanvasLineSize) * g_Scale;
			int CanvasCurPos =		CanvasCurPosBit /8;								//Canvas�̑ΏۃA�h���X
			int CanvasCurPosDiffBit = CanvasCurPosBit & 0x7;						//�r�b�g�̂���
			
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
*	ReductDIB�̍œK���o�[�W�����ǂݍ���
*******************************************************************************/
#if defined(__MMX__)
	#include "ReductDIB_Q8_mmx.cpp"	
#elif defined(__SSE2__)
	#include "ReductDIB_Q8_sse2.cpp"
#endif

/*******************************************************************************
*	Buffer��h��Ԃ�
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
*	Canvas���[���N���A
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
	if(width % size_alignment)width += size_alignment - (width % size_alignment);//128bit�A���C�������g
	width += 128/BitCount;//128bit�}�[�W�������

	if(dibSize[num].cx < width || dibSize[num].cy < height) {
		if(dibSize[num].cx > width)  width = dibSize[num].cx;
		if(dibSize[num].cy > height) height = dibSize[num].cy;
		BITMAPINFOHEADER bmiHeader = {sizeof(bmiHeader), width, -height, 1, BitCount, BI_RGB,};

		HBITMAP hbmpNew = CreateDIBSection(CreateDC(num), (CONST BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS, (LPVOID*)lplpPixels, NULL, 0);
		if(!hbmpNew)return NULL;

		//�������s������hbmpNew==NULL�̏ꍇ��z�肵�A���������Ƃ��̂݃L���b�V�����X�V
		if(hbmp[num])DeleteObject(hbmp[num]);

		DWORD address = (DWORD)*lplpPixels;
		if(address % 16)address += 16 - (address % 16);//128bit�A���C�������g
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
*	API�t�b�N�̂��߂̃v���v���Z�b�T
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

//firefox����
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32A, (HDC hdc, LPCSTR  lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
HOOK_DEFINE(gdi32.dll, BOOL, GetTextExtentPoint32W, (HDC hdc, LPCWSTR lpString, int cbString, LPSIZE lpSize), (hdc, lpString, cbString, lpSize))
#undef HOOK_DEFINE

/*******************************************************************************
*	API�t�b�N
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


// Windows 2000�p
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

//gdi32.dll/win2k���t�����ď����N�������֐�
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

// ANSI->Unicode�ɕϊ�����ExtTextOutW�ɓ�����ExtTextOutA�iWindows 2000�p�j
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
		free(lpDxW);	// free(NULL)�͖�薳�������ۏ؂���Ă����͂�
		return result;
	}
}

BOOL WINAPI _RealGetTextExtentPoint32A(HDC hdc, LPCSTR lpString, int cbString, LPSIZE lpSize, const TEXTMETRICA* lptm, INT* lpDx)
{
	//ANSI�łł�lptm��lpDx�͗��p���Ȃ�
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

//���m�ȃe�L�X�g�̃T�C�Y��Ԃ�
//����ɐ��m�ȕ`��ʒu���v�Z����lpDx�ɕԂ�
//(����lpDx��ExtTextOut�ɓn���ĕ`�悷��ƃY���������Ȃ�)
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
	//IMPL_ExtTextOutW����̌Ăяo���ɂ͊��Ɏ擾�ς݂�TEXTMETRIC���g����
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
*	�L���C��ExtTextOut�i���Д�j�̎���
*	BOOL ExtTextOut(
*	  HDC hdc,          // �f�o�C�X�R���e�L�X�g�̃n���h��
*	  int X,            // �J�n�ʒu�i��_�j�� x ���W
*	  int Y,            // �J�n�ʒu�i��_�j�� y ���W
*	  UINT fuOptions,   // �����`�̈�̎g�����̃I�v�V����
*	  CONST RECT *lprc, // �����`�̈�̓������\���̂ւ̃|�C���^
*	  LPCTSTR lpString, // ������
*	  UINT cbCount,     // ������
*	  CONST INT *lpDx   // �����Ԋu�̓������z��
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

//��O���h�L
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

	HDC		hCanvasDC = NULL;//�g�傳�ꂽ1bit
	SIZE	canvasSize;
	BYTE*	lpCanvasPixels  = NULL;
	HBITMAP	hBmp1      = NULL;
	HGDIOBJ	hPrevBmp1  = NULL;
	
	HDC		hBufferDC = NULL;//���̃T�C�Y��32bit
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

//***************************** �����̃`�F�b�N ******************************
	if(lpString == NULL || cbString == 0 || g_Quality==0) { _THROW(ETOE_INVALIDARG);}
	if(!IsValidDC(hdc)) { _THROW(ETOE_INVALIDHDC);}
	
//***************************** HDC�쐬 ******************************
	hCanvasDC = pCache->CreateDC(NUM_ETO_CANVAS);
	if(!hCanvasDC) {_THROW(ETOE_CREATEDC);}
	hBufferDC = pCache->CreateDC(NUM_ETO_BUFFER);
	if(!hBufferDC) {_THROW(ETOE_CREATEDC);}

//***************************** �`���ʒu�ƍ��W�ϊ� ******************************
	align = GetTextAlign(hdc);
	if(align & TA_UPDATECP) {GetCurrentPositionEx(hdc, &curPos);}

	GetWorldTransform(hdc, &xformNoTrns);
	if(abs(xformNoTrns.eM12) > 0.1f || abs(xformNoTrns.eM21) > 0.1f) {
		_THROW(ETOE_ROTATION);// ��]�ϊ��̓T�|�[�g�ł��Ȃ�
	}
	xformNoTrns.eDx = 0.0f; // �ړ��ϊ�����������
	xformNoTrns.eDy = 0.0f;
	xformScale.eM11 = xformNoTrns.eM11 * g_Scale;//scaling
	xformScale.eM22 = xformNoTrns.eM22 * g_Scale;
	SetGraphicsMode(hCanvasDC, GM_ADVANCED);
	SetWorldTransform(hCanvasDC, &xformScale);
//	SetGraphicsMode(hBufferDC, GM_ADVANCED);
	SetWorldTransform(hBufferDC, &xformNoTrns);

//***************************** ���݂̃t�H���g���擾 ******************************
	//copy font
	//�t�H���g����̓R�X�g���傫���A���x��GetCurrentObject��GetTextMetrics����̂�
	//�����������̂�1�x���������擾���Ďg����
	hCurFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
	if (!hCurFont) {
		// NULL�̏ꍇ��System��ݒ肵�Ă���
		hCurFont = (HFONT)GetStockObject(SYSTEM_FONT);
		if (!hCurFont) {_THROW(ETOE_SETFONT);}// System����NULL�̏ꍇ�̓G���[��Ԃ�
	}
	hPrevFont = SelectObject(hCanvasDC, hCurFont);
	GetTextMetrics(hdc, &tm);
	GetObject(hCurFont,sizeof(LOGFONT), &lf);

//***************************** �����Ԋu�̐ݒ� ******************************
	//GetTextExtentPoint�n��SetTextCharacterExtra��K�v�Ƃ���炵��
	SetTextCharacterExtra(hCanvasDC, GetTextCharacterExtra(hdc));
	//text size
	if(fuOptions & ETO_GLYPH_INDEX) {
		if(!GetTextExtentPointI(hCanvasDC, (LPWORD)lpString, (int)cbString, &textSize)) {
			_THROW(ETOE_GETTEXTEXTENT);
		}
	} else {
		//GetTextExtentPointW��TeraPad on win2k�Ő������`�悳��Ȃ�
		//���r�b�g�}�b�v�t�H���g��Transform�ɑΉ����ĂȂ��H
		if(!lpDx) {
			lpDxTemp = (int*)calloc(sizeof(int), cbString);
			if(!lpDxTemp) {_THROW(ETOE_GETTEXTEXTENT);}
		}
		if(!_RealGetTextExtentPoint32W(hCanvasDC, lpString, (int)cbString, &textSize, &tm, lpDxTemp)) {
			_THROW(ETOE_GETTEXTEXTENT);
		}
	}
//***************************** ���ԍs�Ԃ̐ݒ� ******************************
	if(lpDx) {
		textSize.cx = 0;
		if(fuOptions & ETO_PDY) {
			for(UINT i=0; i < cbString; textSize.cx += lpDx[i],i+=2);
		} else {
			for(UINT i=0; i < cbString; textSize.cx += lpDx[i++]);
		}
	}

//***************************** �X�����g�� ******************************
	// �C�^���b�N���̃X�����g���𑫂�(�\���̖̂��O��ABC���
	// �Q�l: http://bugzilla.mozilla.gr.jp/show_bug.cgi?id=3253
	if (tm.tmItalic) {
		ABC abcWidth={0, 0, 0};
		GetCharABCWidthsW(hdc, lpString[cbString-1], lpString[cbString-1], &abcWidth);
		textSize.cx += tm.tmOverhang - abcWidth.abcC;
	}

//***************************** �ŏI�I�ȕ����̃T�C�Y�`�F�b�N ******************************
	if (g_MaxHeight > 0) {
		//max 26pixel
		if(textSize.cy * xformNoTrns.eM22 >= g_MaxHeight) {
			_THROW(ETOE_LARGESIZE);//throw large size
		}
	}

//***************************** �c�������̏��� ******************************
// �܂��������ĂȂ��̂ŁA�b��I�Ɏg����悤�ɃG���[�𓊂���
	if (lf.lfEscapement != 0) {_THROW(ETOE_ROTATEFONT);}

//***************************** �ŏI�I�ȋ�`�̌v�Z ******************************
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

//***************************** �N���b�s���O�̌v�Z ******************************
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

//***************************** ��`����DIB���쐬 ******************************
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

	if(destSize.cx < 1 || destSize.cy < 1) {_THROW(ETOE_NOAREA); }//����������0�ł����

	hBmp1 = pCache->CreateDIB(NUM_ETO_CANVAS,canvasSize.cx, canvasSize.cy, 1, &lpCanvasPixels);
	if(!hBmp1) {_THROW(ETOE_CREATEDIB);}
	hPrevBmp1 = SelectObject(hCanvasDC, hBmp1);
	hBmp32 = pCache->CreateDIB(NUM_ETO_BUFFER,bufferSize.cx, bufferSize.cy, 32, &lpBufferPixels);
	if(!hBmp32) {_THROW(ETOE_CREATEDIB);}
	hPrevBmp32 = SelectObject(hBufferDC, hBmp32);

//***************************** �w�i�̓h��Ԃ�/BitBlt ******************************
	{
		const BOOL fillrect = (lprc && (fuOptions & ETO_OPAQUE));//�w�i��h��Ԃ����ǂ���

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

//***************************** �����F/�A���C���̐ݒ� ******************************
//	SetTextColor(hCanvasDC, GetTextColor(hdc));
	SetTextAlign(hCanvasDC, TA_LEFT | TA_TOP);

//***************************** �����`�� ******************************
	//textout
	ZeroCanvas(pCache, lpCanvasPixels, canvasSize.cx,canvasSize.cy);

//	for (UINT w=0; w<=g_Weight; w++) {
	ORIG_ExtTextOutW(hCanvasDC, canvasPos.x, canvasPos.y, fuOptions, NULL, lpString, cbString, lpDx ? lpDx:lpDxTemp);
//	}

//***************************** �k��/�G�b�W���� ******************************
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

//***************************** �`����bitblt ******************************
//	SetWorldTransform(hBufferDC, &xformNoTrns);
	BitBlt(hdc, destPos.x, destPos.y, destSize.cx, destSize.cy, hBufferDC, 0,0, SRCCOPY);

//***************************** �J�[�\���ړ� ******************************
	if(align & TA_UPDATECP) {MoveToEx(hdc, curPos.x, curPos.y, NULL);}

//***************************** �G���[���� ******************************
_CATCH();
	free(lpDxTemp);
	if(hPrevBmp1) SelectObject(hCanvasDC, hPrevBmp1);
	if(hPrevBmp32) SelectObject(hBufferDC, hPrevBmp32);
	if(hPrevFont) SelectObject(hCanvasDC, hPrevFont);
	if(error == ETOE_OK) return TRUE;
	
	return ORIG_ExtTextOutW(hdc, nXStart, nYStart, fuOptions, lprc, lpString, cbString, lpDx);
}

//EOF
