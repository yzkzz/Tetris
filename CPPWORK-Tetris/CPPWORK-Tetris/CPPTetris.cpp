#include<iostream>
#include<windows.h>
#include<stdlib.h>   
#include<graphics.h> 
#include<time.h>
#include<conio.h>

using namespace std;

enum Operate { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, REVOLVE };
enum FLAG {SHOW,CLEAR,FIX};         
//正常显示 擦除 固定

struct BLOCK
{
    int dir[4];
    COLORREF color;
};

struct BLOCKINFO
{
    int dir;            //LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3
    int type;           //方块种类
    int x, y;           //左上角坐标
};

//基本量
#define X_MIN 10          //游戏界面左横坐标
#define Y_MIN 60          //游戏界面上纵坐标
#define X_MAX 310         //游戏界面右横坐标
#define Y_MAX 600         //游戏界面下纵坐标
#define WIDTH 10          //游戏界面宽10个方格边长
#define HEIGHT 18         //游戏界面高18个方格边长
#define SIZE 30           //方块的边长

int map[HEIGHT][WIDTH] = { 0 };             //先行后列（18行10列）用来记录固定的方块
int color[HEIGHT][WIDTH] = { 0 };           //对应坐标的填充颜色(type值)
int score = 0;                              //得分
unsigned int speed = 1200;                  //下落速度
DWORD oldTime;

/*
每一种形状都可以限定在一个4*4的方格中，这样就可以得到一个16位2进制数，
将其转化为4位16进制数作为对每一种方块每一种状态的描述。
如：0x0660 = 0000 0110 0110 0000
        0000 
        0110 
        0110 
        0000    
为田字型，其它同理。
*/
BLOCK Block[13] =
{
    { 0x0660, 0x0660, 0x0660, 0x0660, GREEN },	    //SQUARE        type = 0
    { 0x4460, 0x02E0, 0x0622, 0x0740, RED },	    //L             type = 1     
    { 0x2260, 0x0E20, 0x0644, 0x0470, MAGENTA },	//REVERSEL      type = 2
    { 0x0C60, 0x2640, 0x0C60, 0x2640, YELLOW },	    //Z             type = 3
    { 0x0360, 0x4620, 0x0360, 0x4620, WHITE },	    //REVERSEZ      type = 4
    { 0x0F00, 0x4444, 0x0F00, 0x4444, CYAN },	    //I             type = 5
    { 0x4E00, 0X4C40, 0x0E40, 0X4640, BLUE },	    //T             type = 6
    { 0x4E40, 0x4E40, 0x4E40, 0x4E40, BLUE },	    //CROSS         type = 7
    { 0xE264, 0x0EB1, 0x4C8E, 0x01D7, MAGENTA },	//WAND          type = 8
    { 0x0F60, 0X4664, 0x06F0, 0X2662, GREEN },	    //T_MAX         type = 9
    { 0xA444, 0x8780, 0x444A, 0x1E10, YELLOW },	    //Y             type = 10
    { 0xF24F, 0x9DB9, 0xF24F, 0x9DB9, CYAN },	    //Z_MAX         type = 11
    { 0x5665, 0X096F, 0xA66A, 0X0F69, RED }	        //K             type = 12
};

BLOCKINFO CurrentBlock, NextBlock;  
//NextBlock只是用来存储下一个方块的属性并用DrawBlock方法提取，操作全部是对于CurrentBlock的

//函数原型
void clear(int x);
void move();
void MoreBlock_move();
void revolve();
void moveLeft();
void moveRight();
void sink();
void MoreBlock_sink();
void Select();
void MoreBlock_Select();
void DrawBlock(BLOCKINFO BlockIn,FLAG flag);
void MoreBlock_DrawBlock(BLOCKINFO BlockIn, FLAG flag);
bool CheckBlock(BLOCKINFO BlockIn, Operate OperateIn);
void NewBlock();
void MoreBlock_NewBlock();
void GameOver();
void GameInit();
void MoreBlockInit();
void DrawGameInterface();
void MoreBlockInterface();
void DrawGameSettings();
void DrawNextBlock(BLOCKINFO BlockIn, FLAG flag);
void menu();

