/*
 * Various utilities.
 *
 * W.D.L 020526
 */

//#include "stdafx.h"

// ZZZ This file is also in FrameWork.dll. Need to consolidate. W.D.L 030510

#ifdef	_WIN32_WCE	// [
#include <windows.h>

_CRTIMP int __cdecl 	   _isctype(int, int);
#define isalpha(_c)      ( _isctype(_c,_ALPHA) )
#define isalnum(_c)      ( _isctype(_c,_ALPHA|_DIGIT) )
#define isdigit(_c)      ( _isctype(_c,_DIGIT) )
#endif				/ ]

/*
#include <tchar.h>                  // char macros
#include <stdio.h>                  // fprintf
#include <Wbemidl.h>
*/

#include "utils.h"

// Used in HttpCookie & HttpResponse for Headers.
char * daysAbbrev[] = 
{
	"Sun","Mon","Tues","Wed","Thu","Fri","Sat"
};

char * monthsAbbrev[] = 
{
	"Jan","Feb","Mar","Apr","May","Jun","Jul",
	"Aug","Sep","Oct","Nov","Dec"
};

//#define CVTFAILED _T("WideCharToMultiByte failed")
#define CVTBUFSIZE (309+40) /* # of digits in max. dp value + slop  (this size stolen from cvt.h in c runtime library) */
#define BLOCKSIZE (32 * sizeof(WCHAR))

//*****************************************************************************
// Function:   cvt
// Purpose:    Converts unicode to oem for console output
// Note:       y must be freed by caller
//*****************************************************************************
WSERVER_API WCHAR *cvt(const char *x, WCHAR **y)
{
	int dwRet, i;
	i = MultiByteToWideChar( CP_OEMCP,
		0,
		x,
		-1,
		NULL,
		NULL);

	// bug#2696 - the number returned by MultiByteToWideChar is the number
	// of WCHARs not the number of BYTEs needed
	//*y = (WCHAR *)calloc(i, sizeof(WCHAR));
	//*y = new WCHAR[i * sizeof(WCHAR)];
	*y = new WCHAR[i];

	if (*y)
	{
		dwRet = MultiByteToWideChar( CP_OEMCP,
			0,
			x,
			-1,
			*y,
			i);

		if (dwRet == 0)
		{
			//free(*y);
			delete[] *y;
			*y = NULL;
/*
			//*y = (WCHAR *)malloc(sizeof(CVTFAILED));
			*y = new WCHAR[sizeof(CVTFAILED)];
			if (*y)
			{
				memcpy(*y, CVTFAILED, sizeof(CVTFAILED));
			}
			else
			{
				printf("!!!cvt() out of memory!!!");
			}
*/
		}
	}
/*
	else
	{
		printf("!!!cvt() out of memory!!!");
	}
*/

	return *y;
}

