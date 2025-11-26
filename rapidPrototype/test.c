#include <stdlib.h>

int main(int argx,char**argv)
{
  char command[80];
  int tag;

  tag = atoi(argv[1]);

  snprintf(command,sizeof(command),
    "netcat -l -u -p 8001 | ./analyzer -B 500 -t %d",tag);

  system(command);

  return (0);

} // main
