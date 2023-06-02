#include <gtest/gtest.h>
#include <iostream>
#include <valarray>
#include <vector>

#include "../src/core/core.hpp"
#include "../src/eds/eds.cpp"

TEST(ParserTest, NoEpsilons) {
  eds::EDS n;
  std::string input_string{""};
  std::vector<std::vector<eds::slice_eds>> n_slices{};

  input_string = "ACTGACCT";
  n_slices = std::vector<std::vector<eds::slice_eds>>{std::vector<eds::slice_eds>{
      eds::slice_eds(0, 8, 0),
  }};
  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 1);
  EXPECT_EQ(n.get_size(), 8);
  EXPECT_EQ(n.get_eps_count(), 0);
  EXPECT_EQ(n.get_slices(), n_slices);

  n = eds::EDS();
  input_string = "{AT,TC}{ATC,T}";
  n_slices = std::vector<std::vector<eds::slice_eds>>{std::vector<eds::slice_eds>{
                                                      eds::slice_eds(0, 2, 0),
                                                      eds::slice_eds(2, 2, 0),
                                                  },
                                                  std::vector<eds::slice_eds>{
                                                      eds::slice_eds(4, 3, 0),
                                                      eds::slice_eds(7, 1, 0),
                                                  }};

  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 2);
  EXPECT_EQ(n.get_size(), 8);
  EXPECT_EQ(n.get_eps_count(), 0);
  EXPECT_EQ(n.get_slices(), n_slices);
}



TEST(ParserTest, ParseEpsilons) {
  eds::EDS n;
  std::string input_string{"GACCC{TA,}TGAAG{CG,,}ATCGGGCG"};

  std::vector<std::vector<eds::slice_eds>> n_slices{};
  n_slices = std::vector<std::vector<eds::slice_eds>>{
      std::vector<eds::slice_eds>{eds::slice_eds(0, 5, 0)},
      std::vector<eds::slice_eds>{
          eds::slice_eds(5, 2, 0),
          eds::slice_eds(7, 0, 1),
      },
      std::vector<eds::slice_eds>{eds::slice_eds(8, 5, 0)},
      std::vector<eds::slice_eds>{
          eds::slice_eds(13, 2, 0),
          eds::slice_eds(15, 0, 1),
      },
      std::vector<eds::slice_eds>{eds::slice_eds(16, 8, 0)},
  };

  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 5);
  EXPECT_EQ(n.get_size(), 22);
  EXPECT_EQ(n.get_eps_count(), 2);
  EXPECT_EQ(n.get_slices(), n_slices);


  n = eds::EDS();
  input_string = "{AT,TC}{ATC,}CTA";
  n_slices = std::vector<std::vector<eds::slice_eds>>{std::vector<eds::slice_eds>{
                                                      eds::slice_eds(0, 2, 0),
                                                      eds::slice_eds(2, 2, 0),
                                                  },
                                                  std::vector<eds::slice_eds>{
                                                      eds::slice_eds(4, 3, 0),
                                                      eds::slice_eds(7, 0, 1),
                                                  },
                                                  std::vector<eds::slice_eds>{
                                                      eds::slice_eds(8, 3, 0)
                                                  }};
  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 3);
  EXPECT_EQ(n.get_size(), 10);
  EXPECT_EQ(n.get_eps_count(), 1);
  EXPECT_EQ(n.get_slices(), n_slices);

  n = eds::EDS();
  input_string = "ACTGAC{AT,,TC}AGG{,ATC,}CT{AT,TC}A";
  n_slices = std::vector<std::vector<eds::slice_eds>>{
      std::vector<eds::slice_eds>{eds::slice_eds(0, 6, 0)},
      std::vector<eds::slice_eds>{
          eds::slice_eds(6, 2, 0),
          eds::slice_eds(8, 2, 0),
          eds::slice_eds(10, 0, 1),
      },
      std::vector<eds::slice_eds>{eds::slice_eds(11, 3, 0)},
      std::vector<eds::slice_eds>{
          eds::slice_eds(14, 3, 0),
          eds::slice_eds(17, 0, 1),
      },
      std::vector<eds::slice_eds>{eds::slice_eds(18, 2, 0)},
      std::vector<eds::slice_eds>{
          eds::slice_eds(20, 2, 0),
          eds::slice_eds(22, 2, 0),
      },
      std::vector<eds::slice_eds>{eds::slice_eds(24, 1, 0)}
  };
  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 7);
  EXPECT_EQ(n.get_size(), 23);
  EXPECT_EQ(n.get_eps_count(), 2);
  EXPECT_EQ(n.get_slices(), n_slices);

  n = eds::EDS();
  input_string = "{,G}{CT,T}";
  n_slices = std::vector<std::vector<eds::slice_eds>>{std::vector<eds::slice_eds>{
                                                      eds::slice_eds(0, 1, 0),
                                                      eds::slice_eds(1, 0, 1),
                                                  },
                                                  std::vector<eds::slice_eds>{
                                                      eds::slice_eds(2, 2, 0),
                                                      eds::slice_eds(4, 1, 0),
                                                  }};
  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 2);
  EXPECT_EQ(n.get_size(), 4);
  EXPECT_EQ(n.get_eps_count(), 1);
  EXPECT_EQ(n.get_slices(), n_slices);

  n = eds::EDS();
  input_string = "{G,}{CT,T}";
  parse_data(input_string, &n);
  EXPECT_EQ(n.get_length(), 2);
  EXPECT_EQ(n.get_size(), 4);
  EXPECT_EQ(n.get_eps_count(), 1);
  EXPECT_EQ(n.get_slices(), n_slices);
}
