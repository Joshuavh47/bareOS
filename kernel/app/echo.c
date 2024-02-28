#include <bareio.h>
#include <barelib.h>


/*
 * 'builtin_echo' reads in a line of text from the UART
 * and prints it.  It will continue to do this until the
 * line read from the UART is empty (indicated with a \n
 * followed immediately by another \n).
 */
byte builtin_echo(char* arg) {
  char last='\0';
  char curr='\0';
  char buff[2048];
  //int x=0;
  // while(arg[x]!='\0'){
  //   buff[x]='\0';
  //   x++;
  // }
  if(arg[5]!='\0'||arg[6]!='\0'){
    for(int i=5;i<2048;i++){
      if(arg[i]=='\0'){
        break;
      }
      uart_putc(arg[i]);
    }
    printf("\n");
    int x=0;
    while(arg[x]!='\0'){
      arg[x]='\0';
    }
    return 0;
  }
  int count=0;
  int total=0;
  while(last!='\n'||curr!='\n'){
    last=curr;
    curr=uart_getc();
    
    buff[count]=curr;
    if(curr!='\n'){
      count++;
    }
    else{
      total+=count;
      if(last!='\n'&&total>0){
        printf(buff);
      }
      for(int i=0;i<2048;i++){
        buff[i]='\0';
      }
      if(curr=='\n'&&total==0){
        return total;
      }
      count=0;
    }
    
  }
  
  //x=0;
  // while(arg[x]!='\0'){
  //   arg[x]='\0';
  //   x++;
  // }
  return total;
}
