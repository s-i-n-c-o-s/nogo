#include<fstream>
#include<stdlib.h>
#include <climits>
#include <cstring>
#include <ctime>
#include <iostream>
#include<graphics.h>
#include<conio.h>
#include <mmsyscom.h>
#include<windows.h>
#pragma comment(lib,"winmm.lib")
using namespace std;



int board[9][9] = { 0 };
bool via_search[9][9] = { 0 };
int value[9][9] = { 0 };

int dx[4] = { 0,0,1,-1 };
int dy[4] = { 1,-1,0,0 };

int start = 0;
int timeout = (int)(0.9 * (double)CLOCKS_PER_SEC);
int musicon = 1;



//�������ͼƬ���˵�������ͼ���ڣ�1000*500����
void background()
{
    IMAGE menu;
    loadimage(&menu, L"����.png", 1000, 500);
    putimage(0, 0, &menu);
}

//��������
void chessboard()
{
    int step = 50;
    setlinestyle(PS_SOLID, 3);  //�߿�3����
    setcolor(RGB(0, 0, 0));  //��ɫΪ��ɫ
    int i, j;
    for (i = 1; i <= 9; i++)
    {
        line(i * step, 1 * step, i * step, 9 * step);
        line(1 * step, i * step, 9 * step, i * step);
    }

}

//���ֿ��ع���
void music_on()
{
    if (musicon)
    {
        mciSendString(L"close mymusic", 0, 0, 0);
        musicon = 0;
    }
    else {
        mciSendString(L"open music.mp3 alias mymusic", NULL, 0, NULL);

        mciSendString(L"play mymusic repeat", 0, 0, 0);
        musicon = 1;
    }
}

//�浵����
void save_board()
{
    setfillcolor(RED);
    fillcircle(670, 115, 10);
    ofstream fout("data.txt", ios::out);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (j == 0)fout << board[i][0];
            else fout << " " << board[i][j];
        }
        if (i != 8)fout << endl;
    }
    fout.close();
}


//�ж������Ƿ���������
bool if_inboard(int x, int y)
{
    if (x >= 0 && x < 9 && y >= 0 && y < 9)
        return true;
    else return false;
}

//�жϸ�λ���Ƿ�����
bool if_hasair(int x, int y)
{
    via_search[x][y] = true;
    for (int i = 0; i < 4; i++)
    {
        int x1 = x + dx[i];
        int y1 = y + dy[i];
        if (if_inboard(x1, y1))
        {
            if (board[x1][y1] == 0)
                return true;
            if (board[x1][y1] == board[x][y] && !via_search[x1][y1] && if_hasair(x1, y1))
                return true;
        }
    }
    return false;
}

//�жϸ�λ���ܷ�����
bool if_put(int x, int y, int color)
{
    if (board[x][y])
        return false;
    board[x][y] = color;
    memset(via_search, 0, sizeof(via_search));
    if (!if_hasair(x, y))
    {
        board[x][y] = 0;
        return false;
    }
    for (int i = 0; i < 4; i++)
    {
        int x1 = x + dx[i];
        int y1 = y + dy[i];
        if (if_inboard(x1, y1) && board[x1][y1] && !via_search[x1][y1] && !if_hasair(x1, y1))
        {
            board[x][y] = 0;
            return false;
        }
    }

    board[x][y] = 0;
    return true;
}

//ʤ�ʼ���
int eva(int color)
{
    int w = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            if (if_put(i, j, color))w++;
            if (if_put(i, j, -color))w--;

        }
    return w;
}

//ʤ����ʾ
void winrate()
{
        double wb = 0,ww=0;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
            {
                if (if_put(i, j, 1))wb++;
                if (if_put(i, j, -1))ww++;


            }
    setcolor(WHITE);
    setfillcolor(WHITE);
    fillrectangle(100, 475, 400, 490);
  
    double rate = wb / (wb + ww);
  
    setfillcolor(BLACK);
    fillrectangle(100, 475, 100+300*rate, 490);
}

