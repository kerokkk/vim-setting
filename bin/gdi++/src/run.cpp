#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#pragma comment(linker, "/subsystem:windows,5.0 /defaultlib:kernel32.lib /defaultlib:user32.lib")

typedef void (CALLBACK *RUNDLLPROCA)(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
typedef void (CALLBACK *RUNDLLPROCW)(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);
#ifdef _UNICODE
typedef RUNDLLPROCW		RUNDLLPROC;
static const char c_szGDIPlusPlusRunDLL[] = "GDIPlusPlus_RunDLLW";
#else
typedef RUNDLLPROCA		RUNDLLPROC;
static const char c_szGDIPlusPlusRunDLL[] = "GDIPlusPlus_RunDLL";
#endif

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	if(!*lpCmdLine) {
		MessageBox(NULL,
			L"gdi++.exe <file> argument ...\n"
			L"\n"
			L"EXE, �܂��̓V���[�g�J�b�g�Ȃǂ� �h���b�v���Ă�������."
			,L"gdi++", MB_OK|MB_ICONINFORMATION);
		return -1;
	}

	HINSTANCE hinstDLL = LoadLibrary(_T("GDI++.DLL"));
	if(!hinstDLL) {
		MessageBox(NULL, _T("GDI++.DLL�����[�h�ł��܂���"), NULL, MB_ICONHAND);
		return 1;
	}

	union {
		FARPROC		pfn;
		RUNDLLPROC	pfnRunDLL;
	};
	pfn = GetProcAddress(hinstDLL, c_szGDIPlusPlusRunDLL);
	if(!pfn) {
		TCHAR szMessage[256];
		wsprintf(szMessage, _T("GetProcAddress() ���֐� %hs �Ŏ��s���܂���"), c_szGDIPlusPlusRunDLL);
		MessageBox(NULL, szMessage, NULL, MB_ICONHAND);
		return 2;
	}

	//MSKB�́��͊ԈႢ(���[�h�������W���[����HINSTANCE)
	//hinst - DLL �̃C���X�^���X �n���h��
	pfnRunDLL(NULL, hInstance, lpCmdLine, nCmdShow);
	return 0;
}
