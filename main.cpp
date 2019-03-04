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

	int HamI =White;

};

vector<IntVec2> movePawn(State& state, IntVec2 p)
{
	vector<IntVec2> v;
	auto WmyO = state.HamI== White ? Black : White;
	auto isWhite = state.omap[p] == White;
	int dir = isWhite ? 1 : -1;
	auto enemy = isWhite ? Black : White;

	//if (state.omap[p] == White)
	if (state.omap[p] != WmyO)
	{
		if (state.gmap[p] == Pawn)
		{
			int yNext = p.y + dir;

			if (p.y + 1 < 8 && p.x < 8 && p.x >= 0)
			{
				if (state.omap[p.x][yNext] == Neutral)
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
				for( auto n : walkT)
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
				&& state.omap[p.x][p.y + 1 * dir] == Neutral)
			{
				v.emplace_back(p.x, p.y + 2 * dir);

			}
		}

	

	if (state.gmap[p] == Knight)
	{
		if (state.isNE(p.x-1, p.y+2))
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

		if (yN1 < 8 && xN1<8)
		{
			if (state.omap[p.x][yN1] == Neutral)
				v.emplace_back(p.x, yN1);

			if (state.omap[p.x + 1][yN1] == Neutral)
				v.emplace_back(p.x + 1, yN1);

			if (state.omap[p.x - 1][yN1] == Neutral)
				v.emplace_back(p.x - 1, yN1);
		}

		if (yN2 >= 0)
		{
			if (state.omap[p.x][yN2] == Neutral)
				v.emplace_back(p.x, yN2);

			if (state.omap[xN1][yN2] == Neutral)
				v.emplace_back(xN1, yN2);

			if (state.omap[xN2][yN2] == Neutral)
				v.emplace_back(xN2, yN2);
		}

		if (xN1 < 8 && state.omap[p.x + 1][p.y] == Neutral)
			v.emplace_back(p.x + 1, p.y);

		if (xN2>=0 && state.omap[p.x - 1][p.y] == Neutral)
			v.emplace_back(p.x - 1, p.y);

		auto& king = state.HamI == White ? state.kingW : state.kingB;
		auto& rook = state.HamI == White ? state.rookW : state.rookB;
		if (king)
		{
			
			if (state.gmap[p.x - 1][p.y] == None && state.gmap[p.x - 2][p.y] == None 
				&& state.gmap[p.x - 3][p.y] == Rook && rook[0] == true)
			{
				v.emplace_back(p.x - 2, p.y);
			}
		
			
			if (state.gmap[p.x + 1][p.y] == None && state.gmap[p.x + 2][p.y] == None 
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
			if (state.isN(p.x, p.y+ i))
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
	return v;
}										//Сделано движение фигур. Должно работать...



class MyApp : public App
{
	void load()
	{

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
		field.setView(200, 200);
	}
	
	IntVec2 cell(Vec2 v)
	{
		v.x += 25;
		v.y += 25;
		v /=  50;
		return IntVec2(v.x , v.y);
	}

	/*IntVec2 free(IntVec2 v)
	{
		int x, y;
		vector<IntVec2> n;
		IntVec2 p = state.gmap[x][y];
		auto enemy = state.HamI == White ? Black : White;
		for (int x = 0; x < state.gmap.w; ++x)
		{
			for (int y = 0; y < state.gmap.h; ++y)
			{
				if (state.omap[x][y] == enemy)
				{
					n = movePawn(state, p);
					for (auto f : n)
					{
						if(v == f)
							
						
					}
				}
			}
		}
	}*/


	
	void process(Input input)
	{
		auto p = field.mousePos();
		using namespace gamebase::InputKey;
		//auto WmyO = state.HamI == White ? Black : White;

		if (input.justPressed(MouseLeft))
		{
			g1 = cell(p);
			lights.clear();
			if (cell(p).x * 50 < 400 && cell(p).x * 50 >= 0 && cell(p).y * 50 < 400 && cell(p).y * 50 >= 0)
			{
				lights.load("Light.json", cell(p).x * 50, cell(p).y * 50);
				tPos = movePawn(state, g1);
				for (auto v : tPos)
				{
					auto light = lights.load("Light.json", v.x * 50, v.y * 50);
					light.skin<Texture>().setColor(0, 75, 255, 100);

					if (state.gmap[v] == King) 
					{
					auto light = lights.load("Light.json", v.x * 50, v.y * 50);
					light.skin<Texture>().setColor(255, 0, 0, 100);
					}
					
					auto enemy = state.HamI == White ? Black : White;
					for (int x = 0; x < state.gmap.w; ++x)
					{
						for (int y = 0; y < state.gmap.h; ++y)
						{
							if (state.omap[x][y] == enemy && state.gmap[x][y] == King)
							{
								IntVec2 g;
								g.x = x;
								g.y = y;
								auto kPos = movePawn(state, g);
								for (auto n : kPos)
								{
									if (v == n)
									{
										auto light = lights.load("Light.json", v.x * 50, v.y * 50);
										light.skin<Texture>().setColor(84, 0, 122, 100);
									}
								}
							}
						}       //  попытка шаха 
					}
				}
			}
		}
		if (input.justPressed(MouseRight))
		{
			g2 = cell(p);
			for (auto v : tPos)
			{
				if (g2 == v)
				{
					auto isWhite = state.HamI == White;
					auto enemy = isWhite ? Black : White;
					auto& mywalk = isWhite ? state.walkTW : state.walkTB;
					auto& enemywalk = isWhite ? state.walkTB : state.walkTW;
					if (state.gmap[g1] == Pawn)
					{
						int dir = isWhite ? 1 : -1;
						if (g1.y == g2.y - 2*dir)
							mywalk.emplace_back(g2.x, g2.y - dir);

						for (auto n : enemywalk)
						{
							if (n == g2)
							{
								state.gmap[g2.x][g2.y - dir] = None;
								state.omap[g2.x][g2.y - dir] = Neutral;
								for (auto i : figures.find(g2.x * 50, (g2.y-dir) * 50))
									i.kill();
							}

						}
					}

					
					if (state.omap[g2] == enemy)
						for (auto i : figures.find(g2.x * 50, g2.y * 50))
							i.kill();
					enemywalk.clear();

					figures.find(g1.x * 50, g1.y * 50).back().setPos(g2.x * 50, g2.y * 50);
					state.gmap[g2] = state.gmap[g1];
					state.omap[g2] = state.HamI;
					auto& king = state.HamI == White ? state.kingW : state.kingB;
					auto& rook = state.HamI == White ? state.rookW : state.rookB;
					if (state.gmap[g1] == Rook)
					{
						if (g2.x > 4)
						{
							rook[1] = false;
						}
						else
						{
							rook[0] = false;
						}
					}
					if (state.gmap[g1] == King && !(g1.x + 2 == g2.x || g1.x - 2 == g2.x))   //!!!!!!!!!!!!!!!!!!!!!!!!!!
					{
						king = false;
					}
					if ((g1.x + 2 == g2.x || g1.x - 2 == g2.x) && state.gmap[g1] == King)
					{
						king = false;
						figures.find(g1.x * 50, g1.y * 50).back().setPos(g2.x * 50, g2.y * 50);
						if (g2.x > 4)
						{
							figures.find((g1.x + 4) * 50, g1.y * 50).back().setPos((g1.x + 1) * 50, g1.y * 50);
							rook[1] = false;
							state.gmap[g1.x + 1][g1.y] = Rook;
							state.omap[g1.x + 1][g1.y] = state.HamI;
							state.gmap[g1.x + 4][g1.y] = None;
							state.omap[g1.x + 4][g1.y] = Neutral;
						}
						else
						{ 
							figures.find((g1.x - 3) * 50, g1.y * 50).back().setPos((g1.x - 1) * 50, g1.y * 50);
							rook[0] = false;
							state.gmap[g1.x - 1][g1.y] = Rook;
							state.omap[g1.x - 1][g1.y] = state.HamI;
							state.gmap[g1.x - 3][g1.y] = None;
							state.omap[g1.x - 3][g1.y] = Neutral;
						}
					}
						state.gmap[g1] = None;
						state.omap[g1] = Neutral;

					if (state.HamI == White) { state.HamI = Black; }
					else { state.HamI = White; }
					lights.clear();
					tPos.clear();
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
	IntVec2 g1;
	IntVec2 g2;
	IntVec2 vp;
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
