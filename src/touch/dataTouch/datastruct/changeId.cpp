#include "datastruct.h"
#include "utils/WCommonScript.h"
#include "core/WMutexLocker.h"

// this function is usable only in this .o file
force_inline void datastruct::__changeId(int IndexPoint, Stroke &stroke, Page &page, cbool threadSafe)
{
    int lenPointInStroke;

    W_ASSERT(stroke.makeNormal() == nullptr);

    StrokeNormal *strokeNormal = dynamic_cast<StrokeNormal *>(&stroke);

    if (threadSafe) {
        _changeIdMutex.lock();

        // draw to old stroke with color_null
        page.drawForceColorStroke(stroke, -1, COLOR_NULL, nullptr);

        _changeIdMutex.unlock();
    }

    StrokeNormal *strokeToAppend = stroke.split(IndexPoint);

    lenPointInStroke = strokeNormal.length();
    strokeToAppend->reset();

    for (   auto from = IndexPoint, to = lenPointInStroke;
            from < to;
            from ++) {
        strokeToAppend.append(  stroke._point.at(from),
                                stroke.getPressure(from));
    }

    strokeToAppend.setMetadata(stroke.getMetadata());

    if(threadSafe){
        _changeIdMutex.lock();
    }

    // we remove the point
    __stroke.removeAt(IndexPoint, __stroke.length() - 1);

    // we draw the new 2 stroke
    page.drawForceColorStroke(__stroke, -1, __stroke.getColor(1.0), nullptr);
    page.drawForceColorStroke(strokeToAppend, -1, strokeToAppend.getColor(1.0), nullptr);

    // we append the stroke
    page.append(strokeToAppend);

    if(threadSafe){
        _changeIdMutex.unlock();
    }
}

void datastruct::changeIdThreadSave(int indexPoint, Stroke &stroke, Page &page)
{
    return __changeId(indexPoint, stroke, page, true);
}
