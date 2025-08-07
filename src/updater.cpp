#include "updater.h"

#include "constants.h"
#include "ipwatchdebug.h"

namespace Updater {

bool Updater::run() {
    if (!m_cidr.isValid()) {
        qCCritical(LOG_IPV6_CONFIG_UPDATE) << "Invalid CIDR:" << m_cidr.string();
        m_sdUtil.notify(0, "STATUS=Could not get current CIDR.\nERRNO=22");
        return false;
    }
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Generated CIDR:" << m_cidr.string();
    QRegularExpression re(cidrRe.arg(m_cidr.string().left(2)).arg(m_prefixLength));
    qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Regular expression:" << re.pattern();
    auto needsRestarts = false;
    for (const auto &fileName : m_files) {
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Reading" << fileName;
        const auto originalContent = m_fileManager.readContent(fileName);
        auto newContent = QString(originalContent).replace(re, m_cidr.string());
        if (originalContent == newContent) {
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << "No changes needed for" << fileName;
            continue;
        }
        m_sdUtil.notifyStatus(QStringLiteral("Updating config file."));
        m_fileManager.replaceFile(fileName, newContent);
        needsRestarts = true;
    }
    if (needsRestarts) {
        m_sdUtil.notifyStatus(QStringLiteral("Restarting units."));
        for (const auto &serviceName : m_units) {
            qCDebug(LOG_IPV6_CONFIG_UPDATE) << "Restarting" << serviceName;
            m_sdUtil.restartUnit(serviceName, unitModeReplace);
        }
    } else {
        m_sdUtil.notifyStatus(QStringLiteral("No unit restarts needed."));
        qCDebug(LOG_IPV6_CONFIG_UPDATE) << "No unit restarts needed.";
    }
    return true;
}

} // namespace Updater
