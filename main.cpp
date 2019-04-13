#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

enum Type
{
	None,
	Pawn,
	Bishop,
	Rook,
	Knight,
	King,
	Queen
};

enum Owner
{
	Neutral,
	White,
	Black
};

enum MoveType
{
	Regular,
	EnPassant,
	Castling,
	Promotion
};

struct Move
{
	Move() {}
	Move(IntVec2 f, float tx, float ty, MoveType t)
	{
		From = f;
		To.x = tx;
		To.y = ty;
		type = t;
	}

	IntVec2 From;
	IntVec2 To;
	MoveType type;
	float q;
};

struct Flags
{
	vector<IntVec2> walkTB;
	vector<IntVec2> walkTW;
	bool kingW = true;
	bool kingB = true;
	array<bool, 2> rookW = { { true, true } };
	array<bool, 2> rookB = { { true, true } };
};

struct State
{
	GameMap gmap;
	GameMap omap;
	Flags flags;

	bool isN(int x, int y)
	{
		if (x >= 0 && x < 8 && y >= 0 && y < 8)
			return omap[x][y] == Neutral;
		return false;
	}

	bool isE(int x, int y)
	{
		auto enemy = HamI == White ? Black : White;
		if (x >= 0 && x < 8 && y >= 0 && y < 8)
			return omap[x][y] == enemy;
		return false;
	}

	bool isNE(int x, int y)
	{
		auto enemy = HamI == White ? Black : White;
		if (x >= 0 && x < 8 && y >= 0 && y < 8)
		{
			if (omap[x][y] == enemy || omap[x][y] == Neutral)
				return true;
			return false;
		}
		return false;
	}

	void changeP()
	{
		if (HamI == White)
		{
			HamI = Black;
		}
		else
		{
			HamI = White;
		}
	}

	IntVec2 findKing(int owner)
	{
		IntVec2 king;
		for (int x = 0; x < gmap.w; ++x)
		{
			for (int y = 0; y < gmap.h; ++y)
			{
				if (gmap[x][y] == King && omap[x][y] == owner)
				{
					king = IntVec2(x, y);
				}
			}
		}
		return king;
	}

	int HamI = White;
};

enum MoveClass
{
	AllAllowed,
	Hits
};

bool isAttacking(State& state, IntVec2 v);
void MoveTo(State& state, Move m)
{
	auto isWhite = state.HamI == White;
	auto& mywalk = isWhite ? state.flags.walkTW : state.flags.walkTB;
	auto& enemywalk = isWhite ? state.flags.walkTB : state.flags.walkTW;
	int dir = isWhite ? 1 : -1;
	if (state.gmap[m.From] == Pawn)
	{
		if (m.From.y == m.To.y - 2 * dir)
			mywalk.emplace_back(m.To.x, m.To.y - dir);

		if (m.type == EnPassant)
		{
			state.gmap[m.To.x][m.To.y - dir] = None;
			state.omap[m.To.x][m.To.y - dir] = Neutral;
		}
	}

	enemywalk.clear();
	auto& king = state.HamI == White ? state.flags.kingW : state.flags.kingB;
	auto& rook = state.HamI == White ? state.flags.rookW : state.flags.rookB;
	if (state.gmap[m.From] == Rook)
	{
		if (m.To.x > 4)
		{
			rook[1] = false;
		}
		else
		{
			rook[0] = false;
		}
	}
	if (state.gmap[m.From] == King && !(m.From.x + 2 == m.To.x || m.From.x - 2 == m.To.x))
	{
		king = false;
	}
	if (m.type == Castling)
	{
		king = false;
		if (m.To.x > 4)
		{
			rook[1] = false;
			state.gmap[m.From.x + 1][m.From.y] = Rook;
			state.omap[m.From.x + 1][m.From.y] = state.HamI;
			state.gmap[m.From.x + 4][m.From.y] = None;
			state.omap[m.From.x + 4][m.From.y] = Neutral;
		}
		else
		{
			rook[0] = false;
			state.gmap[m.From.x - 1][m.From.y] = Rook;
			state.omap[m.From.x - 1][m.From.y] = state.HamI;
			state.gmap[m.From.x - 3][m.From.y] = None;
			state.omap[m.From.x - 3][m.From.y] = Neutral;
		}
	}

	if (m.type == Promotion)
	{
		state.gmap[m.To] = Queen;
	}
	else
	{
		state.gmap[m.To] = state.gmap[m.From];
	}
	state.omap[m.To] = state.omap[m.From];
	state.gmap[m.From] = None;
	state.omap[m.From] = Neutral;
}

