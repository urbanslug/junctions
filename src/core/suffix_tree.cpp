#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>



// TODO: remove
#include "./core.hpp"
#include "../eds/eds.hpp"

using namespace std;

namespace match_st {

STvertex *root;
const char *txt;
int liscie; /* liczba utworzonych lisci */

/**
 *
 *
 */
inline void Canonize(STedge &kraw,const char *x) {
  if (kraw.l<=kraw.r) {
    STedge e=kraw.v->g[x[kraw.l]];
    while (e.r-e.l <= kraw.r-kraw.l) {
      kraw.l+=e.r-e.l+1;
      kraw.v=e.v;
      if (kraw.l<=kraw.r) e=kraw.v->g[x[kraw.l]];
    }
  }
}

/**
 *
 *
 */
inline bool Test_and_split(STvertex* &w,const STedge &kraw) {
  w=kraw.v;
  if (kraw.l<=kraw.r) {
    char c=txt[kraw.l];
    STedge e=kraw.v->g[c];
    if (txt[kraw.r+1] == txt[e.l+kraw.r-kraw.l+1]) return true;
    w=new STvertex; w->numer=-1;
    kraw.v->g[c].r = e.l+kraw.r-kraw.l;
    kraw.v->g[c].v = w;
    e.l+=kraw.r-kraw.l+1;
    w->g[txt[e.l]]=e;
    return false;
  }
  return kraw.v->g.find(txt[kraw.l]) != kraw.v->g.end();
}

/**
 *
 *
 */
void Update(STedge &kraw,int n) {
  STvertex *oldr=root,*w;
  while (!Test_and_split(w,kraw))
  {
    STedge e;
    e.v=new STvertex; e.l=kraw.r+1; e.r=n-1;
    e.v->numer=liscie++;
    w->g[txt[kraw.r+1]]=e;
    if (oldr!=root) oldr->f=w;
    oldr=w;
    kraw.v=kraw.v->f;
    Canonize(kraw,txt);
  }
  if (oldr!=root) oldr->f=kraw.v;
}

/**
 * x should have a '$' at the end
 *
 */
STvertex* Create_suffix_tree(const char* x, int n) {
  STvertex *top; /* pinezka */
  STedge e;
  top=new STvertex; root=new STvertex; txt=x;
  top->numer = root->numer = root->string_id = top->string_id =  -1;
  e.v=root; liscie=0;
  REP(i,n) { e.r=-i; e.l=-i; top->g[x[i]]=e; }
  root->f=top;
  e.l=0; e.v=root;
  REP(i,n)
  {
    e.r=i-1; Update(e,n);
    e.r++; Canonize(e,x);
  }
  return root;
}

/**
 *
 *
 */
void STDelete(STvertex *w) {
  FORE(it,w->g) STDelete(it->second.v);
  delete w;
}

/**
 * Does a match exist
 *
 */
bool Find(const char *query, STvertex *r, const char *x) {
  // from the start to end of the query
  int i = 0, query_len = strlen(query);

  while (i < query_len) {
    if (r->g.find(query[i]) == r->g.end())
      return false;
    STedge e = r->g[query[i]];

    FOR(j, e.l, e.r) {
      if (i == query_len)
        return true;

      if (query[i++] != x[j])
        return false;
    }
    r = e.v;
  }
  return true;
}

/**
 *
 *
 */
bool is_leaf (STvertex const *v) {
  return v->numer >= 0;
};

/**
 *
 *
 */
// TODO: remove
void print_vec(std::vector<int> &leaves) {
  for (auto l : leaves) { printf("%d ", l); }
}

/**
 *
 *
 */
std::vector<int> DFS(STvertex const *current_vertex) {

  std::set<STvertex const *> explored;
  std::stack<STvertex const *> visited;

  std::vector<int> leaves;

  visited.push(current_vertex);

  while (!visited.empty()) {
    current_vertex = visited.top();

    if (explored.find(current_vertex) != explored.end()) {
      continue;
    }

    std::vector<STvertex const *> temp_store;

    for (auto k = current_vertex->g.begin(); k != current_vertex->g.end(); k++) {
      STvertex *vv = k->second.v;
      if (explored.find(vv) == explored.end()) {
        temp_store.push_back(vv);
      }
    }

    if  (temp_store.empty()) {
      explored.insert(current_vertex);
      visited.pop();

      if (is_leaf(current_vertex)) {
        leaves.push_back(current_vertex->numer);
      }

    } else {
      for (auto a : temp_store) {
        visited.push(a);
      }
    }
  }


  return leaves;
}

/**
 * Like DFS
 * given a vertex walk down the leaves and return all the match loci
 *
 *
 */
std::vector<n_core::match_locus> Get_Leaf_Data(STvertex const *current_vertex) {

  std::set<STvertex const *> explored;
  std::stack<STvertex const *> visited;

  // std::vector<std::pair<int, int>> leaves;
  std::vector<n_core::match_locus> leaves;

  visited.push(current_vertex);

  while (!visited.empty()) {
    current_vertex = visited.top();

    if (explored.find(current_vertex) != explored.end()) { continue; }

    std::vector<STvertex const *> temp_store;

    for (auto k = current_vertex->g.begin(); k != current_vertex->g.end(); k++) {
      STvertex *vv = k->second.v;
      if (explored.find(vv) == explored.end()) {
        temp_store.push_back(vv);
      }
    }

    if (temp_store.empty()) {
      explored.insert(current_vertex);
      visited.pop();

      if (is_leaf(current_vertex)) {
        leaves.push_back(n_core::match_locus{
            .string_index = current_vertex->string_id,
            .char_index = current_vertex->numer - current_vertex->string_id});
      }
    } else {
      for (auto a : temp_store) { visited.push(a); }
    }
  }

  return leaves;
}

/**
 * Mutates the suffix tree so that the leaves contain the string ID of the suffix...
 *
 * @param[in]
 * @param[in]
 */
void update_leaves(STvertex *current_vertex, std::vector<eds::slice_eds>const& text_offsets) {

  std::set<STvertex*> explored;
  std::stack<STvertex*> visited;

  std::vector<int> leaves;
  int string_start;

  visited.push(current_vertex);

  while (!visited.empty()) {
    current_vertex = visited.top();

    if (explored.find(current_vertex) != explored.end()) { continue; }

    std::vector<STvertex *> temp_store;

    for (auto k = current_vertex->g.begin(); k != current_vertex->g.end(); k++) {
      STvertex *vv = k->second.v;
      if (explored.find(vv) == explored.end()) { temp_store.push_back(vv); }
    }

    if (temp_store.empty()) {
      explored.insert(current_vertex);
      visited.pop();

      if (is_leaf(current_vertex)) {

        for (size_t i = 0; i < text_offsets.size(); i++) {

          string_start = text_offsets.at(i).start;

          string_start += i; // account for $ separators


          // in the middle
          if (current_vertex->numer < string_start) {
            current_vertex->string_id = i - 1;
            break;
          }

          // suffix starts at an entire string
          // or
          // we are at the end of text offsets
          if (string_start == current_vertex->numer || i == text_offsets.size() - 1) {
            current_vertex->string_id = (int)i;
            break;
          }
        }
      }

    } else {
      for (auto a : temp_store) {
        visited.push(a);
      }
    }
  }

  // return leaves;
}




/**
 *
 *
 * @param[in] query the query string
 * @param[in] current_vertex  root of the suffix tree (can taeke another vertex)
 * @param[in] x the first char to the txt
 */
// TODO: use junctions::extended_match
vector<n_core::extended_match> FindEndIndexes(const char *query, STvertex *current_vertex, const char *x) {

  std::vector<n_core::extended_match> matches;
  matches.reserve(strlen(x)); // is there a better value related to number of possible matches

  // TODO : rename i to q_idx or q_pos
  int i = 0; // the query position of the match
  int query_len = strlen(query);
  int match_length = 0;
  int d = -1;

  std::vector<n_core::match_locus> l_data;
  STvertex *last_with_underscore = NULL;
  STedge current_edge;
  bool has_dollar = false, has_underscore = false,
    has_qry_char = false, matched_a_char = false;

  auto looper = [&](std::vector<n_core::match_locus>& l_data, bool b = false, bool a = false) {
    for (auto l : l_data) {
      matches.push_back(
          n_core::extended_match{.beyond_text = b,
                                    .match_length = (a ? d : match_length),
                                    .str_idx = l.string_index,
                                    .chr_idx = l.char_index});
    }
  };

  auto append_matches = [&](bool b = false, bool a = false) {
    l_data = Get_Leaf_Data(current_edge.v);
    looper(l_data, b, a);
  };

  auto append_underscore_matches = [&]() {
    if (last_with_underscore == NULL) { return; }
    l_data = Get_Leaf_Data(last_with_underscore->g['_'].v);
    looper(l_data, true, true);
  };


  while (i < query_len) {

    has_dollar = current_vertex->g.find('$') != current_vertex->g.end();
    has_underscore = current_vertex->g.find('_') != current_vertex->g.end();
    has_qry_char = current_vertex->g.find(query[i]) != current_vertex->g.end();

    // TODO: make these if statements not be order dependent?

    // We matched at least one query to the end a text string
    // because we saw a $ sign and i > 0
    if (has_dollar && matched_a_char) {
      current_edge = current_vertex->g['$'];
      append_underscore_matches(); // TODO: remove useless

      append_matches(true);
      if (!has_qry_char) { return matches; }
    }

    // we can no longer match the query and we found a _
    if (!has_qry_char && has_underscore && matched_a_char) {
      current_edge = current_vertex->g['_'];
      append_underscore_matches();
      append_matches(true);
      return matches;
    }

    // save the last vertex we found with an _ going out of it
    // and is not the root
    if (has_underscore && matched_a_char) {
      d = match_length;
      last_with_underscore = current_vertex;
    }


    // matching failed
    if (!has_qry_char) {
      // we cannot match anymore but maybe we can go back up to the last vertex
      // with an underscore.
      // There's no need to check if i> 0 because it was checked when
      // last_with_underscore got set
      append_underscore_matches();

      return matches;
    }

    current_edge = current_vertex->g[query[i]];

    // matched at least one character
    matched_a_char = true;

    FOR(j, current_edge.l, current_edge.r) {

      // we have finished matching the query along a branch
      if (i == query_len) {
        append_matches();
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a $ or _
      // and we just finished processing the query as well
      if ((x[j] == '_' || x[j] == '$') && ((i == query_len) || current_edge.l == j)) {
        // std::cerr << "case 0\n";
        append_matches();
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a _
      // and have not finished processing the query
      if (x[j] == '_') {
        // std::cerr << "case 1\n" ;
        append_matches(true);
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a $
      // and have not finished processing the query
      if (x[j] == '$') {
        // std::cerr << "case 3\n";
        append_matches(true);
        append_underscore_matches();
        return matches;
      }

      // Find a mismatch
      if (query[i++] != x[j]) {
        // we cannot match anymore but maybe we can go back up to the last
        // vertex with an underscore.
        // There's no need to check if i> 0 because it was checked when
        // last_with_underscore got set
        append_underscore_matches();
        return matches;
      }

      match_length += 1;
    }

    current_vertex = current_edge.v;
  }

  // We have finished matching the query along a branch
  append_matches();
  append_underscore_matches();

  return matches;
}

// TODO remove deprecated
/**
 *
 *
 */
void gen_suffix_tree_new(
    eds::EDS& eds,
    std::vector<std::pair<STvertex, std::string>> *suffix_trees) {

  // std::vector<std::string>& i_letter;
  // const std::vector<std::string>& i_letter{};

  // std::vector<slicex> const *str_slices;
  std::string text;
  STvertex *root;

  for (size_t i = 0; i < eds.get_length(); i++) {
    text.clear();

    std::vector<std::string>& i_letter = eds.get_d_letter(i).get_strs();
    std::vector<eds::slice_eds>& str_slices = eds.get_slice(i); //&eds.str_slices[i];

    n_junctions::join(i_letter, '$', text); // concat the strings with dollar sign
    text += '_';                          // add a terminator char

    // Create the suffix tree
    root = Create_suffix_tree(text.c_str(), text.length());

    // add string ids
    update_leaves(root, str_slices);

    suffix_trees->push_back(std::make_pair(*root, text));
  }
};
} // namespace match_st