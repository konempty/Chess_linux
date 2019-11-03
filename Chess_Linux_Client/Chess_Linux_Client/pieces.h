#pragma once
#ifndef PIECES_H
#define PIECES_H


class PIECE {
protected:
	POINT location;
	bool isNotMine;
public:
	POINT getLoc() { return location; }
	virtual void move(POINT, int[8][8])=0;
	virtual void show(int[8][8]) = 0;
	virtual ~PIECE() {};
	PIECE(POINT p, bool isNotMine) { location = p; this->isNotMine = !isNotMine; }
	bool inPiece(POINT p) { return (p.x == location.x&&p.y == location.y); }
};

class KING :public PIECE {
	PIECE *super;
public:
	bool firstMove;
	void move(POINT, int[8][8]);
	void show(int[8][8]);

	void show(int[8][8],int[8][8]);
	KING(POINT p) :PIECE(p,true) { firstMove = true;super = this; }
};

class QUEEN :public PIECE {
public:
	void move(POINT, int[8][8]);
	void show(int[8][8]);
	QUEEN(POINT p, bool b) :PIECE(p, b) { ; }
};

class ROOK :public PIECE {
	PIECE *super;
public:
	bool firstMove;
	void move(POINT, int[8][8]);
	void show(int[8][8]);
	ROOK(POINT p,bool b,bool isNotMine) :PIECE(p, isNotMine) { firstMove = b; super = this; }
};

class BISHOP :public PIECE {
public:
	void move(POINT, int[8][8]);
	void show(int[8][8]);
	BISHOP(POINT p, bool b) :PIECE(p, b) { ; }
};

class KNIGHT :public PIECE {
public:
	void move(POINT, int[8][8]);
	void show(int[8][8]);
	KNIGHT(POINT p, bool b) :PIECE(p,b) { ; }
};

class PAWN :public PIECE {
	bool firstMove,moveTwo;
	PIECE *super;
public:
	void move(POINT, int[8][8]);
	void show(int[8][8]);
	PAWN(POINT p, bool b) :PIECE(p,b) {firstMove = true;super = this;moveTwo = false;}
	bool movetwo(){ return moveTwo; }
};

void KING::move(POINT p, int board[8][8]) {
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_KING;
	firstMove = false;
}
void KING::show(int board[8][8]) {
	for (int i = -1;i < 2;i++) {
		if (location.y + i < 0 || location.y + i>7)
			continue;
		for (int j = -1;j < 2;j++) {
			if ((location.x + j < 0 || location.x + j>7) || (i == 0 && j == 0))
				continue;
			if (board[location.y + i][location.x + j] == NONE)
				board[location.y + i][location.x + j] = CAN_MOVE;
			else if (board[location.y + i][location.x + j] > NOT_MINE)
				board[location.y + i][location.x + j] += CATCH;
		}
	}
}
void KING::show(int board[8][8], int moveP[8][8]) {
	for (int i = -1;i < 2;i++) {
		if (location.y + i < 0 || location.y + i>7)
			continue;
		for (int j = -1;j < 2;j++) {
			if ((location.x + j < 0 || location.x + j>7) || (i == 0 && j == 0))
				continue;
			if (board[location.y + i][location.x + j] == NONE&&moveP[location.y + i][location.x + j]!=CAN_MOVE)
				board[location.y + i][location.x + j] = CAN_MOVE;
			else if (board[location.y + i][location.x + j] > NOT_MINE|| board[location.y + i][location.x + j]==MY_KING)
				board[location.y + i][location.x + j] += CATCH;
		}
	}
}
void QUEEN::move(POINT p, int board[8][8]) {
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_QUEEN;
}
void QUEEN::show(int board[8][8]) {
	int i, j, k, x, y;
	bool poss[] = { 1,1,1,1 };

	i = 1;
	while (1) {
		if (location.y + i > 7)
			poss[3] = poss[2] = 0;
		if (location.y < i)
			poss[0] = poss[1] = 0;
		if (location.x + i > 7)
			poss[1] = poss[3] = 0;
		if (location.x < i)
			poss[0] = poss[2] = 0;

		if (!(poss[0] || poss[1] || poss[2] || poss[3]))
			break;

		y = -1;
		for (j = 0;j < 2;j++) {
			x = -1;
			for (k = 0;k < 2;k++) {
				if (poss[j * 2 + k]) {
					if (board[location.y + i*y][location.x + i*x] == NONE|| board[location.y + i*y][location.x + i*x] == CAN_MOVE)
						board[location.y + i*y][location.x + i*x] = CAN_MOVE;
					else {
						if (board[location.y + i*y][location.x + i*x] > NOT_MINE|| (isNotMine&&board[location.y + i*y][location.x + i*x]==MY_KING))
							board[location.y + i*y][location.x + i*x] += CATCH;
						poss[j * 2 + k] = 0;
					}
				}
				x = -x;
			}
			y = -y;
		}
		i++;
	}

	i = 1;
	poss[0] = poss[1] = poss[2] = poss[3] = 1;
	while (1) {
		if (location.y + i > 7)
			poss[0] = 0;
		if (location.y < i)
			poss[1] = 0;
		if (location.x + i > 7)
			poss[2] = 0;
		if (location.x < i)
			poss[3] = 0;

		if (!(poss[0] ||poss[1] || poss[2] || poss[3]))
			break;

		y = 1;
		for (j = 0;j < 2;j++) {
			if (poss[j]) {
				if (board[location.y + i*y][location.x] == NONE|| board[location.y + i*y][location.x] == CAN_MOVE)
					board[location.y + i*y][location.x] = CAN_MOVE;
				else {
					if (board[location.y + i*y][location.x] > NOT_MINE|| (isNotMine&&board[location.y + i*y][location.x]==MY_KING))
						board[location.y + i*y][location.x] += CATCH;
					poss[j] = 0;
				}
			}
			if (poss[j + 2]) {
				if (board[location.y][location.x + i*y] == NONE|| board[location.y][location.x + i*x] == CAN_MOVE)
					board[location.y][location.x + i*y] = CAN_MOVE;
				else {
					if (board[location.y][location.x + i*y] > NOT_MINE|| (isNotMine&&board[location.y][location.x + i*y]==MY_KING))
						board[location.y][location.x + i*y] += CATCH;
					poss[j + 2] = 0;
				}
			}
			y = -y;
		}
		i++;
	}
}
void ROOK::move(POINT p, int board[8][8]) {
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_ROOK;
	firstMove = false;
}

