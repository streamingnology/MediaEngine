/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */

#include "datastream.h"
#include "core/snyplatform.h"
#include <cassert>

#ifdef Q_OS_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#define _access access
#endif

namespace sny
{

IOStream::IOStream(ByteOrder byteOrder)
{
    sys_byte_order_ = IOStream::getSysByteOrder();
    byte_order_ = byteOrder;
    read_ok_ = false;
    write_ok_ = false;
}

IOStream::~IOStream()
{
}

IOStream::ByteOrder IOStream::getSysByteOrder()
{
    uint16_t value = 0x0102;
    uint8_t value0 = ((uint8_t *)&value)[0];

    ByteOrder byteOrder = IOStream::LittleEndian;
    if (value0 == 0x01)
        byteOrder = IOStream::BigEndian;
    else
        byteOrder = IOStream::LittleEndian;

    return byteOrder;
}

void IOStream::setLittleEndian()
{
    byte_order_ = IOStream::LittleEndian;
}

void IOStream::setBigEndian()
{
    byte_order_ = IOStream::BigEndian;
}

bool IOStream::isLittleEndian()
{
    return (byte_order_ == IOStream::LittleEndian);
}

bool IOStream::isBigEndian()
{
    return (byte_order_ == IOStream::BigEndian);
}

int64_t IOStream::remainSize() const
{
    int64_t nPos = pos();
    int64_t nSize = size();
    if ((nPos < 0) || (nSize < 0))
        return 0;
    return (nSize - nPos);
}

char *IOStream::data()
{
    return nullptr;
}

const char *IOStream::data() const
{
    return nullptr;
}

int64_t IOStream::write(IOStream &stream, const int64_t size)
{
    if (size <= 0)
        return 0;

    const char *dataBegin = stream.data();
    if (dataBegin != nullptr)
    {
        int64_t copySize = stream.size() - stream.pos();
        if (size < copySize)
            copySize = size;
        const char *data = dataBegin + stream.pos();
        return write(data, copySize);
    }
    else
    {
        char buffer[64 * 1024];
        int64_t dataLeft = size;
        int64_t readSize = 0;
        int64_t writeSize = 0;
        do
        {
            readSize = stream.read(buffer, dataLeft > 64 * 1024 ? 64 * 1024 : dataLeft);
            dataLeft -= readSize;
            if (readSize > 0)
            {
                writeSize = write(buffer, readSize);
            }
            else
            {
                break;
            }
        } while (dataLeft > 0 && readSize == writeSize);
        return size - dataLeft;
    }
}

bool IOStream::isReadOk()
{
    return read_ok_;
}

bool IOStream::isWriteOk()
{
    return write_ok_;
}

void IOStream::setReadOk(const bool ok)
{
    read_ok_ = ok;
}

void IOStream::setWriteOk(const bool ok)
{
    write_ok_ = ok;
}

bool IOStream::swapOrder(char *buffer, int64_t offset, int64_t count)
{
    if (count <= 1)
        return true;

    if ((offset < 0) || (offset >= count))
        return false;

    char *pointer = (char *)(buffer + offset);

    for (int64_t i = 0; i < count / 2; i++)
    {
        int n = (int)(count - i - 1);

        char temp = pointer[i];
        pointer[i] = pointer[n];
        pointer[n] = temp;
    }
    return true;
}

template <typename T> IOStream &IOStream::_read(T &v)
{
    read((char *)&v, sizeof(v));
    if (read_ok_)
    {
        if ((isBigEndian() && (sys_byte_order_ == IOStream::LittleEndian)) ||
            (isLittleEndian() && (sys_byte_order_ == IOStream::BigEndian)))
        {
            IOStream::swapOrder((char *)&v, 0, sizeof(v));
        }
    }
    return *this;
}

template <typename T> IOStream &IOStream::_write(T v)
{
    if ((isBigEndian() && (sys_byte_order_ == IOStream::LittleEndian)) ||
        (isLittleEndian() && (sys_byte_order_ == IOStream::BigEndian)))
    {
        IOStream::swapOrder((char *)&v, 0, sizeof(v));
    }
    write((const char *)&v, sizeof(v));
    return *this;
}

IOStream &IOStream::operator>>(bool &v)
{
    char tempC = 0;
    IOStream::_read(tempC);
    v = (tempC ? true : false);
    return *this;
}

IOStream &IOStream::operator>>(int8_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(uint8_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(int16_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(uint16_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(int32_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(uint32_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(int64_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(uint64_t &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(float &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(double &v)
{
    return IOStream::_read(v);
}

IOStream &IOStream::operator>>(std::string &v)
{
    uint32_t sSize = 0;
    (*this) >> sSize;

    assert(sSize <= 65536);

    char sBuffer[256];
    char *buffer = sBuffer;
    if (sSize > 256)
    {
        buffer = new char[sSize];
    }
    // memset(buffer, 0, sSize);
    read(buffer, sSize);
    if (isReadOk())
    {
        v = std::string(buffer, sSize);
    }
    if (buffer != sBuffer)
    {
        delete[] buffer;
    }

    return *this;
}

IOStream &IOStream::operator<<(const bool v)
{
    char tempC = (v ? 0xFF : 0);
    IOStream::_write(tempC);
    return *this;
}

IOStream &IOStream::operator<<(const int8_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const uint8_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const int16_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const uint16_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const int32_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const uint32_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const int64_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const uint64_t v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const float v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const double v)
{
    return IOStream::_write(v);
}

IOStream &IOStream::operator<<(const std::string &v)
{
    uint32_t sSize = (uint32_t)v.size();
    (*this) << sSize;
    write(v.data(), sSize);

    return *this;
}

IOStream &IOStream::writeBytes(const uint64_t v, const int size)
{
    assert((size > 0) && (size <= 8));
    uint64_t v64 = v;
    for (int i = 0; i < size; i++)
    {
        uint8_t v8 = (uint8_t)(v64 & 0xFF);
        IOStream::_write(v8);
        v64 >>= 8;
    }
    return *this;
}

IOStream &IOStream::readBytes(uint64_t &v, const int size)
{
    assert((size > 0) && (size <= 8));
    v = 0;
    uint8_t v8 = 0;
    for (int i = 0; i < size; i++)
    {
        v <<= 8;
        IOStream::_read(v8);
        v |= v8;
    }
    return *this;
}

IOStream &IOStream::writeBytes(const uint32_t v, const int size)
{
    assert((size > 0) && (size <= 4));
    uint32_t v32 = v;
    for (int i = 0; i < size; i++)
    {
        uint8_t v8 = (uint8_t)(v32 & 0xFF);
        IOStream::_write(v8);
        v32 >>= 8;
    }
    return *this;
}

IOStream &IOStream::readBytes(uint32_t &v, const int size)
{
    assert((size > 0) && (size <= 4));
    v = 0;
    uint8_t v8 = 0;
    uint32_t v32 = 0;
    for (int i = 0; i < size; i++)
    {
        IOStream::_read(v8);
        v32 = v8;
        v32 <<= (8 * i);
        v |= v32;
    }
    return *this;
}

IOStream &IOStream::readUInt24(int &v)
{
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    *this >> v8 >> v16;
    v = ((unsigned int)v8 << 16) | v16;
    return *this;
}

IOStream &IOStream::writeUInt24(const int v)
{
    uint16_t v16 = v & 0xFFFF;
    uint8_t v8 = v >> 16;
    *this << v8 << v16;
    return *this;
}

IOStream &IOStream::readInt24(int &v)
{
    uint8_t v8 = 0;
    uint16_t v16 = 0;
    *this >> v8 >> v16;
    v = ((int)v8 << 16) | v16;
    return *this;
}

IOStream &IOStream::writeInt24(const int v)
{
    uint16_t v16 = v & 0xFFFF;
    int8_t v8 = v >> 16;
    *this << v8 << v16;
    return *this;
}

MemIOStream::MemIOStream(ByteOrder byteOrder) : IOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    pos_ = 0;
    alloc_ = false;
    max_size_ = 0;
}

MemIOStream::MemIOStream(const char *buffer, int64_t size, ByteOrder byteOrder) : IOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    pos_ = 0;
    alloc_ = false;
    setBuffer(buffer, size);
}

MemIOStream::MemIOStream(int64_t size, ByteOrder byteOrder) : IOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    pos_ = 0;
    alloc_ = false;
    allocSize(size);
}

MemIOStream::MemIOStream(const MemIOStream &s)
{
    buffer_ = nullptr;
    size_ = s.size_;
    pos_ = s.pos_;
    alloc_ = false;
    max_size_ = s.max_size_;

    assert(size_ <= max_size_);

    if (s.buffer_ != nullptr)
    {
        buffer_ = MemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.buffer_, (size_t)size_);
            alloc_ = true;
        }
    }
}

MemIOStream::~MemIOStream()
{
    freeBuffer();
}

MemIOStream &MemIOStream::operator=(const MemIOStream &s)
{
    freeBuffer();

    buffer_ = nullptr;
    size_ = s.size_;
    pos_ = s.pos_;
    alloc_ = false;
    max_size_ = s.max_size_;

    assert(size_ <= max_size_);

    if (s.buffer_ != nullptr)
    {
        buffer_ = MemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.buffer_, (size_t)size_);
            alloc_ = true;
        }
    }

    return (*this);
}

char *MemIOStream::allocBufer(int64_t size)
{
    if (size <= 0)
        return nullptr;
    return (char *)malloc((size_t)size);
}

char *MemIOStream::reallocBuffer(char *buffer, int64_t size)
{
    if ((buffer == nullptr) || (size <= 0))
        return nullptr;
    return (char *)realloc(buffer, (size_t)size);
}

void MemIOStream::freeBuffer(char *buffer)
{
    if (buffer != nullptr)
    {
        free(buffer);
    }
}

void MemIOStream::freeBuffer()
{
    if ((buffer_ != nullptr) && alloc_)
    {
        MemIOStream::freeBuffer(buffer_);
    }
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    pos_ = 0;
    alloc_ = false;
}

void MemIOStream::allocSize(int64_t size)
{
    buffer_ = MemIOStream::allocBufer(size);
    if (buffer_ != nullptr)
    {
        size_ = size;
        max_size_ = size_;
        pos_ = 0;
        alloc_ = true;
    }
    else
    {
        buffer_ = nullptr;
        size_ = 0;
        max_size_ = 0;
        pos_ = 0;
        alloc_ = false;
    }
}

void MemIOStream::reallocSize(int64_t size)
{
    if (buffer_ == nullptr)
    {
        allocSize(size);
        return;
    }

    if (!alloc_)
        return;

    if (size <= max_size_)
    {
        if (size > size_)
        {
            size_ = size;
        }
        return;
    }

    char *pOldBuffer = buffer_;
    int64_t maxSize = size + 256;
    maxSize += (maxSize >> 3);
    if (maxSize < 4096)
    {
        maxSize = (maxSize + 255) & 0xFFFFFFFFFFFFFF00LL;
    }
    else
    {
        maxSize = (maxSize + 4095) & 0xFFFFFFFFFFFFFC00LL;
    }
    // maxSize = size + 512;
    buffer_ = MemIOStream::reallocBuffer(buffer_, maxSize);
    if (buffer_ == nullptr)
    {
        MemIOStream::freeBuffer(pOldBuffer);
        buffer_ = nullptr;
        size_ = 0;
        max_size_ = 0;
        pos_ = 0;
        alloc_ = false;
    }
    else
    {
        size_ = size;
        max_size_ = maxSize;
    }
}

void MemIOStream::setBuffer(const char *buffer, int64_t size)
{
    freeBuffer();
    buffer_ = (char *)buffer;
    size_ = size;
    max_size_ = size;
    alloc_ = false;
}

void MemIOStream::reserve(const int64_t size)
{
    int64_t s = size_;
    reallocSize(size);
    size_ = s;
}

void MemIOStream::resize(const int64_t size)
{
    reallocSize(size);
}

void MemIOStream::reset()
{
    size_ = 0;
    pos_ = 0;
}

int64_t MemIOStream::read(char *buffer, const int64_t size)
{
    if (size <= 0)
    {
        read_ok_ = true;
        return 0;
    }
    if ((buffer_ == nullptr) || (size_ <= 0))
    {
        read_ok_ = false;
        return -1;
    }
    if ((buffer == nullptr) || (size <= 0))
    {
        read_ok_ = false;
        return -1;
    }

    if (pos_ >= size_)
    {
        read_ok_ = false;
        return -1;
    }

    int64_t readLen = size;
    if (readLen > (size_ - pos_))
        readLen = (size_ - pos_);

    memcpy(buffer, buffer_ + pos_, (size_t)readLen);
    pos_ += readLen;

    read_ok_ = (size == readLen);

    return readLen;
}

int64_t MemIOStream::write(const char *buffer, const int64_t size)
{
    if (size <= 0)
    {
        write_ok_ = true;
        return 0;
    }
    if ((buffer == nullptr) || (size <= 0))
    {
        write_ok_ = false;
        return -1;
    }

    reallocSize(size + pos_);
    if ((buffer_ == nullptr) || ((size + pos_) > size_))
    {
        write_ok_ = false;
        return -1;
    }

    memcpy(buffer_ + pos_, buffer, (size_t)size);
    pos_ += size;

    write_ok_ = true;

    return size;
}

bool MemIOStream::seek(int64_t pos)
{
    if (pos < 0 || pos > size_)
        return false;
    pos_ = pos;
    return true;
}

bool MemIOStream::seekToBegin()
{
    pos_ = 0;
    return true;
}

bool MemIOStream::seekToEnd()
{
    pos_ = size_;
    return true;
}

bool MemIOStream::skip(int64_t count)
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return false;

    int64_t pos = pos_ + count;
    if (pos > size_)
    {
        pos_ = size_;
        return false;
    }
    pos_ = pos;
    return true;
}

int64_t MemIOStream::pos() const
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return 0;

    return pos_;
}

int64_t MemIOStream::size() const
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return 0;
    return size_;
}

