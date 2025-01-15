cls

cl.exe /c /EHsc Window.c

rc.exe Resource.rc

link.exe Window.obj Resource.res Kernel32.lib User32.lib GDI32.lib /SUBSYSTEM:WINDOWS

Window.exe
