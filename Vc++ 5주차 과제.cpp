#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HBRUSH pinkBrush; // 핑크색 브러시 핸들



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0,
                        GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"202207047 김선빈", NULL };
    RegisterClassEx(&wcex);

    HWND hwnd = CreateWindow(L"202207047 김선빈", L"202207047김선빈", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    pinkBrush = CreateSolidBrush(RGB(255, 182, 193)); // 핑크색 브러시 생성

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(pinkBrush); // 브러시 객체 삭제

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static POINT startPoint;
    static bool isDrawing = false;
    static RECT drawingRect;


    switch (message) {
    case WM_LBUTTONDOWN:
        isDrawing = true;
        startPoint.x = LOWORD(lParam);
        startPoint.y = HIWORD(lParam);
        drawingRect.left = drawingRect.right = startPoint.x;
        drawingRect.top = drawingRect.bottom = startPoint.y;
        break;

    case WM_MOUSEMOVE:
        if (isDrawing) {
            HDC hdc = GetDC(hwnd);
            SelectObject(hdc, GetStockObject(NULL_PEN)); // 테두리를 그리지 않음
            SelectObject(hdc, pinkBrush); // 핑크색 브러시 선택
            Rectangle(hdc, drawingRect.left, drawingRect.top, drawingRect.right, drawingRect.bottom);
            drawingRect.right = LOWORD(lParam);
            drawingRect.bottom = HIWORD(lParam);
            Rectangle(hdc, drawingRect.left, drawingRect.top, drawingRect.right, drawingRect.bottom);
            ReleaseDC(hwnd, hdc);
        }
        break;

    case WM_LBUTTONUP:
        isDrawing = false;
        break;

    case WM_PAINT:
        // 화면을 다시 그리는 작업 (필요한 경우 구현)
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;

   
}
