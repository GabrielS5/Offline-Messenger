#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<time.h>

#define PORT 2728

extern int errno;
char * conv_addr (struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  strcpy (str, inet_ntoa (address.sin_addr));
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));
  strcat (str, port);
  return (str);
}

int verificareLogin(char nume[20])
	{
	int file;

	if((file=open("usernames.txt",O_RDONLY|O_CREAT,00777))==-1)
                perror("Eroare la deschidere fisier\n");

	char caracter,numeDinFisier[20];
        int i=0;

        while(read(file,&caracter,1))
                {
                if(caracter!='\n')
                        {
                        numeDinFisier[i]=caracter;
                        i++;
                        } 
                else
                        {
                        numeDinFisier[i]='\0';
                        if(strcmp(nume,numeDinFisier)==0)
                                {
                                return 1;
                                }
                        i=0;
                        }
                }
	return 0;
	}

void adaugaUser(char nume[20])
	{
	int file;
	if((file=open("usernames.txt",O_WRONLY|O_APPEND|O_CREAT,00777))==-1)
		perror("Eroare la deschidere fisier\n");

	strcat(nume,"\n");

	if(write(file,nume,strlen(nume))<0)
		perror("Eroare la write\n");
	if(close(file)==-1)
		perror("Eroare la close\n");
	}

void desparteInComenzi(char mesaj[1000],char comenzi[10][20])
	{
	int N=0,j=0;

	for(int i=0;i<strlen(mesaj);i++)
		{
		if(N==10)
			break;
		if(mesaj[i]==' ')
			{
			comenzi[N][j]='\0';
			j=0;
			N++;
			}
		else
			{
			comenzi[N][j]=mesaj[i];
			j++;
			}
		}
	}

void insert(int clienti[100],int nrClienti,int descriptor)
	{
	clienti[nrClienti]=descriptor;
	}

void erase(int clienti[100],int nrClienti,int pos)
	{
	for(int i=pos;i<nrClienti-1;i++)
		clienti[i]=clienti[i+1];
	}

void afisareClienti(int clienti[100],int logati[100],char numeClienti[20][20],int nrClienti)
	{
	for(int i=0;i<nrClienti;i++)
		printf("%d  %d  %s ",clienti[i],logati[i],numeClienti[i]);
	printf("\n");
	}

int findClient(int clienti[100],int nrClienti,int descriptor)
	{
	for(int i=0;i<nrClienti;i++)
		if(clienti[i]==descriptor)
		return i;
	return -1;
	}

int findClientByName(char numeClienti[20][20], int nrClienti,char nume[20])
	{
	for(int i=0;i<nrClienti;i++)
		{
		if(strcmp(nume,numeClienti[i])==0)
			{
			return i;
			}
		}
	return -1;
	}

void insertNume(char numeClienti[20][20],int nrClienti,char nume[20])
	{
	strcpy(numeClienti[nrClienti],nume);
	}

void eraseNume(char numeClienti[20][20],int nrClienti,int pos)
	{
	for(int i=pos;i<nrClienti-1;i++)
		strcpy(numeClienti[i],numeClienti[i+1]);
	}

void copyFromToEnd(char *to,char *from,int n)
	{
	int j=0;

	for(int i=n;i<strlen(from);i++,j++)
		to[j]=from[i];
	to[j]='\0';
	}

void checkMail(int descriptor,char nume[20])
	{
	char fisier[30],buffer[2000];

	bzero(fisier,30);
	strcat(fisier,nume);
	strcat(fisier,"Mail.txt");
	int file=open(fisier,O_RDONLY);
	bzero(buffer,2000);
	fflush(stdout);
	if(file!=-1)
		{
		if(read(file,buffer,2000)<0)
			perror("Eroare la read\n");
		if(write(descriptor,buffer,strlen(buffer))<0)
			perror("Eroare la write\n");
		}
	if(close(file)==-1)
		perror("Eroare la inchidere fisier");
	if((file=open(fisier,O_WRONLY|O_TRUNC|O_CREAT,00777))==-1)
		perror("Eroare la deschidere fisier\n");
	bzero(buffer,2000);
	strcat(buffer,"Mail-ul tau:\n");
	if(write(file,buffer,strlen(buffer))<0)
		perror("Eroare la write\n");
	if(close(file)==-1)
		 perror("Eroare la inchidere fisier\n");
	}

