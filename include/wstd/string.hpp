
#ifndef _WSTD_STRING_HPP_
#define _WSTD_STRING_HPP_

#include <string>
#include <stdlib.h>

namespace wstd {

  static std::string itos(int value, int radix = 10) {
    char buf[32] = "";
    _itoa(value, buf, radix);
    return buf;
  }
  static int stoi(const std::string& str) {
    return atoi(str.c_str());
  }
  // repeatedCharIgnored��ָ�Ƿ���Ӵ��ָ��ַ����е��ظ��ָ���
  static int split(std::vector<std::string>& resultStringVector, std::string srcStr, std::string delimStr, bool repeatedCharIgnored = true)
  {
    std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//�����ֵ����зָ������滻��Ϊһ����ͬ���ַ����ָ����ַ����ĵ�һ����
    size_t pos = srcStr.find(delimStr.at(0));
    std::string addedString = "";
    while (pos != std::string::npos) {
      addedString = srcStr.substr(0, pos);
      if (!addedString.empty() || !repeatedCharIgnored) {
        resultStringVector.push_back(addedString);
      }
      srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
      pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
      resultStringVector.push_back(addedString);
    }
    return resultStringVector.size();
  }
} // namespace wstd

#endif // _WSTD_STRING_HPP_

