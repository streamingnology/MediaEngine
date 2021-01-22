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
  ./Configure darwin64-x86_64-cc enable-ec_nistp_64_gcc_128 no-ssl2 no-ssl3 no-comp --prefix=$library_build_path && make && make install_sw
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

PREFIX=$build_path/ffmpeg
TEMP_PATH=$source_path/ffmpeg

OPUS_VERSION=1.1.3
X264_VERSION=20190513-2245-stable
X265_VERSION=3.2.1
VPX_VERSION=1.7.0
FDKAAC_VERSION=0.1.5
FFMPEG_VERSION=3.4

if [[ "$OSTYPE" == "darwin"* ]]; then
    NCPU=$(sysctl -n hw.ncpu)
    OSNAME=$(sw_vers -productName)
    OSVERSION=$(sw_vers -productVersion)
else
    NCPU=$(nproc)

    # CentOS, Fedora
    if [ -f /etc/redhat-release ]; then
        OSNAME=$(cat /etc/redhat-release |awk '{print $1}')
        OSVERSION=$(cat /etc/redhat-release |sed s/.*release\ // |sed s/\ .*// | cut -d"." -f1)
    # Ubuntu
    elif [ -f /etc/os-release ]; then
        OSNAME=$(cat /etc/os-release | grep "^NAME" | tr -d "\"" | cut -d"=" -f2)
        OSVERSION=$(cat /etc/os-release | grep ^VERSION= | tr -d "\"" | cut -d"=" -f2 | cut -d"." -f1 | awk '{print  $1}')
    fi
fi
MAKEFLAGS="${MAKEFLAGS} -j${NCPU}"
CURRENT=$(pwd)

fail_exit()
{
    echo "($1) installation has failed."
    cd
    exit 1
}

install_libopus()
{
    (DIR=${TEMP_PATH}/opus && \
    mkdir -p ${DIR} && \
    cd ${DIR} && \
    curl -sLf https://archive.mozilla.org/pub/opus/opus-${OPUS_VERSION}.tar.gz | tar -xz --strip-components=1 && \
    autoreconf -fiv && \
    ./configure --prefix="${PREFIX}" --enable-shared --disable-static && \
    make -j$(nproc) && \
    make install && \
    rm -rf ${PREFIX}/share && \
    rm -rf ${DIR}) || fail_exit "opus"
}

install_libx264()
{
    (DIR=${TEMP_PATH}/x264 && \
    mkdir -p ${DIR} && \
    cd ${DIR} && \
    curl -sLf https://download.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-${X264_VERSION}.tar.bz2 | tar -jx --strip-components=1 && \
  ./configure --prefix="${PREFIX}" --enable-shared --enable-pic --disable-cli && \
    make -j$(nproc) && \
    make install && \
    rm -rf ${DIR}) || fail_exit "x264"
}

install_libx265()
{
    (DIR=${TEMP_PATH}/x265 && \
    mkdir -p ${DIR} && \
    cd ${DIR} && \
    curl -sLf  https://get.videolan.org/x265/x265_${X265_VERSION}.tar.gz | tar -xz --strip-components=1 && \
    cd ${DIR}/build/linux && \
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="${PREFIX}" -DENABLE_SHARED:bool=on ../../source && \
    make -j$(nproc) && \
    make install && \
    rm -rf ${DIR}) || fail_exit "x265"
}

install_fdk_aac()
{
    (DIR=${TEMP_PATH}/aac && \
    mkdir -p ${DIR} && \
    cd ${DIR} && \
    curl -sLf https://github.com/mstorsjo/fdk-aac/archive/v${FDKAAC_VERSION}.tar.gz | tar -xz --strip-components=1 && \
    autoreconf -fiv && \
    ./configure --prefix="${PREFIX}" --enable-shared --disable-static --datadir=/tmp/aac && \
    make -j$(nproc) && \
    make install && \
    rm -rf ${DIR}) || fail_exit "fdk_aac"
}

install_ffmpeg()
{
    (DIR=${TEMP_PATH}/ffmpeg && \
    mkdir -p ${DIR} && \
    cd ${DIR} && \
    curl -sLf https://github.com/AirenSoft/FFmpeg/archive/ome/${FFMPEG_VERSION}.tar.gz | tar -xz --strip-components=1 && \
    PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH} ./configure \
    --prefix="${PREFIX}" \
    --enable-gpl \
    --enable-nonfree \
    --extra-cflags="-I${PREFIX}/include"  \
    --extra-ldflags="-L${PREFIX}/lib -Wl,-rpath,${PREFIX}/lib" \
    --extra-libs=-ldl \
    --enable-shared \
    --disable-static \
    --disable-debug \
    --disable-doc \
    --disable-programs  \
    --disable-avdevice --disable-dct --disable-dwt --disable-lsp --disable-lzo --disable-rdft --disable-faan --disable-pixelutils\
    --enable-zlib --enable-libfdk_aac --enable-libx264 --enable-libx265 \
    --disable-everything \
    --enable-encoder=libopus,libfdk_aac,libx264,libx265,mjpeg,png \
    --enable-decoder=aac,aac_latm,aac_fixed,h264,hevc \
    --enable-parser=aac,aac_latm,aac_fixed,h264,hevc \
    --enable-network --enable-protocol=tcp --enable-protocol=udp --enable-protocol=rtp,file,rtmp --enable-demuxer=rtsp --enable-muxer=mp4,webm,mpegts,flv,mpjpeg \
    --enable-bsf=aac_adtstoasc,imx_dump_header,null,chomp,mjpeg2jpeg,remove_extradata,dca_core,mjpega_dump_header,text2movsub,dump_extradata,h264_mp4toannexb,mpeg4_unpack_bframes,vp9_superframe_split,hevc_mp4toannexb,noise\
    --enable-filter=asetnsamples,aresample,aformat,channelmap,channelsplit,scale,transpose,fps,settb,asettb,format # && \
    make -j$(nproc) && \
    make install && \
    rm -rf ${PREFIX}/share && \
    rm -rf ${DIR}) || fail_exit "ffmpeg"
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

  install_libopus
  install_libx264
  install_libx265
  install_fdk_aac
  install_ffmpeg
}

main