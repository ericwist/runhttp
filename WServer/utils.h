/*
 * utils.h
 *
 * Various utilities.
 *
 * E.J.W 11/11/23
 */

// ZZZ This file is also in FrameWork.dll. Need to consolidate.

#ifndef _WServer_UtilsH_
#define _WServer_UtilsH_

#include <windows.h>
#include "WServer.h"

// Used in HttpCookie & HttpResponse for Headers.
extern char * daysAbbrev[];
extern char * monthsAbbrev[];

//*****************************************************************************
// Function:   cvt
// Purpose:    Converts unicode to oem for console output
// Note:       y must be freed by caller
//*****************************************************************************
WSERVER_API WCHAR *cvt(const char *x, WCHAR **y);

//*****************************************************************************
// Function:   cvt
// Purpose:    Converts unicode to oem for console output
// Note:       y must be freed by caller
//*****************************************************************************
WSERVER_API char *cvt(const WCHAR *x, char **y);

#ifndef	_WIN32_WCE	// [
//*****************************************************************************
// Function:   ValueToString
// Purpose:    Takes a variant, returns a string representation of that variant
//*****************************************************************************
WSERVER_API WCHAR * valueToString(CIMTYPE dwType, VARIANT *pValue, WCHAR *fnHandler(VARIANT *pv));
#endif				// ]

// String type definitions for getStringType()/stringToVariant()
#define	UndefinedStringType	0
#define	AlphaStringType 1
#define	DigitStringType 2
#define	AlphaNumericStringType 3

WSERVER_API int getStringType(char * str);
WSERVER_API BOOL stringToVariant(char * str, VARIANT& var);

WSERVER_API BOOL shouldEntityEncode(char x);
WSERVER_API char * entityEncode(char * instr);

#endif