char *MemIOStream::data()
{
    return buffer_;
}

const char *MemIOStream::data() const
{
    return buffer_;
}

bool MemIOStream::atEnd() const
{
    return (pos_ >= size_);
}

QueueIOStream::QueueIOStream(ByteOrder byteOrder) : MemIOStream(byteOrder)
{
}

QueueIOStream::QueueIOStream(const QueueIOStream &s) : MemIOStream(s)
{
}

QueueIOStream::~QueueIOStream()
{
}

int64_t QueueIOStream::push(const char *buffer, const int64_t size)
{
    if ((buffer == nullptr) && (size > 0))
    {
        assert(false);
        return -1;
    }
    const int64_t endPos = size_;
    reallocSize(size_ + size);
    if (buffer_ == nullptr)
        return -1;
    if (size > 0)
    {
        memcpy(buffer_ + endPos, buffer, (size_t)size);
    }
    return size;
}

int64_t QueueIOStream::cut(const int64_t size)
{
    if (size <= size_)
    {
        memmove(buffer_, buffer_ + size, (size_t)(size_ - size));
        size_ = (size_ - size);
        pos_ = ((pos_ - size) >= 0) ? (pos_ - size) : 0;
    }
    else
    {
        size_ = 0;
        pos_ = 0;
    }
    return size_;
}

