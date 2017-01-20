#include "networkFunctions.h"

int createSocketListen(const char* portnum) {
  int s=-1;
	/* const int yes=1; */

  //TODO make shure it works with IPv6
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ALL; // TODO not only use local IP
  if (getaddrinfo("::0", portnum, &hints, &addresses) != 0) {
    perror("getaddrinfo");
    return 1;
  }

  for(p = addresses; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
		/* if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { */
		/* 	perror("setsockopt"); */
		/* 	exit(1); */
		/* } */
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
  // TODO hand over the address
  int s=-1;
	/* const int yes=1; */

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


void recieveList(int sock){
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
  unsigned long int filesize = 0;
	recv(sock,&filesize,sizeof(filesize),0);
  printf("recieved size: %ld\n", filesize);
  // TODO Split recieving for large files
  char *buff= (char*) malloc(sizeof(char)*filesize);
  recv(sock, buff, filesize, 0);
  //TODO write file to filesystem
  printf("Recieved File:\n%s\n", buff);
  free(buff);
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


void sendList(int sock, fileList *fl)	//sends all files from a given file list
{
  // send length of file List
  send(sock, (const void*) &fl->length, sizeof(fl->length),0);
  for (unsigned int i = 0; i< fl->length; i++){
    // TODO size of file? */
    send(sock, (const void*) &fl->entry[i].filesize, sizeof(fl->entry[i].filesize), 0);
    sendFile(sock, fl->entry[i].filename);
    // TODO send fileListEntry in addition? )Includes timestamp and hash
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

