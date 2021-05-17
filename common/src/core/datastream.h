/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <cstdio>
#include <string>

namespace sny
{

// clang-format off
class IOStream
{
public:
	enum ByteOrder { BigEndian, LittleEndian };

public:
	IOStream(ByteOrder byteOrder = LittleEndian); // default : no swap order
	virtual ~IOStream();

public:
	virtual int64_t read(char *buffer, const int64_t size)	      = 0;
	virtual int64_t write(const char *buffer, const int64_t size) = 0;
	virtual int64_t write(IOStream &stream, const int64_t size);
	virtual bool    seek(const int64_t pos)                       = 0;
	virtual bool    seekToBegin()                                 = 0;
	virtual bool    seekToEnd()                                   = 0;
	virtual bool    skip(const int64_t count)                     = 0;
	virtual int64_t pos() const                                   = 0;
	virtual int64_t size() const                                  = 0;
	virtual bool    atEnd() const                                 = 0;
	virtual int64_t remainSize() const;

	virtual char *      data();
	virtual const char *data() const;

	virtual bool  isReadOk();
	virtual bool  isWriteOk();

	virtual void  setReadOk(const bool ok);
	virtual void  setWriteOk(const bool ok);

	virtual void  setLittleEndian();
	virtual void  setBigEndian();
	virtual bool  isLittleEndian();
	virtual bool  isBigEndian();

	// read
	virtual IOStream & operator >> (bool     & v);
	virtual IOStream & operator >> (int8_t   & v);
	virtual IOStream & operator >> (uint8_t  & v);
	virtual IOStream & operator >> (int16_t  & v);
	virtual IOStream & operator >> (uint16_t & v);
	virtual IOStream & operator >> (int32_t  & v);
	virtual IOStream & operator >> (uint32_t & v);
	virtual IOStream & operator >> (int64_t  & v);
	virtual IOStream & operator >> (uint64_t & v);
	virtual IOStream & operator >> (float    & v);
	virtual IOStream & operator >> (double   & v);
	virtual IOStream & operator >> (std::string   & v);

	// write
	virtual IOStream & operator << (const bool     v);
	virtual IOStream & operator << (const int8_t   v);
	virtual IOStream & operator << (const uint8_t  v);
	virtual IOStream & operator << (const int16_t  v);
	virtual IOStream & operator << (const uint16_t v);
	virtual IOStream & operator << (const int32_t  v);
	virtual IOStream & operator << (const uint32_t v);
	virtual IOStream & operator << (const int64_t  v);
	virtual IOStream & operator << (const uint64_t v);
	virtual IOStream & operator << (const float    v);
	virtual IOStream & operator << (const double   v);
	virtual IOStream & operator << (const std::string & v);

	virtual IOStream & writeBytes(const uint64_t v, const int size);
	virtual IOStream & readBytes(uint64_t &v, const int size);
	virtual IOStream & writeBytes(const uint32_t v, const int size);
	virtual IOStream & readBytes(uint32_t &v, const int size);

	virtual IOStream & readUInt24(int &v);
	virtual IOStream & writeUInt24(const int v);
	virtual IOStream & readInt24(int &v);
	virtual IOStream & writeInt24(const int v);

public:
	static bool swapOrder(char *buffer, int64_t offset, int64_t count);
	static ByteOrder getSysByteOrder();

protected:
	template <typename T> IOStream & _read(T &v);
	template <typename T> IOStream & _write(const T v);

protected:
	ByteOrder sys_byte_order_;
	ByteOrder byte_order_;
	bool      read_ok_;
	bool      write_ok_;
};

class MemIOStream : public IOStream
{
public:
	MemIOStream(ByteOrder byteOrder = LittleEndian);
	MemIOStream(const char *buffer, int64_t size, ByteOrder byteOrder = LittleEndian);
	MemIOStream(int64_t size, ByteOrder byteOrder = LittleEndian);
	MemIOStream(const MemIOStream &s);

	virtual ~MemIOStream();

public:
	void setBuffer(const char *buffer, int64_t size);
	void freeBuffer();

	void reserve(const int64_t size);
	void resize(const int64_t size);
	void reset();

