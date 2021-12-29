#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <afxwin.h>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <ctime>
#include <unordered_set>
#include <functional>

using std::vector;
using std::sort;
using std::unordered_set;


#ifndef Round
#define Round(x) int(floor(x + .5))
#endif

int calcX(int x, int picture_width) {
    return x - picture_width / 2;
}

// 设置图像的颜色
void set_pixel(BYTE* btmb, BITMAP btmh, CPoint point,BYTE red, BYTE green, BYTE blue) {
    auto index = (btmh.bmHeight - (point.y - 11)) * btmh.bmWidthBytes + (point.x) * 4;
    btmb[index + 2] = red;
    btmb[index + 1] = green;
    btmb[index] = blue;
}

template<typename T, typename U, typename V>
T rangeLimit(T value, U min, V max) {
    return (value > max) ? max : ((value < min) ? min : value);
}

/* 直线一般方程 */
class GeneralEquationOfLine {
public:
    int A, B, C;
public:
    GeneralEquationOfLine() :A(0), B(0), C(0) {}
    //GeneralEquationOfLine(const GeneralEquationOfLine& ref) :A(ref.A), B(ref.B), C(ref.C) {}
    //GeneralEquationOfLine(int _A = 0, int _B = 0, int _C = 0) :A(_A), B(_B), C(_C) {}
    GeneralEquationOfLine(CPoint point1, CPoint point2 = CPoint(0, 0)) :A(point2.y - point1.y), B(point1.x - point2.x), C(point1.y* point2.x - point1.x* point2.y) {}
    double get_value(double x, double y) const {
        return A * x + B * y + C;
    }
};

/* 直线斜截式方程 */
class ObliqueSectionEquationOfLine {
public:
    double k, b;
    ObliqueSectionEquationOfLine(GeneralEquationOfLine ge) :k(-(double)ge.A / ge.B), b(-(double)ge.C / ge.B) {}
    double calculate_y(int x) const {
        return k * x + b;
    }
    double calculate_x(int y) const {
        return ((double)y - b) / k;
    }
};

class HSLColor {
public:
    double 
        /* 0~360 */
        H,
        /* 0~1 */
        S,
        /* 0~1 */
        L;
    /* 算法来自 https://www.cnblogs.com/kevinzhwl/archive/2010/05/10/3878939.html */
    static HSLColor fromRGB(COLORREF value) {
        double H, S, L;
        double 
            r = GetRValue(value) / 255.0f,
            g = GetGValue(value) / 255.0f,
            b = GetBValue(value) / 255.0f;

        double 
            maxVal = max(r, max(g, b)),
            minVal = min(r, min(g, b));

        if (maxVal == minVal)
            H = 0; // undefined
        else if (maxVal == r && g >= b)
            H = 60.0f * (g - b) / (maxVal - minVal);
        else if (maxVal == r && g < b)
            H = 60.0f * (g - b) / (maxVal - minVal) + 360.0f;
        else if (maxVal == g)
            H = 60.0f * (b - r) / (maxVal - minVal) + 120.0f;
        else if (maxVal == b)
            H = 60.0f * (r - g) / (maxVal - minVal) + 240.0f;

        L = (maxVal + minVal) / 2.0f;

        if (L == 0 || maxVal == minVal)
            S = 0;
        else if (0 < L && L <= 0.5f)
            S = (maxVal - minVal) / (maxVal + minVal);
        else if (L > 0.5f)
            S = (maxVal - minVal) / (2 - (maxVal + minVal));
        return { rangeLimit(H, 0, 360), rangeLimit(S, 0, 1), rangeLimit(L, 0, 1) };
    }
    /* 算法来自 https://www.cnblogs.com/kevinzhwl/archive/2010/05/10/3878939.html */
    COLORREF toRGB() {
        double R, G, B;
        if (S == 0)
            R = G = B = L * 255.0f;
        else {
            float q = (L < 0.5f) ? (L * (1.0f + S)) : (L + S - (L * S));
            float p = (2.0f * L) - q;
            float Hk = H / 360.0f;
            float T[3];
            T[0] = Hk + 1. / 3;
            T[1] = Hk;
            T[2] = Hk - 1. / 3;
            for (int i = 0; i < 3; i++)
            {
                if (T[i] < 0)
                    T[i] += 1.0f;
                if (T[i] > 1) 
                    T[i] -= 1.0f;
                if ((T[i] * 6) < 1)
                    T[i] = p + ((q - p) * 6.0f * T[i]);
                else if ((T[i] * 2.0f) < 1)
                    T[i] = q;
                else if ((T[i] * 3.0f) < 2)
                    T[i] = p + (q - p) * ((2.0f / 3.0f) - T[i]) * 6.0f;
                else 
                    T[i] = p;
            }
            R = T[0] * 255.0f;
            G = T[1] * 255.0f;
            B = T[2] * 255.0f;
        }
        return RGB(R, G, B);
    }
};

