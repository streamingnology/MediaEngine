/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snydatabuffer.h"
#include <memory.h>
#include <stdlib.h>

namespace sny {

#define SNY_DATABUFFER_PADDING_SIZE 16

int SnyDataBuffer::global_ref_count_ = 0;

class SnyDataBuffer::SnyData : public SnyRefObj {
 public:
  SnyData(const char *data, int size);
  virtual ~SnyData();

 public:
  virtual char *data() { return ptr_data_; }
  virtual const char *data() const { return ptr_data_; }
  virtual char *data(int offset);
  virtual const char *data(int offset) const;
  virtual int size() const { return size_; }

 public:
  char *ptr_data_;
  int size_;
};

SnyDataBuffer::SnyData::SnyData(const char *data, int size) {
  size_ = 0;
  ptr_data_ = (char *)malloc(size + SNY_DATABUFFER_PADDING_SIZE);
  SnyDataBuffer::global_ref_count_++;
  if (ptr_data_ != nullptr) {
    size_ = size;
    if (data != nullptr) {
      memcpy(ptr_data_, data, size);
    }
    ptr_data_[size] = 0x00;
  }
}

SnyDataBuffer::SnyData::~SnyData() {
  if (ptr_data_ != nullptr) {
    free(ptr_data_);
    ptr_data_ = nullptr;
    SnyDataBuffer::global_ref_count_--;
  }
  size_ = 0;
}

char *SnyDataBuffer::SnyData::data(int offset) {
  if (ptr_data_ == nullptr) {
    return nullptr;
  }
  return ptr_data_ + offset;
}

const char *SnyDataBuffer::SnyData::data(int offset) const {
  if (ptr_data_ == nullptr) {
    return nullptr;
  }
  return ptr_data_ + offset;
}

SnyDataBuffer::SnyDataBuffer() { allocData(nullptr, 0); }

SnyDataBuffer::SnyDataBuffer(const SnyDataBuffer &sny_data_buffer) {
  ptr_sny_data_ = nullptr;
  offset_ = -1;
  length_ = 0;
  operator=(sny_data_buffer);
}

SnyDataBuffer::SnyDataBuffer(const SnyDataBuffer &sny_data_buffer, int offset, int length) {
  ptr_sny_data_ = nullptr;
  offset_ = -1;
  length_ = 0;
  subData(sny_data_buffer, offset, length);
}

SnyDataBuffer::SnyDataBuffer(const char *data, int size) { allocData(data, size); }

SnyDataBuffer::SnyDataBuffer(const char *str) { allocData(str, (int)strlen(str)); }

SnyDataBuffer::SnyDataBuffer(int size) { allocData(nullptr, size); }

SnyDataBuffer::~SnyDataBuffer() { releaseData(); }

int SnyDataBuffer::globalRefCount() { return SnyDataBuffer::global_ref_count_; }

void SnyDataBuffer::allocData(const char *data, int size) {
  ptr_sny_data_ = new SnyData(data, size);
  offset_ = -1;
  length_ = 0;
}

void SnyDataBuffer::releaseData() {
  if (ptr_sny_data_ != nullptr) {
    ptr_sny_data_->release();
    ptr_sny_data_ = nullptr;
  }
  offset_ = -1;
  length_ = 0;
}

void SnyDataBuffer::assign(const char *data, int size) {
  releaseData();
  allocData(data, size);
}

bool SnyDataBuffer::resize(int size) {
  releaseData();
  allocData(nullptr, size);
  return (!isEmpty());
}

char *SnyDataBuffer::data() {
  if (ptr_sny_data_ == nullptr) {
    return nullptr;
  }
  if (offset_ >= 0) {
    return ptr_sny_data_->data(offset_);
  }
  return ptr_sny_data_->data();
}

const char *SnyDataBuffer::data() const {
  if (ptr_sny_data_ == nullptr) {
    return nullptr;
  }
  if (offset_ >= 0) {
    return ptr_sny_data_->data(offset_);
  }
  return ptr_sny_data_->data();
}

char *SnyDataBuffer::data(int offset) { return (data() + offset); }

const char *SnyDataBuffer::data(int offset) const { return (data() + offset); }

int SnyDataBuffer::size() const {
  if (ptr_sny_data_ == nullptr) {
    return 0;
  }
  if (offset_ >= 0) {
    return length_;
  }
  return ptr_sny_data_->size();
}

bool SnyDataBuffer::isEmpty() const { return (this->size() <= 0); }

void SnyDataBuffer::clear() { releaseData(); }

void SnyDataBuffer::fill(const char c) { memset(data(), c, size()); }

SnyDataBuffer &SnyDataBuffer::subData(const SnyDataBuffer &sny_data_buffer, int offset, int length) {
  releaseData();
  if (sny_data_buffer.ptr_sny_data_ != nullptr) {
    sny_data_buffer.ptr_sny_data_->addRef();
    ptr_sny_data_ = sny_data_buffer.ptr_sny_data_;
    offset_ = offset;
    length_ = length;
  }
  return (*this);
}

bool SnyDataBuffer::append(const char *oData, int oSize) {
  int newSize = oSize + this->size();
  SnyData *newData = new SnyData(nullptr, newSize);
  if (newData == nullptr) {
    return false;
  }
  if (newData->data() == nullptr) {
    newData->release();
    return false;
  }
  int offset = 0;
  if (!this->isEmpty()) {
    memcpy(newData->data(), this->data(), this->size());
    offset += this->size();
  }
  memcpy(newData->data(offset), oData, oSize);
  releaseData();
  ptr_sny_data_ = newData;

  return true;
}

bool SnyDataBuffer::append(const SnyDataBuffer &sny_data_buffer) {
  return this->append(sny_data_buffer.data(), sny_data_buffer.size());
}

SnyDataBuffer SnyDataBuffer::clone() const { return SnyDataBuffer(this->data(), this->size()); }

SnyDataBuffer &SnyDataBuffer::operator=(const SnyDataBuffer &sny_data_buffer) {
  releaseData();
  if (sny_data_buffer.ptr_sny_data_ != nullptr) {
    sny_data_buffer.ptr_sny_data_->addRef();
    ptr_sny_data_ = sny_data_buffer.ptr_sny_data_;
    offset_ = sny_data_buffer.offset_;
    length_ = sny_data_buffer.length_;
  }
  return (*this);
}

}  // namespace sny