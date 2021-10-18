// 0.1.1

#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <afxwin.h>
#include <vector>
#include <sstream>

using namespace std;

#define __MFCTool_stack_like_structure_statement_generator(classname, name) void push##name (classname value); classname pop##name##(); void clear##name##(); classname& get##name();

#define __MFCTools_stack_like_structure_push_generator(classname, name) inline void MFCTool::Manager::push##name(classname value) { name.push_back(value); }
#define __MFCTools_stack_like_structure_get_generator(classname, name) inline classname& MFCTool::Manager::get##name() { return name.back(); }

#define __MFCTools_stack_like_structure_push_function_header_generator(classname, name) inline void MFCTool::Manager::push##name(classname value)
#define __MFCTools_stack_like_structure_pop_function_header_generator(classname, name) inline classname MFCTool::Manager::pop##name()
#define __MFCTools_stack_like_structure_clear_function_header_generator(name) inline void MFCTool::Manager::clear##name()

namespace MFCTool
{
	enum class PenStyle {
		/* 实线
		示例：_____  */
		Solid = PS_SOLID,
		/* 虚线
		示例：_ _ _ _
		当画笔宽度大于1时不能被使用*/
		Dash = PS_DASH,
		/* 点线
		示例：. . . .
		当画笔宽度大于1时不能被使用*/
		Dot = PS_DOT,
		/* 点划线
		示例：. ___ . ___
		当画笔宽度大于1时不能被使用*/
		DashDot = PS_DASHDOT,
		/* 双点划线
		示例：. . __ . . __
		当画笔宽度大于1时不能被使用*/
		DashDotDot = PS_DASHDOTDOT,
		/* 不可见线，不会被绘制 */
		Null = PS_NULL,
		/* 内框架线
		该画笔在封闭形状的框架内画线，此输出函数指定一个限定矩形（例如，Ellipse，Rectangle，RoundRect，Pie和Chord成员函数），
		当此样式用于没有指定限定矩形的 Windows GDI 输出函数（例如 LineTo 成员函数）时，此画笔的绘制区域不受框架的限制。*/
		InsideFrame = PS_INSIDEFRAME,
	};

	enum class HatchStyle {
		/* 45°上升阴影线 
		示例：//// */
		BDiagonal = HS_BDIAGONAL,
		/* 水平和垂直交叉阴影线 
		示例：++++ */
		Cross = HS_CROSS,
		/* 45°十字交叉阴影线
		示例：xxxx */
		DiagCross = HS_DIAGCROSS,
		/* 45°下降阴影线
		示例：\\\\ */
		FDiagonal = HS_FDIAGONAL,
		/* 水平阴影线
		示例：____ */
		Horizontal = HS_HORIZONTAL,
		/* 垂直阴影线
		示例：|||| */
		Vectical = HS_VERTICAL,
	};

	enum class StockPenType {
		/* 黑色 */
		Black = BLACK_PEN,
		/* 透明 */
		Null = NULL_PEN,
		/* 白色 */
		White = WHITE_PEN,
	};

	enum class StockBrushType {
		/* 黑色 */
		Black = BLACK_PEN,
		/* 暗灰色 */
		DarkGray = DKGRAY_BRUSH,
		/* 灰色 */
		Gray = GRAY_BRUSH,
		/* 空心 */
		Hollow = HOLLOW_BRUSH,
		/* 亮灰色 */
		LightGray = LTGRAY_BRUSH,
		/* 透明 */
		Null = NULL_BRUSH,
		/* 白色 */
		White = WHITE_BRUSH,
	};

	class Manager;
	class Builder;
	class Vector2D;
	class Line2D;
};

class MFCTool::Manager {
private:
	CDC* _pdc;
	vector<CPoint> _origin{ CPoint(0,0) };
	vector<CPen*> _pen;
	vector<CBrush*> _brush;
public:
	Manager(CDC* pdc);
	__MFCTool_stack_like_structure_statement_generator(CPoint, _origin);
	__MFCTool_stack_like_structure_statement_generator(CPen*, _pen);
	__MFCTool_stack_like_structure_statement_generator(CBrush*, _brush);

	/* 推入已存在的画笔 */
	void push_stock_pen(StockPenType stock_pen_type);
	/* 推入已存在的刷子 */
	void push_stock_brush(StockBrushType stock_brush_type);

