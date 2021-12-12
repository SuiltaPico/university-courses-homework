#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <afxwin.h>
#include <cstdlib>

#ifndef Round
#define Round(x) int(floor(x + .5))
#endif

void dda_line(CDC* cdc, CPoint point1, CPoint point2, COLORREF color = RGB(0, 0, 0))
{
    int x_difference = point2.x - point1.x;
    int y_difference = point2.y - point1.y;

    double k = abs((double)y_difference / (double)x_difference);
    int x_unit = x_difference / abs(x_difference);
    int y_unit = y_difference / abs(y_difference);

    if (abs(x_difference) > abs(y_difference))
    {
        double y = point1.y;
        k *= y_unit;
        for (int x = point1.x; x != point2.x; x += x_unit)
        {
            cdc->SetPixelV(CPoint(x, Round(y)), color);
            y += k;
        }
    }
    else
    {
        double x = point1.x;
        double kd1 = 1 / k * x_unit;
        for (int y = point1.y; y != point2.y; y += y_unit)
        {
            cdc->SetPixelV(CPoint(Round(x), y), color);
            x += kd1;
        }
    }
}