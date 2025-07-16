#include<graphics.h>
#include<time.h>
#include<math.h>
#include<stdio.h>
#pragma comment(lib, "MSIMG32.LIB")						// 链接器在链接过程中包含指定的库文件
#pragma comment(lib, "winmm.lib")	// 加载多媒体静态库

//背景图一小格像素点为31*31
//左上角第一个点坐标为59，86
//人物43*82
//背景下底y坐标753，左边界x坐标37，右边界762

void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}
DWORD startTime = 0;
DWORD totalMS;   // 总毫秒数
bool l[800][800] = { 0 };
IMAGE current0, imgdoor[7];
bool paused = 0;
bool start = 0;
class Current
{
private:
	int x = 0, y = 0;
	IMAGE img;
public:
	bool get = 0;
	Current() {}
	void paint(int a, int b)
	{
		putimage_alpha(a - 40, b - 40, &img);
		x = a;
		y = b;
	}
	void load(LPCTSTR a)
	{
		loadimage(&img, a, 80, 80, false);
	}
	void dis()
	{
		loadimage(&img, _T("image\\empty.png"), 80, 80, false);
	}
	int getx()
	{
		return x;
	}
	int gety()
	{
		return y;
	}
};
class Brick
{
	IMAGE img;
public:
	Brick()
	{
		loadimage(&img, _T("image\\brick.png"), 31, 31, false);
	}
	void paint(int x, int y)
	{
		putimage_alpha(x, y, &img);
		for (int i = x; i <= x + 31; i++)
		{
			l[i][y] = true;
		}
	}
};
IMAGE background, move, moveY, jump, jumpY, over;
ExMessage msg;
int imageNum = 0;
int tick = 0, timer = 0, imgIndex = 0;
int doorx, doory;
int a[6];
int b[5];
Brick brick;
int jumpnumber = 0;
Current current[6][25];
typedef struct Person
{
	int x;						// 小球圆心坐标x
	int y;						// 小球圆心坐标y
	int width;
	int hight;
	int dx;						// 小球在x轴方向移动的增量
	int dy;						// 小球在y轴方向移动的增量
	double G;
	bool isFly = false;
	bool isMoveUp = false;		// 小球是否向三个方向移动
	bool isMoveLeft = false;
	bool isMoveRight = false;
	int sum = 0;                    // 总金币数
} Person;
Person person{ 57,671,43,82,7,80,0 };

