#pragma once

#include "Stroke.h"
#include "StrokeComplexCommon.h"

class StrokeRectGenerator
{
private:
    static constexpr bool StrokeRectGeneratorDebug = true;

    static bool   is_near_rect_x(const RectF &area, const PointF &point);
    static bool   is_near_rect_y(const RectF &area, const PointF &point);
    static double is_near_rect(const RectF &area, const PointF &point);

public:
    static std::shared_ptr<Stroke> make(const class StrokePre *from);
    static double model_near(const class StrokePre &stroke);

};
