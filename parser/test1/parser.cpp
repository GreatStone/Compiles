#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "parser.h"
#include <vector>
using namespace std;

vector< pair<int, string> > contents;
int cur;

int read_tuple (FILE* in, vector< pair<int, string> >& save)
{
  if (feof (in))
    return 0;
  char tmp[200];
  if (fgets (tmp, 200, in) == NULL)
    {
      return 0;
    }
  int id;
  char content[200];
  sscanf (tmp, "< %d , %[^\n]", &id, tmp);
  int len = strlen(tmp);
  for (int i = 0; i < len-2; i++)
    {
      content[i] = tmp[i];
    }
  content[len-1] = 0;
  save.push_back(pair<int, string>(id,content));
  return 1;
}

int accept_E ()
{
  if (cur >= contents.size())
    return 0;
  return accept_T()&& accept_E2();
}

int accept_E2 ()
{
  if (cur > contents.size())
    return 0;
  else if (cur == contents.size())
    return 1;
  if (contents[cur].first == ADD || contents[cur].first == SUB)
    {
      return accept_A() && accept_T() && accept_E2();
    }
  else
    return 1;
}
int accept_T ()
{
  if (cur >= contents.size())
    return 0;
  return accept_F() && accept_T2();
}

int accept_T2 ()
{
  if (cur > contents.size())
    return 0;
  else if (cur == contents.size())
    return 1;
  if (contents[cur].first == MUL || contents[cur].first == DIV)
    {
      return accept_M() && accept_F() && accept_T2();
    }
  else
    return 1;
}

int accept_F ()
{
  if (cur >= contents.size())
    return 0;
  if (contents[cur].first == LPARENT)
    {
      cur++;
      int tres = accept_E();
      if (!tres)
	return tres;
      if (contents[cur].first == RPARENT)
	{
	  cur++;
	  return 1;
	}
      else
	return 0;
    }
  else
    {
      if (contents[cur].first == VAR)
	{
	  cur++;
	  return 1;
	}
      else
	return 0;
    }
}

int accept_A ()
{
    if (cur >= contents.size())
      return 0;
    if (contents[cur].first == ADD || contents[cur].first == SUB)
      {
	cur++;
	return 1;
      }
    else
      return 0;
}

int accept_M ()
{
    if (cur >= contents.size())
    return 0;
    if (contents[cur].first == MUL || contents[cur].first == DIV)
      {
	cur++;
	return 1;
      }
    else
      return 0;
}


int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      printf("parameters error");
      return -1;
    }
  char fin[200];
  char fout[200];
  sprintf(fin, "%s", argv[1]);
  sprintf(fout, "%s" ,argv[2]);
  FILE* in = fopen(fin, "r");
  while (read_tuple (in, contents));
  cout << "contents end" << endl;
  cur = 0;
  FILE* out = fopen(fout, "w");
  while (cur != contents.size())
    {
      int bef = cur;
      if ( accept_E() )
	{
	  fprintf(out, "pass:\n");
	  cout << "pass" << endl;
	  for (int i = bef; i < cur; i++)
	    {
	      fprintf(out, "%s ", contents[i].second.c_str());
	      printf("%s ", contents[i].second.c_str());
	    }
	  fprintf(out,"\n");
	  cout << endl;
	}
      else
	{
	  fprintf(out, "not pass:\n");
	  cout << "npass" << endl;
	  for (int i = bef; i < contents.size(); i++)
	    {
	      fprintf(out, "%s ", contents[i].second.c_str());
	      cout << contents[i].second << " ";
	    }
	  fprintf(out,"\n");
	  cout << endl;
	  break;
	}
    }
}