void currentload()
{
	for (int i = 0; i < 6; i++)
	{
		current[i][0].load(_T("image\\current\\current.0.png"));
		current[i][1].load(_T("image\\current\\current.1.png"));
		current[i][2].load(_T("image\\current\\current.2.png"));
		current[i][3].load(_T("image\\current\\current.3.png"));
		current[i][4].load(_T("image\\current\\current.4.png"));
		current[i][5].load(_T("image\\current\\current.5.png"));
		current[i][6].load(_T("image\\current\\current.6.png"));
		current[i][7].load(_T("image\\current\\current.7.png"));
		current[i][8].load(_T("image\\current\\current.8.png"));
		current[i][9].load(_T("image\\current\\current.9.png"));
		current[i][10].load(_T("image\\current\\current.10.png"));
		current[i][11].load(_T("image\\current\\current.11.png"));
		current[i][12].load(_T("image\\current\\current.12.png"));
		current[i][13].load(_T("image\\current\\current.13.png"));
		current[i][14].load(_T("image\\current\\current.14.png"));
		current[i][15].load(_T("image\\current\\current.15.png"));
		current[i][16].load(_T("image\\current\\current.16.png"));
		current[i][17].load(_T("image\\current\\current.17.png"));
		current[i][18].load(_T("image\\current\\current.18.png"));
		current[i][19].load(_T("image\\current\\current.19.png"));
		current[i][20].load(_T("image\\current\\current.20.png"));
		current[i][21].load(_T("image\\current\\current.21.png"));
		current[i][22].load(_T("image\\current\\current.22.png"));
		current[i][23].load(_T("image\\current\\current.23.png"));
		current[i][24].load(_T("image\\current\\current.24.png"));
	}
}
bool check()
{
	for (int i = person.y + person.hight / 2; i <= person.y + person.hight + 1; i++)
	{
		for (int j = person.x; j < person.x + person.width; j++)
		{
			if (l[j][i + 1] == 1)
			{
				person.y = i - 1 - person.hight;

				return true;
			}
		}
	}
	return false;
}
bool endcheck()
{
	if (person.sum == 6)
	{
		if ((doorx + 72 - person.x - 21) * (doorx + 72 - person.x - 21) + (doory + 128 - person.y - 41) * (doory + 128 - person.y - 41) <= 70 * 70)
		{
			return true;
		}
	}
	return false;
}
void Move()
{
	if (!endcheck())
	{
		bool keyjumppressed = false;
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN && !paused)			// 按下按键处理
			{
				switch (msg.vkcode)
				{
				case 'w':
				case 'W':
				case VK_UP:
				case VK_SPACE:

					keyjumppressed = true;
					break;

				case 'a':
				case 'A':
				case VK_LEFT:

					person.isMoveLeft = true;
					break;

				case 'd':
				case 'D':
				case VK_RIGHT:

					person.isMoveRight = true;
					break;
				}
			}
			if (msg.message == WM_KEYDOWN && msg.vkcode == 0x1B)
				paused = !paused;
			if (msg.message == WM_KEYUP && !paused)			// 松开按键处理
			{
				switch (msg.vkcode)
				{
				case 'a':
				case 'A':
				case VK_LEFT:
					person.isMoveLeft = false;
					break;

				case 'd':
				case 'D':
				case VK_RIGHT:
					person.isMoveRight = false;
					break;
				}
			}
		}
		if (!paused)
		{
			if (keyjumppressed == 1 && jumpnumber == 0)
			{
				person.y -= person.dy;
				jumpnumber++;
			}
			else if (jumpnumber == 1 && keyjumppressed) {
				person.y = person.y - person.dy;
				jumpnumber++;
			}
			if (keyjumppressed && jumpnumber < 2)
			{
				person.G = 0;
			}
			if (person.isMoveLeft)
			{
				person.x -= (int)person.dx;		//小球坐标 = 方向增速 * 方向的标准化分量
			}
			if (person.isMoveRight)
			{
				person.x += (int)person.dx;		//小球坐标 = 方向增速 * 方向的标准化分量
			}

			// 如果着地
			if (check())
			{
				person.isMoveUp = false;
				person.isFly = false;
				jumpnumber = 0;
				person.G = 0;
			}
			else
			{
				person.y += (int)person.G;
				person.G += 0.4;
				person.isMoveUp = true;
				person.isFly = true;
				check();
			}
		}


		// 边缘检测
		if (person.y - person.hight / 2 <= 0)					// 上
			person.y = person.hight / 2;
		if (person.x <= 40)					// 左
			person.x = 40;
		if (person.x + person.width >= 762)			// 右
			person.x = 762 - person.width - 1;

	}
}
void UpdateAnimation() {
	static DWORD lastTime = 0;
	DWORD now = GetTickCount();
	if (now - lastTime > 100) {  // 每100ms切换一帧
		imageNum = (imageNum + 1) % 5;
		lastTime = now;
	}
}
void currentset()
{
	srand(time(NULL));
	for (int i = 0; i < 6; i++)
	{
		a[i] = rand() % 600 + 100;
	}
}
void brickset()
{
	srand(time(NULL));
	for (int i = 0; i < 5; i++)
	{
		b[i] = rand() % 16;
	}
}
void doorset()
{
	srand(time(NULL));
	doorx = rand() % 580 + 37;
	doory = rand() % 400 + 97;
}
//结束检测

