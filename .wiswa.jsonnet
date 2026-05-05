local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  project_name: 'ipv6-config-update',
  version: '0.1.2',
  description: 'Update IPv6 CIDR in config files and restart relevant systemd units.',
  social+: {
    custom_badges: [
      '[![Tests](https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml)',
      '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/ipv6-config-update/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/ipv6-config-update?branch=master)',
    ],
  },
  keywords: ['linux', 'qt', 'cmake', 'systemd', 'ipv6', 'configuration management'],
  clang_format_args: 'src/*.cpp src/*.h src/autotests/*.cpp',
  want_codeql: false,
  want_tests: false,
  package_json+: {
    cspell+: {
      ignorePaths+: ['*.tags'],
    },
  },
  vscode+: {
    c_cpp+: {
      configurations: [
        {
          cStandard: 'gnu23',
          compilerPath: '/usr/bin/gcc',
          cppStandard: 'gnu++23',
          includePath: [
            '${workspaceFolder}/build/src',
            '${workspaceFolder}/build/src/generated',
            '${workspaceFolder}/src/**',
            '/usr/include/qt6',
            '/usr/include/qt6/QtCore',
          ],
          name: 'Linux',
        },
      ],
    },
    settings+: {
      'cmake.configureArgs': ['-DBUILD_TESTS=ON', '-DCOVERAGE=ON'],
    },
  },
  prettierignore+: ['*.service', '*.tags', '*.timer'],
  cz+: {
    commitizen+: {
      version_files+: [
        'man/ipv6-config-update.1',
      ],
    },
  },
  // C++ only
  cmake+: {
    uses_qt: true,
  },
  project_type: 'c++',
  vcpkg+: {
    dependencies: [
      {
        name: 'ecm',
        'version>=': '6.7.0',
      },
      {
        name: 'qtbase',
        'version>=': '6.8.3',
      },
    ],
  },
}
