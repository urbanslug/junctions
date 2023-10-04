#include <iostream>
#include <immintrin.h> // Include the AVX header
#include <set>
#include <fstream>
#include <iomanip>
#include <valarray>


#include "./eds.hpp"
#include "./common.hpp"


namespace eds {
const std::size_t REGISTER_WIDTH = 8;

  
int computeMultiple(int number) {
  if (number % REGISTER_WIDTH == 0) {
	return number;
  } else {
	return number + (REGISTER_WIDTH - number % REGISTER_WIDTH);
  }
}

void sumColumnsAVX(const float* vec1, const float* vec2,
				   float* result, std::size_t numCols) {
  // The following code assumes that numRows and numCols are multiples of 8
  for (size_t col = 0; col < numCols; col += 8) {
	// Initialize an AVX register with zeros
	__m256 sum = _mm256_setzero_ps();

	// Load 8 elements from each vector into AVX registers
	__m256 v1 = _mm256_loadu_ps(&vec1[col]);
	__m256 v2 = _mm256_loadu_ps(&vec2[col]);

	// Add the elements from both vectors
	sum = _mm256_add_ps(v1, v2);

	// Store the result of the sum back to memory
	_mm256_storeu_ps(&result[col], sum);
  }
}

void sumColumnsAVX_a(float* vec1, const float* vec2, std::size_t numCols) {
  // The following code assumes that numRows and numCols are multiples of 8
  for (size_t col = 0; col < numCols; col += 8) {
	// Initialize an AVX register with zeros
	__m256 sum = _mm256_setzero_ps();

	// Load 8 elements from each vector into AVX registers
	__m256 v1 = _mm256_loadu_ps(&vec1[col]);
	__m256 v2 = _mm256_loadu_ps(&vec2[col]);

	// Add the elements from both vectors
	sum = _mm256_add_ps(v1, v2);

	// Store the result of the sum back to memory
	_mm256_storeu_ps(&vec1[col], sum);
  }
}

std::valarray<std::size_t> xor_vectors(
  std::valarray<std::size_t> const& a, std::valarray<std::size_t> const& b
) {
  std::valarray<std::size_t> c = (a ^ b);
  return c;
}

std::valarray<std::size_t>
compute_similar_regions(std::vector<std::string> const& raw_strings) {
  std::size_t cols{raw_strings.front().length()};
  std::size_t rows{raw_strings.size()};

  std::valarray<std::size_t> a(cols);
  std::valarray<std::size_t> b(cols);

  //std::cout << "cols: " << cols << std::endl;

  std::vector<std::valarray<std::size_t>> xor_vecs{};

  for (std::size_t row{}; row < rows-1; ++row) {

	//std::cout << raw_strings[row] << std::endl;

	//std::cout << raw_strings[row+1] << std::endl;

	for (std::size_t col{}; col < cols; ++col) {

	  char c = raw_strings[row][col];
	  char _c = raw_strings[row+1][col];

	  //std::cout << "c: " << c << " _c: " << _c << std::endl;

	  a[col] = c;
	  b[col] = _c;
	}

	//std::valarray<std::size_t> res = xor_vectors(a, b);
	xor_vecs.push_back(xor_vectors(a, b));

  }

  //for (auto i : xor_vecs) {
  //for (int j = 0; j < cols; ++j) {
  //   std::cout  << std::left << std::setw(4) << i[j] << " ";
  //}
  //std::cout << "\n";
  //}

  //std::cout << "cols: " << cols << std::endl;

  std::size_t x = computeMultiple(cols);

  float _result[x];

  for (int i = 0; i < cols; ++i) {
	_result[i] = static_cast<float>(xor_vecs[0][i]);
  }
  if (cols < x) {
	for (int i = cols; i < x; ++i) {
	  _result[i] = 0;
	}
  }

  // TODO: is this always rows-1?
  std::size_t xor_rows = xor_vecs.size();




  //return;

  float vec1[x];
  for (std::size_t row {1}; row < xor_rows; ++row) {
	// populate vectors vec1 and vec2

	// std::cout << "row: " << row << std::endl;

	for (std::size_t i{}; i < cols; ++i) {
	  vec1[i] = static_cast<float>(xor_vecs[row][i]);
	}

	if (cols < x) {
	  for (std::size_t i = cols; i < x; ++i) { vec1[i] = 0; }
	}

	sumColumnsAVX_a(_result, vec1, x);
  }

  // print the _result vector
  //for (int i = 0; i < cols; ++i) {
  //std::cout << std::left << std::setw(4) << _result[i] << " ";
  //}

  // convert my float array to a std::size_t array
  // cols or x ?
  std::valarray<std::size_t> result(cols);
  for (int i = 0; i < cols; ++i) {
	result[i] = static_cast<std::size_t>(_result[i]);
  }
  // std::cout << std::endl;

  return result;
}

struct Foo {
  bool similar;
  std::size_t start;
  std::size_t length;
};

std::vector<Foo>
compute_region_vector(std::vector<std::string> const& raw_strings) {
  std::valarray<std::size_t> r = compute_similar_regions(raw_strings);

  std::size_t start{};
  std::size_t length{};
  std::size_t prev{r[0]};

  std::vector<Foo> foo{};

  std::vector<std::pair<std::size_t, std::size_t>> similar{};
	std::vector<std::pair<std::size_t, std::size_t>> different{};

  for (std::size_t i{}; i < r.size(); ++i) {
	if (r[i]  == 0 && prev == 0 ) {
	  // continue with zeros
	  ++length;
	}
	else if (prev == 0 && r[i] != 0) {
	  // start of non zeros
	  //std::cout << "start: " << start << " length: " << length << std::endl;
	  //similar.push_back(std::make_pair(start, length));
	  foo.push_back({true, start, length});
	  start = i;
	  length = 1;
	}
	else if (prev != 0 && r[i] != 0) {
	  // continue with non-zeros
	  ++length;
	}
	else if (prev != 0 && r[i] == 0) {
	  // end of non-zeros & start of zeros
	  //std::cout << "start: " << start << " length: " << length << std::endl;
	  //different.push_back(std::make_pair(start, length));
	  foo.push_back({false, start, length});
	  start = i;
	  length = 1;
	}
	else {
	  std::cout << "unexpected state" << std::endl;
	  //std::cout << "1start: " << start << " length: " << length << std::endl;
	  start = i;
	  length = 0;
	}
	prev = r[i];
  }

  // the last one
  if (prev == 0) {
	foo.push_back({true, start, length});
  }
  else {
	foo.push_back({false, start, length});
  }

  return foo;
}

void parse_msa_data(std::vector<std::string> const& raw_strings, EDS* e) {
  std::vector<Foo> foo = compute_region_vector(raw_strings);
  DegenerateLetter d = DegenerateLetter();
  std::vector<eds::slice_eds> d_slice{};
  std::set<std::string, std::greater<std::string>> d_set{};

  std::string current_string{};
  std::size_t rows{raw_strings.size()};
  std::size_t counter{};

  for (auto i : foo) {

	//std::cout << "start: " << i.start << " length: " << i.length << std::endl;
	
	if (i.similar) {
	  current_string = raw_strings[0].substr(i.start, i.length);

	  std::string ss{};

	  for (char c: current_string) {
		  if (c != '-') { ss += c; }
	  }

	  if (ss.empty()) { continue; }
	  
	  d.append_string(ss);

	  d_slice.push_back(eds::slice_eds(counter, ss.length()));
	  counter += ss.length();

	  e->inc_size_by(ss.length());
	  e->inc_str_count();
	}
	else {
	  for (std::size_t row{}; row < rows; ++row) {
		current_string = raw_strings[row].substr(i.start, i.length);
		d_set.insert(current_string);
	  }

	  for (std::string s: d_set) {
		std::string ss{};
		for (char c: s) {
		  if (c != '-') { ss += c; }
		}

		if (ss.empty()) {
		  d.set_epsilon(true);

		  d_slice.push_back(eds::slice_eds(counter, 0, true));

		  e->inc_epsilons();
		  //continue;
		  ++counter;
		}
		else {
		  e->inc_size_by(ss.length());
		  d.append_string(ss);
		  d_slice.push_back(eds::slice_eds(counter, ss.length()));
		  counter = counter + ss.length();
		}

	  e->inc_str_count();

		//d_slice.push_back({counter, ss.length()});

	  }

	}

	e->append_d_letter(d);
	e->append_slice(d_slice);

	d.reset();
	d_set.clear();
	d_slice.clear();
  }


  // std::cerr << *e << std::endl;
  // e->print_properties();

  // std::cout << std::endl << std::endl;
  
  //e->print_eds();

  //std::cout << std::endl;
  return;

  for (auto s : e->get_slices()) {
	for (auto sl: s) {
	  std::cout << "start: " << sl.start << " length: " << sl.length << " epsilon: " << sl.eps_slice << std::endl;
	}
	std::cout << std::endl;
  }

}


void read_msa_as_format_txt(const std::string& fp, std::vector<std::string>* raw_strings) {
  std::ifstream f{fp};
  std::string temp;

  if (!f) { FILE_ERROR(fp); }

  auto starts_with = [&]() -> bool {
	return temp[0] == '>';
  };

  while (std::getline(f, temp)) {
	if (starts_with()) {
	  raw_strings->push_back(std::string());
	} else {
	  raw_strings->back().append(temp);
	}
  }

  // TODO: remove
  /*
  while (f >> temp) {
	if (starts_with()) {
	  raw_strings->push_back(std::string());
	} else {
	  raw_strings->back().append(temp);
	}
  }*/
}


EDS Parser::from_msa(const std::string &fp) {
  EDS n;
  std::vector<std::string> raw_strings;
  std::string raw_string;
  size_t file_size = get_file_size(fp);

  raw_string.reserve(file_size);
  read_msa_as_format_txt(fp, &raw_strings);
  raw_string.shrink_to_fit();

  parse_msa_data(raw_strings, &n);

  //exit(0);

  //parse_data(raw_string, &n);

  return n;
}
}
