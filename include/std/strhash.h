
// �ַ���Hash�����Ա�
// http://www.byvoid.com/blog/string-hash-compare/
//�Ҷ���Щhash��ɢ��������Ч������һ���򵥲��ԣ����Խ�����£�
//����1����100000���ɴ�Сд��ĸ�����������ANSI�ַ��������ظ���ÿ���ַ�����󳤶Ȳ�����64�ַ�������ɢ�У�
//�ַ�������    ��ͻ��  ��1000003ȡ���ĳ�ͻ��
//
// BKDRHash     0       4826
// SDBMHash     2       4814
// RSHash       2       4886
// APHash       0       4846
// ELFHash      1515    6120
// JSHash       779     5587
// DEKHash      863     5643
// FNVHash      2       4872
// DJBHash      832     5645
// DJB2Hash     695     5309
// PJWHash      1515    6120
//
// ����2����100000��������UNICODE�������ַ��������ظ���ÿ���ַ�����󳤶Ȳ�����64�ַ�������ɢ�У�
// �ַ�������   ��ͻ��  ��1000003ȡ���ĳ�ͻ��
// BKDRHash     3       4710
// SDBMHash     3       4904
// RSHash       3       4822
// APHash       2       4891
// ELFHash      16      4869
// JSHash       3       4812
// DEKHash      1       4755
// FNVHash      1       4803
// DJBHash      1       4749
// DJB2Hash     2       4817
// PJWHash      16      4869
//
// ����3����1000000�����ANSI�ַ��������ظ���ÿ���ַ�����󳤶Ȳ�����64�ַ�������ɢ�У�
// �ַ�������   ��ʱ�����룩
// BKDRHash     109
// SDBMHash     109
// RSHash       124
// APHash       187
// ELFHash      249
// JSHash       172
// DEKHash      140
// FNVHash      125
// DJBHash      125
// DJB2Hash     125
// PJWHash      234
//
// ���ۣ�Ҳ�����ҵ���������һЩ�����ԣ��ڶ�ASCII���ַ�������ɢ��ʱ��PJW��ELF Hash��������ʵ��ͬһ���㷨����������������Ч�ʣ����൱��⣻���磺"b5"�롰aE"���������ַ�������PJWɢ�г�����hashֵ����һ���ġ����⣬�����������������ɢ�еĹ�ϣ�������磺JS/DEK/DJB Hash���ڶ���ĸ��������ɵ��ַ�����ɢ��Ч��Ҳ����ô�á���Զ��ԣ�����BKDR��SDBM����򵥵�HashЧ����Ч�����á�
//���ߣ�icefireelf
//������http://blog.csdn.net/icefireelf/article/details/5796529

//�����ַ���Hash�����Ƚ�
//���õ��ַ���Hash��������ELFHash��APHash�ȵȣ�����ʮ�ּ���Ч�ķ�������Щ����ʹ��λ����ʹ��ÿһ���ַ��������ĺ���ֵ���� Ӱ�졣���⻹����MD5��SHA1Ϊ������Ӵպ�������Щ���������������ҵ���ײ��
//�����ַ�����ϣ������ BKDRHash��APHash��DJBHash��JSHash��RSHash��SDBMHash��PJWHash��ELFHash�ȵȡ��������ϼ��ֹ� ϣ�������Ҷ��������һ��СС�����⡣
// Hash����     ����1   ����2   ����3   ����4   ����1�÷�       ����2�÷�       ����3�÷�       ����4�÷�       ƽ����
// BKDRHash     2       0       4774    481     96.55   100     90.95   82.05   92.64
// APHash       2       3       4754    493     96.55   88.46   100     51.28   86.28
// DJBHash      2       2       4975    474     96.55   92.31   0       100     83.43
// JSHash       1       4       4761    506     100     84.62   96.83   17.95   81.94
// RSHash       1       0       4861    505     100     100     51.58   20.51   75.96
// SDBMHash     3       2       4849    504     93.1    92.31   57.01   23.08   72.41
// PJWHash      30      26      4878    513     0       0       43.89   0       21.95
// ELFHash      30      26      4878    513     0       0       43.89   0       21.95
//
// ��������1Ϊ100000����ĸ��������ɵ��������ϣ��ͻ����������2Ϊ100000���������Ӣ�ľ��ӹ�ϣ��ͻ����������3Ϊ����1�Ĺ�ϣֵ�� 1000003(������)��ģ��洢�����Ա��г�ͻ�ĸ���������4Ϊ����1�Ĺ�ϣֵ��10000019(��������)��ģ��洢�����Ա��г�ͻ�ĸ�����
//
// �����Ƚϣ��ó�����ƽ���÷֡�ƽ����Ϊƽ��ƽ���������Է��֣�BKDRHash��������ʵ��Ч�����Ǳ���ʵ���У�Ч��������ͻ���ġ�APHashҲ �ǽ�Ϊ������㷨��DJBHash,JSHash,RSHash��SDBMHash����ǧ�PJWHash��ELFHashЧ�������÷����ƣ����� �����������Ƶġ�

