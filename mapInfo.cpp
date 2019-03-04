// mapInfo.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "mapInfo.h"
#include <vector>

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
struct RealPOINT {
	float x;
	float y;
};

enum TOOL{TOOL_DEFAULT,TOOL_MOVE,TOOL_ADD};
TOOL CurrentTool;
int iSelectedPolygon = -1, iSelectedVertex = -1, iHoverPolygon = -1;
int MouseXwhenSelected = 0, MouseYwhenSelected = 0;
using namespace std;
vector<RealPOINT> PolygonBeingModified;
vector<std::vector<RealPOINT>> Polygons(0);
vector<RealPOINT> PolygonBeingAdded;


// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPINFO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPINFO));

    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPINFO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPINFO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void fillPolygon()
{

}


WCHAR* GetToolString(enum TOOL currentTool)
{
	switch (currentTool)
	{
	case TOOL_ADD:
		return L"Tool_ADD";
		break;
	case TOOL_MOVE:
		return L"Tool_Move";
		break;
	default:
		return L"Tool_Default";
	}
}

void SaveMapInfo()
{

}

void LoadMapInfo()
{
	//CreateFile
	//Just two layered vectors easy to store. one vector by one vector sequentially.  easy to handle. 
}

void DrawInfo(HDC hdc, int x, int y)
{
	WCHAR szBuffer[20];
	wsprintf(szBuffer, L"X:%d Y:%d", x, y);
	TextOut(hdc, 33, 33, szBuffer, lstrlen(szBuffer));
	wsprintf(szBuffer, L"Tool:%s", GetToolString(CurrentTool));
	TextOut(hdc, 33, 63, szBuffer, lstrlen(szBuffer));
}



