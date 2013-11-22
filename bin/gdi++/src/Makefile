TARGET_EXE = ../gdi++.exe
TARGET_DLL = ../generic/gdi++.dll
TARGET_DLL_MMX = ../mmx/gdi++.dll
TARGET_DLL_SSE2 = ../gdi++.dll

.SUFFIXES: .cpp .obj .rc .res

!ifdef debug
CPPFLAGS  = /Od /GZ /Zi /MTd /D "DEBUG" /D "_DEBUG"
LINKFLAGS = /incremental:no /debug
!else
CPPFLAGS  = /Ox /MT
LINKFLAGS = 
!endif

all: $(TARGET_EXE) $(TARGET_DLL) $(TARGET_DLL_MMX) $(TARGET_DLL_SSE2)

$(TARGET_EXE): run.obj
	link /nologo /out:$@ run.obj

$(TARGET_DLL): hook.obj override.obj rundll.obj gdidll.res
	link /dll /nologo $(LINKFLAGS) /map /out:$@ hook.obj override.obj rundll.obj gdidll.res
$(TARGET_DLL_MMX): hook.obj override_mmx.obj rundll.obj gdidll.res
	link /dll /nologo $(LINKFLAGS) /map /out:$@ hook.obj override_mmx.obj rundll.obj gdidll.res
$(TARGET_DLL_SSE2): hook.obj override_sse2.obj rundll.obj gdidll.res
	link /dll /nologo $(LINKFLAGS) /map /out:$@ hook.obj override_sse2.obj rundll.obj gdidll.res

.cpp.obj:
	cl /nologo $(CPPFLAGS) /Gz /GF /GA /W3 /Fo$@ /c $<
override_mmx.obj: override.cpp ReductDIB_Q8_mmx.cpp
	cl /nologo $(CPPFLAGS) /Gz /GF /GA /W3 -D__MMX__ /Fooverride_mmx.obj /c override.cpp
override_sse2.obj: override.cpp ReductDIB_Q8_sse2.cpp
	cl /nologo $(CPPFLAGS) /Gz /GF /GA /W3 -D__SSE2__ /arch:SSE2 /Fooverride_sse2.obj /c override.cpp
	
.rc.res:
	rc /l 0x411 $<

clean:
	@if exist hook.obj del hook.obj
	@if exist override.obj del override.obj
	@if exist override_mmx.obj del override_mmx.obj
	@if exist override_sse2.obj del override_sse2.obj
	@if exist gdidll.res del gdidll.res
	@if exist run.obj del run.obj
	@if exist rundll.obj del rundll.obj
	@if exist vc60.pdb del vc60.pdb
	@if exist vc80.pdb del vc80.pdb
	@if exist "..\gdi++.pdb" del "..\gdi++.pdb"
	@if exist "..\gdi++.exp" del "..\gdi++.exp"
	@if exist "..\gdi++.lib" del "..\gdi++.lib"
	@if exist "..\gdi++.map" del "..\gdi++.map"
	@if exist "..\mmx\gdi++.map" del "..\mmx\gdi++.map"
	@if exist "..\mmx\gdi++.exp" del "..\mmx\gdi++.exp"
	@if exist "..\mmx\gdi++.lib" del "..\mmx\gdi++.lib"
	@if exist "..\generic\gdi++.map" del "..\generic\gdi++.map"
	@if exist "..\generic\gdi++.exp" del "..\generic\gdi++.exp"
	@if exist "..\generic\gdi++.lib" del "..\generic\gdi++.lib"
	@if exist "..\gdi++.dll.manifest" del "..\gdi++.dll.manifest"
	@if exist "..\gdi++.exe.manifest" del "..\gdi++.exe.manifest"
