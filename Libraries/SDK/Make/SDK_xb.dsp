# Microsoft Developer Studio Project File - Name="SDK_xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=SDK_xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDK_xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDK_xb.mak" CFG="SDK_xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDK_xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "SDK_xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "SDK_xb - Xbox Tuning" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SDK_xb_r"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\Sources\timer\profiler\xbox" /D "_DX8" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SDK_xb_r.lib"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SDK_xb_d"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\Sources\timer\profiler\xbox" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SDK_xb_d.lib"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SDK_xb___Xbox_Tuning"
# PROP BASE Intermediate_Dir "SDK_xb___Xbox_Tuning"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SDK_xb_t"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\Sources\timer\profiler\xbox" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\Sources\timer\profiler\xbox" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /YX /FD /G6 /Ztmp /c
# SUBTRACT CPP /X
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\SDK_xb_d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\SDK_xb_t.lib"

!ENDIF 

# Begin Target

# Name "SDK_xb - Xbox Release"
# Name "SDK_xb - Xbox Debug"
# Name "SDK_xb - Xbox Tuning"
# Begin Group "BIGfiles"

# PROP Default_Filter ""
# Begin Group "LOAding"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOA.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOA.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOAdefs.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOAdefs.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOAerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOAread.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\LOAding\LOAread.h
# End Source File
# End Group
# Begin Group "SAVing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BIGfiles\SAVing\SAVdefs.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\SAVing\SAVdefs.h
# End Source File
# End Group
# Begin Group "BIG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIG.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIG.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGcheck.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGcheck.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGchecksum.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGchecksum.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGcomp.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGcomp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGdefs.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGdefs.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGexport.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGexport.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGfat.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGfat.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGgroup.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGgroup.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGio.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGio.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGkey.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGkey.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmdfy_dir.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmdfy_dir.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmdfy_file.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmdfy_file.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmerge.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGmerge.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGopen.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGopen.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGread.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGread.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGspecial.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\BIGspecial.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\lzoconf.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\minilzo.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\minilzo.h
# End Source File
# End Group
# Begin Group "IMPort"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BIGfiles\IMPort\IMPbase.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\IMPort\IMPbase.h
# End Source File
# End Group
# Begin Group "VERsion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Check.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Check.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Number.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update10.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update11.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update12.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update13.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update14.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update15.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update16.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update17.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update18.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update19.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update2.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update20.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update21.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update22.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update23.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update24.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update25.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update26.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update27.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update28.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update29.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update3.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update30.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update31.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update32.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update33.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update34.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update4.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update5.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update6.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update7.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update8.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BIGfiles\VERsion\VERsion_Update9.cpp
# End Source File
# End Group
# End Group
# Begin Group "TABles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TABles\TABles.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TABles\TABles.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TABles\TABvars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TABles\TABvars.h
# End Source File
# End Group
# Begin Group "TIMer"

# PROP Default_Filter ""
# Begin Group "PROfiler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\PerfTest.cpp

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PRO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PRO.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\PRO_xb.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PROdefs.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PROdefs.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PROdisplay.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\PROdisplay.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\xbapp.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbFont.Cpp

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbFont.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\xbinput.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XBInterface.Cpp

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbResource.Cpp

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbResource.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbUtil.Cpp

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\PROfiler\XBox\XbUtil.h

!IF  "$(CFG)" == "SDK_xb - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Debug"

!ELSEIF  "$(CFG)" == "SDK_xb - Xbox Tuning"

!ENDIF 

# End Source File
# End Group
# Begin Group "TIM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TIMer\TIM.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\TIM.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\TIMdefs.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TIMer\TIMdefs.h
# End Source File
# End Group
# End Group
# Begin Group "LINKs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\LINks\LINK.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINK.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKstruct.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKstruct_reg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKstruct_reg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKtoed.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LINks\LINKtoed.h
# End Source File
# End Group
# Begin Group "INOut"

# PROP Default_Filter ""
# Begin Group "xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\INOut\xbox\xbINOsaving.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\xbox\xbINOsaving.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\INOut\INO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INO.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOfile.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOfile.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOjoystick.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOjoystick.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOkeyboard.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOkeyboard.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOsaving.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOsaving.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INOut\INOstruct.h
# End Source File
# End Group
# Begin Group "MATHs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MATHs\MATH.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATH.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHasm.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHextern.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHfloat.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHlong.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrix.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixAdvanced.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixAdvanced.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixBase.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixBase.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixRot.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixRot.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixScale.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixScale.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixTrans.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixTransform.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHmatrixTransform.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHquat.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHvars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHvars.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATHs\MATHvector.h
# End Source File
# End Group
# Begin Group "BASe"

# PROP Default_Filter ""
# Begin Group "CLIbrary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLIerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLIfile.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLImem.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLIstr.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLIwin.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\CLIbrary\CLIxxx.h
# End Source File
# End Group
# Begin Group "ERRors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERR.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERR.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERRasser.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERRdefs.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERRdefs.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\ERRors\ERRstr.h
# End Source File
# End Group
# Begin Group "MEMory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEM.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEMdyn.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEMLog.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEMLog.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEMpro.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\MEMory\MEMpro.h
# End Source File
# End Group
# Begin Group "BAS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BASe\BAS.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BAS.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BASarray.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BASarray.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BASlog.c
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BASlog.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BASsys.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BASe\BAStypes.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
