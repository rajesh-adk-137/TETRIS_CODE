
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
#include<iostream>

int block[7][4][4] =
{
	1,1,1,1,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,

	1,0,0,0,
	1,1,0,0,
	0,1,0,0,
	0,0,0,0,

	0,1,0,0,
	1,1,0,0,
	1,0,0,0,
	0,0,0,0,

	1,1,0,0,
	1,1,0,0,
	0,0,0,0,
	0,0,0,0,

	1,0,0,0,
	1,1,0,0,
	1,0,0,0,
	0,0,0,0,

	1,0,0,0,
	1,0,0,0,
	1,1,0,0,
	0,0,0,0,

	0,1,0,0,
	0,1,0,0,
	1,1,0,0,
	0,0,0,0,
};
int block_board[7][4][4]; //above block[][][] shouldn't be changed thus its value is given to block_board[][][] each time new block is called. 

const int cell_size = 40;
const int w_cnt = 10;
const int h_cnt = 20;
int world[h_cnt][w_cnt] = { 0 };
//long int world_temp[h_cnt][w_cnt] = { 0 };


int main(void)
{
	RenderWindow window(VideoMode((w_cnt + 8) * cell_size, h_cnt * cell_size), "Tetris", sf::Style::Close);

	RectangleShape grid(Vector2f(cell_size - 1.5, cell_size - 1.5));
	RectangleShape dead_block(Vector2f(cell_size - 2, cell_size - 2));
	sf::Texture t_d_block;
	t_d_block.loadFromFile("texture/dead_block.jpg");
	dead_block.setTexture(&t_d_block);
	sf::Vector2u d_Size = t_d_block.getSize();

	RectangleShape cell(Vector2f(cell_size, cell_size));
	sf::Texture cellTexture;
	cellTexture.loadFromFile("texture/tiles.png");
	cell.setTexture(&cellTexture);
	sf::Vector2u textureSize = cellTexture.getSize();
	textureSize.x /= 8;
	textureSize.y /= 1;

	RectangleShape shadow(Vector2f(cell_size - 2, cell_size - 2));

	//RectangleShape background(Vector2f(w_cnt * cell_size, h_cnt * cell_size));

	sf::RectangleShape preview_border(sf::Vector2f(320, h_cnt * cell_size));
	preview_border.setFillColor(sf::Color(100, 100, 100, 150));
	preview_border.setOutlineThickness(1);
	preview_border.setPosition(w_cnt * cell_size, 0);

	sf::RectangleShape preview_border1(sf::Vector2f(cell_size * 4, cell_size * 4));
	preview_border1.setFillColor(sf::Color(25, 0, 0, 255));
	preview_border1.setOutlineThickness(-1);
	preview_border1.setPosition((w_cnt + 2) * cell_size, cell_size * 2);

	//for preview block in the side window
	RectangleShape p_block(Vector2f(cell_size, cell_size));
	sf::Texture p_Texture;
	p_Texture.loadFromFile("texture/tiles.png");
	p_block.setTexture(&p_Texture);
	sf::Vector2u p_Size = p_Texture.getSize();
	p_Size.x /= 8;
	p_Size.y /= 1;

	int next = rand() % 7;//to initialize with a random block on new block as (kind=next in new block function)
	int px;//preview
	int py;//preview
	int kind;
	int cx;
	int cy;
	int sx;//not needed we can use cx instead.. but for uniformity it is used.
	int sy = 0;

	Clock clock;

	auto game_over = [&]()
	{
		for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
			if (block_board[kind][y][x])
				if (world[y][x + 4])
					return true;
		return false;
	};

	auto new_block = [&]()
	{
		kind = next, cx = 4, cy = 0;
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				block_board[kind][y][x] = block[kind][y][x];
		if (kind == 0)
			cx--;
	};

	new_block();
	auto preview_block = [&]()
	{
		next = rand() % 7;
		px = w_cnt + 2 + 1;
		py = 2 + 1;
		if (next == 0)//for the long block to get it to center
		{
			px--;
		}
	};
	preview_block();


	auto check_block = [&]()
	{
		for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
		{
			if (block_board[kind][y][x] == 0) continue;
			if (x + cx < 0 || x + cx >= w_cnt || y + cy >= h_cnt) return false; // hit wall
			if (world[cy + y][cx + x]) return false;//hit another block
		}
		return true;
	};

	auto clear_lines = [&]()
	{
		/*float timer = 0, delay = 0.3;
		float time = clock1.getElapsedTime().asSeconds();
		clock1.restart();
		timer += time;*/
		int to = h_cnt - 1;
		//from bottom line to top line...
		for (int from = h_cnt - 1; from >= 0; from--)
		{
			int cnt = 0;
			for (int x = 0; x < w_cnt; x++)
				if (world[from][x])cnt++;

			/*if (cnt == w_cnt)
			{
				for (int x = 0; x < w_cnt; x++)
					world[to][x] = 0;
			}*/
			if (cnt < w_cnt)
			{

				//timer = 0;
				for (int x = 0; x < w_cnt; x++)
					world[to][x] = world[from][x];

				to--;

			}



		}
	};


	auto go_down = [&]()
	{
		cy++;
		if (check_block() == false)
		{
			cy--;
			for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
				if (block_board[kind][y][x])
				{
					world[cy + y][cx + x] = kind + 1;

				}

			clear_lines();
			if (!game_over())
				new_block();
			preview_block();

			return false;
		}
	};
	auto rotate = [&]()
	{
		int len = 0; //to check rotation block size
		int temp[4][4];// check rotation block size
		int d[4][4] = { 0 };
		for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
		{
			temp[y][x] = block_board[kind][y][x];
			if (block_board[kind][y][x])
				len = max(max(x, y) + 1, len);
		}

		//rotate conter-clock wise 90 degree
		for (int y = 0; y < len; y++)for (int x = 0; x < len; x++)
			if (block_board[kind][y][x])
				d[len - 1 - x][y] = 1;
		for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
			block_board[kind][y][x] = d[y][x];
		if (!check_block())
			for (int y = 0; y < 4; y++)for (int x = 0; x < 4; x++)
				block_board[kind][y][x] = temp[y][x];



	};
	/* char b[6] = "56";
	Font font;
	font.loadFromFile("fonts/Montague.ttf");
	Text text;
	text.setFont(font);
	text.setCharacterSize(50);
	text.setStyle(Text::Bold);
	text.setFillColor(sf::Color(0, 0, 0, 255));
	text.setPosition(400.f, window.getSize().y / 2);
	text.setString("score" +b[1]);
	  //I WAS TRYING TO ADD TEXT FOR SCORE BUT LEFT IT...*/


	while (window.isOpen())
	{
		static float prev = clock.getElapsedTime().asSeconds();
		if (clock.getElapsedTime().asSeconds() - prev >= 0.5)
		{
			prev = clock.getElapsedTime().asSeconds();
			go_down();
		}

		sf::Event e;
		while (window.pollEvent(e))
		{

			if (e.type == Event::Closed)
				window.close();


			if (e.type == Event::KeyPressed)
			{
				if (e.key.code == Keyboard::Left)
				{
					cx--;
					if (check_block() == false) cx++;
				}
				else if (e.key.code == Keyboard::Right)
				{
					cx++;
					if (check_block() == false) cx--;
				}
				else if (e.key.code == Keyboard::Down)
				{
					go_down();
				}
				else if (e.key.code == Keyboard::Space)
				{
					while (go_down() == true);
				}
				else if (e.key.code == Keyboard::Up)
					rotate();
				else if (e.key.code == Keyboard::N)//new game shortcut for now
				{

					//if (game_over()
					for (int y = 0; y < h_cnt; y++)for (int x = 0; x < w_cnt; x++)
						world[y][x] = 0;
					new_block();
					preview_block();
				}
			}
		}

		window.clear();

		for (int a = 0; a < w_cnt; a++)for (int b = 0; b < h_cnt; b++)
		{
			grid.setPosition(cell_size * static_cast<float> (a), cell_size * static_cast<float> (b));
			grid.setFillColor(sf::Color(50, 50, 100, 250));
			window.draw(grid);
		}


		window.draw(preview_border);
		window.draw(preview_border1);




		auto shadow_check = [&]()
		{
			int ch = 0;
			sx = cx;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (block_board[kind][y][x])
					{
						if (y + sy >= h_cnt)
						{
							sy--;
							ch = 1;
						}
						if (world[sy + y][cx + x])
						{
							sy--;
							ch = 1;
						}


					}



				}
			if (ch == 1)
				return false;
			else
				return true;

		};

		auto draw_shadow = [&]()
		{


			shadow.setFillColor(Color(100, 100, 100, 255));
			while (shadow_check())
				sy++;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)

					if (block_board[kind][y][x])
					{
						shadow.setPosition(Vector2f((sx + x) * cell_size, (sy + y) * cell_size));
						window.draw(shadow);

					}

			sy = cy;//this needs to be explained by me...ask me!!

		};
		if (!game_over())
			draw_shadow();



		auto draw_world = [&]()
		{
			for (int y = 0; y < h_cnt; y++)for (int x = 0; x < w_cnt; x++)
				if (world[y][x])
				{
					// cell.setFillColor(color_map[world[y][x]-1]);
					cell.setTextureRect(sf::IntRect(textureSize.x * (world[y][x] - 1), textureSize.y * 0, textureSize.x, textureSize.y));
					cell.setPosition(Vector2f(x * cell_size, y * cell_size));
					window.draw(cell);
				}
		};
		if (!game_over())
			draw_world();


		auto draw_block = [&]()
		{
			cell.setTextureRect(sf::IntRect(textureSize.x * kind, textureSize.y * 0, textureSize.x, textureSize.y));

			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{

					if (block_board[kind][y][x])
					{
						cell.setPosition(Vector2f((cx + x) * cell_size, (cy + y) * cell_size));
						window.draw(cell);
					}
				}
		};

		if (!game_over())
			draw_block();


		auto draw_Pblock = [&]()
		{
			p_block.setTextureRect(sf::IntRect(p_Size.x * next, p_Size.y * 0, p_Size.x, p_Size.y));
			//cell.setFillColor(color_map[kind]);
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (block[next][y][x])
					{
						p_block.setPosition(Vector2f((px + x) * (cell_size), (py + y) * (cell_size)));
						window.draw(p_block);
					}
				}
		};
		if (!game_over())
			draw_Pblock();
		if (game_over())
		{
			for (int a = 0; a < w_cnt; a++)for (int b = 0; b < h_cnt; b++)
			{
				if (world[b][a])
				{
					dead_block.setTextureRect(sf::IntRect(0, 0, d_Size.x, d_Size.y));
					dead_block.setPosition(cell_size * static_cast<float>(a), cell_size * static_cast<float>(b));

					window.draw(dead_block);
					//world[b][a] = 9;
				}
			}
		}
		// window.draw(text);
		window.display();
	}

}

