#pragma once

#include "utils/WCommonScript.h"
#include <QList>
#include <QDebug>
#include <QRect>
#include <QPainterPath>
#include "touch/dataTouch/point.h"
#include "utils/common_def.h"
#include "touch/dataTouch/datastruct/utils_datastruct.h"
#include "core/WLine.h"
#include "stroke_complex_data.h"
#include "touch/object_finder/model/model.h"
#include "testing/memtest.h"
#include "core/WZipWriterSingle.h"
#include "core/WZipReaderSingle.h"
#include "core/WImage.h"

constexpr uchar current_ver = 0;

struct metadata_stroke{
    int posizione_audio;
    struct colore_s color;

    bool operator!=(const metadata_stroke &other) const;
    bool operator==(const metadata_stroke &other) const;

    metadata_stroke &operator=(const metadata_stroke &other)
    {
        this->color = other.color;
        this->posizione_audio = other.posizione_audio;
        return *this;
    }
};

inline bool metadata_stroke::operator!=(const metadata_stroke &other) const
{
    return !(*this == other);
}

inline bool metadata_stroke::operator==(const metadata_stroke &other) const
{
    return  this->posizione_audio == other.posizione_audio and
            this->color == other.color;
}

class Stroke
{
private:
    struct metadata_stroke _metadata;

    QRect _biggerData;

    enum flag_status : unsigned char{
        UPDATE_BIGGER_DATA = BIT(1),
        UPDATE_PRESSURE = BIT(2)
    };

    unsigned char _flag;

    [[nodiscard]] bool isPressureVal() const;
    [[nodiscard]] bool needToUpdatePressure() const;

    void setFlag(unsigned char type, bool value) const;

    static_assert(sizeof(_flag) * 8 >= 4 );

    void updateFlagPressure() const;

    void reset_flag();
public:
    virtual ~Stroke() = default;

    virtual void draw(QPainter &painter, cbool is_rubber, cint page, QPen &pen, cdouble prop) const = 0;
    virtual int is_inside(const WLine &rect, int from, int precision, cbool needToDeletePoint) const = 0;

    QColor getColor(double division = 1.) const;

#   define stroke_append_default (-1.)
    virtual void append(const point_s &point, pressure_t pressure) = 0;

    void setMetadata(int posizione_audio, const colore_s &color);
    void setMetadata(const metadata_stroke &metadata);
    void setPositioneAudio(int m_pos_ris);
    virtual size_t createControll() const;

    int getPosizioneAudio() const;
    virtual QRect getBiggerPointInStroke() const = 0;
    virtual bool isInside(const QRectF &rect) const = 0;

    void clearAudio();

    const struct metadata_stroke &getMetadata() const;

    bool is_highlighter() const;
    uchar get_alfa() const;
    virtual size_t getSizeInMemory() const = 0;

    virtual size_t getSizeInFile() const;
    virtual void decreasePrecision() = 0;
    void setAlfaColor(uchar alfa);

    /** instance of *this == StrokeNormal ==> @return == NULL*/
    virtual Stroke *makeNormal() const = 0;

    void setColor(const colore_s &color);
    /* this function physically adds the x and y value of the point to all of its points. */
    virtual void movePoint(const QPointF &translation) = 0;

    virtual Stroke *clone() const;

    virtual bool isEmpty() const = 0;

    virtual void scale(const QPointF &offset) = 0;

    virtual void adjust(double zoom) = 0;

    static bool cmp(const Stroke &stroke1, const Stroke &stroke2);
    static void copy(const Stroke &src, Stroke &dest);

    friend class Page;
    friend class xmlstruct;
    friend class stroke_drawer;
    friend class stroke_file;
    friend class page_file;
    friend void stroke_complex_adjust(Stroke *stroke, cdouble zoom);

    virtual bool operator==(const Stroke &other) const;
    virtual bool operator!=(const Stroke &other) const { return !(*this == other); };

#ifdef DEBUGINFO
    friend class page_file;
#endif // DEBUGINFO

    static Stroke* load(WZipReaderSingle &reader, int version, int *ok);
    virtual int save(WZipWriterSingle &file) const;
protected:
    Stroke &operator=(const Stroke &other);

    Stroke();
    Stroke(const metadata_stroke& met);
    void modify();
    void setBiggerData(const QRect &newRect) const;
    [[nodiscard]] bool needToUpdateBiggerData() const;

    virtual void preappend(int l) = 0;

    enum type_stroke_private: int{
            COMPLEX_NORMAL = 0,
            COMPLEX_CIRCLE = 1,
            COMPLEX_RECT = 2,
            COMPLEX_LINE = 3
    };

    friend class StrokePre;

    /** all stroke derivated class needs to implements this method to recognize yourself */
    [[nodiscard]] virtual int type() const = 0;
};

void set_press( QPen &pen, pressure_t press, double prop,
                cbool is_rubber, const QColor &color);

inline Stroke &Stroke::operator=(const Stroke &other)
{
    if(unlikely(this == &other)){
        return *this;
    }

    this->_biggerData = other._biggerData;
    this->_metadata = other._metadata;
    this->_flag = other._flag;
    return *this;
}

inline void Stroke::setBiggerData(const QRect &newRect) const
{
    auto &r = (QRect &) this->_biggerData;
    r = newRect;
    setFlag(UPDATE_BIGGER_DATA, false);
}

inline void Stroke::modify()
{
    _flag = UPDATE_BIGGER_DATA | UPDATE_PRESSURE;

    W_ASSERT(this->needToUpdateBiggerData());
    W_ASSERT(this->needToUpdatePressure());
}

inline bool Stroke::needToUpdateBiggerData() const
{
    return _flag & UPDATE_BIGGER_DATA;
}

inline bool Stroke::needToUpdatePressure() const
{
    return _flag & UPDATE_PRESSURE;
}

inline void Stroke::setFlag(unsigned char type, bool value) const
{
    auto &f = (uchar &) _flag;
    if(value){
        f |= type;
    }else{
        f &= ~type;
    }
}

