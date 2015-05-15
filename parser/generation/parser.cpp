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
using namespace std;

vector<string> rules;
map<string, int> symbol_id;
vector<string> symbols;
int empty_id;
map<int ,vector< vector<int> > > formulas;
vector< pair< int, string > > srcs;
vector< bitset< max_terminal > > firstset;
vector < vector < bitset < max_terminal > > > fml_firstset;


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
	  ret.set (-cur[i][0] + 1);
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

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      printf("parameter error!\n");
      return -1;
    }
  char frule[200];
  char fin[200];
  char fout[200];
  sprintf (frule, "%s", argv[0]);
  sprintf (fin, "%s", argv[1]);
  sprintf (fout, "%s", argv[2]);
  FILE* rule = fopen(frule, "r");
  load_rules(rule);
}