int64_t QueueIOStream::cut()
{
    return cut(pos_);
}

QueueIOStream &QueueIOStream::operator=(const QueueIOStream &s)
{
    MemIOStream::operator=(s);
    return (*this);
}

FileIOStream::FileIOStream(ByteOrder byteOrder) : IOStream(byteOrder)
{
    file_ = nullptr;
}

FileIOStream::FileIOStream(const char *filename, FileIOStream::OpenMode openMode, ByteOrder byteOrder)
    : IOStream(byteOrder)
{
    file_ = nullptr;
    open(filename, openMode);
}

FileIOStream::~FileIOStream()
{
    close();
}

std::string _openMode2fopenMode(FileIOStream::OpenMode openMode, const char *fileName = nullptr)
{
    std::string sMode;
    if ((openMode & FileIOStream::ReadOnly) && !(openMode & FileIOStream::Truncate))
    {
        sMode = "rb";
        if (openMode & FileIOStream::WriteOnly)
        {
            if ((fileName != nullptr) && (_access(fileName, 0) != -1))
                sMode = "rb+";
            else
                sMode = "wb+";
        }
    }
    else if (openMode & FileIOStream::WriteOnly)
    {
        sMode = "wb";
        if (openMode & FileIOStream::ReadOnly)
            sMode += "+";
    }
    if (openMode & FileIOStream::Append)
    {
        sMode = "ab";
        if (openMode & FileIOStream::ReadOnly)
            sMode += "+";
    }
    return sMode;
}

