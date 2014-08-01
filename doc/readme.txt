Micromouse project build instructions:

To build in Visual Studio 2008,
1. Simply open Micromouse.sln under prj\VS2008\Micromouse
2. Click build solution or press F7
3. Go to bin\VS2008\x86chk and look for Micromouse.exe


Eclipse Setup Instructions
==========================
Checkout svn://someprofs.org/chan/trunk into a folder (eg. C:\Micromouse)
	
Windows:
	1. Install WinAVR, which contains all the GNU-ported AVR executables
		1.1 Install into C:\WinAVR or C:\Program Files\WinAVR
	2. Install AVR Studio, which contains the part description files
	3. Get Eclipse for C/C++, extract it somewhere, no installation needed
	4. Get the AVR Eclipse plugin, extract to the eclipse folder
	5. Open up Eclipse
		5.1 Set workspace to the Eclipse folder under the prj folder
			eg. C:\Micromouse\prj\Eclipse
		5.2 Make sure the AVR plugin is set up correctly
			Go to Window | Preferences | AVR | Paths
			The 'current values' should be filled in and looks something
			like this:
			AVR-GCC				C:\WinAVR\bin
			GNU make			C:\WinAVR\utils\bin
			AVR Header Files	C:\WinAVR\avr\invclude
			AVRDude				C:\WinAVR\bin
			Atmel Part Des...	C:\Program Files\Atmel\AVR Tools\Partdescriptionfiles
		5.3 Right click inside the Project Explorer window and select Import...
				or go to File | Import...
			Select General | Existing Projects into Workspace...
			Next to Select root directory, click Browse...
			Browse to the prj/Eclipse folder and click OK
			Select the "Micromouse" project in the Projects window and click Finish
		5.4 Set the MM_ROOT path variable
			Go to File | New | Folder
			Click the <<Advanced button
			Check Link to folder in the file system checkbox
			Click Variables...
			Click New...
			Name: MM_ROOT
			Click Folder... and browse to the trunk folder containing the 
				src, inc, lib, prj folders eg. C:\Micromouse
			Click OK
			Click OK
			Click Cancel
		5.5 Click the project inside the Project Explorer and refresh
		5.6 Right click project | Build Configurations | Set Active | Release
	6. Troubleshooting
		If you have problem building the first time, make sure Eclipse is compiling using avr-gcc
		and not avr-g++. Right click project | Properties | C/C++ Build | Settings | Tool Settings.
		For AVR Assembler, AVR Compiler, and AVR C Linker, check that Command: is avr-gcc.
Linux:
	1. Get AVR-GCC and optionally avrdude and the part description files copied from AVR Studio
	2. Follow step 3-5 above
		