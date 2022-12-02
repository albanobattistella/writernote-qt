#pragma once

#include "Stroke.h"

class StrokeLine final : public Stroke
{
private:
    QPointF _pt1, _pt2;
    pressure_t _press;

    void makeNormalVertical(class StrokeNormal *mergeTo, int from, int to) const;
    void makeNormalGeneric(class StrokeNormal *mergeTo, int from, int to) const;

    void makeGeneric(const StrokeNormal &s);
    void makeVertical(const StrokeNormal &s);

    int load(WZipReaderSingle &reader);

    static constexpr bool debugLine = false;

public:
    StrokeLine();

    void draw(QPainter &painter, cbool is_rubber, cint page, QPen &pen, cdouble prop) const;
    int is_inside(const WLine &rect, int from, int precision, cbool needToDeletePoint) const;
    bool is_inside(const QRectF &rect, double precision) const;

    QColor getColor(double division) const;

#   define stroke_append_default (-1.)
    void append(const point_s &point, pressure_t pressure);
    void setMetadata(int posizione_audio, const colore_s &color);
    void setMetadata(const metadata_stroke &metadata);
    void setPositioneAudio(int m_pos_ris);
    [[nodiscard]] virtual size_t createControll() const;

    int getPosizioneAudio() const;
    QRect getBiggerPointInStroke() const;
    bool isInside(const QRectF &rect) const;

    void clearAudio();

    bool is_highlighter() const;
    int save(WZipWriterSingle &file) const final;

    size_t getSizeInMemory() const;
    size_t getSizeInFile() const final;

    void decreasePrecision();
    void setAlfaColor(uchar alfa);

    void setColor(const colore_s &color);
    /* this function physically adds the x and y value of the point to all of its points. */
    void movePoint(const QPointF &translation);

    void adjust(double zoom);
    Stroke *clone() const;

    Stroke* makeNormal() const;

    [[nodiscard]] bool isEmpty() const;

    void scale(const QPointF &offset);

    void preappend(int) final {};

    bool operator==(const Stroke &other) const final;
    bool operator!=(const Stroke &other) const final;

    static bool cmp(const Stroke &stroke1, const Stroke &stroke2);
    static void copy(const Stroke &src, Stroke &dest);

    static StrokeLine *make(const QPointF& pt1, const QPointF& pt2, const StrokeNormal &s);
    int type() const final;

    friend class Stroke;
};

