library_source = [
    'src/config.cc',
    'src/level.cc',
    'src/logging.cc',
    'src/process.cc',
    'src/setting.cc',
    'src/third_party/inih/ini.c',
    'src/utils.cc',
    'src/setting_validator.cc',
    'src/xmpp.cc',
    'src/xmpp_service.cc',
    'src/xmpp_service_manager.cc',
]

client_source = [
    'src/client.cc',
    'src/client_xmpp.cc',
    'src/client_xmpp_service_manager.cc',
]

node_source = [
    'src/node.cc',
    'src/node_acl.cc',
    'src/node_main.cc',
    'src/node_service_handler.cc',
    'src/node_setting_validator.cc',
    'src/node_xmpp.cc',
    'src/node_xmpp_service_manager.cc',
]

library_list = ['gloox', 'pthread']

AddOption('--prefix',
          dest='prefix',
          type='string',
          nargs=1,
          action='store',
          default='',
          metavar='DIR',
          help='installation prefix')

AddOption('--libdir',
          dest='libdir',
          type='string',
          nargs=1,
          action='store',
          default='/usr/local/lib',
          metavar='DIR',
          help='shared libraries path')

AddOption('--bindir',
          dest='bindir',
          type='string',
          nargs=1,
          action='store',
          default='/usr/local/bin',
          metavar='DIR',
          help='user binary path')

AddOption('--sbindir',
          dest='sbindir',
          type='string',
          nargs=1,
          action='store',
          default='/usr/local/bin',
          metavar='DIR',
          help='system binary path')

AddOption('--static',
          dest='static',
          action='store_true')

AddOption('--install',
          dest='install',
          action='store_true')

# Helper functions

def Abort(message):
    print message
    Exit(1)

# Setup environment

env = Environment()

# Configure environment

conf = Configure(env)

if not conf.CheckCXXHeader('gloox/gloox.h'):
    Abort('The gloox development library is required.')
if not conf.CheckCXXHeader('pthread.h'):
    Abort('pthread.h is required.')

env = conf.Finish()

# Built library

if GetOption('static'):
    tyrion_library = env.StaticLibrary('tyrion', library_source, LIBS=library_list)
else:
    tyrion_library = env.SharedLibrary('tyrion', library_source, LIBS=library_list)

library_list += [tyrion_library]

# Build applications

tyrion = env.Program(target='tyrion', source=client_source, LIBS=library_list)
tyrion_node = env.Program(target='tyrion-node', source=node_source, LIBS=library_list)

# Install

if GetOption('install'):
    prefix = GetOption('prefix')
    libdir = prefix + GetOption('libdir')
    bindir = prefix + GetOption('bindir')
    sbindir = prefix + GetOption('sbindir')

    if not GetOption('static'):
        Default(env.Install(libdir, tyrion_library))

    Default(env.Install(bindir, tyrion))
    Default(env.Install(sbindir, tyrion_node))
