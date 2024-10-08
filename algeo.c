#include <windows.h>
#include <windowsx.h>
#include <stdbool.h>
#include <stdint.h>
#include "vertex_types.h"
#include "vertex_funcs.h"
#include "frame.h"
#include "frame_funcs.h"
#include "draw_util.h"
#include "resource.h"
#include "triangulate.h"

sframe frame = {0};

const char g_szClassName[] = "myWindowClass";

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap;
static HDC frame_device_context = 0;

static HFONT hf = NULL;
char buffer[1000];
bool print = FALSE;

tVertex vertices = NULL;
int mode = 0;

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            {
                HMENU hMenu, hSubMenu;
                HICON hIcon, hIconSm;

                hMenu = CreateMenu();

                hSubMenu = CreatePopupMenu();
                AppendMenu(hSubMenu, MF_STRING, ID_FILE_CLEAR, "&Clear");
                AppendMenu(hSubMenu, MF_STRING, ID_FILE_PRINT, "&Print");
                AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "&Exit");
                AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
                
                hSubMenu = CreatePopupMenu();
                AppendMenu(hSubMenu, MF_STRING, ID_MODE_POINTS, "&Points");
                AppendMenu(hSubMenu, MF_STRING, ID_MODE_CHAIN, "&Chain");
                AppendMenu(hSubMenu, MF_STRING, ID_MODE_POLYGON, "&Polygon");
                AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Mode");

                hSubMenu = CreatePopupMenu();
                AppendMenu(hSubMenu, MF_STRING, ID_TRIANG_EARCLIP, "&Ear Clipping");
                AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Triangulate");

                SetMenu(hwnd, hMenu);

                hIcon = LoadImage(NULL, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
                if (hIcon)
                    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                else
                    MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);

                hIconSm = LoadImage(NULL, "icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
                if (hIconSm)
                    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
                else
                    MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);

                HDC hdc = GetDC(NULL);
                long lfHeight = -MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);
                ReleaseDC(NULL, hdc);
                hf = CreateFont(lfHeight, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, "Times New Roman");
                if(!hf) { MessageBox(hwnd, "Font creation failed!", "Error", MB_OK | MB_ICONEXCLAMATION); }
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;

                case ID_FILE_CLEAR:
                    vertices = clear(hwnd, &frame, vertices);
                    break;

                case ID_FILE_PRINT:
                {                
                    strcpy(buffer, "");
                    print = TRUE;
                    print_vertices(buffer, vertices);
                    update_console(hwnd, &frame);
                    break;
                }

                case ID_MODE_POINTS:
                    mode = POINTS_MODE;
                    clear_screen(&frame);
                    update_frame(hwnd, &frame, vertices, mode);
                    break;

                case ID_MODE_CHAIN:
                    mode = CHAIN_MODE;
                    clear_screen(&frame);
                    update_frame(hwnd, &frame, vertices, mode);
                    break;
                
                case ID_MODE_POLYGON:
                    mode = POLYGON_MODE;
                    clear_screen(&frame);
                    update_frame(hwnd, &frame, vertices, mode);
                    break;

                case ID_TRIANG_EARCLIP:
                {
                    tVertex vertex, vertices_copy = NULL, current = vertices;
                    do {
                        NEW(vertex, tsVertex);
                        vertex->v[X] = current->v[X];
                        vertex->v[Y] = current->v[Y];
                        vertex->vnum = current->vnum;
                        vertex->ear = current->ear;
                        ADD(vertices_copy, vertex);
                        current = current->next;
                    } while (current != vertices);
                    Triangulate(&frame, &vertices_copy);
                    clear_vertices(vertices_copy);
                    update_frame(hwnd, &frame, vertices, POLYGON_MODE);
                    break;
                }
            }
            break;

        case WM_SIZE:
            frame_bitmap_info.bmiHeader.biWidth = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if (frame_bitmap) DeleteObject(frame_bitmap);
            frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
            SelectObject(frame_device_context, frame_bitmap);

            frame.width = LOWORD(lParam);
            frame.height = HIWORD(lParam);

            clear_console(&frame);
            vertices = clear(hwnd, &frame, vertices);
            break;

        case WM_PAINT:
            {
                static PAINTSTRUCT paint;
                static HDC device_context;
                device_context = BeginPaint(hwnd, &paint);

                BitBlt(
                    device_context,
                    paint.rcPaint.left, paint.rcPaint.top,
                    paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                    frame_device_context,
                    paint.rcPaint.left, paint.rcPaint.top,
                    SRCCOPY
                );

                if (print) {
                    HFONT hfOld = (HFONT)SelectObject(device_context, hf);
                    SetBkColor(device_context, WHITE);
                    SetTextColor(device_context, BLACK);
                    SetBkMode(device_context, TRANSPARENT);
                    DrawText(device_context, buffer, -1, &paint.rcPaint, DT_WORDBREAK | DT_EXPANDTABS);
                    SelectObject(device_context, hfOld);
                    print = FALSE;
                }

                EndPaint(hwnd, &paint);
            }
            break;

        case WM_LBUTTONDOWN:
            {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                if (x < FRAME_WIDTH(frame.width)) {
                    tVertex vertex;
                    NEW(vertex, tsVertex);
                    create_vertex(vertices, vertex, x, y);
                    ADD(vertices, vertex);
                    clear_screen(&frame);
                    update_frame(hwnd, &frame, vertices, mode);
                }
            }
            break;

        case WM_RBUTTONDOWN:
            vertices = clear(hwnd, &frame, vertices);
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // Step 1: Registering the Window Class
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.style            = 0;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInstance;
    wc.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = g_szClassName;
    wc.hIconSm          = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Algeo",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