//函数实现

//主菜单
void menu()
{
    initgraph(640, 610);
    setbkmode(TRANSPARENT);

    setlinecolor(WHITE);
    rectangle(160, 60, 480, 120);
    rectangle(40, 210, 280, 270);
    rectangle(360, 210, 600, 270);
    rectangle(40, 330, 280, 390);
    rectangle(360, 330, 600, 390);

    setcolor(YELLOW);
    outtextxy(290, 68, "CPP大作业");
    outtextxy(287, 92, "俄罗斯方块");
    setcolor(MAGENTA);
    outtextxy(130, 230, "1.经典模式");
    setcolor(CYAN);
    outtextxy(450, 230, "2.更多方块！");
    setcolor(GREEN);
    outtextxy(130, 350, "3.难度设置");
    setcolor(RED);
    outtextxy(450, 350, "4.退出");
    setcolor(WHITE);

    int n = 1;
    while (n)
    {
        if (_kbhit())
        {
            int x = _getch();
            switch (x)
            {
            case 49:
                cleardevice();
                DrawGameInterface();
                n = 0;
                break;

            case 50:
                cleardevice();
                MoreBlockInterface();
                n = 0;
                break;

            case 51:
                cleardevice();
                DrawGameSettings();
                n = 0;
                break;

            case 52:         
                closegraph();
                exit(0);	
                break;

            default:
                break;
            }
        }
    }
}

//绘制游戏界面
void DrawGameInterface()
{
    srand((unsigned)time(NULL));
    setbkmode(TRANSPARENT);		       //设置背景颜色为透明

    rectangle(9, 59, 311, 601);        //主游戏框
    rectangle(400, 100, 550, 260);     //方块提示框

    setcolor(GREEN);
    outtextxy(400, 70, "下一个出现的方块是：");
    setcolor(YELLOW);
    outtextxy(400, 280, "得分：");

    char SCORE[10];
    itoa(score, SCORE, 10);           //将整形数据转换为字符串
    outtextxy(480, 280, SCORE);
    setcolor(WHITE);

    GameInit();

    while (1)
    {
        Select();
    }
}

//更多方块模式界面
void MoreBlockInterface()
{
    srand((unsigned)time(NULL));
    setbkmode(TRANSPARENT);		       

    rectangle(9, 59, 311, 601);        
    rectangle(400, 100, 550, 260);     

    setcolor(GREEN);
    outtextxy(400, 70, "下一个出现的方块是：");
    setcolor(YELLOW);
    outtextxy(400, 280, "得分：");

    char SCORE[10];
    itoa(score, SCORE, 10);           
    outtextxy(480, 280, SCORE);
    setcolor(WHITE);

    MoreBlockInit();

    while (1)
    {
        MoreBlock_Select();
    }
}

//绘制游戏设置界面
void DrawGameSettings()
{
    setlinecolor(WHITE);
    rectangle(160, 60, 480, 120);
    rectangle(40, 210, 280, 270);
    rectangle(360, 210, 600, 270);
    rectangle(40, 330, 280, 390);
    rectangle(360, 330, 600, 390);
    rectangle(160, 450, 480, 510);

    setbkmode(TRANSPARENT);
    setcolor(GREEN);
    outtextxy(275, 80, "游戏难度设置");
    setcolor(CYAN);
    outtextxy(115, 230, "1.青铜（较慢）");
    setcolor(WHITE);
    outtextxy(435, 230, "2.白银（正常）");
    setcolor(MAGENTA);
    outtextxy(115, 350, "3.钻石（较快）");
    setcolor(RED);
    outtextxy(435, 350, "4.宗师（极快）");
    setcolor(YELLOW);
    outtextxy(230, 460, "王者选手请按5返回主菜单"); 
    outtextxy(240, 485, "右转尝试更多方块模式");
    setcolor(WHITE);

    int n = 1;
    while (n)
    {
        if (_kbhit())
        {
            int x = _getch();
            switch (x)
            {
            case 49:
                speed = 1500;
                n = 0;
                break;

            case 50:
                speed = 1000;
                n = 0;
                break;

            case 51:
                speed = 500;
                n = 0;
                break;

            case 52:
                speed = 0;
                n = 0;
                break;

            case 53:
                n = 0;
                break;

            default:
                break;
            }
        }
    }
    cleardevice();
    menu();
}

