// ProductionLineApp.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <string.h>
#include <string>
#include <ctime>
#include <random>
#include "ProductionLineApp.h"
#include "ProductionLine.h"

#define MAX_LOADSTRING 100
#define CB_UNCHECKED   0
#define CB_CHECKED	   1
#define TOOLBAR_RUN	   432
#define TOOLBAR_STOP   433

struct WorkplaceLineParameters 
{
	unsigned int workplaceLineCapacity;
	unsigned long workplaceProcTime;
};

struct ProductionLineParameters
{
	unsigned int workplaceLinesCount;
	unsigned int productionLineCapacity;
	unsigned long putProductTimeInterval;
	unsigned long simulationTime;
	std::vector<WorkplaceLineParameters> workplaceLines;
};

struct WorkplaceLineThreadParams
{
	int wpId;
	HWND hWnd;
};

struct BoxSize
{
	unsigned int width;
	unsigned int height;
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
ProductionLine productionLine;
ProductionLine defaultProductionLineCopy;
int hScrollMin = 0;
int hScrollMax = 10000;
int vScrollMin = 0;
int vScrollMax = 100;
int procProd = 0;
bool timeEnds;
bool isColorGammaCold = true;
BoxSize sizeOffset { 100, 100 };
HWND hStatusBar;
wchar_t statusBuf[256];
std::default_random_engine generator;
clock_t startTime;
clock_t blockTime;

static COLORREF fillColor = RGB(0, 0, 0);
static COLORREF contourColor = RGB(255, 255, 255);
static COLORREF workplaceFillColor = RGB(77, 51, 109);
static COLORREF productColor = RGB(52, 208, 182);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Parameters(HWND, UINT, WPARAM, LPARAM);
VOID GeneralParametersToStruct(HWND hDlg, ProductionLineParameters *params);
BOOL GeneralParametersValueCheck(HWND hDlg);
BOOL SelectComboBoxItem(HWND hCombo, unsigned int itemIndex);
HWND CreateStatusBar(HWND hwndParent, int idStatus,
	HINSTANCE hinst, int cParts);
HWND CreateToolbar(HWND hWndParent);
VOID DrawContouredRectangle(HDC hdc, RECT r,
	COLORREF rectColor, COLORREF contourColor);
DWORD WINAPI PutNewProductThreadProc(LPVOID lpParameter);
DWORD WINAPI WorkplaceLineThreadProc(LPVOID lpParameter);
VOID APIENTRY HandlePopupMenu(HWND hwnd, POINT pt);
HWND CreateToolTip(HWND hwndTool, std::wstring text);
INT_PTR CALLBACK Stat(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PRODUCTIONLINEAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRODUCTIONLINEAPP));

	memset(&productionLine, 0, sizeof(productionLine));

    MSG msg;

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRODUCTIONLINEAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PRODUCTIONLINEAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, 
	   WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
	   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
	   nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   RECT r;
   GetClientRect(hWnd, &r);
   SetScrollRange(hWnd, SB_HORZ, hScrollMin, hScrollMax, FALSE);
   SetScrollRange(hWnd, SB_VERT, vScrollMin, vScrollMax, FALSE);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hToolBar, hCheckBox;
	HMENU hMenu;
	static int hScrollPos, vScrollPos = hScrollMin;
	static int hScrollOffset, vScrollOffset = hScrollMin;
	static int stbParts[1];
	RECT rc;    // client area             
	POINT pt;   // location of mouse click  

    switch (message)
    {
	case WM_CREATE:
	{
		unsigned long t = (unsigned long)time(NULL);
		generator = std::default_random_engine(t);
		hStatusBar = CreateStatusBar(hWnd, 456, hInst, 1);
		hToolBar = CreateToolbar(hWnd);
		GetClientRect(hWnd, &rc);
		hCheckBox = CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("Спрятать вертикальную полосу прокрутки"),
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP,
			rc.right - 315, 50, 305, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		wsprintf(statusBuf, L"Количество обработанных изделий: %d", procProd);
		SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)statusBuf);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		GetClientRect(hWnd, (LPRECT)&rc);
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		if (PtInRect((LPRECT)&rc, pt))
			HandlePopupMenu(hWnd, pt);
		break;
	}
	case WM_SIZE:
	{
		short cx = LOWORD(lParam);
		stbParts[0] = cx;
		SendMessage(hStatusBar, SB_SETPARTS, 1, (LPARAM)stbParts);
		MoveWindow(hStatusBar, 0, 0, 0, 0, TRUE);
		MoveWindow(hToolBar, 0, 0, 0, 0, TRUE);
		GetClientRect(hWnd, &rc);
		MoveWindow(hCheckBox, rc.right - 315, 50, 305, 20, TRUE);
		break;
	}
    case WM_COMMAND:
	{
		if (HWND(lParam) == hCheckBox)
		{
			UINT chkRes = SendMessage(hCheckBox, BM_GETCHECK, NULL, NULL);

			if (chkRes == CB_UNCHECKED)
			{
				SetScrollRange(hWnd, SB_VERT, vScrollMin, vScrollMax, FALSE);
			}
			else if (chkRes == CB_CHECKED)
			{
				SetScrollRange(hWnd, SB_VERT, vScrollMin, vScrollMin, FALSE);
			}
			break;
		}

		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case TOOLBAR_RUN:
		{
			if (productionLine.GetWorkplaceLinesRef()->size() == 0)
			{
				MessageBox(hWnd, L"Для запуска требуется чтобы количество рабочих линий было больше 0!",
					L"Ошибка", NULL);
				break;
			}

			productionLine = ProductionLine(defaultProductionLineCopy);
			memset(productionLine.GetStatisticsRef(), 0, sizeof(Statistics));
			procProd = 0;
			startTime = clock();
			wsprintf(statusBuf, L"Количество обработанных изделий: %d", procProd);
			SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)statusBuf);
			timeEnds = false;

			CreateThread(NULL, 0, PutNewProductThreadProc,
				hWnd, NULL, NULL);

			for (size_t i = 0; i < productionLine.GetWorkplaceLines().size(); i++)
			{
				WorkplaceLineThreadParams *params = new WorkplaceLineThreadParams;
				params->hWnd = hWnd;
				params->wpId = i;
				CreateThread(NULL, 0, WorkplaceLineThreadProc, params, NULL, NULL);
			}

			SetTimer(hWnd, IDT_TIMER,
				productionLine.GetSimulationTime(), NULL);
			break;
		}
		case TOOLBAR_STOP:
		{
			timeEnds = true;
			KillTimer(hWnd, IDT_TIMER);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_RUN:
		{
			if (productionLine.GetWorkplaceLinesRef()->size() == 0)
			{
				MessageBox(hWnd, L"Для запуска требуется чтобы количество рабочих линий было больше 0!",
					L"Ошибка", NULL);
				break;
			}

			productionLine = ProductionLine(defaultProductionLineCopy);
			memset(productionLine.GetStatisticsRef(), 0, sizeof(Statistics));
			procProd = 0;
			startTime = clock();
			wsprintf(statusBuf, L"Количество обработанных изделий: %d", procProd);
			SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)statusBuf);
			timeEnds = false;

			CreateThread(NULL, 0, PutNewProductThreadProc,
				hWnd, NULL, NULL);

			for (size_t i = 0; i < productionLine.GetWorkplaceLines().size(); i++)
			{
				WorkplaceLineThreadParams *params = new WorkplaceLineThreadParams;
				params->hWnd = hWnd;
				params->wpId = i;
				CreateThread(NULL, 0, WorkplaceLineThreadProc, params, NULL, NULL);
			}

			SetTimer(hWnd, IDT_TIMER,
				productionLine.GetSimulationTime(), NULL);
		}
		break;
		case IDM_STOP:
		{
			timeEnds = true;
			KillTimer(hWnd, IDT_TIMER);
			break;
		}
		case IDM_CHANGE_PARAMETERS:
		{
			productionLine = ProductionLine(defaultProductionLineCopy);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_PARAMETERS), hWnd, Parameters);
			RECT r;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, TRUE);
		}
			break;
		case IDM_SHOW_STAT:
		{
			if (!timeEnds || procProd == 0)
			{
				MessageBox(hWnd, L"Чтобы отобразить статистику требуется запустить дождаться окончания работы линии!", 
					L"Ошибка", NULL);
				break;
			}

			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Stat);
		}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_HSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_LINEUP: hScrollPos = hScrollPos > hScrollMin ? hScrollPos - 1 : hScrollPos; break;
		case SB_LINEDOWN: hScrollPos = hScrollPos < hScrollMax ? hScrollPos + 1 : hScrollPos; break;
		case SB_PAGEUP:  hScrollPos = hScrollPos > hScrollMin + 10 ? hScrollPos - 10 : hScrollPos - hScrollPos; break;
		case SB_PAGEDOWN: hScrollPos = hScrollPos < hScrollMax - 10 ? hScrollPos + 10 : hScrollPos + hScrollPos; break;
		case SB_TOP: hScrollPos = hScrollMax; break;
		case SB_BOTTOM: hScrollPos = hScrollMin; break;
		case SB_THUMBPOSITION: hScrollPos = HIWORD(wParam); break;
		}
		if (hScrollPos != GetScrollPos(hWnd, SB_HORZ))
		{
			SetScrollPos(hWnd, SB_HORZ, hScrollPos, TRUE);
			RECT r;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, TRUE);
		}
		break;
	}
	case WM_VSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_LINEUP: vScrollPos = vScrollPos > vScrollMin ? vScrollPos - 1 : vScrollPos; break;
		case SB_LINEDOWN: vScrollPos = vScrollPos < vScrollMax ? vScrollPos + 1 : vScrollPos; break;
		case SB_PAGEUP:  vScrollPos = vScrollPos > vScrollMin + 10 ? vScrollPos - 10 : vScrollPos - vScrollPos; break;
		case SB_PAGEDOWN: vScrollPos = vScrollPos < vScrollMax - 10 ? vScrollPos + 10 : vScrollPos + vScrollPos; break;
		case SB_TOP: vScrollPos = vScrollMax; break;
		case SB_BOTTOM: vScrollPos = vScrollMin; break;
		case SB_THUMBPOSITION: vScrollPos = HIWORD(wParam); break;
		}
		if (vScrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, vScrollPos, TRUE);
			RECT r;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, TRUE);
		}
		break;
	}
    case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		int hScrollPos = -GetScrollPos(hWnd, SB_HORZ);
		int vScrollPos = -GetScrollPos(hWnd, SB_VERT);

		int hScrollDifference = hScrollPos - hScrollOffset;
		hScrollOffset += hScrollDifference;
		int vScrollDifference = vScrollPos - vScrollOffset;
		vScrollOffset += vScrollDifference;

		RECT cellRect;
		cellRect.left = hScrollOffset + 100;
		cellRect.top = vScrollOffset + 200;
		cellRect.right = hScrollOffset + 100 + sizeOffset.width;
		cellRect.bottom = vScrollOffset + 200 + sizeOffset.height;

		RECT deferredCellRect = cellRect;
		deferredCellRect.top += 150;
		deferredCellRect.bottom += 150;

		for (size_t i = 0; i < productionLine.GetWorkplaceLines().size(); i++)
		{
			for (size_t j = 0; j < productionLine.GetWorkplaceLines()[i].GetWorkplaceLineCapacity(); j++)
			{
				DrawContouredRectangle(hdc, cellRect,
					fillColor, contourColor);

				if (productionLine.GetWorkplaceLines()[i].GetProducts().size() > j)
				{
					RECT productRect = cellRect;
					productRect.left += 25;
					productRect.top += 25;
					productRect.right -= 25;
					productRect.bottom -= 25;

					DrawContouredRectangle(hdc, productRect,
						productColor, productColor);
				}

				cellRect.left += sizeOffset.width;
				cellRect.right += sizeOffset.width;
			}

			DrawContouredRectangle(hdc, cellRect,
				workplaceFillColor, contourColor);

			if (!productionLine.GetWorkplaceLines()[i].GetWorkplace().GetIsEmpty())
			{
				RECT productRect = cellRect;
				productRect.left += 25;
				productRect.top += 25;
				productRect.right -= 25;
				productRect.bottom -= 25;

				DrawContouredRectangle(hdc, productRect,
					productColor, productColor);
			}

			cellRect.left += sizeOffset.width;
			cellRect.right += sizeOffset.width;
		}

		for (int i = 0; i < procProd; i++)
		{
			RECT productRect = cellRect;
			productRect.left += 25;
			productRect.top += 25;
			productRect.right -= 25;
			productRect.bottom -= 25;

			DrawContouredRectangle(hdc, productRect,
				productColor, productColor);

			cellRect.left += sizeOffset.width;
			cellRect.right += sizeOffset.width;
		}

		for (size_t i = 0; i < productionLine.GetDeferredProducts().size(); i++)
		{
			RECT deferredProductRect = deferredCellRect;
			deferredProductRect.left += 25;
			deferredProductRect.top += 25;
			deferredProductRect.right -= 25;
			deferredProductRect.bottom -= 25;

			DrawContouredRectangle(hdc, deferredProductRect,
				productColor, productColor);

			deferredCellRect.left += sizeOffset.width;
			deferredCellRect.right += sizeOffset.width;
		}

		if (cellRect.right > hScrollMax)
			hScrollMax = cellRect.right + 100;
		if (cellRect.bottom > vScrollMax)
			vScrollMax = cellRect.bottom + 100;

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN:
	{
		WORD hScrollNotify = 0xFFFF;
		WORD vScrollNotify = 0xFFFF;

		switch (wParam)
		{
		case VK_LEFT:
			hScrollNotify = SB_LINEUP;
			break;

		case VK_RIGHT:
			hScrollNotify = SB_LINEDOWN;
			break;

		case VK_UP:
			vScrollNotify = SB_LINEUP;
			break;

		case VK_DOWN:
			vScrollNotify = SB_LINEDOWN;
			break;

		case VK_PRIOR:
			hScrollNotify = SB_PAGEUP;
			break;

		case VK_NEXT:
			hScrollNotify = SB_PAGEDOWN;
			break;

		case VK_HOME:
			hScrollNotify = SB_BOTTOM;
			break;

		case VK_END:
			hScrollNotify = SB_TOP;
			break;
		}

		if (hScrollNotify != -1)
			SendMessage(hWnd, WM_HSCROLL, MAKELONG(hScrollNotify, 0), 0L);

		if (hScrollNotify != -1)
			SendMessage(hWnd, WM_VSCROLL, MAKELONG(vScrollNotify, 0), 0L);

		break;
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case IDT_TIMER:
		{
			timeEnds = true;
			KillTimer(hWnd, IDT_TIMER);
			break;
		}
		default:
			break;
		}
		
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "Статистика".
INT_PTR CALLBACK Stat(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		std::wstring str;
		str = std::to_wstring(productionLine.GetStatisticsRef()->fstWorkplaceLoad / productionLine.GetSimulationTime() * 100);
		SetDlgItemText(hDlg, IDC_EDIT_66, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->sndWorkplaceLoad / productionLine.GetSimulationTime() * 100);
		SetDlgItemText(hDlg, IDC_EDIT_67, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->avgProdProcTime);
		SetDlgItemText(hDlg, IDC_EDIT_68, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->deferredProductsCount);
		SetDlgItemText(hDlg, IDC_EDIT_69, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->fstWorkplaceAvgQueue);
		SetDlgItemText(hDlg, IDC_EDIT_70, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->sndWorkplaceAvgQueue);
		SetDlgItemText(hDlg, IDC_EDIT_71, str.c_str());
		str = std::to_wstring(productionLine.GetStatisticsRef()->fstWorkplaceBlockingTime);
		SetDlgItemText(hDlg, IDC_EDIT_72, str.c_str());

		break;
	}
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

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
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

