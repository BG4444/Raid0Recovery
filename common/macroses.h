#ifndef MACROSES_H
#define MACROSES_H

#ifdef QT_DEBUG

#define ASSERT(n)  if(n)   {\
                                storeLog(QString(tr("trying %1 != 0...ok")).arg(#n));\
                           }\
                           else\
                           {\
                                storeLog(QString(tr("fatal assert on line %1 file %2 : %3 == false")).arg(__LINE__).arg(__FILE__).arg(#n));\
                           }
#else
#define ASSERT(n) if(!(n)) {\
                                storeLog(QString(tr("fatal assert on line %1 file %2 : %3 == false")).arg(__LINE__).arg(__FILE__).arg(#n));\
                           }
#endif
#endif // MACROSES_H
