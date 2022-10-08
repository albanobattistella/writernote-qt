#pragma once

#include "Stroke.h"
#include "core/WList.h"
#include "core/WImage.h"

class StrokePre: private Stroke, private WImage {
private:
    WList<point_s>      _point;
    WList<pressure_t>   _pressure;

    WList<point_s>      ::const_iterator   _last_draw_point;
    WList<pressure_t>   ::const_iterator   _last_draw_press;

#ifdef DEBUGINFO
    bool already_merge = false;
#endif // DEBUGINFO

    [[nodiscard]] WList<pressure_t>::const_iterator get_last_press() const;
    [[nodiscard]] WList<point_s>::const_iterator get_last_point() const;
    [[nodiscard]] const Stroke &get_stroke_for_draw() const;
public:
    StrokePre() noexcept;
    ~StrokePre() = default;

    void adjust(const QPointF &delta);
    void setAlfaColor(int alfa);

    void setTime(int time);
    void setColor(const QColor &color) noexcept;
    [[nodiscard]] bool isEmpty() const noexcept;
    [[nodiscard]] int length() const;

    [[nodiscard]] bool is_normal() const  { return Stroke::is_normal(); };
    [[nodiscard]] bool is_circle() const  { return Stroke::is_circle(); };
    [[nodiscard]] bool is_rect() const    { return Stroke::is_rect(); };
    [[nodiscard]] bool is_line() const    { return Stroke::is_line(); };

    void draw(QPainter &painter);
    void append(const point_s &point, const pressure_t &press, QPen &pen, cdouble prop);
    [[nodiscard]] QColor getColor(double division = 1.) const;

    Stroke& merge();

    StrokePre &operator=(const StrokePre &other);

    friend class stroke_drawer;
};

inline bool StrokePre::isEmpty() const noexcept
{
    return this->_point.isEmpty();
}

inline int StrokePre::length() const
{
    W_ASSERT(Stroke::is_normal());
    return _point.length();
}

inline void StrokePre::setColor(const QColor &color) noexcept
{
    Stroke::setColor(color);
}

inline const Stroke &StrokePre::get_stroke_for_draw() const
{
    W_ASSERT(Stroke::is_complex());
    return *this;
}

inline WList<pressure_t>::const_iterator StrokePre::get_last_press() const
{
    W_ASSERT(_pressure.length() > 1);
    return _last_draw_press;
}

inline WList<point_s>::const_iterator StrokePre::get_last_point() const
{
    W_ASSERT(this->_point.length() > 1);
    return _last_draw_point;
}

