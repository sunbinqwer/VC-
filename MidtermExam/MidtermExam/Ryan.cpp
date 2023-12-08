#include "yuhanCG.h"  // 사용자 정의 헤더 파일

// 전역 변수들
bool isShapeVisible = false;  // 도형이 보이는지 여부
enum ShapeType { NONE, BOX, CIRCLE, BONOBONO, RYAN };  // 도형 유형 열거형
ShapeType selectShape = NONE;  // 현재 선택된 도형 유형  
POINT startPoint, EndPoint, ptOffset, ptMoveStart;  // 좌표 변수들
HWND hDrawingArea;  // 그림이 그려지는 영역의 핸들
int isMouseRButtonPressed = 0;  // 마우스 오른쪽 버튼이 눌렸는지 여부
HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));  // 붓 색상
HBRUSH clear = CreateSolidBrush(RGB(255, 255, 255));  // 지우개 색상
RECT rect;  // 직사각형을 그릴 때 사용되는 RECT 구조체
bool isEyesClosed = false;  // 눈을 감았는지 여부
//int left, int top, int right, int bottom
// 도형을 그리는 함수
void DrawShape(HWND hWnd, HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));  // 붓 색상
    SelectObject(hdc, hBrush);

    switch (selectShape) {
    case BOX:
        Rectangle(hdc, startPoint.x, startPoint.y, EndPoint.x, EndPoint.y);
        break;
    case CIRCLE:
        Ellipse(hdc, startPoint.x, startPoint.y, EndPoint.x, EndPoint.y);
        break;
    case BONOBONO: 
        // 사용자 정의 함수 DrawBonobono 호출
        DrawBonobono(hWnd, hdc, isEyesClosed);
        break;
    
    case RYAN:
        RECT rect1 = { 0, 0, 800, 480 };
        FillRect(hdc, &rect1, clear);
        // 사용자 정의 함수 DrawRyan 호출
        DrawRyan(hWnd, hdc, startPoint.x, startPoint.y, EndPoint.x, EndPoint.y);
        break;
    }

    DeleteObject(hBrush);  // 붓 해제
}

// 윈도우 프로시저 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1:
            selectShape = BOX;
           
            break;
        case 2:
            selectShape = CIRCLE;
            
            break;
        case 3:
            selectShape = BONOBONO;
            startPoint.x = 400;
            startPoint.y = 240;
            EndPoint.x = 400;
            EndPoint.y = 240;

            isShapeVisible = FALSE;
            InvalidateRect(hDrawingArea, NULL, TRUE);
            SetFocus(hDrawingArea);
            break;
        case 4:
            selectShape = RYAN;
            break;
        case 5:
            
            break;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
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