//*****************************************************************************
// Function:   cvt
// Purpose:    Converts unicode to oem for console output
// Note:       y must be freed by caller
//*****************************************************************************
WSERVER_API char * cvt(const WCHAR *x, char **y)
{
	int dwRet, i;
	i = WideCharToMultiByte( CP_OEMCP,
		0,
		x,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	//*y = (char *)calloc(i, 1);
	*y = new char[i+1];

	if (*y)
	{
		dwRet = WideCharToMultiByte( CP_OEMCP,
			0,
			x,
			-1,
			*y,
			i,
			NULL,
			NULL);

		if (dwRet == 0)
		{
			//free(*y);
			delete[] *y;
			*y = NULL;
/*
			//*y = (char *)malloc(sizeof(CVTFAILED));
			*y = new char[sizeof(CVTFAILED)];

			if (*y)
			{
				memcpy(*y, CVTFAILED, sizeof(CVTFAILED));
			}
			else
			{
				printf("!!!cvt() out of memory!!!");
			}
*/
		}
	}
/*
	else
	{
		printf("!!!cvt() out of memory!!!");
	}
*/
	return *y;
}

WSERVER_API int getStringType(char * str)
{
	int	type = UndefinedStringType;

	if ( str )
	{
		int	len = strlen(str);

		while ( *str )
		{
			if ( isalnum(*str) )
			{
				type = AlphaNumericStringType;
				break;
			}
			if ( isalpha(*str) )
			{
				if ( type == DigitStringType )
				{
					type = AlphaNumericStringType;
					break;
				}
				type = AlphaStringType;
			}
			else if ( isdigit(*str) )
			{
				if ( type == AlphaStringType )
				{
					type = AlphaNumericStringType;
					break;
				}
				type = DigitStringType;
			}
			str++;
		}
	}

	return type;
}

#ifndef	_WIN32_WCE	// [
WSERVER_API BOOL stringToVariant(char * str, VARIANT& var)
{
	BOOL	ret = TRUE;

	VariantInit(&var);

	if ( !str )
	{
		var.vt = VT_NULL;
	}
	else if ( !*str )
	{
		var.vt = VT_EMPTY;
	}
	else
	{
		int	stringType = getStringType(str);

		switch ( stringType )
		{
			case AlphaNumericStringType:
			case AlphaStringType:
				if ( !stricmp(str,"true") || !stricmp(str,"false") )
				{
					var.vt = VT_BOOL;
					var.boolVal = (stricmp(str,"true") == 0);
				}
				else
				{
					WCHAR	* temp;
					
					cvt(str, &temp);

					if ( temp )
					{
						var.vt = VT_BSTR;
						var.bstrVal = SysAllocString(temp);
						//free(temp);
						delete[] temp;
					}
					else
						ret = FALSE;
				}
				break;

			case DigitStringType:
				var.vt = VT_INT;
				var.intVal = atoi(str);
				break;

			default:
				ret = FALSE;
		}
	}

	if ( !ret )
	{
		var.vt = VT_ERROR;
		var.scode = DISP_E_PARAMNOTFOUND;
	}

	return ret;
}

//*****************************************************************************
// Function:   ValueToString
// Purpose:    Takes a variant, returns a string representation of that variant
//*****************************************************************************
WSERVER_API WCHAR * valueToString(CIMTYPE dwType, VARIANT *pValue, WCHAR *fnHandler(VARIANT *pv))
{
	DWORD iTotBufSize;

	WCHAR *vbuf = NULL;
	WCHAR *buf = NULL;

	WCHAR lbuf[BLOCKSIZE];

	switch (pValue->vt)
	{
		case VT_EMPTY:
		{
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				wcscpy(buf, L"<empty>");
			}
			break;
		}

		case VT_NULL:
		{
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				wcscpy(buf, L"<null>");
			}
			break;
		}

		case VT_BOOL:
		{
			VARIANT_BOOL b = pValue->boolVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				if (!b)
				{
					wcscpy(buf, L"FALSE");
				}
				else
				{
					wcscpy(buf, L"TRUE");
				}
			}
			break;
		}

		case VT_I1:
		{
			char b = pValue->bVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				if (b >= 32)
				{
					swprintf(buf, L"'%c' (%hd, 0x%hX)", b, (signed char)b, b);
				}
				else
				{
					swprintf(buf, L"%hd (0x%hX)", (signed char)b, b);
				}
			}
			break;
		}

		case VT_UI1:
		{
			unsigned char b = pValue->bVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				if (b >= 32)
				{
					swprintf(buf, L"'%c' (%hu, 0x%hX)", b, (unsigned char)b, b);
				}
				else
				{
					swprintf(buf, L"%hu (0x%hX)", (unsigned char)b, b);
				}
			}
			break;
		}

		case VT_I2:
		{
			SHORT i = pValue->iVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				swprintf(buf, L"%hd (0x%hX)", i, i);
			}
			break;
		}

		case VT_UI2:
		{
			USHORT i = pValue->uiVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				swprintf(buf, L"%hu (0x%hX)", i, i);
			}
			break;
		}

		case VT_I4:
		{
			LONG l = pValue->lVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				swprintf(buf, L"%d (0x%X)", l, l);
			}
			break;
		}

		case VT_UI4:
		{
			ULONG l = pValue->ulVal;
			//buf = (WCHAR *)malloc(BLOCKSIZE);
			buf = new WCHAR[BLOCKSIZE];
			if (buf)
			{
				swprintf(buf, L"%u (0x%X)", l, l);
			}
			break;
		}

		case VT_R4:
		{
			float f = pValue->fltVal;
			//buf = (WCHAR *)malloc(CVTBUFSIZE * sizeof(WCHAR));
			buf = new WCHAR[CVTBUFSIZE * sizeof(WCHAR)];

			if (buf)
			{
				swprintf(buf, L"%10.4f", f);
			}
			break;
		}

		case VT_R8:
		{
			double d = pValue->dblVal;
			//buf = (WCHAR *)malloc(CVTBUFSIZE * sizeof(WCHAR));
			buf = new WCHAR[CVTBUFSIZE * sizeof(WCHAR)];
			if (buf)
			{
				swprintf(buf, L"%10.4f", d);
			}
			break;
		}

		case VT_BSTR:
		{
			if (dwType == CIM_SINT64)
			{
				// a little redundant, but it makes me feel better
				LPWSTR pWStr = pValue->bstrVal;
				__int64 l = _wtoi64(pWStr);

				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					swprintf(buf, L"%I64d", l, l);
				}
			}
			else if (dwType == CIM_UINT64)
			{
				// a little redundant, but it makes me feel better
				LPWSTR pWStr = pValue->bstrVal;
				__int64 l = _wtoi64(pWStr);

				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					swprintf(buf, L"%I64u", l, l);
				}
			}
			else // string, datetime, reference
			{
				LPWSTR pWStr = pValue->bstrVal;

				//buf = (WCHAR *)malloc((wcslen(pWStr) * sizeof(WCHAR)) + sizeof(WCHAR) + (2 * sizeof(WCHAR)));
				buf = new WCHAR[(wcslen(pWStr) * sizeof(WCHAR)) + sizeof(WCHAR) + (2 * sizeof(WCHAR))];
				
				if (buf)
				{
					//swprintf(buf, L"\"%wS\"", pWStr);
					swprintf(buf, L"%wS", pWStr);
				}
			}
			break;
		}

		case VT_BOOL|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];
			
			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					VARIANT_BOOL v;
					SafeArrayGetElement(pVec, &i, &v);
					if (v)
					{
						wcscat(buf, L"TRUE");
					}
					else
					{
						wcscat(buf, L"FALSE");
					}
				}
			}

			break;
		}

		case VT_I1|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];
				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];
			if (buf)
			{
				wcscpy(buf, L"");
				WCHAR *pos = buf;
				DWORD len;

				BYTE *pbstr;
				SafeArrayAccessData(pVec, (void HUGEP* FAR*)&pbstr);

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscpy(pos, L",");
						pos += 1;
					}
					else
					{
						bFirst = FALSE;
					}

					char v;
					//            SafeArrayGetElement(pVec, &i, &v);
					v = pbstr[i];

					if (v < 32)
					{
						len = swprintf(lbuf, L"%hd (0x%X)", v, v);
					}
					else
					{
						len = swprintf(lbuf, L"'%c' %hd (0x%X)", v, v, v);
					}

					wcscpy(pos, lbuf);
					pos += len;
				}
			}

			SafeArrayUnaccessData(pVec);

			break;
		}

		case VT_UI1|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];
			if (buf)
			{
				wcscpy(buf, L"");
				WCHAR *pos = buf;
				DWORD len;

				BYTE *pbstr;
				SafeArrayAccessData(pVec, (void HUGEP* FAR*)&pbstr);

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscpy(pos, L",");
						pos += 1;
					}
					else
					{
						bFirst = FALSE;
					}

					unsigned char v;
					//            SafeArrayGetElement(pVec, &i, &v);
					v = pbstr[i];

					if (v < 32)
					{
						len = swprintf(lbuf, L"%hu (0x%X)", v, v);
					}
					else
					{
						len = swprintf(lbuf, L"'%c' %hu (0x%X)", v, v, v);
					}

					wcscpy(pos, lbuf);
					pos += len;
				}
			}

			SafeArrayUnaccessData(pVec);

			break;
		}

		case VT_I2|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];
				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];
			
			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					SHORT v;
					SafeArrayGetElement(pVec, &i, &v);
					swprintf(lbuf, L"%hd", v);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case VT_UI2|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];
				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];

			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					USHORT v;
					SafeArrayGetElement(pVec, &i, &v);
					swprintf(lbuf, L"%hu", v);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case VT_I4|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];

			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					LONG v;
					SafeArrayGetElement(pVec, &i, &v);
					_ltow(v, lbuf, 10);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case VT_UI4|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
			buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];

			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					ULONG v;
					SafeArrayGetElement(pVec, &i, &v);
					_ultow(v, lbuf, 10);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case CIM_REAL32|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * (CVTBUFSIZE * sizeof(WCHAR)));
			buf = new WCHAR[(iUBound - iLBound + 1) * (CVTBUFSIZE * sizeof(WCHAR))];

			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					FLOAT v;
					SafeArrayGetElement(pVec, &i, &v);
					swprintf(lbuf, L"%10.4f", v);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case CIM_REAL64|VT_ARRAY:
		{
			SAFEARRAY *pVec = pValue->parray;
			long iLBound, iUBound;
			BOOL bFirst = TRUE;

			SafeArrayGetLBound(pVec, 1, &iLBound);
			SafeArrayGetUBound(pVec, 1, &iUBound);
			if ((iUBound - iLBound + 1) == 0)
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"<empty array>");
				}
				break;
			}

			//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * (CVTBUFSIZE * sizeof(WCHAR)));
			buf = new WCHAR[(iUBound - iLBound + 1) * (CVTBUFSIZE * sizeof(WCHAR))];

			if (buf)
			{
				wcscpy(buf, L"");

				for (long i = iLBound; i <= iUBound; i++)
				{
					if (!bFirst)
					{
						wcscat(buf, L",");
					}
					else
					{
						bFirst = FALSE;
					}

					double v;
					SafeArrayGetElement(pVec, &i, &v);
					swprintf(lbuf, L"%10.4f", v);
					wcscat(buf, lbuf);
				}
			}

			break;
		}

		case VT_BSTR|VT_ARRAY:
		{
			if (dwType == (CIM_UINT64|VT_ARRAY))
			{
				SAFEARRAY *pVec = pValue->parray;
				long iLBound, iUBound;
				BOOL bFirst = TRUE;

				SafeArrayGetLBound(pVec, 1, &iLBound);
				SafeArrayGetUBound(pVec, 1, &iUBound);
				if ((iUBound - iLBound + 1) == 0)
				{
					//buf = (WCHAR *)malloc(BLOCKSIZE);
					buf = new WCHAR[BLOCKSIZE];

					if (buf)
					{
						wcscpy(buf, L"<empty array>");
					}
					break;
				}

				//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
				buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"");

					for (long i = iLBound; i <= iUBound; i++)
					{
						if (!bFirst)
						{
							wcscat(buf, L",");
						}
						else
						{
							bFirst = FALSE;
						}

						BSTR v = NULL;

						SafeArrayGetElement(pVec, &i, &v);

						swprintf(lbuf, L"%I64u", _wtoi64(v));
						wcscat(buf, lbuf);
					}
				}
			}
			else if (dwType == (CIM_SINT64|VT_ARRAY))
			{
				SAFEARRAY *pVec = pValue->parray;
				long iLBound, iUBound;
				BOOL bFirst = TRUE;

				SafeArrayGetLBound(pVec, 1, &iLBound);
				SafeArrayGetUBound(pVec, 1, &iUBound);
				if ((iUBound - iLBound + 1) == 0)
				{
					//buf = (WCHAR *)malloc(BLOCKSIZE);
					buf = new WCHAR[BLOCKSIZE];

					if (buf)
					{
						wcscpy(buf, L"<empty array>");
					}
					break;
				}

				//buf = (WCHAR *)malloc((iUBound - iLBound + 1) * BLOCKSIZE);
				buf = new WCHAR[(iUBound - iLBound + 1) * BLOCKSIZE];

				if (buf)
				{
					wcscpy(buf, L"");

					for (long i = iLBound; i <= iUBound; i++)
					{
						if (!bFirst)
						{
							wcscat(buf, L",");
						}
						else
						{
							bFirst = FALSE;
						}

						BSTR v = NULL;

						SafeArrayGetElement(pVec, &i, &v);

						swprintf(lbuf, L"%I64d", _wtoi64(v));
						wcscat(buf, lbuf);
					}
				}
			}
			else // string, datetime, reference
			{
				SAFEARRAY *pVec = pValue->parray;
				long iLBound, iUBound;
				DWORD iNeed;
				size_t iVSize;
				DWORD iCurBufSize;

				SafeArrayGetLBound(pVec, 1, &iLBound);
				SafeArrayGetUBound(pVec, 1, &iUBound);
				if ((iUBound - iLBound + 1) == 0)
				{
					//buf = (WCHAR *)malloc(BLOCKSIZE);
					buf = new WCHAR[BLOCKSIZE];

					if (buf)
					{
						wcscpy(buf, L"<empty array>");
					}
					break;
				}

				iTotBufSize = (iUBound - iLBound + 1) * BLOCKSIZE;
				//buf = (WCHAR *)malloc(iTotBufSize);
				buf = new WCHAR[iTotBufSize];

				if (buf)
				{
					buf[0] = L'\0';
					iCurBufSize = 0;
					iVSize = BLOCKSIZE;
					//vbuf = (WCHAR *)malloc(BLOCKSIZE);
					vbuf = new WCHAR[BLOCKSIZE];
					if (vbuf)
					{
						size_t iLen;

						for (long i = iLBound; i <= iUBound; i++)
						{
							BSTR v = NULL;
							SafeArrayGetElement(pVec, &i, &v);
							iLen = (wcslen(v) + 1) * sizeof(WCHAR);
							if (iLen > iVSize)
							{
								vbuf = (WCHAR *)realloc(vbuf, iLen + sizeof(WCHAR));
								iVSize = iLen;
							}

							// String size + (quotes + comma + null)
							iNeed = (swprintf(vbuf, L"%wS", v) + 4) * sizeof(WCHAR);
							if (iNeed + iCurBufSize > iTotBufSize)
							{
								iTotBufSize += (iNeed * 2);  // Room enough for 2 more entries
								buf = (WCHAR *)realloc(buf, iTotBufSize);
							}
							wcscat(buf, L"\"");
							wcscat(buf, vbuf);
							if (i + 1 <= iUBound)
							{
								wcscat(buf, L"\",");
							}
							else
							{
								wcscat(buf, L"\"");
							}
							iCurBufSize += iNeed;
							SysFreeString(v);
						}
						//free(vbuf);
						delete[] vbuf;
					}
				}
			}

			break;
		}

		default:
		{
			if (fnHandler != NULL)
			{
				buf = fnHandler(pValue);
			}
			else
			{
				//buf = (WCHAR *)malloc(BLOCKSIZE);
				buf = new WCHAR[BLOCKSIZE];
				if (buf)
				{
					wcscpy(buf, L"<conversion error>");
				}
			}
			break;
		}
	}