	template<typename... Ts>
	void printf(CPoint point, char* _string, size_t buffer_size, Ts... arg_list);
	template<typename... Ts>
	void print(Ts... arg_list);

	void draw(CRect rect);
	inline BOOL draw(CPoint point, COLORREF color);
	void draw(CPoint point1, CPoint point2);
	void draw(CPoint point, Vector2D vector);
	BOOL draw(CString &_string, CPoint point);
	BOOL draw(LPCTSTR _string, CPoint point);
	
};


class MFCTool::Builder {
private:
	Builder() {};
	Builder(const Builder& builder) {};
public:
	static CRect create_CRect(int x, int y, int width, int heigth);
};


class MFCTool::Vector2D
{
public:
	double x, y;
public:
	template <typename T, typename U>
	Vector2D(T x, U y);
	template <typename PointLike>
	Vector2D(PointLike& point_like);

	bool operator==(Vector2D value) const;
	bool operator!=(Vector2D value) const;
	Vector2D operator+(Vector2D value) const;
	Vector2D operator-(Vector2D value) const;
	void operator+=(Vector2D value);
	void operator-=(Vector2D value);
	Vector2D operator*(double value);
	Vector2D operator/(double value);
	void operator*=(double value);
	void operator/=(double value);

	/* 点乘 */
	double dot(Vector2D value);
	/* 求向量与x轴的夹角 */
	double angle();
	/* 求向量之间的夹角 */
	double angleWith(Vector2D value);
	/* 叉乘 */
	double cross_product(Vector2D value);
	/* 求模 */
	double module();

	CPoint to_CPoint();
};

MFCTool::Manager::Manager(CDC* pdc) : _pdc(pdc) 
{
	_pen.push_back(_pdc->GetCurrentPen());
	_brush.push_back(_pdc->GetCurrentBrush());
}

// @MFCTool::Manager::_origin

__MFCTools_stack_like_structure_push_generator(CPoint, _origin)
__MFCTools_stack_like_structure_get_generator(CPoint, _origin)
__MFCTools_stack_like_structure_pop_function_header_generator(CPoint, _origin) 
{
	CPoint result = _origin.back();
	if (_origin.size() != 0)
		_origin.pop_back();
	return result;
}
__MFCTools_stack_like_structure_clear_function_header_generator(_origin)
{
	_origin.clear();
	_origin.push_back(CPoint(0, 0));
}

// @MFCTool::Manager::_pen

__MFCTools_stack_like_structure_push_function_header_generator(CPen*, _pen) {
	_pdc->SelectObject(value);
	_pen.push_back(value);
}
__MFCTools_stack_like_structure_get_generator(CPen*, _pen)
__MFCTools_stack_like_structure_pop_function_header_generator(CPen*, _pen)
{
	CPen* result = _pen.back();
	if (_pen.size() != 0) {
		_pen.pop_back();
		_pdc->SelectObject(_pen.back());
	}
	return result;
}
__MFCTools_stack_like_structure_clear_function_header_generator(_pen)
{
	CPen* front = _pen.front();
	_pen.clear();
	_pen.push_back(front);
	_pdc->SelectObject(_pen.back());
}

// @MFCTool::Manager::_brush

__MFCTools_stack_like_structure_push_function_header_generator(CBrush*, _brush) {
	_pdc->SelectObject(value);
	_brush.push_back(value);
}
__MFCTools_stack_like_structure_get_generator(CBrush*, _brush)
__MFCTools_stack_like_structure_pop_function_header_generator(CBrush*, _brush)
{
	CBrush* result = _brush.back();
	if (_brush.size() != 0) {
		_brush.pop_back();
		_pdc->SelectObject(_brush.back());
	}
	return result;
}
__MFCTools_stack_like_structure_clear_function_header_generator(_brush)
{
	CBrush* front = _brush.front();
	_brush.clear();
	_brush.push_back(front);
	_pdc->SelectObject(_brush.back());
}

inline void MFCTool::Manager::push_stock_pen(StockPenType stock_pen_type)
{
	_pdc->SelectStockObject(static_cast<int>(stock_pen_type));
	_pen.push_back(_pdc->GetCurrentPen());
}

inline void MFCTool::Manager::push_stock_brush(StockBrushType stock_brush_type)
{
	_pdc->SelectStockObject(static_cast<int>(stock_brush_type));
	_brush.push_back(_pdc->GetCurrentBrush());
}

