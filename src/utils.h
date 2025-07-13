#pragma once

#include <QtCore/QString>

void sdNotifyErrorStopping(const QString &status, int errorNumber) noexcept
    __attribute__((__noreturn__));
void sdNotifyStatus(const QString &status) noexcept;
