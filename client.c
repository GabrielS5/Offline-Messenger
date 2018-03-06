#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include<signal.h>
#include<fcntl.h>
extern int errno;

void  SIGQUIT_handler(int sig)
	{
       	exit(10);
	}

int getFirstWord(char mesaj[1000],char nume[20],char caracter)
	{
	bzero(nume,20);
	for(int i=0;i<20;i++)
		{
		if(mesaj[i]=='\0')
		return 0;
		else
		if(mesaj[i]==caracter)
			return 1;
		else
		nume[i]=mesaj[i];
		}
	return 0;
	}

void getLastName(char mesaj[1000],char nume[20])
	{
	char numeAux[20];
	bzero(numeAux,20);
	for(int i=0,j=0;i<strlen(mesaj);i++)
		{
		if(mesaj[i]==' ')
			{
			strcpy(nume,numeAux);
			bzero(numeAux,20);
			j=0;
			}
		else
			{
			numeAux[j]=mesaj[i];
			j++;
			}
		}
	}

int charToInt(char caracter[20])
	{
	int rezultat=0;

	for(int i=0;i<strlen(caracter);i++)
		rezultat=rezultat*10+(caracter[i]-48);

	return rezultat;
	}


int port;

int main (int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;
  char msg[1000];


  port =2728;

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[client] Eroare la socket().\n");
      return errno;
    }
  

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("0.0.0.0");
  server.sin_port = htons (port);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }

	int fiu,pip[2];
	pipe(pip);
	int flags=fcntl(pip[0],F_GETFL,0);
	flags=flags|O_NONBLOCK;
	fcntl(pip[0],F_SETFL,flags);

	if(fiu=fork())
	{
		close(pip[1]);
		char mesaj[1000],destinatari[1000],numeDest[20],comanda[20],comanda2[20];
		while(1)
			{
			bzero(destinatari,1000);
			bzero(mesaj,1000);
			scanf ("%[^\n]%*c", mesaj);

			read(pip[0],destinatari,1000);
			getLastName(destinatari,numeDest);
			if(getFirstWord(mesaj,comanda,' '))
				{
				if(strcmp(comanda,"/reply")==0)
					{
					if(strlen(numeDest)>0)
						{
						strcpy(mesaj,mesaj+6);
						bzero(destinatari,1000);
						strcat(destinatari,"/send ");
						strcat(destinatari,numeDest);
						strcat(destinatari,mesaj);
						strcpy(mesaj,destinatari);
						}
					}
				else	if(getFirstWord(comanda,comanda2,'-'))
						{
						if(strcmp(comanda2,"/delay")==0)
							{
							strcpy(comanda,comanda+7);
							int delay=charToInt(comanda);
							if(!fork())
								{
								sleep(delay);
								strcpy(mesaj,mesaj+8+strlen(comanda));
								fflush(stdout);
								if (write (sd, mesaj, strlen(mesaj)) <= 0)
                                                                	{
					                                perror ("[client]Eroare la write() spre server.\n");
                        					        return errno;
                        					        }
								exit(1);
								}
							bzero(mesaj,1000);
							}
						}
				}
		if(strlen(mesaj)>0)
			if (write (sd, mesaj, 1000) <= 0)
		   		{
      				perror ("[client]Eroare la write() spre server.\n");
      				return errno;
    				}
			if(strstr(mesaj,"quit")!=NULL)
				{
				close(sd);
				kill(fiu,SIGQUIT);
				exit(1);
				}

			}

		}
	else
		{
		close(pip[0]);

		while(1)
			{
			bzero(msg,1000);
  			if (read (sd, msg, 1000) < 0)
   	 			{
      				perror ("[client]Eroare la read() de la server.\n");
      				return errno;
    				}
  			printf ("\n%s\n", msg);
			char nume[20];
			bzero(nume,20);
  			if(getFirstWord(msg,nume,':'))
				{
				strcat(nume," ");
				if(write(pip[1],nume,strlen(nume))<0)
					perror("Eroare la write\n");
				}
			}
		}
  close (sd);
}

