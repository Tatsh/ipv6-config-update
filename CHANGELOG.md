<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.2] - 2026-05-04

### Added

- CMake CI workflow adapted from bpmdetect for Linux builds.

### Changed

- Adjusted installation paths for documentation and the man page in CMake.
- Updated development dependencies, including cspell, markdownlint-cli2, prettier, and
  prettier-plugin-sort-json.
- Updated GitHub Actions, including `actions/checkout`, `actions/configure-pages`,
  `actions/deploy-pages`, `actions/upload-artifact`, `actions/upload-pages-artifact`,
  `actions/attest-build-provenance`, `github/codeql-action`, and `softprops/action-gh-release`.

### Fixed

- Fixed an invalid `Version` key in the generated desktop file produced by the CMake workflow.
- Minor README corrections.

### Removed

- Removed the obsolete QA badge from the README.

## [0.1.1]

### Added

- API documentation.

## [0.1.0] - 2025-08-08

### Added

- Option to install documentation.
- More log messages.

### Fixed

- Fixed reading configuration from `/etc`.
- Fixed installation of systemd units.

## [0.0.1] - 2025-00-00

First version.

[unreleased]: https://github.com/Tatsh/ipv6-config-update/compare/v0.1.1...HEAD
[0.1.1]: https://github.com/Tatsh/ipv6-config-update/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/Tatsh/ipv6-config-update/compare/v0.0.1...v0.1.0
[0.0.1]: https://github.com/Tatsh/ipv6-config-update/releases/tag/v0.0.1
