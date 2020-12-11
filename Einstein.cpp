#include "Einstein.h"
#include "cstdlib"
#include "time.h"
#include "list"
#include "fstream"

using namespace std;
//linux下编译！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//192.168.1.100
//往敌人子少的地方去
//（1-p)*value*q   q:该棋子走动的概率
//评分差距拉大


int string_to_int(string s);//parse中解析棋盘情况，储存在int数组
int choices_num(int chessboard[25],int dice);//对我方来说，可以行棋的选择数量
int find_psbChess(int chessboard[25],int dice,int seq);//对掷出的骰子，可以走的棋子
float set_single_value(int psbChessBoard[25],int dice);//单个棋子估值
float set_whole_value(int psbChessBoard[25],int dice);//整个棋盘估值
int roll_P_pre(int board[25],int Dice);//所有棋子被选择到的情况数量之和
int roll_P_all(int board[25],int lmt1,int lmt2);//某个棋子被选择的情况数量

int bwn=0;
int rwn=0;
int tn=0;
int win=0;
int first=0;

time_t now_time_start_all=time(0);
tm* ltm_start_all=localtime(&now_time_start_all);
int year_start_all=1900+ltm_start_all->tm_yday;
int month_start_all=ltm_start_all->tm_mon;
int day_start_all=ltm_start_all->tm_mday;
int hour_start_all=ltm_start_all->tm_hour;
int min_start_all=ltm_start_all->tm_min;
int sec_start_all=ltm_start_all->tm_sec;

Einstein::Einstein()
{
    this->clientsocket.connectServer();
    clientsocket.sendMsg(ID);
}

Einstein::~Einstein()
{
    this->clientsocket.close();
}

int Einstein::parse(string rcvmsg)
{
    int newboard[25];
    
    char str[100];
    int m=0;
    while(rcvmsg[m]!='\0')
    {
        str[m]=rcvmsg[m];
        m++;
    }
    str[m]='\0';

    char* ptr;
    ptr=strtok(str, " [ ] , |");
    
    int k=0;
    while (ptr)
    {
        string t=ptr;
        if(k==25)
            dice=string_to_int(t);
        else
            newboard[k]=string_to_int(t);
        
        k++;
        ptr=strtok(NULL, " [ ] , |");
    }
  
    int dif=0;
    for(int i=0;i<25;i++)
        if(newboard[i]!=chessboard[i])
            dif++;
    
    for(int i=0;i<25;i++)
        chessboard[i]=newboard[i];
    
    if(dif>5)
        return 1;//新一局
    else
        return 0;
}

