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
#include "lexer.h"
using namespace std;

int sz;
map<string,int> symbols;
map<string,int> symbols_e;

int graph::newnode ()
{
  save.push_back(vector<edge> ());
  return save.size()-1;
}

void graph::adde (int u, int v, char c)
{
  save[u].push_back(edge(v,c));
}

graph genNFA (FILE* in)
{
  sz = 0;
  graph ret;
  char cur_symbol[200];
  int sid = ret.newnode();
  while (!feof(in))
    {
      if (fscanf(in,"%s",cur_symbol) < 1)
	{
	  break;
	}

      int len = strlen(cur_symbol);
      if (cur_symbol[len-1] == ':')
	{
	  cur_symbol[len-1] = 0;
	}
      else
	{
	  char tmp;
	  while ((tmp = fgetc(in)) != ':' && tmp != EOF);
	  if (tmp == EOF)
	    {
	      printf("error: symbol not completly\n");
	      return ret;
	    }
	}

      int cur_s_id;
      int cur_e_id;
      string tmp(cur_symbol);
      map<string,int>:: iterator it;
      it = symbols.find(tmp);
      if (it == symbols.end())
	{
	  cur_s_id = sz;
	  symbols.insert(make_pair(tmp, sz++));
	  cur_e_id = sz;
	  symbols_e.insert(make_pair(tmp,sz++));
	}
      else
	{
	  cur_s_id = it->second;
	  cur_e_id = symbols_e.find(tmp)->second;
	}
      ret.adde(sid,cur_s_id,0);

      char curin;
      int tmp_id = cur_s_id;
      while ((curin = fgetc(in)) != EOF)
	{
	  if (isspace(curin))
	    continue;
	  else if (curin == ';')
	    break;
	  else if (curin == '|')
	    {
	      ret.adde(tmp_id,cur_e_id,-1);
	      tmp_id = cur_s_id;
	    }
	  else if (curin == '\\')
	    {
	      curin = fgetc(in);
	      if (curin == EOF)
		break;
	      if (curin == 's')
		curin = ' ';
	      else if (curin == 't')
		curin = '\t';
	      else if (curin == 'n')
		curin = '\n';
	      int e = ret.newnode();
	      ret.adde(tmp_id, e, curin);
	      tmp_id = e;
	    }
	  else if (curin == '\'')
	    {
	      char nxt[200];
	      char end;
	      fscanf(in,"%[^|;]%c",nxt,&end);
	      sscanf(nxt,"%s",nxt);
	      it = symbols.find(string(nxt));
	      int e_s;
	      int e_e;
	      if (it == symbols.end())
		{
		  e_s = sz;
		  symbols.insert(make_pair(string(nxt),sz++));
		  e_e = sz;
		  symbols_e.insert(make_pair(string(nxt),sz++));
		}
	      else
		{
		  e_s = it->second;
		  e_e = symbols_e.find(string(nxt))->second;
		}
	      ret.adde(tmp_id,e_s,0);
	      ret.adde(e_e,cur_e_id,0);
	    }
	  else
	    {
	      int e = ret.newnode();
	      ret.adde(tmp_id, e, curin);
	      tmp_id = e;
	    }
	}
    }
}

int main(int argc, char * argv[])
{
  printf("%d\n",argc);
  if (argc != 3)
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
  
  
  return 0;
}
