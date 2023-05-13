#ifndef CIDR_H
#define CIDR_H

#include <QtCore/QString>
#include <QtNetwork/QNetworkInterface>

namespace Cidr {
class Value {
public:
    explicit Value(QString string, bool isValid = false);
    Value(bool isValid = false);
    bool isValid() const;
    bool operator==(const Value &b) const;
    bool operator!=(const Value &b) const;
    QString string() const;

private:
    QString m_string;
    bool m_isValid;
};

QDebug operator<<(QDebug debug, const Cidr::Value &v);

Value toCidr(const int prefixLength, const Q_IPV6ADDR &val_);
Value current(const QString &interfaceName_, const uint prefixLength);
} // namespace Cidr

#endif // CIDR_H
