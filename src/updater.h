#pragma once

#include "cidr.h"
#include "filemanager.h"
#include "systemdutil.h"

namespace Updater {
    /** The Updater class is responsible for updating configuration files with the current CIDR
    * and notifying systemd about the changes.
    * It reads the CIDR value, updates specified files, and restarts systemd units if necessary.
    */
    class Updater {
    public:
        /**
        * Constructs an Updater object.
        *
        * Initialises the Updater with the provided CIDR value, list of files, systemd units,
        * prefix length, and references to systemd and file manager utilities.
        *
        * @param cidr The CIDR value representing the IP range to be managed.
        * @param files List of file paths to be updated.
        * @param units List of systemd unit names to be managed.
        * @param prefixLength The prefix length for the CIDR block.
        * @param sdUtil Reference to an abstract systemd utility for managing systemd units.
        * @param fileManager Reference to an abstract file manager for file operations.
        */
        Updater(const Cidr::Value &cidr,
                const QStringList &files,
                const QStringList &units,
                uint prefixLength,
                AbstractSystemdUtil &sdUtil,
                AbstractFileManager &fileManager)
            : m_fileManager(fileManager), m_sdUtil(sdUtil), m_cidr(cidr), m_files(files),
              m_units(units), m_prefixLength(prefixLength) {
        }
        /**
        * Runs the updater process.
        *
        * This method performs the following steps:
        * 1. Validates the CIDR value.
        * 2. Reads the specified files and replaces occurrences of the CIDR value.
        * 3. Notifies systemd about the status and restarts units if necessary.
        *
        * @return True if the update was successful, false otherwise.
        */
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
