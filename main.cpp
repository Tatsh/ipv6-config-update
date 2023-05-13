#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtDBus/QDBusPendingReply>
#include <QtNetwork/QNetworkInterface>
#include <systemd/sd-daemon.h>

#include "cidr.h"
#include "ipv6configdebug.h"
#include "systemd1_interface.h"
#include "utils.h"

const QString appName = QStringLiteral("ipv6-config-update");
const QString appVersion = QStringLiteral("0.0.1");
const QString cidrRe = QStringLiteral("%1[0-9]{2}:[^/]+/%2");
const QString orgDomain = QStringLiteral("sh.tat.ipv6-config-update");
const QString orgName = QStringLiteral("Tatsh");
const QString settingsKeyFiles = QStringLiteral("main/files");
const QString settingsKeyInterface = QStringLiteral("main/interface");
const QString settingsKeyPrefixLength = QStringLiteral("main/prefixLength");
const QString settingsKeyUnits = QStringLiteral("main/units");
const QString systemd1Domain = QStringLiteral("org.freedesktop.systemd1");
const QString systemd1Path = QStringLiteral("/org/freedesktop/systemd1");
const QString unitModeReplace = QStringLiteral("replace");
const QString messageFailedToConnectToBus = QStringLiteral("Failed to connect to system bus.");
const QString messageFailedToCreateSystemd1Interface =
    QStringLiteral("Failed to create a valid interface for org.freedesktop.systemd1.");
const QString messageInvalidInterfaceEmpty = QStringLiteral("Interface value is empty.");
const QString messageInvalidPrefixLength =
    QStringLiteral("Invalid prefix length. Must be multiple of 8.");

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
    QRegularExpression re(cidrRe.arg(cidr.string().left(2)).arg(prefixLength));
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Regular expression:" << re.pattern();
    bool needsRestarts = false;
    for (auto fileName : files) {
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Reading" << fileName;
        QFile f(fileName);
        f.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::ExistingOnly);
        auto content = QString::fromLocal8Bit(f.readAll());
        auto original = content;
        content.replace(re, cidr.string());
        if (original == content) {
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << fileName << "needs no changes.";
            f.close();
            continue;
        }
        f.seek(0);
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
        sdNotifyErrorStopping(messageFailedToConnectToBus, 111);
    }
    SystemdManager manager(systemd1Domain, systemd1Path, QDBusConnection::systemBus());
    if (!manager.isValid()) {
        sdNotifyErrorStopping(messageFailedToCreateSystemd1Interface, 38);
    }
    sd_notify(0, "READY=1");
    const auto files = settings.value(settingsKeyFiles).toStringList();
    const auto interface = settings.value(settingsKeyInterface).toString();
    const auto prefixLength = settings.value(settingsKeyPrefixLength, 56).toUInt();
    const auto units = settings.value(settingsKeyUnits).toStringList();
    if (interface.isEmpty()) {
        sdNotifyErrorStopping(messageInvalidInterfaceEmpty, 38);
    }
    if ((prefixLength % 8) != 0) {
        sdNotifyErrorStopping(messageInvalidPrefixLength, 38);
        return 1;
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Files to update:" << files;
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Interface:" << interface;
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Prefix length:" << prefixLength;
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Units:" << units;
    doUpdates(Cidr::current(interface, prefixLength), files, units, manager, prefixLength);
    sd_notify(0, "STOPPING=1");
    return 0;
}
