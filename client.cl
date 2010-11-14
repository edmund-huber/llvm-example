
extern int host_method(int);

int client_method(int x){
  return x * host_method(x);
}