template<typename ...Ts>
inline void MFCTool::Manager::printf(CPoint point, char* _string, size_t buffer_size, Ts ...arg_list)
{
	char* buffer = new char[buffer_size];
	snprintf(buffer, buffer_size > 1 ? buffer_size - 1 : 1, _string, arg_list...);
	draw(CString(buffer), point);
}


inline void MFCTool::Manager::draw(CRect rect)
{
	_pdc->Rectangle(rect.left + get_origin().x, rect.top + get_origin().x, rect.right + get_origin().y, rect.bottom + get_origin().y);
}
inline BOOL MFCTool::Manager::draw(CPoint point, COLORREF color = RGB(0,0,0))
{
	return _pdc->SetPixelV(point.x + get_origin().x, point.y + get_origin().y, color);
}
inline void MFCTool::Manager::draw(CPoint point1, CPoint point2)
{
	_pdc->MoveTo(point1 + get_origin());
	_pdc->LineTo(point2 + get_origin());
}
inline void MFCTool::Manager::draw(CPoint point, Vector2D vector)
{
	_pdc->MoveTo(point + get_origin());
	_pdc->LineTo(point.x + get_origin().x + static_cast<double>(vector.x), point.y + get_origin().y + static_cast<double>(vector.y));
}

inline BOOL MFCTool::Manager::draw(CString &_string, CPoint point)
{
	return _pdc->TextOut(point.x,point.y, _string);
}

inline BOOL MFCTool::Manager::draw(LPCTSTR _string, CPoint point)
{
	return _pdc->TextOut(point.x, point.y, _string, wcslen(_string));
}


// @MFCTool::Builder::

CRect MFCTool::Builder::create_CRect(int x, int y, int width, int heigth)
{
	return CRect(x, y, x + width, y + heigth);
}


// @MFCTool::Vector2D

template <typename T, typename U>
MFCTool::Vector2D::Vector2D(T x, U y) :
	x(static_cast<double>(x)),
	y(static_cast<double>(y))
{}
template <typename PointLike>
MFCTool::Vector2D::Vector2D(PointLike& point_like) :
	x(static_cast<double>(point_like.x)),
	y(static_cast<double>(point_like.y))
{}
bool MFCTool::Vector2D::operator==(Vector2D value) const {
	return x == value.x && y == value.y;
}
bool MFCTool::Vector2D::operator!=(Vector2D value) const {
	return x != value.x || y != value.y;
}
MFCTool::Vector2D MFCTool::Vector2D::operator+(Vector2D value) const {
	return Vector2D(x + value.x, y + value.y);
}
MFCTool::Vector2D MFCTool::Vector2D::operator-(Vector2D value) const {
	return Vector2D(x - value.x, y - value.y);
}
void MFCTool::Vector2D::operator+=(MFCTool::Vector2D value) {
	x += value.x;
	y += value.y;
}
void MFCTool::Vector2D::operator-=(MFCTool::Vector2D value) {
	x -= value.x;
	y -= value.y;
}
inline MFCTool::Vector2D MFCTool::Vector2D::operator*(double value)
{
	return Vector2D(x * value, y * value);
}
inline MFCTool::Vector2D MFCTool::Vector2D::operator/(double value)
{
	return Vector2D(x / value, y / value);
}
inline void MFCTool::Vector2D::operator*=(double value)
{
	x *= value;
	y *= value;
}
inline void MFCTool::Vector2D::operator/=(double value)
{
	x /= value;
	y /= value;
}
double MFCTool::Vector2D::dot(MFCTool::Vector2D value) {
	return x * value.x + y * value.y;
}
double MFCTool::Vector2D::angle() {
	return atan2(y, x);
}
double MFCTool::Vector2D::angleWith(MFCTool::Vector2D value) {
	return acos(dot(value) / (module() * value.module()));
}
double MFCTool::Vector2D::cross_product(MFCTool::Vector2D value) {
	return module() * value.module() * sin(angle());
}
double MFCTool::Vector2D::module() {
	return pow(x, 2) + pow(y, 2);
}
CPoint MFCTool::Vector2D::to_CPoint() {
	return CPoint(static_cast<int>(this->x), static_cast<int>(this->y));
}

