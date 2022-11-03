#include "../../rs/src/junctions.h"
#include <cstdio>
#include <string>
#include<bits/stdc++.h>
#include <stdlib.h>

#include <typeinfo>



int main()
{

  adder(2,3);
  printf("Hello");

  std::string strw = "Test string";
  std::string strq = "Test string _no";
  read_files(strw.data(), strw.length(), strq.data(), strq.length() );

  const char* s = get_str();

  std::string ss = s;

  std::cout << ss << std::endl;

  const char** k = get_strs();
  // const char* i = *k;
  // std::string str = i;

  // std::cout << str << std::endl;

  for (; *k; ++k) {
    const char* i = *k;
    std::string str = i;

    std::cout << str << std::endl;
  }


  const EdString ed_string = get_ed_string();
  const char** data = ed_string.data;
  const size_t* metadata = ed_string.metadata;


  size_t idx = *metadata;
  std::cout << idx << std::endl;

  // print pointer
  // printf("%p\n", ed_string.data);

  const char* i = *data;
  std::string str = i;
  std::cout << str << std::endl;


  std::vector<std::string> data_vec;
  std::vector<size_t> metadata_vec;

  for (; *data; ++data) {
    const char* i = *data;
    std::string str = i;

    std::cout << str << std::endl;

    data_vec.push_back(str);
  }

  for (; *metadata; ++metadata) {
    size_t str_count = *metadata;

    std::cout << str_count << std::endl;

    metadata_vec.push_back(str_count);
  }
}
