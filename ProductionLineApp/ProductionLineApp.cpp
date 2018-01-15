// ProductionLineApp.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include <CommCtrl.h>
#include <string.h>
#include <string>
#include <ctime>
#include "ProductionLineApp.h"
#include "ProductionLine.h"

#define MAX_LOADSTRING 100

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

struct ThreadParams
{
	HWND hWnd;
	bool timeEnds;
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
ProductionLine productionLine;
int hScrollMin = 0;
int hScrollMax = 1000;
int vScrollMin = 0;
int vScrollMax = 1000;

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
DWORD WINAPI PutNewProductThreadProc(LPVOID lpParameter);
VOID DrawContouredRectangle(HDC hdc, RECT r,
	COLORREF rectColor, COLORREF contourColor);

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
	static int hScrollPos, vScrollPos = hScrollMin;
	static int hScrollOffset, vScrollOffset = hScrollMin;
	static HANDLE hProductionLineThread;
	static HANDLE hMutex;
	static COLORREF fillColor = RGB(0, 0, 0);
	static COLORREF contourColor = RGB(255, 255, 255);
	static COLORREF workplaceFillColor = RGB(66, 46, 65);
	static COLORREF productColor = RGB(123, 222, 242);
	static ThreadParams *params;

    switch (message)
    {
	case WM_CREATE:
	{
		CreateStatusBar(hWnd, 456, hInst, 1);
		CreateToolbar(hWnd);
		break;
	}
    case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_RUN:
		{
			params = new ThreadParams;
			params->hWnd = hWnd;
			params->timeEnds = false;

			CreateThread(NULL, 0, PutNewProductThreadProc,
				params, NULL, NULL);

			SetTimer(hWnd, IDT_TIMER,
				productionLine.GetSimulationTime(), NULL);
		}
		break;
		case IDM_STEP:
			break;
		case IDM_STOP:
			break;
		case IDM_CHANGE_PARAMETERS:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_PARAMETERS), hWnd, Parameters);
			RECT r;
			GetClientRect(hWnd, &r);
			InvalidateRect(hWnd, &r, TRUE);
		}
			break;
		case IDM_SHOW_STAT:
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
		cellRect.right = hScrollOffset + 200;
		cellRect.bottom = vScrollOffset + 300;

		RECT deferredCellRect = cellRect;
		deferredCellRect.top += 200;
		deferredCellRect.bottom += 200;

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

				cellRect.left += 100;
				cellRect.right += 100;
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

			cellRect.left += 100;
			cellRect.right += 100;
		}

		for (size_t i = 0; i < productionLine.GetDefferedProducts().size(); i++)
		{
			RECT deferredProductRect = deferredCellRect;
			deferredProductRect.left += 25;
			deferredProductRect.top += 25;
			deferredProductRect.right -= 25;
			deferredProductRect.bottom -= 25;

			DrawContouredRectangle(hdc, deferredProductRect,
				productColor, productColor);

			deferredCellRect.left += 100;
			deferredCellRect.right += 100;
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
			if (params != nullptr)
				params->timeEnds = true;
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
	}
		return (INT_PTR)TRUE;

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
					productionLine.GetWorkplaceLines()[i].SetWorkplaceLineCapacity(
						params.workplaceLines[i].workplaceLineCapacity);
					productionLine.GetWorkplaceLines()[i].GetWorkplace().SetProcTime(
						params.workplaceLines[i].workplaceProcTime);
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

			if (comboCount != 0)
			{
				int curSel = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);
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
					totalCapacity += workplaceParams[i].workplaceLineCapacity;
				}

				if (totalCapacity != params.productionLineCapacity - params.workplaceLinesCount)
				{
					MessageBox(hDlg, _T("Сумма вместимости рабочих мест должна равняться\nвместимости рабочей линии - количество рабочих мест!"),
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
	else if (value < capacity)
	{
		MessageBox(hDlg, _T("Общая вместимость не может быть меньше количества рабочих мест!"),
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
	const int numButtons = 3;
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
		{ MAKELONG(STD_FILENEW,  ImageListID), 666, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"New" },
		{ MAKELONG(STD_FILEOPEN, ImageListID), 667, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Open" },
		{ MAKELONG(STD_FILESAVE, ImageListID), 668, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Save" }
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
	ThreadParams* params = (ThreadParams*)lpParameter;

	while (!params->timeEnds)
	{
		Product newProduct;
		newProduct.isProcessed = false;

		if (!productionLine.PutNewProduct(newProduct))
			productionLine.DeferProduct(newProduct);

		RECT r;
		GetClientRect(params->hWnd, &r);
		InvalidateRect(params->hWnd, &r, TRUE);

		Sleep(productionLine.GetPutProductTimeInterval());
	}

	return 0;
}

DWORD WINAPI WorkplaceLineThreadProc(LPVOID lpParameter)
{
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