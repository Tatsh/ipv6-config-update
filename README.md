# ipv6-config-update

[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/ipv6-config-update)](https://github.com/Tatsh/ipv6-config-update/tags)
[![License](https://img.shields.io/github/license/Tatsh/ipv6-config-update)](https://github.com/Tatsh/ipv6-config-update/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/ipv6-config-update/v0.0.0/master)](https://github.com/Tatsh/ipv6-config-update/compare/v0.0.0...master)
[![CodeQL](https://github.com/Tatsh/ipv6-config-update/actions/workflows/codeql.yml/badge.svg)](https://github.com/Tatsh/ipv6-config-update/actions/workflows/codeql.yml)
[![QA](https://github.com/Tatsh/ipv6-config-update/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/ipv6-config-update/actions/workflows/qa.yml)
[![Tests](https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/ipv6-config-update/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/ipv6-config-update?branch=master)
[![GitHub Pages](https://github.com/Tatsh/ipv6-config-update/actions/workflows/pages/pages-build-deployment/badge.svg)](https://tatsh.github.io/ipv6-config-update/)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/ipv6-config-update?logo=github&style=flat)](https://github.com/Tatsh/ipv6-config-update/stargazers)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor%3Ddid%3Aplc%3Auq42idtvuccnmtl57nsucz72%26query%3D%24.followersCount%26style%3Dsocial%26logo%3Dbluesky%26label%3DFollow%2520%40Tatsh&query=%24.followersCount&style=social&logo=bluesky&label=Follow%20%40Tatsh)](https://bsky.app/profile/Tatsh.bsky.social)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)

This program updates configuration files which contain a CIDR of your IPv6 network. An example
would be `smb.conf` under the `hosts allow` option. If an update is made on this file, you may want
to reload the `smb` unit. This application is fully controlled by settings which are located at
`/etc/ipv6-config-update.conf`.

You may want this to run periodically if your ISP uses DHCPv6 and may change the IP randomly
(or when the lease expires).

## Settings

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
