del *.exe
del *.obj
del *.txt

cls
cl.exe /c /EHsc Assignment_01.c

rc.exe Resource.rc

link.exe Resource.res gdi32.lib user32.lib /SUBSYSTEM:WINDOWS Assignment_01.obj /out:"./OGL.exe"

OGL .exe
