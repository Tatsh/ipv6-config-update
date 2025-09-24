/**
 * @file cidr.h
 * Provides CIDR (Classless Inter-Domain Routing) value representation and utilities.
 *
 * This header defines the Cidr namespace, which contains the Value class for representing
 * CIDR values, as well as utility functions for converting and obtaining CIDR information
 * from network interfaces.
 */

#pragma once

#include <QtCore/QString>
#include <QtNetwork/QNetworkInterface>

#include "constants.h"
#include "networkinfo.h"

namespace Cidr {

    /** The Value class encapsulates a CIDR string and a validity flag. */
    class Value {
    public:
        /**
        * Constructs an invalid CIDR value.
        * @param isValid Indicates if the value is valid (default: false).
        */
        Value(bool isValid = false) : m_isValid(isValid) {};

        /**
        * Constructs a CIDR value from a string and validity flag.
        * @param string The CIDR string representation.
        * @param isValid Indicates if the value is valid.
        */
        explicit Value(QString string, bool isValid) : m_string(string), m_isValid(isValid) {};

        /**
        * Checks if the CIDR value is valid.
        * @return True if valid, false otherwise.
        */
        bool isValid() const {
            return m_isValid;
        }

        /**
        * Equality operator.
        * @param b The other CIDR value to compare.
        * @return True if both values have the same string representation.
        */
        bool operator==(const Value &b) const {
            return m_string == b.m_string;
        }

        /**
        * Inequality operator.
        * @param b The other CIDR value to compare.
        * @return True if both values have different string representations.
        */
        bool operator!=(const Value &b) const {
            return m_string != b.m_string;
        }

        /**
        * Gets the CIDR string representation.
        * @return The CIDR string.
        */
        QString string() const {
            return m_string;
        }

    private:
        QString m_string;
        bool m_isValid;
    };

    /**
    * Outputs the CIDR value to a QDebug stream.
    *
    * @param debug The QDebug stream.
    * @param v The CIDR value to output.
    * @return The QDebug stream.
    */
    QDebug operator<<(QDebug debug, const Cidr::Value &v);

    /**
    * Converts an IPv6 address and prefix length to a CIDR value.
    *
    * @param prefixLength The prefix length.
    * @param val_ The IPv6 address.
    * @return The corresponding CIDR value.
    */
    Value toCidr(int prefixLength, const Q_IPV6ADDR &val_);

    /**
    * Gets the current CIDR value for a network interface.
    *
    * @param interfaceName_ The name of the network interface.
    * @param prefixLength The prefix length.
    * @param networkInfo The network information provider.
    * @return The current CIDR value for the interface.
    */
    Value current(const QString &interfaceName_,
                  uint prefixLength,
                  const AbstractNetworkInfo &networkInfo);

} // namespace Cidr
