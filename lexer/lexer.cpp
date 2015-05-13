#include "lexer.h"
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
#include <stack>
using namespace std;

typedef pair<int,int> pii;

int env_sz;
map<string,int> env_symbols;
vector<string> rules;
map<int,int> end_sym_id;
map<int,int> end_id_sym;
int* dfa_ter_type;
int (* matrix)[128];

typedef struct cmp_bitset
{
  int operator () (const bitset<max_node>& a, const bitset<max_node>& b) const
  {
    return a.to_string() < b.to_string();
  }
} cmp_bitset;

int graph::newnode ()
{
  save.push_back(vector<edge> ());
  return save.size()-1;
}

void graph::adde (int u, int v, char c)
{
  save[u].push_back(edge(v,c));
}

int graph::node_sz ()
{
  return save.size();
}

vector<edge>::iterator graph::edge_begin (int u)
{
  return save[u].begin();
}

vector<edge>::iterator graph::edge_end (int u)
{
  return save[u].end();
}

/* just for DEBUG, can be removed */
void show (bitset<max_node> src){
  printf("Show\n");
  int i;
  for (i = 0; i < max_node; i++)
    {
      if (src.test(i)){
	printf(" %d ",i);
      }
    }
  printf("\n");
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
	  i += strlen(tmp_sym)+1;
	  if (tmp_it == cursymbols.end())
            {
	      int tmp_s = fa.newnode();
	      int tmp_e = fa.newnode();
	      if (i == len-1 || rule[i+1] == '|')
                {
		  cursymbols.insert (make_pair(string(tmp_sym), tmp_s));
                }
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
	  if (i == len-1){
	    fa.adde(curid,end,0);
	  }
        }
      else if (rule[i] == '|')
        {
	  fa.adde(curid,end,0);
	  curid = start;
        }
      else if (rule[i] == '\\')
        {
	  i++;
	  if (i == len - 1)
            {
	      int tmp = fa.newnode();
	      fa.adde (curid, tmp, rule[i]);
	      fa.adde (tmp, end, 0);
            }
	  else
            {
	      int tmp = fa.newnode();
	      fa.adde (curid, tmp, rule[i]);
	      curid = tmp;
            }
        }
      else if (i == len - 1)
        {
	  int tmp = fa.newnode();
	  fa.adde (curid, tmp, rule[i]);
	  fa.adde (tmp, end, 0);
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
  map<string,int> :: iterator it;
  for (it = env_symbols.begin(); it != env_symbols.end(); it++)
    {
      map<string,int> cursymbols;
      /*
        rules : if some symbol start with ' then it will not be join into dfa, but just for write rules more easy.
      */
      if (it->first[0] == '\'')
        {
	  continue;
        }
      int s = fa.newnode();
      int e = fa.newnode();
      cursymbols.insert(make_pair(it->first, s));
      fa.adde(start,s,0);
      genNFA (fa, it->second, cursymbols, s, e);
      end_sym_id.insert(make_pair(it->second,e));
      end_id_sym.insert(make_pair(e,it->second));
    }
}

/*
  res0: the set of nodes, which has been visited after accept the acchar(include).
  res1: the set of nodes, which has been visited before accept the acchar.
*/
void dfs (graph& nfa, int src, int rest, int acchar, bitset<max_node> & res0, bitset<max_node> & res1)
{
  vector<edge>::iterator it;
  for (it = nfa.edge_begin(src); it != nfa.edge_end(src); it++)
    {
      int v = it->v;
      if (res0.test(v) && res1.test(v))
	continue;
      else if (it->accept == acchar)
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

/*
  this function will not make change to nfa.
  dfa: a new graph with none.
*/
void con_NFA_DFA (graph& nfa, graph& dfa)
{
  bitset<max_node> start;
  map< bitset<max_node>, int, cmp_bitset> dfa_node_id;
  queue< bitset<max_node> > que;
  start.set(0);
  que.push(start);
  dfa_node_id.insert (make_pair(start,0));
  dfa.newnode();

  bitset<max_node> in0;
  bitset<max_node> in1;
  while (!que.empty())
    {
      bitset<max_node> cur = que.front();
      que.pop();
      int i,j;
      for (i = 32; i < 127; i++)
        {
	  in0.reset();
	  in1.reset();
	  for (j = 0; j < max_node; j++)
            {
	      if (in0.test(j) && in1.test(j))
		continue;
	      if (cur.test(j))
                {
		  dfs(nfa, j, 1, i, in0, in1);
                }
            }
	  if (in0.none())
	    continue;
	  map< bitset<max_node>, int> :: iterator it;
	  it = dfa_node_id.find(in0);
	  if (it == dfa_node_id.end())
            {
	      dfa_node_id.insert(make_pair(in0, dfa_node_id.size()));
	      int e = dfa.newnode();
	      dfa.adde(dfa_node_id.find(cur)->second, e, i);
	      que.push(in0);
            }
	  else
            {
	      dfa.adde(dfa_node_id.find(cur)->second, it->second, i);
            }
        }
    }
  dfa_ter_type = new int[dfa.node_sz()];
  memset (dfa_ter_type, -1, sizeof(int) * dfa.node_sz());
  for (map< bitset<max_node> ,int> :: iterator it = dfa_node_id.begin(); it != dfa_node_id.end(); it++)
    {
      int e = isend(it->first);
      if (e < 0)
	continue;
      dfa_ter_type[it->second] = e;
    }
  matrix = new int[dfa.node_sz()][128];
  memset (matrix, -1, sizeof(int) * dfa.node_sz() * 128);
  int i;
  for (i = 0; i < dfa.node_sz(); i++)
    {
      for (vector<edge>::iterator it = dfa.edge_begin(i); it != dfa.edge_end(i); it++)
        {
	  matrix[i][it->accept] = it->v;
        }
    }
}

/*
  return the type of terminal node in dfa. types are numbered from 0, include the symbol start with "'".
*/
int isend(const bitset<max_node>& nodeset)
{
  /* init the set of terminal node*/
  static bitset<max_node> ends;
  if (ends.none())
    {
      map<int, int> :: iterator it;
      for (it = end_id_sym.begin(); it != end_id_sym.end(); it++)
        {
	  ends.set(it->first);
        }
    }
  bitset<max_node> test = ends & nodeset;
  if (test.none())
    return -1;
  int i;
  int ret = -1;
  for (i = 0; i < max_node; i++)
    {
      if (test.test(i))
        {
	  if (ret == -1)
            {
	      ret = end_id_sym.find(i)->second;
            }
	  else
            {
	      int tmp = end_id_sym.find(i)->second;
	      if (tmp < ret)
		ret = tmp;
            }
        }
    }
  return ret;
}

int match (FILE*& in, FILE*& out, graph& dfa)
{
  char readin;
  int state = 0;
  queue<char> bck_buf;
  int res = -1;
  char buffer[200];
  int tsz = 0;
  while (!feof(in))
    {
      if (bck_buf.empty())
	{
	  readin = fgetc(in);
	  readin = tolower(readin);
	}
      else
        {
	  readin = bck_buf.front();
	  bck_buf.pop();
        }

      if (readin == EOF)
	break;

      /*
        special handle for comment
      */
      if (readin == '/' && tsz == 0)
        {
	  readin = fgetc(in);
	  if (readin == '*')
            {
	      while (true){
		char e = fgetc(in);
		if (e == '*')
		  {
		    if (feof(in))
		      return -1;
		    e = fgetc(in);
		    if (e == '/')
		      {
			if (feof(in))
			  return 0;
			readin = fgetc(in);
			break;
		      }
		    else
		      fseek(in,-1,SEEK_CUR);
		  }
	      }
            }
	  else
            {
	      fseek(in,-1,SEEK_CUR);
            }
        }

      if (isspace(readin) && state == 0)
	continue;
      if (res != -1)
	bck_buf.push(readin);
      buffer[tsz++] = readin;
      state = matrix[state][readin];
      if (res == -1 && state == -1)
	return -1;
      else if (res != -1 && state== -1)
        {
	  buffer[tsz] = 0;
	  int len = strlen(buffer);
	  buffer[len - bck_buf.size()] = 0;
	  fprintf(out,"< %d , %s >\n", res, buffer);
	  tsz = 0;
	  res = -1;
	  state = 0;
        }
      else if (res != -1 && dfa_ter_type[state] != -1)
        {
	  while (!bck_buf.empty()) bck_buf.pop();
	  res = dfa_ter_type[state];
        }
      else if (res != -1 && dfa_ter_type[state] == -1)
	continue;
      else if (res == -1 && dfa_ter_type[state] != -1)
        {
	  res = dfa_ter_type[state];
        }
    }
  if (tsz && dfa_ter_type[state] != -1)
    {
      buffer[tsz] = 0;
      fprintf(out,"< %d , %s >\n", res, buffer);
    }
  else if (tsz)
    return -1;

  return 0;
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
  graph dfa;
  genNFA(nfa);
  con_NFA_DFA(nfa,dfa);
  printf("Genarate DFA successfully.\n");

  in = fopen(src,"r");
  FILE* out = fopen(output,"w");
  if (match(in,out,dfa) == -1)
    {
      printf("There're some errors in source file.\n");
      fclose(in);
      fclose(out);
      return -1;
    }
  fclose(in);
  fclose(out);
  printf("Result successfully write into %s\n",output);
  return 0;
}