//操作
void Select()
{
    DWORD newTime = GetTickCount();
    if (newTime - oldTime > speed)           //下落速度
    {
        oldTime = newTime;
        move();
    }

    if (_kbhit())
    {
        int ch = _getch();
        switch (ch)
        {
        case 'w':
        case 'W':
        case 72:
            revolve();      //旋转
            break;

        case 'a':
        case 'A':
        case 75:
            moveLeft();	    //左移
            break;

        case 'd':
        case 'D':
        case 77:
            moveRight();	//右移
            break;

        case 's':
        case 'S':
        case 80:
            move();	        //下移
            break;

        case ' ':           //沉底
            sink();
            break;

        default:
            break;
        }
    }
}

//更多方块模式操作
void MoreBlock_Select()
{
    DWORD newTime = GetTickCount();
    if (newTime - oldTime > speed)           //下落速度
    {
        oldTime = newTime;
        MoreBlock_move();
    }

    if (_kbhit())
    {
        int ch = _getch();
        switch (ch)
        {
        case 'w':
        case 'W':
        case 72:
            revolve();      
            break;

        case 'a':
        case 'A':
        case 75:
            moveLeft();	    
            break;

        case 'd':
        case 'D':
        case 77:
            moveRight();	
            break;

        case 's':
        case 'S':
        case 80:
            MoreBlock_move();	        
            break;

        case ' ':           
            MoreBlock_sink();
            break;

        default:
            break;
        }
    }
}

//自动下落
void move()                 
{
    int tempy;
    int currentId = Block[CurrentBlock.type].dir[CurrentBlock.dir];

    if (CheckBlock(CurrentBlock, MOVE_DOWN))
    {
        DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.y += SIZE;
        DrawBlock(CurrentBlock, SHOW);
    } 
    
    else
    {
        for (int i = 0; i < 16; i++, currentId <<= 1)
            if (currentId & 0x8000)
            {
                tempy = CurrentBlock.y + ((i / 4) * SIZE);
               
                if (((tempy) <= Y_MIN))
                {
                    GameOver();
                }

                else
                {
                    DrawBlock(CurrentBlock, CLEAR);
                    DrawBlock(CurrentBlock, FIX);
                }
            }
    }
}

//更多方块模式自动下落
void MoreBlock_move()
{
    int tempy;
    int currentId = Block[CurrentBlock.type].dir[CurrentBlock.dir];

    if (CheckBlock(CurrentBlock, MOVE_DOWN))
    {
        MoreBlock_DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.y += SIZE;
        MoreBlock_DrawBlock(CurrentBlock, SHOW);
    }

    else
    {
        for (int i = 0; i < 16; i++, currentId <<= 1)
            if (currentId & 0x8000)
            {
                tempy = CurrentBlock.y + ((i / 4) * SIZE);

                if (((tempy) <= Y_MIN))
                {
                    GameOver();
                }

                else
                {
                    MoreBlock_DrawBlock(CurrentBlock, CLEAR);
                    MoreBlock_DrawBlock(CurrentBlock, FIX);
                }
            }
    }
}

void GameOver()
{
    closegraph();
    exit(0);
}

