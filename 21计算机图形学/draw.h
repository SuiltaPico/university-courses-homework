#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <afxwin.h>
#include <cstdlib>
#include <cstdint>
#include <vector>

using std::vector;


#ifndef Round
#define Round(x) int(floor(x + .5))
#endif

/* 直线一般方程 */
class GeneralEquationOfLine {
public:
    int A, B, C;
public:
    //GeneralEquationOfLine() :A(0), B(0), C(0) {}
    //GeneralEquationOfLine(const GeneralEquationOfLine& ref) :A(ref.A), B(ref.B), C(ref.C) {}
    //GeneralEquationOfLine(int _A = 0, int _B = 0, int _C = 0) :A(_A), B(_B), C(_C) {}
    GeneralEquationOfLine(CPoint point1, CPoint point2 = CPoint(0, 0)) :A(point2.y - point1.y), B(point1.x - point2.x), C(point1.x* point2.y - point1.y * point2.x) {}
    double get_value(double x, double y) const {
        return A * x + B * y + C;
    }
};

/* 直线斜截式方程 */
class ObliqueSectionEquationOfLine {
public:
    double k, b;
    ObliqueSectionEquationOfLine(GeneralEquationOfLine ge) :k(-(double)ge.A / ge.B), b((double)-ge.C / ge.B) {}
    double calculate_y(int x) const {
        return k * x + b;
    }
    double calculate_x(int y) const {
        return ((double)y - b) / k;
    }
};

// 使用 DDA 算法绘制直线
void DDA_line(CDC* cdc, CPoint point1, CPoint point2 = CPoint(0, 0), COLORREF color = RGB(0, 0, 0)) {
    // 两点 x 坐标的差值
    const int dx = point2.x - point1.x;
    // 两点 y 坐标的差值
    const int dy = point2.y - point1.y;

    // 带符号的差值单位
    int x_unit, y_unit;

    // 先处理 dx 或 dy 为0的情况，防止除0错误。
    if (dx == 0)
    {
        y_unit = dy / abs(dy);
        for (int y = point1.y; y != point2.y; y += y_unit)
            cdc->SetPixelV(CPoint(point1.x, y), color);
    }
    else if (dy == 0) {
        x_unit = dx / abs(dx);
        for (int x = point1.x; x != point2.x; x += x_unit)
            cdc->SetPixelV(CPoint(x, point1.y), color);
    }
    else {
        // 计算 k 的绝对值
        double k = abs((double)dy / (double)dx);
        // 获得符号
        x_unit = dx / abs(dx);
        y_unit = dy / abs(dy);

        if (abs(dx) > abs(dy))
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
            k = 1 / k * x_unit;
            for (int y = point1.y; y != point2.y; y += y_unit)
            {
                cdc->SetPixelV(CPoint(Round(x), y), color);
                x += k;
            }
        }
    }
}

// 使用中点 Bresenham 算法绘制直线
void mid_point_Bresenham_line(CDC* cdc, CPoint point1, CPoint point2 = CPoint(0, 0), COLORREF color = RGB(0, 0, 0)) {
    const int dx = point2.x - point1.x;
    const int dy = point2.y - point1.y;

    int x_unit, y_unit;

    // 先处理 dx 或 dy 为0的情况，防止除0错误。
    if (dx == 0)
    {
        y_unit = dy / abs(dy);
        for (int y = point1.y; y != point2.y; y += y_unit)
            cdc->SetPixelV(CPoint(point1.x, y), color);
    }
    else if (dy == 0) {
        x_unit = dx / abs(dx);
        for (int x = point1.x; x != point2.x; x += x_unit)
            cdc->SetPixelV(CPoint(x, point1.y), color);
    }
    else {
        // 计算直线一般方程
        const auto ge = GeneralEquationOfLine(point1, point2);

        const int x_unit = dx / abs(dx);
        const int y_unit = dy / abs(dy);

        double d;

        if (abs(dx) > abs(dy))
        {
            const double half_y_unit = y_unit * .5;
            int y = point1.y;
            for (int x = point1.x; x != point2.x; x += x_unit)
            {
                cdc->SetPixelV(CPoint(x, y), color);
                d = ge.get_value(x + x_unit, y + half_y_unit);
                y += (x_unit != y_unit) ? ((d < 0) ? y_unit : 0) : ((d > 0) ? y_unit:0);
            }
        }
        else
        {
            const double half_x_unit = x_unit * .5;
            int x = point1.x;
            for (int y = point1.y; y != point2.y; y += y_unit)
            {
                cdc->SetPixelV(CPoint(x, y), color);
                d = ge.get_value(x + half_x_unit, y + y_unit);
                x += (x_unit == y_unit) ? ((d < 0) ? x_unit : 0) : ((d > 0) ?  x_unit:0);
            }
        }
    }
}

