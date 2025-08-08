/**
 * @file constants.h
 * Defines application-wide constant values used throughout the project.
 */
#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

/** Application name constant. */
const QString appName = QStringLiteral("ipv6-config-update");
/** CIDR regular expression format string. */
const QString cidrRe = QStringLiteral("%1[0-9]{2}:[^/]+/%2");
/** Organisation domain. */
const QString orgDomain = QStringLiteral("sh.tat.ipv6-config-update");
/** Organisation name. */
const QString orgName = QStringLiteral("Tatsh");
/** Setting key for files to monitor. */
const QString settingsKeyFiles = QStringLiteral("main/files");
/** Settings key for the network interface to monitor. */
const QString settingsKeyInterface = QStringLiteral("main/interface");
/** Settings key for th prefix length. */
const QString settingsKeyPrefixLength = QStringLiteral("main/prefixLength");
/** Settings key for units to restart. */
const QString settingsKeyUnits = QStringLiteral("main/units");
const QString slashFormat = QStringLiteral("%1/%2");
const QString systemd1Domain = QStringLiteral("org.freedesktop.systemd1");
const QString systemd1Path = QStringLiteral("/org/freedesktop/systemd1");
const QString unitModeReplace = QStringLiteral("replace");
