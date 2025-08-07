#pragma once

#include "cidr.h"
#include "filemanager.h"
#include "systemdutil.h"

namespace Updater {
class Updater {
public:
    Updater(const Cidr::Value &cidr,
            const QStringList &files,
            const QStringList &units,
            uint prefixLength,
            AbstractSystemdUtil &sdUtil,
            AbstractFileManager &fileManager)
        : m_fileManager(fileManager), m_sdUtil(sdUtil), m_cidr(cidr), m_files(files),
          m_units(units), m_prefixLength(prefixLength) {
    }
    bool run();

private:
    AbstractFileManager &m_fileManager;
    AbstractSystemdUtil &m_sdUtil;
    const Cidr::Value &m_cidr;
    const QStringList &m_files;
    const QStringList &m_units;
    uint m_prefixLength;
};
} // namespace Updater
