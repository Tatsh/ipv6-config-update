#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtDBus/QDBusPendingReply>
#include <QtNetwork/QNetworkInterface>
#include <systemd/sd-daemon.h>

#include "ipv6configdebug.h"
#include "systemd1_interface.h"

const QString appName = QStringLiteral("ipv6-config-update");
const QString appVersion = QStringLiteral("0.0.1");
const QString cidrRe = QStringLiteral("%1[0-9]{2}:.*/%2");
const QString orgDomain = QStringLiteral("sh.tat.ipv6-config-update");
const QString orgName = QStringLiteral("Tatsh");
const QString settingsKeyFiles = QStringLiteral("main/files");
const QString settingsKeyInterface = QStringLiteral("main/interface");
const QString settingsKeyPrefixLength = QStringLiteral("main/prefixLength");
const QString settingsKeyUnits = QStringLiteral("main/units");
const QString slashFormat = QStringLiteral("%1/%2");
const QString systemd1Domain = QStringLiteral("org.freedesktop.systemd1");
const QString systemd1Path = QStringLiteral("/org/freedesktop/systemd1");
const QString unitModeReplace = QStringLiteral("replace");

namespace Cidr {
class Value {
public:
    explicit Value(QString string, bool isValid = false) : m_string(string), m_isValid(isValid) {
    }
    Value(bool isValid = false) : m_isValid(isValid) {
    }
    bool isValid() const {
        return m_isValid;
    }
    bool operator!=(const Value &b) const {
        return m_string != b.m_string;
    }
    QString string() const {
        return m_string;
    }

private:
    QString m_string;
    bool m_isValid;
};

QDebug operator<<(QDebug debug, const Value &v) {
    QDebugStateSaver saver(debug);
    debug.nospace() << v.string();
    return debug;
}

inline Value toCidr(const int prefixLength, const Q_IPV6ADDR &val_) {
    auto val = Q_IPV6ADDR(val_);
    std::fill(val.c + (prefixLength / 8), val.c + 16, 0);
    auto asString = QHostAddress(val).toString();
    return Value(slashFormat.arg(asString).arg(prefixLength).trimmed(), !asString.isEmpty());
}

inline Value current(const QString &interfaceName_, const uint prefixLength) {
    for (auto entry : QNetworkInterface::interfaceFromName(interfaceName_).addressEntries()) {
        if (entry.ip().isGlobal() && entry.ip().protocol() == QAbstractSocket::IPv6Protocol) {
            return toCidr(prefixLength, entry.ip().toIPv6Address());
        }
    }
    return Value();
}
} // namespace Cidr

void doUpdates(const Cidr::Value &cidr,
               const QStringList &files,
               const QStringList &units,
               SystemdManager &manager,
               const uint prefixLength) {
    if (!cidr.isValid()) {
        qCCritical(LOG_IPV6_CONFIG_UPDATE) << "Invalid CIDR:" << cidr;
        sd_notify(0, "STATUS=Could not get current CIDR\nERRNO=22");
        return;
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Generated CIDR:" << cidr;
    QRegularExpression re(cidrRe.arg(cidr.string().left(2)).arg(prefixLength).trimmed());
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Regular expression:" << re.pattern();
    bool needsRestarts = false;
    for (auto fileName : files) {
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Reading" << fileName;
        QFile f(fileName);
        f.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::ExistingOnly);
        auto content = QString::fromLocal8Bit(f.readAll());
        if (content.contains(cidr.string())) {
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << fileName << "needs no changes.";
            f.close();
            continue;
        }
        f.seek(0);
        content.replace(re, cidr.string());
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Writing" << fileName;
        sd_notify(0, "STATUS=Updating config file");
        f.write(content.toLocal8Bit());
        f.close();
        needsRestarts = true;
    }
    if (needsRestarts) {
        sd_notify(0, "STATUS=Restarting units");
        QList<QDBusPendingReply<>> replies;
        for (auto serviceName : units) {
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Restarting" << serviceName;
            replies << manager.ReloadOrRestartUnit(serviceName, unitModeReplace);
        }
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Waiting for D-Bus replies";
        sd_notify(0, "STATUS=Waiting for D-Bus replies");
        for (auto reply : replies) {
            reply.waitForFinished();
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Reply arg(0)" << reply.argumentAt(0).toString();
        }
        sd_notify(0, "STATUS=All replies received. Done.");
    } else {
        sd_notify(0, "STATUS=No service restarts needed.");
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "No service restarts needed.";
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setApplicationVersion(appVersion);
    QCoreApplication::setOrganizationDomain(orgDomain);
    QCoreApplication::setOrganizationName(orgName);
    QSettings settings;
    if (!QDBusConnection::systemBus().isConnected()) {
        sd_notify(0, "STATUS=Failed to connect to the system bus.\nERRNO=111\nSTOPPING=1");
        qFatal("Failed to connect to the system bus.");
    }
    SystemdManager manager(systemd1Domain, systemd1Path, QDBusConnection::systemBus());
    if (!manager.isValid()) {
        sd_notify(0, "STATUS=Failed to connect to the system bus.\nERRNO=38\nSTOPPING=1");
        qFatal("Failed to create a valid interface for org.freedesktop.systemd1.");
    }
    sd_notify(0, "READY=1");
    auto interface = settings.value(settingsKeyInterface).toString();
    if (interface.isEmpty()) {
        sd_notify(0, "STATUS=Invalid interface (empty).\nERRNO=38\nSTOPPING=1");
        qFatal("No interface specified.");
    }
    auto prefixLength = settings.value(settingsKeyPrefixLength, 56).toUInt();
    if ((prefixLength % 8) != 0) {
        sd_notify(0, "STATUS=Invalid prefix length.\nERRNO=38\nSTOPPING=1");
        qFatal("Only prefix lengths of multiples of 8 are supported.");
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Prefix length:" << prefixLength;
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Interface:" << interface;
    doUpdates(Cidr::current(interface, prefixLength),
              settings.value(settingsKeyFiles).toStringList(),
              settings.value(settingsKeyUnits).toStringList(),
              manager,
              prefixLength);
    sd_notify(0, "STOPPING=1");
    return 0;
}
