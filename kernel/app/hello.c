#include <bareio.h>
#include <barelib.h>


/*
 * 'builtin_hello' prints "Hello, <text>!\n" where <text> is the contents 
 * following "builtin_hello " in the argument and returns 0.  
 * If no text exists, print and error and return 1 instead.
 */
byte builtin_hello(char* arg) {
  
  char txt[2048];
  int ptr=0;
  //int x=0;
  int space=0;
  for(int i=5;i<2048;i++){
    if(space==0&&arg[i]!=' '){
      space=1;
    }
    if(space==0&&arg[i]==' '){
      continue;
    }
    if((i==5)&&arg[i]=='\0'){
      printf("Error - bad argument\n");
      return 1;
    }
    else{
      txt[ptr]=arg[i];
      ptr++;
    }
  }
  printf("Hello, ");
  printf(txt);
  printf("!\n");
  // while(arg[x]!='\0'){
  //   arg[x]='\0';
  //   x++;
  // }
  
  return 0;
}
