#include "spvm_native.h"
#include<stdio.h>

#include<zlib.h>

int32_t SPNATIVE__MyZlib__test(SPVM_ENV* env, SPVM_VALUE* stack) {

  void* obj_out_file = stack[0].oval;
  if (!obj_out_file){ SPVM_DIE("File name must be specified", "MyZlib.c", __LINE__); }
  const char* out_file = (const char*)env->belems(env, obj_out_file);
  
  char buf[]="0123456789abcdefghijklmnopqrstuvwxyz\n";
  int cnt = 0;
  gzFile zp;

  zp = gzopen(out_file, "w9");
  if(zp == NULL){
      fprintf(stderr, "gzopen error\n");
      return -1;
  }
  for(cnt = 0; cnt < 100; cnt++){
    gzputs(zp, buf);
  }
  gzclose(zp);

  return 0;
}
