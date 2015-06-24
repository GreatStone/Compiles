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
char relations[max_terminal][max_terminal];       //0 for equal, 1 for lt, 2 for gt, -1 for nothing

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
  int mark;
  int i;
  for (i = 0; i < symbols.size(); i++)
    {
      firstvt.insert (make_pair (i, bitset< max_terminal > ()));
    }
  while (1)
    {
      mark = 0;
      map< int, vector< vector< int > > >::iterator it;
      for (it = formulas.begin(); it != formulas.end(); it++)
	{
	  vector< vector< int > > & cur = it->second;
	  bitset< max_terminal > & ret = firstvt.find (it->first)->second;
	  for (i = 0; i < cur.size(); i++)
	    {
	      if (cur[i][0] < 0)
		{
		  int tmp = - cur[i][0] - 1;
		  if (!ret.test(tmp))
		    {
		      mark = 1;
		      ret.set(tmp);
		    }
		}
	      else if (cur[i].size() > 1 && cur[i][1] < 0)
		{
		  int tmp = - cur[i][1] - 1;
		  if (!ret.test(tmp))
		    {
		      mark = 1;
		      ret.set(tmp);
		    }
		}
	      
	      if (cur[i][0] >= 0)
		{
		  bitset< max_terminal > & bef = firstvt.find(cur[i][0])->second;
		  for (int j = 0; j < max_terminal; j++)
		    {
		      if (!ret.test(j) && bef.test(j))
			{
			  ret.set(j);
			  mark = 1;
			}
		    }
		}
	    }
	}
      if (!mark)
	break;
    }
}

void gen_lastvt ()
{
  int mark;
  int i;
  for (i = 0; i < symbols.size(); i++)
    {
      lastvt.insert (make_pair (i, bitset< max_terminal > ()));
    }
  while (1)
    {
      mark = 0;
      map< int, vector< vector< int > > >::iterator it;
      for (it = formulas.begin(); it != formulas.end(); it++)
	{
	  vector< vector< int > > & cur = it->second;
	  bitset< max_terminal > & ret = lastvt.find (it->first)->second;
	  for (i = 0; i < cur.size(); i++)
	    {
	      int len = cur[i].size();
	      if (cur[i][len-1] < 0)
		{
		  int tmp = - cur[i][len-1] - 1;
		  if (!ret.test(tmp))
		    {
		      mark = 1;
		      ret.set(tmp);
		    }
		}
	      else if (cur[i].size() > 1 && cur[i][len-2] < 0)
		{
		  int tmp = - cur[i][len-2] - 1;
		  if (!ret.test(tmp))
		    {
		      mark = 1;
		      ret.set(tmp);
		    }
		}
	      
	      if (cur[i][len-1] >= 0)
		{
		  bitset< max_terminal > & bef = lastvt.find(cur[i][len-1])->second;
		  for (int j = 0; j < max_terminal; j++)
		    {
		      if (!ret.test(j) && bef.test(j))
			{
			  ret.set(j);
			  mark = 1;
			}
		    }
		}
	    }
	}
      if (!mark)
	break;
    }
}

inline int get_terminal_id (int x)
{
  return - x - 1;
}