void ROOK::show(int board[8][8]) {
	int i = 1, j, s;
	bool poss[] = { 1,1,1,1 };
	while (1) {
		if (location.y + i > 7)
			poss[0] = 0;
		if (location.y < i)
			poss[1] = 0;
		if (location.x + i > 7)
			poss[2] = 0;
		if (location.x < i)
			poss[3] = 0;

		if (!(poss[0] || poss[1] || poss[2] || poss[3]))
			break;

		s = 1;
		for (j = 0;j < 2;j++) {
			if (poss[j]) {
				if (board[location.y + i*s][location.x] == NONE|| board[location.y + i*s][location.x] == CAN_MOVE)
					board[location.y + i*s][location.x] = CAN_MOVE;
				else {
					if (board[location.y + i*s][location.x] > NOT_MINE|| (isNotMine&&board[location.y + i*s][location.x]==MY_KING))
						board[location.y + i*s][location.x] += CATCH;
					poss[j] = 0;
				}
			}
			if (poss[j + 2]) {
				if (board[location.y][location.x + i*s] == NONE|| board[location.y][location.x + i*s] == CAN_MOVE)
					board[location.y][location.x + i*s] = CAN_MOVE;
				else {
					if (board[location.y][location.x + i*s] > NOT_MINE|| (isNotMine&&board[location.y][location.x + i*s]==MY_KING))
						board[location.y][location.x + i*s] += CATCH;
					poss[j + 2] = 0;
				}
			}
			s = -s;
		}
		i++;
	}
}

