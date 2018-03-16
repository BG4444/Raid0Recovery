#include "progresssignaler.h"
#include "signaturedef.h"

SignatureDef::SignatureDef(const uchar *base, const quint64 size):sign({'D','I','C','M'}),
                                                                  sgn(ProgressSignaler::make(size,this)),
                                                                  base(base),
                                                                  size(size)

{
    connect(sgn,&ProgressSignaler::percent,this,&SignatureDef::percent);
}

void SignatureDef::run()
{    
    auto j=sign.begin();

    for(quint64 i=0; i < size; ++i)
    {        
        if(j==sign.end())
        {
            emit found(base+i-sign.size());            
            j=sign.begin();            
        }                                
        
        if(base[i]!=*j)
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
