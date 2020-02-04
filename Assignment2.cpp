// Assignment2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Assignment2.h"
#include <cstdlib>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//Probability variables for determining probability of cars spawning.
int probW = 0;
int probT = 0;

//Color variables for traffic lights.
COLORREF red = RGB(255, 0, 0);
COLORREF green = RGB(0, 255, 0);
COLORREF yellow = RGB(255, 255, 0);
COLORREF grey = RGB(100, 100, 100);
COLORREF lights[3] = {grey, grey, grey};
COLORREF lights2[3] = { grey, grey, grey };

//enumerator for switch statement for traffic lights.
enum lightColor { Red, Redyellow, Green, Yellow };
lightColor color = Red;

//Various bools used for checking conditions of buttons pressed and traffic light states.
bool lButtonDown = false;
bool topRed = false;
bool leftRed = false;
bool left = true;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SetProbDia(HWND, UINT, WPARAM, LPARAM);

//Class for car object. Contains X, Y coordinates, color and whether the car has stopped or not.
class Car {
public:
    int x, y;
    COLORREF color;
    Car(int _x, int _y, COLORREF _color) : x(_x), y(_y), color(_color){}

    void Draw(HDC hdc) {
        HBRUSH brush = CreateSolidBrush(color);
        HGDIOBJ org = SelectObject(hdc, brush);
        Rectangle(hdc, x, y, x+10, y+10);
        SelectObject(hdc, org);
        DeleteObject(brush);
    }
};

//List class for Car. contains method to add and delete cars from list, moving cars in list and returning cars from list.
class CarList {
public:
    Car* t[1000];
    int i;
    CarList() {
        i = 0;
    }
    ~CarList() {
        Clear();
    }

    void push(Car* pCar) {
        t[i++] = pCar;
    }

    void Draw(HDC hdc) {
        for (int k = 0; k < i; k++) {
            t[k]->Draw(hdc);
        }
    }
    void MoveW(int c, int dx, int y) {
        t[c]->x += dx;
        t[c]->y = y;
    }
    void MoveT(int c, int dy, int x) {
        t[c]->y += dy;
        t[c]->x = x;
    }

    void Remove(int ic) {
        delete t[ic];
        if (t[ic + 1] != 0) {
            for (int k = ic+1; k < i; k++) {
                t[k - 1] = t[k];
            }
        }
        i--;
    }

    void Clear() {
        for (int k = 0; k < i; k++) {
            delete t[k];
        }
        i = 0;
    }

    int getSize() {
        return i;
    }

    Car* getThisCar(int i) {
        return t[i];
    }
};

