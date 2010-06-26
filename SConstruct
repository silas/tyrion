import os
import platform

#
# Base settings
#

defines = []
flags = '-Wall'
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
    flags += ' %s' % GetOption('flags')

if system == 'linux':
    defines += ['LINUX', 'POSIX']
elif system == 'darwin':
    defines += ['OSX', 'POSIX']

if 'POSIX' in defines:
    flags += ' -pthread'
    libraries += ['pthread']

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
    'src/envelope.cc',
    'src/logging.cc',
    'src/process.cc',
    'src/third_party/inih/ini.c',
    'src/utils.cc',
    'src/xmpp_presence_task.cc',
    'src/xmpp_pump.cc',
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


client_src = [
    'src/client.cc',
    'src/client_envelope.cc',
    'src/client_loop.cc',
    'src/client_utils.cc',
    'src/client_xmpp_pump.cc',
]

node_src = [
    'src/node.cc',
    'src/node_envelope.cc',
    'src/node_loop.cc',
    'src/node_service_handler.cc',
    'src/node_utils.cc',
    'src/node_xmpp_pump.cc',
    'src/node_xmpp_service_task.cc',
]

tyrion_client = env.Program(
    target='tyrion',
    source=client_src,
    CPPDEFINES=defines,
    LIBS=libraries,
)

tyrion_node = env.Program(
    target='tyrion-node',
    source=node_src,
    CPPDEFINES=defines,
    LIBS=libraries,
)
