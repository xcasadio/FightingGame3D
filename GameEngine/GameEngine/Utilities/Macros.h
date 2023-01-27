#ifndef _MACROS_H_
#define _MACROS_H_

#include <dxerr.h>

//#pragma comment( lib, "dxerr.lib" )

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = (x); if( FAILED(hr) ) { DXTrace( __FILE__, (DWORD)__LINE__, hr, /*L*/#x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return DXTrace( __FILE__, (DWORD)__LINE__, hr, /*L*/#x, true ); } }
#endif
#else
#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#endif

#ifndef SAFE_DELETE
#	define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
#endif   

#ifndef SAFE_DELETE_ARRAY
#	define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = NULL; } }
#endif 

#ifndef SAFE_RELEASE
#	define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = NULL; } }
#endif

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

#endif //_MACROS_H_
