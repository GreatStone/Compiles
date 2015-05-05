#ifndef LEXER_GREATSTONE_H
#define LEXER_GREATSTONE_H

#include <iostream>
#include <vector>
using namespace std;

class edge{
 private:
  int v;
  /*
    accept
    0 for empty string
   */
  char accept;
 public:
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
  vector<edge>::iterator getalledge(int u);
};

graph genNFA(FILE* in);

#endif
