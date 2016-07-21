#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "x64\\Debug\\xmlrpc.lib")
#else
#pragma comment(lib, "x64\\Release\\xmlrpc.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "Debug\\xmlrpc.lib")
#else
#pragma comment(lib, "Release\\xmlrpc.lib")
#endif
#endif

#pragma comment(lib, "wininet")
#pragma comment(lib, "wsock32")

#include <windows.h>
#include <wininet.h> 
#include "resource.h"
#include "XmlRpc.h"

using namespace XmlRpc;

TCHAR szClassName[] = TEXT("Window");

LPSTR GetMultiByteString(UINT uCodePage, LPCWSTR lpszString)
{
	DWORD dwSize = WideCharToMultiByte(uCodePage, 0, lpszString, -1, 0, 0, 0, 0);
	LPSTR lpszReturnString = (LPSTR)GlobalAlloc(GPTR, dwSize * sizeof(CHAR));
	WideCharToMultiByte(uCodePage, 0, lpszString, -1, lpszReturnString, dwSize, 0, 0);
	return lpszReturnString;
}

LPWSTR GetTextFromEdit(HWND hWnd)
{
	LPWSTR lpszReturnString = 0;
	DWORD dwSize = GetWindowTextLengthW(hWnd);
	if (dwSize)
	{
		lpszReturnString = (LPWSTR)GlobalAlloc(0, (dwSize + 1) * sizeof(WCHAR));
		GetWindowText(hWnd, lpszReturnString, dwSize + 1);
	}
	return lpszReturnString;
}

