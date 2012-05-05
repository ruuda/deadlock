#ifndef _DEADLOCK_CORE_WIN32_EXPORT_H_
#define _DEADLOCK_CORE_WIN32_EXPORT_H_

#if defined (_WIN32) 
     #define  _export __declspec(dllexport) 
#else /* defined (_WIN32) */ 
  #define _export 
#endif 

#endif