void MoveBack(State& state, Move m, int prevFigure, int prevOwner)
{
	auto isWhite = state.HamI == White;
	auto enemy = isWhite == White ? Black : White;
	auto& enemywalk = isWhite ? state.flags.walkTB : state.flags.walkTW;
	int dir = isWhite ? 1 : -1;
	if (m.type == EnPassant)
	{
		state.gmap[m.To.x][m.To.y - dir] = Pawn;
		state.omap[m.To.x][m.To.y - dir] = enemy;
	}

	if (m.type == Castling)
	{
		if (m.To.x > 4)
		{
			state.gmap[m.From.x + 1][m.From.y] = None;
			state.omap[m.From.x + 1][m.From.y] = Neutral;
			state.gmap[m.From.x + 4][m.From.y] = Rook;
			state.omap[m.From.x + 4][m.From.y] = state.HamI;
		}
		else
		{
			state.gmap[m.From.x - 1][m.From.y] = None;
			state.omap[m.From.x - 1][m.From.y] = Neutral;
			state.gmap[m.From.x - 3][m.From.y] = Rook;
			state.omap[m.From.x - 3][m.From.y] = state.HamI;
		}
	}

	if (m.type == Promotion)
	{
		state.gmap[m.From] = Pawn;
	}
	else
	{
		state.gmap[m.From] = state.gmap[m.To];
	}
	state.omap[m.From] = state.omap[m.To];
	state.gmap[m.To] = prevFigure;
	state.omap[m.To] = prevOwner;
}