void sendMail(char nume[20],char mesaj[2000])
	{
	char fisier[30],aux[20];

	bzero(fisier,30);
	strcat(fisier,nume);
	strcat(fisier,"Mail.txt");
	int file=open(fisier,O_WRONLY|O_APPEND);
	if(file==-1)
		{
		if((file=open(fisier,O_WRONLY|O_CREAT,00777))==-1)
			perror("Eroare la open\n");
		bzero(aux,20);
		strcat(aux,"Mail-ul tau:\n");
		if(write(file,aux,strlen(aux))<0)
			 perror("Eroare la write\n");
		}
	if(write(file,mesaj,strlen(mesaj))<0)
		perror("Eroare la write\n");
	if(write(file,"\n",1)<0)
		perror("Eroare la write\n");
	if(close(file)==-1)
		perror("Eroare la inchidere fisier\n");
	}

void checkHistory(char nume1[20],char nume2[20],int descriptor)
	{
	char fisier[50],buffer[2000];

	bzero(fisier,50);
	bzero(buffer,2000);
	strcat(fisier,nume1);
	strcat(fisier,nume2);
	strcat(fisier,"History.txt");
	int file=open(fisier,O_RDONLY);
	if(file!=-1)
		{
		if(read(file,buffer,2000)>0)
			perror("Eroare la read\n");
		if(write(descriptor,buffer,strlen(buffer))<0)
			perror("Eroare la write\n");
		if(close(file)==-1)
			perror("Eroare la close\n");
		}
	else
		if(write(descriptor,"Istoric gol",11)<0)
			perror("Eroare la write\n");
	}

void addHistory(char nume1[20],char nume2[20],char mesaj[2000])
	{
	char fisier[50];

	bzero(fisier,50);
	strcat(fisier,nume1);
	strcat(fisier,nume2);
	strcat(fisier,"History.txt");
	int file;
	if((file=open(fisier,O_WRONLY|O_CREAT|O_APPEND,00777))==-1)
		perror("Eroare la open\n");
	if(write(file,mesaj,strlen(mesaj))<0)
		perror("Eroare la write\n");
	if(write(file,"\n",1)<0)
		perror("Eroare la write\n");
	if(close(file)==-1)
		perror("Eroare la close\n");
	}

void addHistoryLobby(char nume[20],char mesaj[1000])
	{
	char fisier[50];

	bzero(fisier,50);
	strcat(fisier,nume);
	strcat(fisier,"History.txt");
	int file;
        if((file=open(fisier,O_WRONLY|O_CREAT|O_APPEND,00777))==-1)
                perror("Eroare la open\n");
        if(write(file,mesaj,strlen(mesaj))<0)
                perror("Eroare la write\n");
        if(write(file,"\n",1)<0)
                perror("Eroare la write\n");
        if(close(file)==-1)
                perror("Eroare la close\n");
	}

void checkHistoryLobby(char nume[20],int descriptor)
	{
        char fisier[50],buffer[2000];

        bzero(fisier,50);
	bzero(buffer,2000);
        strcat(fisier,nume);
        strcat(fisier,"History.txt");
	int file=open(fisier,O_RDONLY);
        if(file!=-1)
                {
                if(read(file,buffer,2000)>0)
                        perror("Eroare la read\n");
                if(write(descriptor,buffer,strlen(buffer))<0)
                        perror("Eroare la write\n");
                if(close(file)==-1)
                        perror("Eroare la close\n");
                }
        else
                if(write(descriptor,"Istoric gol",11)<0)
                        perror("Eroare la write\n");
        }

