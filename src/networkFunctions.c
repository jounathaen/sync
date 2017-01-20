#include "networkFunctions.h"

int createSocket() {

  int s=-1;
	const int yes=1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // TODO not only use local IP
  if (getaddrinfo(NULL, PORTNUM, &hints, &addresses) != 0) {
    perror("getaddrinfo");
    return 1;
  }

  for(p = addresses; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
			close(s);
			perror("server: bind");
			continue;
		}
		break;
  }

  freeaddrinfo(addresses);
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }
  return s;

}


int recieveSync(int sock) {
	char *init="START_SYNC";
	char *comp="0";
	recv(sock,comp,sizeof(init),0);
	if(!strcmp(comp, init)) {
		printf("Wrong SYNC initilization");
		return 1;
	}
	return 0;
}


void recieveList(fileList *list, int sock){
	// recieve size of list of files
	char *sizeoflist[10];
	recv(sock,sizeoflist,sizeof(sizeoflist),0);
  int sol = atoi((const char*) sizeoflist);

	//recieve file list
  list = (fileList*) malloc(sizeof(fileList)*sol);
  recv(sock,list,sizeof(list),0);
}


void recieveFiles(int sock){
	char *sizeoflist[10];
	recv(sock,sizeoflist,sizeof(sizeoflist),0);
	int sol = atoi((const char*) sizeoflist);
	for(int i=0; i<sol;i++)
    {
      char *sizeoffile[10];
      recv(sock,sizeoffile,sizeof(sizeoffile),0);
      int sol = atoi((const char*) sizeoffile);

      char *file = (char*) malloc(sizeof(char)*sol);
      recv(sock,file,sizeof(file),0);
      //TODO write file to filesystem
      free(file);
    }
}


int sendBuf(int sock, char *buffer, int length) {
  /* struct sockaddr_in myaddr; */
  /* myaddr.sin_family = AF_INET; */
  /* myaddr.sin_port = htons(12345); */
  /* inet_aton("127.0.0.1", &myaddr.sin_addr); */

  int sent = 0;
	int left = length;
	int n;
	while(sent < length) {
    printf("sent: %d, length: %d\n", sent, length);
		n = send(sock, buffer+sent, left, 0);
		/* n = sendto(sock, buffer+sent, left, 0, (struct sockaddr*)&myaddr, sizeof(myaddr) ); */
    printf("send %d bytes\n", n);
		if (n == -1)
      break;
		sent += n;
    //TODO this does not make sense, the interval would shrink from both sides
		/* left -= n; */
	}
  // TODO is this for checks? explanation
	length=sent; //number of actually sent bytes

	return n; // return -1 on error
}


void sendList(int sock, fileList *fl)	//sends all files from a given file list
{
  printFileList(fl);
  // send length of file List
  send(sock, (const void*) &fl->length, sizeof(fl->length),0);
  // TODO wait for ack?

  for(unsigned int i=0; i < fl->length; i++) {
    /* char lengthbuff[10]; */
    /* sprintf(lengthbuff, "%d", fl->entry[i].filesize); //TODO size of file? */
    send(sock, (const void*) &fl->entry[i].filesize, sizeof(fl->entry[i].filesize), 0);
    // TODO wait for ack?
    printf("sending file %s:\n", fl->entry[i].filename);
    sendFile(sock, fl->entry[i].filename);
    // TODO send fileListEntry in addition? Includes timestamp and hash

    /* if ((sendEverything(sock,fopen(fl->entry[i].filename, "TODO"),length))==-1) //TODO send file? */
    /*   fprintf("Error sending file %s, only sent %d bytes \n", fl->entry[1].filename, length); */
  }
}

int sendFile(int sock, const char* filename){
  #define BUFFSIZE (64*1024)
  int bytes;
  FILE *inFile = fopen (filename, "rb");
  char buff[BUFFSIZE];

  if (inFile == NULL) {
    printf ("%s can't be opened.\n", filename);
    return -1;
  }
  while ((bytes = fread (buff, 1, BUFFSIZE, inFile)) != 0){
    if(sendBuf(sock, buff, bytes) == -1){
      printf("Error sending file: %s\n", filename);
      return -1;
    }
  }
  fclose (inFile);
  return 0;
}

// TODO close sockets

