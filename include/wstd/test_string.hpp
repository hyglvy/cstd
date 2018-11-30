
#include "wstd/string.hpp"
#include <stdio.h>
#include <string.h>

using namespace wstd;

int test_strsep() {
  char str[] = "abcdefg";
  char *p = str;
  char *key_point;
  while (p)
  {
    while (key_point = strsep(&p, "cd"))//�ؼ���Ϊc��d����������������
    {
      //��������cʱ��key_pointָ��c���أ�����dʱkey_pointָ��d���أ�ע���ʱd�Ѿ�����дΪ'\0'�ˣ�
      if (*key_point == 0)
        continue;//���������Ĺؼ��֣�����һ��ָ��\0��ָ�룬���������Ҿ���
      else
        break;//�ָ��һ���������ַ�������ȥ��ӡ�ɣ�
    }
    printf("%s\n", key_point);
  }
  return 0;
}

int test_string() {
  vector<string> ret;
  split(ret, "what tatoo"," t",true);
  const char* out[] = {"wha","a","oo"};
  for (int i=0; i<countof(out); ++i) {
    ASSERT(out[i]==ret[i]);
  }
  return 0;
}

#if 0
int test_string_path() {
  using namespace std;
  const char* str = "C:\\outpic\\aaa.jpg";
  assert("C:" == wstd::path_fmt(str, "d"));
  assert("\\outpic\\" == wstd::path_fmt(str, "p"));
  assert("aaa" == wstd::path_fmt(str, "n"));
  assert(".jpg" == wstd::path_fmt(str, "x"));
  assert("C:\\outpic\\" == wstd::path_fmt(str, "dp"));
  assert("C:\\outpic\\.jpg" == wstd::path_fmt(str, "dpx"));
  assert("C:aaa.jpg" == wstd::path_fmt(str, "dxn"));
  return 0;
}


#endif
