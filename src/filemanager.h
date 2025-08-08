#pragma once

#include <QtCore/QFile>

/** Abstract file manager interface for reading and replacing file content.
 *
 * This interface defines methods for reading the content of a file and replacing it with new
 * content.
 */
struct AbstractFileManager {
    /** Possible states for file operations.
     * These states indicate the result of attempting to replace a file's content.
     */
    enum State {
        FailedToOpen,   /*!< Failed to open the file for writing. */
        FailedToRemove, /*!< Failed to remove the original file before renaming. */
        FailedToRename, /*!< Failed to rename the temporary file to the original file name. */
        Success         /*!< The file was successfully replaced. */
    };

    /** Virtual destructor for the abstract file manager. */
    virtual ~AbstractFileManager() = default;
    /**
     * Reads the content of a file.
     *
     * @param fileName The name of the file to read.
     * @return The content of the file as a QString.
     */
    virtual QString readContent(const QString &fileName) const = 0;
    /**
     * Replaces the content of a file with new content.
     *
     * @param fileName The name of the file to replace.
     * @param newContent The new content to write to the file.
     * @return The state of the operation, indicating success or failure.
     */
    virtual State replaceFile(const QString &fileName, const QString &newContent) = 0;
};

/**
 * File manager implementation that uses QFile for file operations.
 *
 * This class implements the AbstractFileManager interface using QFile to read and replace file
 * content.
 */
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
