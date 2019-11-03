#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "macro_table.h"
#include "point.h"
#include "pieces.h"
#include "player.h"

#define BUF_SIZE 100
using namespace std;
void error_handling(char const *message);
int connect(char*[]);
void play_game(int);
void print_board();
void init();
void Promotion(int, POINT);
void movePiece(int);

PLAYER Players[2];
int board[8][8],moveP[8][8];
char AllPieces[][4] = {"폰","나","비","룩","퀸","킹"};
bool turn, game = true, isWhite, isInit,isEnd, isCheck, isCanMove=false;
POINT tmp, bpoint;

int main(int argc, char *argv[])
{
	int sock;
	char ch;
	if (argc != 4) {
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	while (game) {
		system("clear");
		sock = connect(argv);
		//write(sock, message, strlen(message));
		read(sock, &ch, 1); //게임 시작 신호를 받는다.
		play_game(sock);
	}
	
	return 0;
}
int print_games(int sock) {
	int total, len, i, j, read_len;
	char message[BUF_SIZE],ch;
	total = 0;
	cout << "Game No|Player" << endl;
	cout << "-----------------" << endl;
	for (i = 0; i < 4; i++) {
		read(sock, &ch, 1);
		total = total * 10 + ch - '0';
	}
	if (!total)
		cout << "There are no games." << endl;
	for (i = 1; i <= total; i++) {
		len = 0;
		for (j = 0; j < 4; j++) {
			read(sock, &ch, 1);
			len = len * 10 + ch - '0';
		}
		read_len = 0;
		while (read_len < len) {
			read_len = read(sock, &message[read_len], len - read_len);
			
		}
		printf("%5d. | %s\n", i, message);
	}
	cout << "-----------------" << endl;
	return total;
}
int connect(char *argv[]) {
	int sock, selection;
	char message[BUF_SIZE], ch;
	int str_len, read_len, i, j, len, total;
	sockaddr_in serv_adr;
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

	total = print_games(sock);
	while (true) {
		cout << "Refresh games(-1), Create new game(0), Participate in the game(game no)" << endl;
		cout << "Input num : ";
		cin >> selection;
		if (!selection) {
			sprintf(message, "C%04d%s", strlen(argv[3]), argv[3]);
			write(sock, message, strlen(message)+1);
			isWhite=turn = true;
			cout << "Waiting..." << endl;
			break;
		}
		else if (selection > 0 && selection <= total) {
			sprintf(message, "%04d", selection - 1);
			write(sock, message, strlen(message));
			isWhite=turn = false;
			break;
		}
		else if (!~selection) {
			system("clear");
			strcpy(message, "R");
			write(sock, message, strlen(message));
			total = print_games(sock);
		}
		else
			cout << "Wrong Input! Please retry" << endl;
	}
	return sock;
}

void init() {
	int k, i, j;
	isInit = true;
	Players[0].init(true, isWhite);
	Players[1].init(false, isWhite);
	k = !isWhite;
	board[0][0] = board[0][7] = NMY_ROOK;
	board[0][1] = board[0][6] = NMY_KNIGHT;
	board[0][2] = board[0][5] = NMY_BISHOP;
	board[0][3] = isWhite ? NMY_QUEEN : NMY_KING;
	board[0][4] = isWhite ? NMY_KING : NMY_QUEEN;

	for (i = 2; i < 6; i++)
		for (j = 0; j < 8; j++)
			board[i][j] = NONE;

	for (i = 0; i < 8; i++) {
		board[6][i] = MY_PAWN;
		board[1][i] = NMY_PAWN;
	}

	board[7][0] = board[7][7] = MY_ROOK;
	board[7][1] = board[7][6] = MY_KNIGHT;
	board[7][2] = board[7][5] = MY_BISHOP;
	board[7][3] = isWhite ? MY_QUEEN : MY_KING;
	board[7][4] = isWhite ? MY_KING : MY_QUEEN;
}

void play_game(const int sock) {
	int len,i,j, read_len, str_len;
	char ch, message[BUF_SIZE];
	POINT tmp, tmp1;
	bool isCastling;

	init();
	print_board();
	if (isWhite)
		movePiece(sock);
	while (!isEnd) {
		while (!turn) {
			len = 0;
			for (i = 0; i < 2; i++) {
				read(sock, &ch, 1);
				len = len * 10 + ch - '0';
			}
			read_len = 0;
			while (read_len < len) {
				str_len = read(sock, &message[read_len], len - read_len);
				read_len += str_len;
			}
			switch (message[0])
			{
			case 'M':
				tmp.x = '7' - message[2];
				tmp.y = '7' - message[3];
				tmp1.x = '7' - message[5];
				tmp1.y = '7' - message[6];
				Players[1].move(board, tmp, tmp1);
				if (!isCastling) {
					turn = true;
				}
				else
					isCastling = false;
				print_board();

				
				break;
			case 'A':
				tmp.x = '7' - message[2];
				tmp.y = '7' - message[3];
				if (Players[0].attacked(tmp))
					isEnd = true;
				board[tmp.y][tmp.x] = NONE;
				print_board();
				break;
			case 'Q':
				char ans;
				cout << "You Win!!" << endl;
				turn = true;
				close(sock);
				while (true) {
					cout << "Play one more game? (Y/N)" << endl;
					cin >> ans;
					if (ans == 'N' || ans == 'n') {
						game = false;
						break;
					}
					else if (ans == 'Y' || ans == 'y') {
					game = true;
					break;
				}
					cout << "Wrong Input! Please retry" << endl;
				}
				isEnd = true;
				isInit = false;
				break;
			case 'C':
				isCastling = true;
				break;
			case 'P':
				tmp.x = '7' - message[2];
				tmp.y = '7' - message[3];
				tmp1.x = '7' - message[5];
				tmp1.y = '7' - message[6];
				if (Players[0].attacked(tmp)) {
					isEnd = true;
				}
				int kind = message[8] * 10 + message[9] - '0' * 11;
				Players[1].setRef(tmp);
				Players[1].promotion(board, tmp1, kind,false);
				print_board();
				turn = true;
				break;
			}
		}
		if (!isEnd) {
			print_board();
			movePiece(sock);
		}
	}
}

void print_board() {
	int i, j, bg;
	system("clear");
	isCheck = false;

	cout << " ";
	for (i = 0; i < 8; i++)
		printf("%2c", 'a' + i);
	cout << endl;
	for (j = 0; j < 8; j++) {
		printf("%d ", 8 - j);
		for (i = 0; i < 8; i++)
		{
			if (board[j][i] >= MINE && board[j][i] < PROMOTION) {

				if (board[j][i] > MINE) {
					if (board[j][i] < NOT_MINE) {

						if (i % 2 == j % 2)
							bg = 47;
						else
							bg = 40;
						if (isWhite)
							printf("%c[1;37m", 27);
						else
							printf("%c[1;30m", 27);
						printf("%2s", AllPieces[board[j][i] - 11]);
					}
					else {
						if (i % 2 == j % 2)
							bg = 47;
						else
							bg = 40;
						if (isWhite)
							printf("%c[1;30m", 27);
						else
							printf("%c[1;37m", 27);
						printf("%2s", AllPieces[board[j][i] - 21]);
					}
				}

			}
			else {
				if (board[j][i] == CATCH + MY_KING)
					board[j][i] -= CATCH;

				if (board[j][i] == CAN_MOVE)
					printf("%c[1;43m", 27);
				else if (board[j][i] == CASTLING)
					printf("%c[1;42m", 27);
				else if (board[j][i] >= ENPASSANT)
					printf("%c[1;46m", 27);
				else if (board[j][i] > CATCH)
					printf("%c[1;41m", 27);
				else if (board[j][i] >= PROMOTION)
					printf("%c[1;44m", 27);
				else {
					if (i % 2 == j % 2)
						printf("%c[1;47m", 27);
					else
						printf("%c[1;40m", 27);
				}

				printf("%2s", " ");
				printf("%c[0m", 27);
			}
			moveP[i][j] = board[i][j];
		}
		printf("%c[0m\n", 27);
	}
	if (isInit) {
		Players[1].show(moveP);
		POINT kp;
		if (Players[0].isCheck(moveP)) {
			isCheck = true;
			kp = Players[0].getKingPos();
		}
		for (i = 0; i < 8; i++)
			for (j = 0; j < 8; j++)
				moveP[i][j] = board[i][j];
		Players[0].show(moveP);
		if (Players[1].isCheck(moveP)) {
			kp = Players[1].getKingPos();
		}
		for (i = 0; i < 8; i++)
			for (j = 0; j < 8; j++)
				moveP[i][j] = board[i][j];
	}
	printf("%c[1;0m", 27);
	if (isCheck) {
		cout << "You have been checked. Look out!" << endl;
	}if (isEnd) {
		isEnd = turn = false;
		char ans;
		cout << "You Lose..." << endl;
		while (true) {
			cout << "Play one more game? (Y/N)" << endl;
			cin >> ans;
			if (ans == 'N' || ans == 'n')
				game = false;
			else if (ans == 'Y' || ans == 'y')
				game = true;
			cout << "Wrong Input! Please retry" << endl;
		}
	}
}
int count=1;
void movePiece(int sock) {
	bool isWin = false;
	char buffer[BUF_SIZE], tmp_buffer[BUF_SIZE];


	char x;
	while (true) {
		cout << count << ". Select Piece (x,y) : ";
		cin >> x >> tmp.y;
		if (tmp.y > 0 && tmp.y < 9) {
			tmp.y = 8 - tmp.y;
			if (x >= 'a' && x <= 'h') {
				tmp.x = x - 'a';
			}
			else if (x >= 'A' && x <= 'H') {
				tmp.x = x - 'A';
			}
		}

		if (board[tmp.y][tmp.x] >= NOT_MINE || board[tmp.y][tmp.x] <= MINE) {
			cout << "Wrong Input! Please retry" << endl;
			continue;
		}
		Players[1].show(moveP);
		Players[1].can_enpassant(board, tmp, isWhite);

		Players[0].show(board, moveP, tmp);
		bpoint.x = tmp.x;
		bpoint.y = tmp.y;

		print_board();



		cout << "Please select a location to move. (x,y) : ";
		cin >> x >> tmp.y;
		if (tmp.y > 0 && tmp.y < 9) {
			tmp.y = 8 - tmp.y;
			if (x >= 'a' && x <= 'h') {
				tmp.x = x - 'a';
			}
			else if (x >= 'A' && x <= 'H') {
				tmp.x = x - 'A';
			}
		}



		if (board[tmp.y][tmp.x] >= ENPASSANT) {
			tmp.y++;
			if (Players[1].attacked(tmp)) {
				isWin = true;
			}
			sprintf(tmp_buffer, "A %d%d", tmp.x, tmp.y);
			sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
			board[tmp.y][tmp.x] = NONE;
			write(sock, buffer, strlen(buffer));
			turn = false;
			tmp.y--;
		}
		else if (board[tmp.y][tmp.x] > CATCH) {
			sprintf(tmp_buffer, "A %d%d", tmp.x, tmp.y);
			sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
			
			write(sock, buffer, strlen(buffer));

			turn = false;
			if (Players[1].attacked(tmp)) {
				isWin = true;
			}
		}
		else if (board[tmp.y][tmp.x] == CASTLING) {
			turn = false;
			Players[0].castling(board, tmp, sock);
		}
		else if (board[tmp.y][tmp.x] >= PROMOTION) {
			turn = false;
			if (board[tmp.y][tmp.x] > PROMOTION) {
				if (Players[1].attacked(tmp)) {
					isWin = true;
				}
			}
			POINT Ppoint = tmp;
			Promotion(sock, Ppoint);
			print_board();
			break;
			//DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc1);
			//프로모션
		}
		if (Players[0].move(board, tmp)) {
			sprintf(tmp_buffer, "M %d%d %d%d", bpoint.x, bpoint.y, tmp.x, tmp.y);

			sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
			write(sock, buffer, strlen(buffer));
			if (isWin) {
				char ans;
				cout << "You Win!!" << endl;
				close(sock);
				while (true) {
					cout << "Play one more game? (Y/N)" << endl;
					cin >> ans;
					if (ans == 'N' || ans == 'n') {
						game = false;
						break;
					}
					else if (ans == 'Y' || ans == 'y') {
						game = true;
						break;
					}
					cout << "Wrong Input! Please retry" << endl;
				}
				isEnd = true;
				isInit = false;
			}
			isWin = turn = false;
		}
		print_board();
		if (!turn)
			break;
		cout << "Wrong Input! Please retry" << endl;
	}
	count++;
}

void Promotion(int sock,POINT point) {
	char ans, buffer[BUF_SIZE], tmp_buffer[BUF_SIZE];
	sprintf(buffer, "P %d%d %d%d ", bpoint.x, bpoint.y, point.x, point.y);
	cout << "Which one will Promote?(R:Rook, B:Bishop, K:Knight, Q:Queen)" << endl;
	cin >> ans;
	bool isEdited=false;
	int piece;
	while (!isEdited) {
		isEdited = true;
		switch (ans) {
		case 'Q':
			piece = NMY_QUEEN;
			break;
		case 'R':
			piece = NMY_ROOK;
			break;
		case 'B':
			piece = NMY_BISHOP;
			break;
		case 'K':
			piece = NMY_KNIGHT;
			break;
		default:
			cout << "Wrong Input! Please retry" << endl;
			isEdited = false;
			break;
		}
	}
	Players[0].promotion(board, point, piece-10,true);
	sprintf(tmp_buffer, "%s%d", buffer,piece);
	sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
	write(sock, buffer, strlen(buffer));
}

void error_handling(char const *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}