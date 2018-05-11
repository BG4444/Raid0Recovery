#ifndef SIGNATUREDEFINTRFACE_H
#define SIGNATUREDEFINTRFACE_H

#include <QtPlugin>
#include <set>
#include <atomic>

class SignatureDefInterface;
class ProgressSignaler;
class QFile;

class QSettings;

class SignatureDetector:public QObject
{
    Q_OBJECT    
signals:
   void found(const quint64 offset);
   void storeLog(const QString& msg);
public:
   const SignatureDefInterface* parent;
   virtual void run(const std::atomic<bool>& stopper,uchar *base, const quint64 size,ProgressSignaler *sgn, const quint64 ofs)=0;
   virtual void build(QWidget* parent, const qulonglong stripeSize, const uchar* base, const quint64 size)=0;
   virtual quint64 granularity() = 0;
   virtual QString getDescription(const quint64 offset, QFile* file)=0;
   SignatureDetector(const SignatureDefInterface* parent):parent(parent)
   {

   }
};

class SignatureDefInterface
{
public:
   virtual SignatureDetector* make() = 0;
   virtual void configure(QWidget* parent,QSettings* set) = 0;
   virtual ~SignatureDefInterface()
   {

   }
};

#define SignatureDefInterface_iid "com.github.bg4444.SignatureDefInterface"

Q_DECLARE_INTERFACE(SignatureDefInterface, SignatureDefInterface_iid)

#endif // SIGNATUREDEFINTRFACE_H