void getOnlineUsers(char mesaj[1000],char numeClienti[20][20],int nrClienti)
	{
	for(int i=0;i<nrClienti;i++)
		{
		strcat(mesaj,numeClienti[i]);
		strcat(mesaj," -Online\n");
		}
	}

void getOfflineUsers(char mesaj[1000],char numeClienti[20][20],int  nrClienti)
	{
	int file,i=0;
	char caracter,numeDinFisier[20];

        if((file=open("usernames.txt",O_RDONLY))==-1)
                perror("Eroare la deschidere fisier\n");
        while(read(file,&caracter,1))
                {
                if(caracter!='\n')
                        {
                        numeDinFisier[i]=caracter;
                        i++;
                        }
                else
                        {
                        numeDinFisier[i]='\0';
                        if(findClientByName(numeClienti,nrClienti,numeDinFisier)==-1)
                                {
                                strcat(mesaj,numeDinFisier);
                                strcat(mesaj," -Offline\n");
                                }
                        i=0;
                        }
                }
	}

void getUsers(char mesaj[1000],char numeClienti[20][20],int  nrClienti)
	{
	getOnlineUsers(mesaj,numeClienti,nrClienti);
	getOfflineUsers(mesaj,numeClienti,nrClienti);
	}

void kickUsers(int lobby,int userLobby[100],int nrClienti,int clienti[100])
	{
	char mesaj[100];
	strcat(mesaj,"Lobby-ul curent a fost inchis!");
	for(int i=0;i<nrClienti;i++)
		{
		if(userLobby[i]=lobby)
			{
			userLobby[i]=0;
			if(write(clienti[i],mesaj,strlen(mesaj))<0)
				perror("Eroare la write\n");
			}
		}
	}

void sendMessageLobby(char mesajRasp[1000],int clienti[100],int userLobby[100],int index,int nrClienti)
	{
		for(int i=0;i<nrClienti;i++)
		{
		if(i!=index && userLobby[i]==userLobby[index])
			{
			if(write(clienti[i],mesajRasp,strlen(mesajRasp))<0)
				perror("Eroare la write\n");
			}
		}
	}

void help(char mesajRasp[1000])
	{
                strcat(mesajRasp,"<SERVER> Help:\n");
        	strcat(mesajRasp,"/help                         -ajutor\n/quit                         -iesi\n/login <nume>                 -logare\n/register <nume>              -inregistrare\n/send <nume> <mesaj>          -trimitere mesaj\n");
                strcat(mesajRasp,"/reply <mesaj>                -raspuns catre ultimul user\n/mail                         -acces mail\n");
                strcat(mesajRasp,"/history <nume>               -vezi istoricul cu un user\n/create lobby public <nume>   -creare lobby public\n/create lobby private <nume>  -creare lobby privat\n");  
        	strcat(mesajRasp,"/lobbies                      -vezi lobby-urile\n/join <nume>                  -intra in lobby-ul <nume>\n/invite <nume>                -invita user in lobby-ul curent\n");
		strcat(mesajRasp,"/history lobby <nume>         -vezi istoricul lobby-ului\n");
		strcat(mesajRasp,"/exit lobby                   -iesi din lobby\n/close current                -inchide lobby-ul curent\n");
 		strcat(mesajRasp,"/online users                 -vezi useri online\n");
		strcat(mesajRasp,"/offline users                -vezi useri offline\n/users                        -vezi useri\n/delay-<secunde> <comanda>    -trimite o comanda cu delay\n");
	}

int lng(int numar)
	{
	int rezultat=0;

	if(numar%10==0)
		numar++;
	do{
		rezultat++;
		numar/=10;
	}while(numar>0);
	return rezultat;
}


void intToChar(char caracter[30],int numar)
	{
	int lungime=lng(numar);

	bzero(caracter,30);
	for(int i=lungime-1;i>=0;i--)
		{
		caracter[i]=numar%10+48;
		numar/=10;
		}
	caracter[lungime]='\0';
	}

