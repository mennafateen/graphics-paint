// Paint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Paint.h"
#include <iostream>
#include <math.h>
#include <Commdlg.h>
#include <vector>
#define dot ( a , b ) (( conj ( a ) * ( b )). real ())
#define PI 3.14
using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LPCWSTR choice;
COLORREF objColor = RGB(0, 0, 0); // line color
COLORREF fillColor;
HDC hdc;
COLORREF gColor; // background color
int xLeft = 150, xRight = 850, yTop = 50, yBottom = 400;
int radius = 180, xc = 470, yc = 200;

//===========================
// LINE
//===========================

void drawParametricLine(int xs, int ys, int xe, int ye, COLORREF color, HDC hdc) {
	double n = max(abs(ye - ys), abs(xe - xs));
	double dt = 1.0 / n;
	double dx = (double)(xe - xs)*dt, dy = (double)(ye - ys)*dt;
	double x = (double)xs, y = (double)ys;
	for (int i = 0; i < n; i++) {
		SetPixel(hdc, round(x), round(y), color);
		x += dx; y += dy;
	}
}

void drawDDALine(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color)
{
	int dx = xe - xs;
	int dy = ye - ys;
	SetPixel(hdc, xs, ys, color);
	if (abs(dx) >= abs(dy))
	{
		int x = xs, xinc = dx>0 ? 1 : -1;
		double y = ys, yinc = (double)dy / dx*xinc;
		while (x != xe)
		{
			x += xinc;
			y += yinc;
			SetPixel(hdc, x, round(y), color);
		}
	}
	else
	{
		int y = ys, yinc = dy>0 ? 1 : -1;
		double x = xs, xinc = (double)dx / dy*yinc;
		while (y != ye)
		{
			x += xinc;
			y += yinc;
			SetPixel(hdc, round(x), y, color);
		}
	}
}

//===========================
// CIRCLE
//===========================

void draw8Points(int xc, int yc, int a, int b, COLORREF color, HDC hdc) {
	SetPixel(hdc, xc + a, yc + b, color);
	SetPixel(hdc, xc - a, yc + b, color);
	SetPixel(hdc, xc + a, yc - b, color);
	SetPixel(hdc, xc - a, yc - b, color);
	SetPixel(hdc, xc + b, yc + a, color);
	SetPixel(hdc, xc - b, yc + a, color);
	SetPixel(hdc, xc + b, yc - a, color);
	SetPixel(hdc, xc - b, yc - a, color);
}

void drawPolarCircle(HDC hdc, int radius, int xc, int yc, COLORREF color) {
	double dtheta = 1.0 / radius;
	for (double i = 0; i < PI / 4; i += dtheta) {
		int x = radius * cos(i);
		int y = radius * sin(i);
		draw8Points(xc, yc, x, y, color, hdc);
	}
}

