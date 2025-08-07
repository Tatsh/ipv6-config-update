#include <algorithm>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtCore/QTranslator>
#include <QtDBus/QDBusPendingReply>
#include <QtNetwork/QNetworkInterface>

#include "cidr.h"
#include "constants.h"
#include "filemanager.h"
#include "ipwatchdebug.h"
#include "networkinfo.h"
#include "systemdutil.h"
#include "updater.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setApplicationVersion(QString::fromLocal8Bit(VERSION));
    QCoreApplication::setOrganizationDomain(orgDomain);
    QCoreApplication::setOrganizationName(orgName);
    QSettings settings;
    if (geteuid() == 0) {
        settings.setPath(QSettings::IniFormat,
                         QSettings::SystemScope,
                         QStringLiteral("/etc/ipv6-config-update.conf"));
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Using settings file:" << settings.fileName();
    SystemdManager manager(systemd1Domain, systemd1Path, QDBusConnection::systemBus());
    SystemdUtil sdUtil(manager);
    if (!QDBusConnection::systemBus().isConnected()) {
        sdUtil.notifyErrorStopping(QStringLiteral("Failed to connect to system bus."),
                                   ECONNREFUSED);
        return 1;
    }
    if (!manager.isValid()) {
        sdUtil.notifyErrorStopping(QStringLiteral("Failed to get systemd1 interface."), EINVAL);
        return 1;
    }
    sdUtil.notify(0, "READY=1");
    const auto files = settings.value(settingsKeyFiles).toStringList();
    const auto interface = settings.value(settingsKeyInterface).toString();
    const auto prefixLength = settings.value(settingsKeyPrefixLength, 56).toUInt();
    const auto units = settings.value(settingsKeyUnits).toStringList();
    if (interface.isEmpty()) {
        sdUtil.notifyErrorStopping(QStringLiteral("Empty interface name."), EINVAL);
        return 1;
    }
    if ((prefixLength % 8) != 0) {
        sdUtil.notifyErrorStopping(QStringLiteral("Invalid prefix name."), EINVAL);
        return 1;
    }
    qCInfo(LOG_IPV6_CONFIG_UPDATE) << "Files to update:";
    for (const auto &file : files) {
        qCInfo(LOG_IPV6_CONFIG_UPDATE) << "-" << file;
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Interface:" << interface;
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Prefix length:" << prefixLength;
    qCInfo(LOG_IPV6_CONFIG_UPDATE) << "Units:";
    for (const auto &unit : units) {
        qCInfo(LOG_IPV6_CONFIG_UPDATE) << "-" << unit;
    }
    FileManager fileManager;
    Updater::Updater(Cidr::current(interface, prefixLength, NetworkInfo()),
                     files,
                     units,
                     prefixLength,
                     sdUtil,
                     fileManager)
        .run();
    sdUtil.notify(0, "STOPPING=1");
    return 0;
}
