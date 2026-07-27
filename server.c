#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>

int check(char u[],char p[]){
    FILE *f=fopen("users.db","r");
    char user[30],pass[30];
    if(!f) return 0;
    while(fscanf(f,"%s %s",user,pass)!=EOF){
        if(strcmp(u,user)==0 && strcmp(p,pass)==0){
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main(){
    int s,c;
    struct sockaddr_in server,client;
    char buf[100],u[30],p[30];

    s=socket(AF_INET,SOCK_STREAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(8080);
    server.sin_addr.s_addr=INADDR_ANY;

    bind(s,(struct sockaddr*)&server,sizeof(server));
    listen(s,5);

    printf("Server Waiting...\n");

    while(1){
        socklen_t len=sizeof(client);
        c=accept(s,(struct sockaddr*)&client,&len);

        recv(c,buf,sizeof(buf),0);

        if(sscanf(buf,"%s %s",u,p)!=2){
            send(c,"INVALID",7,0);
        }
        else if(check(u,p)){
            send(c,"LOGIN SUCCESS",13,0);
        }
        else{
            send(c,"LOGIN FAILED",12,0);
        }

        close(c);
    }
}