// Обработчик сообщений для окна "Параметры".
INT_PTR CALLBACK Parameters(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static ProductionLineParameters params;
	static std::vector<WorkplaceLineParameters> workplaceParams;
	static int lastSelected = -1;
	static BoxSize boxSizes[] =
	{
		{ 60, 60 },
		{ 80, 80 },
		{ 100, 100 },
		{ 120, 120 },
		{ 140, 140 },
	};

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		if (productionLine.GetWorkplaceLines().size() != 0)
		{
			memset(&params, 0, sizeof(params));
			memset(&workplaceParams, 0, sizeof(workplaceParams));
			lastSelected = -1;
			params.workplaceLinesCount = productionLine.GetWorkplaceLines().size();
			params.putProductTimeInterval = productionLine.GetPutProductTimeInterval();
			params.simulationTime = productionLine.GetSimulationTime();

			for (unsigned int i = 0; i < params.workplaceLinesCount; i++)
			{
				params.productionLineCapacity += productionLine.GetWorkplaceLines()[i].GetWorkplaceLineCapacity() + 1;
				WorkplaceLineParameters wpPar;
				wpPar.workplaceLineCapacity = productionLine.GetWorkplaceLines()[i].GetWorkplaceLineCapacity();
				wpPar.workplaceProcTime = productionLine.GetWorkplaceLines()[i].GetWorkplace().GetProcTime();
				params.workplaceLines.push_back(wpPar);
			}

			workplaceParams = params.workplaceLines;

			SetDlgItemInt(hDlg, IDC_EDIT1, params.workplaceLinesCount, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT2, params.productionLineCapacity, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT3, params.putProductTimeInterval, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, params.simulationTime, FALSE);
			HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);

			for (unsigned int i = 0; i < params.workplaceLinesCount; i++)
			{
				unsigned int x = i + 1;
				std::wstring str = std::to_wstring(x);
				SendMessage(hCombo, CB_ADDSTRING, NULL, (WPARAM)str.c_str());
			}

			SelectComboBoxItem(hCombo, 0);
			lastSelected = 0;
			SetDlgItemInt(hDlg, IDC_EDIT5, params.workplaceLines[0].workplaceLineCapacity, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT6, params.workplaceLines[0].workplaceProcTime, FALSE);
		}

		if (isColorGammaCold)
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		else
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);

		CreateToolTip(GetDlgItem(hDlg, IDCANCEL), L"При нажатии ваши изменения не сохранятся");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT1), L"Количество рабочих мест не может быть больше 10");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT2), L"Общая вместимость не может быть больше 100");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT3), L"Интервал поступления не может равняться 0");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT4), L"Время симуляции не может равняться 0");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT5), L"Вместимость рабочего места не может равняться 0");
		CreateToolTip(GetDlgItem(hDlg, IDC_EDIT6), L"Время обработки изделия не может равняться 0");
		CreateToolTip(GetDlgItem(hDlg, IDC_BUTTON1), L"Нажмите, чтобы сохранить данные в этом блоке");
		CreateToolTip(GetDlgItem(hDlg, IDC_BUTTON2), L"Нажмите, чтобы сохранить данные в этом блоке");
		CreateToolTip(GetDlgItem(hDlg, IDC_BUTTON3), L"Нажмите, чтобы восстановить данные в этом блоке до исходных");
		CreateToolTip(GetDlgItem(hDlg, IDC_COMBO1), L"С помощью этого раскрывающегося списка вы можете выбрать рабочее место");

		HWND hList = GetDlgItem(hDlg, IDC_LIST1);

		for (int i = 0; i < ARRAYSIZE(boxSizes); i++)
		{
			std::wstring x(L"x");
			std::wstring bx = std::to_wstring(boxSizes[i].width)
				+ x + std::to_wstring(boxSizes[i].height);
			int pos = (int)SendMessage(hList, LB_ADDSTRING, 0,
				(LPARAM)bx.c_str());
			SendMessage(hList, LB_SETITEMDATA, pos, (LPARAM)i);

			if (boxSizes[i].width == sizeOffset.width &&
				boxSizes[i].height == sizeOffset.height)
			{
				SendMessage(hList, LB_SETCURSEL, i, NULL);
			}
		}

		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{
			HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
			int curSel = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);
			if (curSel == -1)
			{
				SetDlgItemText(hDlg, IDC_EDIT5, L"");
				SetDlgItemText(hDlg, IDC_EDIT6, L"");
			}
			else
			{
				BOOL isOk = FALSE;
				workplaceParams[lastSelected].workplaceLineCapacity = GetDlgItemInt(hDlg, IDC_EDIT5, &isOk, FALSE);
				workplaceParams[lastSelected].workplaceProcTime = GetDlgItemInt(hDlg, IDC_EDIT6, &isOk, FALSE);

				if (!isOk)
				{
					MessageBox(hDlg, _T("Поля не должны быть пустыми!"),
						_T("Ошибка"), NULL);
					return (INT_PTR)TRUE;
				}

				lastSelected = curSel;
				SetDlgItemInt(hDlg, IDC_EDIT5, workplaceParams[curSel].workplaceLineCapacity, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT6, workplaceParams[curSel].workplaceProcTime, FALSE);
			}
		}
			break;
		default:
			break;
		}

		switch (LOWORD(wParam))
		{
		case IDCANCEL:
		{
			EndDialog(hDlg, LOWORD(wParam));
			memset(&params, 0, sizeof(params));
			memset(&workplaceParams, 0, sizeof(workplaceParams));
			return (INT_PTR)TRUE;
		}
		break;
		case IDOK:
		{
			if (params.workplaceLinesCount != 0)
			{
				memset(&productionLine, 0, sizeof(productionLine));
				productionLine = ProductionLine(params.workplaceLinesCount, params.productionLineCapacity,
					params.putProductTimeInterval, 1000, params.simulationTime);

				for (size_t i = 0; i < productionLine.GetWorkplaceLines().size(); i++)
				{
					productionLine.GetWorkplaceLinesRef()->at(i).SetWorkplaceLineCapacity(
						params.workplaceLines[i].workplaceLineCapacity);
					productionLine.GetWorkplaceLinesRef()->at(i).GetWorkplaceRef()->SetProcTime(
						params.workplaceLines[i].workplaceProcTime);
				}

				defaultProductionLineCopy = ProductionLine(productionLine);

				if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
				{
					workplaceFillColor = RGB(77, 51, 109);
					productColor = RGB(52, 208, 182);
					isColorGammaCold = true;
				}
				else
				{
					workplaceFillColor = RGB(252, 63, 77);
					productColor = RGB(247, 140, 56);
					isColorGammaCold = false;
				}

				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			else
			{
				MessageBox(hDlg, _T("Для подтверждения требует заполнить все поля!"),
					_T("Ошибка"), NULL);
			}
		}
		break;
		case IDC_BUTTON1:
		{
			BOOL isOK = GeneralParametersValueCheck(hDlg);

			if (isOK)
			{
				memset(&params, 0, sizeof(params));
				memset(&workplaceParams, 0, sizeof(workplaceParams));
				memset(&lastSelected, 0, sizeof(int));
				GeneralParametersToStruct(hDlg, &params);
				HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
				SendMessage(hCombo, CB_RESETCONTENT, NULL, NULL);

				for (size_t i = 0; i < params.workplaceLines.size(); i++)
				{
					std::wstring str = std::to_wstring(i + 1);
					SendMessage(hCombo, CB_ADDSTRING, NULL, (WPARAM)str.c_str());
				}

				workplaceParams = params.workplaceLines;

				SelectComboBoxItem(hCombo, 0);
				lastSelected = 0;
				SetDlgItemInt(hDlg, IDC_EDIT5, params.workplaceLines[0].workplaceLineCapacity, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT6, params.workplaceLines[0].workplaceProcTime, FALSE);
			}
		}
		break;
		case IDC_BUTTON2:
		{
			HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
			int comboCount = SendMessage(hCombo, CB_GETCOUNT, NULL, NULL);
			int curSel = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);

			if (comboCount > 0 && curSel >= 0)
			{
				BOOL isOk = FALSE;
				workplaceParams[curSel].workplaceLineCapacity = GetDlgItemInt(hDlg, IDC_EDIT5, &isOk, FALSE);
				workplaceParams[curSel].workplaceProcTime = GetDlgItemInt(hDlg, IDC_EDIT6, &isOk, FALSE);

				if (!isOk)
				{
					MessageBox(hDlg, _T("Поля не должны быть пустыми!"),
						_T("Ошибка"), NULL);
					return (INT_PTR)TRUE;
				}

				int totalCapacity = 0;

				for (int i = 0; i < comboCount; i++)
				{
					if (workplaceParams[i].workplaceLineCapacity == 0)
					{
						MessageBox(hDlg, _T("Вместимость рабочих мест не может равняться нулю!"),
							_T("Ошибка"), NULL);
						return (INT_PTR)FALSE;
					}
					totalCapacity += workplaceParams[i].workplaceLineCapacity;
				}

				if (totalCapacity != params.productionLineCapacity - params.workplaceLinesCount)
				{
					MessageBox(hDlg, _T("Сумма вместимости рабочих мест должна равняться\nобщей вместимости рабочей линии - количество рабочих мест!"),
						_T("Ошибка"), NULL);
					break;
				}

				params.workplaceLines = workplaceParams;
			}
		}
		break;
		case IDC_BUTTON3:
		{
			workplaceParams = params.workplaceLines;
			HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
			int comboCount = SendMessage(hCombo, CB_GETCOUNT, NULL, NULL);
			
			if (comboCount == 0)
			{
				SetDlgItemText(hDlg, IDC_EDIT5, L"");
				SetDlgItemText(hDlg, IDC_EDIT6, L"");
			}
			else 
			{

				int curSel = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);
				std::wstring str = std::to_wstring(
					workplaceParams[curSel].workplaceLineCapacity);
				SetDlgItemText(hDlg, IDC_EDIT5, str.c_str());
				str.clear();
				str = std::to_wstring(workplaceParams[curSel].workplaceProcTime);
				SetDlgItemText(hDlg, IDC_EDIT6, str.c_str());
			}
		}
			break;
		case IDC_BUTTON4:
		{
			HWND hList = GetDlgItem(hDlg, IDC_LIST1);
			int curSel = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

			if (curSel != LB_ERR)
			{
				sizeOffset.width = boxSizes[curSel].width;
				sizeOffset.height = boxSizes[curSel].height;
			}
			else 
			{
				MessageBox(hDlg, TEXT("Для подтверждения требуется выбрать размер!"),
					TEXT("Ошибка"), NULL);
			}
		}
			break;
		default:
			break;
		}
	}
	break;
	}
	return (INT_PTR)FALSE;
}

