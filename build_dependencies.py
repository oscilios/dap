#! /usr/bin/env python

import os, sys
import subprocess

def usage(progname):
    print progname + ' <source_dir> <destination_dir> <build_type>'

if __name__ == '__main__':
    print 'received ' + ' '.join(sys.argv)
    if len(sys.argv) < 3:
        usage(sys.argv[0])
        sys.exit(-1)
    srcdir = sys.argv[1]
    destdir = sys.argv[2]
    build_type = "Debug"
    if len(sys.argv) > 3 :
        build_type = sys.argv[3]
    if not os.path.isdir(destdir):
        os.mkdir(destdir)
    os.chdir(destdir)
    if not os.path.exists('install'):
        os.mkdir('install')
        os.mkdir('install/include')
        os.mkdir('install/lib')
        os.mkdir('install/bin')
    cmake_config_cmd = ['cmake', srcdir, '-DCMAKE_BUILD_TYPE=%s'%build_type]
    cmake_build_cmd = ['cmake', '--build', destdir, '--config', build_type]

    # Configure dependencies project
    subprocess.call(cmake_config_cmd)

    # Build dependencies project
    subprocess.call(cmake_build_cmd)
