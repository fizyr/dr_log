# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## main
### Removed
- Removed optional ROS/log4cxx support.

## 0.3.3 - 2021-06-29
### Added
- Added dependency on Boost.

## 0.3.2 - 2021-03-08
### Fixed
- Removed duplicate source file.

## 0.3.1 - 2020-11-10
### Fixed
- Prevent output buffering from delaying log messages when stderr is redirected.

## 0.3.0 - 2020-11-02
### Added
- Systemd logging mode.

## 0.2.0 - 2020-05-29
### Changed
- Make ROS integration with log4cxx optional.
- Link with --as-needed.

## 0.1.3 - 2020-02-24
### Added
- Add environment flags for disabling syslog and console outputs (`DR_LOG_USE_SYSLOG` and `DR_LOG_USE_CONSOLE`).
