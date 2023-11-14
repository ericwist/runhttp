/* 
 * 
 */ 


#include <windows.h>
#include "CServlet.h" 

	

	

/**
 * A base class which extends CServlet to represent a plugable
 * CServlet generated from a .esp file which handles protocol requests
 * that are mapped to it.
 */
class _espCServlet1 : public CServlet
{
public:
/**
 * Get the plugin specific version string. Callers must NOT modify or
 * delete this constant string.
 *
 * @ret TCHAR&, a reference to the version string.
 */
const TCHAR& _espCServlet1::getVersionString()
{
	return *TEXT("2.0");
}
	/**
	 * Service a protocol request.
	 *
	 * @param request Request&, a protocol specific Request subclass.
	 * @param response Response&, a protocol specific Response subclass.
	 * @ret int, Servlet specific return code.
	 */
	int service(Request& request, Response& response)
	{

	response << "		Hello World!\r\n";
	response << "	";
		//D(cout << __FILE__ << " service() END" << endl;)

		return 0;
	}
};

CServlet * createCServlet()
{
	return new _espCServlet1();
}

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	BOOL ret = TRUE;

    switch (ul_reason_for_call)
	{
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return ret;
}