void BISHOP::move(POINT p, int board[8][8]) {
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_BISHOP;
}
void BISHOP::show(int board[8][8]) {
	int i, j, k, x, y;
	bool poss[] = { 1,1,1,1 };

	i = 1;
	while (1) {
		if (location.y + i > 7)
			poss[3] = poss[2] = 0;
		if (location.y < i)
			poss[0] = poss[1] = 0;
		if (location.x + i > 7)
			poss[1] = poss[3] = 0;
		if (location.x < i)
			poss[0] = poss[2] = 0;

		if (!(poss[0] || poss[1] || poss[2] || poss[3]))
			break;

		y = -1;
		for (j = 0;j < 2;j++) {
			x = -1;
			for (k = 0;k < 2;k++) {
				if (poss[j * 2 + k]) {
					if (board[location.y + i*y][location.x + i*x] == NONE || board[location.y + i*y][location.x + i*x] == CAN_MOVE)
						board[location.y + i*y][location.x + i*x] = CAN_MOVE;
					else {
						if (board[location.y + i*y][location.x + i*x] > NOT_MINE|| (isNotMine&&board[location.y + i*y][location.x + i*x]==MY_KING))
							board[location.y + i*y][location.x + i*x] += CATCH;
						poss[j * 2 + k] = 0;
					}
				}
				x = -x;
			}
			y = -y;
		}
		i++;
	}
}
void KNIGHT::move(POINT p, int board[8][8]) {
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_KNIGHT;
}
void KNIGHT::show(int board[8][8]) {
	int x = 1, y, i, j, k, s;
	for (i = 0;i < 2;i++) {
		y = 1;
		for (j = 0;j < 2;j++) {
			s = 1;
			for (k = 0;k < 2;k++) {

				if (location.y + s + y<8 && location.y + s + y>-1 && location.x + x<8 && location.x + x>-1) {
					if (board[location.y + s + y][location.x + x] == NONE&&(s+y==2||s+y==-2))
						board[location.y + s + y][location.x + x] = CAN_MOVE;
					else if ((board[location.y + s + y][location.x + x] > NOT_MINE|| (isNotMine&&board[location.y + s + y][location.x + x]==MY_KING)) && (s + y == 2 || s + y == -2))
						board[location.y + s + y][location.x + x] += CATCH;
				}
				if(location.y + y<8 && location.y + y>-1 && location.x + s + x<8 && location.x + s + x>-1){
					if (board[location.y + y][location.x + s + x] == NONE && (s + x == 2 || s + x == -2))
						board[location.y + y][location.x + s + x] = CAN_MOVE;
					else if ((board[location.y + y][location.x + s + x] > NOT_MINE|| (isNotMine&&board[location.y + y][location.x + s + x]==MY_KING)) && (s + x == 2 || s + x == -2))
						board[location.y + y][location.x + s + x] += CATCH;
				}
				s = -s;
			}
			y = -y;
		}
		x = -x;
	}
}
void PAWN::move(POINT p, int board[8][8]) {
	if (p.y-location.y == 2)
		moveTwo = true;
	board[location.y][location.x] = NONE;
	location = p;
	board[p.y][p.x] = MY_PAWN;
	firstMove = false;
}
void PAWN::show(int board[8][8]) {
	int i;
	if (isNotMine) {
		if (location.x > 0 && board[location.y + 1][location.x - 1] == MY_KING)
			board[location.y + 1][location.x - 1] = CATCH;
		else
			board[location.y + 1][location.x - 1] = CAN_MOVE;
		if (location.x < 8 && board[location.y + 1][location.x - 1] == MY_KING)
			board[location.y + 1][location.x + 1] = CATCH;
		else
			board[location.y + 1][location.x + 1] = CAN_MOVE;
		if (board[location.y + 1][location.x] == NONE)
			board[location.y + 1][location.x] = CAN_MOVE;
		
	}
	else {
		if (location.x > 0 && board[location.y - 1][location.x - 1]<ENPASSANT&&board[location.y - 1][location.x - 1] > NOT_MINE) {
			if (location.y == 1)
				board[location.y - 1][location.x - 1] += PROMOTION;
			else
				board[location.y - 1][location.x - 1] += CATCH;

		}

		if (location.x < 8 && board[location.y - 1][location.x + 1]<ENPASSANT&&board[location.y - 1][location.x + 1] > NOT_MINE) {
			if (location.y == 1)
				board[location.y - 1][location.x + 1] += PROMOTION;
			else
				board[location.y - 1][location.x + 1] += CATCH;
		}
		if (firstMove)
		{
			for (i = 1; i <= 2; i++)
			{
				if (board[location.y - i][location.x] == NONE)
					board[location.y - i][location.x] = CAN_MOVE;
				else
					break;
			}
		}
		else {

			if (board[location.y - 1][location.x] == NONE) {
				if (location.y == 1)
					board[location.y - 1][location.x] += PROMOTION;
				else
					board[location.y - 1][location.x] = CAN_MOVE;
			}
		}

		

	}
}
#endif // !PIECES_H