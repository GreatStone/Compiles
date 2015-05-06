#ifndef LEXER_GREATSTONE_H
#define LEXER_GREATSTONE_H

#include <iostream>
#include <vector>
using namespace std;

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

#endif