bool isPointInPolygon(vector<RealPOINT> &polygon, int testx, int testy)
{
	bool c = false;
	int i, j;
	for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
	{
		if (((polygon[i].y > testy) != (polygon[j].y > testy)) &&
			(testx < (polygon[j].x - polygon[i].x) * (testy - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x))
			c = !c;
	}
	return c;
}



void DrawAddedPolygon(HDC hdc)
{
	POINT useless;
	auto &polygon = PolygonBeingAdded;
	if (polygon.size() <= 0)
		return;
	
	for (int i = 0; i < polygon.size(); i++)
	{
		if(i == 0)
			MoveToEx(hdc, polygon[0].x, polygon[0].y, &useless);
		else
			LineTo(hdc, polygon[i].x, polygon[i].y);
		Ellipse(hdc, polygon[i].x - 4, polygon[i].y - 4, polygon[i].x + 4, polygon[i].y + 4);
	}
}



void DrawAllPolygons(HDC hdc)
{
	HPEN hNormalPen = CreatePen(PS_SOLID, 2, 0xFF);
	HPEN hHoverPen = CreatePen(PS_SOLID, 2, 0xFF00);
	HPEN hSelectedPen = CreatePen(PS_SOLID, 2, 0xFF0000);
	int iPolygon = 0;
	for(auto polygon : Polygons)
	{
		if (polygon.size() <= 0)
			continue;

		if (iSelectedPolygon == iPolygon)
			SelectObject(hdc, hSelectedPen);
		else if (iHoverPolygon == iPolygon)
			SelectObject(hdc, hHoverPen);
		else
			SelectObject(hdc, hNormalPen);
	
		POINT Non;
		MoveToEx(hdc, polygon[polygon.size()-1].x, polygon[polygon.size() - 1].y,&Non);
		for (int i = 0 ;i < polygon.size(); i++)
		{
			LineTo(hdc, polygon[i].x, polygon[i].y);
			Ellipse(hdc, polygon[i].x - 4, polygon[i].y - 4, polygon[i].x + 4, polygon[i].y + 4);
		}
		iPolygon++;
	}
	DrawAddedPolygon(hdc);
	DeleteObject(hHoverPen);
	DeleteObject(hNormalPen);
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//

void SwitchTool(enum TOOL targetTool)
{
	CurrentTool = targetTool;
}

vector<RealPOINT>& BuildPolygon(float Vertx[], float Verty[], int nVert)
{
	auto *ThisPolygon = new vector<RealPOINT>;
	for(int i = 0 ;i < nVert ;i++)
		ThisPolygon->push_back({ Vertx[i],Verty[i] });
	
	return *ThisPolygon;
}

void StartAddingNewPolygon()
{
	PolygonBeingAdded.clear();
}

void EndAddingNewPolygon()
{
	if(PolygonBeingAdded.size() >= 3)
		Polygons.push_back(PolygonBeingAdded);
	PolygonBeingAdded.clear();
}

int GetiPolygonUnderMouse(int MouseX,int MouseY)
{
	int iPolygon = 0;
	for (auto polygon : Polygons)
	{
		if (isPointInPolygon(polygon,MouseX, MouseY))
			return iPolygon;	
		iPolygon++;
	}
	return -1;
}

bool ProcessMouseMsgs(UINT message, WPARAM wParam, LPARAM lParam)
{
	bool isRefresh = false;
	int MouseX, MouseY;
	MouseX = LOWORD(lParam);
	MouseY = HIWORD(lParam);
	iHoverPolygon = GetiPolygonUnderMouse(MouseX, MouseY);
	switch (CurrentTool)
	{
	case TOOL_ADD:
		switch (message)
		{
		case WM_LBUTTONDOWN:
			PolygonBeingAdded.push_back({ (float)MouseX, (float)MouseY });
			break;
		case WM_RBUTTONDOWN:
			if (PolygonBeingAdded.size() > 0)
				PolygonBeingAdded.erase(PolygonBeingAdded.end()-1);
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	case TOOL_MOVE:
		switch (message) 
		{
		case WM_LBUTTONDOWN:
			MouseXwhenSelected = MouseX;
			MouseYwhenSelected = MouseY;
			iSelectedPolygon = iHoverPolygon;
			if (iHoverPolygon != -1)
				PolygonBeingModified = Polygons[iHoverPolygon];
			break;
		case WM_MOUSEMOVE:
			if ((wParam & MK_LBUTTON) && iHoverPolygon != -1 && iSelectedPolygon != -1)
			{
				for (int i = 0 ;i < Polygons[iHoverPolygon].size() ;i++)
				{
					Polygons[iHoverPolygon][i].x = PolygonBeingModified[i].x + (MouseX - MouseXwhenSelected);
					Polygons[iHoverPolygon][i].y = PolygonBeingModified[i].y + (MouseY - MouseYwhenSelected);
				}
			}
			break;
		case WM_LBUTTONUP:
			break;
		}
		break;
	case TOOL_DEFAULT:
		break;
	}

	//
	return isRefresh = true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float ArrVertx[] = {30,100,200,150,44};
	float ArrVerty[] = {55,100,44,32,22};
	
	auto f = [](float vertx[], float verty[], int nvert, int biasx, int biasy) {for (int i = 0; i < nvert; i++) { vertx[i] += biasx; verty[i] += biasy; }};

	static int MouseX = 0, MouseY = 0;

	switch (message)
	{
	case WM_CREATE:
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 0, 150);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, -150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, -150, 0);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));
		f(ArrVertx, ArrVerty, 5, 0, 150);
		Polygons.push_back(BuildPolygon(ArrVertx, ArrVerty, 5));

		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��: 
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
	case WM_LBUTTONDBLCLK:
		EndAddingNewPolygon();
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		if (ProcessMouseMsgs(message, wParam, lParam))
			InvalidateRect(hWnd, NULL, true);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'A':
			SwitchTool(TOOL_ADD);
			StartAddingNewPolygon();
			break;
		case 'M':
			SwitchTool(TOOL_MOVE);
			break;
		case VK_DELETE:
			if(iSelectedPolygon != -1)
				Polygons.erase(Polygons.begin() + iSelectedPolygon);
			break;
		case VK_RETURN:
			EndAddingNewPolygon();
			break;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			DrawAllPolygons(hdc);
			DrawInfo(hdc,MouseX,MouseY);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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
