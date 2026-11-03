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
  want_codeql: true,
  want_tests: false,
  want_winget: false,
  github+: {
    codeql+: {
      languages: ['actions'],
      runs_on: 'ubuntu-latest',
      workflow_file: 'codeql-actions.yml',
    },
    zizmor: {
      rules: {
        // The hand-maintained Release workflow needs workflow_run to gate the draft release.
        'dangerous-triggers': {
          ignore: ['release.yml'],
        },
        'undocumented-permissions': {
          disable: true,
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
+ {
  want_scan_build: true,
  github+: { workflows+: {
    scan_build+: {
      checkout_options: { 'fetch-depth': 0 },
      configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug',
      documentation_steps: [
        {
          name: 'Install dependencies',
          run: |||
            sudo apt-get update
            sudo apt-get install -y doxygen graphviz
          |||,
        },
        {
          name: 'Configure',
          run: 'cmake -DCMAKE_BUILD_TYPE=Release -B "${GITHUB_WORKSPACE}/build" --install-prefix "${GITHUB_WORKSPACE}/build/prefix" -DBUILD_DOCS_ONLY=ON',
        },
        {
          name: 'Build documentation',
          run: 'cmake --build "${GITHUB_WORKSPACE}/build" --verbose --config Release --target docs',
        },
      ],
      env: {},
      setup_steps: [
        {
          name: 'Install build dependencies',
          run: |||
            sudo apt-get update
            sudo apt-get install -y extra-cmake-modules libsystemd-dev qt6-base-dev
          |||,
        },
        {
          name: 'Install current CMake',
          run: 'pipx install cmake',
        },
      ],
      workflow_file: 'pages.yml',
    },
  } },
}
