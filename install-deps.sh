#!/usr/bin/env sh
set -o errexit

SYSC='systemc-2.3.3'
sysc_url='https://www.accellera.org/images/downloads/standards/systemc/'

SSTCORE='sstcore-8.0.0'
SSTELEM='sstelements-8.0.0'
sst_core_url='https://github.com/sstsimulator/sst-core/releases/download/v8.0.0_Final/'
sst_elem_url='https://github.com/sstsimulator/sst-elements/releases/download/v8.0.0_Final/'

archive_dir='archives'
mkdir -p ${archive_dir}

curl -kL ${sysc_url}${SYSC}.tar.gz | tar xz -C ${archive_dir}
curl -L ${sst_core_url}${SSTCORE}.tar.gz | tar xz -C ${archive_dir}
curl -L ${sst_elem_url}${SSTELEM}.tar.gz | tar xz -C ${archive_dir}

mv ${archive_dir}/sst-elements* ${archive_dir}/${SSTELEM}
mkdir -p ${archive_dir}/${SYSC}/build
mkdir -p ~/.sst
touch ~/.sst/sstsimulator.conf
BASE_DIR=$(pwd)
cmake --version
# cd ${archive_dir}/${SYSC}/build && cmake -DCMAKE_CXX_STANDARD=11 .. && make -j2 && sudo make install
# cd ${BASE_DIR}
# cd ${archive_dir}/${SSTCORE} && ./configure && make -j2 all && sudo make install
# cd ${BASE_DIR}
# cd ${archive_dir}/${SSTELEM} && ./configure && make -j2 all && sudo make install
# cd ${BASE_DIR}

# MSGPACK='msgpack-3.1.1'
# msgpack_url='https://github.com/msgpack/msgpack-c/releases/download/cpp-3.1.1/'

# curl -L ${msgpack_url}${MSGPACK}.tar.gz | tar xz -C ${archive_dir}
# cd ${archive_dir}/${MSGPACK} && cmake -DMSGPACK_CXX11=ON . && sudo make install
# cd ${BASE_DIR}

# https://github.com/Kitware/CMake/releases/download/v3.14.0/cmake-3.14.0.tar.gz