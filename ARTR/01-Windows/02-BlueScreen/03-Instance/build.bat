del *.exe
del *.obj
del *.txt

cls

cl.exe /c /EHsc /I C:\VulkanSDK\Vulkan\Include VK.c

rc.exe VK.rc

link.exe VK.obj VK.res /LIBPATH:C:\VulkanSDK\Vulkan\Lib gdi32.lib user32.lib /SUBSYSTEM:WINDOWS /out:"./VK.exe"

VK.exe