/* 颜色线性数据单元 */
struct ColorInterpolationUnit
{
    /* 颜色值 */
    COLORREF value;
    /* 百分比，范围为0到100，超出后可能会导致无法预测的行为 */
    double percentage;
};

/* 颜色线性插值 */
class HSLColorLinearInterpolation {
public:
    vector<ColorInterpolationUnit> units;
    HSLColorLinearInterpolation() {
        units = {};
    }
    HSLColorLinearInterpolation(vector<ColorInterpolationUnit>& _units) {
        units = _units;
        sort(units.begin(), units.end(), [](ColorInterpolationUnit& u1, ColorInterpolationUnit& u2) {
            return u1.percentage < u2.percentage;
        });
    }
    COLORREF get_color(double percentage) const {
        // 空单元的百分比
        const auto null_unit = DBL_MIN;
        auto data_size = units.size();
        if (data_size > 0) {
            ColorInterpolationUnit front_unit{ 0, null_unit }, back_unit{ 0,null_unit };
            if (percentage < units.front().percentage) {
                return units.front().value;
            }
            else {
                for (size_t i = 0; i < data_size; i++)
                {
                    front_unit = back_unit;
                    back_unit = units[i];
                    if (front_unit.percentage <= percentage && back_unit.percentage >= percentage) {
                        break;
                    }
                }

                // 处理百分比大于所有插值百分比的情况
                if (back_unit.percentage < percentage) {
                    front_unit = back_unit;
                    back_unit = { 0,null_unit };
                }
            }

            // 百分比大于最大插值，返回最大插值的颜色
            if (back_unit.percentage == null_unit) {
                return units.back().value;
            }
            else {
                auto front_hsl_color = HSLColor::fromRGB(front_unit.value);
                auto back_hsl_color = HSLColor::fromRGB(back_unit.value);
                auto local_percentage = (percentage - front_unit.percentage) / (back_unit.percentage - front_unit.percentage);
                auto dH = back_hsl_color.H - front_hsl_color.H;
                // 向距离最近的 H 方向变换
                if (dH > 180) {
                    dH = -(360 - dH);
                }
                auto
                    H = (front_hsl_color.H + dH * local_percentage),
                    S = front_hsl_color.S + (back_hsl_color.S - front_hsl_color.S) * local_percentage,
                    L = front_hsl_color.L + (back_hsl_color.L - front_hsl_color.L) * local_percentage;
                
                HSLColor new_color = { H,S,L };
                return new_color.toRGB();
            }
        }
        else {
            return RGB(0, 0, 0);
        }
    }
};

/** 二维的 HSL 颜色的线性插值
 * 支持把点映射到线性插值的直线上
 */
class HSLColorLinearInterpolation2D : public HSLColorLinearInterpolation {
public:
    CPoint p_start, p_end;
    GeneralEquationOfLine ge;

    using HSLColorLinearInterpolation::get_color;
    /** 
     * @param p_start 渐变色的起始点
     * @param p_end 渐变色的终止点
     * @param _units 颜色插值单元
     */
    HSLColorLinearInterpolation2D(CPoint p_start, CPoint p_end, vector<ColorInterpolationUnit>& _units) {
        this->p_start = p_start;
        this->p_end = p_end;
        ge = GeneralEquationOfLine(p_start, p_end);
        units = _units;
        // 对插值排序
        sort(units.begin(), units.end(), [](ColorInterpolationUnit& u1, ColorInterpolationUnit& u2) {
            return u1.percentage < u2.percentage;
            });
    }
    COLORREF get_color_from_point(CPoint point) const {
        // 如果为一个点
        if (ge.A == 0 && ge.B == 0) {
            return get_color(100.);
        }
        // 如果与x轴平行
        else if (ge.A == 0) {
            return get_color((double)(point.x - p_start.x) / (p_end.x - p_start.x) * 100);
        }
        // 如果与y轴平行
        else if (ge.B == 0) {
            return get_color((double)(point.y - p_start.y) / (p_end.y - p_start.y) * 100);
        }
        else {
            ObliqueSectionEquationOfLine ose(ge);
            double x = (ose.k * ((double)point.y - ose.b) + point.x) / (pow(ose.k, 2)+1);
            return get_color(((double)(x - p_start.x) / -ge.B) * 100);
        }
    }
};

