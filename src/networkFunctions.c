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
    int reuse = 1;
    // SO_REUSEADDR so we don't have to wait before re-starting the program
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
      printf("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
    if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
      printf("setsockopt(SO_REUSEPORT) failed");
#endif

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
	char comp[20];
	recv(sock, comp, strlen(init), 0);
	if(strncmp(comp, init, 5)) {
		printf("Wrong SYNC initilization\nWas %s and %s \n", init, comp);
		return -1;
	}
	return 0;
}


void recieveListFiles(int sock, const char * prependdir){
	// recieve size of list of files
  /* TODO handle empty lists */
  unsigned int listsize;
	recv(sock,&listsize,sizeof(listsize),0);
  printf("Size of List to revieve: %d\n", listsize);

	//recieve file list
  for (unsigned int i = 0; i < listsize; i++){
    recieveFile(sock, prependdir);
  }
  printf("-----------------\nDone revieving List\n");
}


void recieveFile(int sock, const char * prependdir){
  time_t timestamp = 0;
  char *name = (char*) malloc(FILENAME_MAX_SIZE);
  unsigned long int filesize = 0;
	recv(sock,&filesize,sizeof(filesize),0);
  printf("recieved size: %ld\n", filesize);
  char *buff= (char*) malloc(sizeof(char)*filesize);
  recv(sock, name, FILENAME_MAX_SIZE, 0);
  recv(sock, &timestamp, sizeof(time_t), 0);
  recv(sock, buff, filesize, 0);
  printf("Recieved Filename: %s\n", name);
  char tbuff [20];
  strftime(tbuff, sizeof(tbuff), "%b %d %H:%M", (struct tm*) localtime (&timestamp));
  printf("Recieved Timestamp: %-5s\n", tbuff);
  printf("Recieved File:\n%s\n", buff);

  char namebuff[FILENAME_MAX_SIZE];
  char *dirbuff;
  strcpy(namebuff, prependdir);
  strcat(namebuff, name);
  dirbuff = strdup(namebuff);
  dirbuff = dirname(dirbuff);
  /* Check if directory exists  */
  printf("Dirname is: %s\n", dirbuff);
  printf("Filename is: %s\n", namebuff);

  // recursive directory creation
  char tmp[256];
  char *p = NULL;
  size_t len;
  snprintf(tmp, sizeof(tmp),"%s",dirbuff);
  len = strlen(tmp);
  if(tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++)
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);

  FILE *writefile = fopen(namebuff, "w");
  fputs(buff, writefile);
  printf("wrote to fileSystem. Filename: %s Directory %s\n\n", namebuff, dirbuff);
  fclose(writefile);
  //TODO modify timestamp
  free(buff);
  free(name);
}


void recieveList(int sock, fileList * fl){
  int recievedNr = 0;
  recv(sock, &recievedNr, sizeof(int), 0);
  fl->length = ntohl(recievedNr);
  printf("revieved file list header with length %u \n", fl->length);
  if (fl->length > 0) {
    fl->entry = (fileListEntry *) malloc(sizeof(fileListEntry) * fl->length);
    recv(sock, fl->entry, sizeof(fileListEntry) * fl->length, 0);
  }
}


void sendList(int sock, fileList * fl){
  int convertednr = htonl(fl->length);
  send(sock, (const void *) &convertednr, sizeof(convertednr), 0);
  if (fl->length > 0) {
    send(sock, (const void *) fl->entry, sizeof(fileListEntry)*fl->length, 0);
  }
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
	length=sent; //number of actually sent bytes

	return n; // return -1 on error
}


void sendListFiles(int sock, fileList *fl, const char* prependdir){
  send(sock, (const void*) &fl->length, sizeof(fl->length),0);
  for (unsigned int i = 0; i< fl->length; i++){
    sendFile(sock, &fl->entry[i], prependdir);
  }
}


void sendFile(int sock, fileListEntry * fle, const char* prependdir){
  send(sock, (const void*) &fle->filesize, sizeof(&fle->filesize), 0);
  send(sock, (const void*) fle->filename, sizeof(fle->filename), 0);
  send(sock, (const void*) &fle->timestamp, sizeof(&fle->timestamp), 0);
  char buff [FILENAME_MAX_SIZE];
  strcpy(buff, prependdir);
  strcat(buff, fle->filename);
  sendFileContent(sock, buff);
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
