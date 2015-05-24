#ifndef PARSER_GREATSTONE_H
#define PARSER_GREATSTONE_H

const int max_terminal = 1000;

class matrix{
 private:
  int m,n;
  int* save;
  int error;
 public:
  matrix(int a, int b)
    {
      m = a;
      n = b;
      save = (int*) malloc (sizeof(int)*a*b);
      int i;
      for(i = 0; i < a * b; i++)
	{
	  save[i] = i;
	}
    }
  int& get(int x, int y)
  {
    if (x >= m || y >= n)
      return error;
    return save[x*n + y];
  }
};


#endif
