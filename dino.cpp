#include "dino.hpp"
#include "assets.hpp"

using namespace blit;

struct GAME
{
    int state = 2;
    int score = 0;
	int best;
};

struct PLAYER 
{
	short type = 2;
	short ani;
	float y;
	float dx;
	float dy;
};

struct CACTUS
{
	short type = 3;
	float x = 136;
};

struct CLOUD
{
	short type;
	float x;
	short y;
};

GAME game;
PLAYER p;
CACTUS cactus[2];
CACTUS ground[5];
CLOUD cloud[2];
Timer ani_timer;

void ani_update(Timer &t)
{
	p.ani > 0? p.ani = 0: p.ani = 3; 	
}	

void UpdateControl()
{
	if (game.state == 1)
	{
		p.y += p.dy;
		if (p.y <= 0)
		{
			p.y = 0;
			p.dy = 0;
			ani_timer.init(ani_update, 75, -1);
			ani_timer.start();
			game.state = 0;
		}

		p.dy -= .25f;
		if (p.dy < 0)
			p.ani = 3;
	}
	else
	{
		p.dx += .015f;
		if (p.dx > 1.8f)
			p.dx = 1.8f;

		p.type = 0;
		if (buttons & Button::DPAD_UP)
		{
			ani_timer.stop();
			p.ani = 0;
			p.dy = 4;
			game.state = 1;
		}
		else if (buttons & Button::DPAD_DOWN)
			p.type = 3;
	}
}

void NewCactus(short i)
{
	short last = i - 1;
	if (last < 0)
		last = 1;
	
	cactus[i].type = rand() % 9;
	cactus[i].x = cactus[last].x + rand() %160 + 100;
}

void UpdateCactus()
{
	short xsize[9]{-7,-13,-19,-16,-23,-16,-5,-8,-6};
	for (short i=0; i<2; i++)
	{
		cactus[i].x -= p.dx;
		if (cactus[i].x < xsize[cactus[i].type])
			NewCactus(i);
	}
}

void Collision_old(short xs, short ys)
{
	short xsize[9]{5,11,16,14,21,14,6,8,6};
	short ysize[9]{10,10,10,18,18,6,5,6,2};
	for (short i=0; i<2; i++)
	{

		if ((cactus[i].x + 1) < 22 && (cactus[i].x + xsize[cactus[i].type]) > 12)
		{
			if (cactus[i].type > 6)
			{
				if (game.state == 0)
					p.dx = .5;
			}
			else if (p.y < ysize[cactus[i].type]) // game over
			{
				ani_timer.start();
				p.type = 1;
				game.state = 3;
			}
		}
	} 
}

void Collision()
{
	int headx = 20;
	int heady = 76 - p.y;
	int ballyx = 14;
	int ballyy = 86 - p.y;
	if (p.type == 3)
	{
		headx = 31;
		heady = 85;
		ballyx = 17;
	}
 
	int objx[9]{3,7,11,5,11,7,3,3,3};
	int objy[9]{86,89,93,78,85,96,93,93,93};
	int objr[9]{2,7,11,4,11,7,3,3,3};

	for (short i=0; i<2; i++)
	{
		int dx1 = headx - (cactus[i].x + objx[cactus[i].type]);
		int dy1 = heady - objy[cactus[i].type];
		int dx2 = ballyx - (cactus[i].x + objx[cactus[i].type]);
		int dy2 = ballyy - objy[cactus[i].type];
		if (sqrt((dx1 * dx1) + (dy1 * dy1)) < (4 + objr[cactus[i].type]) || sqrt((dx2 * dx2) + (dy2 * dy2)) < (6 + objr[cactus[i].type]))
		{
			if (cactus[i].type > 6)
				p.dx = .5;
			else
			{
				ani_timer.start();
				p.type = 1;
				game.state = 3;
			}
		}
	} 
}

void UpdateGround()
{
	for (short i=0; i<5; i++)
	{
		ground[i].x -= p.dx;
		if (ground[i].x < -7)
		{
			ground[i].type = rand() %16;
			i > 0? ground[i].x = ground[i - 1].x + 25 + rand() %25: ground[i].x = ground[4].x + 25 + rand() %25;
		}
	}
}

void UpdateCloud()
{
	short xout[3]{-8,-16,-32};
	for (short i=0; i<2; i++)
	{
		cloud[i].x -= (p.dx * .1f);
		if (cloud[i].x < xout[cloud[i].type])
		{
			cloud[i].type = rand() %3;
			cloud[i].x = 160 + rand() %32;
			cloud[i].y = 22 + rand() %30;
		}
	}
}

