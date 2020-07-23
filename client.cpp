#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define SERV_TCP_PORT 5035
#define MAXLINE 4096
#define LOCALHOST "127.0.0.1"

void toUpperCase(char str[]){
	for(int i=0; i<strlen(str); i++){
		str[i] = toupper(str[i]);
	}
}

void trimNewline(char str[]){
	int len = strlen(str);
	if(str[len-1] == '\n'){
		str[len-1] = 0;
	}
}

void beautifyStringInput(char input[]){
	fgets(input,MAXLINE,stdin);
	trimNewline(input);
	toUpperCase(input);
}

void clientRegister(int sockfd){
	char registerLine[MAXLINE],registerResponse[MAXLINE];
	char userNameResponse[] = {"username invalid"};
	char username[MAXLINE],password[MAXLINE];
	
	strcpy(userNameResponse,"username invalid");
	while(strcmp(userNameResponse,"username valid")){
		printf("\n>> Enter the Username :: ");
		beautifyStringInput(username);
		write(sockfd,username,MAXLINE);
		read(sockfd,userNameResponse,MAXLINE);           
		if(!strcmp(userNameResponse,"username invalid")){
			printf("<< A user with this username already exists. Please try a different name !!! >>");
		}else{
			printf("<< Username accepted !!! >>");
		}
	}
	printf("\n>> Enter the Password :: ");
	beautifyStringInput(password);
	write(sockfd,password,MAXLINE);
	read(sockfd,registerResponse,MAXLINE);
	printf("Server Response : Registration Status : %s\n",registerResponse);
}

bool clientLogin(int sockfd){
	char userNameResponse[] = {"username invalid"};
	char passwordResponse[] = {"password invalid"};
	char username[MAXLINE],password[MAXLINE];
	char welcomeMessage[MAXLINE];
	
	printf("\nEnter the Username :: ");
	beautifyStringInput(username);
	write(sockfd,username,MAXLINE);
	strcpy(welcomeMessage,username);
	read(sockfd,userNameResponse,MAXLINE);    
	printf("Server Response : %s\n",userNameResponse);
	if(!strcmp(userNameResponse,"username invalid")){
		return false;
	}
	
	while(strcmp(passwordResponse,"password valid")){
		printf("\nEnter the Password :: ");
		beautifyStringInput(password);
		write(sockfd,password,MAXLINE);
		read(sockfd,passwordResponse,MAXLINE);           
		printf("Server Response : %s\n",passwordResponse);
	}
	printf("\n\t\t\t*** WELCOME %s ***",welcomeMessage);
	return true;	
}

void clientLoginRegisterMenu(int sockfd){
	char choice[MAXLINE];
	bool loginFlag = false;
	while(1){
		printf("\n>> Enter the operation you want to perform");
		printf("\n( Possible Operations :: {LOGIN , REGISTER , EXIT} )\n");
		printf("\n>> Enter Operation : \t");
		beautifyStringInput(choice);
		write(sockfd,choice,MAXLINE);
		if(!strcmp(choice,"REGISTER")){
			clientRegister(sockfd);
		}else if(!strcmp(choice,"LOGIN")){
			loginFlag = clientLogin(sockfd);
			if(loginFlag){
				break;
			}
		}else if(!strcmp(choice,"EXIT")){
			printf("\nEXIT OPTION Selected !!!");
			printf("\nClosing Client Program !!!\n");
			exit(0);
		}else{
			printf("Invalid Operation !!!");
		}
	}
}

void soundClientOperation(int sockfd){
	char operation[MAXLINE],animalName[MAXLINE],animalSound[MAXLINE],animalSoundResponse[MAXLINE],queryResponse[MAXLINE];
		
	while(1){
		printf("\n>> Type the operation you want to perform");
		printf("\n( Possible Operations :: {SOUND, STORE, QUERY, BYE, END} )\n");
		printf("\n>> Enter Operation : \t");
		beautifyStringInput(operation);
		write(sockfd,operation,MAXLINE);
		if(!strcmp(operation,"SOUND")){
			printf("\nEnter Name of the Animal :: ");
			beautifyStringInput(animalName);
			write(sockfd,animalName,MAXLINE);
			read(sockfd,animalSoundResponse,MAXLINE);
			printf("Server Response : Animal Sound : %s\n",animalSoundResponse);
		}else if(!strcmp(operation,"STORE")){
			printf("\nEnter Name of the Animal :: ");
			beautifyStringInput(animalName);
			write(sockfd,animalName,MAXLINE);
			printf("\nEnter Sound of the Animal :: ");
			beautifyStringInput(animalSound);
			write(sockfd,animalSound,MAXLINE);
			read(sockfd,animalSoundResponse,MAXLINE);
			printf("Server Response : %s\n",animalSoundResponse);
		}else if(!strcmp(operation,"QUERY")){
			read(sockfd,queryResponse,MAXLINE);
			printf("\n>> List of all animals\n");
			printf("\nServer Response : \n%s\n",queryResponse);
		}else if(!strcmp(operation,"BYE")){
			read(sockfd,queryResponse,MAXLINE);
			printf("\nServer Response : %s\n",queryResponse);
			break;
		}else if(!strcmp(operation,"END")){
			read(sockfd,queryResponse,MAXLINE);
			printf("\nServer Response : %s\n",queryResponse);
			break;
		}else{
			printf("Invalid Operation !!!");
		}
		strcpy(animalSoundResponse,"");
	}
}

void client(){
	int sockfd;
	struct sockaddr_in serv_addr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	printf("Socket() called successfully !!!\n");
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(LOCALHOST);
	serv_addr.sin_port=htons(SERV_TCP_PORT);
	
	connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	printf("Connect() called successfully!!!\n");
	
	//LOGIN-REGISTER
	clientLoginRegisterMenu(sockfd);

	//Sound Operations
	soundClientOperation(sockfd);
	
	close(sockfd);
	printf("\nClient ended!!!\n\n");
}

int main(){
	client();
	return 0;
}
