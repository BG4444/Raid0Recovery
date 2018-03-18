#include "xfsdetector.h"


XFSDetector::XFSDetector(const uchar *base, const quint64 size):sign({'X','F','S','B'}),
    base(base),
    size(size),
    sgn(ProgressSignaler::make(size,this))
{
    connect(sgn,&ProgressSignaler::percent, this, &XFSDetector::percent);


}


void XFSDetector::run()
{
    auto j=sign.begin();

    for(quint64 i=0; i < size; ++i)
    {
        if(j==sign.end())
        {
            emit found(base+i-sign.size());
            j=sign.begin();
        }

        if(base[i] != *j)
        {
            if(j!=sign.begin())
            {
                j=sign.begin();
            }
        }
        else
        {
            ++j;
        }
        sgn->tick();
    }
}
