#include <iostream>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
using namespace std;
#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
//void send_msg(char * msg, int len, int clnt_sock);
void error_handling(char const * msg);
void * match(void * arg);

class List
{
	pthread_t t_id;
	List *next;
	int pipe_fd[2], ownerno;
	char ownerName[100];
public:
	List(int player, char const name[]) {next = NULL; pipe(pipe_fd); ownerno = player; strcpy(ownerName, name); }
	void setT_id(pthread_t t_id) { this->t_id = t_id; }
	int getWrPipe() { return pipe_fd[1]; }
	int getRdPipe() { return pipe_fd[0]; }
	int getOwner() { return ownerno; }
	char* getName() { return ownerName; }
	List* getNext() { return next; }
	List* addNext(int player, char const name[]) {
		if (next)
			return next->addNext(player, name);
		else
			return next = new List(player, name);
	}
	void setNext(List* next) { this->next = next; }
};

pthread_mutex_t mutx;
List *list = NULL, *tail = NULL;
int count = 0; //들어갈 수 있는 방의 수

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz, i;
	pthread_t t_id;

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	/*for (i = 0; i < 10; i++) {
		if (tail)
			tail = tail->addNext(1, "a");
		else
			tail = list = new List(1, "a");
		count++;
	}*/
	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (sockaddr*)&clnt_adr, (socklen_t*)&clnt_adr_sz);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);

		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
/*pthread_mutex_lock(&mutx);
  clnt_socks[clnt_cnt++] = clnt_sock;
  pthread_mutex_unlock(&mutx);*/
void * handle_clnt(void * arg)
{
	int clnt_sock = *((int*)arg);
	char msg[101] = { 0 };
	char buf1[100], buf2[100],ch;
	int i, s, len,total;
	List *tmp;
	pthread_t t_id;

	
	while (true) {
		sprintf(msg, "%04d", count); //현재 생성되어있는 방의 개수 전달
		write(clnt_sock, msg, 4);
		tmp = list;
		while (tmp)
		{
			sprintf(buf1, "%s", tmp->getName());
			len = strlen(buf1) + 1;
			sprintf(buf2, "%04d%s", len, buf1);

			write(clnt_sock, buf2, strlen(buf2) + 1);
			tmp = tmp->getNext();
		}
		read(clnt_sock, msg, 1);
		len = 0;
		if (msg[0] == 'C')//방을 만들겠다
		{
			total = 0;
			for (i = 0; i < 4; i++) {
				read(clnt_sock, &ch, 1);
				total = total * 10 + ch - '0';
			}
			while (len <= total) //방장이름
				len += read(clnt_sock, &msg[len], total - len + 1);
			if (count)
				tail = tail->addNext(clnt_sock, msg);
			else
				tail = list = new List(clnt_sock, msg);
			pthread_create(&t_id, NULL, match, (void*)tail);
			pthread_detach(t_id);
			tail->setT_id(t_id);
			pthread_mutex_lock(&mutx);
			count++;
			pthread_mutex_unlock(&mutx);
			cout << "Room has been created" << endl;
			break;
		}
		else if (msg[0] == 'R') { continue; } //방정보 재전송
		else {
			s = msg[0] - '0';
			while (len < 3) { //들어갈 방번호
				len += read(clnt_sock, &ch, 1);
				s = s * 10 + ch - '0';
			}
			tmp = list;

			//꽉찬방의 정보는 리스트에서 지우기
			if (s == 0) {
				list = list->getNext();
				write(tmp->getWrPipe(), (char*)&clnt_sock, 4);
			}
			else {
				for (i = 1; i < s; i++)
					tmp = tmp->getNext();
				List * tmp2 = tmp->getNext();
				tmp->setNext(tmp2->getNext());
				write(tmp2->getWrPipe(), (char*)&clnt_sock, 4);
			}

			pthread_mutex_lock(&mutx);
			count--; //들어갈수 있는방의 개수 감소
			pthread_mutex_unlock(&mutx);
			break;

		}
	}

	return NULL;
}
void * match(void * arg)
{
	List *data = (List*)arg, *tmp;
	bool turn = false;
	int len, str_len = 0, total, i, j;
	int rd_pipe = data->getRdPipe();
	int players[2] = { data->getOwner() };
	char msg[BUF_SIZE];
	bool isCastling = false;

	
	
	while (len < 4)
		len += read(rd_pipe, &msg[len], 1);
	players[1] = *(int*)msg;

	strcpy(msg, "R");
	for (i = 0; i < 2; i++)
		write(players[i], msg, 1);


	while (true) {
		len = 0;
		for (i = 0; i < 2; i++) {
			if (!read(players[turn], &msg[i], 1)) { //게임도중 플레이어가 나갔을때
				strcpy(msg, "01Q");
				write(players[!turn], msg, strlen(msg));
				for (i = 0; i < 2; i++)
					close(players[i]);
				delete data;
				return NULL;
			}
			len = len * 10 + msg[i] - '0';
		}
		total = 0;
		while (total < len) {
			total += read(players[turn], &msg[total + 2], len - total);
		}
		turn = !turn;
		
		
		if (msg[2] == 'Q') { //게임종료 신호
			write(players[turn], msg, strlen(msg));
			for (i = 0; i < 2; i++)
				close(players[i]);
			delete data;

			return NULL;
		}
		else {
			write(players[turn], msg, len + 2);
		}
		if (msg[2] == 'C') {
			isCastling = true;
			turn = !turn;
		}
		else if (msg[2] == 'A') {
			turn = !turn;
		}
		else if (isCastling) {
			isCastling = false;
			turn = !turn;
		}
	}

	/*pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(clnt_sock==clnt_socks[i])
		{
			for(j=i; j<clnt_cnt-1; j++)
				clnt_socks[j]=clnt_socks[j+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);*/
	return NULL;
}
/*void send_msg(char * msg, int len, int clnt_sock)   // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for (i = 0; i < clnt_cnt; i++) {
		if (clnt_sock != clnt_socks[i])
			write(clnt_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);
}*/
void error_handling(char const * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}