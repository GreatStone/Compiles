#ifndef PARSER_GREATSTONE_H
#define PARSER_GREATSTONE_H

#include <iostream>
#include <bitset>
using namespace std;

const int max_terminal = 1000;

void load_rules (FILE* in);

int load_formulas();

void gen_firstset ();

bitset < max_terminal > gen_firstset (int src, char* vis);

#endif
