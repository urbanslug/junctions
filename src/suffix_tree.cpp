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

using namespace std;


STvertex *root;
const char *txt;
int liscie; /* liczba utworzonych lisci */

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

// x should have a '$' at the end
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

void STDelete(STvertex *w) {
  FORE(it,w->g) STDelete(it->second.v);
  delete w;
}

bool Find(const char *query, STvertex *r, const char *x) {
  // from the start to end of the query
  int i = 0, query_len = strlen(query);
  int end = -1;
  while (i < query_len) {
    if (r->g.find(query[i]) == r->g.end())
      return false;
    STedge e = r->g[query[i]];

    // int start = e.l;
    // int end = e.r;

    // printf("i: %d start %d end %d \n", i, start, end);

    FOR(j, e.l, e.r) {
      // printf("j %d\n", j);
      end = j;
      if (i == query_len) {
        printf("j: %d\n", j);
        // break
        return true;
      }

      if (query[i++] != x[j])
        return false;
    }
    r = e.v;
  }
  printf("j -> %d\n", end);
  return true;
}

bool is_leaf (STvertex const *v) {
  return v->numer >= 0;
};

void print_vec(std::vector<int> &leaves) {
  for (auto l : leaves) {
    printf("%d ", l);
  }
}

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
 *
 */
