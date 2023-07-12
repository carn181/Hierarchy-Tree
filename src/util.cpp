#include <iostream>
#include <bits/stdc++.h>
#include <string>

std::vector<std::string> splitstring(std::string s, char del) {
  std::vector<std::string> str_list;
  std::stringstream ss(s);
  while (getline(ss, s, del)) {
    str_list.push_back(s);
  }
  return str_list;
}

int count_occurences(std::string s, char c) {
  int count = 0;
  for (auto &chr : s) {
    if (chr == c)
      count++;
  }
  return count;
}

void print_char(char c, int n) {
  for (int i = 1; i <= n; i++)
    std::cout << c;
}

void print_tabs(int n) { print_char('\t', n); }