void gen_relation ()
{
  gen_firstvt ();
  gen_lastvt ();
  memset (relations, -1, sizeof (relations));
  int i, j, k;
  map< int, vector< vector< int > > >::iterator it;
  for (it = formulas.begin(); it != formulas.end(); it++)
    {
      vector< vector <int> > & cur = it->second;
      for (i = 0; i < cur.size(); i++)
	{
	  vector <int> & cur_rule = cur[i];
	  int len = cur_rule.size();
	  if (len  == 1)
	    continue;
	  for (j = 0; j < len - 1; j++)
	    {
	      if (cur_rule[j] < 0)
		{
		  if (j + 2 < len && cur_rule[j+2] < 0)
		    {
		      relations[get_terminal_id(cur_rule[j])][get_terminal_id(cur_rule[j+2])] = 0;
		    }
		  if (cur_rule[j+1] < 0)
		    {
		      relations[get_terminal_id(cur_rule[j])][get_terminal_id(cur_rule[j+1])] = 0;
		    }
		  else
		    {
		      bitset< max_terminal > & tmp = firstvt.find (cur_rule[j+1])->second;
		      for (k = 0; k < max_terminal; k++)
			{
			  if (tmp.test(k))
			    {
			      relations[get_terminal_id(cur_rule[j])][k] = 1;
			    }
			}
		    }
		}
	      else
		{
		  bitset< max_terminal > & tmp = lastvt.find(cur_rule[j])->second;
		  for (k = 0; k < max_terminal; k++)
		    {
		      if (tmp.test(k))
			{
			  relations[k][get_terminal_id(cur_rule[j+1])] = 2;
			}
		    }
		}
	    }
	}
    }
}

int match_and_replace (int l, int r, vector<int> & __stack)
{
  map< int, vector< vector< int > > >::iterator it;
  int len = r - l + 1;
  for (it = formulas.begin(); it != formulas.end(); it++)
    {
      vector< vector< int > > & cur = it->second;
      for (int i = 0; i < cur.size(); i++)
	{
	  vector< int > & cur_rule = cur[i];
	  if (cur_rule.size() != len)
	    continue;
	  int choose = 1;
	  for (int j = 0; j < cur_rule.size(); j++)
	    {
	      if (__stack[l+j] < 0)
		{
		  if (cur_rule[j] != __stack[l+j])
		    {
		      choose = 0;
		      break;
		    }
		}
	      else
		{
		  if (cur_rule[j] < 0)
		    {
		      choose = 0;
		      break;
		    }
		}
	    }
	  if (choose)
	    {
	      if (len == 1)
		__stack[l] = it->first;
	      else
		{
		  __stack[l] = it->first;
		  __stack.erase (__stack.begin() + l + 1, __stack.begin() + r + 1);
		}
	      return 0;
	    }
	}
    }
  return -1;
}

int statute ()
{
  vector<int> __stack;
  int i;
  for (i = 0; i < srcs.size(); i++)
    {
      __stack.push_back (-srcs[i].first-1);
    }
  while (__stack.size() > 1 || __stack[0] < 0)
    {
      int r = __stack.size() - 1;
      for (i = 0; i < __stack.size() - 1; i++)
	{
	  if (__stack[i] >= 0)
	    continue;
	  if (__stack[i+1] >= 0)
	    {
	      if (i+2 < __stack.size() && relations[get_terminal_id(__stack[i])][get_terminal_id(__stack[i+2])] == 2)
		{
		  r = i+1;
		  break;
		}
	    }
	  else if (relations[get_terminal_id(__stack[i])][get_terminal_id(__stack[i+1])] == 2)
	    {
	      r = i;
	      break;
	    }
	}
      int l = 0;
      for (i = r; i > 0; i--)
	{
	  if (__stack[i] >= 0)
	    continue;
	  if (__stack[i-1] >= 0)
	    {
	      if (i-2 >= 0 && relations[get_terminal_id(__stack[i-2])][get_terminal_id(__stack[i])] == 1)
		{
		  l = i-1;
		  break;
		}
	    }
	  if (relations[get_terminal_id(__stack[i-1])][get_terminal_id(__stack[i])] == 1)
	    {
	      l = i;
	      break;
	    }
	}
      if (match_and_replace (l, r, __stack))
	return -1;
    }
  return 0;
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
  FILE *fp = fopen (rule, "r");
  load_rules (fp);
  load_formulas ();
  gen_relation ();
  fclose (fp);
  
  fp = fopen (src,"r");
  load_src (fp);
  fclose (fp);
  for (int i = 0; i < srcs.size(); i++)
    {
      cout << srcs[i].second << " ";
    }
  cout << endl;
  if (statute ())
    {
      cout << "Can't be statute" << endl;
    }
  else
    {
      cout << "Can be statute" << endl;
    }
  return 0;
}