int Einstein::handle()
{
    while(1)
    {
        if(this->clientsocket.recvMsg()==0)
        {
            string s=this->clientsocket.getRecvMsg();
            string logmsg;//log的信息

            if(s=="close"||parse(s)==1)//close:所以结束 没有发送新的棋局  parse=1:开了新的一局 应该发送上一局的结果
            {//parse(s)有没有给chessboard赋新的值？ yes
                
                if(first==0)
                {
                    logmsg="NEW ROUND";
                    first++;
                    now_time_start_all=time(0);
                    ltm_start_all=localtime(&now_time_start_all);
                    min_start_all=ltm_start_all->tm_min;
                    sec_start_all=ltm_start_all->tm_sec;
                }
                else
                {
                    if(dice<=6)//我是red
                    {
                        if(win==1)
                        {
                            logmsg="RESULT: RED WIN";
                            rwn++;
                        }
                        else 
                        {
                            logmsg="RESULT: BLUE WIN";
                            bwn++;
                        }
                        
                      
                    }
                    else//我是blue
                    {
                        if(win==2)
                        {
                            logmsg="RESULT: BLUE WIN";
                            bwn++;
                        }
                        else
                        {
                            logmsg="RESULT: RED WIN";
                            rwn++;
                        }
                
                    }
                    tn++;
                }
                
            }
            else
                logmsg=s;
            
            if(s=="close")
            {
                logging(logmsg);
                return 0;
            }
            
            parse(s);//parse 中解析
            string sndmsg;//传输出去的信息
      
            logging(logmsg);
            
            int real_board[5][5];
            int k=0;
            for(int i=0;i<5;i++)
                for (int j=0;j<5;j++)
                {
                    real_board[i][j]=chessboard[k];
                    k++;
                }
            
            int choiceNum;
            //1:dice  2:两个可能的选择  3:有小无大 4:有大无小
            if(choices_num(chessboard, dice)==2) choiceNum=2;
            else  choiceNum=1;
            
            int psbChess[choiceNum];
            if(choices_num(chessboard, dice)==1)
                psbChess[0]=dice;
            else if(choices_num(chessboard, dice)==2)
            {
                psbChess[0]=find_psbChess(chessboard, dice, 1);
                psbChess[1]=find_psbChess(chessboard, dice, 0);
            }
            else if(choices_num(chessboard, dice)==3)
                psbChess[0]=find_psbChess(chessboard, dice, 0);
            else
                psbChess[0]=find_psbChess(chessboard, dice, 1);
            

            
            int psbSltNum=0;
            for(int k=0;k<choiceNum;k++)
            {
                if(dice<=6)//red
                    for(int i=0;i<5;i++)
                        for(int j=0;j<5;j++)
                            if(real_board[i][j]==psbChess[k])
                            {
                                if(i!=4&&j!=4)
                                    psbSltNum+=3;
                                else
                                    psbSltNum++;
                                break;
                            }
                if(dice>6)//blue
                {
                    for(int i=0;i<5;i++)
                        for(int j=0;j<5;j++)
                            if(real_board[i][j]==psbChess[k])
                            {
                                if(i!=0&&j!=0)
                                    psbSltNum+=3;
                                else
                                    psbSltNum++;
                                break;
                            }
                }
                
            }
            
            string psbSndMsg[psbSltNum];
            int psbChessBoard[psbSltNum][25];
            for(int i=0;i<psbSltNum;i++)
                for(int j=0;j<25;j++)
                    psbChessBoard[i][j]=chessboard[j];
            int p=0;
            for(int k=0;k<choiceNum;k++)
            {
                if(dice<=6)//red: down right
                    for(int i=0;i<5;i++)
                        for(int j=0;j<5;j++)
                            if(real_board[i][j]==psbChess[k])
                            {
                                if(i!=4&&j!=4)//rightdown
                                {
                                    
                                    psbSndMsg[p]="0|rightdown";
                                    psbSndMsg[p][0]=psbChess[k]+'0';
                                    psbChessBoard[p][5*i+j]=0;
                                    psbChessBoard[p][5*i+j+6]=psbChess[k];
                                    p++;
                                    
                                    psbSndMsg[p]="0|right";
                                    psbSndMsg[p][0]=psbChess[k]+'0';
                                    psbChessBoard[p][5*i+j]=0;
                                    psbChessBoard[p][5*i+j+1]=psbChess[k];
                                    p++;
                                    
                                    psbSndMsg[p]="0|down";
                                    psbSndMsg[p][0]=psbChess[k]+'0';
                                    psbChessBoard[p][5*i+j]=0;
                                    psbChessBoard[p][5*i+j+5]=psbChess[k];
                                    p++;
                                    
                                    
                                    
                                   
                                }
                                else if(j==4)//down
                                {
                                    psbSndMsg[p]="0|down";
                                    psbSndMsg[p][0]=psbChess[k]+'0';
                                    psbChessBoard[p][5*i+j]=0;
                                    psbChessBoard[p][5*i+j+5]=psbChess[k];
                                    p++;
                                }
                                else//right
                                    {
                                        psbSndMsg[p]="0|right";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j+1]=psbChess[k];
                                        p++;
                                    }
                                break;
                            }
                if(dice>6)//blue: upleft
                {
                    for(int i=0;i<5;i++)
                        for(int j=0;j<5;j++)
                            if(real_board[i][j]==psbChess[k])
                            {
                                if(psbChess[k]<10)
                                {
                                    if(j==0)//left
                                    {
                                        psbSndMsg[p]="0|up";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-5]=psbChess[k];
                                        p++;
                                    }
                                    else if(i==0)//left
                                    {
                                        psbSndMsg[p]="0|left";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-1]=psbChess[k];
                                        p++;
                                    }
                                    else//leftup
                                    {
                                        psbSndMsg[p]="0|leftup";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-6]=psbChess[k];
                                        p++;
                                        
                                        
                                        psbSndMsg[p]="0|up";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-5]=psbChess[k];
                                        p++;
                                        
                                        psbSndMsg[p]="0|left";
                                        psbSndMsg[p][0]=psbChess[k]+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-1]=psbChess[k];
                                        p++;
                                        
                                       
                                        
                                    }
                                }
                                else
                                {
                                    if(j==0)//up
                                    {
                                        
                                        psbSndMsg[p]="10|up";
                                        psbSndMsg[p][1]=psbChess[k]%10+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-5]=psbChess[k];
                                        p++;
                            
                                    }
                                    else if(i==0)//left
                                    {
                                        psbSndMsg[p]="10|left";
                                        psbSndMsg[p][1]=psbChess[k]%10+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-1]=psbChess[k];
                                        p++;
                                    }
                                    else//leftup
                                    {
                                        psbSndMsg[p]="10|leftup";
                                        psbSndMsg[p][1]=psbChess[k]%10+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-6]=psbChess[k];
                                        p++;
                                        
                                        psbSndMsg[p]="10|up";
                                        psbSndMsg[p][1]=psbChess[k]%10+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-5]=psbChess[k];
                                        p++;
                                        
                                        psbSndMsg[p]="10|left";
                                        psbSndMsg[p][1]=psbChess[k]%10+'0';
                                        psbChessBoard[p][5*i+j]=0;
                                        psbChessBoard[p][5*i+j-1]=psbChess[k];
                                        p++;
                                        
                                        
                                        
                                     
                                    }
                                    
                                }
                            }
                }
                
            }

            float E[psbSltNum];//me的期望值
            for(int i=0;i<psbSltNum;i++)
                E[i]=0;
            

            for(int i=0;i<psbSltNum;i++)//第i个 psbChessBoard
            {
                int lmt1,lmt2,player;//rival player
                if(dice<=6)//me: red   rival: blue
                {
                    lmt1=7;lmt2=12;player=2;
                }
                else
                {
                    lmt1=1;lmt2=6;player=1;
                }
                int totalPNum=roll_P_all(chessboard, lmt1,lmt2);//
                for(int j=0;j<25;j++)//第i个 psbChessBoard中的第j个棋子
                {
                    if(psbChessBoard[i][j]>=lmt1&&psbChessBoard[i][j]<=lmt2)//是rival方的棋子
                    {
                        int vtChessBoard_rival[25];//每一步之后 可能的棋盘
                        for(int k=0;k<25;k++)
                            vtChessBoard_rival[k]=psbChessBoard[i][k];
                        
                        float P=roll_P_pre(psbChessBoard[i], psbChessBoard[i][j]);
                        float score_selected=9999999;//该棋子下最优解（操作方向）
                        if(player==1)//rival red right down
                        {
                            if(j>=20)//right
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j+1]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
                            else if(j%5==4)//down
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j+5]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
                            else//rightdown
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j+6]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
        
                            float p=P*score_selected/totalPNum;
                            E[i]+=p;
                        }
                        else//rival blue left up
                        {
                            if(j<5)//left
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j-1]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
                            else if(j%5==0)//up
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j-5]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
                            else//leftup
                            {
                                vtChessBoard_rival[j]=0;
                                vtChessBoard_rival[j-6]=psbChessBoard[i][j];
                                float my_this_score=set_whole_value(vtChessBoard_rival, dice);
                                float this_score=my_this_score;
                                score_selected=score_selected<this_score?score_selected:this_score;
                            }
                            float p=P*score_selected/totalPNum;
                            
                            E[i]+=p;
                        }
                    }
                }
            }

            int maxSeq=0;
            for(int i=1;i<psbSltNum;i++)
                if(E[i]>E[maxSeq])
                    maxSeq=i;
   
            sndmsg=psbSndMsg[maxSeq];
            
            for(int i=0;i<25;i++)
                chessboard[i]=psbChessBoard[maxSeq][i];
   
            this->clientsocket.sendMsg(sndmsg.c_str());
            //判断输赢
            int bcnt=0,rcnt=0;

            for(int i=0;i<25;i++)
            {
                if(chessboard[i]>0&&chessboard[i]<=6)
                    rcnt++;
                if(chessboard[i]>=7&&chessboard[i]<=12)
                    bcnt++;
            }
            if(bcnt==0||(chessboard[24]>0&&chessboard[24]<=6))
                win=1;
            else if(rcnt==0||chessboard[0]>=7)
                win=2;
            else
                win=0;
  
            
            logging(sndmsg);

            return 1;//一局结束
        }
    }
}

