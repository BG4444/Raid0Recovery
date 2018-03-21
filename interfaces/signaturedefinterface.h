#ifndef SIGNATUREDEFINTRFACE_H
#define SIGNATUREDEFINTRFACE_H

#include <QtPlugin>


class SignatureDetector:public QObject
{
    Q_OBJECT

signals:
   void found(const uchar* offset);
   void percent(int percent);
public:
   virtual void run()=0;

};

class SignatureDefInterface
{
public:
   virtual SignatureDetector* make(uchar* offset, quint64 size) = 0;
   virtual ~SignatureDefInterface(){};
};


#define SignatureDefInterface_iid "com.github.bg4444.SignatureDefInterface"

Q_DECLARE_INTERFACE(SignatureDefInterface, SignatureDefInterface_iid)

#endif // SIGNATUREDEFINTRFACE_H
