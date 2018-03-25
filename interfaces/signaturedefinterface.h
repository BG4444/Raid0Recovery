#ifndef SIGNATUREDEFINTRFACE_H
#define SIGNATUREDEFINTRFACE_H

#include <QtPlugin>
#include <set>
#include <atomic>

class SignatureDefInterface;

class SignatureDetector:public QObject
{
    Q_OBJECT    
signals:
   void found(const uchar* offset);
   void percent(int percent);
   void storeLog(const QString& msg);
public:
   const SignatureDefInterface* parent;
   virtual void run(const std::atomic<bool>& stopper)=0;
   virtual void build(QWidget* parent, const qulonglong stripeSize)=0;
   SignatureDetector(const SignatureDefInterface* parent):parent(parent)
   {

   }
};

class SignatureDefInterface
{
public:
   virtual SignatureDetector* make(const uchar* offset,const  quint64 size) = 0;
   virtual ~SignatureDefInterface(){};
};

#define SignatureDefInterface_iid "com.github.bg4444.SignatureDefInterface"

Q_DECLARE_INTERFACE(SignatureDefInterface, SignatureDefInterface_iid)

#endif // SIGNATUREDEFINTRFACE_H
