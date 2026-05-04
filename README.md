# ipv6-config-update

<!-- WISWA-GENERATED-README:START -->

[![C++](https://img.shields.io/badge/C++-00599C?logo=c%2B%2B)](https://isocpp.org)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/ipv6-config-update)](https://github.com/Tatsh/ipv6-config-update/tags)
[![License](https://img.shields.io/github/license/Tatsh/ipv6-config-update)](https://github.com/Tatsh/ipv6-config-update/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/ipv6-config-update/v0.1.2/master)](https://github.com/Tatsh/ipv6-config-update/compare/v0.1.2...master)
[![Dependabot](https://img.shields.io/badge/Dependabot-enabled-blue?logo=dependabot)](https://github.com/dependabot)
[![GitHub Pages](https://github.com/Tatsh/ipv6-config-update/actions/workflows/pages.yml/badge.svg)](https://tatsh.github.io/ipv6-config-update/)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/ipv6-config-update?logo=github&style=flat)](https://github.com/Tatsh/ipv6-config-update/stargazers)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![CMake](https://img.shields.io/badge/CMake-6E6E6E?logo=cmake)](https://cmake.org/)
[![Prettier](https://img.shields.io/badge/Prettier-black?logo=prettier)](https://prettier.io/)

[![Qt](https://img.shields.io/badge/Qt-41cd52?logo=qt)](https://www.qt.io/)
[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor=did%3Aplc%3Auq42idtvuccnmtl57nsucz72&query=%24.followersCount&label=Follow+%40Tatsh&logo=bluesky&style=social)](https://bsky.app/profile/Tatsh.bsky.social)
[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-Tatsh-black?logo=buymeacoffee)](https://buymeacoffee.com/Tatsh)
[![Libera.Chat](https://img.shields.io/badge/Libera.Chat-Tatsh-black?logo=liberadotchat)](irc://irc.libera.chat/Tatsh)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)
[![Patreon](https://img.shields.io/badge/Patreon-Tatsh2-F96854?logo=patreon)](https://www.patreon.com/Tatsh2)

<!-- WISWA-GENERATED-README:STOP -->

This program updates configuration files which contain a CIDR of your IPv6 network. An example
would be `smb.conf` under the `hosts allow` option. If an update is made on this file, you may want
to reload the `smb` unit. This application is fully controlled by settings.

You may want this to run periodically if your ISP uses DHCPv6 and may change the IP randomly
(or when the lease expires).

## Settings

Located in XDG location such as `~/.config/Tatsh/ipv6-config-update.conf`. If running as root,
`/etc/ipv6-config-update.conf` will be used.

```ini
[main]
interface = interface name to use
files = file, paths, to, update
prefixLength = 56
units = systemd, units, to, restart, like, smb.service
```

## Limitations

Only multiple of eight prefix lengths are supported.

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
