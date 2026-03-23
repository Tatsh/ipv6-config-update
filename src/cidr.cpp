#include <QtCore/QDebugStateSaver>

#include "cidr.h"
#include "ipwatchdebug.h"

namespace Cidr {

QDebug operator<<(QDebug debug, const Value &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << v.string();
    return debug;
}

Value current(const QString &interfaceName_,
              uint prefixLength,
              const AbstractNetworkInfo &networkInfo) {
    for (const auto &entry : networkInfo.addressEntries(interfaceName_)) {
        if (entry.ip().isGlobal() && entry.ip().protocol() == QAbstractSocket::IPv6Protocol) {
            return toCidr(prefixLength, entry.ip().toIPv6Address());
        }
    }
    return Value();
}

Value toCidr(const int prefixLength, const Q_IPV6ADDR &val_) {
    auto val = Q_IPV6ADDR(val_);
    std::fill(val.c + (prefixLength / 8), val.c + 16, 0);
    auto asString = QHostAddress(val).toString();
    return Value(slashFormat.arg(asString).arg(prefixLength), !asString.isEmpty());
}

} // namespace Cidr
