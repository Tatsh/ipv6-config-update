#pragma once

#include <systemd/sd-daemon.h>

#include "ipwatchdebug.h"
#include "systemd1_interface.h"

struct AbstractSystemdUtil {
    virtual ~AbstractSystemdUtil() = default;
    virtual void notify(int unsetEnvironment, const char *state) const noexcept = 0;
    virtual void notifyErrorStopping(const QString &status, int errorNumber) const noexcept = 0;
    virtual void notifyStatus(const QString &status) const noexcept = 0;
    virtual void restartUnit(const QString &unitName, const QString &mode) = 0;
};

class SystemdUtil : public AbstractSystemdUtil {
public:
    SystemdUtil(SystemdManager &manager) : m_manager(manager) {
    }

    void notify(int unsetEnvironment, const char *state) const noexcept override {
        sd_notify(unsetEnvironment, state);
    }

    void notifyErrorStopping(const QString &status, int errorNumber) const noexcept override {
        sd_notify(0,
                  QStringLiteral("STATUS=%1\nERRNO=%2\nSTOPPING=1")
                      .arg(status)
                      .arg(errorNumber)
                      .toUtf8()
                      .constData());
        qCCritical(LOG_IPV6_CONFIG_UPDATE) << "Status:" << status;
    }

    void notifyStatus(const QString &status) const noexcept override {
        sd_notify(0, QStringLiteral("STATUS=%1").arg(status).toUtf8().constData());
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Status:" << status;
    }

    void restartUnit(const QString &unitName, const QString &mode) override {
        auto reply = m_manager.ReloadOrRestartUnit(unitName, mode);
        reply.waitForFinished();
        if (reply.isError()) {
            qCCritical(LOG_IPV6_CONFIG_UPDATE)
                << "Failed to restart unit" << unitName << "with mode" << mode << ":"
                << reply.error().message();
        } else {
            qCDebug(LOG_IPV6_CONFIG_UPDATE)
                << "Successfully restarted unit" << unitName << "with mode" << mode;
        }
    }

private:
    SystemdManager &m_manager;
};
