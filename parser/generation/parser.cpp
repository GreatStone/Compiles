#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include "parser.h"
#include <bitset>
#include <map>
#include <vector>
#include <string>
#include <stack>
using namespace std;

vector<string> rules;
map<string, int> symbol_id;
vector<string> symbols;
int empty_id;
map<int ,vector< vector<int> > > formulas;
vector< pair< int, string > > srcs;
vector< bitset< max_terminal > > firstset;
vector < vector < bitset < max_terminal > > > fml_firstset;

void load_src (FILE* in)
{
  char readin[200];
  while (!feof(in))
    {
      if (fgets (readin, 200, in) == NULL)
	{
	  return;
	}
      int id;
      char content[200];
      sscanf (readin, "< %d , %[^\n]", &id, readin);
      int len = strlen(readin);
      for (int i = 0; i < len-2; i++)
	{
	  content[i] = readin[i];
	}
      content[len-1] = 0;
      srcs.push_back(pair<int, string>(id,content));
    }
  return;
}

/*empty string be the final terminal symbol none*/
void load_rules (FILE* in)
{
  char readin[200];
  while (!feof(in))
    {
      if (fscanf(in, "%[^:]%*c", readin) < 1)
	break;
      sscanf(readin,"%s", readin);
      symbols.push_back(string(readin));
      symbol_id.insert ( make_pair ( string(readin), symbol_id.size()));
      int tsz = 0;
      char tmp;
      while (!feof(in) && (tmp = fgetc(in)) != ';')
	{
	  if (isspace(tmp))
	    {
	      continue;
	    }
	  if (tmp == '\\')
	    {
	      tmp = fgetc(in);
	    }
	  readin[tsz++] = tmp;
	}
      readin[tsz] = 0;
      rules.push_back(string(readin));
    }
  symbol_id.insert (make_pair ("none",symbol_id.size()));
  rules.push_back("");
  empty_id = symbol_id.size()-1;
}

/*
  save terminal symbol's id as -id-1
*/
int load_formulas()
{
  int i;
  for (i = 0; i < symbol_id.size(); i++)
    {
      vector < vector<int> > curformula;
      vector <int> inserted;
      int j;
      const char* ins = rules[i].c_str();
      for (j = 0; j < rules[i].length(); j++)
	{
	  char tmp_symbol[20];
	  if (ins[j] == '[')
	    {
	      sscanf(ins+j+1,"%[^]]",tmp_symbol);
	      int len = strlen(tmp_symbol);
	      j += len + 1;
	      int x;
	      sscanf(tmp_symbol,"%d",&x);
	      inserted.push_back (- x - 1);
	    }
	  else if (ins[j] == '<')
	    {
	      sscanf(ins+j+1,"%[^>]",tmp_symbol);
	      int len = strlen(tmp_symbol);
	      j += len+1;
	      map<string, int> :: iterator it;
	      it = symbol_id.find (string(tmp_symbol));
	      if (it == symbol_id.end())
		return -1;
	      inserted.push_back (it->second);
	    }
	  if (ins[j] == '|' || j == rules[i].length() -1)
	    {
	      curformula.push_back(inserted);
	      inserted.clear();
	    }
	}
      formulas.insert (make_pair(i, curformula));
    }
}

void gen_firstset ()
{
  char* vis = (char*) malloc(sizeof(char) * symbol_id.size());
  memset (vis, 0, sizeof(char) * symbol_id.size());
  int i,j,k;
  for (i = 0; i < symbol_id.size(); i++)
    {
      vector< bitset<max_terminal> > cur_fml_firstset;
      vector< vector<int> > & x = formulas.find(i)->second;
      bitset< max_terminal > symbol_fset;
      for (j = 0; j < x.size(); j++)
	{
	  bitset< max_terminal > inserted;
	  for (k = 0; k < x[j].size(); k++)
	    {
	      if (x[j][k] < 0)
		inserted.set(-x[j][k]-1);
	      else if (x[j][k] == empty_id)
		inserted.set(empty_id);
	      else
		inserted = gen_firstset(x[j][k], vis);
	      if (!inserted.none())
		break;
	    }
	  cur_fml_firstset.push_back(inserted);
	  symbol_fset = symbol_fset | inserted;
	}
      fml_firstset.push_back (cur_fml_firstset);
      firstset.push_back (symbol_fset);
    }
}

bitset < max_terminal > gen_firstset (int src, char* vis)
{
  bitset< max_terminal > ret;
  vis[src] = 1;
  vector< vector<int> > & cur = formulas.find(src)->second;
  int len = cur.size();
  int i;
  for (i = 0; i < len; i++)
    {
      if (cur[i].empty())
	continue;
      if (cur[i][0] < 0)
	{
	  ret.set (-cur[i][0] - 1);
	  continue;
	}
      else
	{
	  int j;
	  bitset< max_terminal > tmp;
	  for (j = 0; j < cur[i].size(); j++)
	    {
	      tmp = gen_firstset (cur[i][j], vis);
	      if (!tmp.none())
		{
		  ret = ret | tmp;
		  break;
		}
	    }
	}
    }
  vis[src] = 0;
  return ret;
}

int match ()
{
  int cur = 0;
  stack<int> sym_stk;
  sym_stk.push(0x3f3f3f3f);
  sym_stk.push(0);
  while (sym_stk.size() > 1)
    {
      int top = sym_stk.top();
      if (top < 0)
	{
	  if (cur == srcs.size())
	    {
	      return -1;
	    }

	  if (-top-1 == srcs[cur].first)
	    {
	      sym_stk.pop();
	      cur++;
	    }
	  else
	    {
	      return -1;
	    }
	}
      else
	{
	  if (cur == srcs.size())
	    {
	      if (firstset[top].test(empty_id))
		{
		  sym_stk.pop();
		  continue;
		}
	      else
		return -1;
	    }

	  int i;
	  int pass = 1;
	  int hasnone = 0;
	  for (i = 0; i < fml_firstset[top].size(); i++)
	    {
	      if (fml_firstset[top][i].test(empty_id))
		hasnone = 1;
	      if (fml_firstset[top][i].test(srcs[cur].first))
		{
		  pass = 0;
		  vector<int> & tmp = formulas.find(top)->second[i];
		  int j;
		  sym_stk.pop();
		  for (j = tmp.size()-1; j >= 0; j--)
		    {
		      sym_stk.push(tmp[j]);
		    }
		  break;
		}
	    }
	  if (pass && !hasnone)
	    {
	      return -1;
	    }
	  else if (pass && hasnone)
	    {
	      sym_stk.pop();
	    }
	}
    }
  if (sym_stk.size() != 1 || cur != srcs.size())
    return -2;
  return 0;
}

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      printf("parameter error!\n");
      return -1;
    }
  char frule[200];
  char fin[200];
  sprintf (frule, "%s", argv[1]);
  sprintf (fin, "%s", argv[2]);

  FILE* rule = fopen(frule, "r");
  load_rules(rule);
  load_formulas();
  fclose(rule);

  gen_firstset();

  FILE* in = fopen(fin, "r");
  load_src(in);
  fclose(in);
  
  int res = match();
  if (res == 0)
    {
      printf("Check pass: \n");
    }
  else
    {
      printf("Check not pass: \n");
    }
  int i;
  for (i = 0; i < srcs.size(); i++)
    {
      printf("%s ",srcs[i].second.c_str());
    }
  printf("\n");
}