void movePawn(State& state, IntVec2 p, MoveClass mclass, vector<Move>& v)
{
	v.clear();
	auto WmyO = state.HamI == White ? Black : White;
	auto isWhite = state.omap[p] == White;
	int dir = isWhite ? 1 : -1;
	auto enemy = isWhite ? Black : White;

	//if (state.omap[p] == White)
	if (state.omap[p] != WmyO)
	{
		if (state.gmap[p] == Pawn)
		{
			int yNext = p.y + dir;
			auto yLast = isWhite ? 7 : 0;
			auto type = yLast == yNext ? Promotion : Regular;

			if (0 <= yNext && yNext < 8 && p.x < 8 && p.x >= 0)
			{
				if (state.omap[p.x][yNext] == Neutral && mclass != Hits)
				{
					v.emplace_back(p, p.x, yNext, type);
				}

				if (p.x != 7 && state.omap[p.x + 1][yNext] == enemy)
				{
					v.emplace_back(p,p.x + 1, yNext, type);
				}

				if (p.x != 0 && state.omap[p.x - 1][yNext] == enemy)
				{
					v.emplace_back(p,p.x - 1, yNext, type);
				}

				auto& walkT = state.HamI == White ? state.flags.walkTB : state.flags.walkTW;
				for (auto n : walkT)
				{
					if (p.x + 1 == n.x && yNext == n.y)
					{
						v.emplace_back(p, p.x + 1, yNext, EnPassant);
					}

					if (p.x - 1 == n.x && yNext == n.y)
					{
						v.emplace_back(p,p.x - 1, yNext, EnPassant);
					}
				}
			}

			int d = isWhite ? 1 : 6;
			if (p.y == d && state.omap[p.x][p.y + 2 * dir] == Neutral
				&& state.omap[p.x][p.y + 1 * dir] == Neutral && mclass != Hits)
			{
				v.emplace_back(p,p.x, p.y + 2 * dir, Regular);

			}
		}



		if (state.gmap[p] == Knight)
		{
			if (state.isNE(p.x - 1, p.y + 2))
				v.emplace_back(p,p.x - 1, p.y + 2, Regular);

			if (state.isNE(p.x + 1, p.y + 2))
				v.emplace_back(p,p.x + 1, p.y + 2, Regular);

			if (state.isNE(p.x - 1, p.y - 2))
				v.emplace_back(p,p.x - 1, p.y - 2, Regular);

			if (state.isNE(p.x + 1, p.y - 2))
				v.emplace_back(p,p.x + 1, p.y - 2, Regular);

			if (state.isNE(p.x + 2, p.y + 1))
				v.emplace_back(p,p.x + 2, p.y + 1, Regular);

			if (state.isNE(p.x + 2, p.y - 1))
				v.emplace_back(p,p.x + 2, p.y - 1, Regular);

			if (state.isNE(p.x - 2, p.y + 1))
				v.emplace_back(p,p.x - 2, p.y + 1, Regular);

			if (state.isNE(p.x - 2, p.y - 1))
				v.emplace_back(p,p.x - 2, p.y - 1, Regular);
		}

		if (state.gmap[p] == King)
		{
			int yN1 = p.y + 1, yN2 = p.y - 1;
			int xN1 = p.x + 1, xN2 = p.x - 1;

			if (yN1 < 8)
			{
				if (state.omap[p.x][yN1] == Neutral)
					v.emplace_back(p,p.x, yN1, Regular);

				if (xN1 < 8 && state.omap[xN1][yN1] == Neutral)
					v.emplace_back(p,xN1, yN1, Regular);

				if (xN2 >= 0 && state.omap[xN2][yN1] == Neutral)
					v.emplace_back(p,xN2, yN1, Regular);
			}

			if (yN2 >= 0)
			{
				if (state.omap[p.x][yN2] == Neutral)
					v.emplace_back(p,p.x, yN2, Regular);

				if (xN1 < 8 && state.omap[xN1][yN2] == Neutral)
					v.emplace_back(p,xN1, yN2, Regular);

				if (xN2 >= 0 && state.omap[xN2][yN2] == Neutral)
					v.emplace_back(p,xN2, yN2, Regular);
			}

			if (xN1 < 8 && state.omap[p.x + 1][p.y] == Neutral)
				v.emplace_back(p,p.x + 1, p.y, Regular);

			if (xN2 >= 0 && state.omap[p.x - 1][p.y] == Neutral)
				v.emplace_back(p,p.x - 1, p.y, Regular);

			auto& king = state.HamI == White ? state.flags.kingW : state.flags.kingB;
			auto& rook = state.HamI == White ? state.flags.rookW : state.flags.rookB;
			if (king)
			{

				if (p.x - 3 > 0 && state.gmap[p.x - 1][p.y] == None && state.gmap[p.x - 2][p.y] == None
					&& state.gmap[p.x - 3][p.y] == Rook && rook[0] == true)
				{
					v.emplace_back(p,p.x - 2, p.y, Castling);
				}


				if (p.x + 4 < 8 && state.gmap[p.x + 1][p.y] == None && state.gmap[p.x + 2][p.y] == None
					&& state.gmap[p.x + 3][p.y] == None && state.gmap[p.x + 4][p.y] == Rook
					&& rook[1] == true)
				{
					v.emplace_back(p,p.x + 2, p.y, Castling);
				}
			}
		}

		if (state.gmap[p] == Rook || state.gmap[p] == Queen)
		{

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y))
					v.emplace_back(p,p.x + i, p.y, Regular);
				else
				{
					if (state.isE(p.x + i, p.y))
					{
						v.emplace_back(p, p.x + i, p.y, Regular);
						break;
					}
					else
						break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y))
					v.emplace_back(p,p.x - i, p.y, Regular);
				else
				{
					if (state.isE(p.x - i, p.y))
					{
						v.emplace_back(p, p.x - i, p.y, Regular);
						break;
					}
					else break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x, p.y + i))
					v.emplace_back(p,p.x, p.y + i, Regular);
				else
				{
					if (state.isE(p.x, p.y + i))
					{
						v.emplace_back(p, p.x, p.y + i, Regular);
						break;
					}
					else break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x, p.y - i))
					v.emplace_back(p,p.x, p.y - i, Regular);
				else
				{
					if (state.isE(p.x, p.y - i))
					{
						v.emplace_back(p, p.x, p.y - i, Regular);
						break;
					}
					else break;
				}
			}
		}

		if (state.gmap[p] == Bishop || state.gmap[p] == Queen)
		{
			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y + i))
					v.emplace_back(p,p.x + i, p.y + i, Regular);
				else
				{
					if (state.isE(p.x + i, p.y + i))
					{
						v.emplace_back(p, p.x + i, p.y + i, Regular);
						break;
					}
					else break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y - i))
					v.emplace_back(p,p.x - i, p.y - i, Regular);
				else
				{
					if (state.isE(p.x - i, p.y - i))
					{
						v.emplace_back(p, p.x - i, p.y - i, Regular);
						break;
					}
					else break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y + i))
					v.emplace_back(p,p.x - i, p.y + i, Regular);
				else
				{
					if (state.isE(p.x - i, p.y + i))
					{
						v.emplace_back(p, p.x - i, p.y + i, Regular);
						break;
					}
					else break;
				}
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y - i))
					v.emplace_back(p,p.x + i, p.y - i, Regular);
				else
				{
					if (state.isE(p.x + i, p.y - i))
					{
						v.emplace_back(p, p.x + i, p.y - i, Regular);
						break;
					}
					else break;
				}
			}
		}
		if (state.gmap[p] == King)
		{
			if (p.x + 1 <= 7 && state.isNE(p.x + 1, p.y))
			{
				v.emplace_back(p,p.x + 1, p.y, Regular);
			}
			if (p.x - 1 >= 0 && state.isNE(p.x - 1, p.y))
			{
				v.emplace_back(p,p.x - 1, p.y, Regular);
			}
			if (p.y + 1 <= 7 && state.isNE(p.x, p.y + 1))
			{
				v.emplace_back(p,p.x, p.y + 1, Regular);
			}
			if (p.y - 1 >= 0 && state.isNE(p.x, p.y - 1))
			{
				v.emplace_back(p,p.x, p.y - 1, Regular);
			}
			if (p.x + 1 <= 7 && p.y + 1 <= 7 && state.isNE(p.x + 1, p.y + 1))
			{
				v.emplace_back(p,p.x + 1, p.y + 1, Regular);
			}
			if (p.x + 1 <= 7 && p.y - 1 >= 0 && state.isNE(p.x + 1, p.y - 1))
			{
				v.emplace_back(p,p.x + 1, p.y - 1, Regular);
			}
			if (p.x - 1 >= 0 && p.y + 1 <= 7 && state.isNE(p.x - 1, p.y + 1))
			{
				v.emplace_back(p,p.x - 1, p.y + 1, Regular);
			}
			if (p.x - 1 >= 0 && p.x - 1 >= 0 && state.isNE(p.x - 1, p.y - 1))
			{
				v.emplace_back(p,p.x - 1, p.y - 1, Regular);
			}
		}
	}

	if (mclass == AllAllowed)
	{
		vector<Move> v2;
		IntVec2 king = state.findKing(state.HamI);

		auto flags = state.flags;
		for (auto p2 : v)
		{
			auto Ifking = p == king ? p2.To : king;

			auto prevFigure = state.gmap[p2.To];
			auto prevOwner = state.omap[p2.To];
			MoveTo(state,p2);
			state.changeP();

			if (!isAttacking(state, Ifking))
			{
				v2.push_back(p2);
			}

			state.changeP();
			state.flags = flags;
			MoveBack(state, p2, prevFigure, prevOwner);
		}
		swap(v, v2);
	}
}