//消除整行
void clear(int x)
{
    COLORREF tempColor = TRANSPARENT;
    int valueofColor;

    for (int tempx = 0; tempx <= x; tempx++)
    {
        for (int y = 0; y < 10; y++)
        {
            setfillcolor(TRANSPARENT);
            setlinecolor(TRANSPARENT);
            fillrectangle(X_MIN + SIZE * y, Y_MIN + SIZE * tempx, X_MIN + SIZE * (y + 1), Y_MIN + SIZE * (tempx + 1));
        }
    }

    for (int mapx = x; mapx > 0; mapx--)
    {
        for (int mapy = 0; mapy < 10; mapy++)
        {
            map[mapx][mapy] = map[mapx - 1][mapy];
            color[mapx][mapy] = color[mapx - 1][mapy];
        }
    }

    for (int i = 0; i < 10; i++)
    {
        map[0][i] = 0;
        color[0][i] = 0;
    }

    for (int tempx = 0; tempx <= x; tempx++)
    {
        for (int y = 0; y < 10; y++)
        {
            if (map[tempx][y])
            {
                valueofColor = color[tempx][y];
                switch (valueofColor)
                {
                case 0:
                case 9:
                    tempColor = GREEN;
                    break;

                case 1:
                case 12:
                    tempColor = RED;
                    break;

                case 2:
                case 8:
                    tempColor = MAGENTA;
                    break;

                case 3:
                case 10:
                    tempColor = YELLOW;
                    break;

                case 4:
                    tempColor = WHITE;
                    break;

                case 5:
                case 11:
                    tempColor = CYAN;
                    break;

                case 6:
                case 7:
                    tempColor = BLUE;
                    break;

                default:
                    break;
                }

                setfillcolor(RGB(GetRValue(tempColor) * 2 / 3, GetGValue(tempColor) * 2 / 3, GetBValue(tempColor) * 2 / 3));
                setlinecolor(DARKGRAY);
                fillrectangle(X_MIN + SIZE * y, Y_MIN + SIZE * tempx, X_MIN + SIZE * (y + 1), Y_MIN + SIZE * (tempx + 1));
            }
        }
    }
}

//绘制方块
void DrawBlock(BLOCKINFO BlockIn, FLAG flag)
{
    //取出当前方块的4位16进制编码
    int currentId = Block[BlockIn.type].dir[BlockIn.dir];     
    COLORREF currentColor = Block[BlockIn.type].color;
    int tempx = BlockIn.x;
    int tempy = BlockIn.y;
    int checkLine = 0;
    
    switch (flag)
    {
    case SHOW:
        //循环四次画四行
        for (int j = 0; j < 4; j++)
        {
            //把4位二进制编码画出来(一行)
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(Block[BlockIn.type].color);
                    setlinecolor(LIGHTGRAY);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        Sleep(500);
        break;

    case CLEAR:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(TRANSPARENT);
                    setlinecolor(TRANSPARENT);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        break;

    case FIX:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(RGB(GetRValue(currentColor) * 2 / 3, GetGValue(currentColor) * 2 / 3, GetBValue(currentColor) * 2 / 3));
                    setlinecolor(DARKGRAY);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);

                    int mapx = (tempy - Y_MIN) / SIZE;
                    int mapy = (tempx + SIZE * i - X_MIN) / SIZE;
                    map[mapx][mapy] = 1;
                    color[mapx][mapy] = BlockIn.type;
                }
            }
            tempy += SIZE;
        }

        for (int x = 1; x < 18; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (map[x][y])
                {
                    checkLine++;
                }
            }
            
            if (checkLine == 10)
            {
                clear(x);
                score += 10;

                clearrectangle(470, 270, 570, 370);
                setcolor(YELLOW);
                char SCORE[10];
                itoa(score, SCORE, 10);
                outtextxy(480, 280, SCORE);
                setcolor(WHITE);
            }

            checkLine = 0;
        }

        NewBlock();
        break;

    default:
        break;
    }
}

