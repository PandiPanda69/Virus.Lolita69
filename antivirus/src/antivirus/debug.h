#ifndef __ANTIVIRUS__DEBUG__FILE__
#define __ANTIVIRUS__DEBUG__FILE__

#define DEBUG


#ifdef DEBUG
	#include <iostream>
	#include <unistd.h>
	#define TRACE(__str)  std::cout << "[TRACE] " << __FILE__ << ":" << __LINE__ << " pid:" << getpid() << "\t" << __str << std::endl;
#else
	#define TRACE(__str)	; 
#endif


#endif
