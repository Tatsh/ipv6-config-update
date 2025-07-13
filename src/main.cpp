#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtCore/QTranslator>
#include <QtDBus/QDBusPendingReply>
#include <QtNetwork/QNetworkInterface>
#include <systemd/sd-daemon.h>

#include "cidr.h"
#include "ipwatchdebug.h"
#include "systemd1_interface.h"
#include "utils.h"

const QString appName = QStringLiteral("ipv6-config-update");
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
const QString messageFailedToConnectToBus =
    QCoreApplication::translate("", "Failed to connect to system bus.");
const QString messageFailedToCreateSystemd1Interface = QCoreApplication::translate(
    "", "Failed to create a valid interface for org.freedesktop.systemd1.");
const QString messageInvalidInterfaceEmpty =
    QCoreApplication::translate("", "Interface value is empty.");
const QString messageInvalidPrefixLength =
    QCoreApplication::translate("", "Invalid prefix length. Must be multiple of 8.");

void doUpdates(const Cidr::Value &cidr,
               const QStringList &files,
               const QStringList &units,
               SystemdManager &manager,
               const uint prefixLength) {
    if (!cidr.isValid()) {
        auto message = QCoreApplication::translate("", "Invalid CIDR: %1").arg(cidr.string());
        qCCritical(LOG_IPV6_CONFIG_UPDATE) << message;
        sd_notify(0, "STATUS=Could not get current CIDR\nERRNO=22");
        return;
    }
    auto message = QCoreApplication::translate("", "Generated CIDR: %1").arg(cidr.string());
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    QRegularExpression re(cidrRe.arg(cidr.string().left(2)).arg(prefixLength));
    message = QCoreApplication::translate("", "Regular expression: %1").arg(re.pattern());
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    bool needsRestarts = false;
    for (auto fileName : files) {
        message = QCoreApplication::translate("", "Reading %1.").arg(fileName);
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
        QFile f(fileName);
        f.open(QIODevice::Text | QIODevice::ReadWrite | QIODevice::ExistingOnly);
        auto content = QString::fromLocal8Bit(f.readAll());
        auto original = content;
        content.replace(re, cidr.string());
        if (original == content) {
            message = QCoreApplication::translate("", "No changes needed for %1.").arg(fileName);
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
            f.close();
            continue;
        }
        f.seek(0);
        message = QCoreApplication::translate("", "Writing %1.").arg(fileName);
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
        sd_notify(0, "STATUS=Updating config file");
        f.write(content.toLocal8Bit());
        f.close();
        needsRestarts = true;
    }
    if (needsRestarts) {
        sd_notify(0, "STATUS=Restarting units");
        QList<QDBusPendingReply<QString>> replies;
        for (auto serviceName : units) {
            message = QCoreApplication::translate("", "Restarting %1.").arg(serviceName);
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
            replies << manager.ReloadOrRestartUnit(serviceName, unitModeReplace);
        }
        qCDebug(LOG_IPV6_CONFIG_UPDATE)
            << QCoreApplication::translate("", "Waiting for D-Bus replies.");
        sd_notify(0, "STATUS=Waiting for D-Bus replies");
        for (auto reply : replies) {
            reply.waitForFinished();
            message = QCoreApplication::translate("", "Reply arg(0): %1")
                          .arg(reply.argumentAt(0).toString());
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
        }
        sd_notify(0, "STATUS=All replies received. Done.");
    } else {
        sd_notify(0, "STATUS=No service restarts needed.");
        qCDebug(LOG_IPV6_CONFIG_UPDATE)
            << QCoreApplication::translate("", "No service restarts needed.");
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setApplicationVersion(QString::fromLocal8Bit(VERSION));
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
    auto message = QCoreApplication::translate("", "Files to update: %1")
                       .arg(files.join(QStringLiteral(", ")));
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    message = QCoreApplication::translate("", "Interface: %1").arg(interface);
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    message = QCoreApplication::translate("", "Prefix length: %1").arg(prefixLength);
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    message = QCoreApplication::translate("", "Units: %1").arg(units.join(QStringLiteral(", ")));
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << message;
    doUpdates(Cidr::current(interface, prefixLength), files, units, manager, prefixLength);
    sd_notify(0, "STOPPING=1");
    return 0;
}
