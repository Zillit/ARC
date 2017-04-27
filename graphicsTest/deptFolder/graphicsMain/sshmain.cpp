#include <libssh/libssh.h> 
#include <stdlib.h>
#include <iostream>
using namespace std;
int main()
{
  ssh_session my_ssh_session = ssh_new();
  if (my_ssh_session == NULL)
    exit(-1);
  cout << "test" << endl;
  ssh_free(my_ssh_session);
}
