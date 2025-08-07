#pragma once

#include <QtCore/QString>
#include <QtNetwork/QNetworkInterface>

struct AbstractNetworkInfo {
    virtual ~AbstractNetworkInfo() = default;
    virtual QList<QNetworkAddressEntry> addressEntries(const QString &name) const = 0;
};

struct NetworkInfo : AbstractNetworkInfo {
    QList<QNetworkAddressEntry> addressEntries(const QString &name) const override {
        return QNetworkInterface::interfaceFromName(name).addressEntries();
    };
};
