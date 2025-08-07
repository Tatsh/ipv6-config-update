#include <QtCore/QDebug>
#include <QtNetwork/QNetworkInterface>
#include <QtTest/QTest>

#include "cidr.h"

class CidrTest : public QObject {
    Q_OBJECT

public:
    CidrTest(QObject *parent = nullptr);
    ~CidrTest() override;

private Q_SLOTS:
    void testIsValid();
    void testCurrentNotValid();
    void testCurrentValid();
    void testToCidr();
    void testOperatorLeftShift();
    void testOperatorEquals();
    void testOperatorNotEquals();
};

CidrTest::CidrTest(QObject *parent) : QObject(parent) {
    Q_UNUSED(parent);
}

CidrTest::~CidrTest() {
}

void CidrTest::testIsValid() {
    auto hostAddress = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr = Cidr::toCidr(64, hostAddress.toIPv6Address());
    QVERIFY(cidr.isValid());
}

class TestNetworkInfo : public AbstractNetworkInfo {
public:
    QList<QNetworkAddressEntry> addressEntries(const QString &name) const override {
        Q_UNUSED(name);
        // Mock implementation returning an empty list to simulate no valid entries
        return QList<QNetworkAddressEntry>();
    }
};

void CidrTest::testCurrentNotValid() {
    auto cidr = Cidr::current(QStringLiteral("eth0"), 64, TestNetworkInfo());
    QVERIFY(!cidr.isValid());
}

class TestNetworkValidInfo : public AbstractNetworkInfo {
public:
    QList<QNetworkAddressEntry> addressEntries(const QString &name) const override {
        Q_UNUSED(name);
        // Mock implementation returning a valid address entry
        QNetworkAddressEntry entry;
        entry.setIp(QHostAddress(QStringLiteral("2001:db8::1")));
        entry.setPrefixLength(64);
        return {entry};
    }
};

void CidrTest::testCurrentValid() {
    auto hostAddress = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr = Cidr::current(QStringLiteral("eth0"), 64, TestNetworkValidInfo());
    QVERIFY(cidr.isValid());
    QCOMPARE(cidr.string(), QStringLiteral("2001:db8::/64"));
}

void CidrTest::testToCidr() {
    auto hostAddress = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr = Cidr::toCidr(64, hostAddress.toIPv6Address());
    QCOMPARE(cidr.string(), QStringLiteral("2001:db8::/64"));
    QVERIFY(cidr.isValid());
}

void CidrTest::testOperatorLeftShift() {
    // Solely for coverage.
    auto hostAddress = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr = Cidr::toCidr(64, hostAddress.toIPv6Address());
    qDebug() << "CIDR:" << cidr;
}

void CidrTest::testOperatorEquals() {
    auto hostAddress1 = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr1 = Cidr::toCidr(64, hostAddress1.toIPv6Address());
    auto hostAddress2 = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr2 = Cidr::toCidr(64, hostAddress2.toIPv6Address());
    QVERIFY(cidr1 == cidr2);
}

void CidrTest::testOperatorNotEquals() {
    auto hostAddress1 = QHostAddress(QStringLiteral("2001:db8::1"));
    auto cidr1 = Cidr::toCidr(64, hostAddress1.toIPv6Address());
    auto hostAddress2 = QHostAddress(QStringLiteral("2001:db9::2"));
    auto cidr2 = Cidr::toCidr(64, hostAddress2.toIPv6Address());
    QVERIFY(cidr1 != cidr2);
}

QTEST_MAIN(CidrTest)

#include "cidrtest.moc"