int Einstein::logging(string s)
{
    //将对战记录依次输出到终端，并存储在list容器中
    // 能够在控制台上实时显示对战日志
    //格式: YYYY-MM-dd hh-mm-ss : content
    //一轮结束需要明确显示出对战结果及对战时长
    //handle中需要调用
    
    time_t now_time=time(0);
    tm* ltm=localtime(&now_time);
    int year=1900+ltm->tm_year;
    int month=ltm->tm_mon;
    int day=ltm->tm_mday;
    int hour=ltm->tm_hour;
    int min=ltm->tm_min;
    int sec=ltm->tm_sec;
    
    if(s=="done!")
        //传入的s是 R: int chessboard[25](red走之后的棋盘) , 3|left
        //共28个信息
    {
        cout<<"red wins "<<rwn<<" times"<<"  rate "<<rwn*100/tn<<endl;
        cout<<"blue wins "<<bwn<<" times"<<"  rate "<<bwn*100/tn<<endl;
        int timeSec=60*(min-min_start_all)+sec-sec_start_all;
        cout<<"time: "<<timeSec<<endl;//
        
        int m=timeSec/60;
        int s=timeSec-m*60;
        cout<<"time used: "<<m<<"minutes "<<s<<"seconds"<<endl;
    }
    else
    {
        //time
        cout<<year<<"-";
        if(month<10) cout<<"0";
        cout<<month<<"-";
        if(day<0) cout<<"0";
        cout<<day<<" ";
        if(hour<10) cout<<"0";
        cout<<hour<<"-";
        if(min<10) cout<<"0";
        cout<<min<<"-";
        if(sec<10) cout<<"0";
        cout<<sec<<" :";
        
        cout<<s<<endl;
    }
    
    //list
    logger.push_back(s);
    return 0;
}

