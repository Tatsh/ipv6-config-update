#pragma once

#include <QtCore/QString>
#include <QtNetwork/QNetworkInterface>

/** Abstract class for providing network information.
 *
 * This abstract class defines a method to retrieve network address entries
 * for a given network interface name.
 */
struct AbstractNetworkInfo {
    /** Virtual destructor for the abstract network info. */
    virtual ~AbstractNetworkInfo() = default;
    /**
     * Retrieves the list of network address entries for a given interface name.
     *
     * @param name The name of the network interface.
     * @return A list of QNetworkAddressEntry objects for the specified interface.
     */
    virtual QList<QNetworkAddressEntry> addressEntries(const QString &name) const = 0;
};

/**
 * Provides network information by implementing AbstractNetworkInfo using QNetworkInterface.
 */
struct NetworkInfo : AbstractNetworkInfo {
    QList<QNetworkAddressEntry> addressEntries(const QString &name) const override {
        return QNetworkInterface::interfaceFromName(name).addressEntries();
    };
};