// 使用 DDA 算法绘制单色直线
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

// 使用中点 Bresenham 算法绘制单色直线
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

// 使用中点 Bresenham 算法与彩色渐变支持的 Wu 反走样算法绘制直线
void Bresenham_with_Wu_anti_aliasing_line(
    CDC* cdc, CPoint point1, CPoint point2 = CPoint(0, 0),
    HSLColorLinearInterpolation cli = HSLColorLinearInterpolation(vector<ColorInterpolationUnit>()),
    bool center_origin = true) {
    const int dx = point2.x - point1.x;
    const int dy = point2.y - point1.y;

    int x_unit, y_unit;

    //cdc->GetPixel() 效率太低，改用 GetCurrentBitmap
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);
    const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
    BYTE* btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, btmb);

    CPoint origin(0, 0);
    if (center_origin) {
        origin = CPoint(btmh.bmWidth / 2, btmh.bmHeight / 2);
    }

    // 先处理 dx 或 dy 为0的情况，防止除0错误。
    if (dx == 0)
    {
        y_unit = dy / abs(dy);
        for (int y = point1.y; y != point2.y; y += y_unit) {
            auto color = cli.get_color((double)(y - point1.y) / dy * 100);
            set_pixel(btmb, btmh, CPoint(point1.x, y) + origin, GetRValue(color), GetGValue(color), GetBValue(color));
            //cdc->SetPixelV(CPoint(point1.x, y), cli.get_color((double)(y - point1.y) / dy * 100));
        }
    }
    else if (dy == 0) {
        x_unit = dx / abs(dx);
        for (int x = point1.x; x != point2.x; x += x_unit) {
            auto color = cli.get_color((double)(x - point1.x) / dx * 100);
            set_pixel(btmb, btmh, CPoint(x, point1.y) + origin, GetRValue(color), GetGValue(color), GetBValue(color));
            //cdc->SetPixelV(CPoint(x, point1.y), cli.get_color((double)(x - point1.x) / dx * 100));
        }
    }
    else {
        // 计算直线斜截式方程
        const auto ose = ObliqueSectionEquationOfLine(GeneralEquationOfLine(point1, point2));

        // xy 的带符号单位
        const int x_unit = dx / abs(dx);
        const int y_unit = dy / abs(dy);


        double d = 0;

        if (abs(dx) > abs(dy))   
        {
            int y = point1.y;
            // point1.x 加 x 单位到 point2.x
            for (int x = point1.x; x != point2.x + x_unit; x += x_unit)
            {
                auto color = cli.get_color((double)(x - point1.x) / dx * 100);
                const auto color_R = GetRValue(color);
                const auto color_G = GetGValue(color);
                const auto color_B = GetBValue(color);

                // 映射规则：index = (btmh.bmHeight - (原点.y + y - 11)) * btmh.bmWidthBytes + (原点.x + x) * 4;
                // R = index + 2，G = index + 1，B = index
                const auto index0 = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (origin.x + x) * 4;
                const auto index1 = index0 - y_unit * btmh.bmWidthBytes;
                const auto d_add_n1 = 1. - d;

                btmb[index0 + 2] = ((int)btmb[index0 + 2] - color_R) * d + color_R;
                btmb[index0 + 1] = ((int)btmb[index0 + 1] - color_G) * d + color_G;
                btmb[index0] = ((int)btmb[index0] - color_B) * d + color_B;

                btmb[index1 + 2] = ((int)btmb[index1 + 2] - color_R) * d_add_n1 + color_R;
                btmb[index1 + 1] = ((int)btmb[index1 + 1] - color_G) * d_add_n1 + color_G;
                btmb[index1] = ((int)btmb[index1] - color_B) * d_add_n1 + color_B;

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
                auto color = cli.get_color((double)(y - point1.y) / dy * 100);
                const auto color_R = GetRValue(color);
                const auto color_G = GetGValue(color);
                const auto color_B = GetBValue(color);

                const auto index0 = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (origin.x + x) * 4;
                const auto index1 = index0 + x_unit * 4;
                const auto d_add_n1 = 1. - d;

                btmb[index0 + 2] = ((int)btmb[index0 + 2] - color_R) * d + color_R;
                btmb[index0 + 1] = ((int)btmb[index0 + 1] - color_G) * d + color_G;
                btmb[index0] = ((int)btmb[index0] - color_B) * d + color_B;

                btmb[index1 + 2] = ((int)btmb[index1 + 2] - color_R) * d_add_n1 + color_R;
                btmb[index1 + 1] = ((int)btmb[index1 + 1] - color_G) * d_add_n1 + color_G;
                btmb[index1] = ((int)btmb[index1] - color_B) * d_add_n1 + color_B;
                d += 1 / abs(ose.k);
                if (d >= 1) {
                    d -= 1;
                    x += x_unit;
                }
            }
        }
    }
    btm->SetBitmapBits(btms, btmb);
    // 释放内存
    delete[] btmb;
}