bool FileIOStream::open(const char *filename, FileIOStream::OpenMode openMode)
{
    close();
    std::string mode = _openMode2fopenMode(openMode, filename);
#ifdef Q_OS_WINDOWS
    file_ = nullptr;
    fopen_s(&file_, filename, mode.c_str());
#else
    file_ = fopen(filename, mode.c_str());
#endif
    m_fileName = std::string(filename);
    return isOpen();
}

const std::string &FileIOStream::getFileName()
{
    return m_fileName;
}

bool FileIOStream::isOpen() const
{
    return (file_ != nullptr);
}

void FileIOStream::close()
{
    if (file_ != nullptr)
    {
        fclose(file_);
        file_ = nullptr;
    }
    m_fileName = "";
}

bool FileIOStream::flush()
{
    if (file_ == nullptr)
        return false;
    return (fflush(file_) == 0);
}

int64_t FileIOStream::read(char *buffer, const int64_t size)
{
    if (file_ == nullptr)
    {
        read_ok_ = false;
        return -1;
    }
    if (size <= 0)
    {
        read_ok_ = true;
        return 0;
    }
    int64_t readLen = fread(buffer, 1, (size_t)size, file_);
    read_ok_ = (readLen == size);
    return readLen;
}

int64_t FileIOStream::write(const char *buffer, const int64_t size)
{
    if (file_ == nullptr)
    {
        write_ok_ = false;
        return -1;
    }
    if (size <= 0)
    {
        write_ok_ = true;
        return 0;
    }
    int64_t writeLen = fwrite(buffer, 1, (size_t)size, file_);
    write_ok_ = (writeLen == size);
    return writeLen;
}

