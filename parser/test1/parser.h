#ifndef PARSER_GREATSTONE_H
#define PARSER_GREATSTONE_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
using namespace std;

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
#define LPARENT 4
#define RPARENT 5
#define VAR 6

pair<int, string> read_tuple (FILE* in);

int accept_E ();

int accept_E2 ();

int accept_T ();

int accept_T2 ();

int accept_F ();

int accept_A ();

int accept_M ();

#endif