/* 扫描线算法，支持彩色渐变，很慢 */
void scan_line_filling(
    CDC* cdc, const vector<CPoint> points,
    HSLColorLinearInterpolation2D cli2d = HSLColorLinearInterpolation2D(CPoint(0, 0), CPoint(0, 0), vector<ColorInterpolationUnit>())
) {
    // 缓存之前的 bitmap 作为背景色
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);
    const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
    BYTE* btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, btmb);

    CPoint min_point = CPoint(points.front().x, points.front().y);
    CPoint max_point = CPoint(points.back().x, points.back().y);

    /* 找到最大最小 xy 坐标 */
    for (auto iter = points.begin(); iter != points.end(); iter++) {
        min_point.x = min(min_point.x, iter->x);
        min_point.y = min(min_point.y, iter->y);
        max_point.x = max(max_point.x, iter->x);
        max_point.y = max(max_point.y, iter->y);
    }

    for (int y = min_point.y; y <= max_point.y; y++)
    {
        auto points_size = points.size();
        unordered_set<int> x_set;
        for (size_t p_i = 0; p_i < points_size; p_i++)
        {
            CPoint p0 = points[p_i % points_size];
            CPoint p1 = points[(p_i + 1) % points_size];

            auto ge = GeneralEquationOfLine(p0, p1);

            if (y >= min(p0.y, p1.y) && y < max(p0.y, p1.y)) {
                // 跳过 X1 - X2 == 0
                if (ge.B == 0)
                    continue;
                // 垂直线算一个点
                else if (ge.A == 0) {
                    x_set.insert(p0.x);
                    continue;
                }

                auto ose = ObliqueSectionEquationOfLine(ge);
                // 计算 x 坐标
                x_set.insert(Round(ose.calculate_x(y)));
            }
        }

        // 绘画开关
        int draw_switch = false;
        // 扫描
        for (int x = min_point.x; x <= max_point.x; x++)
        {
            draw_switch += x_set.count(x);
            draw_switch = (bool)(draw_switch % 2);
            if (draw_switch)
            {
                auto point = CPoint(x, y);
                auto color = cli2d.get_color_from_point(point);
                cdc->SetPixelV(point, color);
            }
        }
    }
}

