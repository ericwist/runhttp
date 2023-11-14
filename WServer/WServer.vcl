<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: WServer - Win32 (WCE ARM) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Wayne\LOCALS~1\Temp\RSP3.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC=310" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "_USRDLL" /D "WSERVER_EXPORTS" /D "_DLL" /Fp"ARMDbg/WServer.pch" /YX /Fo"ARMDbg/" /Fd"ARMDbg/" /MC /c 
"C:\projects\CCode\PPC\WServer\AppSocket.cpp"
"C:\projects\CCode\PPC\WServer\AsciiParameterManager.cpp"
"C:\projects\CCode\PPC\WServer\AttributeManager.cpp"
"C:\projects\CCode\PPC\WServer\Chttp.cpp"
"C:\projects\CCode\PPC\WServer\EchoRequestProcessor.cpp"
"C:\projects\CCode\PPC\WServer\EchoRequestProcessorFactory.cpp"
"C:\projects\CCode\PPC\WServer\HeaderManager.cpp"
"C:\projects\CCode\PPC\WServer\HttpCookie.cpp"
"C:\projects\CCode\PPC\WServer\HttpRequest.cpp"
"C:\projects\CCode\PPC\WServer\HttpRequestProcessor.cpp"
"C:\projects\CCode\PPC\WServer\HttpRequestProcessorFactory.cpp"
"C:\projects\CCode\PPC\WServer\HttpResponse.cpp"
"C:\projects\CCode\PPC\WServer\KeyValue.cpp"
"C:\projects\CCode\PPC\WServer\Platform.cpp"
"C:\projects\CCode\PPC\WServer\RequestListener.cpp"
"C:\projects\CCode\PPC\WServer\RequestProcessor.cpp"
"C:\projects\CCode\PPC\WServer\RequestProcessorFactory.cpp"
"C:\projects\CCode\PPC\WServer\Session.cpp"
"C:\projects\CCode\PPC\WServer\SessionIDGenerator.cpp"
"C:\projects\CCode\PPC\WServer\SessionManager.cpp"
"C:\projects\CCode\PPC\WServer\SocketConnector.cpp"
"C:\projects\CCode\PPC\WServer\SocketListener.cpp"
"C:\projects\CCode\PPC\WServer\SSLAppSocket.cpp"
"C:\projects\CCode\PPC\WServer\SSLRequestListener.cpp"
"C:\projects\CCode\PPC\WServer\StdRedirect.cpp"
"C:\projects\CCode\PPC\WServer\StringTokenizer.cpp"
"C:\projects\CCode\PPC\WServer\URLCoder.cpp"
"C:\projects\CCode\PPC\WServer\utils.cpp"
"C:\projects\CCode\PPC\WServer\ws-util.cpp"
"C:\projects\CCode\PPC\WServer\WServer.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\Wayne\LOCALS~1\Temp\RSP3.tmp" 
Creating temporary file "C:\DOCUME~1\Wayne\LOCALS~1\Temp\RSP4.tmp" with contents
[
commctrl.lib coredll.lib Winsock.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"ARMDbg/WServer.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /nodefaultlib:oldnames.lib" /out:"ARMDbg/WServer.dll" /implib:"ARMDbg/WServer.lib" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
.\ARMDbg\AppSocket.obj
.\ARMDbg\AsciiParameterManager.obj
.\ARMDbg\AttributeManager.obj
.\ARMDbg\Chttp.obj
.\ARMDbg\EchoRequestProcessor.obj
.\ARMDbg\EchoRequestProcessorFactory.obj
.\ARMDbg\HeaderManager.obj
.\ARMDbg\HttpCookie.obj
.\ARMDbg\HttpRequest.obj
.\ARMDbg\HttpRequestProcessor.obj
.\ARMDbg\HttpRequestProcessorFactory.obj
.\ARMDbg\HttpResponse.obj
.\ARMDbg\KeyValue.obj
.\ARMDbg\Platform.obj
.\ARMDbg\RequestListener.obj
.\ARMDbg\RequestProcessor.obj
.\ARMDbg\RequestProcessorFactory.obj
.\ARMDbg\Session.obj
.\ARMDbg\SessionIDGenerator.obj
.\ARMDbg\SessionManager.obj
.\ARMDbg\SocketConnector.obj
.\ARMDbg\SocketListener.obj
.\ARMDbg\SSLAppSocket.obj
.\ARMDbg\SSLRequestListener.obj
.\ARMDbg\StdRedirect.obj
.\ARMDbg\StringTokenizer.obj
.\ARMDbg\URLCoder.obj
.\ARMDbg\utils.obj
".\ARMDbg\ws-util.obj"
.\ARMDbg\WServer.obj
]
Creating command line "link.exe @C:\DOCUME~1\Wayne\LOCALS~1\Temp\RSP4.tmp"
<h3>Output Window</h3>
Compiling...
AppSocket.cpp
AsciiParameterManager.cpp
AttributeManager.cpp
Chttp.cpp
EchoRequestProcessor.cpp
EchoRequestProcessorFactory.cpp
HeaderManager.cpp
HttpCookie.cpp
HttpRequest.cpp
HttpRequestProcessor.cpp
HttpRequestProcessorFactory.cpp
HttpResponse.cpp
KeyValue.cpp
Platform.cpp
RequestListener.cpp
RequestProcessor.cpp
RequestProcessorFactory.cpp
Session.cpp
SessionIDGenerator.cpp
SessionManager.cpp
SocketConnector.cpp
SocketListener.cpp
SSLAppSocket.cpp
SSLRequestListener.cpp
StdRedirect.cpp
StringTokenizer.cpp
URLCoder.cpp
utils.cpp
ws-util.cpp
WServer.cpp
Linking...
   Creating library ARMDbg/WServer.lib and object ARMDbg/WServer.exp



<h3>Results</h3>
WServer.dll - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
