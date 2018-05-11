#include "finduuiddef.h"
#include "finduuiddetector.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>

SignatureDetector *FindUUIDDef::make()
{
    return new FindUUIDDetector(this,sign);
}


void FindUUIDDef::configure(QWidget *parent, QSettings *sets)
{

    this->set.reset(new QPrefixedSettings(sets,metaObject()->className()));
    auto layout=new QVBoxLayout(parent);
    parent->setLayout(layout);

    auto edit= new QLineEdit(parent);
    layout->addWidget(edit);
    connect(edit,&QLineEdit::textChanged,this,&FindUUIDDef::onTextChanged);

    edit->setText(set->value("searchString","").toString());

    auto status = new QLabel(parent);
    layout->addWidget(status);
    connect(this,&FindUUIDDef::updateStatus,status,&QLabel::setText);
}

void FindUUIDDef::onTextChanged(const QString &text)
{
    if(!text.length())
    {
        emit updateStatus(tr("input is empty"));
        return;
    }

    if(text.length() % 2)
    {
        emit updateStatus(tr("input is non odd"));
        return;
    }

    std::vector<uchar> tsign;
    tsign.resize(text.length() / 2);

    for(int i=0;i<text.length();i+=2)
    {
        const QString& sym=text.mid(i,2);

        bool ok;
        tsign[i/2]=sym.toUInt(&ok,16);
        if(!ok)
        {
            emit updateStatus(tr("input contains wrong symbols %1 start %2").arg(sym).arg(i));
            return;
        }
    }

    sign.swap(tsign);

    set->setValue("searchString",text);

    emit updateStatus(tr("OK"));
}
