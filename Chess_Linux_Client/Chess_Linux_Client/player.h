#pragma once
class PLAYER {
	PAWN **Pawn;
	KNIGHT **Knight;
	ROOK **Rook;
	BISHOP **Bishop;
	QUEEN **Queen;
	KING *King;
	PIECE *ref;
	int pawnN, knightN, rookN, bishopN, queenN;
public:
	void init(bool, bool);
	void show(int[8][8], int[8][8],POINT);
	void show(int[8][8]);
	bool move(int[8][8], POINT);
	void move(int[8][8], POINT, POINT);
	bool attacked(POINT);
	void promotion(int[8][8], POINT,int,bool);
	void castling(int[8][8], POINT,int);
	void can_enpassant(int[8][8],POINT,bool);
	void setRef(POINT);
	bool isCheck(int[8][8]);
	POINT getKingPos();
};

void PLAYER::init(bool isMine, bool isServer) {
	POINT p;

	pawnN = 8;
	knightN = rookN = bishopN = 2;
	queenN = 1;
	Pawn = new PAWN*[pawnN];
	Knight = new KNIGHT*[knightN];
	Rook = new ROOK*[rookN];
	Bishop = new BISHOP*[bishopN];
	Queen= new QUEEN*[queenN];
	p.x = 0;
	p.y = isMine ? 7 : 0;
	Rook[0] = new ROOK(p,true, isMine);
	p.x = 7;
	Rook[1] = new ROOK(p,true, isMine);
	p.x = 1;
	Knight[0] = new KNIGHT(p, isMine);
	p.x = 6;
	Knight[1] = new KNIGHT(p, isMine);
	p.x = 2;
	Bishop[0] = new BISHOP(p, isMine);
	p.x = 5;
	Bishop[1] = new BISHOP(p, isMine);
	p.x = isServer ?3:4;
	Queen[0] = new QUEEN(p, isMine);
	p.x = isServer ?4:3;
	King = new KING(p);
	p.y = isMine ? 6 : 1;
	for (int i = 0;i < 8;i++) {
		p.x = i;
		Pawn[i] = new PAWN(p,isMine);
	}
}

