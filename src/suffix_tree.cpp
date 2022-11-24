#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <stack>
#include <string>

#include "./core.hpp"

using namespace std;


/* Budowa drzewa sufiksowego algorytmem Ukkonena
 * (z przykladem wyszukiwania w nim slow)
 * Zlozonosc: O(n).
 */

struct STedge;
struct STvertex {
  map<char,STedge> g; /* edges to children */
  STvertex *f; /* suffix link */
  // suffix number
  // (0 is an inch word, -1 means that the vertex is not a leaf)
  /* numer sufiksu (0 to cale slowo, -1 oznacza ze wierzcholek
   * nie jest lisciem) */
  int numer;
};

struct STedge {
  int l, r; /* x[l]..x[r] is a piece of text that represents an edge */
  STvertex *v;
};

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
  top->numer = root->numer = -1;
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
