//////////////////////////////////////////////////////////////////////
/// Desc  StringRef
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#include "StringRef.hpp"

#define BIG_CONSTANT(x) (x##LLU)

NS_LS_BEGIN

static uint32_t MurmurHash32 ( const void * key, uint32_t len, uint32_t seed )
{
  /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  /* Initialize the hash to a 'random' value */

  uint32_t h = seed ^ len;

  /* Mix 4 bytes at a time into the hash */

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k; // = *(uint32_t*)data;
    memcpy(&k, data, sizeof(k));

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  /* Handle the last few bytes of the input array  */

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  /* Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.  */

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

/*-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby
//
// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
// and endian-ness issues if used across multiple platforms.
//
// 64-bit hash for 64-bit platforms
*/

static uint64_t MurmurHash64A ( const void * key, uint64_t len, uint64_t seed )
{
  const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 8)
  {
    uint64_t k; // = *data;
    memcpy(&k, data, sizeof(k));

    k *= m; 
    k ^= k >> r; 
    k *= m; 
    
    h ^= k;
    h *= m; 

    data += 8;
    len -= 8;
  }

  switch(len)
  {
  case 7: h ^= ((uint64_t) data[6]) << 48;
  case 6: h ^= ((uint64_t) data[5]) << 40;
  case 5: h ^= ((uint64_t) data[4]) << 32;
  case 4: h ^= ((uint64_t) data[3]) << 24;
  case 3: h ^= ((uint64_t) data[2]) << 16;
  case 2: h ^= ((uint64_t) data[1]) << 8;
  case 1: h ^= ((uint64_t) data[0]);
          h *= m;
  };
 
  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
} 


const size_t StringRef::npos = std::string::npos;

static size_t s_seed;

size_t StringRef::computeHash() const
{
#if LS_64BIT
    return (size_t)MurmurHash64A(str_, length_, uint64_t(&s_seed));
#else
    return (size_t)MurmurHash32(str_, length_, uint32_t(&s_seed));
#endif
}

size_t StringRef::find(const StringRef& s) const
{
    if(length_ < s.length_)
    {
        return npos;
    }

    size_t end = length_ - s.length_ + 1;
    for(int i = 0; i < (int)end; ++i)
    {
        int k;
        for(k = 0; k < s.length_; ++k)
        {
            if(str_[i + k] != s.str_[k])
            {
                break;
            }
        }

        if(k == s.length_)
        {
            return i;
        }
    }

    return npos;
}

NS_LS_END
