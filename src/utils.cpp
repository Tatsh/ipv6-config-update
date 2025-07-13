#include <systemd/sd-daemon.h>

#include "ipwatchdebug.h"
#include "utils.h"

void sdNotifyErrorStopping(const QString &status, int errorNumber) noexcept {
    sd_notify(0,
              QStringLiteral("STATUS=%1\nERRNO=%2\nSTOPPING=1")
                  .arg(status)
                  .arg(errorNumber)
                  .toUtf8()
                  .constData());
    qCCritical(LOG_IPV6_CONFIG_UPDATE) << status;
    exit(EXIT_FAILURE);
}

void sdNotifyStatus(const QString &status) noexcept {
    sd_notify(0, QStringLiteral("STATUS=%1").arg(status).toUtf8().constData());
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << status;
}
