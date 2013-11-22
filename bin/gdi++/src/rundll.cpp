#define _CRT_SECURE_NO_DEPRECATE 1
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN 1
#define UNICODE  1
#define _UNICODE 1
#include <Windows.h>
#include <ShellApi.h>
#include <ComDef.h>
#include <ShlObj.h>

#include "override.h"

// _vsnwprintf用
#include <wchar.h>		
#include <stdarg.h>

#define for if(0);else for

#include <tlhelp32.h>
#include <tchar.h>
// win2k以降
#pragma comment(linker, "/subsystem:windows,5.0")
#pragma comment(linker, "/defaultlib:kernel32.lib /defaultlib:shell32.lib")

//手抜き
//VC以外でコンパイルする時は注意
#pragma comment(linker, "/export:GDIPlusPlus_RunDLL@16=GDIPlusPlus_RunDLL")
#pragma comment(linker, "/export:GDIPlusPlus_RunDLLW@16=GDIPlusPlus_RunDLLW")
#pragma comment(linker, "/export:AllGDIPlusPlus_RunDLL@16=AllGDIPlusPlus_RunDLL")
#pragma comment(linker, "/export:AllGDIPlusPlus_RunDLLW@16=AllGDIPlusPlus_RunDLLW")
#pragma comment(linker, "/export:GetMsgProc@12=GetMsgProc")

HINSTANCE g_hinstDLL;

#define GUID_ALLGDIPLUSPLUS		_T("{FD21E0BB-24FB-41d4-8DB0-081D2E4DF0EC}")
static void errmsg(HWND hwnd, LPCWSTR format, ...);
static bool inject_dll(HANDLE process, LPVOID entrypoint);
static LPVOID get_entrypoint(HWND hwnd, LPCWSTR path);
static size_t  wcscpy_arg(LPWSTR buffer, size_t size, LPCWSTR str);
static LPWSTR  get_commandline(HWND hwnd, LPCWSTR lpszCmdLine, LPWSTR o_app, LPWSTR o_workdir);
void UnloadALL();
LPWSTR _StrDupAtoW(LPCSTR pszMB, int cchMB = -1);

extern "C" {
void CALLBACK GDIPlusPlus_RunDLL(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
void CALLBACK GDIPlusPlus_RunDLLW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);
void CALLBACK AllGDIPlusPlus_RunDLL(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);
void CALLBACK AllGDIPlusPlus_RunDLLW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow);
LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);
}

void CALLBACK GDIPlusPlus_RunDLL(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	LPWSTR lpszCmdLineW = _StrDupAtoW(lpszCmdLine);
	if (lpszCmdLineW) {
		GDIPlusPlus_RunDLLW(hwnd, hinst, lpszCmdLineW, nCmdShow);
		delete[] lpszCmdLineW;
	}
}

void CALLBACK GDIPlusPlus_RunDLLW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow)
{
#ifdef _DEBUG
//	MessageBoxA(hwnd,"dbg",0,0);
#endif
	CoInitialize(NULL);

	WCHAR app [MAX_PATH];
	WCHAR workdir [MAX_PATH];
	LPWSTR cmdline = get_commandline(hwnd, lpszCmdLine, app, workdir);
	LPVOID entrypoint = NULL;

	if(cmdline) {
		entrypoint = get_entrypoint(hwnd, app);
	}
	if(entrypoint) {
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);

		struct _CREATE_MAGIC {
			int dummy;
			FOURCC magic;
			_CREATE_MAGIC() : dummy(0), magic(CREATEPROCESS_MAGIC) {}
		};
		_CREATE_MAGIC magic;
		si.cbReserved2 = sizeof(int) + sizeof(FOURCC);
		si.lpReserved2 = (LPBYTE)&magic;

		if(!CreateProcess(app,cmdline, NULL,NULL, FALSE, CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, NULL, workdir[0]? workdir: NULL, &si, &pi)) {
			errmsg(hwnd, L"実行できません.\n%s", cmdline);
		}
		else if(!inject_dll(pi.hProcess, entrypoint)) {
			TerminateProcess(pi.hProcess, 0);
			errmsg(hwnd, L"DLLの挿入に失敗しました.\n%s", cmdline);
		}
		else {
			ResumeThread(pi.hThread);
		}
		if(pi.hThread) CloseHandle(pi.hThread);
		if(pi.hProcess) CloseHandle(pi.hProcess);
	}

	if(cmdline) delete [] cmdline;
	CoUninitialize();
}

