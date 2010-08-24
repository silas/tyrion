import glob
import os
import platform
import shutil
import subprocess

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
    '--with-tests',
    dest='tests',
    action='store_true',
)

AddOption(
    '--with-debug',
    dest='debug',
    action='store_true',
)

AddOption(
    '--with-devel',
    dest='devel',
    action='store_true',
)

def Abort(message):
    print message
    Exit(1)

def ChangeExt(src_list, ext):
    return map(lambda x: x[:-2] + ext, src_list)

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

env = Environment()

defines = ['POSIX']
flags = '-Wall -pedantic'
frameworks = []
libraries = ['txmpp']
name = 'tyrion'
prefix = GetOption('prefix')
system = platform.system().lower()

library_src = [
    'src/config.cc',
    'src/constants.cc',
    'src/envelope.cc',
    'src/logging.cc',
    'src/loop.cc',
    'src/main.cc',
    'src/process.cc',
    'src/service_handler.cc',
    'src/settings.cc',
    'src/third_party/inih/ini.c',
    'src/utils.cc',
    'src/xmpp_presence_task.cc',
    'src/xmpp_pump.cc',
    'src/xmpp_service_task.cc',
]

if GetOption('debug'):
    flags += ' -g'
    defines += ['_DEBUG']

if GetOption('flags'):
    flags += ' %s' % GetOption('flags')

if system == 'linux':
    defines += ['LINUX']
elif system == 'darwin':
    defines += ['OSX']
else:
    Abort('Unknown OS.')

if 'POSIX' in defines:
    flags += ' -pthread'
    libraries += ['pthread']

if not GetOption('devel'):
    flags += ' -O2'

env.Append(CFLAGS=flags)
env.Append(CXXFLAGS=flags)
env.Append(FRAMEWORKS=frameworks)

if GetOption('devel'):
    env.Append(CXXFLAGS=' -Weffc++')

SetupEnvironment(env)

conf = Configure(env)

if system in ('darwin', 'linux'):
    if not conf.CheckCXX():
        Abort('No C++ compiler found.')
    for library in libraries:
        if not conf.CheckLib(library):
            Abort('Unable to find library %s.' % name)

env = conf.Finish()

tyrion = env.Program(
    target='tyrion',
    source=src,
    CPPDEFINES=defines,
    LIBS=libraries,
)

if GetOption('install'):
    sbindir = GetOption('sbindir').replace('${PREFIX}', prefix)
    tyrion = str(tyrion[0])
    CreateDirectory(sbindir)
    tyrion_path = os.path.join(sbindir, tyrion)
    Remove(tyrion_path)
    Copy(tyrion, tyrion_path)
