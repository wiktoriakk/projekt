#include <windows.h>
#include "Klasa.h"
#include <string>

#define IDC_CITY_EDIT 101
#define IDC_WEATHER_BUTTON 102
#define IDC_WEATHER_LABEL 103

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowW(L"Edit", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 10, 200, 20,
            hwnd, (HMENU)IDC_CITY_EDIT, NULL, NULL);

        CreateWindowW(L"Button", L"Get Weather",
            WS_CHILD | WS_VISIBLE,
            220, 10, 100, 20,
            hwnd, (HMENU)IDC_WEATHER_BUTTON, NULL, NULL);

        CreateWindowW(L"Static", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 40, 310, 20,
            hwnd, (HMENU)IDC_WEATHER_LABEL, NULL, NULL);
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_WEATHER_BUTTON) {
            wchar_t city[256];
            GetWindowTextW(GetDlgItem(hwnd, IDC_CITY_EDIT), city, 256);

            std::wstring wcity(city);
            std::string cityStr(wcity.begin(), wcity.end());
       
            Klasa klasa;
            std::string apiKey = "apiKey"; // Podaj swój klucz API
            std::string weatherInfo = klasa.getWeather(cityStr, apiKey);

            std::wstring wweatherInfo(weatherInfo.begin(), weatherInfo.end());
            LPCWSTR weatherInfoStr = wweatherInfo.c_str();
            
            SetWindowTextW(GetDlgItem(hwnd, IDC_WEATHER_LABEL), weatherInfoStr);
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

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Weather Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
