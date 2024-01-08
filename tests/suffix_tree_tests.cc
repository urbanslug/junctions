#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

#include "../src/core/core.hpp"
#include "../src/core/suffix_tree.cpp"

TEST(SuffixTreeTest, Matching) {

  std::string qry, text;

  // actual and expected query results
  std::vector<match_st::STQueryResult> match_positions, exp;

  std::vector<std::string> txt_strs;
  std::vector<eds::slice_eds> text_slices;
  match_st::STvertex *root;

  auto setup = [&]() {
    text.clear();
    core::join(txt_strs, '$', text);
    text += '_'; // add a terminator char
    root = match_st::Create_suffix_tree(text.c_str(), text.length());
    update_leaves(root, text_slices);
  };

  txt_strs = {"CACC"};
  text_slices = std::vector<eds::slice_eds>{eds::slice_eds(0,4)};
  setup();

  qry = "CT";

  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str());

  // ----

  txt_strs = {"A", "G"};
  text_slices =
      std::vector<eds::slice_eds>{eds::slice_eds(0, 1), eds::slice_eds(1, 1)};
  setup();

  qry = "GA";
  // qry = "A";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str());

  txt_strs = {"TCG", "CGC"};
  text_slices =
      std::vector<eds::slice_eds>{eds::slice_eds(0, 3), eds::slice_eds(3, 3)};
  setup();
  qry = "GAACA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str());

  txt_strs = {"AAGT","CT"};
  text_slices =
    std::vector<eds::slice_eds>{eds::slice_eds(0, 4), eds::slice_eds(4, 2)};
  setup();
  qry = "TG";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str());

  // print each match_pos in match positions vector
  std::cout << "Match positions.......: \n";
  for (auto &m : match_positions) {

    std::cout << "str: " << m.get_txt_str_idx()  << " char: " << m.get_char_idx() << " len: " << m.get_match_length() << "\n";
        //std::cout << m.get_txt_str_idx()  << " " << m.get_char_idx() << " " << m.get_match_length() << "\n";

  }

  EXPECT_EQ(0,0);
}

TEST(SuffixTreeTest, CommonPrefix) {

  std::string qry, text;

  // actual and expected query results
  std::vector<match_st::STQueryResult> match_positions, exp;

  std::vector<std::string> txt_strs;
  std::vector<eds::slice_eds> text_slices;
  match_st::STvertex *root;

  auto setup = [&]() {
    text.clear();
    core::join(txt_strs, '$', text);
    text += '_'; // add a terminator char
    root = match_st::Create_suffix_tree(text.c_str(), text.length());
    update_leaves(root, text_slices);
  };

  // ----
  txt_strs = {"G", "TG"};
  text_slices = std::vector<eds::slice_eds>{eds::slice_eds(0, 1), eds::slice_eds(1, 2)};
  setup();
  qry = "TT";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), true);

  std::vector<match_st::STQueryResult> expected = {
    match_st::STQueryResult(false, 1, 1, 2)
  };

  EXPECT_EQ(match_positions, expected);


  // ----
  //setup();
  qry = "TG";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), true);

  std::cout << "Match positions: \n";
  for (auto &m : match_positions) {
    std::cout << "str: " << m.get_txt_str_idx()  << " char: " << m.get_char_idx() << " len: " << m.get_match_length() << "\n";
  }
  std::cout << "\n\n";


  expected = {
    match_st::STQueryResult(false, 2, 1, 2)
  };

  EXPECT_EQ(match_positions, expected);
}


