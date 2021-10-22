#include "datastruct.h"
#include <QList>
#include <QDebug>
#include "../../sheet/fast-sheet/fast_sheet_ui.h"

#define mov_if_neg(p, x) \
    p = x; \
    return;

void datastruct::moveIfNegative(uint &p, uint &page, const uint lenPage,
                                const uint height,
                                const uint width) const{
    static uint r, lenPoint;
    static const point_s *f, *s;

    r = p;
    for(; page<lenPage; page++){
        lenPoint = at(page)->length();
        if(!at(page)->isVisible())
            continue;

        for(; p<lenPoint-1; ++p){

            f = &at_draw(p, page);
            s = &at_draw(p+1, page);

            if(!datastruct::isIdUser(f))
                return;

            if((f->m_y >= 0.0 && f->m_x >= 0.0) &&
                    (f->m_y <= height && f->m_x <= width)){
                mov_if_neg(p, r);
                p = r;
                return;
            }

            if(s->idtratto == f->idtratto
                    && std::abs((f->m_y + s->m_y)/2) < height
                    && std::abs((f->m_x + s->m_x)/2) < width){
                p = r;
                return;
            }

            if(f->idtratto != s->idtratto){
                r = p;
            }
        }
    }
}

datastruct::datastruct(frompdf *m_pdf, fromimage *m_img)
{
    this->m_pdf = m_pdf;
    this->m_img = m_img;
    __last_translation.setX(0);
    __last_translation.setY(0);
}

/* the function returns true if the id is available */
bool datastruct::isAvailable(int id)
{
    int i, len;

    for(const page &page : this->m_page){
        len = page.length();
        for(i=0; i<len; i++){
            if(page.at(i)->idtratto == id)
                return 0;
        }
    }

    return 1;
}

void datastruct::reset(){
    this->m_page.clear();
    pointFirstPage = QPointF(0, 0);
    zoom = 1.00;
}

uint datastruct::positionId(int id)
{
    uint i, k, len;
    const uint lenPage = this->lengthPage();

    for(k=0; k<lenPage; k++){
        len = at(k)->length();

        for(i=0; i<len; i++){
            if(at(i, k)->idtratto == id)
                return i;
        }
    }

    return 0;
}

uint datastruct::decreaseAlfa(const int id,
                              const uchar decrease,
                              const uint lenPage)
{
    uint i, k, len;
    point_s *__point;

    for(k=0; k<lenPage; k++){
        len = this->m_page.at(k).length();
        for(i=0; i<len; ++i){
            __point = at_mod(i, k);

            if(__point->idtratto == id){
                __point->m_color.colore[3] /= decrease;
            }
        }
    }

    return i;
}

void datastruct::copy(const datastruct &src, datastruct &dest)
{
    uint i;
    const uint len = dest.lengthPage();
    uint diff = len - dest.lengthPage();

    if(diff > 0){
        for(i=0; i<diff; ++i){
            dest.newPage(n_style::white);
        }
    }
    else if(diff != 0){
        diff = -diff;
        for(i=diff; i>0; i--)
            dest.removePage(i);
    }

    for(i=0; i<len; ++i){
        page::copy(src.m_page.at(i), dest.m_page.operator[](i));
    }

    dest.zoom = src.zoom;

    dest.__last_translation = src.__last_translation;
}

void datastruct::removeat(const uint index, const uint page){
    this->m_page.operator[](page).removeAt(index);
}

/*
 * the function return true if the point in position i
 * is in the middle of a stretch
 *
 * return false if it's the first point or the lastone
 */
bool datastruct::needtochangeid(const uint index, const uint indexPage){
    const page *page;
    const uint lenPointForPage = at(indexPage)->length();

    if(!index || (index+1) >= lenPointForPage)
        return false;

    page = at(indexPage);
    int temp_id = page->at(index)->idtratto;

    return (temp_id == page->at(index-1)->idtratto
            && temp_id == page->at(index+1)->idtratto);

}
