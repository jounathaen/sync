#include "networkFunctions.h"

int createSocketListen(const char* portnum) {
  int s=-1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ALL;
  if (getaddrinfo("::0", portnum, &hints, &addresses) != 0) {
    perror("getaddrinfo");
    return 1;
  }

  for(p = addresses; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
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


int createSocketSending(const char* ipaddr, const char* portnum) {
  int s=-1;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ALL;
  hints.ai_protocol = 0;          /* Any protocol */

  if (getaddrinfo(ipaddr, portnum, &hints, &addresses) != 0) {
    printf("getaddrinfo");
    return 1;
  }
  for(p = addresses; p != NULL; p = p->ai_next) {
    s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (s == -1)
        continue;

    if (connect(s, p->ai_addr, p->ai_addrlen) != -1)
        break;                  /* Success */

    close(s);
  }

  freeaddrinfo(addresses);
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }
  return s;
}

int recieveBuf(int sock) {
	char buff[1024];
	recv(sock, buff, sizeof(buff), 0);
  printf("Buffer:\n %s\n", buff);
	return 0;
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


void recieveListFiles(int sock){
	// recieve size of list of files
  unsigned int listsize;
	recv(sock,&listsize,sizeof(listsize),0);
  printf("Size of List to revieve: %d\n", listsize);

	//recieve file list
  for (unsigned int i = 0; i < listsize; i++){
    recieveFile(sock);
  }
  printf("-----------------\nDone revieving List\n");
  /* list = (fileList*) malloc(sizeof(fileList)*sol); */
  /* recv(sock,list,sizeof(list),0); */
}


void recieveFile(int sock){
  time_t timestamp = 0;
  char *name = (char*) malloc(FILENAME_MAX_SIZE);
  unsigned long int filesize = 0;
	recv(sock,&filesize,sizeof(filesize),0);
  printf("recieved size: %ld\n", filesize);
  // TODO Split recieving for large files
  /* recieve filename timestamp */
  char *buff= (char*) malloc(sizeof(char)*filesize);
  recv(sock, name, FILENAME_MAX_SIZE, 0);
  recv(sock, &timestamp, sizeof(time_t), 0);
  recv(sock, buff, filesize, 0);
  printf("Recieved Filename: %s\n", name);
  char tbuff [20];
  strftime(tbuff, sizeof(tbuff), "%b %d %H:%M", (struct tm*) localtime (&timestamp));
  printf("Recieved Timestamp: %-5s\n", tbuff);
  printf("Recieved File:\n%s\n", buff);
  //TODO write file to filesystem
  printf("\n\n");
  free(buff);
  free(name);
}


int sendBuf(int sock, char *buffer, int length) {
  int sent = 0;       // already sent bytes
	int left = length;  // remaining bytes to send
	int n;

	while(sent < length) {
		n = send(sock, buffer+sent, left, 0);
    printf("send %d bytes\n", n);
		if (n == -1)
      break;
		sent += n;
		left -= n;
	}
  // TODO add check for error
	length=sent; //number of actually sent bytes

	return n; // return -1 on error
}


void sendListFiles(int sock, fileList *fl)	//sends all files from a given file list
{
  // send length of file List
  send(sock, (const void*) &fl->length, sizeof(fl->length),0);
  for (unsigned int i = 0; i< fl->length; i++){
    sendFile(sock, &fl->entry[i]);
  }
}


void sendFile(int sock, fileListEntry * fle){
  send(sock, (const void*) &fle->filesize, sizeof(&fle->filesize), 0);
  send(sock, (const void*) fle->filename, sizeof(fle->filename), 0);
  send(sock, (const void*) &fle->timestamp, sizeof(&fle->timestamp), 0);
  sendFileContent(sock, fle->filename);
}


int sendFileContent(int sock, const char* filename){
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
