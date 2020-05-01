#include<iostream>
#include<windows.h>
#include<stdlib.h>   
#include<graphics.h> 
#include<time.h>
#include<conio.h>

using namespace std;

enum Operate { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, REVOLVE };
enum FLAG {SHOW,CLEAR,FIX};         
//������ʾ ���� �̶�

struct BLOCK
{
    int dir[4];
    COLORREF color;
};

struct BLOCKINFO
{
    int dir;            //LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3
    int type;           //��������
    int x, y;           //���Ͻ�����
};

//������
#define X_MIN 10          //��Ϸ�����������
#define Y_MIN 60          //��Ϸ������������
#define X_MAX 310         //��Ϸ�����Һ�����
#define Y_MAX 600         //��Ϸ������������
#define WIDTH 10          //��Ϸ�����10������߳�
#define HEIGHT 18         //��Ϸ�����18������߳�
#define SIZE 30           //����ı߳�

int map[HEIGHT][WIDTH] = { 0 };             //���к��У�18��10�У�������¼�̶��ķ���
int color[HEIGHT][WIDTH] = { 0 };           //��Ӧ����������ɫ(typeֵ)
int score = 0;                              //�÷�
unsigned int speed = 1200;                  //�����ٶ�
DWORD oldTime;

/*
ÿһ����״�������޶���һ��4*4�ķ����У������Ϳ��Եõ�һ��16λ2��������
����ת��Ϊ4λ16��������Ϊ��ÿһ�ַ���ÿһ��״̬��������
�磺0x0660 = 0000 0110 0110 0000
        0000 
        0110 
        0110 
        0000    
Ϊ�����ͣ�����ͬ��
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
//NextBlockֻ�������洢��һ����������Բ���DrawBlock������ȡ������ȫ���Ƕ���CurrentBlock��

//����ԭ��
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

//����ʵ��

//���˵�
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
    outtextxy(290, 68, "CPP����ҵ");
    outtextxy(287, 92, "����˹����");
    setcolor(MAGENTA);
    outtextxy(130, 230, "1.����ģʽ");
    setcolor(CYAN);
    outtextxy(450, 230, "2.���෽�飡");
    setcolor(GREEN);
    outtextxy(130, 350, "3.�Ѷ�����");
    setcolor(RED);
    outtextxy(450, 350, "4.�˳�");
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

//������Ϸ����
void DrawGameInterface()
{
    srand((unsigned)time(NULL));
    setbkmode(TRANSPARENT);		       //���ñ�����ɫΪ͸��

    rectangle(9, 59, 311, 601);        //����Ϸ��
    rectangle(400, 100, 550, 260);     //������ʾ��

    setcolor(GREEN);
    outtextxy(400, 70, "��һ�����ֵķ����ǣ�");
    setcolor(YELLOW);
    outtextxy(400, 280, "�÷֣�");

    char SCORE[10];
    itoa(score, SCORE, 10);           //����������ת��Ϊ�ַ���
    outtextxy(480, 280, SCORE);
    setcolor(WHITE);

    GameInit();

    while (1)
    {
        Select();
    }
}

//���෽��ģʽ����
void MoreBlockInterface()
{
    srand((unsigned)time(NULL));
    setbkmode(TRANSPARENT);		       

    rectangle(9, 59, 311, 601);        
    rectangle(400, 100, 550, 260);     

    setcolor(GREEN);
    outtextxy(400, 70, "��һ�����ֵķ����ǣ�");
    setcolor(YELLOW);
    outtextxy(400, 280, "�÷֣�");

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

//������Ϸ���ý���
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
    outtextxy(275, 80, "��Ϸ�Ѷ�����");
    setcolor(CYAN);
    outtextxy(115, 230, "1.��ͭ��������");
    setcolor(WHITE);
    outtextxy(435, 230, "2.������������");
    setcolor(MAGENTA);
    outtextxy(115, 350, "3.��ʯ���Ͽ죩");
    setcolor(RED);
    outtextxy(435, 350, "4.��ʦ�����죩");
    setcolor(YELLOW);
    outtextxy(230, 460, "����ѡ���밴5�������˵�"); 
    outtextxy(240, 485, "��ת���Ը��෽��ģʽ");
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

//����
void Select()
{
    DWORD newTime = GetTickCount();
    if (newTime - oldTime > speed)           //�����ٶ�
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
            revolve();      //��ת
            break;

        case 'a':
        case 'A':
        case 75:
            moveLeft();	    //����
            break;

        case 'd':
        case 'D':
        case 77:
            moveRight();	//����
            break;

        case 's':
        case 'S':
        case 80:
            move();	        //����
            break;

        case ' ':           //����
            sink();
            break;

        default:
            break;
        }
    }
}

//���෽��ģʽ����
void MoreBlock_Select()
{
    DWORD newTime = GetTickCount();
    if (newTime - oldTime > speed)           //�����ٶ�
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

//�Զ�����
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

//���෽��ģʽ�Զ�����
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

//��������
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

//���Ʒ���
void DrawBlock(BLOCKINFO BlockIn, FLAG flag)
{
    //ȡ����ǰ�����4λ16���Ʊ���
    int currentId = Block[BlockIn.type].dir[BlockIn.dir];     
    COLORREF currentColor = Block[BlockIn.type].color;
    int tempx = BlockIn.x;
    int tempy = BlockIn.y;
    int checkLine = 0;
    
    switch (flag)
    {
    case SHOW:
        //ѭ���Ĵλ�����
        for (int j = 0; j < 4; j++)
        {
            //��4λ�����Ʊ��뻭����(һ��)
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

//���෽��ģʽ���Ʒ���
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

//�ڸ������л�����һ������
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

//���ƶ���
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

//��ת(��ʱ��)
void revolve()              
{
    if (CheckBlock(CurrentBlock, REVOLVE))
    {
        DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.dir = (CurrentBlock.dir + 1) % 4;
        DrawBlock(CurrentBlock, SHOW);
    }
}

//����һ��
void moveLeft()             
{
    if (CheckBlock(CurrentBlock, MOVE_LEFT))
    {
        DrawBlock(CurrentBlock, CLEAR);
        CurrentBlock.x -= SIZE;
        DrawBlock(CurrentBlock, SHOW);
    }
}

//����һ��
void moveRight()            
{
    if (CheckBlock(CurrentBlock, MOVE_RIGHT))
    { 
    DrawBlock(CurrentBlock, CLEAR);
    CurrentBlock.x += SIZE;
    DrawBlock(CurrentBlock, SHOW);
    }
}

//�³�����
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

//���෽��ģʽ�³�����
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

//��Ϸ��ʼ��
void GameInit()
{
    ZeroMemory(map, WIDTH * HEIGHT * sizeof(int));

    //��4*4���ϽǷ�������Ͻ�������Ϊλ������
    NextBlock.x = 160;           
    NextBlock.y = -30;
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 7;
    NewBlock();
}
 
//���෽��ģʽ��ʼ��
void MoreBlockInit()
{
    ZeroMemory(map, WIDTH * HEIGHT * sizeof(int));
    NextBlock.x = 160;
    NextBlock.y = -30;
    NextBlock.dir = rand() % 4;
    NextBlock.type = rand() % 13;
    MoreBlock_NewBlock();
}

//���෽��ģʽ���Դ���
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

//���Դ���
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