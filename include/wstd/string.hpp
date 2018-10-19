
#ifndef _WSTD_STRING_HPP_
#define _WSTD_STRING_HPP_


#include <varargs.h>
#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <sstream>

namespace wstd {

  using namespace std;
  static std::string itos(int value, int radix = 10) {
    char buf[32] = "";
    _itoa(value, buf, radix);
    return buf;
  }
  static int stoi(const std::string& str) {
    return atoi(str.c_str());
  }
  struct string_set_256 {
    unsigned char ucmap[256];
    string_set_256(const char* p) {
      memset(ucmap, 0, 256);
      for (; *p; ) { ucmap[(unsigned char)(*p++)] = 1; }
    }
    bool operator [](int index) const {
      return 1==ucmap[index&0xff];
    }
  };

  inline int trim_lindex(const char* s, int l, int r, const unsigned char* ucmap)
  {
    for (; l<r && ucmap[(unsigned char)s[l]]; l++);
    return l;
  }

  inline int trim_rindex(const char* s, int l, int r, const unsigned char* ucmap)
  {
    for (; l<r && ucmap[(unsigned char)s[r-1]]; --r);
    return r;
  }
  static string trim(const string& s, const char* delims)
  {
    const string_set_256 strset(delims);
    int l = trim_lindex(s.data(), 0, (int)s.size(), strset.ucmap);
    int r = trim_rindex(s.data(), 0, (int)s.size(), strset.ucmap);
    return s.substr(l, r-l);
  }

  static std::string format_int(int n, int numberOfLeadingZeros = 0)
  {
    std::ostringstream s;
    s << std::setw(numberOfLeadingZeros) << std::setfill('0') << n;
    return s.str();
  }

  static string format(const char* fmt, ...)
  {
    vector<char> buf(1024);
    for (; ; ) {
      va_list va;
      va_start(va, fmt);
      int bsize = static_cast<int>(buf.size());
      int len = vsnprintf(buf.data(), bsize, fmt, va);
      va_end(va);
      assert(len >= 0 && "Check format string for errors");
      if (len >= bsize) {
        buf.resize(len + 1);
        continue;
      }
      buf[bsize - 1] = 0;
      return std::string(buf.data(), len);
    }
  }
  // repeatedCharIgnored��ָ�Ƿ���Ӵ��ָ��ַ����е��ظ��ָ���
  static int split(vector<string>& resultStringVector, string srcStr, const std::string& delimStr, bool repeatedCharIgnored = true)
  {
    std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//�����ֵ����зָ������滻��Ϊһ����ͬ���ַ����ָ����ַ����ĵ�һ����
    size_t pos = srcStr.find(delimStr.at(0));
    std::string addedString = "";
    resultStringVector.reserve(0);
    while (pos != std::string::npos) {
      addedString = srcStr.substr(0, pos);
      if (!addedString.empty() || !repeatedCharIgnored) {
        addedString = trim(addedString, " \r\t");
        resultStringVector.push_back(addedString);
      }
      srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
      pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
      addedString = trim(addedString, " \r\t");
      resultStringVector.push_back(addedString);
    }
    return (int)resultStringVector.size();
  }

  static std::string strtime(const char* fmt) {
    time_t rawtime;
    tm *time1;
    char buf[256];
    time(&rawtime);
    time1 = localtime(&rawtime);
    fmt = fmt ? fmt : "%Y-%m-%d %H:%M:%S";
    strftime(buf, 256, fmt, time1);
    return buf;
  }

  inline char* strend(const char* str) {
    return (char*)str + strlen(str);
  }
  static void path_split(const char* fullpath, string* path, string* filename, string* filenameext, string* ext) {
    const char* p;
    const char *e;
    p = strrchr(fullpath, '\\');
    e = strrchr(fullpath, '/');
    p = max(p+1, e+1);
    p = max(p, fullpath);
    e = (e = strrchr(p, '.')) ? e : strend(p);
    if (path) path->assign(fullpath, p);
    if (filename) filename->assign(p, e);
    if (filenameext) filenameext->assign(p);
    if (ext) ext->assign(e);
  }
  static string path_split_filename(const char* fullpath) {
    string ret;
    path_split(fullpath, NULL, &ret, NULL, NULL);
    return ret;
  }
  static string path_split_filenameext(const char* fullpath) {
    string ret;
    path_split(fullpath, NULL, NULL, &ret, NULL);
    return ret;
  }
  static string path_split_ext(const char* fullpath) {
    string ret;
    path_split(fullpath, NULL, NULL, NULL, &ret);
    return ret;
  }
  static string path_fmt(const char* fullpath, const char* fmt) {
    char flag[256] = {0};
    string ret;
    for (const char*p = fmt; *p; ++p) {
      flag[(unsigned char)(*p)] = 1;
    }
    const char* d;
    const char* p;
    const char *e;
    d = strchr(fullpath, ':');
    d = max(fullpath, d+1);
    p = strrchr(fullpath, '\\');
    e = strrchr(fullpath, '/');
    p = max(p + 1, e + 1);
    p = max(p, fullpath);
    e = (e = strrchr(p, '.')) ? e : strend(p);
    if (flag['d']) ret.assign(fullpath, d);
    if (flag['p']) ret += string(d, p);
    if (flag['n']) ret += string(p, e);
    if (flag['x']) ret += string(e);
    return ret;
  }

} // namespace wstd

#endif // _WSTD_STRING_HPP_

