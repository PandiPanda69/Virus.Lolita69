#ifndef __ANTIVIRUS__DEBUG__FILE__
#define __ANTIVIRUS__DEBUG__FILE__

#define DEBUG


#ifdef DEBUG
	#include <iostream>
	#define TRACE(__str)  std::cout << "[TRACE] " << __FILE__ << ":" << __LINE__ << "\t" << __str << std::endl;
#else
	#define TRACE(__str)	; 
#endif


#endif
