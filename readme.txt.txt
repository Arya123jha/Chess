This project uses SFML 2.6.2 with Visual Studio (C++17).
Follow the steps below to set up SFML environment.

1. Download and Extract SFML

Go to the official SFML website: https://www.sfml-dev.org/download.php.
Download SFML for Visual C++ 17 (choose 32-bit or 64-bit Visual Studio project).
Extract the downloaded ZIP file to a location on computer (for example:C:\SFML-2.6.2)

2. Open Project Properties in Visual Studio
In Visual Studio, open the project.
In Solution Explorer, right-click the name of project → select Properties.
At the top of the Properties window:
Configuration: choose All Configurations (so settings apply to both Debug and Release at once).
Platform: make sure it matches SFML download (x86 for 32-bit or x64 for 64-bit).

3. Add SFML Include and Library Paths

A. Add Include Directory
Go to: C/C++ → General → Additional Include Directories
Add the path to SFML’s include folder, e.g(C:\SFML-2.6.2\include)

B. Add Library Directory
Go to: Linker → General → Additional Library Directories
Add the path to SFML’s lib folder, e.g.(C:\SFML-2.6.2\lib)

4. Link SFML Libraries
This step differs for Debug and Release builds.
To switch between them:
At the top of the Properties window, change Configuration from All Configurations to either Debug or Release.

Debug Configuration
Go to: Linker → Input → Additional Dependencies
Add the following:
sfml-graphics-d.lib
sfml-window-d.lib
sfml-system-d.lib

(Note the -d suffix for debug libraries.)

Release Configuration
Go to: Linker → Input → Additional Dependencies
Add the following:
sfml-graphics.lib
sfml-window.lib
sfml-system.lib

5. Copy SFML DLL Files
The program needs the SFML DLLs at runtime.For that,
Open the folder:(C:\SFML-2.6.2\bin)

For Debug
Copy the files (sfml-graphics-2-d.dll, sfml-window-2-d.dll, sfml-system-2-d.dll).
Paste them into:
project’s Debug folder (where the Debug .exe is generated).

For Release
Copy the files (sfml-graphics-2.dll, sfml-window-2.dll, sfml-system-2.dll).
Paste them into:
project’s Release folder (where the Release .exe is generated).

By default, these folders are inside the project under:
<ProjectName>\x64\Debug
<ProjectName>\x64\Release

(or x86 instead of x64 if 32-bit).
NOTE:to create debug and release folder the project should run at least once for both debug and release (with a simple sample c++ code) 
If setup was correct, your SFML project will compile and run.


LINKS:
https://www.sfml-dev.org/download/sfml/2.6.2/
https://www.sfml-dev.org/tutorials/3.0/getting-started/visual-studio/


...........................................................................................
The project contains some files that needs to be included in the project.
They are:
Assets and arial-font that contains the images and text needed for formatting the project(should in the same location as the project)
All the project files and folders are kept in the following git profile.
https://github.com/Arya123jha/Chess