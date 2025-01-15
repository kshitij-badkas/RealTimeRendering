del *.exe
del *.obj
del *.txt

cls
cl.exe /c /EHsc VK.c

rc.exe VK.rc

link.exe VK.obj VK.res gdi32.lib user32.lib /SUBSYSTEM:WINDOWS /out:"./VK.exe"

VK.exe