int64_t FileIOStream::fseek64Inner(FILE *stream, int64_t offset, int whence) const
{
    if (stream == nullptr)
        return -1;

    if (feof(stream))
    {
        rewind(stream);
    }
    else
    {
        setbuf(stream, nullptr);
    }

#if defined(Q_OS_WINDOWS)
    int fd = _fileno(stream);
    int64_t curpos = _lseeki64(fd, offset, whence);
#else
    int fd = fileno(stream);
    int64_t curpos = lseek64(fd, offset, whence);
#endif

    if (curpos < 0)
    {
        return -1;
    }
    return curpos;
}

int64_t FileIOStream::ftell64Inner(FILE *stream) const
{
    if (stream == nullptr)
        return -1;
    int64_t curPos = fseek64Inner(stream, 0, SEEK_CUR);
    if (curPos >= 0)
    {
        return curPos;
    }
    else
    {
        return -1;
    }
}

bool FileIOStream::seek(const int64_t pos)
{
    if (file_ == nullptr)
        return false;

    return (fseek64Inner(file_, pos, SEEK_SET) >= 0);
}

bool FileIOStream::seekToBegin()
{
    if (file_ == nullptr)
        return false;

    return (fseek64Inner(file_, 0, SEEK_SET) >= 0);
}

bool FileIOStream::seekToEnd()
{
    if (file_ == nullptr)
        return false;

    return (fseek64Inner(file_, 0, SEEK_END) >= 0);
}

bool FileIOStream::skip(int64_t count)
{
    if (file_ == nullptr)
        return false;

    int64_t curpos = ftell64Inner(file_);
    return (fseek64Inner(file_, (curpos + count), SEEK_SET) >= 0);
}

int64_t FileIOStream::pos() const
{
    if (file_ == nullptr)
        return -1;

    return ftell64Inner(file_);
}

int64_t FileIOStream::size() const
{
    if (file_ == nullptr)
        return 0;

    int64_t curpos = ftell64Inner(file_);
    int64_t filesize = fseek64Inner(file_, 0, SEEK_END);
    if (filesize < 0)
    {
        return -1;
    }

    if (fseek64Inner(file_, curpos, SEEK_SET) < 0)
        return -1;

    return filesize;
}

