#ifndef STROKE_H
#define STROKE_H

#include <QList>
#include <QImage>
#include <QPainterPath>
#include "point.h"
#include "zip.h"
#include "../../utils/color/setcolor.h"

#define VER_STROKE 0

struct metadata_stroke{
    int page;
    int idtratto;
    int posizione_audio;
    struct colore_s color;
};

class stroke
{
private:
    QList<point_s> m_point;

    struct metadata_stroke metadata;
    bool metadataSet: 1;
    int versione = VER_STROKE;

    QPainterPath path;

    QRectF biggerData;
    bool biggerDataSet: 1;
    bool constantPressureVal: 1;

    void updateFlag();

public:
    stroke();

    /* this function is used to set the pressure equal for all points. */
    void __setPressureForAllPoint(const double pressure);

    bool isIdUser() const;

    int save(zip_source_t *file) const;
    int load(zip_file_t *file);

    QColor getColor(const double division) const;

    const point_s   &at(const int index) const;
    point_s         &at_mod(const int index);

    void                append(const point_s &point);
    void                setMetadata(const metadata_stroke & metadata);
    void                setMetadata(const int page, const int idtratto,
                                    const int posizione_audio, const struct colore_s color);

    size_t createControll() const;

    int getId() const;
    int getPage() const;
    int getPosizioneAudio() const;

    QRectF getBiggerPointInStroke();
    bool isInside(const QRectF &rect) const;

    void clearAudio();

    int length() const;
    void changeId(const int newId);
    void commitChange();

    bool constantPressure() const;

    size_t getSizeInMemory() const;
    void decreasePrecision() const;

    void setAlfaColor(const uchar alfa);
    void setColor(const colore_s &color);

    /* this function physically adds the x and y value of the point to all of its points. */
    void movePoint(const QPointF &translation);

    QPainterPath getQPainterPath() const;
    void createQPainterPath();

    void reset();

    /* return true if equals */
    static bool cmp(const stroke &stroke1, const stroke &stroke2);
    static void copy(const stroke &src, stroke &dest);
};

inline void stroke::updateFlag()
{
    int i, len;
    const point_s *point, *point2;

    len = this->length();

    point = &at(0);
    for (i = 0; i < len-1; i++){
        point2 = &at(i+1);
        if(at(i).pressure != point->pressure){
            this->constantPressureVal = false;
            return;
        }
        point = point2;
    }

    this->constantPressureVal = true;
}

inline bool stroke::isIdUser() const
{
    return metadata.idtratto >= 0;
}

inline QColor stroke::getColor(const double division = 1.0) const
{
    return setcolor(this->metadata.color, division);
}

inline const point_s &stroke::at(const int index) const
{
    return this->m_point.at(index);
}

inline point_s &stroke::at_mod(const int index)
{
    return this->m_point.operator[](index);
}

inline void stroke::append(const point_s &point)
{
    this->m_point.append(point);
    this->updateFlag();
}

inline void stroke::setMetadata(const metadata_stroke &metadata)
{
    Q_ASSERT(this->metadataSet == 0);
    memcpy(&this->metadata, &metadata, sizeof(this->metadata));
    metadataSet = 1;
}

// return the "old" idtratto
inline int stroke::getId() const
{
    return metadata.idtratto;
}

inline int stroke::getPage() const
{
    return this->metadata.page;
}

inline int stroke::getPosizioneAudio() const
{
    return this->metadata.posizione_audio;
}

/* after append data we need to call this funcion to update */
inline QRectF stroke::getBiggerPointInStroke()
{
    if(this->biggerDataSet)
        return this->biggerData;

    int i, len = this->length();
    QRectF conf;
    conf.setTopLeft(QPointF(this->at(0).m_x, this->at(0).m_y));
    conf.setBottomRight(conf.topLeft());

    for (i = 0; i < len; i++){
        const point_s &point = at(i);
        if(point.m_x >= conf.x())
            conf.setX(point.m_x);
        if(point.m_y >= conf.y())
            conf.setY(point.m_y);
    }

    biggerDataSet = true;
    biggerData = conf;
    return biggerData;
}

inline void stroke::clearAudio()
{
    this->metadata.posizione_audio = -1;
}

inline int stroke::length() const
{
    return this->m_point.length();
}

inline void stroke::changeId(const int newId)
{
    this->metadata.idtratto = newId;
}

inline bool stroke::constantPressure() const
{
    return this->constantPressureVal;
}

inline void stroke::setAlfaColor(const uchar alfa)
{
    this->metadata.color.colore[3] = alfa;
}

inline void stroke::setColor(const colore_s &color)
{
    memcpy(&this->metadata.color, &color, sizeof(metadata.color));
}

inline QPainterPath stroke::getQPainterPath() const
{
    return this->path;
}

#endif // STROKE_H
