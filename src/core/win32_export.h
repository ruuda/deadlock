#ifndef _DEADLOCK_CORE_WIN32_EXPORT_H_
#define _DEADLOCK_CORE_WIN32_EXPORT_H_

#if defined (_WIN32)
	#if defined(libdeadlock_EXPORTS)
		#define  _export __declspec(dllexport)
	#else
		#define _export __declspec(dllimport)
	#endif
#else /* defined (_WIN32) */ 
  #define _export 
#endif 

#endif