void CALLBACK AllGDIPlusPlus_RunDLL(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	LPWSTR lpszCmdLineW = _StrDupAtoW(lpszCmdLine);
	if (lpszCmdLineW) {
		AllGDIPlusPlus_RunDLLW(hwnd, hinst, lpszCmdLineW, nCmdShow);
		delete[] lpszCmdLineW;
	}
}

void CALLBACK AllGDIPlusPlus_RunDLLW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow)
{
	HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, GUID_ALLGDIPLUSPLUS);
	if (hEvent) {
		//常駐済み
		if(MessageBox(hwnd, _T("終了しますか?"), _T("GDI++"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
			SetEvent(hEvent);
			if(MessageBox(hwnd, _T("ついでに全てのGDI++.DLLをアンロードしますか?"), _T("GDI++"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
				UnloadALL();
				RedrawWindow(GetDesktopWindow(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
			}
			return;
		}
		CloseHandle(hEvent);
	}
	
	HHOOK hhk = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hinstDLL, 0);
	if (!hhk) {
		MessageBox(hwnd, _T("フックに失敗しました"), NULL, MB_ICONHAND);
		return;
	}
	PostMessage(HWND_BROADCAST, WM_NULL, 0, 0);
	RedrawWindow(GetDesktopWindow(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
	
	if (lstrcmpi(lpszCmdLine, _T("-p")) == 0) {
		hEvent = CreateEvent(NULL, TRUE, FALSE, GUID_ALLGDIPLUSPLUS);
		if (hEvent) {
			WaitForSingleObject(hEvent, INFINITE);
		}
	}
	UnhookWindowsHookEx(hhk);
}

static void errmsg(HWND hwnd, LPCWSTR format, ...)
{
	WCHAR  buffer [1024];
	va_list val;
	va_start(val, format);
	_vsnwprintf(buffer, 1024, format, val);
	va_end(val);
	MessageBox(hwnd, buffer, L"gdi++ ERROR", MB_OK|MB_ICONSTOP);
}

// 止めているプロセスのエントリポイントを書き換え、先にLoadLibraryする
static bool inject_dll(HANDLE process, LPVOID entrypoint)
{
	// gdi++.dllのパス
	WCHAR dllpath [MAX_PATH];
	UINT len = GetModuleFileName(g_hinstDLL, dllpath, MAX_PATH);
//	wcscpy(dllpath+len-4, L".dll");

	//EntryPointから opcode2へjmp
	BYTE opcode1 [8] = {
		0xB8,0,0,0,0,	// mov  eax, 00000000h
		0xFF,0xE0,	// jmp  eax
		0xCC,		// int  3
	};
	//EntryPointを戻して LoadLibraryして EntryPointへjmp
	BYTE  opcode2 [32+16+sizeof(dllpath)] = {
		0x8B, 0x48, 0x24,	// mov  ecx, dword ptr [eax+36]
		0x8B, 0x50, 0x28,	// mov  edx, dword ptr [eax+40]
		0x89, 0x11,		// mov  dword ptr [ecx+0], edx
		0x8B, 0x50, 0x2C,	// mov  edx, dword ptr [eax+44]
		0x89, 0x51, 0x04,	// mov  dword ptr [ecx+4], edx
		0x51,			// push ecx
		0x8B, 0xD0,		// mov  edx, eax
		0x83, 0xC2, 0x30,	// add  edx, 48
		0x52,			// push edx
		0xFF, 0x50, 0x20,	// call dword ptr [eax+32]
		0x59,			// pop  ecx
		0xFF, 0xE1,		// jmp  ecx
		0xCC,0xCC,0xCC,0xCC,0xCC,//int3...
	};

	HMODULE kernel32 = GetModuleHandle(L"Kernel32.dll");
	LPVOID  loadlibrary = GetProcAddress(kernel32, "LoadLibraryW");
	DWORD   n;

	//opcode2の後にデータを置く
	memcpy(opcode2+32, &loadlibrary, sizeof(LPVOID));
	memcpy(opcode2+36, &entrypoint,  sizeof(LPVOID));
	if(!ReadProcessMemory(process, entrypoint, opcode2+40, sizeof(opcode1), &n))//opcode1分(8byte)保存する
		return false;
	wcscpy((LPWSTR)(opcode2+48), dllpath);//LoadLibraryWに渡すパス

	//opcode2, メモリ確保して転送
	LPVOID mem = VirtualAllocEx(process, NULL, sizeof(opcode2), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!mem)
		return false;
	if(!WriteProcessMemory(process, mem, opcode2, sizeof(opcode2), &n))
		return false;

	//opcode1, EntryPointを書き換える
	memcpy(opcode1+1, &mem, sizeof(LPVOID));
	if(!VirtualProtectEx(process, entrypoint, sizeof(opcode1), PAGE_EXECUTE_READWRITE, &n))
		return false;
	if(!WriteProcessMemory(process, entrypoint, opcode1, sizeof(opcode1), &n))
		return false;
	return true;
}

// EXEファイル(PE)のヘッダーを読み、
// エントリポイントのアドレスを返す
static LPVOID get_entrypoint(HWND hwnd, LPCWSTR path)
{
	LPVOID entrypoint = NULL;
	DWORD len;
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS pe;
	HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(file == INVALID_HANDLE_VALUE) {
		errmsg(hwnd, L"ファイルが開けません.\n%s", path);
	}
	else if(!ReadFile(file, &dos, sizeof(dos), &len, NULL) ||
		dos.e_magic != IMAGE_DOS_SIGNATURE ||
		!SetFilePointer(file, dos.e_lfanew, NULL, FILE_BEGIN) ||
		!ReadFile(file, &pe, sizeof(pe), &len, NULL) ||
		pe.Signature != IMAGE_NT_SIGNATURE) {
		errmsg(hwnd, L"EXEファイルではない.\n%s", path);
	}
	else {
		#pragma warning(push)
		#pragma warning(disable:4312)
		entrypoint = (LPVOID)(pe.OptionalHeader.ImageBase + pe.OptionalHeader.AddressOfEntryPoint);
		#pragma warning(pop)
	}
	if(file != INVALID_HANDLE_VALUE)
		CloseHandle(file);
	return entrypoint;
}

// Spaceが入っていたら""でくくる
// 無ければそのまま
static size_t  wcscpy_arg(LPWSTR buffer, size_t size, LPCWSTR str)
{
	LPCWSTR c = str;
	for( ; *c > L' ' ; c++);
	return _snwprintf(buffer, size, (!*c)? L"%s ": L"\"%s\" ", str);
}

// １つ目の引数だけファイルとして扱う。
// 実行するコマンド、EXE、Workdirを返す
//
// コマンドは こんな感じで連結されます。
//  exe linkpath linkarg cmdarg2 cmdarg3 cmdarg4 ...
//
static LPWSTR  get_commandline(HWND hwnd, LPCWSTR lpszCmdLine, LPWSTR o_app, LPWSTR o_workdir)
{
	o_app[0] = o_workdir[0] = L'\0';

	int     argc = 0;
	LPWSTR* argv = CommandLineToArgvW(lpszCmdLine, &argc);
	if(!argv) {
		return NULL;
	}
	if(argc <= 0) {
		MessageBox(hwnd,
			L"rundll32.exe gdi++.dll,GDIPlusPlus_RunDLL <file> argument ...\n"
			L"\n"
			L"コマンドライン引数の渡し方が間違っています.\n"
			,L"gdi++", MB_OK|MB_ICONINFORMATION);
		return NULL;
	}

	LPWSTR o_cmdline = NULL;
	LPWSTR file = argv[0];
	WCHAR  fullpath [MAX_PATH];
	WCHAR  linkpath [MAX_PATH];
	WCHAR  linkarg [MAX_PATH];
	linkarg[0] = L'\0';

	size_t len = (int) wcslen(file);
	if(len >= 4 && _wcsicmp(file+len-4, L".lnk") == 0) {
		// ショートカットから引数を得る
		bool result = true;
		IShellLink* shlink = NULL;
		IPersistFile* persist = NULL;

		if(S_OK != CoCreateInstance(CLSID_ShellLink,NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&shlink) ||
		   S_OK != shlink->QueryInterface(IID_IPersistFile, (void**)&persist) ||
		   S_OK != persist->Load(file, STGM_READ) ||
		   S_OK != shlink->Resolve(NULL, SLR_UPDATE) ||
		   S_OK != shlink->GetPath(linkpath, MAX_PATH, NULL, 0) ||
		   S_OK != shlink->GetArguments(linkarg, MAX_PATH)) {
			errmsg(hwnd, L"ショートカットの読み取り失敗\n%s", argv[0]);
			result = false;
		}
		else {
			shlink->GetWorkingDirectory(o_workdir, MAX_PATH);
			file = linkpath;
		}

		if(persist) persist->Release();
		if(shlink)  shlink->Release();
		if(!result) return NULL;
	}

	if(FindExecutable(file, NULL, o_app) < (HINSTANCE)32) {
		errmsg(hwnd, L"実行ファイルが見つからない\n%s", file);
		return NULL;
	}
	DWORD filetype = GetLastError();
	if(filetype == ERROR_ALREADY_EXISTS)
		filetype = 0;
	if(filetype == 0) {
		WCHAR tmp [MAX_PATH];
		if(GetFullPathName(file, MAX_PATH, tmp, NULL) &&
		   GetShortPathName(tmp, fullpath, MAX_PATH))
			file = fullpath;
	}

	//cmdline文字数
	len = wcslen(o_app) +32;
	if(filetype == 0)
		len += wcslen(file) +1;
	len += wcslen(linkarg) +1;
	for(int i=1 ; i < argc ; i++)
		len += wcslen(argv[i]) +3;

	//cmdline作成
	o_cmdline = new WCHAR [len];
	size_t n = wcscpy_arg(o_cmdline, len, o_app);
	if(filetype == 0)
		n += wcscpy_arg(o_cmdline+n,len-n, file);
	if(linkarg[0])
		n += wcscpy_arg(o_cmdline+n,len-n, linkarg);
	for(int i=1 ; i < argc ; i++)
		n += wcscpy_arg(o_cmdline+n,len-n, argv[i]);

	//workdirの処理
	if(!o_workdir[0]) {
		LPWSTR c1 = wcsrchr(file, L'\\');
		LPWSTR c2 = wcsrchr(file, L'/');
		if(c1 < c2) c1 = c2;
		if(c1) {
			*c1 = L'\0';
			GetFullPathName(file, MAX_PATH, o_workdir, NULL);
		}
	}
	if(!(GetFileAttributes(o_workdir) & FILE_ATTRIBUTE_DIRECTORY)) {
		o_workdir[0] = L'\0';
	}

	//fprintf(stdout, "app='%S'\n", o_app);
	//fprintf(stdout, "cmd='%S'\n", o_cmdline);
	//fprintf(stdout, "dir='%S'\n", o_workdir);

	LocalFree(argv);
	return o_cmdline;
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	//何もしない
	return CallNextHookEx(NULL, code, wParam, lParam);
}

LPWSTR _StrDupAtoW(LPCSTR pszMB, int cchMB /*= -1*/)
{
	if (!pszMB) {
		return NULL;
	}
	const int cchWC = MultiByteToWideChar(CP_ACP, 0, pszMB, cchMB, NULL, 0);
	LPWSTR pszWC = new WCHAR[cchWC + 1];
	if (!pszWC) {
		return NULL;
	}
	MultiByteToWideChar(CP_ACP, 0, pszMB, cchMB, pszWC, cchWC + 1);
	return pszWC;
}

BOOL _RemoteFreeLibrary(DWORD dwProcessId, HMODULE hModule, BOOL bForceUnload = FALSE)
{
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
									PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
									TRUE, dwProcessId);
	if (!hProcess)
		return FALSE;

	DWORD ret = 0;
	DWORD dwThreadId;
	HANDLE hThread;
	
	for (;;) {
		hThread = CreateRemoteThread(hProcess, NULL, 0,
									(LPTHREAD_START_ROUTINE)FreeLibrary,
									hModule, 0, &dwThreadId);

		if(!hThread) {
			ret = FALSE;
			break;
		}

		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &ret);
		CloseHandle(hThread);

		if(!bForceUnload)
			break;
		if(!ret) {
			ret = TRUE;
			break;
		}
	}

	CloseHandle(hProcess);
	return ret;
}

void UnloadALL()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD dwMyProcessId = GetCurrentProcessId();
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	BOOL ret = Process32First(hSnapshot, &pe);
	
	while (ret && GetLastError() != ERROR_NO_MORE_FILES) {
		//自分自身はアンロードさせない
		if (pe.th32ProcessID != dwMyProcessId) {
			HANDLE hSnapModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
			if (hSnapModule != INVALID_HANDLE_VALUE) {
				MODULEENTRY32 me = { sizeof(MODULEENTRY32) };
				ret = Module32First(hSnapModule, &me);
				while (ret && GetLastError() != ERROR_NO_MORE_FILES) {
//					MessageBox(NULL, me.szExePath, me.szModule, MB_OK);
					if(lstrcmpi(me.szModule, _T("gdi++.dll")) == 0) {
						_RemoteFreeLibrary(pe.th32ProcessID, me.hModule, TRUE);
					}
					SetLastError(0);
					ret = Module32Next(hSnapModule, &me);
				}
				CloseHandle(hSnapModule);
			}
		}
		SetLastError(0);
		ret = Process32Next(hSnapshot, &pe);
	}
	CloseHandle(hSnapshot);
}
