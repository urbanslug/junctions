#include <string>
#include <map>
#include <cstring>
#include "./tmp.cpp"

using namespace std;

/* Budowa drzewa sufiksowego algorytmem Ukkonena
 * (z przykladem wyszukiwania w nim slow)
 * Zlozonosc: O(n).
 */

struct STedge;
struct STvertex
{
  map<char,STedge> g; /* krawedzie do synow */
  STvertex *f; /* suffix link */
  /* numer sufiksu (0 to cale slowo, -1 oznacza ze wierzcholek
   * nie jest lisciem) */
  int numer;
};
struct STedge
{
  int l,r; /* x[l]..x[r] to fragment tekstu reprezentujacy krawedz */
  STvertex *v;
};

STvertex *root;
const char *txt;
int liscie; /* liczba utworzonych lisci */

inline void Canonize(STedge &kraw,const char *x)
{
  if (kraw.l<=kraw.r)
  {
    STedge e=kraw.v->g[x[kraw.l]];
    while (e.r-e.l <= kraw.r-kraw.l)
    {
      kraw.l+=e.r-e.l+1;
      kraw.v=e.v;
      if (kraw.l<=kraw.r) e=kraw.v->g[x[kraw.l]];
    }
  }
}

inline bool Test_and_split(STvertex* &w,const STedge &kraw)
{
  w=kraw.v;
  if (kraw.l<=kraw.r)
  {
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

void Update(STedge &kraw,int n)
{
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

/* x powinna miec na koncu jakis '$' */
STvertex* Create_suffix_tree(const char *x,int n)
{
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

void STDelete(STvertex *w)
{
  FORE(it,w->g) STDelete(it->second.v);
  delete w;
}

bool Find(const char *s,STvertex *r,const char *x)
{
  int i=0,n=strlen(s);
  while (i<n)
  {
    if (r->g.find(s[i]) == r->g.end()) return false;
    STedge e=r->g[s[i]];
    FOR(j,e.l,e.r)
    {
     if (i==n) return true;
     if (s[i++]!=x[j]) return false;
    }
    r=e.v;
  }
  return true;
}

