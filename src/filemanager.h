#pragma once

#include <QtCore/QFile>

struct AbstractFileManager {
    enum State { FailedToOpen, FailedToRemove, FailedToRename, Success };

    virtual ~AbstractFileManager() = default;
    virtual QString readContent(const QString &fileName) const = 0;
    virtual State replaceFile(const QString &fileName, const QString &newContent) = 0;
};

struct FileManager : AbstractFileManager {
    QString readContent(const QString &fileName) const override {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::ExistingOnly);
        return QString::fromUtf8(file.readAll());
    }

    State replaceFile(const QString &fileName, const QString &newContent) override {
        QString tempFileName = fileName + QStringLiteral(".tmp");
        QFile tempFile(tempFileName);
        if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return FailedToOpen;
        }
        tempFile.write(newContent.toUtf8());
        tempFile.close();
        if (!QFile::remove(fileName)) {
            return FailedToRemove;
        }
        return QFile::rename(tempFileName, fileName) ? Success : FailedToRename;
    }
};
