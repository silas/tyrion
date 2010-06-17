import os
import platform

#
# Base settings
#

defines = []
flags = ''
frameworks = []
libraries = ['txmpp']
name = 'tyrion'
system = platform.system().lower()

#
# Build options
#

AddOption(
    '--flags',
    dest='flags',
    type='string',
    nargs=1,
    action='store',
    default='',
    metavar='DIR',
    help='compiler flags',
)

AddOption(
    '--with-debug',
    dest='debug',
    action='store_true',
)

AddOption(
    '--with-static',
    dest='static',
    action='store_true',
)

#
# Helper functions
#

def Abort(message):
    print message
    Exit(1)

def SetupEnvironment(env):
    # Include paths
    path_list = []
    if 'C_INCLUDE_PATH' in os.environ:
        path_list.extend(os.environ['C_INCLUDE_PATH'].split(':'))
    if 'CPLUS_INCLUDE_PATH' in os.environ:
        path_list.extend(os.environ['CPLUS_INCLUDE_PATH'].split(':'))
    env.Append(CPPPATH=path_list)
    # Library paths
    path_list = []
    if 'LIBRARY_PATH' in os.environ:
        path_list.extend(os.environ['LIBRARY_PATH'].split(':'))
    env.Append(LIBPATH=path_list)

#
# Setup environment
#

if GetOption('debug'):
    flags += ' -g'

if GetOption('flags'):
    flags += GetOption('flags')

if system == 'linux':
    defines += ['LINUX']
elif system == 'darwin':
    defines += ['OSX']

if 'POSIX' in defines:
    flags += ' -pthread'

env = Environment(
    CXXFLAGS=flags,
    FRAMEWORKS=frameworks,
)

SetupEnvironment(env)

#
# Configure environment
#

conf = Configure(env)

if not conf.CheckCXXHeader('pthread.h'):
    Abort('pthread.h is required.')
if not conf.CheckCXXHeader('txmpp/config.h'):
    Abort('The txmpp development library is required.')

env = conf.Finish()

#
# Built library
#

library_src = [
    'src/config.cc',
    'src/constants.cc',
    'src/logging.cc',
    'src/process.cc',
    'src/third_party/inih/ini.c',
    'src/utils.cc',
]

if GetOption('static'):
    tyrion_library = env.StaticLibrary(
        'tyrion',
        library_src,
        CPPDEFINES=defines,
        LIBS=libraries,
    )
else:
    tyrion_library = env.SharedLibrary(
        'tyrion',
        library_src,
        CPPDEFINES=defines,
        LIBS=libraries,
    )

libraries += [tyrion_library]

#
# Build application
#

node_src = [
    'src/node.cc',
    'src/node_stanza.cc',
    'src/node_utils.cc',
    'src/xmpppump.cc',
    'src/xmpptasks.cc',
    'src/xmppthread.cc',
]

tyrion_node = env.Program(
    target='tyrion-node',
    source=node_src,
    CPPDEFINES=defines,
    LIBS=libraries,
)