/// @brief BKDR Hash Function
/// @detail ���㷨������Brian Kernighan��Dennis Ritchie�ġ�The C Programming Language��һ�鱻չʾ����������һ�ּ򵥿�ݵ�hash�㷨��Ҳ��JavaĿǰ���õ��ַ�����Hash�㷨���۳�����Ϊ31����
// Ҳ���Գ���31��131��1313��13131��131313..
// ����˵���˷��ֽ�Ϊλ���㼰�Ӽ����������Ч�ʣ��罫��ʽ���Ϊ��hash = hash << 7 + hash << 1 + hash + ch;
// ����ʵ��Intelƽ̨�ϣ�CPU�ڲ��Զ��ߵĴ���Ч�ʶ��ǲ��ģ�
// �ҷֱ������100�ڴε������������㣬���ֶ���ʱ�������Ϊ0�������Debug�棬�ֽ��λ�����ĺ�ʱ��Ҫ��1/3����
// ��ARM����RISCϵͳ��û�в��Թ�������ARM�ڲ�ʹ��Booth's Algorithm��ģ��32λ�����˷����㣬����Ч��������йأ�
// ������8-31λ��Ϊ1��0ʱ����Ҫ1��ʱ������
// ������16-31λ��Ϊ1��0ʱ����Ҫ2��ʱ������
// ������24-31λ��Ϊ1��0ʱ����Ҫ3��ʱ������
// ������Ҫ4��ʱ������
// ��ˣ���Ȼ��û��ʵ�ʲ��ԣ���������Ȼ��Ϊ����Ч���ϲ�𲻴�
static unsigned int BKDRHash(const char* str, unsigned int len)
{
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    /* 31 131 1313 13131 131313 etc.. */
    hash = (hash * 131) + (*str);
  }
  return hash;
}
/// @brief SDBM Hash Function
/// @detail ���㷨�������ڿ�Դ��ĿSDBM��һ�ּ򵥵����ݿ����棩�б�Ӧ�ö�����������BKDRHash˼��һ�£�ֻ�����Ӳ�ͬ���ѡ�
static unsigned int SDBMHash(const char* str, unsigned int len)
{
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = 65599 * hash + (*str);
    //hash = (*str) + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}
/// @brief RS Hash Function
/// @detail ��Robert Sedgwicks���䡶Algorithms in C��һ����չʾ��������
static unsigned int RSHash(const char* str, unsigned int len)
{
  unsigned int a = 63689;
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = hash * a + (*str);
    a = a * 378551;
  }
  return hash;
}
/// @brief AP Hash Function
/// @detail ��Arash Partow������һ��hash�㷨��
/* End Of FNV Hash Function */
static unsigned int APHash(const char* str, unsigned int len)
{
  unsigned int hash = 0xAAAAAAAA;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) : ((~((hash << 11)) + ((*str) ^ (hash >> 5))));
  }
  return hash;
}
/// @brief JS Hash Function
/// ��Justin Sobel������һ��hash�㷨��
static unsigned int JSHash(const char* str, unsigned int len)
{
  unsigned int hash = 1315423911;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash ^= ((hash << 5) + (*str) + (hash >> 2));
  }
  return hash;
}
/// @brief DEK Function
/// @detail ���㷨������Donald E. Knuth�ڡ�Art Of Computer Programming Volume 3����չʾ��������
static unsigned int DEKHash(const char* str, unsigned int len)
{
  unsigned int hash = 1315423911; // len
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
  }
  return hash;
}
/// @brief FNV Hash Function
/// @detail Unix systemϵͳ��ʹ�õ�һ������hash�㷨������΢��Ҳ����hash_map��ʵ�֡�
static unsigned int FNVHash(const char* str, unsigned int len)
{
  unsigned int hash = 0x811c9dc5;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash *= 16777619;
    hash ^= (*str);
  }
  return hash;
}
/* End Of BP Hash Function */
static unsigned int FNVHash1(const char* str, unsigned int len)
{
  const unsigned int fnv_prime = 0x811C9DC5;
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash *= fnv_prime;
    hash ^= (*str);
  }
  return hash;
}
/// @brief DJB Hash Function
/// @detail ��Daniel J. Bernstein���ڷ�����һ��hash�㷨��
/* End Of SDBM Hash Function */
static unsigned int DJBHash(const char* str, unsigned int len)
{
  unsigned int hash = 5381;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = ((hash << 5) + hash) + (*str);
  }
  return hash;
}
/// @brief DJB Hash Function 2
/// @detail ��Daniel J. Bernstein ��������һ��hash�㷨��
static unsigned int DJB2Hash(const char* str, unsigned int len)
{
  register unsigned int hash = 5381;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = hash * 33 ^ (*str);
  }
  return hash;
}
/// @brief PJW Hash Function
/// @detail ���㷨�ǻ���AT&T����ʵ���ҵ�Peter J. Weinberger�����Ķ�������һ��hash�㷨��
/* End Of JS Hash Function */
static unsigned int PJWHash(const char* str, unsigned int len)
{
  const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
  const unsigned int ThreeQuarters = (unsigned int)((BitsInUnsignedInt * 3) / 4);
  const unsigned int OneEighth = (unsigned int)(BitsInUnsignedInt / 8);
  const unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
  unsigned int hash = 0;
  unsigned int test = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = (hash << OneEighth) + (*str);
    if ((test = hash & HighBits) != 0) {
      hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
    }
  }
  return hash;
}
/// @brief ELF Hash Function
/// @detail ������Unix��Extended Library Function��������������һ��hash�㷨������ʵ����PJW Hash�ı��Ρ�
static unsigned int ELFHash(const char* str, unsigned int len)
{
  enum {
    TotalBits = sizeof(unsigned int) * 8,
    ThreeQuarters = (TotalBits * 3) / 4,
    OneEighth = TotalBits / 8,
    HighBits = ((unsigned int)-1) << (TotalBits - OneEighth)
  };
  register unsigned int hash = 0;
  unsigned int magic = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = (hash << OneEighth) + (*str);
    if ((magic = hash & HighBits) != 0) {
      hash ^= (magic >> ThreeQuarters);
      hash &= ~magic;
    }
  }
  return hash;
}
/* End Of P. J. Weinberger Hash Function */
static unsigned int ELFHash2(const char* str, unsigned int len)
{
  unsigned int hash = 0;
  unsigned int x = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = (hash << 4) + (*str);
    if ((x = hash & 0xF0000000L) != 0) {
      hash ^= (x >> 24);
    }
    hash &= ~x;
  }
  return hash;
}
/* End Of DEK Hash Function */
static unsigned int BPHash(const char* str, unsigned int len)
{
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len; str++, i++) {
    hash = hash << 7 ^ (*str);
  }
  return hash;
}
/* End Of AP Hash Function */
//���־����Hash�㷨��ʵ��(Դ����)
//��PHP�г��ֵ��ַ���Hash����
static uint32_t hashpjw(char* arKey, unsigned int nKeyLength)
{
  uint32_t h = 0, g;
  char* arEnd = arKey + nKeyLength;
  while (arKey < arEnd) {
    h = (h << 4) + *arKey++;
    if ((g = (h & 0xF0000000))) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h;
}
//��OpenSSL�г��ֵ��ַ���Hash����
static uint32_t lh_strhash11(const char* str)
{
  int i, l;
  uint32_t ret = 0;
  unsigned short* s;
  if (str == NULL) {
    return(0);
  }
  l = ((int)strlen(str) + 1) / 2;
  s = (unsigned short*)str;
  for (i = 0; i < l; ++i) {
    ret ^= (s[i] << (i & 0x0f));
  }
  return(ret);
}
/* The following hash seems to work very well on normal text strings
* no collisions on /usr/dict/words and it distributes on %2^n quite
* well, not as good as MD5, but still good.
*/
static uint32_t lh_strhash1(const char* c)
{
  uint32_t ret = 0;
  long n;
  uint32_t v;
  int r;
  if ((c == NULL) || (*c == '\0')) {
    return(ret);
  }
  /*
  unsigned char b[16];
  MD5(c,strlen(c),b);
  return(b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24));
  */
  n = 0x100;
  while (*c) {
    v = n | (*c);
    n += 0x100;
    r = (int)((v >> 2) ^ v) & 0x0f;
    ret = (ret * (32 - r));
    ret &= 0xFFFFFFFFL;
    ret ^= v * v;
    c++;
  }
  return((ret >> 16) ^ ret);
}
//��MySql�г��ֵ��ַ���Hash����
#ifndef NEW_HASH_FUNCTION
/* Calc hashvalue for a key */
static uint32_t calc_hashnr(const char* key, uint32_t length)
{
  register uint32_t nr = 1, nr2 = 4;
  while (length--) {
    nr ^= (((nr & 63) + nr2) * ((uint32_t)(uint8_t)* key++)) + (nr << 8);
    nr2 += 3;
  }
  return((uint32_t)nr);
}
/* Calc hashvalue for a key, case indepenently */
static uint32_t calc_hashnr_caseup(const char* key, uint32_t length)
{
  register uint32_t nr = 1, nr2 = 4;
  while (length--) {
    nr ^= (((nr & 63) + nr2) * ((uint32_t)(uint8_t)toupper(*key++))) + (nr << 8);
    nr2 += 3;
  }
  return((uint32_t)nr);
}
#else
/*
* Fowler/Noll/Vo hash
*
* The basis of the hash algorithm was taken from an idea sent by email to the
* IEEE Posix P1003.2 mailing list from Phong Vo (kpv@research.att.com) and
* Glenn Fowler (gsf@research.att.com). Landon Curt Noll (chongo@toad.com)
* later improved on their algorithm.
*
* The magic is in the interesting relationship between the special prime
* 16777619 (2^24 + 403) and 2^32 and 2^8.
*
* This hash produces the fewest collisions of any function that we've seen so
* far, and works well on both numbers and strings.
*/
static uint32_t calc_hashnr(const byte* key, uint32_t len)
{
  const byte* end = key + len;
  uint32_t hash;
  for (hash = 0; key < end; key++) {
    hash *= 16777619;
    hash ^= (uint32_t) * (uint8_t*)key;
  }
  return (hash);
}
static uint32_t calc_hashnr_caseup(const byte* key, uint32_t len)
{
  const byte* end = key + len;
  uint32_t hash;
  for (hash = 0; key < end; key++) {
    hash *= 16777619;
    hash ^= (uint32_t)(uint8_t)toupper(*key);
  }
  return (hash);
}
#endif
//Mysql�ж��ַ���Hash�����������˴�Сд
//����һ�������ַ���Hash����
static unsigned int MysqlHash(const char* str, uint32_t len)
{
  register unsigned int h = 0;
  register const unsigned char* p = (const unsigned char*)str;
  uint32_t i = 0;
  for (; i<len; ++i, ++p) {
    h = 31 * h + *p;
  }
  return h;
}
//���Կ������¼�����ϣ������
//MurmurHash��CityHash��FarmHash��XXHash��SpookyHash��lookup3��SimHash��SuperFastHash��FNV��CRC32��
//������ϣ�����Ĺ�ϣֵһ����32bit��64bit��128bit������Ҳ�����ñ���MD5��SHA1�ȼ��ܹ�ϣ������
//��Ȼ���ܹ�ϣ��������ڷǼ��ܹ�ϣ�����ٶ�Ҫ��Щ���������ǵĹ�ϣֵҪ������������ͻ�ĸ��ʻ��С��
//һ����ΪSHA�����г�ͻ����Ϊ��������ͻ�ĸ���ԶԶС��Ӳ����������ĸ��ʡ�
//���⣬������һ��ʵ�飬��ϣ������javaʵ�֣�������һǧ���ٶ������ַ�������ϣֵ��int�ͣ���������Χ��0~0x7fffffff��
//����SuperFastHash�������Ĺ�ϣ�����ĳ�ͻ����0.3%���ң�����Ҷ�ĳ�������Լ��޸ģ�
//�����murmurhash�еĳ˷�������Ϊ�����������߰�XXHash��ĳһ�д���ȥ������ͻ��һ������٣��Ƚ����档 

// MurMurHash �㷨���ǷǼ���HASH�㷨�����ܸܺߣ� 
// �ȴ�ͳ��CRC32,MD5��SHA-1���������㷨���Ǽ���HASH�㷨�����Ӷȱ���ͺܸߣ������������ϵ���Ҳ���ɱ��⣩ 
// ��HASH�㷨Ҫ��ܶ࣬���Ҿ�˵����㷨����ײ�ʺܵ�. 
// http://murmurhash.googlepages.com/ 
static uint64_t MurMurHash(const char* key, int len) {
  uint8_t* buf = (uint8_t*)key;
  uint32_t seed = 0x1234ABCD;
  uint64_t m = 0xc6a4a7935bd1e995L;
  int r = 47, remaining = len;
  uint64_t h = seed ^ (len * m);
  uint64_t k;
  while (remaining >= 8) {
    k = *(uint64_t*)buf;
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
    remaining -= sizeof(k);
    buf += sizeof(k);
  }

  if (remaining > 0) {
    uint64_t finish = 0;
    memcpy(&finish, buf, remaining);
    // for big-endian version, do this first:   
    // finish.position(8-buf.remaining());  
    h ^= finish;
    h *= m;
  }

  h ^= h >> r;
  h *= m;
  h ^= h >> r;
  return h;
}
static unsigned int murMurHash(const void *key, int len)
{
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  const int seed = 97;
  unsigned int h = seed ^ len;
  // Mix 4 bytes at a time into the hash
  const unsigned char *data = (const unsigned char *)key;
  while (len >= 4)
  {
    unsigned int k = *(unsigned int *)data;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    len -= 4;
  }
  // Handle the last few bytes of the input array
  switch (len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
    h *= m;
  };
  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}