//更多方块模式绘制方块
void MoreBlock_DrawBlock(BLOCKINFO BlockIn, FLAG flag)
{
    int currentId = Block[BlockIn.type].dir[BlockIn.dir];
    COLORREF currentColor = Block[BlockIn.type].color;
    int tempx = BlockIn.x;
    int tempy = BlockIn.y;
    int checkLine = 0;

    switch (flag)
    {
    case SHOW:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(Block[BlockIn.type].color);
                    setlinecolor(LIGHTGRAY);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        Sleep(500);
        break;

    case CLEAR:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(TRANSPARENT);
                    setlinecolor(TRANSPARENT);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        break;

    case FIX:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, currentId <<= 1)
            {
                if (currentId & 0x8000 && tempy >= 60)
                {
                    setfillcolor(RGB(GetRValue(currentColor) * 2 / 3, GetGValue(currentColor) * 2 / 3, GetBValue(currentColor) * 2 / 3));
                    setlinecolor(DARKGRAY);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);

                    int mapx = (tempy - Y_MIN) / SIZE;
                    int mapy = (tempx + SIZE * i - X_MIN) / SIZE;
                    map[mapx][mapy] = 1;
                    color[mapx][mapy] = BlockIn.type;
                }
            }
            tempy += SIZE;
        }

        for (int x = 1; x < 18; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (map[x][y])
                {
                    checkLine++;
                }
            }

            if (checkLine == 10)
            {
                clear(x);
                score += 10;

                clearrectangle(470, 270, 570, 370);
                setcolor(YELLOW);
                char SCORE[10];
                itoa(score, SCORE, 10);
                outtextxy(480, 280, SCORE);
                setcolor(WHITE);
            }

            checkLine = 0;
        }

        MoreBlock_NewBlock();
        break;

    default:
        break;
    }
}

//在副窗口中画出下一个方块
void DrawNextBlock(BLOCKINFO BlockIn, FLAG flag)            
{
    int tempx = 415;
    int tempy = 130;
    int nextId = Block[BlockIn.type].dir[BlockIn.dir];

    switch (flag)
    {
    case SHOW:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, nextId <<= 1)
            {
                if (nextId & 0x8000)
                {
                    setfillcolor(Block[BlockIn.type].color);
                    setlinecolor(LIGHTGRAY);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        break;

    case CLEAR:
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 4; i++, nextId <<= 1)
            {
                if (nextId & 0x8000)
                {
                    setfillcolor(TRANSPARENT);
                    setlinecolor(TRANSPARENT);
                    fillrectangle(tempx + SIZE * i, tempy, tempx + SIZE * (i + 1), tempy + SIZE);
                }
            }
            tempy += SIZE;
        }
        break;
    }
}

//可移动性
bool CheckBlock(BLOCKINFO BlockIn, Operate OperateIn)
{
    int currentId = Block[BlockIn.type].dir[BlockIn.dir];
    int tempx, tempy;
    int mapx, mapy;
    int tempDir = BlockIn.dir;
    int tempId;

    switch (OperateIn)
    {
    case MOVE_LEFT:
        for (int i = 0; i < 16; i++, currentId <<= 1)
            if (currentId & 0x8000)
            {
                tempx = BlockIn.x + ((i % 4) * SIZE);
                mapx = (BlockIn.y + ((i / 4) * SIZE) - Y_MIN) / SIZE;
                mapy = (BlockIn.x + ((i % 4) * SIZE) - X_MIN) / SIZE;

                if (((tempx - SIZE) < X_MIN) || (map[mapx][mapy - 1]))
                {
                    return false;
                }
            }
        break;

    case MOVE_RIGHT:
        for (int i = 0; i < 16; i++, currentId <<= 1)
            if (currentId & 0x8000)
            {
                tempx = BlockIn.x + ((i % 4 + 1) * SIZE);
                mapx = (BlockIn.y + ((i / 4) * SIZE) - Y_MIN) / SIZE;
                mapy = (BlockIn.x + ((i % 4) * SIZE) - X_MIN) / SIZE;

                if (((tempx + SIZE) > X_MAX) || (map[mapx][mapy + 1]))
                {
                    return false;
                }
            }
        break;

    case MOVE_DOWN:
        for (int i = 0; i < 16; i++, currentId <<= 1)
            if (currentId & 0x8000)
            {
                tempy = BlockIn.y + ((i / 4 + 1) * SIZE); 
                mapx = (BlockIn.y + ((i / 4 + 1) * SIZE) - Y_MIN) / SIZE;
                mapy = (BlockIn.x + ((i % 4) * SIZE) - X_MIN) / SIZE;

                if (((tempy + SIZE) > Y_MAX) || (map[mapx][mapy]))
                {
                    return false;
                }
            }
        break;

    case REVOLVE:
        tempDir = (tempDir + 1) % 4;
        tempId = Block[BlockIn.type].dir[tempDir];
        
        for (int i = 0; i < 16; i++, tempId <<= 1)
            if (tempId & 0x8000)
            {
                tempx = BlockIn.x + ((i % 4) * SIZE);
                tempy = BlockIn.y + ((i / 4 + 1) * SIZE);
                mapx = (BlockIn.y + ((i / 4) * SIZE) - Y_MIN) / SIZE;
                mapy = (BlockIn.x + ((i % 4) * SIZE) - X_MIN) / SIZE;

                if (((tempx + SIZE) > X_MAX) || (tempx < X_MIN) || (tempy > Y_MAX) || (tempy - SIZE < Y_MIN) || (map[mapx][mapy]))
                {
                    return false;
                }
            }
        break;

    default:
        break;
    }
}

