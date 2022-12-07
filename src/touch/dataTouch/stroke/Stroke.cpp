#include "Stroke.h"
#include "touch/tabletcanvas.h"
#include "StrokeNormal.h"
#include "StrokeLine.h"
#include "StrokeCircle.h"
#include "StrokeRect.h"

Stroke *Stroke::load_ver_1(WZipReaderSingle &reader, int *ok)
{
    auto *s = new StrokeNormal();
    int len_point;

    if(ok) *ok = OK;

    if(reader.read_object(len_point) < 0)
        return ERROR;
    if(reader.read_object(s->_metadata) < 0)
        return ERROR;

    const auto res = s->load_ver_1(reader, len_point);

    if(res != OK and ok)
        *ok = res;

    return s;
}

Stroke* Stroke::load_ver_2(WZipReaderSingle &reader, int *ok)
{
#define ver_2_manage_error(contr)   \
    do {                            \
        if(contr)                   \
            *contr = ERROR;         \
        return nullptr;             \
    } while(0);


    int type;
    metadata_stroke meta;

    static_assert(sizeof(_metadata) == 8);

    if(reader.read_by_size(&meta, sizeof(meta)) < 0)
        ver_2_manage_error(ok);
    if(reader.read_object(type) < 0)
        ver_2_manage_error(ok);

    switch (type) {
    case COMPLEX_NORMAL:
    {
        auto *tmp = new StrokeNormal();
        tmp->load_ver_2(reader);
        return tmp;
    }
    case COMPLEX_LINE:
    {
        auto *tmp = new StrokeLine();
        tmp->load(reader);
        return tmp;
    }
    case COMPLEX_CIRCLE:
    {
        auto *tmp = new StrokeCircle();
        tmp->load(reader);
        return tmp;
    }
    case COMPLEX_RECT:
    {
        auto *tmp = new StrokeRect();
        tmp->load(reader);
        return tmp;
    }
    }

    if(ok)
        *ok = ERROR;
    return nullptr;
}

Stroke* Stroke::load(WZipReaderSingle &reader, int version_stroke, int *ok)
{
    switch (version_stroke) {
        case 0:
            if(ok)
                *ok = ERROR_VERSION;
            return nullptr;
        case 1:
            return Stroke::load_ver_1(reader, ok);
        case 2:
            return Stroke::load_ver_2(reader, ok);
    default:
        break;
    }

    if(ok)
        *ok = ERROR_VERSION_NEW;
    return nullptr;
}

void Stroke::reset_flag()
{
    _flag = UPDATE_BIGGER_DATA;
    W_ASSERT(this->needToUpdateBiggerData() == true);
}

size_t Stroke::getSizeInFile() const
{
    size_t s = 0;
    s += sizeof(this->_metadata);
    s += sizeof(int); // prop

    static_assert(sizeof(type()) == sizeof(int));

    return s;
}

bool Stroke::operator==(const Stroke &other) const
{
    if(this->_metadata != other._metadata)
        return false;
    return true;
}

void set_press(
                            QPen &pen,
                            const pressure_t press,
                            const double prop,
                            cbool is_rubber,
                            const QColor &color)
{
    pen.setWidth(TabletCanvas::pressureToWidth(press / deltaPress) * prop);
    if (unlikely(is_rubber)) {
        const auto _press = pen.widthF() * deltaColorNull;
        pen.setWidthF(_press);
    } else {
        pen.setColor(color);
    }
}

int Stroke::save(WZipWriterSingle &file) const
{
    const auto t = type();
    file.write_object(_metadata);
    file.write_object(t);

    static_assert(sizeof(t) == sizeof(int));
    return OK;
}

void Stroke::setAlfaColor(uchar alfa)
{
    _metadata.color.set_alfa(alfa);
}

QColor Stroke::getColor(double division) const
{
    return this->_metadata.color.toQColor(division);
}

void Stroke::setMetadata(int posizione_audio, const colore_s &color)
{
    this->_metadata = {
        .posizione_audio = posizione_audio,
        .color = color
    };
}

size_t Stroke::createControll() const
{
    size_t controll = 0;
    int i;

    for(i = 0; i < NCOLOR; i++){
        controll += _metadata.color.colore[i];
    }

    return controll;
}

void Stroke::clearAudio()
{
    this->_metadata.posizione_audio = -1;
}

Stroke::Stroke()
{
    Stroke::reset_flag();
}

Stroke::Stroke(const metadata_stroke &met)
{
    Stroke::reset_flag();
    this->_metadata = met;
}
