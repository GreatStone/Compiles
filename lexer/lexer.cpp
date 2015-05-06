#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <string>
#include <vector>
#include <ctype.h>
#include <map>
#include <bitset>
#include <queue>
#include "lexer.h"
using namespace std;

typedef pair<int,int> pii;

int env_sz;
map<string,int> env_symbols;
vector<string> rules;
vector<int> s_symbols;
map<int,int> end_sym_id;
map<int,int> end_id_sym;
map<int,int> end_dfa_nfa;
map<int,int> end_nfa_dfa;

const int max_node = 1000;

int graph::newnode ()
{
  save.push_back(vector<edge> ());
  return save.size()-1;
}

void graph::adde (int u, int v, char c)
{
  save[u].push_back(edge(v,c));
}

int graph::node_sz () const
{
  return save.size();
}

vector<edge>::iterator graph::edge_begin (int u) const
{
  return save[u].begin();
}

vector<edge>::iterator graph::edge_end (int u) const
{
  return save[u].end();
}

void pre_read (FILE* & in)
{
  char sym_in[100];
  char rule_in[500];
  while (!feof(in))
    {
      fscanf(in,"%[^:]%*c",sym_in);
      sscanf(sym_in,"%s",sym_in);
      char c_in;
      int tsz = 0;
      while ((c_in = fgetc(in)) != EOF && c_in != ';')
	{
	  if (isspace (c_in))
	    continue;
	  rule_in[tsz++] = c_in;
	  if (c_in == '\\')
	    {
	      c_in = fgetc(in);
	      rule_in[tsz++] = c_in;
	    }
	}
      rule_in[tsz] = 0;
      env_symbols.insert(make_pair(string(sym_in),env_sz++));
      rules.push_back(string(rule_in));
    }
}

/*
  end should be start+1(for finding end easy)
*/
void genNFA (graph& fa, int id, map<string,int>& cursymbols, int start, int end)
{
  char rule[300];
  sprintf(rule,"%s",rules[id].c_str());
  int len = strlen(rule);
  int i;
  int curid = start;
  map<string,int> :: iterator tmp_it;
  for (i = 0; i < len; i++)
    {
      if (rule[i] == '<')
	{
	  char tmp_sym[200];
	  sscanf(rule+i+1,"%[^>]",tmp_sym);
	  tmp_it = cursymbols.find(string(tmp_sym));
	  if (tmp_it == cursymbols.end())
	    {
	      int tmp_s = fa.newnode();
	      int tmp_e = fa.newnode();
	      cout << tmp_sym << " , " << tmp_s << endl;
	      cursymbols.insert (make_pair(string(tmp_sym), tmp_s));
	      fa.adde (curid, tmp_s, 0);
	      curid = fa.newnode();
	      fa.adde (tmp_e, curid, 0);
	      genNFA (fa, env_symbols.find(string(tmp_sym))->second, cursymbols, tmp_s, tmp_e);
	    }
	  else
	    {
	      fa.adde (curid, tmp_it->second, 0);
	      curid = fa.newnode();
	      fa.adde (tmp_it->second+1, curid, 0);
	    }
	}
      else if (rule[i] == '|')
	{
	  fa.adde (curid, end, 0);
	  curid = start;
	}
      else if (rule[i] == '\\')
	{
	  i++;
	  int tmp = fa.newnode();
	  fa.adde (curid, tmp, rule[i]);
	  curid = tmp;
	}
      else if (i == len - 1)
	{
	  fa.adde (curid, end, 0);
	}
      else
	{
	  int tmp = fa.newnode();
	  fa.adde (curid, tmp, rule[i]);
	  curid = tmp;
	}
    }
}

void genNFA (graph& fa)
{
  int start = fa.newnode();
  int end = fa.newnode();
  int i;
  map<string,int> cursymbols;
  map<string,int> :: iterator it;
  for (it = env_symbols.begin(); it != env_symbols.end(); it++)
    {
      cursymbols.insert(*it);
      int s = fa.newnode();
      int e = fa.newnode();
      cout << it->first << " , " << s << endl;
      fa.adde(start,s,0);
      fa.adde(e,end,0);
      genNFA (fa, it->second, cursymbols, s, e);
      end_sym_id.insert(make_pair(it->second,e));
      end_id_sym.insert(make_pair(e,it->second));
    }
}

void con_NFA_DFA (const graph& nfa, graph& dfa)
{
  bitset<max_node> start;
  map< bitset<max_node>, int> new_node_id;
  queue< bitset<max_node> > que;
  start.set(0);
  que.push(start);
  new_node_id.insert (make_pair(start,0));
  dfa.newnode();
  
  while (!que.empty())
    {
      bitset<max_node> cur = que.front();
      que.pop();
      int i;
      for (i = 0; i < max_node; i++)
	{
	  if (start.test(i) && (!in0.test(i) || !in1.test(i)))
	    {
	      for (int j = 32; j < 127; j++)
		{
		  bitset<max_node> in0;
		  bitset<max_node> in1;
		  dfs (nfa, i, 1, j, in0, in1);
		  if (in0.none())
		    continue;
		  it = new_node_id.find (in0);
		  if (it == new_node_id.end())
		    {
		      new_node_id.insert(make_pair(in0, new_node_id.size()));
		      int e = dfa.newnode();
		      dfa.adde(cur, e, j);
		    }
		}
	    }
	}
    }
}

void dfs (const graph& nfa, int src, int rest, int acchar, bitset<max_node> & res0, bitset<max_node> & res1)
{
  vector<edge>::iterator it;
  for (it = nfa.edge_begin(src); it != nfa.edge_end(src); it++)
    {
      int v = it->v;
      if (res0.test(v) && res1.test(v))
	continue;
      else if (it->accept != 0 && it->accept == acchar)
	{
	  if (rest == 0)
	    continue;
	  else
	    {
	      if (res0.test(v))
		continue;
	      res0.set(v);
	      dfs(nfa, v, 0, acchar, res0, res1);
	    }
	}
      else if (it->accept == 0)
	{
	  if (rest)
	    {
	      if (res1.test(v))
		continue;
	      res1.set(v);
	    }
	  else
	    {
	      if (res0.test(v))
		continue;
	      res0.set(v);
	    }
	  dfs(nfa, v, rest, acchar, res0, res1);
	}
    }
}

int main (int argc, char * argv[])
{
  if (argc != 4)
    {
      printf("parameters error");
      return -1;
    }
  char rule[200];
  char src[200];
  char output[200];
  sprintf(rule,"%s",argv[1]);
  sprintf(src,"%s",argv[2]);
  sprintf(output,"%s",argv[3]);
  FILE* in = fopen(rule,"r");
  env_sz = 0;
  pre_read(in);
  fclose(in);
  graph nfa;
  genNFA(nfa);
  for (int i = 0; i < nfa.node_sz(); i++)
    {
      vector<edge>::iterator it = nfa.edge_begin(i);
      cout << "from " << i << endl;
      for (; it != nfa.edge_end(i); it++)
	{
	  if (it->accept == 0)
	    cout << "to " <<it->v << " with " << "space" << endl;
	  else
	    cout << "to " <<it->v << " with " << (it->accept) << endl;
	}
    }
  return 0;
}