BOOL SelectComboBoxItem(HWND hCombo, unsigned int itemIndex)
{
	unsigned int comboCount = SendMessage(hCombo, CB_GETCOUNT, NULL, NULL);

	if (comboCount > itemIndex - 1)
		return FALSE;

	int selIdex = SendMessage(hCombo, CB_SETCURSEL, itemIndex, NULL);

	if (selIdex == -1)
		return FALSE;

	return TRUE;
}

VOID GeneralParametersToStruct(HWND hDlg, ProductionLineParameters *params)
{
	params->workplaceLinesCount = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
	params->productionLineCapacity = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
	params->putProductTimeInterval = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
	params->simulationTime = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);

	for (size_t i = 0; i < params->workplaceLinesCount; i++)
	{
		WorkplaceLineParameters workplaceLineParameters;

		int res = params->productionLineCapacity / params->workplaceLinesCount - 1;
		workplaceLineParameters.workplaceLineCapacity = res < 0 ? 0 : res;

		if (params->workplaceLinesCount == 1)
		{
			workplaceLineParameters.workplaceLineCapacity =
				params->productionLineCapacity;
		}
		else if (i == params->workplaceLinesCount - 1)
		{
			res = 0;
			res = params->productionLineCapacity % params->workplaceLinesCount - 1;
			workplaceLineParameters.workplaceLineCapacity += res < 0 ? 0 : res;
		}

		workplaceLineParameters.workplaceProcTime = 1000;

		params->workplaceLines.push_back(workplaceLineParameters);
	}
}

