#pragma once

#include "touch/tools/InsertTools.h"
#include "currenttitle/document.h"

class PenMethod: public InsertTools
{
private:
public:
    PenMethod(std::function<pressure_t (double size)> getSize,
              std::function<int()> getTime,
              std::function<void(const QPointF &point)> objectMove,
              QColor &color,
              QPen &pen);

    int touchBegin(const QPointF& point, double size, Document &doc) final;
    int touchUpdate(const QPointF& point, double size, Document &doc) final;
    int touchEnd(const QPointF& point, Document &doc) final;

    [[nodiscard]] uchar getAlfa() const final;
};

