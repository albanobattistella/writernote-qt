#include "checksimilecopybook.h"
#include "document.h"
#include <stdio.h>
#include <string.h>


static int checkSpeed(const Document &first,
                      const Document &second){
    int i, counterPage;
    const int lenPage = first.datatouch->lengthPage();
    int len;

    if(lenPage != second.datatouch->lengthPage())
        return LEN;

    for(counterPage = 0; counterPage < lenPage; counterPage ++){
        const page &page1 = first.datatouch->at(counterPage);
        const page &page2 = second.datatouch->at(counterPage);

        len = page1.lengthStroke();
        if(len != page2.lengthStroke())
            return LEN;

        for(i = 0; i < len; i++){
            if(!stroke::cmp(page1.atStroke(i), page2.atStroke(i)))
                return IDTRATTO;
        }
    }

    return OK_CHECK;
}

/*static int checkSlow(const Document &first,
                     const Document &second){
    uint i, counterPage;
    const page *page1, *page2;
    const uint lenPage = first.datatouch->lengthPage();

    if(lenPage != second.datatouch->lengthPage())
        return LEN;

    for(counterPage = 0; counterPage < lenPage; counterPage ++){
        page1 = &first.datatouch->at(counterPage);
        page2 = &second.datatouch->at(counterPage);

        const uint len = page1->lengthStroke();
        if(len != page2->lengthStroke())
            return LEN;

        for(i=0; i<len; i++){
            const stroke &stroke1 = page1->atStroke(i);
            const stroke &stroke2 = page2->atStroke(i);

            if(stroke1.getId() != stroke2.getId())
                return IDTRATTO;

            if(point1->m_x != point2->m_x)
                return XCHECK;
            if(point1->m_y != point2->m_y)
                return YCHECK;

            if(memcmp(&point1->m_color,
                      &point2->m_color,
                      sizeof(point1->m_color)) != 0)
                return COLORE;

            if(point1->m_posizioneaudio != point2->m_posizioneaudio)
                return AUDIOPOSITION;

            if(point1->m_pressure != point2->m_pressure)
                return PRESSURE;

            if(point1->rotation != point2->rotation)
                return ROTATION;
        }
    }
    return OK_CHECK;
}*/


/*
 * speed = true return checkSpeed, it do a memcpm
 * it's a much faster function if you don't care
 * what the difference is
 */

int checksimilecopybook(const Document &primo,
                        const Document &secondo)
{
    int res;
    if(primo.count_img != secondo.count_img)
        return LEN;
    if(primo.count_pdf != secondo.count_pdf)
        return LEN;

    res = checkSpeed(primo, secondo);

    return res;

}