int Einstein::writelog()
{
    //存储当前棋局信息到log中，用于复盘(用list)
    //YYYY-MM-dd-hh-mm-ss.log （时间为对战开始时间）//文件名
    
    //std::list<std::string> logger;
    //std::string filename; // log filename
    
    //char d[20]="GoldenGlobal"; char *s="View"; strcat(d,s);

    
    char y[]="2019";
    char mon[]="05";
    
    char d[10];
    d[0]=day_start_all/10+'0';
    d[1]=day_start_all%10+'0';
    d[2]='\0';
    
    char h[10];
    h[0]=hour_start_all/10+'0';
    h[1]=hour_start_all%10+'0';
    h[2]='\0';
    
    char min[10];
    min[0]=min_start_all/10+'0';
    min[1]=min_start_all%10+'0';
    min[2]='\0';
    
    char s[10];
    s[0]=sec_start_all/10+'0';
    s[1]=sec_start_all%10+'0';
    s[2]='\0';
    cout<<"s:"<<s<<endl;//

    strcat(y, "-");
    strcat(y, mon); strcat(y, "-");
    strcat(y, d); strcat(y, "-");
    strcat(y, h); strcat(y, "-");
    strcat(y, min); strcat(y, "-");
    strcat(y, s);
    strcat(y, ".log");
    


 //   cout<<"name: "<<y<<endl;//
    filename=y;
    cout<<filename<<endl;//
  
   // cout<<"filename: "<<filename<<endl;//
    
    ofstream outfile(filename,ofstream::app);
    for(list<string>::iterator it=logger.begin();it!=logger.end();++it)
        outfile<<*it<<endl;
    outfile.close();

    return 0;

}