//旋转(逆时针)
void revolve()              
{
    if (CheckBlock(CurrentBlock, REVOLVE))
    {
        DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.dir = (CurrentBlock.dir + 1) % 4;
        DrawBlock(CurrentBlock, SHOW);
    }
}

//左移一格
void moveLeft()             
{
    if (CheckBlock(CurrentBlock, MOVE_LEFT))
    {
        DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.x -= SIZE;
        DrawBlock(CurrentBlock, SHOW);
    }
}

//右移一格
void moveRight()            
{
    if (CheckBlock(CurrentBlock, MOVE_RIGHT))
    { 
    DrawBlock(CurrentBlock, CLEAR);
    CurrentBlock.x += SIZE;
    DrawBlock(CurrentBlock, SHOW);
    }
}

//下沉到底
void sink()
{
    DrawBlock(CurrentBlock, CLEAR);
    BLOCKINFO tempBlock = CurrentBlock;

    while (CheckBlock(tempBlock,MOVE_DOWN))
    {
        CurrentBlock.y += SIZE;
        tempBlock.y += SIZE;
    }
    DrawBlock(CurrentBlock, FIX);
}

//更多方块模式下沉到底
void MoreBlock_sink()
{
    MoreBlock_DrawBlock(CurrentBlock, CLEAR);
    BLOCKINFO tempBlock = CurrentBlock;

    while (CheckBlock(tempBlock, MOVE_DOWN))
    {
        CurrentBlock.y += SIZE;
        tempBlock.y += SIZE;
    }
    MoreBlock_DrawBlock(CurrentBlock, FIX);
}

//游戏初始化
void GameInit()
{
    ZeroMemory(map, WIDTH * HEIGHT * sizeof(int));

    //用4*4左上角方块的左上角坐标作为位置坐标
    NextBlock.x = 160;           
    NextBlock.y = -30;
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 7;
    NewBlock();
}
 
//更多方块模式初始化
void MoreBlockInit()
{
    ZeroMemory(map, WIDTH * HEIGHT * sizeof(int));
    NextBlock.x = 160;
    NextBlock.y = -30;
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 13;
    MoreBlock_NewBlock();
}

//更多方块模式属性传递
void MoreBlock_NewBlock()
{
    DrawNextBlock(NextBlock, CLEAR);
    CurrentBlock.x = NextBlock.x;
    CurrentBlock.y = NextBlock.y;
    CurrentBlock.dir = NextBlock.dir;
    CurrentBlock.type = NextBlock.type;
    MoreBlock_DrawBlock(CurrentBlock, SHOW);
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 13;
    DrawNextBlock(NextBlock, SHOW);
}

//属性传递
void NewBlock()
{
    DrawNextBlock(NextBlock, CLEAR);
    CurrentBlock.x = NextBlock.x;
    CurrentBlock.y = NextBlock.y;
    CurrentBlock.dir = NextBlock.dir;
    CurrentBlock.type = NextBlock.type;
    DrawBlock(CurrentBlock, SHOW);
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 7;
    DrawNextBlock(NextBlock, SHOW);
}

int main()
{
    menu();
    return 0;
}