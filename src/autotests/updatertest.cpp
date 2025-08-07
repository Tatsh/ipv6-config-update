#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtTest/QTest>

#include "cidr.h"
#include "filemanager.h"
#include "systemdutil.h"
#include "updater.h"

class UpdaterTest : public QObject {
    Q_OBJECT

public:
    UpdaterTest(QObject *parent = nullptr);
    ~UpdaterTest() override;

private Q_SLOTS:
    void testUpdaterDoesNothing();
    void testUpdaterInvalidCidr();
    void testUpdaterUpdatesFile();
};

UpdaterTest::UpdaterTest(QObject *parent) : QObject(parent) {
    Q_UNUSED(parent);
}

UpdaterTest::~UpdaterTest() {
}

class TestSystemdUtil : public AbstractSystemdUtil {
public:
    void notify(int unsetEnvironment, const char *state) const noexcept override {
        Q_UNUSED(unsetEnvironment);
        qDebug() << "Notify called with message:" << state;
    }

    void notifyErrorStopping(const QString &status, int errorNumber) const noexcept override {
        qDebug() << "Error stopping with status:" << status << "and error number:" << errorNumber;
    }

    void notifyStatus(const QString &status) const noexcept override {
        qDebug() << "Status notification:" << status;
    }

    void restartUnit(const QString &unitName, const QString &mode) override {
        qDebug() << "Restarting unit:" << unitName << "with mode:" << mode;
        m_restartedUnits.append(unitName);
    }

    bool didRestartUnit(const QString &unitName) const {
        return m_restartedUnits.contains(unitName);
    }

private:
    QList<QString> m_restartedUnits;
};

class TestUpdaterDoesNothingFileManager : public AbstractFileManager {
public:
    QString readContent(const QString &fileName) const override {
        return QStringLiteral("Original content of %1").arg(fileName);
    }

    State replaceFile(const QString &fileName, const QString &newContent) override {
        Q_UNUSED(fileName);
        Q_UNUSED(newContent);
        return Success;
    }
};

void UpdaterTest::testUpdaterDoesNothing() {
    const Cidr::Value cidr(QStringLiteral("2001:db8::/64"), true);
    const QList<QString> files = {QStringLiteral("/etc/config1"), QStringLiteral("/etc/config2")};
    const QList<QString> units = {QStringLiteral("unit1.service"), QStringLiteral("unit2.service")};
    TestSystemdUtil sdUtil;
    TestUpdaterDoesNothingFileManager fileManager;
    Updater::Updater updater(cidr, files, units, 64, sdUtil, fileManager);
    QVERIFY(updater.run());
}

void UpdaterTest::testUpdaterInvalidCidr() {
    const Cidr::Value cidr(QStringLiteral("invalid_cidr"), false);
    const QList<QString> files = {QStringLiteral("/etc/config1")};
    const QList<QString> units = {QStringLiteral("unit1.service")};
    TestSystemdUtil sdUtil;
    TestUpdaterDoesNothingFileManager fileManager;
    Updater::Updater updater(cidr, files, units, 64, sdUtil, fileManager);
    QVERIFY(!updater.run());
}

class TestUpdaterUpdatesFileFileManager : public AbstractFileManager {
public:
    QString readContent(const QString &fileName) const override {
        Q_UNUSED(fileName);
        return QStringLiteral("Original content with CIDR 2001:db8::/64");
    }

    State replaceFile(const QString &fileName, const QString &newContent) override {
        qDebug() << "Replacing file" << fileName << "with content:" << newContent;
        m_replacedFiles.append(fileName);
        return Success;
    }

    bool didReplaceFile(const QString &fileName) const {
        return m_replacedFiles.contains(fileName);
    }

private:
    QList<QString> m_replacedFiles;
};

void UpdaterTest::testUpdaterUpdatesFile() {
    const Cidr::Value cidr(QStringLiteral("2001:db9::/64"), true);
    const QList<QString> files = {QStringLiteral("/etc/config1")};
    const QList<QString> units = {QStringLiteral("unit1.service")};
    TestSystemdUtil sdUtil;
    TestUpdaterUpdatesFileFileManager fileManager;
    Updater::Updater updater(cidr, files, units, 64, sdUtil, fileManager);
    QVERIFY(updater.run());
    QVERIFY(sdUtil.didRestartUnit(QStringLiteral("unit1.service")));
    QVERIFY(fileManager.didReplaceFile(QStringLiteral("/etc/config1")));
}

QTEST_MAIN(UpdaterTest)

#include "updatertest.moc"
