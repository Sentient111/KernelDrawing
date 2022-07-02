# KernelDrawing
Drawing from kernelmode without any hooks

-DESCRIPTION-
All the examples I have seen so far that call the windows gdi functions hook a gdi function that gets called often to get a valid win32 thread value.
In this project I achieve the same by spoofing the win32 thread value (and some other things) to bypass the security checks in the kernel gdi functions.

This is only a proof of concept so it does not include any other drawing functions other than a box but I have provided every needed function (as far as I know) to get everything else to work. 

-NOTES-
I have only provided the right nt offsets for my windows version (21h1). You can get the correct offsets for your windows version from vergiliusproject (https://www.vergiliusproject.com/kernels/x64/Windows%2010%20%7C%202016). 
Every offset that might change across windows versions is in Nt.h and has comments on where to find them.

-USAGE-
Compile this in x64 release and set Sign mode to off. You can load it with a driver manual mapper such as KDmapper (https://github.com/TheCruZ/kdmapper). 

When loading the driver it should look like this:
![image](https://img001.prntscr.com/file/img001/WyqwZTXpT9y0fXayNb9-fQ.png)