void start()
{
    game.score = 0;

    ani_timer.init(ani_update, 75, -1);
    ani_timer.start();

	p.type = 0;
	p.dx = 0;
	p.y = 0;

	cactus[0].x = 136;
	NewCactus(1);

    game.state = 0;
}

void init() 
{
    set_screen_mode(ScreenMode::lores);

    screen.sprites = Surface::load(sprites);

    channels[0].waveforms = Waveform::SQUARE; 
    channels[0].attack_ms = 5;
    channels[0].decay_ms = 50;
    channels[0].release_ms = 50;
    channels[0].sustain = 0;
	channels[0].volume = 18000;

    channels[1].waveforms = Waveform::SQUARE; 
    channels[1].attack_ms = 5;
    channels[1].decay_ms = 50;
    channels[1].release_ms = 50;
    channels[1].sustain = 0;
	channels[1].volume = 12000;

	ani_timer.init(ani_update, 3000, 1);
	ani_timer.start();

//    if (read_save(game.best) == false)
//        game.best = 0;

	NewCactus(1);

	ground[0].type = rand() %16;
	ground[0].x = rand() %50;
	for (short i=1; i<5; i++)
	{
		ground[i].type = rand() %16;
		ground[i].x = ground[i - 1].x + 25 + rand() %25;
	}

	cloud[0].type = 2;
	cloud[0].x = 16;
	cloud[0].y = 48;
	cloud[1].type = 1;
	cloud[1].x = 110;
	cloud[1].y = 32;
}


// render(time)

void render(uint32_t time) 
{
    screen.alpha = 255;
    screen.mask = nullptr;

//  screen.pen = Pen(0, 0, 0);
//  screen.clear();

//  screen.pen = Pen(170, 120, 120);
//	screen.rectangle(Rect(0, 27, 160, 56));
//  screen.pen = Pen(170, 145, 145);
//	screen.rectangle(Rect(0, 83, 160, 8));
  	screen.stretch_blit(screen.sprites,Rect(120, 0, 8, 12), Rect(0, 0, 160, 120));

	short ctype[3]{0,2,5};
	short cxsize[3]{2,3,4};
	short cysize[3]{1,1,2};
	for (short i=0; i<2; i++)
	{
		if (cloud[i].x < 160)
			screen.sprite(Rect(ctype[cloud[i].type], 10, cxsize[cloud[i].type], cysize[cloud[i].type]), Point(cloud[i].x, cloud[i].y));
	}


	for (short i=0; i<5; i++)
	{
		if (ground[i].x < 160)
			screen.sprite(144 + ground[i].type, Point(ground[i].x, 91));
	}
	
	short type[9]{0,1,3,6,8,11,13,14,15};
	short xsize[9]{1,2,3,2,3,2,1,1,1};
	short ysize[9]{2,2,2,3,3,1,1,1,1};
	for (short i=0; i<2; i++)
	{
		if (cactus[i].x < 160)
			screen.sprite(Rect(type[cactus[i].type], 6, xsize[cactus[i].type], ysize[cactus[i].type]), Point(cactus[i].x, 96 - (ysize[cactus[i].type] * 8)));
	}

	p.type == 3? screen.sprite(Rect(p.type * 3, 1 + p.ani, 4, 2), Point(6, 79 - p.y)) :screen.sprite(Rect(p.type * 3, p.ani, 3, 3), Point(6, 71 - p.y));

    screen.pen = Pen(0, 0, 0);
   	if (game.state == 2) // title
       	screen.text("press a to start", minimal_font, Point(80, 60), true, TextAlign::center_center);
    else if (game.state == 3) // game over
       	screen.text("game over", minimal_font, Point(80, 60), true, TextAlign::center_center);
}

// update(time)

void update(uint32_t time) 
{
    if (game.state < 2) // game
	{
		UpdateControl();
		UpdateCactus();
		UpdateGround();
		UpdateCloud();
		Collision();
	}
    else if (game.state == 2) // title
    {
		if (ani_timer.is_finished())
		{
			p.ani == 0? ani_timer.init(ani_update, 1000 + rand() %2000, 1): ani_timer.init(ani_update, 150, 1);
	    	ani_timer.start();
		}
        if (buttons.released & Button::A)
            start();        
    }
    else if (game.state == 3) // game over
    {
        if (buttons.released & Button::A)
            start();        
    }
}