void Time() {
	if (startTime == 0) {
		startTime = GetTickCount();
	}
	DWORD now = GetTickCount();
	if (!endcheck() && !paused)
	{
		totalMS = now - startTime;
	}
	int totals = totalMS / 1000;
	int minute = totals / 60;
	int second = totals % 60;

	TCHAR timeText[32];
	wsprintf(timeText, _T("%02d:%02d"), minute, second);  // 格式：MM:SS

	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	settextstyle(45, 0, _T("Arial"));  // 字号 30，无粗体斜体
	outtextxy(560, 18, timeText);       // 屏幕右上角显示时间
}
// 渲染画面
void Render() {
	if (!endcheck())
	{
		BeginBatchDraw();
		cleardevice();
		loadimage(NULL, _T("image\\background.jpg"), 800, 800, false);
		for (int i = 0; i < 5; i++)
		{
			for (int j = b[i]; j < b[i] + 7; j++)
				brick.paint(43 + j * 31, 153 + i * 120);
		}
		for (int i = 0; i < 6; i++)
		{
			current[i][tick / 5 % 25].paint(a[i], 123 + i * 120);
		}
		tick++;
		for (int i = 0; i < 6; i++)
		{
			if ((person.x + 21 - current[i][0].getx()) * (person.x + 21 - current[i][0].getx())
				+ (person.y + 41 - current[i][0].gety()) * (person.y + 41 - current[i][0].gety()) < 45 * 45)
			{
				for (int j = 0; j < 25; j++)
				{
					current[i][j].load(_T("image//empty.png"));
					mciSendString(_T("open music\\4.mp3 alias BGM"), 0, 0, 0);			// 打开音乐文件，alias指定别名
					mciSendString(_T("play BGM "), 0, 0, 0);

				}
				if (current[i][0].get == 0)
				{
					person.sum++;
					mciSendString(_T("open music\\4.mp3 alias BGM"), 0, 0, 0);			// 打开音乐文件，alias指定别名
					mciSendString(_T("play BGM "), 0, 0, 0);

				}
				current[i][0].get = 1;
				break;
			}
		}
		if (person.sum == 6) {
			timer += 5;
			if (timer > 15)					//定时器超过预定的时间间隔时切换下一张图片
			{
				imgIndex = (imgIndex + 1) % 7;			//循环切换图片
				timer = 0;										//重置计时器
			}
			putimage_alpha(doorx, doory, &imgdoor[imgIndex]);
		}
		putimage_alpha(650, 0, &current0);
		// 设置文字背景模式为透明
		setbkmode(TRANSPARENT);
		// 设置字体格式
		settextstyle(48, 0, _T("微软雅黑"));
		// 设置文字颜色
		settextcolor(WHITE);
		// 输出带有透明背景的文字
		outtextxy(725, 15, (char)(person.sum + 48));
		Time();


		// 根据方向绘制动画
		if (person.isFly) {

			if (person.isMoveLeft) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
			else if (person.isMoveRight) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
			else if (person.isFly) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
		}
		else if (person.isMoveLeft) {
			putimage(person.x, person.y, 43, 82, &move, 43 * imageNum, 82, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 43 * imageNum, 82, SRCPAINT);
		}
		else if (person.isMoveRight) {
			putimage(person.x, person.y, 43, 82, &move, 41 * imageNum, 0, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 41 * imageNum, 0, SRCPAINT);
		}
		else if (person.isFly) {
			putimage(person.x, person.y, &jump, SRCAND);
			putimage(person.x, person.y, &jumpY, SRCPAINT);
		}
		else if (check() == false)
		{
			putimage(person.x, person.y, &jump, SRCAND);
			putimage(person.x, person.y, &jumpY, SRCPAINT);
		}
		else {
			// 默认站立姿势
			putimage(person.x, person.y, 43, 82, &move, 0, 0, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 0, 0, SRCPAINT);
		}
		if (paused)
		{
			setbkmode(TRANSPARENT);
			settextcolor(BLACK);
			settextstyle(50, 20, _T("宋体"));
			setfillcolor(YELLOW);
			solidroundrect(320, 375, 480, 425, 5, 5);
			outtextxy(400 - 60, 400 - 25, _T("paused"));
			person.isMoveLeft = false;
			person.isMoveRight = false;
		}

		FlushBatchDraw();
	}
	else
	{
		BeginBatchDraw();
		cleardevice();
		loadimage(NULL, _T("image\\background.jpg"), 800, 800, false);
		for (int i = 0; i < 5; i++)
		{
			for (int j = b[i]; j < b[i] + 7; j++)
				brick.paint(43 + j * 31, 153 + i * 120);
		}
		for (int i = 0; i < 6; i++)
		{
			current[i][tick / 5 % 25].paint(a[i], 123 + i * 120);
		}
		putimage_alpha(650, 0, &current0);
		// 设置文字背景模式为透明
		setbkmode(TRANSPARENT);
		// 设置字体格式
		settextstyle(48, 0, _T("微软雅黑"));
		// 设置文字颜色
		settextcolor(WHITE);
		// 输出带有透明背景的文字
		outtextxy(725, 15, (char)(person.sum + 48));
		Time();


		// 根据方向绘制动画
		if (person.isFly) {

			if (person.isMoveLeft) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
			else if (person.isMoveRight) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
			else if (person.isFly) {
				putimage(person.x, person.y, &jump, SRCAND);
				putimage(person.x, person.y, &jumpY, SRCPAINT);
			}
		}
		else if (person.isMoveLeft) {
			putimage(person.x, person.y, 43, 82, &move, 43 * imageNum, 82, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 43 * imageNum, 82, SRCPAINT);
		}
		else if (person.isMoveRight) {
			putimage(person.x, person.y, 43, 82, &move, 41 * imageNum, 0, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 41 * imageNum, 0, SRCPAINT);
		}
		else if (person.isFly) {
			putimage(person.x, person.y, &jump, SRCAND);
			putimage(person.x, person.y, &jumpY, SRCPAINT);
		}
		else if (check() == false)
		{
			putimage(person.x, person.y, &jump, SRCAND);
			putimage(person.x, person.y, &jumpY, SRCPAINT);
		}
		else {
			// 默认站立姿势
			putimage(person.x, person.y, 43, 82, &move, 0, 0, SRCAND);
			putimage(person.x, person.y, 43, 82, &moveY, 0, 0, SRCPAINT);
		}
		LOGFONT f;									// 定义LOGFONT结构体变量
		gettextstyle(&f);							// 获取默认字体设置
		f.lfHeight = 50;							// 设置文本高度
		f.lfWidth = 20;								// 设置文本宽度
		_tcscpy_s(f.lfFaceName, _T("黑体"));	       // 设置字体为宋体
		f.lfQuality = ANTIALIASED_QUALITY;			// 设置输出效果为抗锯齿  
		settextstyle(&f);							// 设置字体样式	
		setfillcolor(BLACK);
		solidroundrect(150, 350, 650, 450, 20, 20);
		putimage_alpha(150, 250, &over);
		settextcolor(RGB(234, 238, 244));
		setbkmode(TRANSPARENT);
		outtextxy(180, 385, _T("用时："));
		TCHAR timeText[32];
		wsprintf(timeText, _T("%02d:%02d"), totalMS / 1000 / 60, totalMS / 1000 % 60);  // 格式：MM:SS
		outtextxy(310, 385, timeText);
		f.lfUnderline = true;                       // 设置文本有下划线
		settextstyle(&f);							// 设置字体样式	
		outtextxy(450, 385, _T("点击返回"));
		f.lfUnderline = false;						// 设置文本无下划线
		settextstyle(&f);
		FlushBatchDraw();
	}
}

