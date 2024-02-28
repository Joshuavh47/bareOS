#include <bareio.h>
#include <barelib.h>

#define PROMPT "bareOS$ "  /*  Prompt printed by the shell to the user  */


/*
 * 'shell' loops forever, prompting the user for input, then calling a function based
 * on the text read in from the user.
 */
byte shell(char* arg) {
    while(1){
        printf(PROMPT);
        char buff[2048];
        int buff_ptr=0;
        
        int space=0;
        int cmd_ptr=-1;
        unsigned char last_ret=0;
        while(buff_ptr==0||buff[buff_ptr-1]!='\n'){
            
            if(buff_ptr<1024){
                char ch=uart_getc();
                buff[buff_ptr]=ch;
                printf("%d\n",buff_ptr);
                
            }
            
            if(buff_ptr>0&&buff[buff_ptr-1]=='$'&&buff[buff_ptr]=='?'){
                buff[buff_ptr]='\0';
                buff[buff_ptr-1]=last_ret;

                continue;
            }
            
            if(buff[buff_ptr]==' '&&space!=1){
                space=1;
                cmd_ptr=buff_ptr;
            }
            ++buff_ptr;
            printf("%d\n",buff_ptr);
        }
        printf("%d\n",buff_ptr);
        if(space!=1){
            cmd_ptr=buff_ptr;
        }
        int match=0;
        printf("%d\n",buff_ptr);
        for(int i=0;i<=cmd_ptr;i++){
            if(i==0){
                for(int j=0;j<2048;j++){
                    printf("%d ",buff[i]);
                }
            }
            if(i<cmd_ptr&&buff[i]=="hello"[i]){
                printf("1");
                printf("%d\n",cmd_ptr);
                continue;
            }
            else{
                break;
            }
            if(i==cmd_ptr&&(buff[i]==' '||cmd_ptr==buff_ptr)){
                match=1;
                //last_ret=builtin_hello();
            }
        }
        for(int i=0;i<=cmd_ptr;i++){
            if(i<cmd_ptr&&buff[i]=="echo"[i]){
                continue;
            }
            else{
                break;
            }
            if(i==cmd_ptr&&(buff[i]==' '||cmd_ptr==buff_ptr)){
                match=1;
                //last_ret=builtin_echo();
            }
        }
        if(match!=1){
            printf("Unknown command");
        }
        printf("\n");

    }
    return 0;
}


