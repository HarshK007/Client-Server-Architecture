#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>

#include<stdlib.h>

#define SERV_TCP_PORT 5035
#define MAXLINE 4096

void trimNewline(char str[]){
	int len = strlen(str);
	if(str[len-1] == '\n'){
		str[len-1] = 0;
	}
}

void registration(char input[],char output[]){
	strcat(input,"\n");
	FILE* file = fopen("userDatabase.txt","a");
	fwrite(input,strlen(input),1,file);
	strcpy(output,"SUCCESSFUL");
	fclose(file);
}

void retrievePassword(char input[],char output[]){
	strcat(input," ");
    char* line = NULL;
    size_t len = 0;
    char outputMessage[MAXLINE];
    bool found = false;
    FILE* file = fopen("userDatabase.txt","r");
    
    while (!found && (getline(&line, &len, file) != -1)){
    	found = false;
		for(int i=0;i<strlen(input);i++){
			found = true;
			if(line[i]!=input[i]){
				found = false;
				break;
			}
		}
    }
    
    if(found){
    	trimNewline(line);
    	line = strchr(line,' ');
    	line++;
    	strcpy(outputMessage,line);
	}else{
		strcpy(outputMessage,"no username found");
	}
	strcpy(output,outputMessage); 

    fclose(file);
}

void retrieveSound(char input[],char output[]){
	strcat(input," ");
    char* line = NULL;
    size_t len = 0;
    char outputMessage[MAXLINE];
    bool found = false;
    FILE* file = fopen("soundDatabase.txt","r");

    while (!found && (getline(&line, &len, file) != -1)){
    	found = false;
		for(int i=0;i<strlen(input);i++){
			found = true;
			if(line[i]!=input[i]){
				found = false;
				break;
			}
		}
    }
    
    if(found){
    	strcpy(outputMessage,input);
    	strcat(outputMessage,"SAYS");
    	line = strchr(line,' ');
    	strcat(outputMessage,line);
	}else{
		strcpy(outputMessage,"I DON'T KNOW ");
		strcat(outputMessage,input);
	}
	strcpy(output,outputMessage); 

    fclose(file);
}

void retrieveAllAnimals(char output[]){
    char* line = NULL;
    size_t len = 0;
    int animalNameLength;
    FILE* file = fopen("soundDatabase.txt","r");
    while (getline(&line, &len, file) != -1){
    	animalNameLength = 0;
		for(int i=0;i<strlen(line);i++){
			if(line[i] == ' ')
				break;
			animalNameLength++;
		}
		strncat(output,line,animalNameLength);
		strcat(output,"\n");
    }
	strcat(output,"QUERY : OK");
    fclose(file);
}

void storeAnimalSound(char animalName[],char animalSound[],char output[]){
	char* line = NULL;
    size_t len = 0;
    char newAnimal[50];
    int numOfAnimals = 0;
    int maxNumberOfAnimals = 15;
    bool found = false;
    
    FILE* file = fopen("soundDatabase.txt","r");
	while (!found && (getline(&line, &len, file) != -1) && numOfAnimals<maxNumberOfAnimals){
		numOfAnimals++;
    	found = false;
		for(int i=0;i<strlen(animalName);i++){
			found = true;
			if(line[i]!=animalName[i]){
				found = false;
				break;
			}
		}
    }
    fclose(file);
    // Checking Space Available
    if(numOfAnimals == maxNumberOfAnimals){
    	printf("\nMax Limit Reached !!!\n");
	}else{	
		// Animal already exist !!! Update Sound !!!
		if(found){
			printf("\nAnimal already present!!!\n");
			FILE* file1 = fopen("soundDatabase.txt","r");
			FILE* file2 = fopen("temp.txt","w");
			while(numOfAnimals > 1){
				getline(&line, &len, file1);
				fwrite(line,strlen(line),1,file2);
				numOfAnimals--;
			}
			getline(&line, &len, file1);
			strcpy(newAnimal,animalName);
			strcat(newAnimal," ");
			strcat(newAnimal,animalSound);
			strcat(newAnimal,"\n");
			fwrite(newAnimal,strlen(newAnimal),1,file2);
			while(getline(&line, &len, file1) != -1){
				fwrite(line,strlen(line),1,file2);
				numOfAnimals--;
			}
			remove("soundDatabase.txt");
			rename("temp.txt","soundDatabase.txt");
			fclose(file1);
			fclose(file2);
		}else{
			// New Animal added to database
			FILE* file = fopen("soundDatabase.txt","a");
			strcpy(newAnimal,"\n");
			strcat(newAnimal,animalName);
			strcat(newAnimal," ");
			strcat(newAnimal,animalSound);
			fwrite(newAnimal,strlen(newAnimal),1,file);
			fclose(file);	
		}	
	}
	strcpy(output,"STORE : OK");
}

void resetDatabase(){
	char* line = NULL;
    size_t len = 0;
    // Restting user database
    remove("userDatabase.txt");
    FILE* file = fopen("userDatabase.txt","w");
    fclose(file);
	
	//Resetting sound database
	FILE* file1 = fopen("defaultSoundDatabase.txt","r");
	FILE* file2 = fopen("soundDatabase.txt","w");
	while(getline(&line, &len, file1) != -1){
		fwrite(line,strlen(line),1,file2);
	}
	fclose(file1);
	fclose(file2);
}

void soundServerOperation(int connfd){
	char enteredAnimalName[MAXLINE],enteredAnimalSound[MAXLINE],enteredOperation[MAXLINE],queryResponse[MAXLINE];
	while(1){
		strcpy(queryResponse,"");
		printf("\nWaiting for Operation Selection!!!\n");
		read(connfd,enteredOperation,MAXLINE);
		printf("\nOperation entered by client is : %s\n",enteredOperation);
		if(!strcmp(enteredOperation,"SOUND")){
			printf("\nSOUND OPTION Selected!!!");
			read(connfd,enteredAnimalName,MAXLINE);
			retrieveSound(enteredAnimalName,queryResponse);
			send(connfd,queryResponse,MAXLINE,0);
		}else if(!strcmp(enteredOperation,"STORE")){
			printf("\nSTORE OPTION Selected!!!");
			read(connfd,enteredAnimalName,MAXLINE);
			read(connfd,enteredAnimalSound,MAXLINE);
			storeAnimalSound(enteredAnimalName,enteredAnimalSound,queryResponse);
			send(connfd,queryResponse,MAXLINE,0);
		}else if(!strcmp(enteredOperation,"QUERY")){
			printf("\nQUERY OPTION Selected!!!");
			retrieveAllAnimals(queryResponse);
			send(connfd,queryResponse,MAXLINE,0);
		}else if(!strcmp(enteredOperation,"BYE")){
			printf("\nBYE OPTION Selected!!!");
			strcpy(queryResponse,"BYE : OK");
			send(connfd,queryResponse,MAXLINE,0);
			printf("\nChild Ended using BYE (Changes persist) !!!\n");
			close(connfd);
			exit(0);
		}else if(!strcmp(enteredOperation,"END")){
			printf("\nEND OPTION Selected!!!");
			strcpy(queryResponse,"END : OK");
			send(connfd,queryResponse,MAXLINE,0);
			resetDatabase();
			printf("\nChild Ended using END (Database Reset)!!!\n");
			close(connfd);
			exit(0);
		}
	}
}

void soundServerRegister(int connfd){
	char userNameResponse[] = {"username invalid"};
	char enteredUsername[MAXLINE],enteredPassword[MAXLINE],newUserDetails[MAXLINE],passwordForUsername[MAXLINE];
	char queryResponse[MAXLINE];
	
	strcpy(userNameResponse,"username invalid");
	while(strcmp(userNameResponse,"username valid")){
		printf("\nWaiting for Username !!!\n");
		read(connfd,enteredUsername,MAXLINE);
		printf("\nUsername entered by client is : %s\n",enteredUsername);
		retrievePassword(enteredUsername,passwordForUsername);
		if(!strcmp(passwordForUsername,"no username found")){
			strcpy(queryResponse,"username valid");
		}else{
			strcpy(queryResponse,"username invalid");
		}
		printf("\nUsername Response : %s\n",queryResponse);
		send(connfd,queryResponse,MAXLINE,0);
		strcpy(userNameResponse,queryResponse);
	}
	printf("\nWaiting for Password !!!\n");
	read(connfd,enteredPassword,MAXLINE);
	strcpy(newUserDetails,enteredUsername);
	strcat(newUserDetails,enteredPassword);
	registration(newUserDetails,queryResponse);
	printf("\nRegistration %s !!!",queryResponse);
	send(connfd,queryResponse,MAXLINE,0);
}

bool soundServerLogin(int connfd){
	char userNameResponse[] = {"username invalid"};
	char passwordResponse[] = {"password invalid"};
	char enteredUsername[MAXLINE],enteredPassword[MAXLINE],passwordForUsername[25];
	char queryResponse[MAXLINE];
	// Verifying Username
	printf("\nWaiting for Username !!!\n");
	read(connfd,enteredUsername,MAXLINE);
	printf("\nUsername entered by client is : %s\n",enteredUsername);
	retrievePassword(enteredUsername,passwordForUsername);	
	if(!strcmp(passwordForUsername,"no username found")){
		strcpy(queryResponse,"username invalid");
	}else{
		strcpy(queryResponse,"username valid");
	}
	printf("\nUsername Response : %s\n",queryResponse);
	send(connfd,queryResponse,MAXLINE,0);
	strcpy(userNameResponse,queryResponse);
	if(!strcmp(userNameResponse,"username invalid")){
		return false;
	}
	// Verifying Password
	while(strcmp(passwordResponse,"password valid")){
		printf("\nWaiting for Password !!!\n");
		read(connfd,enteredPassword,MAXLINE);
		printf("\nPassword entered by client is : %s\n",enteredPassword);
		if(!strcmp(passwordForUsername , enteredPassword)){
			strcpy(passwordResponse,"password valid");
		}else{
			strcpy(passwordResponse,"password invalid");
		}
		printf("\nPassword Response : %s\n",passwordResponse);
		send(connfd,passwordResponse,MAXLINE,0);
	}
	return true;
}

void soundServerLoginRegisterMenu(int connfd){
	char enteredChoice[MAXLINE];
	bool loginFlag = false;
	while(1){
		printf("\nWaiting for register-login choice !!!\n");
		read(connfd,enteredChoice,MAXLINE);
		printf("\nOperation entered by client is : %s\n",enteredChoice);
		if(!strcmp(enteredChoice,"REGISTER")){
			printf("\nREGISTER OPTION Selected!!!");
			soundServerRegister(connfd);
		}else if(!strcmp(enteredChoice,"LOGIN")){
			printf("\nLOGIN OPTION Selected!!!");
			loginFlag = soundServerLogin(connfd);		
			if(loginFlag){
				break;
			}
		}else if(!strcmp(enteredChoice,"EXIT")){
			printf("\nEXIT OPTION Selected!!!");
			printf("\nChild Ended using EXIT in login menu !!!\n");
			close(connfd);
			exit(0);
		}
	}
}

void server(){
	int sockfd, connfd;
	socklen_t clength;
	struct sockaddr_in serv_addr,cli_addr;
	
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	printf("Socket() called !!!\n");
	
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(SERV_TCP_PORT);
	
	bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	printf("Bind() called!!!\n");
	
	listen(sockfd,5);
	printf("Listen() called!!!\n");
	
	pid_t childPid;
	
	for( ; ; ){
		clength=sizeof(cli_addr);
		connfd=accept(sockfd,(struct sockaddr*)&cli_addr,&(clength));
		printf("Accept() called!!!\n");

		if((childPid = fork()) == 0){
			// close sockfd for child
			close(sockfd);
			//Performing login Operation
			soundServerLoginRegisterMenu(connfd);
			//Performing Sound Operation
			soundServerOperation(connfd);
		}
		close(connfd);
	}
		
	printf("\nServer ended!!!\n");
    close(sockfd);
}

int main(){
	server();
    return 0;
}	