int string_to_int(string s)
{
    int num=0;
    int k=s.length();
    if(k==1)
        num=s[0]-'0';
    else
        num=10*(s[0]-'0')+s[1]-'0';
    return num;
    
}

int choices_num(int chessboard[25],int dice)
{//1:dice  2:两个可能的选择  3:有小无大 4:有大无小
    for(int i=0;i<25;i++)
        if(chessboard[i]==dice)
            return 1;
    int cntS=0,cntL=0;
    int lmt1,lmt2;
    if(dice<=6)
    {
        lmt1=1;lmt2=6;
    }
    else
    {
        lmt1=7;lmt2=12;
    }
    for(int i=0;i<25;i++)
    {
        if(chessboard[i]>=lmt1&&chessboard[i]<dice)
            cntS++;
        if(chessboard[i]<=lmt2&&chessboard[i]>dice)
            cntL++;
    }
    if(cntS!=0&&cntL!=0)
        return 2;
    else if(cntL==0&&cntS!=0)
        return 3;
    else
        return 4;

}

int find_psbChess(int chessboard[25],int dice,int seq)//seq=1:up. seq=0: down
{
    
    int lmt1,lmt2;
    if(dice<=6)
    {
        lmt1=1;lmt2=6;
    }
    else
    {
        lmt1=7;lmt2=12;
    }
    if(seq==1)
        for(int k=dice+1;k<=lmt2;k++)
            for(int i=0;i<25;i++)
                if(chessboard[i]==k)
                    return k;
    if(seq==0)
        for(int k=dice-1;k>=lmt1;k--)
            for(int i=0;i<25;i++)
                if(chessboard[i]==k)
                    return k;
    return 0;
    
}

float set_single_value(int psbChessBoard[25],int dice)
{
    int realBoard[5][5];
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            realBoard[i][j]=psbChessBoard[5*i+j];
    
    int d=0,n=0;
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            if(realBoard[i][j]==dice)
            {
                if(dice<=6)
                    d=(4-i)>(4-j)?(4-i):(4-j);
                else
                    d=i>j?i:j;
                break;
            }
    if(dice<=6)
        for(int i=0;i<25;i++)
            if(psbChessBoard[i]>=1&&psbChessBoard[i]<=6)
                n++;
    if(dice>=7)
        for(int i=0;i<25;i++)
            if(psbChessBoard[i]>=7&&psbChessBoard[i]<=12)
                n++;
    
    int dv=0,nv=1;
    
    if(d==0) dv=10000;
    if(d==1) dv=500;
    if(d==2) dv=50;
    if(d==3) dv=10;
    
    if(n==1) nv=500;
    if(n==2) nv=20;
    if(n==3) nv=10;
    if(n==4) nv=5;
    if(n==5) nv=2;
    

    
    float value=nv+dv;
    return value;
}

float set_whole_value(int psbChessBoard[25],int dice)
{
    int lmt1,lmt2;
    if(dice<=6)
    {
        lmt1=1;lmt2=6;
    }
    else
    {
        lmt1=7;lmt2=12;
    }
    
    float value=0;
    for(int i=0;i<25;i++)
        if(psbChessBoard[i]>=lmt1&&psbChessBoard[i]<=lmt2)
            value+=set_single_value(psbChessBoard, dice);
    return value;
}

int roll_P_pre(int board[25],int Dice)//dice必然存活
{
    int n=0;
    for(int i=0;i<25;i++)
        if(board[i]==Dice)
        { n++;break;}
    for(int i=0;i<25;i++)
        if(find_psbChess(board, board[i], 1)==Dice||find_psbChess(board, board[i], 0)==Dice)
            n++;
    
    return n;
    
}

int roll_P_all(int board[25],int lmt1,int lmt2)
{
    int n=0;
    for(int i=0;i<25;i++)
        if(board[i]>=lmt1&&board[i]<=lmt2)
            n+=roll_P_pre(board, board[i]);
    return n;
}