void drawMidpointCircle(int xc, int yc, int radius, COLORREF color, HDC hdc) {
	int x = 0, y = radius;
	int d = 1 - radius;
	draw8Points(xc, yc, x, y, color, hdc);
	while (x < y) {
		if (d < 0) {
			d += (2 * x) + 3;
		}
		else {
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
		draw8Points(xc, yc, x, y, color, hdc);

	}
}

void drawIterativePolarCircle(int xc, int yc, int radius, COLORREF color, HDC hdc) {
	double x = 0, y = radius;
	double dtheta = (double) 1.0 / radius;
	double cosdt = cos(dtheta); double sindt = sin(dtheta);
	draw8Points(xc, yc, round(x), round(y), color, hdc);
	while (x < y) {
		double tempx = (x * cosdt) - (y * sindt);
		y = (y * cosdt) + (x * sindt);
		x = tempx;
		draw8Points(xc, yc, round(x), round(y), color, hdc);
	}
}

void drawCartesianCircle(int xc, int yc, int radius, COLORREF color, HDC hdc) {
	int x = 0, y = radius;
	while (x <= y) {
		y = round(sqrt(pow(radius, 2) - pow(x, 2)));
		draw8Points(xc, yc, x, y, color, hdc);
		x++;
	}
}

int getDistance(int x0, int y0, int x1, int y1) {
	double d = sqrt(pow((y1 - y0), 2) + pow((x1 - x0), 2));
	return (int)d;
}
//==================
// CURVES
//==================

void drawFirstDegreeCurve(HDC hdc, int xs, int ys, int xe, int ye, COLORREF color) {
	double alpha1 = xe - xs;
	double alpha2 = ye - ys;
	double beta1 = xs;
	double beta2 = ys;
	double x, y;
	for (double t = 0; t < 1; t += 0.001) {
		x = alpha1 * t + beta1;
		y = alpha2 * t + beta2;
		SetPixel(hdc, round(x), round(y), color);
	}
}

void drawSecondDegreeCurveUsingMiddlePoint(HDC hdc, int xs, int ys, int xm, int ym, int xe, int ye, COLORREF color) {
	double alpha1 = 2 * xe - 4 * xm + 2 * xs;
	double alpha2 = 2 * ye - 4 * ym + 2 * ys;
	double beta1 = 4 * xm - xe - 3 * xs;
	double beta2 = 4 * ym - ye - 3 * ys;
	double gamma1 = xs;
	double gamma2 = ys;
	double x, y;
	for (double t = 0; t < 1; t += 0.001) {
		x = alpha1 * pow(t, 2) + beta1 * t + gamma1;
		y = alpha2 * pow(t, 2) + beta2 * t + gamma2;
		SetPixel(hdc, round(x), round(y), color);
	}
}

void drawSecondDegreeCurveUsingSlope(HDC hdc, int x0, int x1, int y0, int y1, double slope, COLORREF color) {
	double dt = (double)1 / 1000,
		alpha1 = x1 - slope - x0,
		alpha2 = y1 - slope - y0;

	for (double t = 0; t <= 1; t += dt) {
		double x = (alpha1*pow(t, 2)) + (slope*t) + x0,
			y = (alpha2 * pow(t, 2)) + (slope*t) + y0;
		SetPixel(hdc, round(x), round(y), color);
	}
}

void drawHermiteCurve(HDC hdc, int xs, int ys, int xe, int ye, double T0, double T1, COLORREF color) {
	double alpha1 = T1 + T0 - 2 * xe + 2 * xs;
	double alpha2 = T1 + T0 - 2 * ye + 2 * ys;
	double beta1 = 3 * xe - T1 - 2 * T0 - 3 * xs;
	double beta2 = 3 * ye - T1 - 2 * T0 - 3 * ys;
	double gamma = T0;
	double sigma1 = xs;
	double sigma2 = ys;
	double x, y;
	for (double t = 0; t < 1; t += 0.001) {
		x = alpha1 * pow(t, 3) + beta1 * pow(t, 2) + gamma * t + sigma1;
		y = alpha2 * pow(t, 3) + beta2 * pow(t, 2) + gamma * t + sigma2;
		SetPixel(hdc, x, y, color);
	}
}

void drawBezier(HDC hdc, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color) {
	double dt = (double)1 / 1000;
	double alphax = (-1 * x0) + (3 * x1) + (-3 * x2) + x3,
		betax = (3 * x0) + (-6 * x1) + (3 * x2),
		gammax = (-3 * x0) + (3 * x1),
		sigmax = x0;
	double alphay = (-1 * y0) + (3 * y1) + (-3 * y2) + y3,
		betay = (3 * y0) + (-6 * y1) + (3 * y2),
		gammay = (-3 * y0) + (3 * y1),
		sigmay = y0;

	for (double t = 0; t <= 1; t += dt) {
		double x = (alphax * pow(t, 3)) + (betax * pow(t, 2)) + (gammax * t) + sigmax,
			y = (alphay * pow(t, 3)) + (betay * pow(t, 2)) + (gammay * t) + sigmay;
		SetPixel(hdc, round(x), round(y), color);
	}
}

void DrawCardinalSpline(HDC hdc, POINT points[], double c, int numOfPoints, COLORREF color)
{
	double c1 = (1 - c);
	double* tangents = new double[numOfPoints]; // tangents[0] & tangents[numOfPoints-1] are empty
	for (int i = 1; i < numOfPoints - 1; i++) {
		tangents[i] = (points[i - 1].y - points[i + 1].y) / (points[i - 1].x - points[i + 1].x) * (c1 / 2);
	}
	for (int i = 1; i < numOfPoints - 1; i++) {
		drawHermiteCurve(hdc, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, tangents[i], tangents[i + 1], color);
	}
	drawSecondDegreeCurveUsingSlope(hdc, points[0].x, points[0].y, points[1].x, points[1].y, tangents[1], color);
	drawSecondDegreeCurveUsingSlope(hdc, points[numOfPoints - 1].x, points[numOfPoints - 1].y, points[numOfPoints - 2].x, points[numOfPoints - 2].y, tangents[numOfPoints - 2], color);
}

//==================
// COLOR DIALOG
//==================

COLORREF ShowColorDialog(HWND hwnd) {

	CHOOSECOLOR cc;
	static COLORREF crCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)crCustClr;
	cc.rgbResult = RGB(255, 255, 255);
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	ChooseColor(&cc);

	return cc.rgbResult;
}

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
    LoadStringW(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

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


//==============
// CLIPPING
//==============

void pointClipping(int x, int y, int xleft, int ytop, int xright, int ybottom, COLORREF color)
{
	if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
		SetPixel(hdc, x, y, color);
}

union OutCode
{
	unsigned All : 4;
	struct { unsigned left : 1, top : 1, right : 1, bottom : 1; };
};

OutCode GetOutCode(double x, double y, int xleft, int ytop, int xright, int ybottom)
{
	OutCode out;
	out.All = 0;
	if (x<xleft)out.left = 1; else if (x>xright)out.right = 1;
	if (y<ytop)out.top = 1; else if (y>ybottom)out.bottom = 1;
	return out;
}

void VIntersect(double xs, double ys, double xe, double ye, int x, double *xi, double *yi)
{
	*xi = x;
	*yi = ys + (x - xs)*(ye - ys) / (xe - xs);
}
void HIntersect(double xs, double ys, double xe, double ye, int y, double *xi, double *yi)
{
	*yi = y;
	*xi = xs + (y - ys)*(xe - xs) / (ye - ys);
}

void CohenSuth(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom)
{
	double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
	OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
	OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
	while ((out1.All || out2.All) && !(out1.All & out2.All))
	{
		double xi, yi;
		if (out1.All)
		{
			if (out1.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
			else if (out1.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
			else if (out1.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
			else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
			x1 = xi;
			y1 = yi;
			out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
		}
		else
		{
			if (out2.left)VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
			else if (out2.top)HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
			else if (out2.right)VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
			else HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
			x2 = xi;
			y2 = yi;
			out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
		}
	}
	if (!out1.All && !out2.All)
	{
		MoveToEx(hdc, round(x1), round(y1), NULL);
		LineTo(hdc, round(x2), round(y2));
	}
}

void circlePoint(HDC hdc, int x, int y, int xc, int yc, int radius, COLORREF color) {
	int distance = sqrt(pow((xc - x), 2) + pow((yc - y), 2));
	if (distance <= radius)
		SetPixel(hdc, x, y, color);
}

int circleLineIntersection(const POINT & p0, const POINT & p1,const POINT & cen, double rad, POINT & r1, POINT &r2) {
	// handle degenerate case if p0 == p1
	double a, b, c, t1, t2;
		a = dot(p1 - p0, p1 - p0)?
		b = 2 * dot(p1 - p0, p0 - cen);
		c = dot(p0 - cen, p0 - cen) - rad * rad;
		double det = b * b - 4 * a * c ?
		int res?
		if (fabs(det) < EPS)
			det = 0, res = 1 ?
		else if (det < 0)
			res = 0 ?
		else
			res = 2 ?
			det = sqrt(det)?
			t1 = (-b + det) / (2 * a)?
			t2 = (-b - det) / (2 * a)?
			r1 = p0 + t1 * (p1 - p0)?
			r2 = p0 + t2 * (p1 - p0)?
			return res?
}

void circleLine(HDC hdc, int x0, int y0, int x1, int y1, int xc, int yc, int radius, COLORREF color) {

	bool insidex0 = false, insidex1 = false;
	int distancex0 = sqrt(pow((xc - x0), 2) + pow((yc - y0), 2));
	int distancex1 = sqrt(pow((xc - x1), 2) + pow((yc - y1), 2));
	if (distancex0 <= radius) // if inside
		insidex0 = true;
	if (distancex1 <= radius) // if inside
		insidex1 = true;


}

//==================
// FILLING
//==================


struct Entry
{
	int xmin, xmax;
};

void InitEntries(Entry table[])
{
	for (int i = 0; i<600; i++)
	{
		table[i].xmin = MAXINT;
		table[i].xmax = -MAXINT;
	}
}

void ScanEdge(POINT v1, POINT v2, Entry table[])
{
	if (v1.y == v2.y)return;
	if (v1.y>v2.y)swap(v1, v2);
	double minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
	double x = v1.x;
	int y = v1.y;
	while (y<v2.y)
	{
		if (x<table[y].xmin)table[y].xmin = (int)ceil(x);
		if (x>table[y].xmax)table[y].xmax = (int)floor(x);
		y++;
		x += minv;
	}
}

void DrawSanLines(HDC hdc, Entry table[], COLORREF color)
{
	for (int y = 0; y<600; y++)
		if (table[y].xmin<table[y].xmax)
			for (int x = table[y].xmin; x <= table[y].xmax; x++)
				SetPixel(hdc, x, y, color);
}

void ConvexFill(HDC hdc, POINT p[], int n, COLORREF color)
{
	Entry *table = new Entry[600];
	InitEntries(table);
	POINT v1 = p[n - 1];
	for (int i = 0; i<n; i++)
	{
		POINT v2 = p[i];
		ScanEdge(v1, v2, table);
		v1 = p[i];
	}
	DrawSanLines(hdc, table, color);
	delete table;
}


//==================
// MAIN
//==================

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PAINT);
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
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int numOfPoints = 0;
	static vector<POINT> ps;
    switch (message)
    {

	case WM_RBUTTONDOWN: {
		if (choice == L"convexfilling") {
			hdc = GetDC(hWnd);
			POINT * points = new POINT[numOfPoints];
			fabs
			for (int i = 0; i < numOfPoints; i++)
				points[i] = ps[i];
			ConvexFill(hdc, points, numOfPoints, fillColor);
			ReleaseDC(hWnd, hdc);
			numOfPoints = 0;
			ps.clear();
		}
		else if (choice == L"spline") {
			hdc = GetDC(hWnd);
			POINT * points = new POINT[numOfPoints];
			for (int i = 0; i < numOfPoints; i++)
				points[i] = ps[i];
			DrawCardinalSpline(hdc, points, 0.7, numOfPoints, objColor);
			//ConvexFill(hdc, points, numOfPoints, fillColor);
			ReleaseDC(hWnd, hdc);
			numOfPoints = 0;
			ps.clear();
		}

	} break;
	case WM_LBUTTONDOWN: {
		static int flag = 0;
		static int x0, y0, x1, y1, x2, y2, x3, y3;
		if (choice == L"ddaline") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				drawDDALine(hdc, x0, y0, x1, y1, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"parametricline") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				drawParametricLine(x0, y0, x1, y1, objColor, hdc);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"polarcircle") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				int radius = getDistance(x0, y0, x1, y1);
				drawPolarCircle(hdc, radius, x0, y0, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"iterativepolarcircle") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				int radius = getDistance(x0, y0, x1, y1);
				drawIterativePolarCircle(x0, y0, radius, objColor, hdc);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"midpointcircle") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				int radius = getDistance(x0, y0, x1, y1);
				drawMidpointCircle(x0, y0, radius, objColor, hdc);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"cartesiancircle") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				int radius = getDistance(x0, y0, x1, y1);
				drawCartesianCircle(x0, y0, radius, objColor, hdc);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"seconddegree") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				flag++;
				
			}
			else if (flag == 2) {
				x2 = LOWORD(lParam);
				y2 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				drawSecondDegreeCurveUsingMiddlePoint(hdc, x0, y0, x1, y1, x2, y2, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"hermite") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				drawHermiteCurve(hdc, x0, y0, x1, y1, 500, 500, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"bezier") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				flag++;

			}
			else if (flag == 2) {
				x2 = LOWORD(lParam);
				y2 = HIWORD(lParam);
				flag++;
				
			}
			else if (flag == 3) {
				hdc = GetDC(hWnd);
				drawBezier(hdc, x0, y0, x1, y1, x2, y2, x3, y3, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"spline") {
			numOfPoints++;
			//hdc = GetDC(hWnd);
			x0 = LOWORD(lParam);
			y0 = HIWORD(lParam);
			POINT p;
			p.x = x0;
			p.y = y0;
			ps.push_back(p);
			//if (numOfPoints == 4) {
				//POINT * points = new POINT[numOfPoints];
				//for (int i = 0; i < numOfPoints; i++)
					//points[i] = ps[i];
				//DrawCardinalSpline(hdc, points, 0.5, 4, objColor);
			//}
			//ReleaseDC(hWnd, hdc);
		}
		else if (choice == L"firstdegree") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				drawFirstDegreeCurve(hdc, x0, y0, x1, y1, objColor);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"pointclipping") {
			hdc = GetDC(hWnd);
			x0 = LOWORD(lParam);
			y0 = HIWORD(lParam);
			pointClipping(x0, y0, xLeft, yTop, xRight, yBottom, objColor);
			ReleaseDC(hWnd, hdc);
		}
		else if (choice == L"lineclipping") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				CohenSuth(hdc, x0, y0, x1, y1, xLeft, yTop, xRight, yBottom);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"circlepoint") {
			hdc = GetDC(hWnd);
			x0 = LOWORD(lParam);
			y0 = HIWORD(lParam);
			circlePoint(hdc, x0, y0, xc, yc, radius, objColor);
			ReleaseDC(hWnd, hdc);
		}
		else if (choice == L"circleline") {
			if (flag == 0) {
				x0 = LOWORD(lParam);
				y0 = HIWORD(lParam);
				flag++;
			}
			else if (flag == 1) {
				x1 = LOWORD(lParam);
				y1 = HIWORD(lParam);
				hdc = GetDC(hWnd);
				//circleLine(hdc, x0, y0, x1, y1, xc, yc, radius);
				ReleaseDC(hWnd, hdc);
				flag = 0;
			}
		}
		else if (choice == L"convexfilling") {
			numOfPoints++;
			//hdc = GetDC(hWnd);
			x0 = LOWORD(lParam);
			y0 = HIWORD(lParam);
			POINT p;
			p.x = x0;
			p.y = y0;
			ps.push_back(p);
		//	if (numOfPoints == 4) {
			//	

		}

		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_LINE_DDA: {
				choice = L"ddaline";
			}
				break;
			case ID_LINE_PARAMETRIC: {
				choice = L"parametricline";
			}
				 break;
			case ID_CIRCLE_CARTESIAN: {
				choice = L"cartesiancircle";
			}
				break;
			case ID_CIRCLE_POLAR: {
				choice = L"polarcircle";
			}
				break;
			case ID_CIRCLE_ITERATIVEPOLAR: {
				choice = L"iterativepolarcircle";
			}
				break;
			case ID_CIRCLE_MIDPOINT: {
				choice = L"midpointcircle";
			}
				break;
			case ID_CURVE_FIRSTDEGREE: {
				choice = L"firstdegree";
			}
				break;
			case ID_CURVE_SECONDDEGREE: {
				choice = L"seconddegree";
			}
				break;
			case ID_THIRDDEGREE_HERMITE: {
				choice = L"hermite";
			}
				 break;
			case ID_THIRDDEGREE_BEZIER: {
				choice = L"bezier";
			}
				break;
			case ID_CURVE_CARDINALSPLINE: {
				choice = L"spline";
			} break;
			case ID_CLIPPING_POINT: {
				choice = L"pointclipping";
				hdc = GetDC(hWnd);
				drawParametricLine(xLeft, yTop, xLeft, yBottom, objColor, hdc);
				drawParametricLine(xLeft, yTop, xRight, yTop, objColor, hdc);
				drawParametricLine(xRight, yTop, xRight, yBottom, objColor, hdc);
				drawParametricLine(xRight, yBottom, xLeft, yBottom, objColor, hdc);
				ReleaseDC(hWnd, hdc);
			} break;
			case ID_CLIPPING_CIRCLEPOINT: {
				choice = L"circlepoint";
				hdc = GetDC(hWnd);
				drawPolarCircle(hdc, radius, xc, yc, objColor);
				ReleaseDC(hWnd, hdc);
			} break;
			case ID_CLIPPING_LINE: {
				choice = L"lineclipping";
				hdc = GetDC(hWnd);
				drawParametricLine(xLeft, yTop, xLeft, yBottom, objColor, hdc);
				drawParametricLine(xLeft, yTop, xRight, yTop, objColor, hdc);
				drawParametricLine(xRight, yTop, xRight, yBottom, objColor, hdc);
				drawParametricLine(xRight, yBottom, xLeft, yBottom, objColor, hdc);
				ReleaseDC(hWnd, hdc);
			} break;
			case ID_CLIPPING_CIRCLELINE: {
				choice = L"circleline";
				hdc = GetDC(hWnd);
				drawPolarCircle(hdc, radius, xc, yc, objColor);
				ReleaseDC(hWnd, hdc);
			} break;
			case ID_FILL_BACKGROUND: {
				gColor = ShowColorDialog(hWnd);
				InvalidateRect(hWnd, NULL, TRUE);
			} break;
			case ID_FILL_OBJECT: {
				choice = L"convexfilling";
				fillColor = ShowColorDialog(hWnd);
			} break;
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
	case WM_ERASEBKGND:
	{
		HDC  hdc;
		RECT rc;
		hdc = (HDC)wParam;
		GetClientRect(hWnd, &rc);
		HBRUSH brush = CreateSolidBrush(gColor);
		FillRect(hdc, &rc, brush);
		return TRUE;

	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
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
