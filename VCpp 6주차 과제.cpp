#define DEBUG
#ifdef UNICODE
#ifdef DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:WINDOWS")
#endif // DEBUG
#else
#ifdef DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:WINDOWS")
#endif // DEBUG
#endif

#include <windows.h>

POINT startPoint = { 0 };
POINT endPoint = { 0 };
int isKeyPressed = 0;

RECT rect_user = { 50, 50, 100, 100 }; // 파란 상자의 초기 위치와 크기
RECT rect_target = { 250, 250, 350, 350 }; // 핑크 상자의 초기 위치와 크기

bool hasCollided = false; // 두 상자가 충돌했는지 여부

// Function to check collision between two rectangles
bool CheckCollision(const RECT& rect1, const RECT& rect2) {
    return (rect1.left < rect2.right&& rect1.right > rect2.left &&
        rect1.top < rect2.bottom&& rect1.bottom > rect2.top);
}

// 윈도우의 이벤트를 처리하는 콜백(Callback) 함수.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc = GetDC(hwnd);

    HBRUSH hBrush_user = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hBrush_target = CreateSolidBrush(RGB(255, 0, 255));
    HBRUSH hBrush_eraser = CreateSolidBrush(RGB(255, 255, 255));
    const wchar_t* text = L"Crash!!!";

    switch (uMsg)
    {
    case WM_KEYDOWN:
        isKeyPressed = 1;
        if (wParam == VK_RIGHT) {
            rect_user.left += 5;
            rect_user.right += 5;
        }
        else if (wParam == VK_LEFT) {
            rect_user.left -= 5;
            rect_user.right -= 5;
        }
        else if (wParam == VK_DOWN) {
            rect_user.top += 5;
            rect_user.bottom += 5;
        }
        else if (wParam == VK_UP) {
            rect_user.top -= 5;
            rect_user.bottom -= 5;
        }

        // Check for collision when the user moves the blue box
        hasCollided = CheckCollision(rect_user, rect_target);

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_KEYUP:
        isKeyPressed = 0;
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);

        // If there's no collision, draw the blue box first, then the pink box
        if (!hasCollided) {
            FillRect(hdc, &rect_user, hBrush_user);
            FillRect(hdc, &rect_target, hBrush_target);
        }
        // If there's a collision, draw the blue box first, then the pink box
        else {
            FillRect(hdc, &rect_target, hBrush_target);
            FillRect(hdc, &rect_user, hBrush_user);
        }

        // If there's a collision, show the message
        if (hasCollided) {
            TextOut(hdc, 10, 10, text, lstrlen(text));
        }

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_CLOSE:
        // Instead of closing the window, you can just reset the collision status
        hasCollided = false;
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        // PostQuitMessage to exit the message loop, but do not exit the program
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    DeleteObject(hBrush_user);
    DeleteObject(hBrush_target);
    DeleteObject(hBrush_eraser);
    ReleaseDC(hwnd, hdc);

    return S_OK;
}

#ifdef UNICODE
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
#endif
{
    /* 윈도우 클래스 선언.*/
    WNDCLASS wc = { 0 }; // 괄호 추가 및 모든 멤버 초기화

    // 윈도우 클래스 값 설정
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("Computer Software");
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_CROSS);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpfnWndProc = WindowProc;

    // 윈도우 클래스 등록.
    if (RegisterClass(&wc) == 0)
    {
        MessageBox(NULL, L"RegisterClass failed!", L"Error", MB_ICONERROR);
        exit(-1);    //예외
    }

    // Window viewport 영역 조정
    RECT rect = { 150, 100, 800, 600 };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 0);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // 윈도우 생성
    HWND hwnd = CreateWindow(
        wc.lpszClassName,
        TEXT("컴소 Application"),
        WS_OVERLAPPEDWINDOW,
        0, 0,
        width, height,
        NULL, NULL,
        hInstance,
        NULL
    );

    // 오류 검사.
    if (hwnd == NULL)
    {
        MessageBox(NULL, L"CreateWindow failed!", L"Error", MB_ICONERROR);
        exit(-1);
    }

    // 창 보이기.
    ShowWindow(hwnd, SW_SHOW); // 창 띄우고
    UpdateWindow(hwnd); // 업데이트해야 보임. 한 쌍으로 쓴다고 보면 됨.

    // 메시지 처리 루프.
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    // 메시지 처리.
    while (msg.message != WM_QUIT)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            // 메시지 해석해줘.
            TranslateMessage(&msg);
            // 메시지를 처리해야할 곳에 전달해줘.
            DispatchMessage(&msg);
        }
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);

    // 종료 메시지 보내기
    return (int)msg.wParam;
}