	virtual int64_t read(char *buffer, const int64_t size);
	virtual int64_t write(const char *buffer, const int64_t size);
	virtual bool    seek(const int64_t pos);
	virtual bool    seekToBegin();
	virtual bool    seekToEnd();
	virtual bool    skip(const int64_t count);
	virtual int64_t pos() const ;
	virtual int64_t size() const ;
	virtual bool    atEnd() const ;

	virtual char *      data();
	virtual const char *data() const;

	MemIOStream &operator = (const MemIOStream &s);

public:
	static char *allocBufer(int64_t size);
	static char *reallocBuffer(char *buffer, int64_t size);
	static void  freeBuffer(char *buffer);

protected:
	void allocSize(int64_t size);
	void reallocSize(int64_t size);

protected:
	char *  buffer_;
	int64_t pos_;
	int64_t size_;
	bool    alloc_;
	int64_t max_size_;
};

class QueueIOStream : public MemIOStream
{
public:
	QueueIOStream(ByteOrder byteOrder = LittleEndian);
	QueueIOStream(const QueueIOStream &s);
	virtual ~QueueIOStream();

public:
	int64_t  push(const char *buffer, const int64_t size);
	int64_t  cut(const int64_t size);
	int64_t  cut();

	QueueIOStream &operator = (const QueueIOStream &s);

private:
	void setBuffer(const char *buffer, int64_t size) { }
	void resize(const int64_t size) { }
};

class FileIOStream : public IOStream
{
public:
	enum OpenModeFlag
	{
		ReadOnly   = 0x0001,
		WriteOnly  = 0x0002,
		ReadWrite  = ReadOnly | WriteOnly,
		Append     = 0x0004,
		Truncate   = 0x0008,
	};

	typedef int32_t OpenMode;

public:
	FileIOStream(ByteOrder byteOrder = LittleEndian);
	FileIOStream(const char    *filename, FileIOStream::OpenMode openMode, ByteOrder byteOrder = LittleEndian);

	virtual ~FileIOStream();

public:
	bool open(const char    *filename, FileIOStream::OpenMode openMode);
	const std::string &getFileName();
	bool isOpen() const;
	void close();
	bool flush();

	virtual int64_t read(char *buffer, const int64_t size);
	virtual int64_t write(const char *buffer, const int64_t size);
	virtual bool    seek(const int64_t pos);
	virtual bool    seekToBegin();
	virtual bool    seekToEnd();
	virtual bool    skip(const int64_t count);
	virtual int64_t pos() const;
	virtual int64_t size() const;
	virtual bool    atEnd() const;

private:
	int64_t fseek64Inner(FILE *stream, int64_t offset, int whence) const;
	int64_t ftell64Inner(FILE *stream) const;
	std::string m_fileName;
	FILE * file_;
};

class ReversedMemIOStream : public MemIOStream
{
public:
	ReversedMemIOStream(ByteOrder byteOrder = LittleEndian);
	ReversedMemIOStream(const char *buffer, int64_t size, ByteOrder byteOrder = LittleEndian);
	ReversedMemIOStream(int64_t size, ByteOrder byteOrder = LittleEndian);
	ReversedMemIOStream(const MemIOStream &s);
	ReversedMemIOStream(const ReversedMemIOStream &s);

	virtual ~ReversedMemIOStream();

public:
	void setBuffer(const char *buffer, int64_t size);
	void freeBuffer();

	void reserve(const int64_t size);
	void resize(const int64_t size);
	void reset();

	virtual int64_t read(char *buffer, const int64_t size);
	virtual int64_t write(const char *buffer, const int64_t size);
	virtual bool    seek(const int64_t pos);
	virtual bool    seekToBegin();
	virtual bool    seekToEnd();
	virtual bool    skip(const int64_t count);
	virtual int64_t pos() const ;
	virtual int64_t size() const ;
	virtual bool    atEnd() const ;

	virtual char *      data();
	virtual const char *data() const;

	ReversedMemIOStream &operator = (const MemIOStream &s);
	ReversedMemIOStream &operator = (const ReversedMemIOStream &s);

public:
	static char *allocBufer(int64_t size);
	static char *reallocBuffer(char *buffer, int64_t size);
	static void  freeBuffer(char *buffer);

protected:
	void allocSize(int64_t size);
	void reallocSize(int64_t size);

protected:
	int64_t left_end_;
};
// clang-format on

} // namespace sny