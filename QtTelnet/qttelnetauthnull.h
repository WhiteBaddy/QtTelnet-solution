#ifndef QTTELNETAUTHNULL_H
#define QTTELNETAUTHNULL_H

#include "qttelnetauth.h"

class QtTelnetAuthNull : public QtTelnetAuth
{
public:
    QtTelnetAuthNull();

    QByteArray authStep(const QByteArray& data);
};

#endif // QTTELNETAUTHNULL_H