// 边缘填充算法，支持彩色渐变
void edge_filling(
    CDC* cdc, const vector<CPoint> points,
    HSLColorLinearInterpolation2D cli2d = HSLColorLinearInterpolation2D(CPoint(0, 0), CPoint(0, 0), vector<ColorInterpolationUnit>()),
    bool center_origin = true
) {
    auto points_size = points.size();

    // 缓存之前的 bitmap 作为背景色
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);
    const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
    BYTE* before_btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, before_btmb);

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
                auto x = ((dx != 0) ? LONG(ose.calculate_x(y)) : p0.x);
                auto x_i = x + origin.x;
                
                // 不停地向右检查
                for (; x_i < btmh.bmWidth; x_i++,x++)
                {
                    // 计算 BitmapBit 索引
                    auto index = (btmh.bmHeight - (origin.y + y - 11)) * btmh.bmWidthBytes + (x_i) * 4;
                    // 如果颜色与原色相同，则填充
                    if ((btmb[index] == before_btmb[index]) && (btmb[index + 1] == before_btmb[index + 1]) && (btmb[index + 2] == before_btmb[index + 2])) {
                        auto color = cli2d.get_color_from_point(CPoint(x, y));
                        set_pixel(btmb, btmh, CPoint(x, y)+origin, GetRValue(color), GetGValue(color), GetBValue(color));
                    }
                    // 如果颜色与原色不同，则恢复
                    else {
                        set_pixel(btmb, btmh, CPoint(x, y) + origin, before_btmb[index + 2], before_btmb[index + 1], before_btmb[index]);
                    }
                }

                btm->SetBitmapBits(btms, btmb);
            }
        }
    }
    btm->SetBitmapBits(btms, btmb);
    delete[] btmb;
    delete[] before_btmb;
}


enum SeedFillingPointDirection
{
    D_None,
    D_Left,
    D_Up,
    D_Right,
    D_Down
};

struct SeedFillingPointState {
    SeedFillingPointDirection direction;
    CPoint point;
};

/* 四邻域种子填充，很慢 */
void seed_filling(
    CDC* cdc, CPoint seed_point,
    HSLColorLinearInterpolation2D cli2d = HSLColorLinearInterpolation2D(CPoint(0, 0), CPoint(0, 0), vector<ColorInterpolationUnit>())
) {
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);

    vector<SeedFillingPointState> stack;
    stack.reserve(btmh.bmWidth * btmh.bmHeight);
    auto fill_color = cdc->GetPixel(seed_point);
    stack.push_back({ D_None,seed_point });
    while (!stack.empty())
    {
        SeedFillingPointState& state = stack.back();
        CPoint next_point = state.point;
        if (state.direction == D_None) {
            if (stack.size() != 1 && cdc->GetPixel(state.point) != fill_color) {
                stack.pop_back();
            }
            else {
                cdc->SetPixelV(state.point, cli2d.get_color_from_point(state.point));

                next_point.x -= 1;
                stack.push_back({ D_None,next_point });
                state.direction = D_Left;
            }
        }
        else if (state.direction == D_Left) {
            next_point.y += 1;
            stack.push_back({ D_None,next_point });
            state.direction = D_Up;
        }
        else if (state.direction == D_Up) {
            next_point.x += 1;
            stack.push_back({ D_None,next_point });
            state.direction = D_Right;
        } 
        else if (state.direction == D_Right) {
            next_point.y -= 1;
            stack.push_back({ D_None,next_point });
            state.direction = D_Down;
        }
        else if (state.direction == D_Down) {
            stack.pop_back();
        }
    }
}