int minimax(int color, int depth, int nAlpha, int nBeta, int x, int y)
{
    if (depth == 0)
        return eva(color); //Ҷ�ӽڵ㷵�ع�ֵ

    int availablex[81] = { 0 }, availabley[81] = { 0 }; //����λ��
    int availablenum = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
        {
            if (if_put(i, j, color))
            {
                availablex[availablenum] = i;
                availabley[availablenum] = j;
                availablenum++;
            }
        }
    if (availablenum == 0)return eva(color);
    if (color == 1)//min��
    {
        int score = INT_MAX;
        for (int i = 0; i < availablenum; i++)
        {
            board[availablex[i]][availabley[i]] = color;
            int temp_score = minimax((0-color), depth - 1, nAlpha, nBeta, availablex[i], availabley[i]);
            board[availablex[i]][availabley[i]] = 0;
            if (temp_score < score)
                score = temp_score;
            if (score < nBeta)//����betaֵ
                nBeta = score;
            if (nAlpha >= nBeta)//��֦
                break;
            if (clock() - start >= timeout)//����ʱ
                return nBeta;    
        }
        return nBeta;

    }
    else {//MAX��
        int score = INT_MIN;
        for (int i = 0; i < availablenum; i++)
        {
            board[availablex[i]][availabley[i]] = color;
            int temp_score = minimax((0 - color), depth - 1, nAlpha, nBeta, availablex[i], availabley[i]);
            board[availablex[i]][availabley[i]] = 0;
            if (temp_score > score)
                score = temp_score;
            if (score > nAlpha)//����alphaֵ
                nAlpha = score;
            if (nAlpha >= nBeta)//��֦
                break;
            if (clock() - start >= timeout)//����ʱ
                return nAlpha;
        }
        return nAlpha;

    }
}