/*
	if (!buf)
	{
	   PrintErrorAndExit("ValueToString() out of memory", S_OK, 0);
	}
*/
	return buf;
}
#endif				// ]

WSERVER_API BOOL shouldEntityEncode(char x)
{
	if (x < 32) return TRUE;
	if (x > 'z') return TRUE;
	if (x >= 'A' && x <= 'Z') return FALSE;
	if (x >= 'a' && x <= 'z') return FALSE;
	if (x >= '0' && x <= '9') return FALSE;
	if (x == 32) return FALSE;

	if (x == ',') return FALSE;
	if (x == '.') return FALSE;
	if (x == '@') return FALSE;
	if (x == ':') return FALSE;
	if (x == '(') return FALSE;
	if (x == ')') return FALSE;
	if (x == '-') return FALSE;
	if (x == '+') return FALSE;
	if (x == '*') return FALSE;
	if (x == '!') return FALSE;
	if (x == '$') return FALSE;
	if (x == '^') return FALSE;
	if (x == '_') return FALSE;
	if (x == ';') return FALSE;
	
	return TRUE;
}

WSERVER_API char * entityEncode(char * instr)
{
	char * encoded = instr;

	if ( instr )
	{
		char	* str = instr;
		int		  len = strlen(str);
		int		  dstLen = len;

		// First count to see how big to allocate the encoded return string.
		while ( *str )
		{
			if ( shouldEntityEncode(*str++) )
				dstLen += 8;
		}

		encoded = new char[dstLen+1];
		char * ptr = encoded;

		str = instr;
		while ( *str )
		{
			char c = *str++;
			if (shouldEntityEncode(c))
			{
				// The _itoa function converts the digits of the given value argument
				// to a null-terminated character string and stores the result
				// (up to 33 bytes) in string. 
				char buf[34] = {0};

				_itoa(c,buf,10);

				*ptr++ = '&';
				*ptr++ = '#';
				*ptr++ = buf[0];
				if ( buf[1] )
					*ptr++ = buf[1];
				*ptr++ = ';';
			}
			else
				*ptr++ = c;
		}
		*ptr = 0;
	}

	return encoded;
}