bool FileIOStream::atEnd() const
{
    if (file_ == nullptr)
        return true;

    int64_t curPos = pos();
    if (curPos < 0)
        return true;
    int64_t fileSize = size();
    if (fileSize < 0)
        return true;

    return (curPos >= fileSize);
}

ReversedMemIOStream::ReversedMemIOStream(ByteOrder byteOrder) : MemIOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    pos_ = 0;
    alloc_ = false;
    max_size_ = 0;
}

ReversedMemIOStream::ReversedMemIOStream(const char *buffer, int64_t size, ByteOrder byteOrder) : MemIOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    left_end_ = 0;
    pos_ = size;
    alloc_ = false;
    setBuffer(buffer, size);
}

ReversedMemIOStream::ReversedMemIOStream(int64_t size, ByteOrder byteOrder) : MemIOStream(byteOrder)
{
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    left_end_ = 0;
    pos_ = size;
    alloc_ = false;
    allocSize(size);
}

ReversedMemIOStream::ReversedMemIOStream(const MemIOStream &s)
{
    buffer_ = nullptr;
    size_ = s.size();
    left_end_ = s.pos();
    pos_ = s.size();
    alloc_ = false;
    max_size_ = s.size();

    assert(size_ <= max_size_);

    if (s.data() != nullptr)
    {
        buffer_ = ReversedMemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.data(), (size_t)size_);
            alloc_ = true;
        }
    }
}

ReversedMemIOStream::ReversedMemIOStream(const ReversedMemIOStream &s)
{
    buffer_ = nullptr;
    size_ = s.size_;
    pos_ = s.pos_;
    alloc_ = false;
    max_size_ = s.max_size_;

    assert(size_ <= max_size_);

    if (s.buffer_ != nullptr)
    {
        buffer_ = ReversedMemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.buffer_, (size_t)size_);
            alloc_ = true;
        }
    }
}

ReversedMemIOStream::~ReversedMemIOStream()
{
    freeBuffer();
}

ReversedMemIOStream &ReversedMemIOStream::operator=(const MemIOStream &s)
{
    freeBuffer();

    buffer_ = nullptr;
    size_ = s.size();
    pos_ = s.size();
    alloc_ = false;
    max_size_ = s.size();

    assert(size_ <= max_size_);

    if (s.data() != nullptr)
    {
        buffer_ = ReversedMemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.data(), (size_t)size_);
            alloc_ = true;
        }
    }

    return (*this);
}

ReversedMemIOStream &ReversedMemIOStream::operator=(const ReversedMemIOStream &s)
{
    freeBuffer();

    buffer_ = nullptr;
    size_ = s.size_;
    pos_ = s.pos_;
    alloc_ = false;
    max_size_ = s.max_size_;

    assert(size_ <= max_size_);

    if (s.buffer_ != nullptr)
    {
        buffer_ = ReversedMemIOStream::allocBufer(max_size_);
        if (buffer_ != nullptr)
        {
            memcpy(buffer_, s.buffer_, (size_t)size_);
            alloc_ = true;
        }
    }

    return (*this);
}

char *ReversedMemIOStream::allocBufer(int64_t size) // static
{
    if (size <= 0)
        return nullptr;
    return (char *)malloc((size_t)size);
}

char *ReversedMemIOStream::reallocBuffer(char *buffer, int64_t size) // static
{
    if ((buffer == nullptr) || (size <= 0))
        return nullptr;
    return (char *)realloc(buffer, (size_t)size);
}

void ReversedMemIOStream::freeBuffer(char *buffer) // static
{
    if (buffer != nullptr)
    {
        free(buffer);
    }
}

void ReversedMemIOStream::freeBuffer()
{
    if ((buffer_ != nullptr) && alloc_)
    {
        ReversedMemIOStream::freeBuffer(buffer_);
    }
    buffer_ = nullptr;
    size_ = 0;
    max_size_ = 0;
    left_end_ = 0;
    pos_ = 0;
    alloc_ = false;
}

