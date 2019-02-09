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
};

vector<IntVec2> movePawn(State& state, IntVec2 p)
{
	vector<IntVec2> v;
	auto isWhite = state.omap[p] == White;
	int dir = isWhite ? 1 : -1;
	auto enemy = isWhite ? Black : White;

	if (state.omap[p] == White)
	{
		int yNext = p.y + dir;

		if (p.y + 1 < 8 && p.x < 8 && p.x >= 0)
		{
			if (state.omap[p.x][yNext] == Neutral)
			{
				v.emplace_back(p.x, yNext);
			}

			if (state.omap[p.x + 1][yNext] == enemy)
			{
				v.emplace_back(p.x + 1, yNext);
			}

			if (state.omap[p.x - 1][yNext] == enemy)
			{
				v.emplace_back(p.x - 1, yNext);
			}
		}

		int d = isWhite ? 1 : 6;
		if (p.y == d && state.omap[p.x][p.y + 2 * dir] == Neutral)
		{
			v.emplace_back(p.x, p.y + 2 * dir);
		}
	}

	return v;
}


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
		state.omap = loadMap("owner.png", colorToType);


		for (int x = 0; x < state.gmap.w; ++x)
		{
			for (int y = 0; y < state.gmap.h; ++y)
			{
				if (state.gmap[x][y] == Pawn)
				{
					auto pawn = figures.load("Pawn.json", 50 * x, 50 * y);
					if(y>5)
						pawn.skin<Texture>().setColor(0, 0, 0);
				}

				if (state.gmap[x][y] == Rook)
				{
					auto rook = figures.load("Rook.json", 50 * x, 50 * y);
					if (y > 5)
						rook.skin<Texture>().setColor(0, 0, 0);

				}

				if (state.gmap[x][y] == Bishop)
				{
					auto bishop = figures.load("Bishop.json", 50 * x, 50 * y);
						if (y > 5)
						bishop.skin<Texture>().setColor(0, 0, 0);
					
				}

				if (state.gmap[x][y] == Knight)
				{
					auto knight = figures.load("Knight.json", 50 * x, 50 * y);
						if (y > 5)
						knight.skin<Texture>().setColor(0, 0, 0);
					
				}

				if (state.gmap[x][y] == King)
				{
					auto king = figures.load("King.json", 50 * x, 50 * y);
					if (y > 5)
						king.skin<Texture>().setColor(0, 0, 0);
					
				}

				if (state.gmap[x][y] == Queen)
				{
					auto queen = figures.load("Queen.json", 50 * x, 50 * y);
						if (y > 5)
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
			lights.clear();
			auto light = lights.load("Light.json", cell(p).x*50, cell(p).y*50);
			g1 = cell(p);
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
