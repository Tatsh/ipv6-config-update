#pragma once

#include <systemd/sd-daemon.h>

#include "ipwatchdebug.h"
#include "systemd1_interface.h"

/**
 * Abstract interface for interacting with systemd.
 *
 * This struct defines a set of pure virtual methods for notifying systemd about
 * service status, errors, and for restarting units. Implementations should provide
 * concrete behavior for these operations.
 */
struct AbstractSystemdUtil {
    /**
     * Virtual destructor for safe polymorphic usage.
     */
    virtual ~AbstractSystemdUtil() = default;

    /**
     * Notify systemd about the current state of the service.
     *
     * @param unsetEnvironment If non-zero, unsets environment variables after notifying.
     * @param state The state string to send to systemd (e.g., "READY=1").
     */
    virtual void notify(int unsetEnvironment, const char *state) const noexcept = 0;

    /**
     * Notify systemd about an error that occurred while stopping the service.
     *
     * @param status A descriptive status message about the error.
     * @param errorNumber The error code associated with the failure.
     */
    virtual void notifyErrorStopping(const QString &status, int errorNumber) const noexcept = 0;

    /**
     * Notify systemd about the current status of the service.
     *
     * @param status A status message to send to systemd.
     */
    virtual void notifyStatus(const QString &status) const noexcept = 0;

    /**
     * Request systemd to restart a specific unit.
     *
     * @param unitName The name of the systemd unit to restart.
     * @param mode The restart mode (e.g., "replace", "fail").
     */
    virtual void restartUnit(const QString &unitName, const QString &mode) = 0;
};

/**
 * SystemdUtil class that implements AbstractSystemdUtil.
 *
 * This class provides concrete implementations for notifying systemd about service
 * status, errors, and restarting units using the SystemdManager interface.
 */
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
