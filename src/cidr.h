#pragma once

#include <QtCore/QString>
#include <QtNetwork/QNetworkInterface>

#include "constants.h"
#include "networkinfo.h"

namespace Cidr {

class Value {
public:
    Value(bool isValid = false) : m_isValid(isValid) {};
    explicit Value(QString string, bool isValid) : m_string(string), m_isValid(isValid) {};
    bool isValid() const {
        return m_isValid;
    };
    bool operator==(const Value &b) const {
        return m_string == b.m_string;
    };
    bool operator!=(const Value &b) const {
        return m_string != b.m_string;
    };
    QString string() const {
        return m_string;
    };

private:
    QString m_string;
    bool m_isValid;
};

QDebug operator<<(QDebug debug, const Cidr::Value &v);
Value toCidr(int prefixLength, const Q_IPV6ADDR &val_);
Value current(const QString &interfaceName_,
              uint prefixLength,
              const AbstractNetworkInfo &networkInfo);

} // namespace Cidr
