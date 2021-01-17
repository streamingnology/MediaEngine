/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "core/snyrefobj.h"
namespace sny {

class SnyDataBuffer {
 public:
  SnyDataBuffer();
  SnyDataBuffer(const SnyDataBuffer &data_buffer);
  SnyDataBuffer(const SnyDataBuffer &data_buffer, int offset, int length);
  SnyDataBuffer(const char *data, int size);
  SnyDataBuffer(const char *str);
  SnyDataBuffer(int size);

  virtual ~SnyDataBuffer();

 public:
  virtual void assign(const char *data, int size);
  virtual bool resize(int size);
  virtual char *data();
  virtual const char *data() const;
  virtual char *data(int offset);
  virtual const char *data(int offset) const;
  virtual int size() const;
  virtual bool isEmpty() const;
  virtual void clear();
  virtual void fill(const char c = 0);
  virtual SnyDataBuffer &subData(const SnyDataBuffer &data_buffer, int offset,
                                 int length);
  virtual bool append(const char *data, int size);
  virtual bool append(const SnyDataBuffer &data_buffer);
  virtual SnyDataBuffer clone() const;
  virtual SnyDataBuffer &operator=(const SnyDataBuffer &data_buffer);

 public:
  static int globalRefCount();

 private:
  void allocData(const char *data, int size);
  void releaseData();

 private:
  class SnyData;
  SnyData *ptr_sny_data_;
  int offset_;
  int length_;

  static int global_ref_count_;
};

}  // namespace sny