int nrMesaje(char fisier[30])
	{
	int file,numar=0;
	char caracter[1];
	if((file=open(fisier,O_RDONLY))==-1)
		{
		perror("Eroare la open de aici\n");
		return 0;
		}

	while(read(file,caracter,1)>0)
		{
		if(caracter[0]=='\n')
			numar++;
		}
	if(close(file)==-1)
		perror("Eroare la close");
	return numar-1;
	}

void getLobbies(char mesajRasp[1000],int nrLobbies,int lobbies[100],char numeLobbies[20][20])
	{
	for(int i=0;i<nrLobbies;i++)
		{
		if(lobbies[i]>0)
			{
			strcat(mesajRasp,numeLobbies[i]);
			strcat(mesajRasp,"\n");
			}
		}
	}


int main ()
{
srand(time(NULL)); 
  struct sockaddr_in server;
  struct sockaddr_in from;
  fd_set readfds;
  fd_set actfds;
  struct timeval tv;
  int sd, client,clienti[100],nrClienti=0,logati[100],userLobby[100],lobbies[100];
  int aux=0;
  int* nrLobbies=&aux;
  char numeClienti[20][20],numeLobbies[20][20];
  int optval=1;
  int fd;
  int nfds;
  int len;

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server] Eroare la socket().\n");
      return errno;
    }

  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

  bzero (&server, sizeof (server));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);

  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server] Eroare la bind().\n");
      return errno;
    }

    if (listen (sd, 5) == -1)
    {
      perror ("[server] Eroare la listen().\n");
      return errno;
    }

  FD_ZERO (&actfds);
  FD_SET (sd, &actfds);
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  nfds = sd;

  printf ("[server] Asteptam la portul %d...\n", PORT);
  fflush (stdout);
  while (1)
    {
       bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

         if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
	{
	perror ("[server] Eroare la select().\n");
	return errno;
	}
        if (FD_ISSET (sd, &readfds))
	{
	len = sizeof (from);
	bzero (&from, sizeof (from));

	client = accept (sd, (struct sockaddr *) &from, &len);
	insert(clienti,nrClienti,client);
	logati[nrClienti]=0;
	userLobby[nrClienti]=0;
	nrClienti++;
	if(write(client,"<SERVER>Bine ai venit, pentru ajutor tasteaza /help\n",100)<0)
		perror("Eroare la write\n");

	  if (client < 0)
	    {
	      perror ("[server] Eroare la accept().\n");
	      continue;
	    }

          if (nfds < client)
            nfds = client;

	  FD_SET (client, &actfds);

	  printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));
	  fflush (stdout);
	}
      for (fd = 0; fd <= nfds; fd++)
	{
	  if (fd != sd && FD_ISSET (fd, &readfds))
	    {
	      if (Mesaj(fd,findClient(clienti,nrClienti,fd),nrClienti,clienti,logati,numeClienti,nrLobbies,lobbies,numeLobbies,userLobby)==0)
		{
		fflush(stdout);
		close (fd);
		erase(clienti,nrClienti,findClient(clienti,nrClienti,fd));
		erase(logati,nrClienti,findClient(clienti,nrClienti,fd));
		erase(userLobby,nrClienti,findClient(clienti,nrClienti,fd));
		eraseNume(numeClienti,nrClienti,findClient(clienti,nrClienti,fd));
		nrClienti--;
		FD_CLR (fd, &actfds);
		}
	    }
	}
    }
}

int Mesaj(int fd,int index,int nrClienti,int clienti[100],int logati[100],char numeClienti[20][20],int* nrLobbies,int lobbies[100],char numeLobbies[20][20],int userLobby[100])
	{

  	char mesaj[1000],mesajRasp[1000],comenzi[10][20];
	int tinta;
	bzero(comenzi,200);
	bzero(mesaj,1000);
	bzero(mesajRasp,1000);
  	if(read (fd, mesaj, 1000)<= 0)
    	{
      		perror ("Eroare la read() de la client.\n");
      		return 0;
    	}
	mesaj[strlen(mesaj)]=' ';
	desparteInComenzi(mesaj,comenzi);


if(userLobby[index]!=0)
	{
	if(strcmp(comenzi[0],"/exit")==0)
		{
		if(strcmp(comenzi[1],"lobby")==0)
			{
			userLobby[index]=0;
			strcat(mesajRasp,"<SERVER> Ai parasit lobby-ul cu succes");
			}
		}
	else
	if(strcmp(comenzi[0],"/invite")==0)
                {
                int ind;
                if((ind=findClientByName(numeClienti,nrClienti,comenzi[1]))!=-1)
                        {
                        strcat(mesajRasp,"Te invit in lobby-ul ");
                        strcat(mesajRasp,numeLobbies[findClient(lobbies,*nrLobbies,userLobby[index])]);
                      	strcat(mesajRasp,"! </join ");
                        strcat(mesajRasp,numeLobbies[findClient(lobbies,*nrLobbies,userLobby[index])]);
                        strcat(mesajRasp,">");
                        if(write(clienti[ind],mesajRasp,strlen(mesajRasp))<0)
				perror("Eroare la write\n");
                        bzero(mesajRasp,1000);
                        strcat(mesajRasp,"User invitat");
                        }
                else
                        {
                        strcat(mesajRasp,"User offline/inexistent");
                        }
                }
	else
	if(strcmp(comenzi[0],"/close")==0)
                {
                if(strcmp(comenzi[1],"current")==0)
                        {
                        int ind=findClient(lobbies,*nrLobbies,userLobby[index]);
                        kickUsers(lobbies[ind],userLobby,nrClienti,clienti);
                        eraseNume(numeLobbies,*nrLobbies,ind);
                        erase(lobbies,*nrLobbies,ind);
                        }
                else
                        {
                        int ind;
                        if((ind=findClientByName(numeLobbies,*nrLobbies,comenzi[1]))!=-1)
                                {
                                int ind=findClient(lobbies,*nrLobbies,userLobby[index]);
                                kickUsers(lobbies[ind],userLobby,nrClienti,clienti);
                                eraseNume(numeLobbies,*nrLobbies,ind);
                                erase(lobbies,*nrLobbies,ind);
                                strcat(mesajRasp,"Lobby inchis");
                                }
                        else
                                strcat(mesajRasp,"Lobby inexistent");
                        }
                }
	else
        if(strcmp(comenzi[0],"/history")==0)
                {
		if(strcmp(comenzi[1],"lobby")==0)
                        {
			if(strlen(comenzi[2])>0)
                        	checkHistoryLobby(comenzi[2],fd);
                        else	checkHistoryLobby(numeLobbies[findClient(lobbies,*nrLobbies,userLobby[index])],fd);
			}
                else
                if(!verificareLogin(comenzi[1]))
                        strcat(mesajRasp,"<SERVER> User inexistent");
                else
                        checkHistory(numeClienti[index],comenzi[1],fd);
                }
        else
        if(strcmp(comenzi[0],"/online")==0)
                {
                if(strcmp(comenzi[1],"users")==0)
                        {
                        getOnlineUsers(mesajRasp,numeClienti,nrClienti);
                        }
                }
        else
        if(strcmp(comenzi[0],"/offline")==0)
                {
                if(strcmp(comenzi[1],"users")==0)
                        {
                        getOfflineUsers(mesajRasp,numeClienti,nrClienti);
                        }
                }
        else
        if(strcmp(comenzi[0],"/users")==0)
                {
                        getUsers(mesajRasp,numeClienti,nrClienti);
                }
	else
        if(strcmp(comenzi[0],"/lobbies")==0)
                {
                getLobbies(mesajRasp,*nrLobbies,lobbies,numeLobbies);
                }
	else
	if(strcmp(comenzi[0],"/help")==0)
                {
                help(mesajRasp);
                }
        else
        if(strcmp(comenzi[0],"/send")==0)
                {
                char aux[1000];
                bzero(aux,1000);
                copyFromToEnd(aux,mesaj,6+strlen(comenzi[1]));
                strcat(mesajRasp,numeClienti[index]);
                strcat(mesajRasp,": ");
                strcat(mesajRasp,aux);
                tinta=findClientByName(numeClienti,nrClienti,comenzi[1]);
		if(tinta==-1)
                        {
                        if(!verificareLogin(comenzi[1]))
                                {
                                bzero(mesajRasp,1000);
                                strcat(mesajRasp,"<SERVER> User inexistent");
                                }
                                else
                                {
                                sendMail(comenzi[1],mesajRasp);
                                addHistory(numeClienti[index],comenzi[1],mesajRasp);
                                addHistory(comenzi[1],numeClienti[index],mesajRasp);
                                bzero(mesajRasp,1000);
                                }
                        }
                else
                        {
                        if(write(clienti[tinta],mesajRasp,1000)<0)
                                perror("Eroare la  write\n");
                        addHistory(numeClienti[index],comenzi[1],mesajRasp);
                        addHistory(comenzi[1],numeClienti[index],mesajRasp);
                        bzero(mesajRasp,1000);
                        }
                }
        else
        if(strcmp(comenzi[0],"/mail")==0)
                {
                checkMail(fd,numeClienti[index]);
                }
        else if(strlen(mesajRasp)==0)
		{
		strcat(mesajRasp,numeClienti[index]);
		strcat(mesajRasp,": ");
		strcat(mesajRasp,mesaj);
		sendMessageLobby(mesajRasp,clienti,userLobby,index,nrClienti);
		addHistoryLobby(numeLobbies[findClient(lobbies,*nrLobbies,userLobby[index])],mesajRasp);
		bzero(mesajRasp,1000);
		}
        if(write(fd,mesajRasp,strlen(mesajRasp))<0)
		perror("Eroare la write\n");

	}
else
if(logati[index])
	{
	if(strcmp(comenzi[0],"/help")==0)
		{
		help(mesajRasp);
		}
	else
	if(strcmp(comenzi[0],"/send")==0)
		{
		char aux[1000];
		bzero(aux,1000);
		copyFromToEnd(aux,mesaj,6+strlen(comenzi[1]));
		strcat(mesajRasp,numeClienti[index]);
		strcat(mesajRasp,": ");
		strcat(mesajRasp,aux);
		tinta=findClientByName(numeClienti,nrClienti,comenzi[1]);
		if(tinta==-1)
			{
			if(!verificareLogin(comenzi[1]))
				{
				bzero(mesajRasp,1000);
				strcat(mesajRasp,"<SERVER> User inexistent");
				}
				else
				{
				sendMail(comenzi[1],mesajRasp);
				addHistory(numeClienti[index],comenzi[1],mesajRasp);
	                        addHistory(comenzi[1],numeClienti[index],mesajRasp);
				bzero(mesajRasp,1000);
				}
			}
		else
			{
			if(write(clienti[tinta],mesajRasp,1000)<0)
				perror("Eroare la  write\n");
			addHistory(numeClienti[index],comenzi[1],mesajRasp);
			addHistory(comenzi[1],numeClienti[index],mesajRasp);
			bzero(mesajRasp,1000);
			}
		}
	else
	if(strcmp(comenzi[0],"/mail")==0)
		{
		checkMail(fd,numeClienti[index]);
		}
	else
	if(strcmp(comenzi[0],"/history")==0)
                {
		if(strcmp(comenzi[1],"lobby")==0)
			{
			checkHistoryLobby(comenzi[2],fd);
			}
		else
		if(!verificareLogin(comenzi[1]))
			strcat(mesajRasp,"<SERVER> User inexistent");
		else
                	checkHistory(numeClienti[index],comenzi[1],fd);
                }
	else
	if(strcmp(comenzi[0],"/online")==0)
		{
		if(strcmp(comenzi[1],"users")==0)
			{
			getOnlineUsers(mesajRasp,numeClienti,nrClienti);
			}
		}
	else
	if(strcmp(comenzi[0],"/offline")==0)
                {
                if(strcmp(comenzi[1],"users")==0)
                        {
                        getOfflineUsers(mesajRasp,numeClienti,nrClienti);
                        }
                }
	else
	if(strcmp(comenzi[0],"/users")==0)
                {
                        getUsers(mesajRasp,numeClienti,nrClienti);
                }
	else
	if(strcmp(comenzi[0],"/create")==0)
		{
			if(strcmp(comenzi[1],"lobby")==0)
				{
				if(strcmp(comenzi[2],"private")==0)
					{
        		       		insertNume(numeLobbies,*nrLobbies,comenzi[3]);
					lobbies[*nrLobbies]=-(rand()%1000+1);
					userLobby[index]=lobbies[*nrLobbies];
					(*nrLobbies)++;
					strcat(mesajRasp,"<SERVER> Lobby privat creat cu numele ");
					strcat(mesajRasp,comenzi[3]);
					}
				else
				if(strcmp(comenzi[2],"public")==0)
					{
                                        insertNume(numeLobbies,*nrLobbies,comenzi[3]);
                                        lobbies[*nrLobbies]=rand()%1000+1;
					userLobby[index]=lobbies[*nrLobbies];
                                        (*nrLobbies)++;
                                        strcat(mesajRasp,"<SERVER> Lobby public creat cu numele ");
                                        strcat(mesajRasp,comenzi[3]);
                                        }
				}
		}
	else
	if(strcmp(comenzi[0],"/join")==0)
		{
		int ind;
		if((ind=findClientByName(numeLobbies,*nrLobbies,comenzi[1]))!=-1)
			{
			userLobby[index]=lobbies[ind];
			strcat(mesajRasp,"Te-ai alaturat lobby-ului ");
			strcat(mesajRasp,comenzi[1]);
			}
			else
			strcat(mesajRasp,"Lobby inexistent");
		}
	else
	if(strcmp(comenzi[0],"/lobbies")==0)
		{
		getLobbies(mesajRasp,*nrLobbies,lobbies,numeLobbies);
		}
	else  strcat(mesajRasp,"<SERVER>Comanda invalida");
	if(write(fd,mesajRasp,strlen(mesajRasp))<0)
		perror("Eroare la write\n");
	}
else
	{
	if(strcmp(comenzi[0],"/login")==0)
		{
		if(verificareLogin(comenzi[1]) && findClientByName(numeClienti,nrClienti,comenzi[1])==-1)
			{
			logati[index]=1;
			insertNume(numeClienti,index,comenzi[1]);
			strcat(mesajRasp,"<SERVER> Te-ai logat cu succes\n");
			char aux[30];
			fflush(stdout);
			bzero(aux,30);
			strcat(aux,comenzi[1]);
			strcat(aux,"Mail.txt");
			int mesajeNecitite=nrMesaje(aux);
			intToChar(aux,mesajeNecitite);
			strcat(mesajRasp,"Ai ");
			strcat(mesajRasp,aux);
			strcat(mesajRasp," mesaje necitite. Tasteaza /mail pentru a le citii");
			}
		else    strcat(mesajRasp,"<SERVER> Username inexistent");
		}
	else
	if(strcmp(comenzi[0],"/register")==0)
		{
		if(!verificareLogin(comenzi[1]))
			{
			adaugaUser(comenzi[1]);
			strcat(mesajRasp,"<SERVER> Te-ai inregistrat cu succes, continua prin logare");
			}
		else
			strcat(mesajRasp,"<SERVER> Username-ul este folosit deja");
		}
	else
	if(strcmp(comenzi[0],"/help")==0)
                {
                help(mesajRasp);
		}
	else strcat(mesajRasp,"<SERVER>Comanda invalida");
	if(write(fd,mesajRasp,strlen(mesajRasp))<0)
		perror("Eroare la write\n");
	}
  return 1;
}
