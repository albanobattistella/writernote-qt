#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QList>
#include <QColor>
#include <QImage>

#include "../../images/image_struct.h"

/*
    IDVERTICALE -> linee verticali
    IDORIZZONALE -> linee orizzonali
*/

#define NCOLOR 4

struct colore_s{
    uchar colore[NCOLOR];
};

struct point_s{
    double m_x, m_y, rotation;
    float m_pressure;
    int m_posizioneaudio;
    struct colore_s m_color;
    int idtratto;

    size_t createControll() const;
    bool isIdUser() const;
};

#define IDTRATTOZERO -5
#define IDVERTICALE -2
#define IDORIZZONALE -1

class datastruct
{
private:
    int minId();

    QPointF __last_translation;

public:

    uint move_to_positive(uint len);

    void restoreLastTranslation();
    void controllForRepositioning();

    void removePointId(QList<int> &list);
    void removePointId(int id, unsigned int *len = nullptr);

    bool MovePoint( QRectF &, QPointF);

    bool userWrittenSomething();

    /*
     * return true if the user has written this point
    */
    static inline bool isIdUser(const int id){
        return id != IDTRATTOZERO
                && id != IDVERTICALE
                && id != IDORIZZONALE;
    }
    static inline bool isIdUser(const point_s * __point){
        return isIdUser(__point->idtratto);
    }

    bool isinside(QPointF &topleft, QPointF &bottonright, unsigned int index);
    bool isinside(double x1, double y1, double x2, double y2, unsigned int index);
    static bool isinside(QPointF &topleft, QPointF &bottonright, const point_s *__point);
    static bool isinside(QPointF &topleft, QPointF &bottonright, QPointF &point);


    bool adjustAll(const unsigned int width,
                   const unsigned int height);
    bool adjustWidth(unsigned int width,
                     bool controllRepo);
    bool adjustHeight(unsigned int height,
                      bool controllRepo);

    /*
     * this function return the index of the first
     * point with idtratto == IDORIZZONALE
    */
    unsigned int posIdOrizzonal();


    datastruct();

    void moveNextPoint(uint *pos,
                       uint len = 0,
                       int id = -6);

    void reorganize();

    bool maxXIdOrizzonal(double *val);
    bool minXIdOrizzonal(double *val);

    void changeId(unsigned int i, int base = -1);

    bool isAvailable(int id);
    int maxId();

    inline bool isempty(){
        return this->m_point.isEmpty();
    };

    static void inverso(QPointF &point);
    bool repositioning();

    void scala_all(const QPointF &);
    void scala_all();

    void scala_x(double scala);
    void scala_y(double scala);
    void scala_posizionefoglio(double scala);

    double biggerxNoId();
    void reset();

    double biggerynoid();
    double biggerx();
    void removeat(int i);

    bool needtocreatenew();
    bool needtochangeid(const unsigned int);

    double biggery();

    /* the function return the index of the id*/
    unsigned int positionId(int id);

    unsigned decreaseAlfa(int id,
                          uchar decrese,
                          unsigned int index);

    uchar removePage(uint page);

    double miny();
    double minx();

    QList<double> posizionefoglio;

    QList<struct point_s> m_point;

    long double zoom = 1.00;

    inline uint length(){
        return m_point.length();
    }

    /*
     * this function don't provent buffer overload
    */
    inline const point_s * firstPoint(){
        return at(0);
    }

    inline const point_s * at(uint i){
        return & m_point.at(i);
    }

};

/*
 * return true if the user
 * has written this point
*/
inline bool point_s::isIdUser() const
{
    return datastruct::isIdUser(this);
}

#endif // DATASTRUCT_H