void PLAYER::show(int board[8][8],int moveP[8][8], POINT p) {
	int i;
	switch (board[p.y][p.x]) {
	case MY_PAWN:
		for (i = 0;i < pawnN;i++)
			if (Pawn[i]->inPiece(p)) {
				Pawn[i]->show(board);
				ref = Pawn[i];
				break;
			}
		break;
	case MY_KNIGHT:
		for (i = 0;i < knightN;i++)
			if (Knight[i]->inPiece(p)) {
				Knight[i]->show(board);
				ref = Knight[i];
				break;
			}
		break;
	case MY_BISHOP:
		for (i = 0;i < bishopN;i++)
			if (Bishop[i]->inPiece(p)) {
				Bishop[i]->show(board);
				ref = Bishop[i];
				break;
			}
		break;
	case MY_ROOK:
		for (i = 0;i < rookN;i++)
			if (Rook[i]->inPiece(p)) {
				Rook[i]->show(board);
				ref = Rook[i];
				break;
			}
		break;
	case MY_QUEEN:
		for (i = 0;i < queenN;i++)
			if (Queen[i]->inPiece(p)) {
				Queen[i]->show(board);
				ref = Queen[i];
				break;
			}
		break;
	case MY_KING:
		POINT tmp = King->getLoc();
		if (King->firstMove&&board[7][tmp.x]<CATCH) {
			if (Rook[1]->firstMove) {
				for (i = tmp.x + 1;i < 7;i++)
				{
					if (board[7][i] != NONE)
						break;
				}
				if (i == 7) {

					if (moveP[7][tmp.x + 2] != CAN_MOVE&&moveP[7][tmp.x + 1] != CAN_MOVE)
						board[7][tmp.x + 2] = CASTLING;
				}
			}
			if (Rook[0]->firstMove) {
				for (i = tmp.x - 1;i > 0;i--)
				{
					if (board[7][i] != NONE)
						break;
				}
				if (i == 0) {
					if (moveP[7][tmp.x - 2] != CAN_MOVE&&moveP[7][tmp.x - 1] != CAN_MOVE)
						board[7][tmp.x - 2] = CASTLING;
				}
			}
		}
		if (King->inPiece(p)) {
			King->show(board,moveP);
			ref = King;
			break;
		}

	}
}
void PLAYER::show(int board[8][8]) {
	int i;

	for (i = 0;i < pawnN;i++) 
		Pawn[i]->show(board);

	for (i = 0;i < knightN;i++)
		Knight[i]->show(board);

	for (i = 0;i < bishopN;i++) 
		Bishop[i]->show(board);
	
	for (i = 0;i < rookN;i++) 
		Rook[i]->show(board);
	
	for (i = 0;i < queenN;i++)
		Queen[i]->show(board);
	King->show(board);

}
bool PLAYER::move(int board[8][8], POINT p) {
	bool re = false;
	if (board[p.y][p.x] == CAN_MOVE || board[p.y][p.x] > CATCH|| board[p.y][p.x] >=ENPASSANT) {
		ref->move(p, board);
		re = true;
	}
	for (int i = 0;i < 8;i++) {
		for (int j = 0;j < 8;j++) {
			if (board[i][j] == CAN_MOVE || board[i][j] == CASTLING)
				board[i][j] = NONE;
			else if (board[i][j] >= ENPASSANT)
				board[i][j] -= ENPASSANT;
			else if (board[i][j] > CATCH)
				board[i][j] -= CATCH;
			else if (board[i][j] >= PROMOTION)
				board[i][j] -= PROMOTION;

		}
	}
	return re;
}
void PLAYER::move(int board[8][8], POINT p1, POINT p2) {
	int i;
	switch (board[p1.y][p1.x]) {
	case NMY_PAWN:
		for (i = 0;i < pawnN;i++)
			if (Pawn[i]->inPiece(p1)) {
				ref = Pawn[i];
				break;
			}
		break;
	case NMY_KNIGHT:
		for (i = 0;i < knightN;i++)
			if (Knight[i]->inPiece(p1)) {
				ref = Knight[i];
				break;
			}
		break;
	case NMY_BISHOP:
		for (i = 0;i < bishopN;i++)
			if (Bishop[i]->inPiece(p1)) {
				ref = Bishop[i];
				break;
			}
		break;
	case NMY_ROOK:
		for (i = 0;i < rookN;i++)
			if (Rook[i]->inPiece(p1)) {
				ref = Rook[i];
				break;
			}
		break;
	case NMY_QUEEN:
		for (i = 0;i < queenN;i++)
			if (Queen[i]->inPiece(p1)) {
				ref = Queen[i];
				break;
			}
	case NMY_KING:
		if (King->inPiece(p1)) {
			ref = King;
			break;
		}
	}
	board[p2.y][p2.x] = CAN_MOVE;
	ref->move(p2, board);
	board[p2.y][p2.x] += 10;
}
bool PLAYER::attacked(POINT p) {
	int i,j;
	for (i = 0;i < pawnN;i++)
		if (Pawn[i]->inPiece(p)) {
			delete Pawn[i];
			pawnN--;
			for (j = i;j < pawnN;j++)
				Pawn[j] = Pawn[j + 1];
			return false;
		}

	for (i = 0;i < knightN;i++)
		if (Knight[i]->inPiece(p)) {
			delete Knight[i];

			knightN--;
			for (j = i;j < knightN;j++)
				Knight[j] = Knight[j + 1];
			return false;
		}

	for (i = 0;i < bishopN;i++)
		if (Bishop[i]->inPiece(p)) {
			delete Bishop[i];
			bishopN--;
			for (j = i;j < bishopN;j++)
				Bishop[j] = Bishop[j + 1];
			return false;
		}

	for (i = 0;i < rookN;i++)
		if (Rook[i]->inPiece(p)) {
			delete Rook[i];
			rookN--;
			for (j = i;j < rookN;j++)
				Rook[j] = Rook[j + 1];
			return false;
		}
	for (i = 0;i < queenN;i++)
		if (Queen[i]->inPiece(p)) {
			delete Queen;

			queenN--;
			for (j = i;j < queenN;j++)
				Queen[j] = Queen[j + 1];
			return false;
		}
	if (King->inPiece(p)) {
		delete King;
		return true;
	}
	return false;

}
void PLAYER::promotion(int board[8][8], POINT p, int kind,bool isMine) {
	PIECE **tmp,*New;
	int i;
	switch (kind) {
	case NMY_QUEEN:
	case MY_QUEEN:
		tmp = (PIECE**)Queen;
		Queen = new QUEEN*[queenN + 1];
		for (i = 0;i < queenN;i++)
			Queen[i] =(QUEEN*) tmp[i];
		New=Queen[i] = new QUEEN(ref->getLoc(), isMine);
		queenN++;
		break;
	case NMY_ROOK:
	case MY_ROOK:
		tmp = (PIECE**)Rook;
		Rook = new ROOK*[rookN + 1];
		for (i = 0;i < rookN;i++)
			Rook[i] = (ROOK*)tmp[i];
		New=Rook[i] = new ROOK(ref->getLoc(),false, isMine);
		rookN++;
		break;
	case NMY_BISHOP:
	case MY_BISHOP:
		tmp = (PIECE**)Bishop;
		Bishop = new BISHOP*[rookN + 1];
		for (i = 0;i < rookN;i++)
			Bishop[i] = (BISHOP*)tmp[i];
		New = Bishop[i] = new BISHOP(ref->getLoc(), isMine);
		rookN++;
		break;
	case NMY_KNIGHT:
	case MY_KNIGHT:
		tmp = (PIECE**)Knight;
		Knight = new KNIGHT*[rookN + 1];
		for (i = 0;i < knightN;i++)
			Knight[i] = (KNIGHT*)tmp[i];
		New = Knight[i] = new KNIGHT(ref->getLoc(), isMine);
		knightN++;
		break;
	}
	New->move(p, board);
	if (kind > NOT_MINE)
		board[p.y][p.x] += 10;

	for (i = 0;i < pawnN;i++) {
		if (Pawn[i] == ref) {
			delete Pawn[i];
			pawnN--;
			for (int j = i;j < pawnN;j++)
				Pawn[j] = Pawn[j + 1];
			break;
		}
	}
	delete[]tmp;
}

