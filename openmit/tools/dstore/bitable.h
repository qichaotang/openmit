 /*!
 *  Copyright (c) 2017 by Contributors
 *  \file bitable.h
 *  \brief memory-based binary big table structure.
 *  \author ZhouYong
 */
#ifndef OPENMIT_TOOLS_DSTORE_BITABLE_H_
#define OPENMIT_TOOLS_DSTORE_BITABLE_H_

#include "openmit/tools/hash/murmur3.h"
#include <string>

namespace mit {
namespace dstore {
/*! 
 * \brief quadratic search model table structure 
 *        that be suitable to fixed length value
 */
template <typename VType, typename Hasher = mit::hash::MMHash128>
struct QuadSearch {
  /* type structure defination */
  // index. first_key & (num_buckets - 1)
  typedef uint32_t idx_type;
  typedef const idx_type * const_idx_pointer;
  // key. second_key. used to sequential search
  typedef uint64_t key_type;
  typedef const key_type * const_key_pointer;
  // value. data content stored
  typedef VType value_type;
  typedef const VType * const_value_pointer;
  // record. record data: key|value
  typedef unsigned char record_type;
  typedef const record_type * const_record_pointer;
  /*!
   * \brief bucket interval. 
   *        [first, last] stores record numbers offset
   */
  typedef std::pair<idx_type, idx_type> bucket_type;
  typedef const bucket_type * const_bucket_pointer;
  // uint type
  typedef uint32_t uint_type;
  typedef const uint_type * const_uint_pointer;
  // real type
  typedef float real_type;
  typedef const real_type * const_real_pointer;

  /* initialize constants */
  static const uint_type idx_size = sizeof(idx_type);
  static const uint_type key_size = sizeof(key_type);
  uint_type value_size;
  uint_type record_size;// record: key|value

  /*! \brief load model data to bi-model */
  void assign(const void * data) {
    // 1. coefficient. bias item
    coefficient_ = *const_real_pointer(data);
    data = reinterpret_cast<const char *>(data) + sizeof(real_type);
    // 2. value number
    value_num_ = *const_uint_pointer(data);
    value_size = value_num_ * sizeof(value_type);
    record_size = key_size + value_size;
    data = reinterpret_cast<const char *>(data) + sizeof(uint_type);
    // 3. bucket size
    idx_type bucket_size = *const_idx_pointer(data);
    bucket_1_ = bucket_size - 1;
    data = reinterpret_cast<const char *>(data) + sizeof(idx_type);
    // 4. index region
    idx_ = const_idx_pointer(data);
    // 5. value region
    record_ = const_record_pointer(idx_ + bucket_size + 1);
    
    size_ = sizeof(real_type) + sizeof(uint_type) + 
            (2 + bucket_size) * idx_size + idx_[bucket_size] * record_size;
  }

  /*! \brief find value according data and size */
  const_value_pointer find(const void * data, int size) const {
    uint64_t h1, h2;
    hasher_(data, size, &h1, &h2, 0);
    return search(h1, h2);
  }

  /*! \brief bi-model size */
  uint64_t size() const { return size_; }

  private:
    /*! \brief search record data according h1 and h2 */
    const_value_pointer search(uint64_t h1, uint64_t h2) const {
      // h1: find index
      uint64_t bucket_index = (h1 & bucket_1_);
      const_bucket_pointer bucket = 
        const_bucket_pointer(idx_ + bucket_index);

      const_record_pointer first = record_ + bucket->first * record_size;
      const_record_pointer last = record_ + bucket->second * record_size;
      // h2: sequential search
      for (; first != last; first += record_size) {
        if (*const_key_pointer(first) != h2) continue;
        return const_value_pointer(first + key_size);
      }
      return 0;
    }

  private:
    /*! \brief hash mathod for generate index and key */
    typedef Hasher hasher_;
    /*! \brief bias item coefficient */
    float coefficient_;
    /*! \brief number of value (float) */
    uint_type value_num_;
    /*! \brief index of buckets upper boundary */
    idx_type bucket_1_;
    /*! \brief index data region */
    const_idx_pointer idx_;
    /*! \brief record data region */
    const_record_pointer record_;
    /*! \brief offset */
    uint64_t size_;

}; // struct QuadSearch

} // namespace dstore
} // namespace mit
#endif // OPENMIT_TOOLS_DSTORE_BITABLE_H_