/* 扫描线种子填充算法*/
void scan_line_seed_filling(
    CDC* cdc, CPoint seed_point,
    HSLColorLinearInterpolation2D cli2d = HSLColorLinearInterpolation2D(CPoint(0, 0), CPoint(0, 0), vector<ColorInterpolationUnit>()),
    bool center_origin = true
) {
    CBitmap* btm = cdc->GetCurrentBitmap();
    BITMAP btmh;
    btm->GetBitmap(&btmh);
    const auto btms = btmh.bmWidthBytes * btmh.bmHeight;
    BYTE* before_btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, before_btmb);

    BYTE* btmb = new BYTE[btms];
    btm->GetBitmapBits(btms, btmb);

    auto fill_color = cdc->GetPixel(seed_point);

    vector<CPoint> stack;
    stack.reserve(btmh.bmHeight* btmh.bmWidth);
    stack.push_back(seed_point);

    CPoint origin(0, 0);
    if (center_origin) {
        origin = CPoint(btmh.bmWidth / 2, btmh.bmHeight / 2);
    }

    while (!stack.empty())
    {
        const CPoint point = stack.back();

        auto index = (btmh.bmHeight - (origin.y + point.y - 11)) * btmh.bmWidthBytes + ((point.x + origin.x) * 4);
        auto color = cli2d.get_color_from_point(CPoint(point.x, point.y));
        btmb[index + 2] = GetRValue(color);
        btmb[index + 1] = GetGValue(color);
        btmb[index] = GetBValue(color);

        long xLeft = 0, xRight = btmh.bmWidth - 1;
        // 找到左边界
        // x 是图像坐标
        for (long x = point.x + origin.x - 1; x >= 0; x--) {
            long index = (btmh.bmHeight - (origin.y + point.y - 11)) * btmh.bmWidthBytes + (x * 4);
            // 向左一直找填充点
            if (RGB(btmb[index + 2], btmb[index + 1], btmb[index]) != fill_color) {
                xLeft = x + 1;
                break;
            }
            else {
                //cdc->SetPixelV(CPoint(x, point.y), cli2d.get_color_from_point(point));
                auto color = cli2d.get_color_from_point(CPoint(x - origin.x, point.y));
                btmb[index + 2] = GetRValue(color);
                btmb[index + 1] = GetGValue(color);
                btmb[index] = GetBValue(color);
            }
        }
        // 找到右边界
        for (int x = point.x + origin.x + 1; x < btmh.bmWidth; x++) {
            auto index = (btmh.bmHeight - (origin.y + point.y - 11)) * btmh.bmWidthBytes + (x * 4);
            // 向右一直找填充点
            if (RGB(btmb[index + 2], btmb[index + 1], btmb[index]) != fill_color) {
                xRight = x - 1;
                break;
            }
            else {
                auto color = cli2d.get_color_from_point(CPoint(x - origin.x, point.y));
                btmb[index + 2] = GetRValue(color);
                btmb[index + 1] = GetGValue(color);
                btmb[index] = GetBValue(color);
            }
        }

        stack.pop_back();

        for (long x = xLeft; x <= xRight; x++) {
            long top_index = (btmh.bmHeight - (origin.y + point.y + 1 - 11)) * btmh.bmWidthBytes + (x * 4);
            long top_right_index = (btmh.bmHeight - (origin.y + point.y + 1 - 11)) * btmh.bmWidthBytes + ((x + 1) * 4);
            if (
                // 防止过顶
                (point.y + origin.y) >= 0
                // 判断是不是要画填充的点
                && RGB(btmb[top_index + 2], btmb[top_index + 1], btmb[top_index]) == fill_color
                // 判断右上角是不是非填充点
                && (RGB(btmb[top_right_index + 2], btmb[top_right_index + 1], btmb[top_right_index]) != fill_color || x == xRight)
                && btmb[top_index + 2] == before_btmb[top_index + 2]
                && btmb[top_index + 1] == before_btmb[top_index + 1]
                && btmb[top_index] == before_btmb[top_index]
                ) {
                // 把上面的点入栈
                stack.push_back(CPoint(x - origin.x, point.y + 1));
            }

            long bottom_index = (btmh.bmHeight - (origin.y + point.y - 1 - 11)) * btmh.bmWidthBytes + (x * 4);
            long bottom_right_index = (btmh.bmHeight - (origin.y + point.y - 1 - 11)) * btmh.bmWidthBytes + ((x + 1) * 4);

            if (
                (point.y + origin.y) < btmh.bmHeight
                && RGB(btmb[bottom_index + 2], btmb[bottom_index + 1], btmb[bottom_index]) == fill_color
                && (RGB(btmb[bottom_right_index + 2], btmb[bottom_right_index + 1], btmb[bottom_right_index]) != fill_color || x == xRight)
                && btmb[bottom_index + 2] == before_btmb[bottom_index + 2]
                && btmb[bottom_index + 1] == before_btmb[bottom_index + 1]
                && btmb[bottom_index] == before_btmb[bottom_index]
                ) {
                stack.push_back(CPoint(x - origin.x, point.y - 1));
            }
        }
    }

    btm->SetBitmapBits(btms, btmb);
    delete[] btmb;
    delete[] before_btmb;
}

void arc(CDC* cdc,CRect rect, double begin, double end)
{
    cdc->Arc(
        &rect,
        CPoint(
            rect.left + rect.Width() / 2 * (cos(3.14 + 6.2831853071796 * end) + 1),
            rect.top + rect.Height() / 2 * (sin(-3.14 + 6.2831853071796 * end) + 1)),
        CPoint(
            rect.left + rect.Width() / 2 * (cos(3.14 + 6.2831853071796 * begin) + 1),
            rect.top + rect.Width() / 2 * (sin(-3.14 + 6.2831853071796 * begin) + 1))
    );
}