void PLAYER::castling(int board[8][8], POINT p,int s) {
	char buffer[100],tmp_buffer[100];
	strcpy(buffer,"01C");
	write(s, buffer, strlen(buffer));
	sprintf(tmp_buffer, "M %d%d %d%d", King->getLoc().x, King->getLoc().y, p.x, p.y);
	sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
	write(s, buffer, strlen(buffer));
	
	if (p.x == King->getLoc().x + 2) {
		King->move(p, board);
		p.x -= 1;
		sprintf(tmp_buffer, "M %d%d %d%d", Rook[1]->getLoc().x, Rook[1]->getLoc().y, p.x, p.y);
		Rook[1]->move(p, board);
	}
	else {
		King->move(p, board);
		p.x += 1;
		sprintf(tmp_buffer, "M %d%d %d%d", Rook[0]->getLoc().x, Rook[0]->getLoc().y, p.x, p.y);
		Rook[0]->move(p, board);
	}
	sprintf(buffer, "%02d%s", strlen(tmp_buffer), tmp_buffer);
	write(s, buffer, strlen(buffer));
}

void PLAYER::can_enpassant(int board[8][8],POINT p,bool isServer) {
	int i;
	if (p.y ==3&&board[p.y][p.x]==MY_PAWN&&board[p.y-1][p.x] == NONE) {
		for (i = 0;i < pawnN;i++) {
			if (Pawn[i]->getLoc().y == 3 && Pawn[i]->movetwo()) {
				if (Pawn[i]->getLoc().x == p.x + 1)
					board[2][p.x + 1] += ENPASSANT;
				if(Pawn[i]->getLoc().x == p.x - 1)
					board[2][p.x - 1] += ENPASSANT;
			}
		}
	}
}

void PLAYER::setRef(POINT p) {
	for (int i = 0;i < pawnN;i++) {
		if (Pawn[i]->inPiece(p)) {
			ref = Pawn[i];
			break;
		}
	}
}

bool PLAYER::isCheck(int board[8][8]) {
	return (board[King->getLoc().y][King->getLoc().x] > CATCH);
}

POINT PLAYER::getKingPos() { return King->getLoc(); }