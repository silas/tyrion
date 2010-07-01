import glob
import os
import platform
import shutil

#
# Command line options
#

AddOption(
    '--prefix',
    dest='prefix',
    type='string',
    nargs=1,
    action='store',
    default='/usr/local',
    metavar='DIR',
    help='The common installation prefix for all files.',
)

AddOption(
    '--libdir',
    dest='libdir',
    type='string',
    nargs=1,
    action='store',
    default='${PREFIX}/lib',
    metavar='DIR',
    help='The directory for installing object code libraries.',
)


AddOption(
    '--bindir',
    dest='bindir',
    type='string',
    nargs=1,
    action='store',
    default='${PREFIX}/bin',
    metavar='DIR',
    help='The directory for installing user binaries.',
)

AddOption(
    '--sbindir',
    dest='sbindir',
    type='string',
    nargs=1,
    action='store',
    default='${PREFIX}/sbin',
    metavar='DIR',
    help='The directory for installing system binaries.',
)

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
    '--install',
    dest='install',
    action='store_true',
)

AddOption(
    '--with-debug',
    dest='debug',
    action='store_true',
)

#
# Helper functions
#

def Abort(message):
    print message
    Exit(1)

def Copy(src, dst):
    print 'Copying %s to %s' % (src, dst)
    shutil.copy2(src, dst)

def CreateDirectory(path):
    if not os.path.exists(path):
        print 'Creating directory %s' % path
        os.makedirs(path)

def Link(src, dst):
    print 'Linking %s to %s' % (src, dst)
    os.symlink(src, dst)

_push_pop = []

def Pushd(path):
    _push_pop.insert(0, os.getcwd())
    print 'Pushd to %s' % path
    os.chdir(path)

def Popd():
    if len(_push_pop) > 0:
        os.chdir(_push_pop[0])
        print 'Popd to %s' % _push_pop[0]
        del _push_pop[0]
    else:
        print 'Popd stack empty'

def Remove(path):
    for p in glob.glob(path):
        print 'Deleting %s' % p
        os.remove(p)

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

env = Environment()

#
# Default settings
#

defines = ['POSIX']
flags = '-Wall -pedantic'
frameworks = []
libraries = ['txmpp']
link = ''
name = 'tyrion'
prefix = GetOption('prefix')
system = platform.system().lower()
version = '0.0.1'

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

client_src = [
    'src/client.cc',
    'src/client_envelope.cc',
    'src/client_loop.cc',
    'src/client_request.cc',
    'src/client_utils.cc',
    'src/client_xmpp_pump.cc',
    'src/client_xmpp_service_task.cc',
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

#
# Apply various options
#

if GetOption('debug'):
    flags += ' -g'
    defines += ['_DEBUG']

if GetOption('flags'):
    flags += ' %s' % GetOption('flags')

if system == 'linux':
    defines += ['LINUX']
    soname = 'lib%s.so.%s' % (name, version)
    link += ' -Wl,-soname,%s' % soname
elif system == 'darwin':
    defines += ['OSX']
    soname = 'lib%s.dylib.%s' % (name, version)
    link += ' -compatibility_version %s' % version
    link += ' -current_version %s' % version

if 'POSIX' in defines:
    flags += ' -pthread'
    libraries += ['pthread']

if os.environ.get('NO_OPTIMIZATION') != '1':
    flags += ' -O2'

if GetOption('flags'):
    env.Append(CFLAGS=GetOption('flags'))
    env.Append(CXXFLAGS=GetOption('flags'))

env.Append(CFLAGS=flags)
env.Append(CXXFLAGS=flags)
env.Append(FRAMEWORKS=frameworks)

SetupEnvironment(env)

#
# Configure environment
#

conf = Configure(env)

if system in ('darwin', 'linux'):
    if not conf.CheckCXX():
        Abort('No C++ compiler found.')
    for library in libraries:
        if not conf.CheckLib(library):
            Abort('Unable to find library %s.' % name)

env = conf.Finish()

#
# Build library
#

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

if GetOption('install'):

    libdir = GetOption('libdir').replace('${PREFIX}', prefix)
    bindir = GetOption('bindir').replace('${PREFIX}', prefix)
    sbindir = GetOption('sbindir').replace('${PREFIX}', prefix)

    tyrion_library = str(tyrion_library[0])
    tyrion_client = str(tyrion_client[0])
    tyrion_node = str(tyrion_node[0])

    # Install library
    CreateDirectory(libdir)
    Remove(os.path.join(libdir, 'libtyrion.*'))
    Copy(tyrion_library, os.path.join(libdir, soname))
    Pushd(libdir)
    for x in range(1, 4):
        lname = '.'.join(soname.split('.')[:-x])
        Link(soname, lname)
    Popd()

    # Install client
    CreateDirectory(bindir)
    Remove(os.path.join(bindir, tyrion_client))
    Copy(tyrion_client, bindir)

    # Install node
    CreateDirectory(sbindir)
    Remove(os.path.join(sbindir, tyrion_node))
    Copy(tyrion_node, sbindir)
