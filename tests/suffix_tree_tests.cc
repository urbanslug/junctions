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


  // ----
  txt_strs = {"G", "TG"};
  text_slices =
      std::vector<eds::slice_eds>{eds::slice_eds(0, 1), eds::slice_eds(1, 2)};
  setup();

  qry = "TT";
  // qry = "A";
  match_positions = match_st::FindEndIndexes(qry.c_str(), root, text.c_str());

  
  
  // print match positions vector
  std::cout << "Match positions: ";
  for (auto &m : match_positions) {
		std::cout << m.get_txt_str_idx() << " ";
  }

  EXPECT_EQ(1, 0);
  //EXPECT_EQ(match_positions, exp);  
}

