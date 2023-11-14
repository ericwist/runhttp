# NMAKE File, for esp file compilation
!IF "$(CFG)" == ""
CFG=test2.esp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to test2.esp - Win32 Debug.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "test2.esp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "Release\test2.esp.dll"


CLEAN :
	-@erase "$(INTDIR)\test2.esp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\test2.esp.exp"
	-@erase "$(OUTDIR)\test2.esp.lib"
	-@erase "Release\test2.esp.dll"

"$(OUTDIR)" :
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "\dev\xerces-c-src2_1_0\src\xercesc" /I "\dev\xerces-c-src2_1_0\src" /I "\projects\CCode\Unified\PluginServer" /I "\projects\CCode\Unified\FrameWork" /I "\projects\CCode\Unified\WServer" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test2.esp.bsc" 
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\test2.esp.pdb" /machine:I386 /def:".\test2.esp.def" /out:"Release/test2.esp.dll" /libpath:"\dev\xerces-c-src2_1_0\build\win32\vc6\release" /libpath:"\projects\CCode\Unified\PluginServer\Release" /libpath:"\projects\CCode\Unified\FrameWork\Release" /libpath:"\projects\CCode\Unified\WServer\Release"	"PluginServer.lib" \
	"FrameWork.lib" \
	"wserver.lib" \
	"xerces-c_2.lib" \

DEF_FILE= \
	".\test2.esp.def"
LINK32_OBJS= \
	"$(INTDIR)\test2.esp.obj" \

"Release\test2.esp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "test2.esp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "Debug\test2.esp.dll"


CLEAN :
	-@erase "$(INTDIR)\test2.esp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\test2.esp.exp"
	-@erase "$(OUTDIR)\test2.esp.lib"
	-@erase "$(OUTDIR)\test2.esp.pdb"
	-@erase "Debug\test2.esp.dll"
	-@erase "Debug\test2.esp.ilk"

"$(OUTDIR)" :
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "\dev\xerces-c-src2_1_0\src\xercesc" /I "\dev\xerces-c-src2_1_0\src" /I "\projects\CCode\Unified\PluginServer" /I "\projects\CCode\Unified\FrameWork" /I "\projects\CCode\Unified\WServer" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test2.esp.bsc" 
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\test2.esp.pdb" /debug /machine:I386 /def:".\test2.esp.def" /out:"Debug/test2.esp.dll" /pdbtype:sept /libpath:"\dev\xerces-c-src2_1_0\build\win32\vc6\release" /libpath:"\projects\CCode\Unified\PluginServer\Release" /libpath:"\projects\CCode\Unified\FrameWork\Release" /libpath:"\projects\CCode\Unified\WServer\Release"	"PluginServer.lib" \
	"FrameWork.lib" \
	"wserver.lib" \
	"xerces-c_2.lib" \

DEF_FILE= \
	".\test2.esp.def"
LINK32_OBJS= \
	"$(INTDIR)\test2.esp.obj" \

"Debug\test2.esp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF
