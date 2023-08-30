#ifndef QTTELNETAUTHNULL_H
#define QTTELNETAUTHNULL_H

#include "qttelnetauth.h"

class QtTelnetAuthNull : public QtTelnetAuth
{
public:
    QtTelnetAuthNull();

    /*!
     * \brief authStep  : 实现基类中的这个函数
     * \see QtTelnetAuth::authStep()
     */
    QByteArray authStep(const QByteArray& data);
};

#endif // QTTELNETAUTHNULL_H