BOOL GeneralParametersValueCheck(HWND hDlg)
{
	BOOL isOk;
	UINT capacity = 0;
	// количество рабочих мест
	capacity = GetDlgItemInt(hDlg, IDC_EDIT1, &isOk, FALSE);

	if (!isOk)
	{
		MessageBox(hDlg, _T("Требуется ввести количество рабочих мест!"),
			_T("Ошибка"), NULL);
		return isOk;
	}
	else if (capacity == 0 || capacity > 10)
	{
		MessageBox(hDlg, _T("Количество рабочих мест не может равняться 0 или быть больше 10!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}

	UINT value = 0;
	// общая вместимость
	value = GetDlgItemInt(hDlg, IDC_EDIT2, &isOk, FALSE);

	if (!isOk)
	{
		MessageBox(hDlg, _T("Требуется ввести общую вместимость!"),
			_T("Ошибка"), NULL);
		return isOk;
	}
	else if (value > 100)
	{
		MessageBox(hDlg, _T("Общая вместимость не может быть больше 100!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}
	else if (value < capacity * 2)
	{
		MessageBox(hDlg, _T("Общая вместимость не может быть меньше\nколичества рабочих мест умноженное на 2!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}

	value = 0;
	// интервал поступления изделий
	value = GetDlgItemInt(hDlg, IDC_EDIT3, &isOk, FALSE);

	if (!isOk)
	{
		MessageBox(hDlg, _T("Требуется ввести интервал поступления изделий!"),
			_T("Ошибка"), NULL);
		return isOk;
	}
	else if (value == 0)
	{
		MessageBox(hDlg, _T("Интервал поступления изделий не может равняться 0!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}

	value = 0;
	// длительность симуляции
	value = GetDlgItemInt(hDlg, IDC_EDIT4, &isOk, FALSE);

	if (!isOk)
	{
		MessageBox(hDlg, _T("Требуется ввести длительность симуляции!"),
			_T("Ошибка"), NULL);
		return isOk;
	}
	else if (value == 0)
	{
		MessageBox(hDlg, _T("Длительность симуляции не может равняться 0!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}

	return isOk;
}

// Description: 
//   Creates a status bar and divides it into the specified number of parts.
// Parameters:
//   hwndParent - parent window for the status bar.
//   idStatus - child window identifier of the status bar.
//   hinst - handle to the application instance.
//   cParts - number of parts into which to divide the status bar.
// Returns:
//   The handle to the status bar.
//
HWND CreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE
	hinst, int cParts)
{
	HWND hwndStatus;
	RECT rcClient;
	HLOCAL hloc;
	PINT paParts;
	int i, nWidth;

	// Create the status bar.
	hwndStatus = CreateWindowEx(
		0,                       // no extended styles
		STATUSCLASSNAME,         // name of status bar class
		(PCTSTR)NULL,           // no text when first created
		WS_CHILD | WS_VISIBLE,   // creates a visible child window
		0, 0, 0, 0,              // ignores size and position
		hwndParent,              // handle to parent window
		(HMENU)idStatus,       // child window identifier
		hinst,                   // handle to application instance
		NULL);                   // no window creation data

								 // Get the coordinates of the parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	// Allocate an array for holding the right edge coordinates.
	hloc = LocalAlloc(LHND, sizeof(int) * cParts);
	paParts = (PINT)LocalLock(hloc);

	// Calculate the right edge coordinate for each part, and
	// copy the coordinates to the array.
	nWidth = rcClient.right / cParts;
	int rightEdge = nWidth;
	for (i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}

	// Tell the status bar to create the window parts.
	SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
		paParts);

	// Free the array, and return.
	LocalUnlock(hloc);
	LocalFree(hloc);
	return hwndStatus;
}

HWND CreateToolbar(HWND hWndParent)
{
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 2;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	//// Create the image list.
	//g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
	//	ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
	//	numButtons, 0);

	//// Set the image list.
	//SendMessage(hWndToolbar, TB_SETIMAGELIST,
	//	(WPARAM)ImageListID,
	//	(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_REDOW,  ImageListID), TOOLBAR_RUN, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"Запуск" },
		{ MAKELONG(STD_DELETE,  ImageListID), TOOLBAR_STOP, TBSTATE_ENABLED, buttonStyles, { 0 }, 0, (INT_PTR)L"Остановка" }
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}

DWORD WINAPI PutNewProductThreadProc(LPVOID lpParameter)
{
	HWND hWnd = (HWND)lpParameter;

	while (!timeEnds)
	{
		Product newProduct;
		newProduct.isProcessed = false;

		if (!productionLine.PutNewProduct(newProduct))
		{
			productionLine.DeferProduct(newProduct);
			productionLine.GetStatisticsRef()->deferredProductsCount =
				productionLine.GetStatistics().deferredProductsCount + 1;
		}
			

		RECT r;
		GetClientRect(hWnd, &r);
		InvalidateRect(hWnd, &r, TRUE);
		long double val = productionLine.GetPutProductTimeInterval();
		val /= 1000;
		long double distr = val;
		std::normal_distribution<double> distribution(distr);
		distr = distribution(generator);

		while (distr <= 0 || distr >(val + 0.5) || distr < (val - 0.5))
		{
			distr = distribution(generator);
		}

		distr *= 1000;
		Sleep(distr);
	}

	return 0;
}

DWORD WINAPI WorkplaceLineThreadProc(LPVOID lpParameter)
{
	WorkplaceLineThreadParams *params = (WorkplaceLineThreadParams*)lpParameter;
	std::vector<WorkplaceLine> *wpLines = productionLine.GetWorkplaceLinesRef();
	Workplace *wp = wpLines->at(params->wpId).GetWorkplaceRef();
	const unsigned long size = productionLine.GetWorkplaceLines()[params->wpId].GetWorkplaceLineCapacity();
	std::vector<unsigned long> queueCount(size + 1);

	unsigned int lastSize = 0;

	while (!timeEnds)
	{	
		unsigned int sz = productionLine.GetWorkplaceLinesRef()->at(params->wpId).GetProductsRef()->size();

		if (size != lastSize)
		{
			queueCount[sz] = sz + 1;
		}

		bool isPutted = wpLines->at(params->wpId).PutProductToWorkplace();

		if (isPutted)
		{
			RECT r;
			GetClientRect(params->hWnd, &r);
			InvalidateRect(params->hWnd, &r, TRUE);
		}

		clock_t startProcTime = clock();
		bool isProcessed = wp->ProcessProduct();
		clock_t endProcTime = clock();

		if (isProcessed)
		{
			if (params->wpId == 0)
			{
				productionLine.GetStatisticsRef()->fstWorkplaceLoad += endProcTime - startProcTime;
			}
			else if (params->wpId == 1)
			{
				productionLine.GetStatisticsRef()->sndWorkplaceLoad += endProcTime - startProcTime;
			}

			if (productionLine.GetWorkplaceLines().size() - 1 != params->wpId)
			{
				wpLines->at(params->wpId).PutProductToWorkplaceLine(&wpLines->at(params->wpId + 1));

				if (wpLines->at(wpLines->size() - 1).GetProducts().size() ==
					wpLines->at(wpLines->size() - 1).GetWorkplaceLineCapacity())
				{
					productionLine.BlockLines();
					blockTime = clock();
				}
			}
			else
			{
				productionLine.ReleaseProcessedProduct();
				productionLine.UnblockLines();
				productionLine.GetStatisticsRef()->fstWorkplaceBlockingTime += blockTime == 0 ? 0 : clock() - blockTime;
				procProd++;
				wsprintf(statusBuf, L"Количество обработанных изделий: %d", procProd);
				SendMessage(hStatusBar, SB_SETTEXT, 0, (LONG)statusBuf);
				clock_t testTime;
				testTime = clock();
				productionLine.GetStatisticsRef()->avgProdProcTime += testTime - startTime;
				startTime = clock();
			}

			RECT r;
			GetClientRect(params->hWnd, &r);
			InvalidateRect(params->hWnd, &r, TRUE);
		}
	}

	unsigned int total = 0;
	for (size_t i = 0; i < queueCount.size(); i++)
	{
		total += queueCount[i];
	}

	if (params->wpId == 0)
	{
		productionLine.GetStatisticsRef()->fstWorkplaceAvgQueue = total / queueCount.size();
	}
	if (params->wpId == 1)
	{
		productionLine.GetStatisticsRef()->sndWorkplaceAvgQueue = total / queueCount.size();
	}

	return 0;
}

VOID DrawContouredRectangle(HDC hdc, RECT r, 
	COLORREF rectColor, COLORREF contourColor)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, contourColor);
	SelectObject(hdc, hPen);
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	HBRUSH hBrush = CreateSolidBrush(rectColor);
	r.left++;
	r.top++;
	r.right--;
	r.bottom--;
	FillRect(hdc, &r, hBrush);
}

VOID APIENTRY HandlePopupMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu;       
	HMENU hmenuTrackPopup; 

	hmenu = LoadMenu(hInst, MAKEINTRESOURCEW(IDC_PRODUCTIONLINEAPP));
	if (hmenu == NULL)
		return;

	hmenuTrackPopup = GetSubMenu(hmenu, 1);

	ClientToScreen(hwnd, (LPPOINT)&pt);

	TrackPopupMenu(hmenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON,
		pt.x, pt.y, 0, hwnd, NULL);

	DestroyMenu(hmenu);
}

HWND CreateToolTip(HWND hwndTool, std::wstring text)
{
	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP ,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		GetParent(hwndTool), NULL,
		hInst, NULL);

	if (!hwndTool || !hwndTip)
	{
		MessageBox(NULL, TEXT("Couldn't create the ToolTip control."), TEXT("Error"), MB_OK);
		return (HWND)NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = TTTOOLINFOA_V1_SIZE;
	toolInfo.hwnd = GetParent(hwndTool);
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
	toolInfo.lpszText = (LPWSTR)text.c_str();

	if (SendMessage(hwndTip, TTM_ACTIVATE, TRUE, 0)) //Will add the Tool Tip on Control
	{
		int err = GetLastError();
		MessageBox(NULL, std::to_wstring(GetLastError()).c_str(), TEXT("Error"), MB_OK);

	}
	if (!SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo)) //Will add the Tool Tip on Control
	{
		int err = GetLastError();
		MessageBox(NULL, std::to_wstring(GetLastError()).c_str(), TEXT("Error"), MB_OK);

	}

	return hwndTip;
}