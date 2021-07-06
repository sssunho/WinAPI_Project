// galsPanic.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "galsPanic.h"
#include "interface.h"
#include "controlTime.h"
#include "VECTOR.h"
#include "gameObject.h"
#include <iterator>
#pragma comment(lib, "msimg32.lib")

#define MAX_LOADSTRING 100

// 함수 정의:
void Run(); 
void initBitmaps();
void initInst();

HBITMAP hLandScape;
BITMAP bitLandScape;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;
bool g_bLoop = true;

// 전역 인스턴스 : 
extern Land land;
extern Player actor;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GALSPANIC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GALSPANIC));

    MSG msg;

	initBitmaps();
	initInst();

    // 기본 메시지 루프입니다:
    while (g_bLoop)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Run();
		}
    }

    return (int) msg.wParam;
}

void initBitmaps()
{
	hLandScape = (HBITMAP)LoadImage(NULL, TEXT("images/test.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hLandScape, sizeof(BITMAP), &bitLandScape);
}

void initInst()
{/*
	land.push_back({ 100,100 });
	land.push_back({ 100,250 });
	land.push_back({ 300,250 });
	land.push_back({ 300,200 });
	land.push_back({ 150,200 });
	land.push_back({ 150,150 });
	land.push_back({ 300,150 });
	land.push_back({ 300,100 });*/
/*
	land.push_back({ 100,100 });
	land.push_back({ 100,150 });
	land.push_back({ 250,150 });
	land.push_back({ 250,200 });
	land.push_back({ 100,200 });
	land.push_back({ 100,250 });
	land.push_back({ 300,250 });
	land.push_back({ 300,100 });*/

	//land.push_back({ 100,100 });
	//land.push_back({ 100,300 });
	//land.push_back({ 250,300 });
	//land.push_back({ 250,100 });
	//land.push_back({ 200,100 });
	//land.push_back({ 200,250 });
	//land.push_back({ 150,250 });
	//land.push_back({ 150,100 });

	land.push_back({ 100,100 });
	land.push_back({ 100,300 });
	land.push_back({ 150,300 });
	land.push_back({ 150,150 });
	land.push_back({ 200,150 });
	land.push_back({ 200,300 });
	land.push_back({ 250,300 });
	land.push_back({ 250,100 });
	actor.setLand(&land);
}

void Run()
{
	HDC hdc = GetDC(g_hWnd);
	
	static int cntTime = 0;

	actor.collision(&actor);

	DIRECTION key = getDirectionKeyState();

	playerContoller(key);

	if (cntTime > 20)
		actor.update();

	if (actor.isInvading())
	{
		VECTOR temp = land.collision(&actor);
		if (temp != VECTOR({0, 0}))
		{
			actor.endInvading(temp);
		}
	}

	

	if (cntTime > 20)
	{
		HDC buffer = CreateCompatibleDC(hdc);
		HDC tempHDC = CreateCompatibleDC(hdc);
		HDC mask = CreateCompatibleDC(hdc);
		HBITMAP hOldBitmap;

		HBITMAP tempbit = CreateCompatibleBitmap(hdc, 640, 480);
		HBITMAP bufbit = CreateCompatibleBitmap(hdc, 640, 480);

		int bx = bitLandScape.bmWidth;
		int by = bitLandScape.bmHeight;

		SelectObject(tempHDC, hLandScape);
		SelectObject(buffer, bufbit);
		SelectObject(mask, tempbit);
		
		land.draw(mask);

		BitBlt(buffer, 0, 0, bx, by, tempHDC, 0, 0, SRCCOPY);
		TransparentBlt(buffer, 0, 0, 640, 480, mask, 0, 0, 640, 480, RGB(255, 0, 255));


		actor.draw(buffer);

		BitBlt(hdc, 0, 0, bx, by, buffer, 0, 0, SRCCOPY);

		DeleteObject(tempbit);
		DeleteObject(bufbit);
		DeleteDC(tempHDC);
		DeleteDC(mask);
		DeleteDC(buffer);
		cntTime = 0;

	}
	cntTime++;

	ReleaseDC(g_hWnd, hdc);

}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GALSPANIC));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCEW(IDC_GALSPANIC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT rt = { 0, 0, 640, 480 };
   AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rt.right - rt.left, rt.bottom - rt.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;



   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		g_bLoop = false;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
