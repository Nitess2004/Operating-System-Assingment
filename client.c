#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

int main(){

    int s;
    struct sockaddr_in server;
    char user[30],pass[30],msg[100],reply[100];

    s=socket(AF_INET,SOCK_STREAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(8080);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    connect(s,(struct sockaddr*)&server,sizeof(server));

    printf("Username: ");
    scanf("%s",user);

    printf("Password: ");
    scanf("%s",pass);

    sprintf(msg,"%s %s",user,pass);

    send(s,msg,strlen(msg),0);

    recv(s,reply,sizeof(reply),0);

    printf("Server: %s\n",reply);

    close(s);
}
