#ifndef NEW_H
#define NEW_H

#ifdef WIN32

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h> 

#ifdef _DEBUG
#ifndef NEW
/// Definition of NEW macro that can be traced by the debugger.
#define NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#else
#define NEW new
#endif  // _DEBUG

#else // !WIN32
#ifndef NEW
#define NEW new
#endif

#endif // WIN32

#endif // NEW_H
