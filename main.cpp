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

struct Compukter
{
	
};

struct Move
{
	IntVec2 From;
	IntVec2 To;
	float q;
};

struct State
{
	GameMap gmap;
	GameMap omap;
	vector<IntVec2> walkTB;
	vector<IntVec2> walkTW;
	bool kingW = true;
	bool kingB = true;
	vector<bool> rookW = { true, true };
	vector<bool> rookB = { true, true };

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
		auto enemy = HamI  == White ? Black : White;
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
		{ HamI = Black; }
		else 
		{ HamI = White; }
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

	int HamI =White;
};

enum MoveType
{
	AllAllowed,
	Hits
};

bool isAttacking(State& state, IntVec2 v);

vector<IntVec2> movePawn(State& state, IntVec2 p, MoveType type)
{
	vector<IntVec2> v;
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

			if (0 <= yNext && yNext < 8 && p.x < 8 && p.x >= 0)
			{
				if (state.omap[p.x][yNext] == Neutral && type != Hits)
				{
					v.emplace_back(p.x, yNext);
				}

				if (p.x != 7 && state.omap[p.x + 1][yNext] == enemy)
				{
					v.emplace_back(p.x + 1, yNext);
				}

				if (p.x != 0 && state.omap[p.x - 1][yNext] == enemy)
				{
					v.emplace_back(p.x - 1, yNext);
				}

				auto& walkT = state.HamI == White ? state.walkTB : state.walkTW;
				for (auto n : walkT)
				{
					if (p.x + 1 == n.x && yNext == n.y)
					{
						v.emplace_back(p.x + 1, yNext);
					}

					if (p.x - 1 == n.x && yNext == n.y)
					{
						v.emplace_back(p.x - 1, yNext);
					}
				}
			}

			int d = isWhite ? 1 : 6;
			if (p.y == d && state.omap[p.x][p.y + 2 * dir] == Neutral
				&& state.omap[p.x][p.y + 1 * dir] == Neutral && type != Hits)
			{
				v.emplace_back(p.x, p.y + 2 * dir);

			}
		}



		if (state.gmap[p] == Knight)
		{
			if (state.isNE(p.x - 1, p.y + 2))
				v.emplace_back(p.x - 1, p.y + 2);

			if (state.isNE(p.x + 1, p.y + 2))
				v.emplace_back(p.x + 1, p.y + 2);

			if (state.isNE(p.x - 1, p.y - 2))
				v.emplace_back(p.x - 1, p.y - 2);

			if (state.isNE(p.x + 1, p.y - 2))
				v.emplace_back(p.x + 1, p.y - 2);

			if (state.isNE(p.x + 2, p.y + 1))
				v.emplace_back(p.x + 2, p.y + 1);

			if (state.isNE(p.x + 2, p.y - 1))
				v.emplace_back(p.x + 2, p.y - 1);

			if (state.isNE(p.x - 2, p.y + 1))
				v.emplace_back(p.x - 2, p.y + 1);

			if (state.isNE(p.x - 2, p.y - 1))
				v.emplace_back(p.x - 2, p.y - 1);
		}

		if (state.gmap[p] == King)
		{
			int yN1 = p.y + 1, yN2 = p.y - 1;
			int xN1 = p.x + 1, xN2 = p.x - 1;

			if (yN1 < 8)
			{
				if (state.omap[p.x][yN1] == Neutral)
					v.emplace_back(p.x, yN1);

				if (xN1 < 8 && state.omap[xN1][yN1] == Neutral)
					v.emplace_back(xN1, yN1);

				if (xN2 >= 0 && state.omap[xN2][yN1] == Neutral)
					v.emplace_back(xN2, yN1);
			}

			if (yN2 >= 0)
			{
				if (state.omap[p.x][yN2] == Neutral)
					v.emplace_back(p.x, yN2);

				if (xN1 < 8 && state.omap[xN1][yN2] == Neutral)
					v.emplace_back(xN1, yN2);

				if (xN2 >= 0 && state.omap[xN2][yN2] == Neutral)
					v.emplace_back(xN2, yN2);
			}

			if (xN1 < 8 && state.omap[p.x + 1][p.y] == Neutral)
				v.emplace_back(p.x + 1, p.y);

			if (xN2 >= 0 && state.omap[p.x - 1][p.y] == Neutral)
				v.emplace_back(p.x - 1, p.y);

			auto& king = state.HamI == White ? state.kingW : state.kingB;
			auto& rook = state.HamI == White ? state.rookW : state.rookB;
			if (king)
			{

				if (p.x - 3 > 0 && state.gmap[p.x - 1][p.y] == None && state.gmap[p.x - 2][p.y] == None
					&& state.gmap[p.x - 3][p.y] == Rook && rook[0] == true)
				{
					v.emplace_back(p.x - 2, p.y);
				}


				if (p.x + 4 < 8 && state.gmap[p.x + 1][p.y] == None && state.gmap[p.x + 2][p.y] == None
					&& state.gmap[p.x + 3][p.y] == None && state.gmap[p.x + 4][p.y] == Rook
					&& rook[1] == true)
				{
					v.emplace_back(p.x + 2, p.y);
				}
			}
		}

		if (state.gmap[p] == Rook || state.gmap[p] == Queen)
		{

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y))
					v.emplace_back(p.x + i, p.y);
				else
					if (state.isE(p.x + i, p.y))
					{
						v.emplace_back(p.x + i, p.y);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y))
					v.emplace_back(p.x - i, p.y);
				else
					if (state.isE(p.x - i, p.y))
					{
						v.emplace_back(p.x - i, p.y);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x, p.y + i))
					v.emplace_back(p.x, p.y + i);
				else
					if (state.isE(p.x, p.y + i))
					{
						v.emplace_back(p.x, p.y + i);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x, p.y - i))
					v.emplace_back(p.x, p.y - i);
				else
					if (state.isE(p.x, p.y - i))
					{
						v.emplace_back(p.x, p.y - i);
						break;
					}
					else break;
			}
		}

		if (state.gmap[p] == Bishop || state.gmap[p] == Queen)
		{
			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y + i))
					v.emplace_back(p.x + i, p.y + i);
				else
					if (state.isE(p.x + i, p.y + i))
					{
						v.emplace_back(p.x + i, p.y + i);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y - i))
					v.emplace_back(p.x - i, p.y - i);
				else
					if (state.isE(p.x - i, p.y - i))
					{
						v.emplace_back(p.x - i, p.y - i);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x - i, p.y + i))
					v.emplace_back(p.x - i, p.y + i);
				else
					if (state.isE(p.x - i, p.y + i))
					{
						v.emplace_back(p.x - i, p.y + i);
						break;
					}
					else break;
			}

			for (int i = 1; i < 8; i++)
			{
				if (state.isN(p.x + i, p.y - i))
					v.emplace_back(p.x + i, p.y - i);
				else
					if (state.isE(p.x + i, p.y - i))
					{
						v.emplace_back(p.x + i, p.y - i);
						break;
					}
					else break;
			}
		}
	}

	if (type == AllAllowed)
	{
		vector<IntVec2> v2;
		IntVec2 king = state.findKing(state.HamI);

		state.changeP();

		for (auto p2 : v)
		{
			auto Ifking = p == king ? p2 : king;

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
		state.changeP();
		return v2;
	}
	return v;
}

