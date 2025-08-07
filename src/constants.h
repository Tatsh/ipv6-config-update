#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

const QString appName = QStringLiteral("ipv6-config-update");
const QString cidrRe = QStringLiteral("%1[0-9]{2}:[^/]+/%2");
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
