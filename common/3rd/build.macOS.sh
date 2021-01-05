#!/bin/sh

source_path=$(pwd)/git-submodule
build_path=$(pwd)/build

build_Bento4(){
  echo "build Bento4 start"
  Bento4_path_source=$source_path/Bento4
  Bento4_path_build=$build_path/Bento4
  mkdir -p $Bento4_path_build
  cd $Bento4_path_source
  git checkout v1.5.1-629  
  cd $Bento4_path_build
  cmake -DCMAKE_BUILD_TYPE=Release $Bento4_path_source
  make ap4/fast
  echo "build Bento4 end"
}

build_gpac(){
  echo "build gpac start"
  gpac_source_path=$source_path/gpac
  gpac_build_path=$build_path/gpac
  mkdir -p $gpac_build_path
  cd $gpac_source_path
  git checkout v0.8.0
  ./configure --prefix=$gpac_build_path --use-zlib=no && make && make install-lib
  echo "build gpac end"
}

build_openssl(){
  echo "build openssl start"
  openssl_source_path=$source_path/openssl
  openssl_build_path=$build_path/openssl
  mkdir -p $openssl_build_path
  cd $openssl_source_path
  git checkout OpenSSL_1_1_1d
  ./Configure darwin64-x86_64-cc enable-ec_nistp_64_gcc_128 no-ssl2 no-ssl3 no-comp --prefix=$openssl_build_path && make && make install_sw
  echo "build openssl end"
}

build_rapidjson(){
  echo "build rapidjson start"
  rapidjson_source_path=$source_path/rapidjson
  rapidjson_build_path=$build_path/rapidjson
  mkdir -p $rapidjson_build_path
  cd $rapidjson_source_path
  git checkout v1.1.0
  cmake -DCMAKE_INSTALL_PREFIX=$rapidjson_build_path -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_TESTS=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF $rapidjson_source_path
  make && make install
  echo "build rapidjson end"
}

build_easyloggingpp(){
  echo "build easyloggingpp start"
  easyloggingpp_source_path=$source_path/easyloggingpp
  easyloggingpp_build_path=$build_path/easyloggingpp
  mkdir -p $easyloggingpp_build_path
  cd $easyloggingpp_source_path
  git checkout v9.96.7
  cmake -DCMAKE_INSTALL_PREFIX=$easyloggingpp_build_path -Dbuild_static_lib=ON $easyloggingpp_source_path
  make && make install
  echo "build easyloggingpp end"
}

build_jwtcpp(){
  echo "build jwt-cpp start"
  jwtcpp_source_path=$source_path/jwt-cpp
  jwtcpp_build_path=$build_path/jwt-cpp
  cd $jwtcpp_source_path
  git checkout v0.3.1
  echo "build jwt-cpp end"
}

build_boost(){
  #download *nix platform
  cd $build_path
  wget https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz
  rm -fr boost
  tar xzf boost_1_72_0.tar.gz
  mv boost_1_72_0 boost
}

build_sigcplusplus(){
  echo "configure sigc++ start"
  sigcplusplus_source_path=$source_path/libsigcplusplus
  git checkout 3.0.3
  echo "configure sigc++ end"
}

main(){
  build_Bento4
  build_gpac
  build_openssl
  build_rapidjson
  build_easyloggingpp
  build_jwtcpp
  build_boost
  build_sigcplusplus
}

main