BOOL PostWordPress(LPCWSTR lpszURL, LPCWSTR lpszUserName, LPCWSTR lpszPassWord, LPCWSTR lpszTitle, LPCWSTR lpszContent)
{
	URL_COMPONENTS uc = { 0 };
	WCHAR szHostName[1000];
	WCHAR szUrlPath[1000];
	uc.dwStructSize = sizeof(uc);
	uc.lpszHostName = szHostName;
	uc.lpszUrlPath = szUrlPath;
	uc.dwHostNameLength = 1000;
	uc.dwUrlPathLength = 1000;
	if (!InternetCrackUrlW(lpszURL, 0, 0, &uc)) return;
	LPSTR lpszHostName = GetMultiByteString(CP_ACP, szHostName);
	LPSTR lpszUrlPath = GetMultiByteString(CP_ACP, szUrlPath);
	XmlRpcClient c(lpszHostName, uc.nPort, lpszUrlPath);
	GlobalFree(lpszHostName);
	GlobalFree(lpszUrlPath);
	LPSTR lpszUserNameA = GetMultiByteString(CP_UTF8, lpszUserName);
	LPSTR lpszPassWordA = GetMultiByteString(CP_UTF8, lpszPassWord);
	LPSTR lpszTitleA = GetMultiByteString(CP_UTF8, lpszTitle);
	LPSTR lpszContentA = GetMultiByteString(CP_UTF8, lpszContent);
	XmlRpcValue multicall;
	multicall[0] = 1; // blog id
	multicall[1] = lpszUserNameA; // user name
	multicall[2] = lpszPassWordA; // password
	multicall[3]["post_title"] = lpszTitleA; // title
	multicall[3]["post_content"] = lpszContentA; // body
	multicall[3]["post_status"] = "publish";
	GlobalFree(lpszUserNameA);
	GlobalFree(lpszPassWordA);
	GlobalFree(lpszTitleA);
	GlobalFree(lpszContentA);
	XmlRpcValue result;
	const BOOL bReturnValue = c.execute("wp.newPost", multicall, result);
	c.close();
	return bReturnValue;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	static HWND hStaticURL;
	static HWND hStaticUserName;
	static HWND hStaticPassWord;
	static HWND hStaticTitle;
	static HWND hStaticContent;
	static HWND hEditURL;
	static HWND hEditUserName;
	static HWND hEditPassWord;
	static HWND hEditTitle;
	static HWND hEditContent;
	static HWND hButtonPost;
	static HWND hButtonCancel;
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("MS UI Gochic"));
		hStaticURL = CreateWindow(TEXT("STATIC"), TEXT("URL(&A)："), WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEditURL = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("http://example/wp/xmlrpc.php"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1024, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStaticUserName = CreateWindow(TEXT("STATIC"), TEXT("ユーザー名(&U)："), WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEditUserName = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("wpmaster"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1024, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStaticPassWord = CreateWindow(TEXT("STATIC"), TEXT("パスワード(&P)："), WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEditPassWord = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_PASSWORD, 0, 0, 1024, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStaticTitle = CreateWindow(TEXT("STATIC"), TEXT("タイトル(&T)："), WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEditTitle = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("タイトル"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 1024, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStaticContent = CreateWindow(TEXT("STATIC"), TEXT("本文(&B)："), WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_CENTERIMAGE, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEditContent = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("本文"), WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 0, 1024, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButtonPost = CreateWindow(TEXT("BUTTON"), TEXT("投稿(&S)"), WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButtonCancel = CreateWindow(TEXT("BUTTON"), TEXT("キャンセル"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hStaticURL, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hStaticUserName, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hStaticPassWord, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hStaticTitle, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hStaticContent, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hEditURL, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hEditUserName, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hEditPassWord, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hEditTitle, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hEditContent, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hButtonPost, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hButtonCancel, WM_SETFONT, (WPARAM)hFont, 0);
		break;
	case WM_SIZE:
		MoveWindow(hStaticURL, 10, 10, 128, 32, TRUE);
		MoveWindow(hEditURL, 138, 10, LOWORD(lParam) - 148, 32, TRUE);
		MoveWindow(hStaticUserName, 10, 50, 128, 32, TRUE);
		MoveWindow(hEditUserName, 138, 50, LOWORD(lParam) - 148, 32, TRUE);
		MoveWindow(hStaticPassWord, 10, 90, 128, 32, TRUE);
		MoveWindow(hEditPassWord, 138, 90, LOWORD(lParam) - 148, 32, TRUE);
		MoveWindow(hStaticTitle, 10, 130, 128, 32, TRUE);
		MoveWindow(hEditTitle, 138, 130, LOWORD(lParam) - 148, 32, TRUE);
		MoveWindow(hStaticContent, 10, 170, 128, 32, TRUE);
		MoveWindow(hEditContent, 138, 170, LOWORD(lParam) - 148, HIWORD(lParam) - 170 - 50, TRUE);
		MoveWindow(hButtonPost, LOWORD(lParam) - 532, HIWORD(lParam) - 42, 256, 32, TRUE);
		MoveWindow(hButtonCancel, LOWORD(lParam) - 266, HIWORD(lParam) - 42, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			LPWSTR lpszURL = GetTextFromEdit(hEditURL);
			LPWSTR lpszUserName = GetTextFromEdit(hEditUserName);
			LPWSTR lpszPassWord = GetTextFromEdit(hEditPassWord);
			LPWSTR lpszTitle = GetTextFromEdit(hEditTitle);
			LPWSTR lpszContent = GetTextFromEdit(hEditContent);
			const BOOL bSuccess = PostWordPress(lpszURL,lpszUserName, lpszPassWord, lpszTitle, lpszContent);
			GlobalFree(lpszURL);
			GlobalFree(lpszUserName);
			GlobalFree(lpszPassWord);
			GlobalFree(lpszTitle);
			GlobalFree(lpszContent);
			MessageBox(hWnd, bSuccess ? TEXT("投稿しました！") : TEXT("何らかのエラーが発生しました。"), TEXT("確認"), 0);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		LoadIcon(hInstance,(LPCTSTR)IDI_ICON1),
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("WordPress に記事を投稿"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		1024,
		768,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
