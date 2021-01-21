#!/bin/sh

source_path=$(pwd)/download
build_path=$(pwd)/build

build_Bento4(){
  echo "build Bento4 start"
  library_source_path=$source_path/Bento4
  library_build_path=$build_path/Bento4
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/axiomatic-systems/Bento4/archive/v1.5.1-629.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  cp -fr ../../patch/Bento4/CMakeLists.txt ./
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path -DCMAKE_BUILD_TYPE=Release $library_source_path
  make ap4
  make ap4_a
  make install
  echo "build Bento4 end"
}

build_gpac(){
  echo "build gpac start"
  library_source_path=$source_path/gpac
  library_build_path=$build_path/gpac
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/gpac/gpac/archive/v0.8.0.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  ./configure --prefix=$library_build_path --use-zlib=no && make && make install-lib
  echo "build gpac end"
}

build_openssl(){
  echo "build openssl start"
  library_source_path=$source_path/openssl
  library_build_path=$build_path/openssl
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/openssl/openssl/archive/OpenSSL_1_1_1d.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  ./config --prefix=$library_build_path && make && make install_sw
  echo "build openssl end"
}

build_rapidjson(){
  echo "build rapidjson start"
  library_source_path=$source_path/rapidjson
  library_build_path=$build_path/rapidjson
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF $library_source_path
  make && make install
  echo "build rapidjson end"
}

build_easyloggingpp(){
  echo "build easyloggingpp start"
  library_source_path=$source_path/easyloggingpp
  library_build_path=$build_path/easyloggingpp
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/amrayn/easyloggingpp/archive/v9.96.7.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  cp -fr ../../patch/easyloggingpp/CMakeLists.txt ./
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path -Dbuild_static_lib=ON -Dbuild_shared_lib=ON $library_source_path
  make && make install
  echo "build easyloggingpp end"
}

build_jwtcpp(){
  echo "build jwt-cpp start"
  library_source_path=$source_path/jwt-cpp
  library_build_path=$build_path/jwt-cpp
  mkdir -p $library_build_path
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/Thalhammer/jwt-cpp/archive/v0.3.1.tar.gz | tar -xz --strip-components=1
  echo "build jwt-cpp end"
}

build_libuv(){
  echo "build libuv start"
  library_source_path=$source_path/libuv
  library_build_path=$build_path/libuv
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/libuv/libuv/archive/v1.40.0.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path $library_source_path
  make && make install
  echo "build libuv end"
}

build_uvcpp(){
  echo "build uvcpp start"
  library_source_path=$source_path/uv-cpp
  library_build_path=$build_path/uv-cpp
  DIR=$library_source_path && mkdir -p ${DIR} && cd ${DIR} && \
  curl -sLf  https://github.com/wlgq2/uv-cpp/archive/1.5.4.tar.gz | tar -xz --strip-components=1
  mkdir -p $library_build_path
  cd $library_source_path
  cp -fr ../../patch/uv-cpp/CMakeLists.txt ./
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path $library_source_path
  make && make install
  echo "build uvcpp end"
}

main(){
  build_Bento4
  #build_gpac
  build_openssl
  build_rapidjson
  build_easyloggingpp
  #build_jwtcpp
  build_libuv
  build_uvcpp
}

main