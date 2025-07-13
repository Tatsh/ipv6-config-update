local utils = import 'utils.libjsonnet';

(import 'defaults.libjsonnet') + {
  local top = self,
  // General settings

  // Shared
  github_username: 'Tatsh',
  security_policy_supported_versions: { '0.0.x': ':white_check_mark:' },
  authors: [
    {
      'family-names': 'Udvare',
      'given-names': 'Andrew',
      email: 'audvare@gmail.com',
      name: '%s %s' % [self['given-names'], self['family-names']],
    },
  ],
  project_name: 'ipv6-config-update',
  version: '0.0.1',
  description: 'Update IPv6 CIDR in config files and restart relevant systemd units.',
  keywords: ['linux', 'qt', 'cmake', 'systemd', 'ipv6', 'configuration management'],
  copilot: {
    intro: 'ipv6-config-update periodically checks if the IPv6 address has changed and updates configuration files and restarts services.',
  },
  social+: {
    mastodon+: { id: '109370961877277568' },
  },

  // GitHub
  github+: {
    funding+: {
      ko_fi: 'tatsh2',
      liberapay: 'tatsh2',
      patreon: 'tatsh2',
    },
  },

  // C++ only
  cmake+: {
    uses_qt: true,
  },
  project_type: 'c++',
  vcpkg+: {
    dependencies: [{
      name: 'ecm',
      'version>=': '6.7.0',
    }],
  },
}