int main()
{
    //��������
    initgraph(1000, 500);
    background();
   
    mciSendString(L"open music.mp3 alias mymusic", NULL, 0, NULL);
    mciSendString(L"play mymusic repeat", 0, 0, 0);
    
   //�������ѡ��˵���Ϣ
    ExMessage msg,m;
    while (true) {

        if (peekmessage(&msg, EM_MOUSE)) {
            switch (msg.message)
            {
            case WM_LBUTTONDOWN:
                //���������µĿ�ʼ��
                if (msg.x >= 500 && msg.x <= 500 + 100 && msg.y >= 130 && msg.y <= 130 + 280)
                {
                    bool b = 0;
                    background();
                    memset(board, 0, sizeof(board));
                    chessboard();
                    setfillcolor(RED);
                    fillcircle(550, 115, 10);
                    int x = 0, y = 0, decisionx = 500, decisiony = 900; int r = 0;
                    int lastx = 0, lasty = 0, lastdecision_x = 500, lastdecision_y = 900;
                    while (1)
                    {
                        bool bo = 0;
                        //���������һ��������λ��
                       
                        if(_kbhit()) {

                            x = _getch()-'0';
                            y = _getch()-'0';

                            setcolor(BLACK);
                           
                            if (if_put(x, y, 1)) {
                                if (r == 0 && x == 4 && y == 4)
                                {
                                    setbkmode(TRANSPARENT);
                                    settextcolor(RGB(255, 0, 0));
                                    TCHAR s1[] = L"����";
                                    settextstyle(60, 0, s1);
                                    TCHAR s[50] = L"�׷�����ʤ";
                                    outtextxy(120, 200, s);
                                    
                                    break;
                                }
                                r = 1;
                                board[x][y] = 1;
                                setfillcolor(BLACK);
                                fillcircle((x + 1) * 50, (y + 1) * 50, 12);
                                if (!b) {
                                setcolor(WHITE);
                                setfillcolor(WHITE);
                                fillcircle((lastdecision_x + 1) * 50, (lastdecision_y + 1) * 50, 12);
                                b = 0;
                            }
                            }
                            else {
                                setfillcolor(BLACK);
                                fillcircle((x + 1) * 50, (y + 1) * 50, 12);
                                setbkmode(TRANSPARENT);
                                settextcolor(RGB(255, 0, 0));
                                TCHAR s1[] = L"����";
                                settextstyle(60, 0, s1);
                                TCHAR s[50] = L"�׷�����ʤ";
                                outtextxy(120, 200, s);
                                
                                break;
                            }

                            
                            
                            int color = -1;
                            int max1 = INT_MIN;
                            int best[81][2] = { 0 }, bestnum = 0;
                            memset(value, 0, sizeof(value));
                            for (int i = 0; i < 9; i++)
                                for (int j = 0; j < 9; j++)
                                {
                                    if (if_put(i, j, color))
                                    {
                                        board[i][j] = color;
                                        value[i][j] =minimax(color,4,INT_MIN,INT_MAX,i,j);
                                        if (value[i][j] > max1)
                                            max1 = value[i][j];
                                        board[i][j] = 0;
                                    }
                                    else value[i][j] = INT_MIN;
                                    if (clock() - start > timeout)
                                        break;
                                }
                            for (int i = 0; i < 9; i++)
                                for (int j = 0; j < 9; j++)
                                    if (value[i][j] == max1)
                                    {
                                        best[bestnum][0] = i;
                                        best[bestnum][1] = j;
                                        bestnum++;
                                    }
                            int random = rand() % bestnum; //���������value�������ѡ
                            decisionx = best[random][0];
                            decisiony = best[random][1];
                           
                           
                            board[decisionx][decisiony] = -1;
                            setcolor(RED);
                            setfillcolor(WHITE);
                            fillcircle((decisionx+1)*50, (decisiony+1)*50, 12);


                            winrate();
                        }
                        //�������˵�
                        else if (peekmessage(&m, EM_MOUSE))
                        {
                            switch (m.message)
                            {
                                
                            case WM_LBUTTONDOWN:
                                //����������
                                if (m.x >= 860 && m.x <= 860 + 100 && m.y >= 130 && m.y <= 130 + 140)
                                {
                                    setfillcolor(RED);
                                    fillcircle(910, 115, 10);
                                    setbkmode(TRANSPARENT);
                                    settextcolor(RGB(255, 0, 0));
                                    TCHAR s1[] = L"����";
                                    settextstyle(60, 0, s1);
                                    TCHAR s[50] = L"���Ľ���";
                                    outtextxy(140, 200, s);
                                    bo = 1;
                                   
                                }
                                //����������
                                else if (m.x >= 780 && m.x <= 860 && m.y >= 430 && m.y <=480)
                                {
                                    
                                    board[lastx][lasty] = 0;
                                    board[lastdecision_x][lastdecision_y] = 0;
                                    background();
                                    chessboard();
                                    for (int i = 0; i < 9; i++)
                                        for (int j = 0; j < 9; j++)
                                        {
                                           
                                            if (board[i][j] == 1)
                                            {
                                                setfillcolor(BLACK);
                                                fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                                            }
                                            else if (board[i][j] == -1)
                                            {
                                                setfillcolor(WHITE);
                                                fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                                            }
                                        }
                                    b = 1;
                                  
                                }
                               //�������浵
                                else if (m.x >= 620 && m.x <= 620 + 100 && m.y >= 130 && m.y <= 130 + 140)
                                {
                                    save_board();
                                }
                                //����������,ʵ�����ֵĿ���
                                else if (m.x >= 900 && m.x <= 980 && m.y >= 430 && m.y <= 480)
                                {
                                    if (musicon)
                                    {
                                        mciSendString(L"close mymusic", 0, 0, 0);
                                        musicon = 0;
                                    }
                                    else {
                                        mciSendString(L"open music.mp3 alias mymusic", NULL, 0, NULL);
                                        mciSendString(L"play mymusic repeat", 0, 0, 0);
                                        musicon = 1;
                                    }
                                    
                                }
                                

                            default:
                                break;
                           
                            }
                        }
                        if (bo == 1)break;
                        lastx = x;
                        lasty = y;
                        lastdecision_x = decisionx;
                        lastdecision_y = decisiony;
                        

                    }

                }
               
                //�����������̡�
                else if (msg.x >= 740 && msg.x <= 740 + 100 && msg.y >= 130 && msg.y <= 130 + 140)
                {
                int x = 0, y = 0, decisionx = 0, decisiony = 0;
                int lastx = 0, lasty = 0, lastdecision_x = 0, lastdecision_y = 0;
                background();
                chessboard();
                setfillcolor(RED);
                fillcircle(790, 115, 10);
                   
                memset(board, 0, sizeof(board));
                ifstream fin("data.txt", ios::in);
                for(int i=0;i<9;i++)
                    for (int j = 0; j < 9; j++)
                    {
                        fin >> board[i][j];
                        if (board[i][j] == 1)
                        {
                            setfillcolor(BLACK);
                            fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                        }
                        else if (board[i][j] == -1)
                        {
                            setfillcolor(WHITE);
                            fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                        }
                    }
               
                while (1)
                {
                    bool bo = 0;
                    //���������һ��������λ��
                    if (_kbhit()) {

                        x = _getch() - '0';
                        y = _getch() - '0';

                        if (if_put(x, y, 1)) {
                            board[x][y] = 1;
                            setfillcolor(BLACK);
                            fillcircle((x + 1) * 50, (y + 1) * 50, 12);
                        }
                        else {
                           
                            setfillcolor(BLACK);
                            fillcircle((x + 1) * 50, (y + 1) * 50, 12);
                            setbkmode(TRANSPARENT);
                            settextcolor(RGB(255, 0, 0));
                            TCHAR s1[] = L"����";
                            settextstyle(60, 0, s1);
                            TCHAR s[50] = L"�׷�����ʤ";
                            outtextxy(120, 200, s);
                            break;
                        }
                        int color = -1;
                        int max1 = INT_MIN;
                        int best[81][2] = { 0 }, bestnum = 0;
                        for (int i = 0; i < 9; i++)
                            for (int j = 0; j < 9; j++)
                            {
                                if (if_put(i, j, color))
                                {
                                    board[i][j] = color;
                                    value[i][j] = minimax(color, 4, INT_MIN, INT_MAX, i, j);
                                    if (value[i][j] > max1)
                                        max1 = value[i][j];
                                    board[i][j] = 0;
                                }
                                else value[i][j] = INT_MIN;
                                if (clock() - start > timeout)
                                    break;
                            }
                        for (int i = 0; i < 9; i++)
                            for (int j = 0; j < 9; j++)
                                if (value[i][j] == max1)
                                {
                                    best[bestnum][0] = i;
                                    best[bestnum][1] = j;
                                    bestnum++;
                                }
                        int random = rand() % bestnum; //���������value�������ѡ
                        int decisionx = best[random][0];
                        int decisiony = best[random][1];

                       
                        board[decisionx][decisiony] = -1;
                        setfillcolor(WHITE);
                        fillcircle((decisionx + 1) * 50, (decisiony + 1) * 50, 12);

                        winrate();
                    }
                    //�������˵�
                    else if (peekmessage(&m, EM_MOUSE))
                    {
                        switch (m.message)
                        {

                        case WM_LBUTTONDOWN:
                            //����������
                            if (m.x >= 860 && m.x <= 860 + 100 && m.y >= 130 && m.y <= 130 + 140)
                            {
                                setbkmode(TRANSPARENT);
                                settextcolor(RGB(255, 0, 0));
                                TCHAR s1[] = L"����";
                                settextstyle(60, 0, s1);
                                TCHAR s[50] = L"���Ľ���";
                                outtextxy(140, 200, s);
                                bo = 1;

                            }
                            //����������
                            else if (m.x >= 780 && m.x <= 860 && m.y >= 430 && m.y <= 480)
                            {

                                board[x][y] = 0;
                                board[decisionx][decisiony] = 0;
                                background();
                                chessboard();
                                for (int i = 0; i < 9; i++)
                                    for (int j = 0; j < 9; j++)
                                    {

                                        if (board[i][j] == 1)
                                        {
                                            setfillcolor(BLACK);
                                            fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                                        }
                                        else if (board[i][j] == -1)
                                        {
                                            setfillcolor(WHITE);
                                            fillcircle((i + 1) * 50, (j + 1) * 50, 12);
                                        }
                                    }
                            }

                            //�������浵
                            else if (m.x >= 620 && m.x <= 620 + 100 && m.y >= 130 && m.y <= 130 + 140)
                            {
                                save_board();
                            }
                            //����������
                            else if (m.x >= 900 && m.x <= 980 && m.y >= 430 && m.y <= 480)
                            {
                                if (musicon)
                                {
                                    mciSendString(L"close mymusic", 0, 0, 0);
                                    musicon = 0;
                                }
                                else {
                                    mciSendString(L"open music.mp3 alias mymusic", NULL, 0, NULL);

                                    mciSendString(L"play mymusic repeat", 0, 0, 0);
                                    musicon = 1;
                                }

                            }



                        }
                    }

                    if (bo == 1)break;
                    lastx = x;
                    lasty = y;
                    lastdecision_x = decisionx;
                    lastdecision_y = decisiony;
                }
                   
                }
                //�����������֡�
                else if (msg.x >= 900 && msg.x <= 980 && msg.y >= 430 && msg.y <= 480)
                {
                music_on();
                }
               default:
                   break;
            }
        }

    }
  
    closegraph();

    return 0;
}
