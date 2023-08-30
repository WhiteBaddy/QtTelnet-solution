#include "qttelnetauth.h"

QtTelnetAuth::QtTelnetAuth(char code)
    :m_state(AuthIntermidiate)
    , m_code(code)
{
}

QtTelnetAuth::~QtTelnetAuth()
{
}

int QtTelnetAuth::getCode() const
{
    return m_code;
}

QtTelnetAuth::State QtTelnetAuth::getState() const
{
    return m_state;
}

void QtTelnetAuth::setState(State state)
{
    m_state = state;
}
