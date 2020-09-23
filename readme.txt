
Hi,

I was the founder of the ATONCE TECH compagny, but also mainly the CTO and spent a large part of my time coding.
Below are some pieces of code that i wrote myself or as the lead programmer.


ATMAKE
======

it's a global tool for the programer.
the basic idea is a tool helping the programer to be more efficient in his daily tasks, mastering the source code automatically
this tool is implemented in python 3 and can :
- generate marshalling code, from Lua tables to C and C to Lua
- generate unit tests
- generate visual 2008/2010, xcode project files and GNU makefiles
- build binaries from source directly (like scon)
- stringify C strings for a faster access and compare
- generate source code to link an implementation object to its interfaces (AOP paradigm, like IDL). This paradigm avoid large tree of inheritances to a clear set of interfaces of functionalities.

atmake could be called from the CL to do an action (packing some sources, building, ...).
or it could parse SIDL files describing some meta-information about the projects, the C++ classes or file records to generate.
you could find some examples of SIDL files in the SIDL directory.
I developped a visual studio 2008 and 2010 addin to support the SIDL files (syntax coloring, goto, hierarchical browser, ...)

The SIDL_generted_code directory contains samples of generated code by the atmake tool.
I provide you theses files to evaluate the quality of the outputed code.


NEOVA
=====

Neova is the first generation middleware of the ATONCE compagny.
It targeted the old gen plateforms like Wii, PSP, PS2, XB and PC without shading support.
You can find the documentation i wrote some years ago.
Neova was really bug free on all the platforms and was used to develop about 15 SKUs from 2004 to 2009.


SUGAR
=====

SUGAR is the second generation middleware i designed since 2 years.
it's a full framework of services in C++, i/o, memory, network, 2D and 3D, sound, lua integration.
it works on a large range of platforms next gen (ps3, psvita, pc, mac, ios, android).

some videos on youtube :
http://www.youtube.com/user/AtonceTechnologies


JAM EDITOR
==========

Is a pure LUA asset editor implementing the MVC pattern.
The goal was to provide an opened and scriptable worklow all in one tool.



Kind Regards,

Gerald.