// 使用中点 Bresenham 算法与彩色支持的 Wu 反走样算法绘制直线
void Bresenham_with_Wu_anti_aliasing_line(CDC* cdc, CPoint point1, CPoint point2 = CPoint(0, 0), const COLORREF color = RGB(0, 0, 0), bool center_origin = true) {
    const int dx = point2.x - point1.x;
    const int dy = point2.y - point1.y;

    int x_unit, y_unit;

    // 先处理 dx 或 dy 为0的情况，防止除0错误。
    if (dx == 0)
    {
        y_unit = dy / abs(dy);
        for (int y = point1.y; y != point2.y; y += y_unit)
            cdc->SetPixelV(CPoint(point1.x, y), color);
    }
    else if (dy == 0) {
        x_unit = dx / abs(dx);
        for (int x = point1.x; x != point2.x; x += x_unit)
            cdc->SetPixelV(CPoint(x, point1.y), color);
    }
    else {
        // 计算直线斜截式方程
        const auto ose = ObliqueSectionEquationOfLine(GeneralEquationOfLine(point1, point2));

        // xy 的带符号单位
        const int x_unit = dx / abs(dx);
        const int y_unit = dy / abs(dy);

        //cdc->GetPixel() 效率太低，改用 GetCurrentBitmap
        CBitmap* btm = cdc->GetCurrentBitmap();
        BITMAP btmh;
        btm->GetBitmap(&btmh);
        const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
        BYTE* btmb = new BYTE[btms];
        btm->GetBitmapBits(btms, btmb);
        const BYTE* const btmb_c = btmb;

        CPoint origin(0, 0);
        if (center_origin) {
            origin = CPoint(btmh.bmWidth / 2, btmh.bmHeight / 2);
        }

        double d = 0;
        // 提前计算颜色各值，提升效率
        const auto color_R = GetRValue(color);
        const auto color_G = GetGValue(color);
        const auto color_B = GetBValue(color);

        if (abs(dx) > abs(dy))   
        {
            int y = point1.y;
            // point1.x 加 x 单位到 point2.x
            for (int x = point1.x; x != point2.x + x_unit; x += x_unit)
            {
                // 映射规则：index = (btmh.bmHeight - (原点.y + y - 11)) * btmh.bmWidthBytes + (原点.x + x) * 4;
                // R = index + 2，G = index + 1，B = index
                const auto index0 = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (origin.x + x) * 4;
                const auto index1 = index0 - y_unit * btmh.bmWidthBytes;
                const auto d_add_n1 = 1. - d;

                cdc->SetPixelV(CPoint(x, y), RGB(
                    ((int)btmb_c[index0 + 2] - color_R) * d + color_R,
                    ((int)btmb_c[index0 + 1] - color_G) * d + color_G,
                    ((int)btmb_c[index0] - color_B) * d + color_B
                ));
                cdc->SetPixelV(CPoint(x, y + y_unit), RGB(
                    ((int)btmb_c[index1 + 2] - color_R) * d_add_n1 + color_R,
                    ((int)btmb_c[index1 + 1] - color_G) * d_add_n1 + color_G,
                    ((int)btmb_c[index1] - color_B) * d_add_n1 + color_B
                    ));
                d += abs(ose.k);
                if (d >= 1) {
                    d -= 1;
                    y += y_unit;
                }
            }
        }
        else
        {
            int x = point1.x;
            for (int y = point1.y; y != point2.y + y_unit; y += y_unit)
            {
                const auto index0 = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (origin.x + x) * 4;
                const auto index1 = index0 + x_unit * 4;
                const auto d_add_n1 = 1. - d;

                cdc->SetPixelV(CPoint(x, y), RGB(
                    ((int)btmb_c[index0 + 2] - color_R) * d + color_R,
                    ((int)btmb_c[index0 + 1] - color_G) * d + color_G,
                    ((int)btmb_c[index0] - color_B) * d + color_B
                ));
                cdc->SetPixelV(CPoint(x + x_unit, y), RGB(
                    ((int)btmb_c[index1 + 2] - color_R) * d_add_n1 + color_R,
                    ((int)btmb_c[index1 + 1] - color_G) * d_add_n1 + color_G,
                    ((int)btmb_c[index1] - color_B) * d_add_n1 + color_B
                ));
                d += 1 / abs(ose.k);
                if (d >= 1) {
                    d -= 1;
                    x += x_unit;
                }
            }
        } 
        // 释放内存
        delete[] btmb;
    }
}

// 边缘填充算法
void edge_filling(CDC* cdc, const vector<CPoint> points, const COLORREF color = RGB(0, 0, 0), bool center_origin = true) {
    auto points_size = points.size();

    // 缓存之前的 bitmap 作为背景色
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);
    const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
    BYTE* before_btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, before_btmb);

    const auto color_R = GetRValue(color);
    const auto color_G = GetGValue(color);
    const auto color_B = GetBValue(color);

    CPoint origin(0, 0);
    if (center_origin) {
        origin = CPoint(btmh.bmWidth / 2, btmh.bmHeight / 2);
    }

    BYTE* btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, btmb);

    for (size_t p_i = 0; p_i < points_size; p_i++)
    {
        CPoint p0 = points[p_i % points_size];
        CPoint p1 = points[(p_i+1) % points_size];

        auto ose = ObliqueSectionEquationOfLine(GeneralEquationOfLine(p0, p1));

        auto dy = p1.y - p0.y;
        auto dx = p1.x - p0.x;

        if (dy != 0) {
            auto y_unit = dy / abs(dy);

            for (auto y = p0.y; y != p1.y + y_unit; y += y_unit)
            {
                auto x_i = ((dx != 0) ? LONG(ose.calculate_x(y)):0) + origin.x + p0.x;
                for (; x_i < btmh.bmWidth; x_i++)
                {
                    auto index = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (x_i) * 4;
                    if (btmb[index] == before_btmb[index] && btmb[index + 1] == before_btmb[index + 1] && btmb[index + 2] == before_btmb[index + 2]) {
                        //cdc->SetPixelV(CPoint(x, y), color);
                        btmb[index + 2] = color_R;
                        btmb[index + 1] = color_G;
                        btmb[index] = color_B;
                    }
                    else {
                        /*cdc->SetPixelV(CPoint(x, y), RGB(
                            before_btmb[index + 2],
                            before_btmb[index + 1],
                            before_btmb[index]
                        ));*/
                        btmb[index + 2] = before_btmb[index + 2];
                        btmb[index + 1] = before_btmb[index + 1];
                        btmb[index] = before_btmb[index];
                    }
                }
            }
            btm->SetBitmapBits(btms, btmb);
        }
    }
    delete[] btmb;
    delete[] before_btmb;
}