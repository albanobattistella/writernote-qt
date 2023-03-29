#include "StrokeForPage.h"
#include "Stroke.h"
#include "StrokeNormal.h"
#include "utils/common_error_definition.h"
#include "touch/TabletUtils.h"

StrokeForPage::StrokeForPage()
    : _data(new StrokeNormal)
{
    rep();
}

void StrokeForPage::setPressure(pressure_t press)
{
    this->_data->force_pressure(press);
    rep();
}

void StrokeForPage::rep() const
{
    W_ASSERT(this->_data.unique());
}

void StrokeForPage::append(const StrokeNormal &stroke)
{
    using namespace WCommonScript;
    auto &l = this->_data->_point;

    for_each(stroke._point, [&l](const Point &point) {
        l.append(point);
    });
}

int StrokeForPage::load(WZipReaderSingle &reader, int ver_stroke)
{
    int ok;
    auto *tmp =
            dynamic_cast<StrokeNormal*>(
                    Stroke::load(reader, ver_stroke, &ok).get()
            );

    if(ok != OK){

            delete tmp;
        return ok;
    }

    this->_data = std::unique_ptr<StrokeNormal>(tmp);

    if(_data->_pressure[0] > 10)
        _data->_pressure[0] = 1.5;

    return OK;
}

size_t StrokeForPage::getSizeInFile() const
{
    const auto res = this->_data->getSizeInFile();
    rep();
    return res;
}

void StrokeForPage::draw(QPainter &painter, double zoom, double delta,
                         QPen &pen, const QPointF &pointFirstPage,
                         const Page &page) const
{
    int counterPoint, lenPoint;
    pressure_t pressure;

redo:

    lenPoint = this->_data->_point.length();

    if(!lenPoint)
        return;

    pressure = _data->getPressure();

    /**
     * It is due to a bug introduced a long time ago.
    */
    if(un(pressure <= 0.0)){
        if (_data->_point.length()) {
            (pressure_t &)_data->_point[0] = .1f;
        }
        goto redo;
    }

    pressure = TabletUtils::pressureToWidth(pressure * zoom / 2.0) * delta;

    pen.setWidthF(pressure);
    pen.setColor(_data->getColor());

    painter.setPen(pen);

    for(counterPoint = 0; counterPoint < lenPoint; counterPoint += 2){
        const auto ref1 = DataStruct::at_draw_page(counterPoint + 0, page, pointFirstPage, zoom * delta);
        const auto ref2 = DataStruct::at_draw_page(counterPoint + 1, page, pointFirstPage, zoom * delta);

        painter.drawLine(ref1, ref2);
        //painter.drawLine(ref1._x, ref1._y, ref2._x, ref2._y);
    }

    rep();
}

int StrokeForPage::save(WZipWriterSingle &writer) const
{
    rep();
    const auto res = this->_data->save(writer);
    rep();
    return res;
}

void StrokeForPage::scale(const QPointF &delta)
{
    this->_data->scale(delta);
    rep();
}

void StrokeForPage::append(const Point &point, pressure_t pressure)
{
    _data->append(point, pressure);
    rep();
}

void StrokeForPage::reset()
{
    this->_data = std::shared_ptr<StrokeNormal>(new StrokeNormal);
    rep();
}

StrokeForPage &StrokeForPage::operator=(const StrokeForPage &other)
{
    {
        std::shared_ptr<Stroke> res = other._data->clone();
        this->_data = std::dynamic_pointer_cast<StrokeNormal>(res);
    }

    rep();
    return *this;
}

void StrokeForPage::setMetadata(const colore_s &colore)
{
    rep();
    _data->setMetadata(-1, colore);
    rep();
}
