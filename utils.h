#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QString>

void sdNotifyErrorStopping(const QString &status, int errorNumber) noexcept
    __attribute__((__noreturn__));
void sdNotifyStatus(const QString &status) noexcept;

#endif // UTILS_H