bool isAttacking(State& state, IntVec2 v)
{
	int x, y;
	vector<IntVec2> n;
	for (int x = 0; x < state.gmap.w; ++x)
	{
		for (int y = 0; y < state.gmap.h; ++y)
		{
			if (state.omap[x][y] == state.HamI)
			{
				n = movePawn(state, IntVec2(x, y), Hits);
				for (auto f : n)
				{
					if (v == f)
					{
						return true;
					}
				}
			}
		}
	}
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
	int canMove = 0;
	vector<IntVec2> c;

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y <= 7; ++y)
		{
			if (state.omap[x][y] == state.HamI)
			{
				c = movePawn(state, IntVec2(x, y), AllAllowed);
				if (c.size() > 0)
					canMove++;
			}
		}
	}
	return canMove == 0;
}

class MyApp : public App
{
	void load()
	{
		connect(ButRest, Rest);
		ButRest.setPos(0,-270);
		Rest();	
	}
	
	void Rest()
	{
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
	}

	IntVec2 cell(Vec2 v)
	{
		v.x += 25;
		v.y += 25;
		v /=  50;
		return IntVec2(v.x , v.y);
	}

	void Butrook(IntVec2 p)
	{
		state.gmap[p] = Rook;
		auto rook = figures.load("Rook.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			rook.skin<Texture>().setColor(0, 0, 0);
	}
	void Butbishop(IntVec2 p)
	{
		state.gmap[p] = Bishop;
		auto bishop = figures.load("Bishop.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			bishop.skin<Texture>().setColor(0, 0, 0);
	}
	void Butknight(IntVec2 p)
	{
		state.gmap[p] = Knight;
		auto knight = figures.load("Knight.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			knight.skin<Texture>().setColor(0, 0, 0);
	}
	void Butqueen(IntVec2 p)
	{
		state.gmap[p] = Queen;
		auto queen = figures.load("Queen.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			queen.skin<Texture>().setColor(0, 0, 0);
	}
	void Butpawn(IntVec2 p)
	{
		state.gmap[p] = Pawn;
		auto pawn = figures.load("Pawn.json", 50 * p.x, 50 * p.y);
		if (state.omap[p] == Black)
			pawn.skin<Texture>().setColor(0, 0, 0);
	}
	
	vector<Move> computermoves()
	{
		vector<IntVec2> c;
		vector<Move> result;
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (state.omap[x][y] == state.HamI)
				{
					c = movePawn(state, IntVec2(x, y), AllAllowed);
					for (auto i : c)
					{
						Move m;
						m.From = IntVec2(x, y);
						m.To = i;
						m.q = analyze(m);
						result.push_back(m);
					}
				}
			}
		}
		return result;	
	}

	float analyze(Move m)
	{
		float step=0;
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
		state.gmap[m.To] = state.gmap[m.From];
		state.omap[m.To] = state.omap[m.From];
		state.gmap[m.From] = None;
		state.omap[m.From] = Neutral;
		state.changeP();
		if (Ischeck(state))
			step += 1000;

		if(Ischeck(state) && ! Isstalemate(state))
			step += 1000000;
		state.changeP();
		state.gmap[m.From] = state.gmap[m.To];
		state.omap[m.From] = state.omap[m.To];
		state.gmap[m.To] = prevFigure;
		state.omap[m.To] = prevOwner;
		return step + randomFloat(-0.001, 0.001);
	}

	void compmove()
	{
		auto moves = computermoves();
		float q = 0;
		Move m;
		for (auto i : moves)
		{
			if (i.q > q)
			{
				q = i.q; 
				m = i;
			}
		}
		if (q == 0)
			return;

		makemove(m.From, m.To);
	}

	/*vector<IntVec2> moves(vector<IntVec2> c)
	{
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
					}
				}
			}

			auto prevFigure = state.gmap[i];
			auto prevOwner = state.omap[i];
			state.gmap[i] = state.gmap[];
			state.omap[i] = state.omap[];
			state.gmap[] = None;
			state.omap[] = Neutral;

				//v2.push_back(i);

			state.gmap[] = state.gmap[i];
			state.omap[] = state.omap[i];
			state.gmap[i] = prevFigure;
			state.omap[i] = prevOwner;
		}
		state.changeP();
	}*/

	void makemove(IntVec2 from, IntVec2 to)
	{
		auto isWhite = state.HamI == White;
		auto enemy = isWhite ? Black : White;
		auto& mywalk = isWhite ? state.walkTW : state.walkTB;
		auto& enemywalk = isWhite ? state.walkTB : state.walkTW;
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

		}


		if (state.omap[to] == enemy)
			for (auto i : figures.find(to.x * 50, to.y * 50))
				i.kill();
		enemywalk.clear();

		figures.find(from.x * 50, from.y * 50).back().setPos(to.x * 50, to.y * 50);
		state.gmap[to] = state.gmap[from];
		state.omap[to] = state.HamI;
		auto& king = state.HamI == White ? state.kingW : state.kingB;
		auto& rook = state.HamI == White ? state.rookW : state.rookB;
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


		if (ischeck)
		{
			if (isstalemate)
			{
				checkmate.show();
			}
			else
			{
				check.show();
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
				tPos = movePawn(state, g1, AllAllowed);

				for (auto v : tPos)
				{
					auto light = lights.load("Light.json", v.x * 50, v.y * 50);
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
				if (g2 == v)
				{
					makemove(g1, g2);
					compmove();
				}
			}
		}
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
	IntVec2 g1;
	IntVec2 g2;
	IntVec2 vp;
	IntVec2 ch;
	vector<IntVec2> tPos;
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
