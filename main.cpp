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
const QString ipv6_56FormatRe = QStringLiteral("%1[0-9]{2}:.*/56");
const QString orgDomain = QStringLiteral("sh.tat.ipv6-config-update");
const QString orgName = QStringLiteral("Tatsh");
const QString settingsFilePath = QStringLiteral("/etc/ipv6-config-update.conf");
const QString settingsKeyFiles = QStringLiteral("files");
const QString settingsKeyInterface = QStringLiteral("interface");
const QString settingsKeyUnits = QStringLiteral("units");
const QString slash56Format = QStringLiteral("%1/56");
const QString systemd1Domain = QStringLiteral("org.freedesktop.systemd1");
const QString systemd1Path = QStringLiteral("org/freedesktop/systemd1");
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

inline Value to56(const QNetworkAddressEntry &entry) {
    auto val = entry.ip().toIPv6Address();
    std::fill(val.c + 7, val.c + 16, 0);
    auto asString = QHostAddress(val).toString();
    return Value(slash56Format.arg(asString), !asString.isEmpty());
}

inline Value current(const QString &interfaceName_) {
    for (auto entry : QNetworkInterface::interfaceFromName(interfaceName_).addressEntries()) {
        if (entry.ip().isGlobal() && entry.ip().protocol() == QAbstractSocket::IPv6Protocol) {
            return to56(entry);
        }
    }
    return Value();
}
} // namespace Cidr

void doUpdates(const Cidr::Value &cidr,
               const QStringList &files,
               const QStringList &units,
               SystemdManager &manager) {
    if (!cidr.isValid()) {
        qCCritical(LOG_IPV6_CONFIG_UPDATE) << "Invalid CIDR:" << cidr;
        sd_notify(0, "STATUS=Could not get current CIDR\nERRNO=22");
        return;
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Generated CIDR:" << cidr;
    QRegExp re(ipv6_56FormatRe.arg(cidr.string().left(2)));
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
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Reply arg(2)" << reply.argumentAt(2).toString();
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
    QSettings settings(settingsFilePath);
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
    doUpdates(Cidr::current(settings.value(settingsKeyInterface).toString()),
              settings.value(settingsKeyFiles).toStringList(),
              settings.value(settingsKeyUnits).toStringList(),
              manager);
    sd_notify(0, "STOPPING=1");
    return app.exec();
}
