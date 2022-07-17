# ipv6-config-update

This program updates configuration files which contain a CIDR of your IPv6 network. An example
would be `smb.conf` under the `hosts allow` option. If an update is made on this file, you may want
to reload the `smb` unit. This application is fully controlled by settings which are located at
`/etc/ipv6-config-update.conf`.

You may want this to run periodically if your ISP uses DHCPv6 and may change the IP randomly
(or when the lease expires).

## Settings

```ini
interface=interface name to use
files=file, paths, to, update
units=systemd, units, to, restart
```

## Timer

This project contains a systemd timer which runs every 15 minutes.

## Building

You must have CMake, libsystemd, Qt Network and Qt DBus installed.

```shell
cd path-to-source
mkdir build
cd build
cmake ..
make
```

## Debugging

```shell
export "QT_LOGGING_RULES=*.debug=true;qt.*=false"
```
