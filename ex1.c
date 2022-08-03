//207299785 Stav Lidor
#define SIZE 101
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int tokenTheCm (char strCm[SIZE],char* argv[SIZE],char* c,int i){
    int size = 0;
    char* token = strtok(strCm, c);   
    while (token) { 
        size++;
        argv [size +i -1] = token;
        token = strtok(NULL, c);
    }
    argv[size + i] = 0;
    return size; 
}
void isEcho (char strCm[SIZE],char* argv[SIZE]){
    int flag = 0,size = 0,isBegning = 0;
    char * theFrist = strchr(strCm, '\"');
    char* try[101]={0};
    char * other [101] = {0};
    char cpy [101],first[101];
    char * afterStr;
        char save [SIZE] ;
       
    if (argv[1] != 0 &&(!strcmp(argv[1],"-n") ||!strcmp(argv[1],"-e")||!strcmp(argv[1],"-E"))){
        flag = 1;
        strcpy(save,argv[1]);
        // 4=strlen(argv[0])  + 2=strlen(argv[1]) + 1=strlen(" ") = 7
        afterStr = strCm + 8;
    }
    else {
            //4=strlen(argv[0])
            afterStr = strCm +5;
    }
    int index = theFrist -afterStr;
   //befor
    while (index > 0 && afterStr[index]!=' '){
        index --;
    }
    if (index >= 0 && afterStr[index] == ' ')
    {
        isBegning++;
        memset(first, '\0', index + 1);
        strncpy(first, afterStr, index);
        if (afterStr[index + 1]!='\0'){
            afterStr +=index + 1;
        }
        
    }
    strcpy(cpy,afterStr);
    
    int x = strlen(cpy)  -1;
    int j = x;
    // after 
    while (j > 0 && cpy[j] != '\"') {
        j--;
    }
    j= j +1;
    while (j <x && cpy[j] != ' '){
        j++;
    }
    char substr[101];
    int flag2 = 0;
    if(j<x){
        flag2++;
        memset(substr, '\0', x-j+1);
        strncpy(substr, &cpy[j+1], x-j);
        cpy[j] = '\0';
    }
    int newSize =tokenTheCm(cpy, try,"\"",0);
    char * build = try[0];
    for(int i = 1;i<newSize; i++){
        char * n = strncat(build,try[i],strlen(try[i]));
            
        build = n;
    }

    
    argv[0] = "echo"; size++;
    if (flag){
       argv[size++]= save; 
    } 
    if (isBegning){
        size += tokenTheCm(first, argv," ",size);
        
    }
    argv[size++] = build;
    if(flag2){
        size += tokenTheCm(substr, argv," ",size); 
    }
    argv[size] = 0;
}
pid_t notBuiltIn(char strCm[SIZE]){
     pid_t pid;
     pid = -1;
    char* argv[101]={0};
    char* try[101]={0};
    char * other [101] = {0};
    char cpy [101];
    strcpy(cpy,strCm);
    int size = 0,index;
    int sizeStrCm = strlen(strCm);
    size = tokenTheCm(cpy, argv," ",0);
    int isWait =1 ;
    char* last = argv[size-1];
    if (!strcmp(last,"&")){
        strCm[sizeStrCm -1] ='\0';
        argv[size-1] = 0;
        isWait = 0;
    } 
    else{
        argv[size] = 0;
    }
    if(strchr(strCm, '\"')  && size>1 &&!strcmp(argv[0],"echo")){
       isEcho(strCm,argv);
    }
    int execS = 0;
    int cpid = fork();    
    if (cpid == 0) {
        execS  = execvp(argv[0], argv);
        if ( execS == -1){
            printf("exec failed\n");
            exit(0);
        }
        
    } else if (cpid < 0) {
        printf("fork failed\n");
    } else {
            if(isWait)  {
                if(waitpid(cpid,NULL,0)!= cpid){
                    printf("An error occurred\n");
                }
        }else {
            pid = cpid;
        }
    }
    if(execS == -1){
        pid = -1;
    }
    return pid;  
}
void input (char allCm[100][SIZE], int i){
    printf("$ ");
    fflush(stdout); 
    scanf("%100[^\n]%*c", allCm[i]);
    int size = strlen(allCm[i]);
    int j = size - 1;
    while (j > 0 && allCm[i][j] == ' ') {
        j--;
    }
    if (j+1 <= size -1 && allCm[i][j + 1] == ' '){
        allCm[i][j + 1] =  '\0';
    }   
}
void getCwdP(char saveCwd[], int size){
    if(getcwd(saveCwd,size)== NULL){
         printf("An error occurred\n");
    }
}
void allCd(char strCm[],int sizeCm,char pCwd [],char cwdB [],int size){
    char* argv[101]={0};
    char cpyS [sizeCm];
    strcpy(cpyS,strCm);
    int sizeArg  = tokenTheCm (cpyS,argv," ", 0);
    //printf("the str :%s\n",strCm);
    if (sizeArg > 2){
        printf("%s\n", "Too many arguments");
        return;
    }
    if (!strcmp(strCm,"cd ~") ||!strcmp(strCm,"cd") ||!strcmp(strCm,"cd ")){
        
        if( chdir(getenv("HOME")) != 0){
            printf("%s\n", "chdir failed");
            return;
        } 
        else{
            // strcpy(dst, src);
            printf("%s\n", getenv("HOME"));
            strcpy(cwdB,pCwd);
            getCwdP(pCwd,101);
        }
    }
    else if (!strcmp(strCm,"cd -")){
       
        if( chdir(cwdB) != 0){
            printf("%s\n", "chdir failed");
        } else{  
            char saveB [size];
            strcpy( saveB,pCwd); // strcpy(dst, src);
            strcpy(pCwd, cwdB);
            strcpy(cwdB, saveB);
        }          
    }

    else { 
        // after cd and space
        char * path = strCm + 3;
        if(strlen(path) > 1 &&*path == '~'&& *(path + 1) == '/'){
            path++;
            int s = strlen(path);
            if( chdir(strncat(getenv("HOME"),path,s)) != 0){
                printf("%s\n", "chdir failed");
                return;
            }  
        }
        
        else if (chdir(path) != 0){
            printf("%s\n", "chdir failed");
            return;
        } 
        strcpy( cwdB,pCwd);
        getCwdP(pCwd,101);
        

    }
}
void history(char allCm[100][101],pid_t pids[100],int i){
    int k,status = 0;
    for(k = 0; k< i  ;k++){
        if ( pids[k] > 0){
                if (waitpid(pids[k],&status,WNOHANG)){
                    pids[k] = -1;
                    printf( "%s %s\n", allCm[k], "DONE");
                }
                else {
                    printf( "%s %s\n", allCm[k], "RUNNING");
                }
            }
            else{
                printf( "%s %s\n", allCm[k], "DONE");
            }
                    
        } 
        printf( "%s %s\n", allCm[i], "RUNNING");  
}
void jobs (char allCm[100][101],pid_t pids[100],int i){
    int k,status = 0;
    for(int k = 0; k< i ;k++){
        if ( pids[k] > 0){
            if (waitpid(pids[k],&status,WNOHANG)){ 
                pids[k] = -1;
            }
            else {
                printf("%s\n",allCm[k] );
                }
            }             
    }
}
int main (){
    
    char allCm[100][101];
    pid_t pids[100]={-1};
    char cwdB [SIZE],pCwd[SIZE];
    
    // save the path before and after the change.
    getCwdP(pCwd,SIZE);
    strcpy(cwdB, pCwd);
    int i = 0;
    input(allCm,i);
    while(strcmp(allCm[i],"exit")){
        printf(" chack home %s\n", getenv("HOME"));
        int size = strlen(allCm[i]);
        // if build in
        if (!strcmp(allCm[i],"history")){  
            history(allCm,pids,i);
        }
         else if (!strcmp(allCm[i],"jobs")){
              jobs(allCm,pids,i);
         }
        else if ((size == 2 && allCm[i][0] == 'c' && allCm[i][1]== 'd')
                ||(size >= 3 && allCm[i][0] == 'c' && allCm[i][1]== 'd'&&allCm[i][2]== ' ')){
           allCd(allCm[i],size,pCwd,cwdB,SIZE);
           printf("presnt %s\n",pCwd); printf("befor %s\n",cwdB);
        }
        // not build in
        else {
            char cpy [101];
            strcpy(cpy, allCm[i]);
            // if the procces Succeeded and still runing is change from -1
            pids [i] = notBuiltIn(cpy);
            if (size >= 2 && allCm[i][size -2] ==' ' && allCm[i][size -1] =='&'){
                allCm[i][size -1] = 0;
                allCm[i][size -2] = 0;
            }  
        }
        // new commend
        i++;
        input(allCm,i);
    }
    return 0;
}