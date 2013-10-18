#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "4";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2013";
	static const char UBUNTU_VERSION_STYLE[] = "13.4";
	
	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 9;
	static const long BUILD = 3;
	static const long REVISION = 0;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 0;
	#define RC_FILEVERSION 0,9,3,0
	#define RC_FILEVERSION_STRING "0, 9, 3, 0\0"
	static const char FULLVERSION_STRING[] = "0.9.3.0";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