bool isAttacking(State& state, IntVec2 v)
{
	/**/int x, y;
	vector<Move> n;
	for (int x = 0; x < state.gmap.w; ++x)
	{
		for (int y = 0; y < state.gmap.h; ++y)
		{
			if (state.omap[x][y] == state.HamI)
			{
				movePawn(state, IntVec2(x, y), Hits, n);
				for (auto f : n)
				{
					if (v == f.To)                    
					{
						return true;
					}
				}
			}
		}
	}
	//if (state.gmap[v.x + 1][v.y + 1]==Knight)

//	if(state.gmap[v.x+1][v.y+1] == Pawn)

	return false;
}


bool Ischeck(State& state)
{
	IntVec2 king = state.findKing(state.HamI);
	state.changeP();
	auto result = isAttacking(state, king);
	state.changeP();
	return result;
}

bool Isstalemate(State& state)
{
	int n = 0;
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y <= 7; ++y)
		{
			if (state.gmap[x][y] != None)
				n++;
		}
	}
	if (n == 2)
		return true;
	vector<Move> c;
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y <= 7; ++y)
		{
			if (state.omap[x][y] == state.HamI)
			{
				movePawn(state, IntVec2(x, y), AllAllowed, c);
				if (c.size() > 0)
					return false;
			}
		}
	}
	return true;
}

class MyApp : public App
{
	void load()
	{
		connect(ButRest, Rest);
		ButRest.setPos(0, -270);
		Rest();
	}

