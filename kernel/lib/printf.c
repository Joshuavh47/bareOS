#include <bareio.h>
#include <barelib.h>


/*
 *  In this file, we will write a 'printf' function used for the rest of
 *  the semester.  In is recommended  you  try to make your  function as
 *  flexible as possible to allow you to implement  new features  in the
 *  future.
 *  Use "va_arg(ap, int)" to get the next argument in a variable length
 *  argument list.
 */

void printf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  
  int i=0;
  while(format[i]!='\0'){
    if(format[i]=='%'){
      ++i;
      if(format[i]=='d'){
        unsigned int next = va_arg(ap,int);
        /*
        if(next<0){
          uart_putc('-');
          next*=-1;
        }
        */
        int div=10;
        
        if(next>=1000000000){
          div=1000000000;
        }
        else{
          while(div<next){
            div*=10;
          }
          div/=10;
        }
        
        
        while(div>0){
          uart_putc(((next/div)%10)+48);
          div/=10;
        }

        ++i;
        continue;
      }
      else if(format[i]=='x'){
        unsigned int next = va_arg(ap,int);
        uart_putc('0');
        uart_putc('x');
        
        int div=16;
        
        if(next>=268435456){
          div=268435456;
        }
        else{
          while(div<next){
            div*=16;
          }
          div/=16;
        }
        
        
        while(div>0){
          if(((next/div)%16)<=9){
            uart_putc(((next/div)%16)+48);
          }
          else{
            uart_putc(((next/div)%16)+87);
          }
          div/=16;
        }


        ++i;
        continue;
      }
      
    }
    uart_putc(format[i]);
    i++;
  }
  
  
  
  va_end(ap);
  
}
