#ifndef POINT_H
#define POINT_H

#include <QtGlobal>
#include <QColor>
#include <QPointF>
#include "utils/WCommonScript.h"

#define NCOLOR 4

template <class T>
class Settable: public T {
private:
    bool _set;
public:
    Settable ()
        : T()
        , _set(false)
    {
    }

    Settable<T>& operator=(const Settable<T> &other) {
        T::operator=(other);
        _set = other._set;

        return *this;
    }

    Settable<T>& operator=(const T& other) {
        T::operator=(other);
        return *this;
    }

    Settable<T>& operator=(bool set) {
        this->_set = set;
        return *this;
    }
};

class PointSettable : public QPointF {
private:
    bool _set;
public:
    explicit PointSettable(const QPointF &point, bool set);
    explicit PointSettable(double x, double y, bool set);
    PointSettable();

    [[nodiscard]] bool isSet() const noexcept;
    void setSet(bool set);
    PointSettable &operator=(const QPointF &point) noexcept;
    PointSettable &operator=(const PointSettable &point) noexcept;
    PointSettable &operator=(bool enable) noexcept;
};

force_inline PointSettable::PointSettable()
{
    _set = false;
}

inline PointSettable::PointSettable(const QPointF &point, bool set):
    QPointF(point)
{
    this->_set = set;
    W_ASSERT(this->isSet() == set);
}

inline PointSettable::PointSettable(double x, double y, bool set)
    : QPointF(x, y)
    , _set(set)
{
}

inline void PointSettable::setSet(bool set)
{
    _set = set;
    W_ASSERT(this->isSet() == set);
}

inline bool PointSettable::isSet() const noexcept
{
    return _set;
}

inline PointSettable &PointSettable::operator=(const QPointF &point) noexcept
{
    QPointF::operator=(point);
    return *this;
}

inline PointSettable &PointSettable::operator=(const PointSettable &point) noexcept
{
    QPointF::operator=(point);
    this->_set = point._set;
    return *this;
}

inline PointSettable &PointSettable::operator=(bool enable) noexcept
{
    this->_set = enable;
    return *this;
}

struct colore_s{
    colore_s() = default;
    ~colore_s() = default;
    colore_s(const QColor &color);
    colore_s(uchar u1, uchar u2, uchar u3, uchar u4);

    void set_alfa(uchar alfa);

    uchar colore[NCOLOR];
    [[nodiscard]] QColor toQColor(double division) const;
    void fromColor(const QColor &color);

    double getAlfa() const;
    double getRed() const    { return this->colore[0]; };
    double getGreen() const  { return this->colore[1]; };
    double getBlue() const   { return this->colore[2]; };

    static colore_s from_color(const QColor &color);
    bool operator==(const colore_s &other) const;
};

inline void colore_s::set_alfa(uchar alfa)
{
    this->colore[3] = alfa;
}

/**
 * if division == 1 the color don't change
 * if division > 0 the color the alfa is change
*/
force_inline QColor colore_s::toQColor(cdouble division = 1.0) const
{
    return QColor::fromRgb( colore[0], colore[1], colore[2], double(colore[3])/division);
}

force_inline void colore_s::fromColor(const QColor &color)
{
    int val[NCOLOR];
    uchar i;

    color.getRgb(&val[0], &val[1], &val[2], &val[3]);

    for(i = 0; i < NCOLOR; i ++){
        colore[i] = val[i];
    }
}

inline double colore_s::getAlfa() const
{
    return this->colore[3];
}

inline colore_s colore_s::from_color(const QColor &color)
{
    colore_s tmp;
    tmp.fromColor(color);
    return tmp;
}

inline bool colore_s::operator==(const colore_s &other) const
{
    static_assert(sizeof(colore) == sizeof(uchar) * 4);
    return memcmp(this->colore, other.colore, sizeof(this->colore)) == 0;
}

/* this struct contains neither the color, nor the thickness, nor the page to which it belongs, nor the rotation, nor the id */
class Point : public QPointF{
public:
    constexpr Point() = default;
    constexpr Point(double x, double y);
    constexpr Point(const QPointF &point);

    constexpr QPointF toQPointF(double scale) const;
};

inline constexpr Point::Point(double x, double y) :
    QPointF(x, y)
{
}

force_inline constexpr Point::Point(const QPointF &point) :
    QPointF(point)
{
}

inline constexpr QPointF Point::toQPointF(double scale) const
{
    return (*this) * scale;
}

inline colore_s::colore_s(const QColor &color)
{
    *this = colore_s::from_color(color);
}

inline colore_s::colore_s(uchar u1, uchar u2, uchar u3, uchar u4)
{
    colore[0] = u1;
    colore[1] = u2;
    colore[2] = u3;
    colore[3] = u4;
}


#endif // POINT_H