void ReversedMemIOStream::allocSize(int64_t size)
{
    buffer_ = ReversedMemIOStream::allocBufer(size);
    if (buffer_ != nullptr)
    {
        size_ = size;
        max_size_ = size_;
        left_end_ = 0;
        pos_ = 0;
        alloc_ = true;
    }
    else
    {
        buffer_ = nullptr;
        size_ = 0;
        max_size_ = 0;
        left_end_ = 0;
        pos_ = 0;
        alloc_ = false;
    }
}

void ReversedMemIOStream::reallocSize(int64_t size)
{
    if (buffer_ == nullptr)
    {
        allocSize(size);
        return;
    }

    if (!alloc_)
        return;

    if (size <= max_size_)
    {
        if (size > size_)
        {
            size_ = size;
        }
        return;
    }

    char *pOldBuffer = buffer_;
    int64_t maxSize = size + 256;
    maxSize += (maxSize >> 3);
    if (maxSize < 4096)
    {
        maxSize = (maxSize + 255) & 0xFFFFFFFFFFFFFF00LL;
    }
    else
    {
        maxSize = (maxSize + 4095) & 0xFFFFFFFFFFFFFC00LL;
    }
    // maxSize = size + 512;
    buffer_ = MemIOStream::reallocBuffer(buffer_, maxSize);
    if (buffer_ == nullptr)
    {
        MemIOStream::freeBuffer(pOldBuffer);
        buffer_ = nullptr;
        size_ = 0;
        max_size_ = 0;
        left_end_ = 0;
        pos_ = 0;
        alloc_ = false;
    }
    else
    {
        size_ = size;
        max_size_ = maxSize;
    }
}

void ReversedMemIOStream::setBuffer(const char *buffer, int64_t size)
{
    freeBuffer();
    buffer_ = (char *)buffer;
    size_ = size;
    max_size_ = size;
    alloc_ = false;
}

void ReversedMemIOStream::reserve(const int64_t size)
{
    int64_t s = size_;
    reallocSize(size);
    size_ = s;
}

void ReversedMemIOStream::resize(const int64_t size)
{
    reallocSize(size);
}

void ReversedMemIOStream::reset()
{
    size_ = 0;
    pos_ = 0;
    left_end_ = 0;
}

int64_t ReversedMemIOStream::read(char *buffer, const int64_t size)
{
    if (size <= 0)
    {
        read_ok_ = true;
        return 0;
    }
    if ((buffer_ == nullptr) || (size_ <= 0))
    {
        read_ok_ = false;
        return -1;
    }
    if ((buffer == nullptr) || (size <= 0))
    {
        read_ok_ = false;
        return -1;
    }

    if (pos_ <= left_end_)
    {
        read_ok_ = false;
        return -1;
    }

    int64_t readLen = size;
    if (readLen > (pos_ - left_end_))
        readLen = (pos_ - left_end_);

    memcpy(buffer, buffer_ - (size_t)readLen, (size_t)readLen);
    pos_ -= readLen;

    read_ok_ = (size == readLen);

    return readLen;
}

int64_t ReversedMemIOStream::write(const char *buffer, const int64_t size)
{
    return 0;
}

bool ReversedMemIOStream::seek(int64_t pos)
{
    if (pos < 0 || pos > size_)
        return false;
    pos_ = pos;
    return true;
}

bool ReversedMemIOStream::seekToBegin()
{
    pos_ = left_end_;
    return true;
}

bool ReversedMemIOStream::seekToEnd()
{
    pos_ = size_;
    return true;
}

bool ReversedMemIOStream::skip(int64_t count)
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return false;

    int64_t pos = pos_ - count;
    if (pos < left_end_)
    {
        pos_ = left_end_;
        return false;
    }
    pos_ = pos;
    return true;
}

int64_t ReversedMemIOStream::pos() const
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return 0;

    return pos_;
}

int64_t ReversedMemIOStream::size() const
{
    if ((buffer_ == nullptr) || (size_ <= 0))
        return 0;
    return size_;
}

char *ReversedMemIOStream::data()
{
    return buffer_;
}

const char *ReversedMemIOStream::data() const
{
    return buffer_;
}

bool ReversedMemIOStream::atEnd() const
{
    return (pos_ <= left_end_);
}

} // namespace sny