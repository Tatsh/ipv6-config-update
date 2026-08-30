local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  project_name: 'ipv6-config-update',
  version: '0.1.2',
  description: 'Update IPv6 CIDR in config files and restart relevant systemd units.',
  custom_project_badges: [
    {
      anchor: '[![Tests](https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml/badge.svg)]',
      href: 'https://github.com/Tatsh/ipv6-config-update/actions/workflows/tests.yml',
    },
    {
      anchor: '[![Coverage Status](https://coveralls.io/repos/github/Tatsh/ipv6-config-update/badge.svg?branch=master)]',
      href: 'https://coveralls.io/github/Tatsh/ipv6-config-update?branch=master',
    },
  ],
  keywords: ['linux', 'qt', 'cmake', 'systemd', 'ipv6', 'configuration management'],
  clang_format_args: 'src/*.cpp src/*.h src/autotests/*.cpp',
  want_codeql: false,
  want_tests: false,
  want_winget: false,
  github+: {
    zizmor: {
      rules: {
        // The hand-maintained Release workflow needs workflow_run to gate the draft release.
        'dangerous-triggers': {
          ignore: ['release.yml'],
        },
      },
    },
  },
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
        'version>=': utils.latestVcpkgPortVersion('ecm'),
      },
      {
        name: 'qtbase',
        'version>=': utils.latestVcpkgPortVersion('qtbase'),
      },
    ],
  },
}