// 그림이 그려지는 영역의 윈도우 프로시저 함수
LRESULT CALLBACK DrawingProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
    case WM_KEYDOWN:
        if (wParam == VK_SPACE && !isEyesClosed) {
            isEyesClosed = true;

            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_KEYUP:
        if (wParam == VK_SPACE && isEyesClosed) {
            isEyesClosed = false;

            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_LBUTTONDOWN:
        startPoint.x = LOWORD(lParam);
        startPoint.y = HIWORD(lParam);
        isShapeVisible = TRUE;
        break;

    case WM_MOUSEMOVE: {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        if (isMouseRButtonPressed) {
            int offsetX = mouseX - ptMoveStart.x;
            int offsetY = mouseY - ptMoveStart.y;
            static int sumX = 0;
            static int sumY = 0;
            sumX += offsetX;
            sumY += offsetY;

            rect = { startPoint.x, startPoint.y, EndPoint.x, EndPoint.y };

            if (selectShape == CIRCLE) {
                rect.right += offsetX;
                rect.bottom += offsetY;
                startPoint.y += 4;
                EndPoint.y += 4;
                sumX = 0; sumY = 0;
            }
            else {
                OffsetRect(&rect, offsetX, offsetY);
            }
            ptMoveStart.x = LOWORD(lParam);
            ptMoveStart.y = HIWORD(lParam);

            InvalidateRect(hDrawingArea, NULL, TRUE);
        }
        else {
            if (isShapeVisible) {
                EndPoint.x = mouseX;
                EndPoint.y = mouseY;

                InvalidateRect(hDrawingArea, NULL, TRUE);
            }
        }
        break;
    }
    case WM_LBUTTONUP:
        EndPoint.x = LOWORD(lParam);
        EndPoint.y = HIWORD(lParam);

        if (isShapeVisible)
            isShapeVisible = FALSE;
        InvalidateRect(hDrawingArea, NULL, TRUE);
        break;

    case WM_RBUTTONDOWN: {
        int mouseX = LOWORD(lParam);
        int mouseY = HIWORD(lParam);

        if (mouseX >= startPoint.x && mouseX <= EndPoint.x && mouseY >= startPoint.y && mouseY <= EndPoint.y) {
            isMouseRButtonPressed = 1;
            ptMoveStart.x = mouseX;
            ptMoveStart.y = mouseY;
        }
    }
    

    case WM_RBUTTONUP:
        isMouseRButtonPressed = 0;
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (!isMouseRButtonPressed) {
            DrawShape(hWnd, hdc);
        }
        else {
            RECT rect1 = { 0, 0, 800, 480 };
            FillRect(hdc, &rect1, clear);
            if (selectShape == CIRCLE)
                Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
            else {
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            }
            startPoint.x = rect.left;
            startPoint.y = rect.top;
            EndPoint.x = rect.right;
            EndPoint.y = rect.bottom;
        }

        EndPaint(hWnd, &ps);
        break;
    }
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

// WinMain 함수
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    HWND hButtonBox, hButtonCircle, hButtonTriangle, hButtonLine, hButtonStop;
    HBRUSH hBrush[2] = { 0,0 };
    hBrush[0] = CreateSolidBrush(RGB(255, 240, 200));
    hBrush[1] = CreateSolidBrush(RGB(255, 255, 255));

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = hBrush[0];
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ButtonWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        return 1;
    }

    wcex.lpfnWndProc = DrawingProc;
    wcex.hbrBackground = hBrush[1];
    wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcex.lpszClassName = TEXT("DrawingWindow");

    if (!RegisterClassEx(&wcex)) {
        return 1;
    }

    hWnd = CreateWindow(
        L"ButtonWindowClass", L"MidtermExam", WS_OVERLAPPED | WS_SYSMENU,
        CW_USEDEFAULT, 0, 846, 630, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        return FALSE;
    }

    // 각 버튼 및 그림이 그려지는 영역 생성
    hButtonBox = CreateWindow(
        L"BUTTON", L"Box", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        16, 16, 140, 64, hWnd, (HMENU)1, hInstance, NULL);

    hButtonCircle = CreateWindow(
        L"BUTTON", L"Circle", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        180, 16, 140, 64, hWnd, (HMENU)2, hInstance, NULL);

    hButtonTriangle = CreateWindow(
        L"BUTTON", L"Bonobono", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        344, 16, 140, 64, hWnd, (HMENU)3, hInstance, NULL);

    hButtonLine = CreateWindow(
        L"BUTTON", L"Ryan", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        508, 16, 140, 64, hWnd, (HMENU)4, hInstance, NULL);

    hButtonStop = CreateWindow(
        L"BUTTON", L"Cube", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        672, 16, 140, 64, hWnd, (HMENU)5, hInstance, NULL);

    hDrawingArea = CreateWindow(
        L"DrawingWindow", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        16, 96, 800, 480, hWnd, NULL, hInstance, NULL);

    // 윈도우 보이기 및 갱신
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 생성한 붓 해제
    DeleteObject(hBrush[0]);
    DeleteObject(hBrush[1]);

    return (int)msg.wParam;
}
