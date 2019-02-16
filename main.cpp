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
					if (p.x+1 < 8 && p.x + 1 == n.x && yNext == n.y)
					{
						v.emplace_back(p.x + 1, yNext);
					}

					if (p.x-1 >=0 && p.x - 1 == n.x && yNext == n.y)
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

		if (yN1 < 8)
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

			if (state.omap[p.x + 1][yN2] == Neutral)
				v.emplace_back(p.x + 1, yN2);

			if (state.omap[p.x - 1][yN2] == Neutral)
				v.emplace_back(p.x - 1, yN2);
		}

		if (state.omap[p.x + 1][p.y] == Neutral)
			v.emplace_back(p.x + 1, p.y);

		if (state.omap[p.x - 1][p.y] == Neutral)
			v.emplace_back(p.x - 1, p.y);
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
}										//������� �������� �����. ������ ��������...



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

	void process(Input input)
	{
		auto p = field.mousePos();
		using namespace gamebase::InputKey;

		if (input.justPressed(MouseLeft))
		{
			g1 = cell(p);
			lights.clear();
			lights.load("Light.json", cell(p).x * 50, cell(p).y * 50);
			tPos = movePawn(state, g1);
			for (auto v : tPos)
			{
				auto light = lights.load("Light.json", v.x * 50, v.y * 50);
				light.skin<Texture>().setColor(0, 75, 255, 100);
			}
		}
		if (input.justPressed(MouseRight))
		{
			g2 = cell(p);
			for (auto v : tPos)
			{
				if (g2 == v)
				{
					if (state.HamI == White)
					{
						if (state.omap[g1] == Pawn && g2.x-1>=0 && g2.x +1<8 && g1.y == g2.y - 2 && 
							(state.omap[g2.x + 1][g2.y] == Black || state.omap[g2.x - 1][g2.y] == Black))
							state.walkT.emplace_back(g2.x, g2.y - 1);

						if (state.omap[g2] == Black)
							for (auto i : figures.find(g2.x * 50, g2.y * 50))
								i.kill();

						for (auto n : state.walkT)
						{
							if (n == g2)
							{
								state.gmap[g2.x][g2.y - 1] = None;
								state.omap[g2.x][g2.y - 1] = Neutral;
								state.walkT.clear();
							}
							

						}

						state.gmap[g2] = None;
						state.omap[g2] = Neutral;
					}
					else
					{

						if (state.omap[g1] == Pawn && g2.x - 1 >= 0 && g2.x + 1 < 8 && g1.y == g2.y + 2 && (state.omap[g2.x + 1][g2.y] == White ||
							state.omap[g2.x - 1][g2.y] == White))
							state.walkT.emplace_back(g2.x, g2.y + 1); 

							if (state.omap[g2] == White)
								for (auto i : figures.find(g2.x * 50, g2.y * 50))
									i.kill();

						for (auto n : state.walkT)
						{
							if (n == g2)
							{
								state.gmap[g2.x][g2.y + 1] = None;
								state.omap[g2.x][g2.y + 1] = Neutral;
								state.walkT.clear();
							}
							
						}
							state.gmap[g2] = None;
							state.omap[g2] = Neutral;
					}
						figures.find(g1.x * 50, g1.y * 50).back().setPos(g2.x * 50, g2.y * 50);

						
						state.gmap[g2] = state.gmap[g1];
						state.gmap[g1] = None;
						state.omap[g2] = state.HamI;
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
