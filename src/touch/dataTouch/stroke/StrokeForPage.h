#pragma once

#include "core/WImage.h"
#include "core/WZipReaderSingle.h"
#include "core/WZipWriterSingle.h"
#include "touch/dataTouch/datastruct/utils_datastruct.h"
#include "touch/dataTouch/Point.h"
#include <QPainter>
#include <QPen>
#include "core/WList.h"
#include "core/WImage.h"
#include "core/WPixmap.h"

class StrokeForPage {
private:
    std::shared_ptr<class StrokeNormal> _data;

    void rep() const;

    void append(const StrokeNormal &stroke);
    mutable WPixmap _pix;

    mutable bool _needToUpdate = true;
    void draw() const;
public:
    StrokeForPage();
    ~StrokeForPage() = default;
    void setMetadata(const colore_s &colore);
    void append(const Point &point, pressure_t pressure);

    void setPressure(pressure_t press);

    int load(WZipReaderSingle &reader, int ver_stroke);

    size_t getSizeInFile() const;

    void draw(QPainter &painter, double delta,
              const class Page &page, const QSize &target, const QRectF &visibleArea) const;

    int save(WZipWriterSingle& writer) const;
    void scale(const QPointF& delta);
    void reset();

    StrokeForPage &operator=(const StrokeForPage &other);

    friend class page_file;
    friend class Page;
};
