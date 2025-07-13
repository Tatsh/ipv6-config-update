#include <QtCore/QDebugStateSaver>

#include "cidr.h"

const QString slashFormat = QStringLiteral("%1/%2");

namespace Cidr {

Value::Value(QString string, bool isValid) : m_string(string), m_isValid(isValid) {
}

Value::Value(bool isValid) : m_isValid(isValid) {
}

bool Value::isValid() const {
    return m_isValid;
}

bool Value::operator==(const Value &b) const {
    return m_string == b.m_string;
}

bool Value::operator!=(const Value &b) const {
    return m_string != b.m_string;
}

QString Value::string() const {
    return m_string;
}

QDebug operator<<(QDebug debug, const Value &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << v.string();
    return debug;
}

Value toCidr(const int prefixLength, const Q_IPV6ADDR &val_) {
    auto val = Q_IPV6ADDR(val_);
    std::fill(val.c + (prefixLength / 8), val.c + 16, 0);
    auto asString = QHostAddress(val).toString();
    return Value(slashFormat.arg(asString).arg(prefixLength), !asString.isEmpty());
}

Value current(const QString &interfaceName_, const uint prefixLength) {
    for (auto entry : QNetworkInterface::interfaceFromName(interfaceName_).addressEntries()) {
        if (entry.ip().isGlobal() && entry.ip().protocol() == QAbstractSocket::IPv6Protocol) {
            return toCidr(prefixLength, entry.ip().toIPv6Address());
        }
    }
    return Value();
}
} // namespace Cidr
