#include <string>
#include <vector>
#include <utility>
#include <set>
#include <iterator>



void foo(std::vector<std::string>& msa) {
  int last_col_idx = msa.front().length(); // rename to col count
  int last_row_idx = msa.size() ; // rename to row count

  // printf("last row idx: %d last col idx: %d\n", last_row_idx, last_col_idx);

  // find similar regions
  bool variable_region = true;
  std::pair<int,int> span;
  std::vector<std::pair<int,int>> spans;

  for (int col_idx = 0; col_idx < last_col_idx; col_idx++) {
    // printf("col idx: %d\n", col_idx);
    for (int row_idx = 0; row_idx < last_row_idx - 1; row_idx++) {
      // printf("\trow idx: %d ", row_idx);
      // printf("%c %c ", msa[row_idx][col_idx] , msa[row_idx+1][col_idx]);
      // printf("variable region: %d\n", variable_region);

      if (msa[row_idx][col_idx] != msa[row_idx+1][col_idx] && !variable_region) {
        variable_region = true;
        span = std::make_pair(span.first, col_idx-1);
        spans.push_back(span);
        break;
      }

      // could be combined with above
      if (col_idx == last_col_idx-1) {
        span = std::make_pair(span.first, col_idx);
        spans.push_back(span);
        break;
      }

      if (row_idx == last_row_idx - 2 &&
          msa[row_idx][col_idx] == msa[row_idx+1][col_idx] &&
          variable_region) {
        variable_region = false;
        span = std::make_pair(col_idx, -1);
        // span.a = col_idx;
      }
    }
  }

  if (false) {
    printf("Spans: [");
    for (auto span : spans) {
      printf("(%d, %d), ", span.first, span.second);
    }
    printf("]\n");
  }

  // -----
  //
  // -----
  std::vector<std::set<std::string>> raw_edt;
  std::set<std::string> d_letter;
  std::string s;

  int spans_idx = 0;
  bool increment_spans_idx;
  for  (int col_idx = 0; col_idx < last_col_idx; col_idx++) {
    // printf("col idx: %d, spans idx %d \n", col_idx, spans_idx);
    increment_spans_idx = false;

    // start in variable region
    if (col_idx == 0 && spans[spans_idx].first > 0) {
      int len = spans[spans_idx].first;

      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(0, len)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      raw_edt.push_back(d_letter);
      d_letter.clear();
    }

    // variable region
    if (spans_idx > 0 && col_idx == spans[spans_idx].first) {
      // if (spans_idx == 0) { continue; }

      int len = ((spans[spans_idx].first - 1) - spans[spans_idx-1].second);

      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(spans[spans_idx-1].second + 1, len)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      raw_edt.push_back(d_letter);
      d_letter.clear();
      increment_spans_idx = true;
    }

    // collapsible region
    if (col_idx == spans[spans_idx].first) {
      int len = (spans[spans_idx].second - spans[spans_idx].first) + 1;
      for (char ch: msa.front().substr(spans[spans_idx].first, len)) {
        if (ch != '-') {
          s.push_back(ch);
        }
      }
      d_letter.insert(s);
      raw_edt.push_back(d_letter);
      s.clear();
      d_letter.clear();
      increment_spans_idx = true;
    }


    // end in variable region
    if (col_idx == (last_col_idx - 1) && spans.back().second < col_idx) {
      // printf("spans idx sc %d \n", spans.back().second);
      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(spans.back().second + 1)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      raw_edt.push_back(d_letter);
      d_letter.clear();
    }


    if (increment_spans_idx) {
      ++spans_idx;
    }
  }

  if (true) {
    printf("raw_edt: ");
    for (auto d_letter : raw_edt) {
      if (d_letter.size() < 2) {
        printf("%s", d_letter.begin()->c_str());
      } else {
        printf("{");
        for (auto str = d_letter.begin(); str != d_letter.end(); str++) {
          if (str->length() > 0 ) {
            printf("%s", str->c_str());
          } else {
            // printf("e,");
          }
          if (std::next(str) != d_letter.end()) {
            printf(",");
          }
        }
        printf("}");
      }
    }
    printf("\n");
  }
}

int main() {
  std::vector<std::string> msa;
   msa = {
    "ACGTGTACA-GTTGAC",
    "A-G-GTACACGTT-AC",
    "A-GTGT-CACGTTGAC",
    "ACGTGTACA--TTGAC"
  };

   foo(msa);

   msa = {
    "CGTGTACA-GTTGACG",
    "-G-GTACACGTT-ACC",
    "-GTGTTCACGTTGACC",
    "CGTGTACA--TTGACC"
  };

  foo(msa);

  return 0;
}
