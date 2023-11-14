/*
     An FDIS compliant Operator "new"

http://www.codeguru.com/cpp_mfc/FDIS_new.shtml
--------------------------------------------------------------------------------
This article was contributed by Ray Brown.
Microsoft KB article Q167733 points out that Visual C++'s implementation of
operator new adopts non-FDIS compliant behavior in that it does not throw an
exception in the case of an allocation failure. This may cause problems if not
with your own code, then with third-party C++ code that becomes merged into your
translation units. Third-party inline and template functions are especially
susceptible to malfunction under the non-FDIS compliant semantics. In fact,
Microsoft's own STL implementation will fail should new ever return NULL;
its allocator does not expect this.

The KB article suggests a workaround that involves calling _set_new_handler.
This is a good solution if you link statically with the C/C++ runtime library.
But static linking is often an unpopular option among C++ developers on the Win32
platform as it increases module size and can cause a variety of other difficulties
(e.g., KB Q126646, Q193462). Calling _set_new_handler from a module which links
dynamically with the CRT can cause other difficulties, however, as the new handler
is kept in a global variable by the CRT. Different modules in a process might thus
attempt to install different new handlers, and failures will result.

A naive FDIS compliant implementation of operator new, which might resort to malloc
or some other low-level allocation routine, has the disadvantage of changing the
behavior of new on the Win32 platform, especially when it comes to diagnostic support
in debug mode. Unfortunately, from our own implementation of new we cannot now call
CRT's new, since it becomes hidden as the compiler and linker resolve new to our own
implementation. In addition, we cannot simply copy the CRT's implementation as it
makes use of CRT functions that are not exported.

In what follows we present an implementation of ::operator new that does not suffer
from any of the problems outlined above. This solution's key consists of locating
CRT's operator new address dynamically, and avoiding the repeated overhead of module
search through the use of a static object.

Note that if your module links with MFC, then you should not adopt this solution.
MFC provides its own operator new, which throws a CMemoryException* on allocation
failure. This is necessary as MFC exception handlers expect all exceptions to be
derived from MFC's CException base class. While this has the potential of upsetting
third-party code for similar reasons as the FDIS non-compliance mentioned above,
such code often works as long as some exception is thrown from new and NULL is never
returned. Microsoft's STL appears to fall in this category. Code which expects
std::bad_alloc on new failure cannot co-exist peacefully with MFC in a module.

----------
// StdNew.cpp
/*
	Adding this file to a project or, preferred when possible, linking with
	StdNew.obj from a subproject causes the ::operator new
	to assume standard FDIS C++ behavior: the operator will throw
	std::bad_alloc on failure and never return NULL.

	You will not want to link with this file from an MFC project, since the
	MFC library requires that a different exception be thrown on failure and
	arranges for such behavior.

	Note: we choose this approach over setting the new handler, since the new
		 handler function is kept in the runtime library on a per-process
		 basis. Attempting to control the handler could thus lead to
		 contention among dll's that each link dynamically to the runtime
		 library.
*/

//#include "stdafx.h"
#include <tchar.h>

#ifndef	_WIN32_WCE	// [
#include <crtdbg.h>
#else				// ][
#include <windows.h>

#include "WServer.h"
#endif				// ]

#ifdef _AFX
#error ANSI operator new must not be used in MFC project
#endif

#ifndef _MSC_VER
#error This implementation of ANSI operator new is appropriate for Win32 platforms only
#endif

#ifndef _DLL
#error Project must link dynamically with the C++ runtime library
#endif

class COpNewCrtCacher
{
	COpNewCrtCacher();
	COpNewCrtCacher(const COpNewCrtCacher&);                // not impl
	COpNewCrtCacher& operator = (const COpNewCrtCacher&);   // not impl

public:
	typedef void* (__cdecl *t_OpNewFcn)(size_t);
	static t_OpNewFcn GetCrtOpNew();

private:
	static t_OpNewFcn s_pfCrtOpNew;

	// The following static object ensures that GetCrtOpNew is called during
	// module initialization time and hence threading issues do not arise
	// when multiple threads may call into ::operator new simultaneously
	static COpNewCrtCacher s_GlobalModuleOpNewCrtCacherInitializer;
};

COpNewCrtCacher::COpNewCrtCacher()
{
	// Cached pointer to Crt operator new is set when the static object is
	// constructed or when someone calls ::operator new, whichever comes
	// first. In either case, we avoid threading issues.
	GetCrtOpNew();
}

/*
	This member function along with s_pfCrtOpNew are static so that they can
	continue to be used past the time of destruction of the static object.
*/
COpNewCrtCacher::t_OpNewFcn COpNewCrtCacher::GetCrtOpNew()
{
	if (s_pfCrtOpNew)
		return s_pfCrtOpNew;

	// Name of C++ run time library dll
#   ifdef _DEBUG
	const LPCTSTR sCrtName = _T("MSVCRTD.DLL");
#   else
	const LPCTSTR sCrtName = _T("MSVCRT.DLL");
#   endif

	// Get Crt handle
	///HMODULE hCrt = GetModuleHandle(sCrtName);
    HMODULE hCrt = NULL;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN, sCrtName,&hCrt);

#ifndef	_WIN32_WCE	// [
	_ASSERTE (hCrt);

	// Retrieve function pointer to Crt operator new
	s_pfCrtOpNew = reinterpret_cast <t_OpNewFcn>
				   (GetProcAddress(hCrt, "??2@YAPAXI@Z"));
	_ASSERTE (s_pfCrtOpNew);
#else				// ][
	if ( hCrt )
	{
		// Retrieve function pointer to Crt operator new
		s_pfCrtOpNew = reinterpret_cast <t_OpNewFcn>
					   (GetProcAddress(hCrt, L"??2@YAPAXI@Z"));
	}
	else
		s_pfCrtOpNew = NULL;

#endif				// ]

	return s_pfCrtOpNew;
}

COpNewCrtCacher::t_OpNewFcn COpNewCrtCacher::s_pfCrtOpNew;
COpNewCrtCacher COpNewCrtCacher::s_GlobalModuleOpNewCrtCacherInitializer;

/*
void*  __cdecl operator new(size_t nSize) throw(std::bad_alloc)
{
	printf("new operator function called at %d of %s\n",__LINE__,__FILE__);

	// Call the operator new in the Crt
	void* pResult = COpNewCrtCacher::GetCrtOpNew()(nSize);

	// If it returned NULL, throw the exception
	if (! pResult)
		throw std::bad_alloc();

	// Otherwise return pointer to allocated memory
	return pResult;
}
*/

WSERVER_API void * globalNew(size_t nSize)
#ifndef	_WIN32_WCE	// [
throw(std::bad_alloc)
#endif				// ]
{
	//D(printf("globalNew function called at %d of %s\n",__LINE__,__FILE__);)

	// Call the operator new in the Crt
	void* pResult = COpNewCrtCacher::GetCrtOpNew()(nSize);

#ifndef	_WIN32_WCE	// [
	// If it returned NULL, throw the exception
	if (! pResult)
		throw std::bad_alloc();
#endif				// ]

	// Otherwise return pointer to allocated memory
	return pResult;
}

WSERVER_API void globalDelete(void * ptr)
{
	//D(printf("globalDelete function called at %d of %s with ptr= 0x%lx\n",__LINE__,__FILE__,ptr);)
	delete ptr;
}