//Create global lists for cars.
CarList listeT;
CarList listeW;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ASSIGNMENT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASSIGNMENT2));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASSIGNMENT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASSIGNMENT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN: {
        //switch (color) {
        //    case Red:{
        //        lights[0] = red;
        //        lights[1] = grey;
        //        lights[2] = grey;

        //        lights2[0] = grey;
        //        lights2[1] = grey;
        //        lights2[2] = green;

        //        color = Redyellow;
        //    }break;
        //    case Redyellow: {
        //        lights[0] = red;
        //        lights[1] = yellow;
        //        lights[2] = grey;

        //        lights2[0] = grey;
        //        lights2[1] = yellow;
        //        lights2[2] = grey;
        //        color = Green;
        //    }break;
        //    case Green: {
        //        lights[0] = grey;
        //        lights[1] = grey;
        //        lights[2] = green;

        //        lights2[0] = red;
        //        lights2[1] = grey;
        //        lights2[2] = grey;
        //        color = Yellow;
        //    }break;
        //    case Yellow: {
        //        lights[0] = grey;
        //        lights[1] = yellow;
        //        lights[2] = grey;

        //        lights2[0] = red;
        //        lights2[1] = yellow;
        //        lights2[2] = grey;
        //        color = Red;
        //    }break;
        //}
        //InvalidateRect(hWnd, 0, 0);

        //Creates timers for lights and cars on left mouse click. Kills timers if they already exist.
        if (!lButtonDown) {
            SetTimer(hWnd, 1, 3000, 0);
            SetTimer(hWnd, 2, 100, 0);
            SetTimer(hWnd, 3, 1000, 0);
        }
        else if (lButtonDown) {
            KillTimer(hWnd, 1);
            KillTimer(hWnd, 2);
            KillTimer(hWnd, 3);
        }
        lButtonDown = !lButtonDown;
        break;
    }
    //Right mouse button creates dialog box for inputting probability of car spawn.
    case WM_RBUTTONDOWN: {
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, SetProbDia);
        InvalidateRect(hWnd, 0, true);
        break;
    }
    case WM_KEYDOWN:
    {
        switch (wParam) {
        //Decides wether west or top lane is selected.
        case VK_LEFT: {
            left = true;
        }break;

        case VK_RIGHT: {
            left = false;
        }break;
        //Increments probability of car spawn.
        case VK_UP: {
            if (left){
                if (probW >= 94) {
                    probW = 99;
                }
                else probW += 5;
            }if (!left) {
                if (probT >= 94) {
                    probT = 99;
                }
                else probT += 5;
            }
        }break;
        //Decrements probability of car spawn.
        case VK_DOWN: {
            if (left) {
                if (probW <= 4) {
                    probW = 0;
                }
                else probW -= 5;
            }if (!left) {
                if (probT <= 4) {
                    probT = 0;
                }
                else probT -= 5;
            }
        }break;

        }
        break;
    }
    case WM_TIMER: {
        //Creates a screen variable to obtain dynamic screen coords. Also initialize stop points for cars.
        RECT screen;
        GetClientRect(hWnd, &screen);
        //Logic for changing traffic light color.
        switch (wParam) {
        case 1: {
            switch (color) {
            case Red: {
                lights[0] = red;
                lights[1] = grey;
                lights[2] = grey;

                lights2[2] = grey;
                lights2[1] = grey;
                lights2[0] = green;

                color = Redyellow;
                topRed = true;
                leftRed = false;
            }break;
            case Redyellow: {
                lights[0] = red;
                lights[1] = yellow;
                lights[2] = grey;

                lights2[2] = grey;
                lights2[1] = yellow;
                lights2[0] = grey;

                color = Green;
            }break;
            case Green: {
                lights[0] = grey;
                lights[1] = grey;
                lights[2] = green;

                lights2[2] = red;
                lights2[1] = grey;
                lights2[0] = grey;
                color = Yellow;

                topRed = false;
                leftRed = true;
            }break;
            case Yellow: {
                lights[0] = grey;
                lights[1] = yellow;
                lights[2] = grey;

                lights2[2] = red;
                lights2[1] = yellow;
                lights2[0] = grey;
                color = Red;
            }break;
            }
            break;
        }
        case 2: {
            int stopY = screen.bottom / 2 - 15;
            int stopX = screen.right / 2 - 15;
            for (int i = 0; i < listeW.getSize(); i++) {
                if (listeW.getThisCar(i)->x < stopX && leftRed && listeW.getThisCar(i)->x >= stopX - 10) {
                    if (stopX < 0) {
                        stopX == 0;
                    }
                    else {
                        stopX -= 10;
                    }
                }
                else {
                    listeW.MoveW(i, 10, screen.bottom / 2 - 5);
                }
                if (listeW.getThisCar(i)->x >= screen.right) {
                    listeW.Remove(i);
                }
            }

            for (int i = 0; i < listeT.getSize(); i++) {
                //if car Y value is smaller than stop value, and the light is green, or the car has passed the intersection, it will go. If the car in front is stopped then the car behind will stop as well, unless the Y value is smaller than the stopY value.
                if (listeT.getThisCar(i)->y < stopY && topRed && listeT.getThisCar(i)->y >= stopY - 10) {
                    if (stopY < 0) {
                        stopY == 0;
                    }
                    else {
                        stopY -= 10;
                    }
                }
                else {
                    listeT.MoveT(i, 10, screen.right / 2 - 5);
                }
                if (listeT.getThisCar(i)->y >= screen.bottom) {
                    listeT.Remove(i);
                }
            }
            break;
        }case 3: {
            int spawnT = rand() % 100;
            if (probT >= spawnT) {
                Car* car = new Car(screen.right / 2 - 5, screen.top, RGB(rand() % 255, rand() % 255, rand() % 255));
                listeT.push(car);
            }

            int spawnW = rand() % 100;
            if (probW >= spawnW) {
                Car* car = new Car(screen.left, screen.bottom / 2 - 5, RGB(rand() % 255, rand() % 255, rand() % 255));
                listeW.push(car);
            }
        }break;
        }
        InvalidateRect(hWnd, 0, true);
        break;
    }
    
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            //Create the brush to paint with.
            HBRUSH brush = CreateSolidBrush(RGB(0,0,0));
            HGDIOBJ org = SelectObject(hdc, brush);
            RECT screen;
            GetClientRect(hWnd, &screen);
            //Paint rectangles to become traffic lights and streets.
            Rectangle(hdc, screen.right/2 - 150, screen.bottom/2 - 330, screen.right/2 - 50, screen.bottom/2 - 30);
            Rectangle(hdc, screen.right / 2 + 150, screen.bottom / 2 + 330, screen.right / 2 + 50, screen.bottom / 2 + 30);
            Rectangle(hdc, screen.left, screen.bottom/2 + 10, screen.right, screen.bottom/2 -10);
            Rectangle(hdc, screen.right/2 - 10, screen.top, screen.right/2 + 10, screen.bottom);
            DeleteObject(brush);
          
			//Change color of brush and paint ellipses to become the lights.
			int height = -320;
			int bottom = -240;
			for (int i = 0; i < 3; i++) {
				brush = CreateSolidBrush(lights[i]);
				SelectObject(hdc, brush);
				Ellipse(hdc, screen.right / 2 - 140, screen.bottom / 2 + height, screen.right / 2 - 60, screen.bottom / 2 + bottom);
				DeleteObject(brush);

				brush = CreateSolidBrush(lights2[i]);
				SelectObject(hdc, brush);
				Ellipse(hdc, screen.right / 2 + 140, screen.bottom / 2 - height, screen.right / 2 + 60, screen.bottom / 2 - bottom);
				DeleteObject(brush);

				height += 100;
				bottom += 100;
			}
            

            //Drawing cars.
            listeW.Draw(hdc);
            listeT.Draw(hdc);

            //Drawing text that shows current probability on screen.
            TextOut(hdc, 100, 30, _T("Sannsynlighet for biler fra Vest: "), 35);
            TextOut(hdc, 100, 100, _T("Sannsynlighet for biler fra Nord: "), 34);
            WCHAR text[5];
            wsprintf(text, L"%i", probT);
            TextOut(hdc, 100, 120, text, lstrlenW(text));
            TextOut(hdc, 120, 120, L"%", 2);

            wsprintf(text, L"%i", probW);
            TextOut(hdc, 100, 50, text, lstrlenW(text));
            TextOut(hdc, 120, 50, L"%", 2);

            //Delete brush.
            SelectObject(hdc, org);
            DeleteObject(brush);

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

// Message handler for about box.
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

//Custom method for handling dialog box logic and actions.
INT_PTR CALLBACK SetProbDia(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG: {
        SetWindowText(hDlg, L"Sannsynlighetsboks");
        HWND setText = GetDlgItem(hDlg, IDC_STATIC1);
        SetWindowText(setText, L"Skriv inn sannsynligheten for at biler skal komme fra Vest og Nord.");
        setText = GetDlgItem(hDlg, IDC_STATIC2);
        SetWindowText(setText, L"Vest:");
        setText = GetDlgItem(hDlg, IDC_STATIC3);
        SetWindowText(setText, L"Nord:");
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            probW = GetDlgItemInt(hDlg, IDC_EDIT1, 0,0);
            probT = GetDlgItemInt(hDlg, IDC_EDIT2, 0,0);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


