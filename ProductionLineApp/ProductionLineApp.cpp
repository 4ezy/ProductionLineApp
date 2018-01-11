// ProductionLineApp.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "ProductionLineApp.h"
#include "ProductionLine.h"
#include <string.h>
#include <string>

#define MAX_LOADSTRING 100

struct WorkplaceLineParameters 
{
	unsigned int workplaceLineCapacity;
	unsigned long workplaceProcTime;
};

struct ProductionLineParameters
{
	unsigned int workplaceLineCount;
	unsigned int productionLineCapacity;
	unsigned long putProductTimeInterval;
	unsigned long simulationTime;
	std::vector<WorkplaceLineParameters> workplaceLines;
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
ProductionLine productionLine;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Parameters(HWND, UINT, WPARAM, LPARAM);
VOID GeneralParametersToStruct(HWND hDlg, ProductionLineParameters *params);
BOOL GeneralParametersValueCheck(HWND hDlg);
BOOL ParametersValueCheck(HWND hDlg);
BOOL SelectComboBoxItem(HWND hCombo, unsigned int itemIndex);
HWND CreateToolbar(HWND hWndParent);

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
    switch (message)
    {
	case WM_CREATE:
	{

	}
		break;
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
				break;
			case IDM_STEP:
				break;
			case IDM_STOP:
				break;
			case IDM_CHANGE_PARAMETERS:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_PARAMETERS), hWnd, Parameters);
				break;
			case IDM_SHOW_STAT:
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
		// TODO: Добавлять в поля информацию если она уже есть
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
			if (ParametersValueCheck(hDlg) == TRUE)
			{
				memset(&productionLine, 0, sizeof(productionLine));
				productionLine = ProductionLine(params.workplaceLineCount, params.productionLineCapacity,
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
					unsigned int x = i + 1;
					std::wstring str = std::to_wstring(x);
					SendMessage(hCombo, CB_ADDSTRING, NULL, (WPARAM)str.c_str());
				}

				workplaceParams = params.workplaceLines;

				SelectComboBoxItem(hCombo, 0);
				lastSelected = 0;
				std::wstring str = std::to_wstring(
					params.workplaceLines[0].workplaceLineCapacity);
				SetDlgItemText(hDlg, IDC_EDIT5, str.c_str());
				str.clear();
				str = std::to_wstring(params.workplaceLines[0].workplaceProcTime);
				SetDlgItemText(hDlg, IDC_EDIT6, str.c_str());
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
					if (workplaceParams[i].workplaceLineCapacity == 0)
					{
						MessageBox(hDlg, _T("Вместимость рабочих мест должна быть больше 0!"),
							_T("Ошибка"), NULL);
						return (INT_PTR)TRUE;
					}
					totalCapacity += workplaceParams[i].workplaceLineCapacity;
				}

				if (totalCapacity != params.productionLineCapacity)
				{
					MessageBox(hDlg, _T("Сумма вместимости рабочих мест должна равняться вместимость рабочей линии!"),
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
	int comboCount = SendMessage(hCombo, CB_GETCOUNT, NULL, NULL);

	if (comboCount > itemIndex - 1)
		return FALSE;

	int selIdex = SendMessage(hCombo, CB_SETCURSEL, itemIndex, NULL);

	if (selIdex != -1)
		return TRUE;
}

VOID GeneralParametersToStruct(HWND hDlg, ProductionLineParameters *params)
{
	params->workplaceLineCount = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
	params->productionLineCapacity = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
	params->putProductTimeInterval = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
	params->simulationTime = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);

	for (size_t i = 0; i < params->workplaceLineCount; i++)
	{
		WorkplaceLineParameters workplaceLineParameters;

		workplaceLineParameters.workplaceLineCapacity =
			params->productionLineCapacity / params->workplaceLineCount;

		if (params->workplaceLineCount == 1)
		{
			workplaceLineParameters.workplaceLineCapacity =
				params->productionLineCapacity;
		}
		else if (i == params->workplaceLineCount - 1)
		{
			workplaceLineParameters.workplaceLineCapacity +=
				params->productionLineCapacity % params->workplaceLineCount;
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

	int value = 0;
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
		MessageBox(hDlg, _T("длительность симуляции не может равняться 0!"),
			_T("Ошибка"), NULL);
		return isOk = FALSE;
	}

	return isOk;
}

//BOOL WorkplaceParametersCheck(HWND hDlg) 
//{
//	HWND hCombo = GetDlgItem(hDlg, IDC_COMBO1);
//	int comboCount = SendMessage(hCombo, CB_GETCOUNT, NULL, NULL);
//
//	for (int i = 0; i < comboCount; i++)
//	{
//
//	}
//}

BOOL ParametersValueCheck(HWND hDlg)
{
	BOOL isOk;

	isOk = GeneralParametersValueCheck(hDlg);

	return isOk;
}