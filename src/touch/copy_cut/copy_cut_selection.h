#ifndef COPY_SELECTION_H
#define COPY_SELECTION_H

#include <QPointF>
#include <QRectF>
#include <QList>

class stroke;
class datastruct;

class copy
{
public:
    copy();
    ~copy();

#define SELECTION_FLAGS_COPY 0
#define SELECTION_FLAGS_CUT 1
#define SELECTION_FLAGS_PASTE 2
    void selection(datastruct &data, const QList<int> &id, int __flags, QList<int> &page_mod);
    void past_selection(datastruct &data, QPointF &point_past);

    bool isEmpty() const;

    bool isSomeThingCopy() const;
    bool isSomeThingCut() const;

    void reset();

private:
    void adjustData(const QRectF &areaData);

#define FLAG_CUT 0x2 /* if the point is from a cut operation */
    int flags = 0;

    QList<stroke> m_stroke;

};

inline bool copy::isSomeThingCopy() const
{
    return !(this->flags & FLAG_CUT);
}

inline bool copy::isSomeThingCut() const
{
    return this->flags & FLAG_CUT;
}

inline bool copy::isEmpty() const
{
    return m_stroke.isEmpty();
}

#endif // COPY_SELECTION_H
