#include <windows.h>
#include "Klasa.h"
#include <string>
#include <fstream>
#include <exception>
#include <sstream>
#include <locale>
#include <codecvt>
#include "resource.h"


#define IDC_CITY_EDIT 101
#define IDC_WEATHER_BUTTON 102
#define IDC_WEATHER_LABEL 103
#define IDI_APP_ICON 104

HBITMAP hBitmap;

std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hBrushBackground = CreateSolidBrush(RGB(173, 216, 230)); // Light blue color

    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowW(L"Edit", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 50, 200, 20,
            hwnd, (HMENU)IDC_CITY_EDIT, NULL, NULL);

        CreateWindowW(L"Button", L"Get Weather",
            WS_CHILD | WS_VISIBLE,
            220, 50, 100, 20,
            hwnd, (HMENU)IDC_WEATHER_BUTTON, NULL, NULL);

        CreateWindowW(L"Static", L"Enter a city:",
            WS_CHILD | WS_VISIBLE,
            10, 20, 200, 20,
            hwnd, NULL, NULL, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            10, 80, 560, 400,
            hwnd, (HMENU)IDC_WEATHER_LABEL, NULL, NULL);

        CreateWindowW(L"Button", L"Daily (Hourly)",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            10, 490, 150, 20,
            hwnd, (HMENU)IDC_DAILY_RADIO, NULL, NULL);

        CreateWindowW(L"Button", L"14 Days",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            170, 490, 150, 20,
            hwnd, (HMENU)IDC_14DAYS_RADIO, NULL, NULL);

        // Ustawiamy domyślnie wybrany pierwszy przycisk radiowy
        CheckRadioButton(hwnd, IDC_DAILY_RADIO, IDC_14DAYS_RADIO, IDC_DAILY_RADIO);

        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        SetBkColor(hdcStatic, RGB(173, 216, 230)); // Light blue color
        return (INT_PTR)hBrushBackground;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_WEATHER_BUTTON) {
            try {
                wchar_t city[256];
                GetWindowTextW(GetDlgItem(hwnd, IDC_CITY_EDIT), city, 256);
                std::wstring wcity(city);
                std::string cityStr(wcity.begin(), wcity.end());

                bool isDaily = IsDlgButtonChecked(hwnd, IDC_DAILY_RADIO) == BST_CHECKED;

                Klasa klasa;
                std::string apiKey = "5f3747f4cc204dfa945172903241605"; // Podaj swój klucz API
                std::string weatherInfo = klasa.getWeather(cityStr, apiKey, isDaily);
                std::wstring wweatherInfo(weatherInfo.begin(), weatherInfo.end());
                LPCWSTR weatherInfoStr = wweatherInfo.c_str();

                SetWindowTextW(GetDlgItem(hwnd, IDC_WEATHER_LABEL), weatherInfoStr);

                std::ofstream outFile(L"weather_results.txt", std::ios::app);
                if (outFile.is_open()) {
                    outFile << "City: " << cityStr << "\n";
                    outFile << "Weather Info: " << weatherInfo << "\n\n";
                    outFile.close();
                }
                else {
                    throw std::ios_base::failure("Failed to open file");
                }

            }
            catch (const std::exception& e) {
                std::wstring errorMsg = stringToWString(e.what());
                MessageBoxW(hwnd, errorMsg.c_str(), L"Exception", MB_OK | MB_ICONERROR);
            }
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"WeatherApp";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Win32 Guided Tour", NULL);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Weather Application",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    HBRUSH hbrBackground = CreateSolidBrush(RGB(0, 0, 128)); // ciemnoniebieski kolor
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrBackground);


    HICON hIcon = (HICON)LoadImageW(NULL, L"C:\\projekt2.0\\projekt2.0\\app_icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    if (hIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
