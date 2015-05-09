#ifndef LEXER_GREATSTONE_H
#define LEXER_GREATSTONE_H

#include <iostream>
#include <vector>
#include <bitset>
#include <map>
using namespace std;

const int max_node = 1000;

class edge{
 public:
  int v;
  /*
    accept
    0 for empty string
   */
  char accept;
  edge(){}
  edge(int a, char b)
    {
      v = a; accept = b;
    }
};

class graph{
 private:
  vector< vector<edge> > save;
 public:
  int newnode();
  void adde(int u, int v, char c);
  vector<edge>::iterator edge_begin(int u);
  vector<edge>::iterator edge_end(int u);
  int node_sz();
};

void pre_read (FILE* & in);

int isend(const bitset<max_node>& nodeset);

void con_NFA_DFA (graph& nfa, graph& dfa);

void dfs (graph& nfa, int src, int rest, int acchar, bitset<max_node> & res0, bitset<max_node> & res1);

void genNFA (graph& fa);

void genNFA (graph& fa, int id, map<string,int>& cursymbols, int start, int end);

#endif
