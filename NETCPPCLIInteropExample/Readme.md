# Linking to Native C++ Library

## Refereces
* [C++/CLI wrapper for native DLL (CppCLINativeDllWrapper)](http://code.msdn.microsoft.com/windowsdesktop/CppCLINativeDllWrapper-29c32acd)
* [Creating a C++ CLR Project](http://msdn.microsoft.com/en-us/library/ms235635.aspx)
* [Using C++ (managed/unmanaged) interop](http://msdn.microsoft.com/en-us/library/2x8kf7zx.aspx)

## Create Solution
Set up a single solution.

## Setup Native CPP Client Project
First project was a win32 console executable.
Compiled and ran with out a problem.

## Add Native CPP DLL Project
Added second project- win32dll type in the new project wizzard.

Created an exported class with a single method.

## Integrate CPP DLL with Native CPP 
Tried using the library from this native C++ executable.
Compilation failed, couldn't find header.
Added extra include directory to project properties.
Comipled, but didn't link.

Added native library as a reference (via project properties.)
Compiled and linked.

Possible alternative: add path to native library output via extra linker/library paths in the project properties.
NB- suspect adding reference is the better method to use; there are build options to copy the library dll when the native app is built.
Note: not sure if these options work in this way- not tested it yet.

## Created Visual C++ CLR Class Library Project
Added the include directory for the native library's headers.
Added a reference to the native library (as it's in the same solution.) Otherwise would have had to add a library path as well.

*Note:* `marshal.h` has to be `#included` first or else you get compilation (redefinition) errors.

## Created Managed CPP Test  - `CLRCPPTestExecutable`
Just added .net style reference to the C++ CLR Wrapper library and everything worked.
All DLLs got copied to the right places without intervention.

## Created C# Test App
Similar process to the C++ CLR wrapper- just add a reference.

However, C# projects have their own target output directory, unlike C++ projects which seem to always target a solution level output directory.

So there will be a runtime error when running the app as it can't load the native library.

A simple fix is to add a post-build step something like:

`copy /Y $(SolutionDir)\$(ConfigurationName)\NativeCPPLibrary.* $(TargetDir)`

Noting that `ConfigurationName` has to be consistent across all dependant projects.

Also when the C++ CLR wrapper is `clean`ed, the `DLL` is removed, and you will see Intellisense errors. Rebuild to make everything happy.