	void Rest()
	{
		columnB.clear();
		columnW.clear();
		randomize();
		state.HamI = White;
		figures.clear();
		map<Color, int> colorToType = {
		{ Color(255, 255, 255), None },
		{ Color(255, 0, 0), Pawn },
		{ Color(0, 0, 255), Bishop },
		{ Color(0, 255, 0), Rook },
		{ Color(255, 168, 0), Knight },
		{ Color(255, 255, 0), King },
		{ Color(0, 255, 255), Queen }
		};

		map<Color, int> colorToOwner = {
		{ Color(255, 255, 255), White },
		{ Color(150, 150, 150), Neutral },
		{ Color(0, 0, 0), Black }
		};

		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				auto square = squares.load("Square.json", 50 * x, 50 * y);
				if ((x + y) % 2 != 0)
				{
					square.skin<FilledRect>().setColor(172, 125, 96);
				}
				else
				{
					square.skin<FilledRect>().setColor(250, 233, 209);
				}
			}
		}
		state.gmap = loadMap("pole.png", colorToType);
		state.omap = loadMap("owner.png", colorToOwner);

		for (int x = 0; x < state.gmap.w; ++x)
		{
			for (int y = 0; y < state.gmap.h; ++y)
			{
				if (state.gmap[x][y] == Pawn)
				{
					auto pawn = figures.load("Pawn.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						pawn.skin<Texture>().setColor(0, 0, 0);
				}

				if (state.gmap[x][y] == Rook)
				{
					auto rook = figures.load("Rook.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						rook.skin<Texture>().setColor(0, 0, 0);

				}

				if (state.gmap[x][y] == Bishop)
				{
					auto bishop = figures.load("Bishop.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						bishop.skin<Texture>().setColor(0, 0, 0);

				}

				if (state.gmap[x][y] == Knight)
				{
					auto knight = figures.load("Knight.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						knight.skin<Texture>().setColor(0, 0, 0);

				}

				if (state.gmap[x][y] == King)
				{
					auto king = figures.load("King.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						king.skin<Texture>().setColor(0, 0, 0);

				}

				if (state.gmap[x][y] == Queen)
				{
					auto queen = figures.load("Queen.json", 50 * x, 50 * y);
					if (state.omap[x][y] == Black)
						queen.skin<Texture>().setColor(0, 0, 0);

				}

			}
		}
		field.setView(400 / 2 - 50 / 2, 400 / 2 - 50 / 2);
		black.hide();
		check.hide();
		checkmate.hide();
		stalemate.hide();
		ButRook.setPos(260, 155);  ButRook.hide();   //connect(ButRook, Butrook);
		ButBishop.setPos(260, 80); ButBishop.hide(); //connect(ButBishop, Butbishop);
		ButKnight.setPos(260, 5);  ButKnight.hide(); //connect(ButKnight, Butknight);
		ButQueen.setPos(260, -70); ButQueen.hide();  //connect(ButQueen, Butqueen);
		ButPawn.setPos(260, -145); ButPawn.hide();   //connect(ButPawn, Butpawn);
		ButRest.setPos(0, -300); ButPawn.hide();   //connect(ButPawn, Butpawn);
	}

	IntVec2 cell(Vec2 v)
	{
		v.x += 25;
		v.y += 25;
		v /= 50;
		return IntVec2(v.x, v.y);
	}

	void Butrook(IntVec2 p)
	{
		for (auto i : figures.find(p.x * 50, p.y * 50))
			i.kill();
		state.gmap[p] = Rook;
		auto rook = figures.load("Rook.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			rook.skin<Texture>().setColor(0, 0, 0);
	}
	void Butbishop(IntVec2 p)
	{
		for (auto i : figures.find(p.x * 50, p.y * 50))
			i.kill();
		state.gmap[p] = Bishop;
		auto bishop = figures.load("Bishop.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			bishop.skin<Texture>().setColor(0, 0, 0);
	}
	void Butknight(IntVec2 p)
	{
		for (auto i : figures.find(p.x * 50, p.y * 50))
			i.kill();
		state.gmap[p] = Knight;
		auto knight = figures.load("Knight.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			knight.skin<Texture>().setColor(0, 0, 0);
	}
	void Butqueen(IntVec2 p)
	{
		for (auto i : figures.find(p.x * 50, p.y * 50))
			i.kill();
		state.gmap[p] = Queen;
		auto queen = figures.load("Queen.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			queen.skin<Texture>().setColor(0, 0, 0);
	}
	void Butpawn(IntVec2 p)
	{
		for (auto i : figures.find(p.x * 50, p.y * 50))
			i.kill();
		state.gmap[p] = Pawn;
		auto pawn = figures.load("Pawn.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			pawn.skin<Texture>().setColor(0, 0, 0);
	}

	State copyState(State& state)
	{
		State newState;
		newState.gmap.map = state.gmap.map;
		newState.gmap.w = state.gmap.w;
		newState.gmap.h = state.gmap.h;
		newState.omap.map = state.omap.map;
		newState.omap.w = state.omap.w;
		newState.omap.h = state.omap.h;
		newState.flags = state.flags;
		newState.HamI = state.HamI;
		return newState;
	}

	vector<Move> computermoves()
	{
		vector<Move> c;
		vector<Move> result;
		auto copy = copyState(state);
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (state.omap[x][y] == state.HamI)
				{
					movePawn(state, IntVec2(x, y), AllAllowed, c);
					for (auto m : c)
					{      
						m.q = analyze(m);
						result.push_back(m);
					}
				}
			}
		}
		if (state.gmap.map != copy.gmap.map)
			cout << "AAAAAAAAA!!!!!!!!!!!!!!!" << endl;
		state = std::move(copy);
		return result;
	}

	//bool IsInDanger(State& state)
	/*{
		vector<IntVec2> c;
		vector<IntVec2> n;
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y <= 7; ++y)
			{
				if (state.omap[x][y] == state.HamI)
				{
					c = movePawn(state, IntVec2(x, y), AllAllowed);


					for (int x1 = 0; x1 < 8; ++x1)
					{
						for (int y1 = 0; y1 <= 7; ++y1)
						{

						}
					}
				}
			}
		}
	}*/

	float analyzeOneStep(Move m)
	{
		float step = 0;
		if (state.gmap[m.To] == Queen)
			step += 100;
		if (state.gmap[m.To] == Knight || state.gmap[m.To] == Bishop)
			step += 50;
		if (state.gmap[m.To] == Rook)
			step += 75;
		if (state.gmap[m.To] == Pawn)
			step += 25;
		if (state.gmap[m.To] == None)
			step += 10;
		auto prevFigure = state.gmap[m.To];
		auto prevOwner = state.omap[m.To];
		auto flags = state.flags;
		MoveTo(state, m);
		state.changeP();
		if (Ischeck(state))
			step += 20;

		if (Ischeck(state) && Isstalemate(state))
			step += 1000000;
		state.changeP();
		state.flags = flags;
		MoveBack(state, m, prevFigure, prevOwner);
		return step + randomFloat(-0.001, 0.001);

		if (state.gmap[m.From] == King)
			step += 5;
		if (state.gmap[m.From] == Knight)
			step += 20;
		if (state.gmap[m.From] == Rook)
			step += 20;
		if (state.gmap[m.From] == Bishop)
			step += 20;
		if (state.gmap[m.From] == Queen)
			step += 25;
		if (state.gmap[m.From] == Pawn)
			step += 15;
	}


	//bool NotSameMove(State state, Move m)
	/*{
		if (state.gmap[m.From] == lastfigure)
			k.push_back(m.From);
		else
			k.clear();

		if (k[0] == k[3] && k[3] == k[5] && k[2] == k[4] && k[4] == k[6])
		return true;

		return false;
	}*/

	/*float analyze(Move m)
	{
		float max = -10000000;
		float MyStep;
		MyStep = analyzeOneStep(m);
		auto prevFigure = state.gmap[m.To];
		auto prevOwner = state.omap[m.To];
		state.gmap[m.To] = state.gmap[m.From];
		state.omap[m.To] = state.omap[m.From];
		state.gmap[m.From] = None;
		state.omap[m.From] = Neutral;
		state.changeP();
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (state.omap[x][y] == state.HamI)
				{
					auto c = movePawn(state, IntVec2(x, y), AllAllowed);
					for (auto i : c)
					{
						Move n;
						n.From = IntVec2(x, y);
						n.To = i;
						n.q = analyzeOneStep(n);
						if (n.q > max)
							max = n.q;
					}
				}
			}
		}
		state.changeP();
		state.gmap[m.From] = state.gmap[m.To];
		state.omap[m.From] = state.omap[m.To];
		state.gmap[m.To] = prevFigure;
		state.omap[m.To] = prevOwner;
		return MyStep - max;
	}*/

	

	float analyze(Move m, int depth)
	{
		float MyStep = analyzeOneStep(m);
		vector<Move> c;
		if (depth > 1)
		{
			float max = -10000000;
			auto prevFigure = state.gmap[m.To];
			auto prevOwner = state.omap[m.To];
			auto flags = state.flags;
			MoveTo(state, m);
			state.changeP();
			for (int x = 0; x < 8; ++x)
			{
				for (int y = 0; y < 8; ++y)
				{
					if (state.omap[x][y] == state.HamI)
					{
						movePawn(state, IntVec2(x, y), AllAllowed, c);
						for (auto n : c)
						{
							
							n.q = analyze(n, depth - 1);
							if (n.q > max)
								max = n.q;
						}
					}
				}
			}
			state.changeP();
			state.flags = flags;
			MoveBack(state, m, prevFigure, prevOwner);
			return MyStep - max;
		}
		return MyStep;
	}

	float analyze(Move m)
	{
		return analyze(m, 4);
	}

	/*float analyze(Move m)
	{
		float max = -10000000,  maxn = -10000000;
		float MyStep;
		MyStep = analyzeOneStep(m);
		auto prevFigure = state.gmap[m.To];
		auto prevOwner = state.omap[m.To];
		state.gmap[m.To] = state.gmap[m.From];
		state.omap[m.To] = state.omap[m.From];
		state.gmap[m.From] = None;
		state.omap[m.From] = Neutral;
		state.changeP();
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (state.omap[x][y] == state.HamI)
				{
					auto c = movePawn(state, IntVec2(x, y), AllAllowed);
					for (auto i : c)
					{
						Move n;
						n.From = IntVec2(x, y);
						n.To = i;
						n.q = analyzeOneStep(n);
						auto prevOpFigure = state.gmap[n.To];
						auto prevOpOwner = state.omap[n.To];
						state.gmap[n.To] = state.gmap[n.From];
						state.omap[n.To] = state.omap[n.From];
						state.gmap[n.From] = None;
						state.omap[n.From] = Neutral;
						state.changeP();

						for (int x = 0; x < 8; ++x)
						{
							for (int y = 0; y < 8; ++y)
							{
								if (state.omap[x][y] == state.HamI)
								{
									auto v = movePawn(state, IntVec2(x, y), AllAllowed);
									for (auto j : v)
									{
										Move f;
										f.From = IntVec2(x, y);
										f.To = j;
										f.q = analyzeOneStep(f);
										if (f.q > maxn)
											maxn = f.q;
									}
								}
							}
						}
						state.changeP();
						state.gmap[n.From] = state.gmap[n.To];
						state.omap[n.From] = state.omap[n.To];
						state.gmap[n.To] = prevOpFigure;
						state.omap[n.To] = prevOpOwner;

						if (n.q > max)
							max = n.q- maxn;
					}
				}
			}
		}
		state.changeP();
		state.gmap[m.From] = state.gmap[m.To];
		state.omap[m.From] = state.omap[m.To];
		state.gmap[m.To] = prevFigure;
		state.omap[m.To] = prevOwner;
		return MyStep - max;
	}*/

	//void useful(Move m)
	//{}

	void compmove()
	{
		auto moves = computermoves();
		bool hasMove = false;
		float q = -100000000.0;
		Move m;
		for (auto i : moves)
		{
			if (!hasMove || i.q > q)
			{
				q = i.q;
				m = i;
				hasMove = true;
			}
		}
		if (!hasMove)
			return;
		makemove(m.From, m.To);
	}

	//vector<IntVec2> moves(vector<IntVec2> c)
	/*{
		vector<IntVec2> n;
		for(auto i: c)
		{
			for (int x = 0; x < 8; ++x)
			{
				for (int y = 0; y < 8; ++y)
				{
					if (state.omap[x][y] == state.HamI)
					{
						n = movePawn(state, IntVec2(x, y), AllAllowed);

						for (auto j : n)
						{
							auto prevFigure = state.gmap[p2];
							auto prevOwner = state.omap[p2];
							state.gmap[p2] = state.gmap[p];
							state.omap[p2] = state.omap[p];
							state.gmap[p] = None;
							state.omap[p] = Neutral;

							if (!isAttacking(state, Ifking))
							{
								v2.push_back(p2);
							}

							state.gmap[p] = state.gmap[p2];
							state.omap[p] = state.omap[p2];
							state.gmap[p2] = prevFigure;
							state.omap[p2] = prevOwner;
						}

					}
				}
			}
		}
	}*/

	void text(State& state, IntVec2 from, IntVec2 to)
	{
		char h = state.gmap[to] == None ? h = '-' : h = ':';

		auto k = state.gmap[from];
		vector<string> fs = { "", "", "B", "R", "N", "K", "Q" };
		if (state.HamI == White)
		{
			auto labelW = columnW.load<Label>("Label.json");
			labelW << fs[k] << usefulltext(from) << from.y + 1 << ' ' << h << ' ' << usefulltext(to) << to.y + 1;
		}
		else
		{
			auto labelB = columnB.load<Label>("Label.json");
			labelB << fs[k] << usefulltext(from) << from.y + 1 << ' ' << h << ' ' << usefulltext(to) << to.y + 1;
		}
		design.update();
	}

	char usefulltext(IntVec2 some)
	{
		return 'a' + some.x;
	}


	void makemove(IntVec2 from, IntVec2 to)
	{
		if (!Isstalemate(state))
		{
			auto isWhite = state.HamI == White;
			auto enemy = isWhite ? Black : White;
			auto& mywalk = isWhite ? state.flags.walkTW : state.flags.walkTB;
			auto& enemywalk = isWhite ? state.flags.walkTB : state.flags.walkTW;
			if (state.gmap[from] == Pawn)
			{
				int dir = isWhite ? 1 : -1;
				if (from.y == to.y - 2 * dir)
					mywalk.emplace_back(to.x, to.y - dir);

				for (auto n : enemywalk)
				{
					if (n == to)
					{
						state.gmap[to.x][to.y - dir] = None;
						state.omap[to.x][to.y - dir] = Neutral;
						for (auto i : figures.find(to.x * 50, (to.y - dir) * 50))
							i.kill();
					}
				}
			}

			bool isCompUpgradePawn = false;
			if (state.gmap[from] == Pawn && (to.y == 0 || to.y == 7))
			{
				ButRook.show();
				ButBishop.show();
				ButKnight.show();
				ButQueen.show();
				ButPawn.show();

				connect(ButRook, Butrook, to);
				connect(ButBishop, Butbishop, to);
				connect(ButKnight, Butknight, to);
				connect(ButQueen, Butqueen, to);
				connect(ButPawn, Butpawn, to);

				if (state.HamI == Black)
					isCompUpgradePawn = true;
			}

			text(state, from, to);

			if (state.omap[to] == enemy)
				for (auto i : figures.find(to.x * 50, to.y * 50))
					i.kill();
			enemywalk.clear();

			figures.find(from.x * 50, from.y * 50).back().setPos(to.x * 50, to.y * 50);
			state.gmap[to] = state.gmap[from];



			state.omap[to] = state.HamI;
			auto& king = state.HamI == White ? state.flags.kingW : state.flags.kingB;
			auto& rook = state.HamI == White ? state.flags.rookW : state.flags.rookB;
			if (state.gmap[from] == Rook)
			{
				if (to.x > 4)
				{
					rook[1] = false;
				}
				else
				{
					rook[0] = false;
				}
			}
			if (state.gmap[from] == King && !(from.x + 2 == to.x || from.x - 2 == to.x))
			{
				king = false;
			}
			if ((from.x + 2 == to.x || from.x - 2 == to.x) && state.gmap[from] == King)
			{
				king = false;
				figures.find(from.x * 50, from.y * 50).back().setPos(to.x * 50, to.y * 50);
				if (to.x > 4)
				{
					figures.find((from.x + 4) * 50, from.y * 50).back().setPos((from.x + 1) * 50, from.y * 50);
					rook[1] = false;
					state.gmap[from.x + 1][from.y] = Rook;
					state.omap[from.x + 1][from.y] = state.HamI;
					state.gmap[from.x + 4][from.y] = None;
					state.omap[from.x + 4][from.y] = Neutral;
				}
				else
				{
					figures.find((from.x - 3) * 50, from.y * 50).back().setPos((from.x - 1) * 50, from.y * 50);
					rook[0] = false;
					state.gmap[from.x - 1][from.y] = Rook;
					state.omap[from.x - 1][from.y] = state.HamI;
					state.gmap[from.x - 3][from.y] = None;
					state.omap[from.x - 3][from.y] = Neutral;
				}
			}

			state.gmap[from] = None;
			state.omap[from] = Neutral;


			state.changeP();
			if (state.HamI == Black)
			{
				black.show();
				white.hide();
			}
			else
			{
				white.show();
				black.hide();
			}

			auto ischeck = Ischeck(state);
			auto isstalemate = Isstalemate(state);



			checkmate.hide();
			stalemate.hide();
			check.hide();
			auto p = state.HamI == White ? columnB : columnW;

			if (ischeck)
			{
				if (isstalemate)
				{
					checkmate.show();
					auto label = p.get<Label>(p.size() - 1);
					label << label.text() << '#';
				}
				else
				{
					check.show();
					auto label = p.get<Label>(p.size() - 1);
					label << label.text() << '+';
				}
			}
			else
			{
				if (isstalemate)
					stalemate.show();
			}

			lights.clear();
			tPos.clear();
			figures.update();

			if (isCompUpgradePawn)
			{
				state.changeP();
				Butqueen(to);
				state.changeP();
			}
		}
	}

	void process(Input input)
	{
		auto p = field.mousePos();
		using namespace gamebase::InputKey;
		//auto WmyO = state.HamI == White ? Black : White;

		if (input.justPressed(MouseLeft) && field.isMouseOn())
		{
			g1 = cell(p);
			lights.clear();
			if (cell(p).x * 50 < 400 && cell(p).x * 50 >= 0 && cell(p).y * 50 < 400 && cell(p).y * 50 >= 0)
			{
				if (state.omap[g1] == state.HamI)
					lights.load("Light.json", cell(p).x * 50, cell(p).y * 50);
				movePawn(state, g1, AllAllowed, tPos);

				for (auto v : tPos)
				{
					auto light = lights.load("Light.json", v.To.x * 50, v.To.y * 50);  
					light.skin<Texture>().setColor(0, 75, 255, 100);
				}
			}
			ButRook.hide();
			ButBishop.hide();
			ButKnight.hide();
			ButQueen.hide();
			ButPawn.hide();
		}
		if (input.justPressed(MouseRight))
		{
			g2 = cell(p);
			for (auto v : tPos)
			{
				if (g2 == v.To)       
				{
					makemove(g1, g2);
					//compmove();
					break;
				}
			}
		}
		compmove();
		//	if (Isstalemate) 
	}

	void move()
	{

	}

	State state;
	FromDesign(GameView, field);
	LayerFromDesign(void, figures);
	LayerFromDesign(void, squares);
	LayerFromDesign(void, lights);
	FromDesign(Label, black);
	FromDesign(Label, white);
	FromDesign(Label, check);
	FromDesign(Label, checkmate);
	FromDesign(Label, stalemate);
	FromDesign(Button, ButRook);
	FromDesign(Button, ButBishop);
	FromDesign(Button, ButKnight);
	FromDesign(Button, ButQueen);
	FromDesign(Button, ButPawn);
	FromDesign(Button, ButRest);
	FromDesign(Layout, columnW);
	FromDesign(Layout, columnB);
	IntVec2 g1;
	IntVec2 g2;
	IntVec2 vp;
	IntVec2 ch;
	vector<IntVec2> k;
	vector<Move> tPos;
	int lastmoves;
	int lastfigure;
};

int main(int argc, char** argv)
{
	MyApp app;
	app.setConfig("MyProjConfig.json");
	app.setDesign("Design.json");
	if (!app.init(&argc, argv))
		return 1;
	app.run();
	return 0;
}
