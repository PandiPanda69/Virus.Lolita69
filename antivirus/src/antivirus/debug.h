#ifndef __ANTIVIRUS__DEBUG__FILE__
#define __ANTIVIRUS__DEBUG__FILE__

#define DEBUG


#ifdef DEBUG
	#define TRACE(__str)   debug_trace(__str, __FILE__, __LINE__)

	#include <iostream>
	
	using namespace std;

	void debug_trace(const char* str, const char* file, int line)
	{
		cout << "[TRACE] " << file << ":" << line << "\t" << str << endl;
	}
#else
	#define TRACE(__str)	; 
#endif


#endif