int main()
{
	initgraph(800, 800);
	HWND hWnd = GetHWnd();						// 获得窗口句柄	
	SetWindowText(hWnd, _T("gamedemo"));		// 使用 Windows API 修改窗口名称
	IMAGE back, Start;
	loadimage(&back, _T("image\\back.png"), 800, 800, false);
	loadimage(&Start, _T("image\\start.png"));
	loadimage(&imgdoor[0], _T("image\\door\\door.png"), 144, 256, false);
	loadimage(&imgdoor[1], _T("image\\door\\door1.png"), 144, 256, false);
	loadimage(&imgdoor[2], _T("image\\door\\door2.png"), 144, 256, false);
	loadimage(&imgdoor[3], _T("image\\door\\door3.png"), 144, 256, false);
	loadimage(&imgdoor[4], _T("image\\door\\door4.png"), 144, 256, false);
	loadimage(&imgdoor[5], _T("image\\door\\door5.png"), 144, 256, false);
	loadimage(&imgdoor[6], _T("image\\door\\door6.png"), 144, 256, false);
	loadimage(&over, _T("image\\gameover.png"), 500, 200, false);
	ExMessage START, Return;
	mciSendString(_T("open music\\222.mp3 alias bgm"), 0, 0, 0);
	// 打开音乐文件，alias指定别名
	mciSendString(_T("play bgm repeat"), NULL, 0, NULL);
	// 使用别名播放音乐，repeat重复播放


	//设定地面
	for (int i = 0; i < 800; i++)
	{
		l[i][753] = 1;
	}
	bool running = true;

	loadimage(&background, _T("image\\background.jpg"));
	loadimage(&moveY, _T("image\\ALLREAL1.png"));
	loadimage(&move, _T("image\\ALLREALY11.png"));
	loadimage(&jumpY, _T("image\\FLYY1.png"));
	loadimage(&jump, _T("image\\FLY1.png"));
	loadimage(&current0, _T("image\\current\\current0.png"), 80, 80, false);
	putimage(0, 0, &background);
	putimage(person.x, person.y, 43, 82, &move, 0, 0, SRCAND);
	putimage(person.x, person.y, 43, 82, &moveY, 0, 0, SRCPAINT);


	//绘制
	BeginBatchDraw();
	while (running)
	{
		if (start == 0)
		{
			cleardevice();
			putimage(0, 0, &back);
			putimage_alpha(250, 500, &Start);
			FlushBatchDraw();
			while (peekmessage(&START))
			{
				if (START.message == WM_LBUTTONDOWN)
				{
					if (START.x >= 250 + 47 && START.x <= 250 + 253 && START.y >= 500 + 107 && START.y <= 500 + 195)
						start = 1;
					currentset();
					brickset();
					doorset();
					person.sum = 0;
					startTime = 0;
					currentload();
					for (int i = 0; i < 800; i++)
					{
						for (int j = 0; j < 753; j++)
						{
							l[i][j] = 0;
						}
					}
					for (int i = 0; i < 6; i++)
					{
						current[i][0].get = 0;
					}
					person.x = 57;
					person.y = 671;
					person.isMoveLeft = false;
					person.isMoveRight = false;
					person.isFly = false;
				}
			}
		}
		else
		{
			cleardevice();

			DWORD beginTime = GetTickCount();
			Move();
			UpdateAnimation();   // 更新动画帧
			Render();            // 渲染画面
			if (endcheck())
			{
				while (peekmessage(&Return))
				{
					if (Return.message == WM_LBUTTONDOWN)
					{
						if (Return.x >= 445 && Return.x <= 445 + 172 && Return.y >= 383 && Return.y <= 383 + 57)
							start = 0;
					}
				}
			}

			DWORD endTime = GetTickCount();				// 记录循环结束时间
			DWORD elapsedTime = endTime - beginTime;	// 计算循环耗时
			if (elapsedTime < 1000 / 60)				// 按每秒60帧进行补时
				Sleep(1000 / 60 - elapsedTime);
			FlushBatchDraw();
		}
	}
	EndBatchDraw();
	system("pause");
	closegraph();

	return 0;
}
