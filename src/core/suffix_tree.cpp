#include <cstddef>
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

#include "./core.hpp"
#include "./constants.hpp"
#include "../eds/eds.hpp"

namespace match_st {

STvertex *root;
const char *txt;
int leaves; /* number of created leaves */

bool operator==(const STQueryResult& lhs, const STQueryResult& rhs) {
  std::size_t w = lhs.is_beyond_txt();
  std::size_t x = lhs.get_match_length();
  std::size_t y = lhs.get_char_idx();
  std::size_t z = lhs.get_txt_str_idx();

  std::size_t a = rhs.is_beyond_txt();
  std::size_t b = rhs.get_match_length();
  std::size_t c = rhs.get_char_idx();
  std::size_t d = rhs.get_txt_str_idx();

  return std::tie(w, x, y, z) == std::tie(a, b, c, d);
}

bool operator<(const STQueryResult& lhs, const STQueryResult& rhs) {
  std::size_t w = lhs.is_beyond_txt();
  std::size_t x = lhs.get_match_length();
  std::size_t y = lhs.get_char_idx();
  std::size_t z = lhs.get_txt_str_idx();

  std::size_t a = rhs.is_beyond_txt();
  std::size_t b = rhs.get_match_length();
  std::size_t c = rhs.get_char_idx();
  std::size_t d = rhs.get_txt_str_idx();

  return std::tie(w, x, y, z) < std::tie(a, b, c, d);
}

bool operator<(const internal_st_vertex& lhs, const internal_st_vertex& rhs) {
  return std::tie(lhs.vertex, lhs.depth, lhs.start_char, lhs.in_node_offset) <
    std::tie(rhs.vertex, rhs.depth, rhs.start_char, rhs.in_node_offset);
}

/**
 *
 *
 */
inline void Canonize(STedge &edge,const char *x) {
  if (edge.l<=edge.r) {
    STedge e=edge.v->g[x[edge.l]];
    while (e.r-e.l <= edge.r-edge.l) {
      edge.l+=e.r-e.l+1;
      edge.v=e.v;
      if (edge.l<=edge.r) e=edge.v->g[x[edge.l]];
    }
  }
}

/**
 *
 *
 */
inline bool Test_and_split(STvertex* &w,const STedge &edge) {
  w=edge.v;
  if (edge.l<=edge.r) {
    char c=txt[edge.l];
    STedge e=edge.v->g[c];
    if (txt[edge.r+1] == txt[e.l+edge.r-edge.l+1]) return true;
    w=new STvertex; w->number=-1;
    edge.v->g[c].r = e.l+edge.r-edge.l;
    edge.v->g[c].v = w;
    e.l+=edge.r-edge.l+1;
    w->g[txt[e.l]]=e;
    return false;
  }
  return edge.v->g.find(txt[edge.l]) != edge.v->g.end();
}

/**
 *
 *
 */
void Update(STedge &edge,int n) {
  STvertex *oldr=root,*w;
  while (!Test_and_split(w,edge))
  {
    STedge e;
    e.v=new STvertex; e.l=edge.r+1; e.r=n-1;
    e.v->number=leaves++;
    w->g[txt[edge.r+1]]=e;
    if (oldr!=root) oldr->f=w;
    oldr=w;
    edge.v=edge.v->f;
    Canonize(edge,txt);
  }
  if (oldr!=root) oldr->f=edge.v;
}

/**
 * x should have a core::constants::terminar_char at the end
 *
 */
STvertex* Create_suffix_tree(const char* x, int n) {
  STvertex *top; /* pin */
  STedge e;
  top=new STvertex; root=new STvertex; txt=x;
  top->number = root->number = root->string_id = top->string_id =  -1;
  e.v=root; leaves=0;
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
 * Query a string (not GST)
 * Does a match exist
 * reuturns a bool
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
  return v->number >= 0;
};


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
        leaves.push_back(current_vertex->number);
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
std::vector<match_st::LeafData> Get_Leaf_Data(STvertex const *current_vertex) {

  std::set<STvertex const *> explored;
  std::stack<STvertex const *> visited;

  std::vector<match_st::LeafData> leaves;

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

      // returns a string id and the position in the concatenated string
      // string separator included
      if (is_leaf(current_vertex)) {
        leaves.push_back(
           match_st::LeafData(current_vertex->string_id,
                              current_vertex->number));
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
// Can we avoid passing the eds and letter idx?
void update_leaves(STvertex *current_vertex,
                   std::vector<eds::slice_eds> const &text_offsets,
                   eds::EDS& eds, std::size_t letter_idx) {
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

          string_start = eds.to_local_idx(letter_idx, text_offsets.at(i).start);

          string_start += i; // account for $ separators


          // in the middle
          if (current_vertex->number < string_start) {
            current_vertex->string_id = i - 1;
            break;
          }

          // suffix starts at an entire string
          // or
          // we are at the end of text offsets
          if (string_start == current_vertex->number || i == text_offsets.size() - 1) {
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

void update_leaves(STvertex *current_vertex,
                   std::vector<eds::slice_eds> const &text_offsets_local) {
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

        for (size_t i = 0; i < text_offsets_local.size(); i++) {

          string_start = text_offsets_local.at(i).start;

          string_start += i; // account for $ separators


          // in the middle
          if (current_vertex->number < string_start) {
            current_vertex->string_id = i - 1;
            break;
          }

          // suffix starts at an entire string
          // or
          // we are at the end of text offsets
          if (string_start == current_vertex->number || i == text_offsets_local.size() - 1) {
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

std::vector<match_st::STQueryResult>
FindEndIndexes(const char *query,
               const internal_st_vertex &root,
               const char *text,
               std::map<std::size_t, std::set<internal_st_vertex>> &marked_nodes,
               std::size_t qry_letter_idx,
               bool end_in_imp_imp) {
  std::vector<match_st::STQueryResult> matches{};
  matches.reserve(strlen(text));
  if (strlen(query) == 0) { return matches; }

  // TODO : rename i to q_idx or q_pos
  int i {}, // the query position of the match
    match_length {},
    query_len { static_cast<int>(strlen(query)) },
    d { -1 }; // last underscore match length TODO: give a better name

  std::set<STQueryResult> seen; // TODO: use better duplicate catcher

  char last_branching_char {};
  std::size_t last_branching_q_idx {};
  std::vector<match_st::LeafData> l_data;
  STedge current_edge {}; // an int
  STvertex *current_vertex { root.vertex }, *last_with_underscore{nullptr};
  bool has_dollar{false}, has_underscore{false}, has_qry_char{false}, matched_a_char{false};

  // TODO: read the value of l_data from scope
  auto looper = [&](std::vector<match_st::LeafData> l_data, bool q_bynd_txt = false, bool a = false) -> void {
    for (match_st::LeafData l : l_data) {
      l.get_txt_char_idx_mut() += static_cast<int>(root.depth);
      if ((a ? d : match_length) == 0) { continue; }
      match_st::STQueryResult match { match_st::STQueryResult(q_bynd_txt, (a ? d : match_length),l)};
      if (seen.find(match) == seen.end()) {
        matches.push_back(match);
        seen.insert(match);
      }
    }
  };

  // make sure this call is made after initializing the value of current edge
  auto append_matches = [&](bool q_bynd_txt = false, bool a = false) -> void {
    l_data = Get_Leaf_Data(current_edge.v);
    looper(l_data, q_bynd_txt, a);
  };

  // when only_term_branch is:
  //   true, we check the branch with the terminator char only
  //   false, we check all branches leading out
  auto append_underscore_matches = [&](bool only_term_branch = true ) -> void {
    if (last_with_underscore == nullptr) { return; }
    STvertex *search_v = only_term_branch
      ? last_with_underscore->g[core::constants::terminator_char].v
      : last_with_underscore;
    l_data = Get_Leaf_Data(search_v);
    looper(l_data, true, true);
  };

  auto mark_st = [&](char b_char, std::size_t in_node_offset) -> void {
    if (match_length > 0 && !is_leaf(current_vertex) && i == query_len) {
      marked_nodes[qry_letter_idx].insert(
        internal_st_vertex {current_vertex,
                            static_cast<size_t>(match_length),
                            b_char,
                            in_node_offset});
    }
  };

  while (i < query_len) {
    has_dollar = current_vertex->g.find(core::constants::string_separator) != current_vertex->g.end();
    has_underscore = current_vertex->g.find(core::constants::terminator_char) != current_vertex->g.end();

    char c { matched_a_char || (!matched_a_char && root.in_node_offset == 0 )
             ? query[i]
             : root.start_char };
    has_qry_char = current_vertex->g.find(c) != current_vertex->g.end();

    // These if statements are order dependent

    // both has a $ and _ going out of it
    if (has_underscore && has_dollar && matched_a_char) {
      append_matches(true);
      if (!has_qry_char) { return matches; }
    }

    // We matched at least one query to the end a text string
    // because we saw a $ sign
    // we can no longer match the query
    // i > 0
    if (has_dollar && !has_qry_char && matched_a_char) {
      append_matches(true);
      return matches;
    }

    if (has_underscore && matched_a_char) {
      // save the last vertex we found with an _ going out of it
      // and is not the root
      d = match_length;
      last_with_underscore = current_vertex;

      // we can no longer match the query and we found a _
      if (!has_qry_char) {
        append_matches(true);
        return matches;
      }
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

    current_edge = current_vertex->g[c];
    last_branching_char = c;
    // query char idx at which we last branched in the ST (i could work as well)
    last_branching_q_idx = match_length;

    std::size_t l { !matched_a_char
                    ? static_cast<size_t>(current_edge.l) + root.in_node_offset
                    : static_cast<size_t>(current_edge.l) };

    matched_a_char = true;

    FOR(j, l, current_edge.r) {
      // we have finished matching the query along a branch
      if (i == query_len) {
        append_matches();
        append_underscore_matches();

        // ended match in branch and not at the end of a string
        if (text[j] != core::constants::terminator_char && text[j] != core::constants::string_separator) {
          mark_st(last_branching_char, match_length - last_branching_q_idx);
        }

        return matches;
      }

      // In the process of matching we find a $ or _
      // and we just finished processing the query as well
      if ((text[j] == core::constants::terminator_char || text[j] == core::constants::string_separator) &&
          (i == query_len || current_edge.l == j)) {
        append_matches();
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a _
      // and have not finished processing the query
      if (text[j] == core::constants::terminator_char) {
        append_matches(true);
        append_underscore_matches();
        return matches;
      }

      if (has_underscore) {
        append_underscore_matches(false);
      }

      // In the process of matching we find a $
      // and have not finished processing the query
      if (text[j] == core::constants::string_separator) {
        append_matches(true);
        append_underscore_matches();
        return matches;
      }

      // Find a mismatch
      if (query[i++] != text[j]) {
        // we cannot match anymore but maybe we can go back up to the last
        // vertex with an underscore.
        // There's no need to check if i> 0 because it was checked when
        // last_with_underscore got set
        append_underscore_matches();
        if (end_in_imp_imp) { append_matches(false); }
        return matches;
      }

      match_length += 1;
    }

    current_vertex = current_edge.v;
  }

  /*
    We have finished matching the query along a branch
   */

  // zero is the offset from the start of the branch we are marking
  mark_st(core::constants::null_char, 0);
  append_matches();
  append_underscore_matches(); // TODO: is this useful?

  return matches;
}


void gen_suffix_tree_(eds::EDS& eds, std::vector<meta_st> *suffix_trees) {
  std::string text;
  STvertex* root;

  for (size_t i = 0; i < eds.get_length(); i++) {
    text.clear();

    std::vector<std::string>& i_letter = eds.get_d_letter(i).get_strs();
    std::vector<eds::slice_eds>& str_slices = eds.get_slice(i);

    // concat the strings with seperator & add add a terminator char
    core::join(i_letter, core::constants::string_separator, text);
    text += core::constants::terminator_char;

    // Create the suffix tree
    root = Create_suffix_tree(text.c_str(), text.length());

    // add string ids
    update_leaves(root, str_slices, eds, i);

    suffix_trees->push_back(meta_st{text, root, std::map<std::size_t, std::set<internal_st_vertex>>{} });
  }
}


} // namespace match_st
