MakeDirectories:
	@echo off
	@echo -- CALBUT DE SLIPAVIERE --
	xbmkdir xe:\$(KK_XENON_DEPLOYPATH)
	xbmkdir xe:\$(KK_XENON_DEPLOYPATH)\shaders
	
All:
	@echo off
!IF !defined(XBECOPY_SUPPRESS_COPY)
	
	@xbcp /t /y .\XeSharedDefines.h xe:\$(KK_XENON_DEPLOYPATH)\shaders\ 
    @xbcp /t /y .\Shaders\*.hlsl xe:\$(KK_XENON_DEPLOYPATH)\Shaders\ 
!else
	@echo -- Skipped copying shaders because XBECOPY_SUPPRESS_COPY=1 --
!endif

All_PC:
	@echo off
!IF !defined(XBECOPY_SUPPRESS_COPY)	
 
	xcopy /y /q /r .\XeSharedDefines.h $(KK_DEPLOYPATH)\Shaders\  
    xcopy /y /q /r .\Shaders\*.hlsl $(KK_DEPLOYPATH)\Shaders\  

!else
	@echo -- Skipped copying shaders because XBECOPY_SUPPRESS_COPY=1 --
!endif
