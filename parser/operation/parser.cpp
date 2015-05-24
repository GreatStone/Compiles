#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <bitset>
#include "parser.h"
using namespace std;

vector< pair< int, string > > srcs;
map< string, int > symbol_id;
vector< string > symbols;
vector< string > rules;
map< int, vector< vector< int > > > formulas;
map< int, bitset< max_terminal > > firstvt;
map< int, bitset< max_terminal > > lastvt;

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

/*
  generate firstvt set
*/
void gen_firstvt ()
{
  
}

void gen_firstvt (int v)
{
  
}

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      printf("parameter error!\n");
      return -1;
    }
  char rule[200];
  char src[200];
  sprintf(rule, "%s", argv[1]);
  sprintf(src, "%s", argv[2]);
  
  return 0;
}
