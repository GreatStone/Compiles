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
map<int ,vector< vector<int> > > formulas;
vector< pair< int, string > > srcs;


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
}

/*
  save terminal symbol's id as -id-1
*/
int load_formulas()
{
  int i;
  for (i = 0; i < symbol_id.size(); i++)
    {
      formulas.push_back(vector<int>());
    }
  for (i = 0; i < symbol_id.size(); i++)
    {
      vector < vector<int> > curformula;
      vector <int> inserted;
      int j;
      char* ins = rules[i].c_str();
      for (j = 0; j < rules[i].length(); j++)
	{
	  char tmp_symbol[20];
	  if (ins[j] == '[')
	    {
	      sscanf(ins+j+1,"%[^\]]",tmpsymbol);
	      int len = strlen(tmpsymbol);
	      j += len + 1;
	      int x;
	      sscanf(tmp_symbol,"%d",&x);
	      inserted.push_back (- x - 1);
	    }
	  else if (ins[j] == '<')
	    {
	      sscanf(ins+j+1,"%[^>]",tmpsymbol);
	      int len = strlen(tmpsymbol);
	      j += len+1;
	      map<int, vector< vector <int> > > :: iterator it;
	      it = formulas.find (string(tmpsymbol));
	      if (it == formulas.end())
		return -1;
	      inserted.push_back (it->first);
	    }
	  if (ins[j] == '|' || j == rules[i].length() -1)
	    {
	      curformula.push_back(inserted);
	    }
	}
      formulas.insert (make_pair(i, curformula));
    }
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