TEST(SuffixTreeTest, AllPrefixSuffixes) {

  std::string qry, text;

  // actual and expected query results
  std::vector<match_st::STQueryResult> match_positions, exp;

  std::vector<std::string> txt_strs;
  std::vector<eds::slice_eds> text_slices;
  match_st::STvertex *root;

  std::vector<match_st::STQueryResult> expected;

  auto setup = [&]() {
    text.clear();
    core::join(txt_strs, '$', text);
    text += '_'; // add a terminator char
    root = match_st::Create_suffix_tree(text.c_str(), text.length());
    update_leaves(root, text_slices);
  };

  txt_strs = {"CCC"};
  text_slices = std::vector<eds::slice_eds>{ eds::slice_eds(0, 3), eds::slice_eds(3, 3) };
  setup();
  qry = "CCA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), false);

  for (auto x : match_positions){
    std::cout << "byd: " << x.is_beyond_txt() << " str idx " << x.get_txt_str_idx() << " char idx " << x.get_char_idx() << " m_len " << x.get_match_length() << "\n";
  }

  expected = {
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 1),
    match_st::STQueryResult(true, 1, 0, 0)
  };

  EXPECT_EQ(match_positions, expected);

  // ----


  txt_strs = {"CCA", "AAA"};
  text_slices = std::vector<eds::slice_eds>{ eds::slice_eds(0, 3), eds::slice_eds(3, 3) };
  setup();
  qry = "CCC";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), false);

  std::cout << "Match positions: " << "query: " << qry << "txt strs:";
  for (auto &s : txt_strs) {
    std::cout << s << " ";
  }

  for (auto x : match_positions){
    std::cout << "byd: " << x.is_beyond_txt() << " str idx " << x.get_txt_str_idx() << " char idx " << x.get_char_idx() << " m_len " << x.get_match_length() << "\n";
  }

  expected = {
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 1),
    match_st::STQueryResult(true, 1, 0, 0)
  };

  EXPECT_EQ(match_positions, expected);

  txt_strs = {"CCC", "CGC", "CC"};
  text_slices = std::vector<eds::slice_eds>{ eds::slice_eds(0, 3), eds::slice_eds(3, 3), eds::slice_eds(6, 2) };
  setup();
  qry = "CA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), false);

  for (auto x : match_positions){
    std::cout << "byd: " << x.is_beyond_txt() << " str idx " << x.get_txt_str_idx() << " char idx " << x.get_char_idx() << " m_len " << x.get_match_length() << "\n";
  }

   expected = {
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 1),
    match_st::STQueryResult(true, 1, 0, 0)
  };

  EXPECT_EQ(match_positions, expected);



  txt_strs = {"CCC", "CCG"};
  text_slices = std::vector<eds::slice_eds>{ eds::slice_eds(0, 3), eds::slice_eds(3, 3) };
  setup();
  qry = "CA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), false);

  for (auto x : match_positions){
    std::cout << "byd: " << x.is_beyond_txt() << " str idx " << x.get_txt_str_idx() << " char idx " << x.get_char_idx() << " m_len " << x.get_match_length() << "\n";
  }

  expected = {
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 1),
    match_st::STQueryResult(true, 1, 0, 0)
  };

  EXPECT_EQ(match_positions, expected);


  // ----
  txt_strs = {"AAAA"};
  text_slices =
      std::vector<eds::slice_eds>{eds::slice_eds(0, 4)};
  setup();
  qry = "AAAA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), true);

  expected = {
    match_st::STQueryResult(true, 1, 0, 3),
    match_st::STQueryResult(true, 2, 0, 2),
    match_st::STQueryResult(true, 3, 0, 1),
    match_st::STQueryResult(false, 4, 0, 0),
    match_st::STQueryResult(true, 3, 0, 1),
  };

  EXPECT_EQ(match_positions, expected);

  txt_strs = {"CCC"};
  text_slices =
      std::vector<eds::slice_eds>{eds::slice_eds(0, 3)};
  setup();
  qry = "CA";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str(), false);

  for (auto x : match_positions){
    std::cout << "byd: " << x.is_beyond_txt() << " str idx " << x.get_txt_str_idx() << " char idx " << x.get_char_idx() << " m_len " << x.get_match_length() << "\n";
  }

  expected = {
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 2),
    match_st::STQueryResult(true, 1, 0, 1),
    match_st::STQueryResult(true, 1, 0, 0)
  };

  EXPECT_EQ(match_positions, expected);

  // ----

}
