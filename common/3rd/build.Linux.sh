#!/bin/sh

source_path=$(pwd)/git-submodule
build_path=$(pwd)/build

build_Bento4(){
  echo "build Bento4 start"
  library_source_path=$source_path/Bento4
  library_build_path=$build_path/Bento4
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout v1.5.1-629  
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
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout v0.8.0
  ./configure --prefix=$library_build_path --use-zlib=no && make && make install-lib
  echo "build gpac end"
}

build_openssl(){
  echo "build openssl start"
  library_source_path=$source_path/openssl
  library_build_path=$build_path/openssl
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout OpenSSL_1_1_1d
  ./config --prefix=$library_build_path && make && make install_sw
  echo "build openssl end"
}

build_rapidjson(){
  echo "build rapidjson start"
  library_source_path=$source_path/rapidjson
  library_build_path=$build_path/rapidjson
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout v1.1.0
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF $library_source_path
  make && make install
  echo "build rapidjson end"
}

build_easyloggingpp(){
  echo "build easyloggingpp start"
  library_source_path=$source_path/easyloggingpp
  library_build_path=$build_path/easyloggingpp
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout v9.96.7
  cp -fr ../../patch/easyloggingpp/CMakeLists.txt ./
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path -Dbuild_static_lib=ON -Dbuild_shared_lib=ON $library_source_path
  make && make install
  echo "build easyloggingpp end"
}

build_jwtcpp(){
  echo "build jwt-cpp start"
  library_source_path=$source_path/jwt-cpp
  library_build_path=$build_path/jwt-cpp
  cd $library_source_path
  git checkout v0.3.1
  echo "build jwt-cpp end"
}

build_libuv(){
  echo "build libuv start"
  library_source_path=$source_path/libuv
  library_build_path=$build_path/libuv
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout v1.40.0
  cmake -DCMAKE_INSTALL_PREFIX=$library_build_path $library_source_path
  make && make install
  echo "build libuv end"
}

build_uvcpp(){
  echo "build uvcpp start"
  library_source_path=$source_path/uv-cpp
  library_build_path=$build_path/uv-cpp
  mkdir -p $library_build_path
  cd $library_source_path
  git checkout 1.5.4
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