// Like DFS
// gven a vertex walk down the leaves and return all the match loci
std::vector<junctions::match_locus> Get_Leaf_Data(STvertex const *current_vertex) {

  std::set<STvertex const *> explored;
  std::stack<STvertex const *> visited;

  // std::vector<std::pair<int, int>> leaves;
  std::vector<junctions::match_locus> leaves;

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
        leaves.push_back(junctions::match_locus{
            .string_index = current_vertex->string_id,
            .char_index = current_vertex->numer - current_vertex->string_id});
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
 * Mutates the suffix tree so that the leaves contain the string ID of the suffix...
 *
 * @param[in]
 * @param[in]
 */
void update_leaves(STvertex *current_vertex, std::vector<slicex> const *text_offsets) {

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

        for (size_t i = 0; i < text_offsets->size(); i++) {

          string_start = text_offsets->at(i).start;

          string_start += i; // account for $ separators


          // in the middle
          if (current_vertex->numer < string_start) {
            current_vertex->string_id = i - 1;
            break;
          }

          // suffix starts at an entire string
          // or
          // we are at the end of text offsets
          if (string_start == current_vertex->numer || i == text_offsets->size() - 1) {
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
 * @param[in] query the query string
 * @param[in] current_vertex  root of the suffix tree (can taeke another vertex)
 * @param[in] x the first char to the txt
 */
vector<junctions::extended_match> FindEndIndexesThree(const char *query,
                                                      STvertex *current_vertex,
                                                      const char *x) {

  std::vector<junctions::extended_match> matches;
  matches.reserve(strlen(x)); // is there a better value related to number of possible matches

  // TODO : rename i to q_idx or q_pos
  int i = 0; // the query position of the match
  int query_len = strlen(query);
  int match_length = 0;
  int d = -1;

  std::vector<junctions::match_locus> l_data;
  STvertex *last_with_underscore = NULL;
  STedge current_edge;
  bool has_dollar = false, has_underscore = false,
    has_qry_char = false, matched_a_char = false;

  auto looper = [&](std::vector<junctions::match_locus>& l_data, bool b = false, bool a = false) {
    for (auto l : l_data) {
      matches.push_back(
          junctions::extended_match{.beyond_text = b,
                                    .match_length = (a ? d : match_length),
                                    .str_idx = l.string_index,
                                    .chr_idx = l.char_index});
    }
  };

  auto append_matches = [&](bool b = false, bool a = false) {
    l_data = Get_Leaf_Data(current_edge.v);
    looper(l_data);
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

    // we matched at least one query to the end a text string
    // because we saw a $ sign and i > 0
    if (has_dollar && matched_a_char) {
      current_edge = current_vertex->g['$'];
      append_matches(true);
      if (!has_qry_char) { return matches; }
    }

    // we can no longer match the query and we found a _
    if (!has_qry_char && has_underscore && matched_a_char) {
      current_edge = current_vertex->g['_'];
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
        append_matches();
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a _
      // and have not finished processing the query
      if (x[j] == '_') {
        append_matches(true);
        append_underscore_matches();
        return matches;
      }

      // In the process of matching we find a $
      // and have not finished processing the query
      if (x[j] == '$') {
        append_matches();
        append_underscore_matches();
        return matches;
      }

      // find a mismatch
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

junctions::query_result FindEndIndexesTwo(const char *query, STvertex *current_vertex, const char *x) {

  STvertex* start_vertex = current_vertex;

  // from the start to end of the query
  int i = 0, query_len = strlen(query);
  int match_length = 0;
  // int end = -1;

  // printf("%d\n", query_len);

  // std::cerr << "qlen "  << query_len << std::endl;

  // std::vector<junctions::match_locus> empty_vec;
  // std::vector<junctions::match_locus> match_loci;

  junctions::query_result res = junctions::query_result();

  STvertex at_end;
  STvertex* previous_vertex = NULL;

  STvertex *last_with_underscore = NULL;

  bool has_end = false;

  auto foo = [&has_end](std::vector<junctions::match_locus> *results, STvertex* end_vertex) {
    if (has_end) {
      results->push_back(Get_Leaf_Data(end_vertex->g['_'].v).front());
    }
  };

  while (i < query_len) {

    // Found a match at the end of a ST
    // TODO: explain more

    // TODO: make these not be order dependent
    if (current_vertex->g.find(query[i]) == current_vertex->g.end() &&
        current_vertex->g.find('$') != current_vertex->g.end() && i > 0) {
      STedge current_edge = current_vertex->g['$'];
      res.match_length = match_length;
      res.results = Get_Leaf_Data(current_edge.v);

      res.beyond_text = true;
      return res;
    }

    if (current_vertex->g.find(query[i]) == current_vertex->g.end() &&
        current_vertex->g.find('_') != current_vertex->g.end() &&
        i > 0) {

      STedge current_edge;
      current_edge = current_vertex->g['_'];

      res.match_length = match_length;
      res.results = Get_Leaf_Data(current_edge.v);

      res.beyond_text = true;
      return res;
    }

    if (current_vertex->g.find('_') != current_vertex->g.end() && i > 0) {
      last_with_underscore = current_vertex;
    }

    if (current_vertex->g.find(query[i]) == current_vertex->g.end() &&
        previous_vertex != NULL &&
        previous_vertex->g.find('_') != previous_vertex->g.end()  &&
        i > 0) {

      STedge current_edge;
      current_edge = previous_vertex->g['_'];

      if (current_edge.v == NULL) { return res; }

      res.match_length = match_length;
      res.results = Get_Leaf_Data(current_edge.v);

      res.beyond_text = true;
      return res;
    }


    // matching failed
    if (current_vertex->g.find(query[i]) == current_vertex->g.end()) {

      // std::cerr << "1 " << std::endl;
      return res;
    }

    has_end = current_vertex->g.find('_') != current_vertex->g.end() && start_vertex != current_vertex;

    STedge current_edge = current_vertex->g[query[i]];

    // std::cerr << "l: " << current_edge.l << " r: " << current_edge.r << std::endl;
    // int start = e.l;
    // int end = e.r;

    // printf("i: %d start %d end %d \n", i, start, end);

    // std::cerr << "branching query[i]: " << query[i] << std::endl;

    FOR(j, current_edge.l, current_edge.r) {

      // std::cerr << "x[j]: " << x[j] << std::endl;

      // printf("j %d %c i: %d %c\n", j, x[j], i, query[i]);
      // end = j;

      // printf("match_length %d \n", i);

      // we have finished matching the query
      if (i == query_len) {
        res.match_length = match_length;
        res.results = Get_Leaf_Data(current_edge.v);

        foo(&res.results, current_vertex);

        return res;
      }

      // ends right at the end of the text
      if ((x[j] == '_' || x[j] == '$') && ((i == query_len) || current_edge.l == j)) {
        // printf("match_length %d \n", q);
        // match_loci = Get_Leaf_Data(current_edge.v);
        res.match_length = match_length;
        res.results = Get_Leaf_Data(current_edge.v);


        foo(&res.results, current_vertex);

        // res.beyond_text = false;
        return res;
        // return Get_Leaf_Data(current_edge.v);
      }

      // we have finished matching the suffix tree
      // but not the query
      // TODO: check for different chars other than _

      if (x[j] == '_' || x[j] == '$') {
        // printf("i: %d qlen: %d\n", i, query_len);
        // printf("match_length %d \n", q);
        // match_loci = Get_Leaf_Data(current_edge.v);
        res.match_length = match_length;
        res.results = Get_Leaf_Data(current_edge.v);
        // std::cerr << "here" << std::endl;

        if (x[j] == '_') { res.beyond_text = true; }

        foo(&res.results, current_vertex);

        // foo(&res.results, current_vertex);

        return res;
        // return Get_Leaf_Data(current_edge.v);
      }

      // char o= query[i++];
      // std::cerr << "===========" << o;
      // found a mismatch
      if (query[i++] != x[j]) {
        // printf("match_length %d \n", q);
        // std::cerr << "2 " << std::endl;

        // TODO: remove?
        if (last_with_underscore != NULL) {

          // std::cerr << "case 4" << std::endl;

          res.match_length = match_length;

          // std::cerr << "here" << std::endl;
          res.results = Get_Leaf_Data(last_with_underscore->g['_'].v);
        }

        return res;
        // return empty_vec;
      }

      match_length += 1;
    }

    previous_vertex = current_vertex;
    current_vertex = current_edge.v;
  }

  // printf("match_length %d \n", q);
  // return Get_Leaf_Data(current_vertex);
  res.match_length = match_length;
  res.results = Get_Leaf_Data(current_vertex);
  foo(&res.results, previous_vertex);

  return res;
}

vector<int> FindEndIndexes(const char *query, STvertex *current_vertex, const char *x) {
  // from the start to end of the query
  int i = 0, query_len = strlen(query);
  //int end = -1;

  std::vector<int> empty_vec;

  while (i < query_len) {
    if (current_vertex->g.find(query[i]) == current_vertex->g.end()) {
      return empty_vec;
    }

    STedge current_edge = current_vertex->g[query[i]];

    // int start = e.l;
    // int end = e.r;

    // printf("i: %d start %d end %d \n", i, start, end);

    FOR(j, current_edge.l, current_edge.r) {
      // printf("j %d\n", j);
      // end = j;
      if (i == query_len) {
        // printf("j: %d\n", j);
        // break
        // return j;
        return DFS(current_edge.v);
      }

      if (query[i++] != x[j])
        return empty_vec;
    }

    current_vertex = current_edge.v;
  }

  return DFS(current_vertex);
}
