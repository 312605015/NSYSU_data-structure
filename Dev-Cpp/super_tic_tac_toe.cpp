#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <time.h>
#include <math.h>

#define SCREEN_HEIGHT 500 //設定遊戲視窗高度 
#define SCREEN_WIDTH 500 //設定遊戲視窗寬度
#define GRID_NUM 9 //設定遊戲方陣每邊格子數量 
#define LEFT_MARGINE 30 //設定左邊界 
#define TOP_MARGINE 45 //設定上邊界 
#define INIT_SPEED  80 //設定初始速度 
#define PLAYONE_COLOR  GREEN //設定玩家一顏色
#define PLAYTWO_COLOR  RED //設定玩家二顏色

//宣告棋子種類 
enum Chess {
	EMPTY, //空白 
	PLAY_ONE, //玩家一 
	PLAY_TWO //玩家二 
};

enum Mode {
	PLAYER, //玩家模式 
	AI_SINGLE, //玩家一, AI 玩家二 
	AI_BOTH //玩家一, 玩家二 都是AI 
};

//宣告儲存棋子位置的變數 
struct Location{
	int row; 
	int col;
};

//宣告紀錄格子號碼的節點結構 
struct Node {
	int cell; // 格子號碼
	struct Node *next;	//指向下一個節點 
};

//定義指向節點結構的指標變數 
typedef struct Node *NodePointer;

void openWindow(); //開啟游戲視窗 
void closeGame(); //處理遊戲結束邏輯 
void playGame(int field[][GRID_NUM]); //遊戲進行邏輯 
int getGlobalCellNum(Location loc); //依據下子位置來判斷該位置所屬大格子編號  
int getLocalCellNum(Location loc); //依據下子位置來判斷該位置所屬小格子編號  
Location* getLocByCell(int globalCellNum, int localCellNum); // 依據大小格子編號來計算座標  
NodePointer createcellState();  // 紀錄空閒格子
void removeCellNum(NodePointer cellState); //從閒置格子紀錄中移除小格子編號 
bool checkFreeCell(NodePointer cellState); //檢查是否還有空閒小格子  
bool checkEndGame(NodePointer cellState); //檢查是否到達結束遊戲的條件，沒有任何空閒小格子
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr); //取得玩家落子位置 
void setChess(int field[][GRID_NUM], Location chessLoc); //設定落子 
int countLinking(int field[][GRID_NUM], Location chessLoc); //判斷是否3子連線 
void drawChessboard(int field[][GRID_NUM]); //繪製遊戲區域 
void drawFocus(int field[][GRID_NUM], Location focus); //繪製目標十字 
void cleanFocus(int field[][GRID_NUM], Location focus);//清除十字 
void drawCircle(int row, int col, int color); //繪製圓圈  
void drawCross(int row, int col, int color); //繪製X 
void drawLink(Location src, Location dest); //繪製連線 
void showGameWinMsg(); //遊戲結束訊息
void showInfo(); //顯示遊戲相關資訊 
void printBoardState(int field[][GRID_NUM], NodePointer cellState); //印出棋盤狀態  
Location* PLAY_AI(int field[][GRID_NUM],  Location *focusPtr, NodePointer cellState);//實作電腦控制邏輯 

char key; //操作按鍵 
int speed; //移動速度 
int currentPlayer = PLAY_ONE; // 先手是玩家1 
int totalTime = 0; //紀錄遊戲時間 
int playMode = PLAYER; //設定初始遊戲模式為玩家手動下子 
int modeCount = 0; //紀錄設定遊戲模式的次數 
int globalCellNum; //超級井字遊戲將棋盤切割成9個大格子，從棋盤的左上到右下依序是0~8，限定玩家這回合可以下子的範圍，如果是-1則棋盤所有位置都可以下子 
int localCellNum; //一個大格子包含9個小格子，小格子編號一樣是從左上到右下為0~8 
int play1Score; //玩家1累積的連線數 
int play2Score; //玩家2累積的連線數  
int stepCount; //所有玩家累計的步數 
      
int main( )
{  	
	openWindow(); //開啟游戲視窗 
	while(TRUE){
		
		speed = INIT_SPEED; //初始化遊戲速度 
		srand((unsigned)time(NULL)); //設定亂數種子，這裡的亂數種子是依據電腦內部時間，確保短時間內每次遊戲的亂數種子都不一樣 
		
	   	int field[GRID_NUM][GRID_NUM] = {0}; //初始化棋盤  
	   	
		playGame(field); //進行遊戲
		if (key == 'q' || key == 'Q')
			closeGame(); //如果玩家輸入'q'離開遊戲	
		else if (key == 's' || key == 'S')
			continue; //如果玩家輸入's' 繼續遊戲 		    
	}
}

//開啟游戲視窗
void openWindow(){
	initwindow(SCREEN_WIDTH + LEFT_MARGINE * 3, SCREEN_HEIGHT + TOP_MARGINE * 3, "Backgammon");
}

//處理遊戲結束邏輯 
void closeGame(){
	exit(0);
}

//遊戲進行邏輯
void playGame(int field[][GRID_NUM]){
	
	drawChessboard(field); //繪製超級井字遊戲的棋盤
	//初始化遊戲會使用到的相關變數 
	Location focus = {0, 0};
	currentPlayer = PLAY_ONE;
	Location* chessLocPtr;
	globalCellNum = 0;
	localCellNum = 0;
	play1Score = 0;
	play2Score = 0;
	stepCount = 0;
	
	
	key = 0;
	NodePointer cellState = createcellState(); //初始化紀錄空閒格子的資料結構變數 
	printBoardState(field, cellState);
	
	while(true){
		showInfo(); //顯示時間和分數資訊
		
		//判斷目前遊戲模式，依據模式來設定控制邏輯，玩家模式代表玩家必須手動透過建盤來設定下子位置
		//AI模式則是由程式來決定下子位置 
		if(playMode == PLAYER){
			chessLocPtr = getChessDown(field, &focus);
		}else if(playMode == AI_SINGLE){
			if(currentPlayer == PLAY_ONE)
				chessLocPtr = getChessDown(field, &focus);
			else
				chessLocPtr = PLAY_AI(field, &focus, cellState);			
		}
		else { // playMode == AI_BOTH
			getChessDown(field, &focus);
			chessLocPtr = PLAY_AI(field, &focus, cellState);
		}
		
		//如果只是移動位置游標，則會pass，如果使用者按下space決定下子位置，或AI程式決定下子位置，則會進行下面的計算 
		if(chessLocPtr != NULL){
			setChess(field, *chessLocPtr); //在棋盤上繪製該玩家棋子，並記錄在棋盤矩陣上
			
			//如果是自由下子情況，計算下子位置所代表的大格子 
			if(globalCellNum == -1)
				globalCellNum = getGlobalCellNum(*chessLocPtr);
				
			//由下子的座標計算所代表的小格子編號	
			localCellNum = getLocalCellNum(*chessLocPtr);
			removeCellNum(cellState); //移除該位置的空閒狀態 
			
			//計算下子後，是否有任何3子連線，計算連線的數量，並累加到該玩家的分數上 
			if(currentPlayer == PLAY_ONE){
				play1Score += countLinking(field, *chessLocPtr);
				currentPlayer = PLAY_TWO;
			}else{
				play2Score += countLinking(field, *chessLocPtr);
				currentPlayer = PLAY_ONE;
			}			

			//依據超級井字遊戲規則，這一步可以下子的大格子是由，上一步對手所下的小格子決定， 
			//這一步所下的小格子，決定對手下一步可以下的大格子，
			//這裡依據目前這一步下子的小格子編號，來限制下一步可以下的大格子是什麼 
			globalCellNum = localCellNum;
			
			//判斷下一步的大格子是否還有可以下子的空閒小格子，如果沒有就開放自由模式，下一手的玩家可以自由下在任一個位置 
			NodePointer localNodeList = (cellState + globalCellNum) -> next;
			if(checkFreeCell(localNodeList) == false)
				globalCellNum = -1;	
			
			//印出棋盤狀態，用來debug使用 
			printBoardState(field, cellState);
			
			//判斷是否遊戲結束，如果沒有剩餘任何可以下子的位置，遊戲就結束 
			if(checkEndGame(cellState)){
				showGameWinMsg(); //顯示遊戲結束訊息，並等待玩家輸入選項
				return;				
			}
		}
		//只有輸入大小寫的a, q 和 s，系統才有反應並將輸入結果拋到外層等待處理 
		if (key == 'q' || key == 'Q' || key == 's' || key == 'S')
			return;				
		else if (key == 'a' || key == 'A'){ 
			//決定是否改變模式 ，主要有玩家模式和電腦操控的AI模式 
			modeCount++;
			playMode = modeCount % 3;
			key = 0;
		}
	
		delay(speed); //決定每回合下棋速度 			
	}		
}

//依據下子位置來判斷該位置所屬大格子編號 
int getGlobalCellNum(Location loc){
	return (loc.row / 3) * 3 + loc.col / 3;
}

//依據下子位置來判斷該位置所屬小格子編號 
int getLocalCellNum(Location loc){
	return loc.row % 3 * 3 + loc.col % 3;
}

//依據大小格子編號來計算座標 
Location* getLocByCell(int globalCellNum, int localCellNum){
	Location* locPtr;
	locPtr = (Location*)malloc(sizeof(Location));
	locPtr -> row =  (globalCellNum / 3) * 3 + localCellNum / 3;
	locPtr -> col = globalCellNum % 3 * 3 + localCellNum % 3;
	return locPtr;
}

// 紀錄空閒格子，是由一維陣列來代表大格子，
// 每一個陣列元素都維護該大格子所包含的空閒狀態的小格子，
// 小格子是由鏈結串列紀錄 
NodePointer createcellState(){
	NodePointer globalCellAray = (NodePointer) malloc(GRID_NUM * sizeof(Node));
	int i, j;
	for(i = 0; i < GRID_NUM; i++){
		NodePointer tail = NULL;

		globalCellAray[i].cell = i;
		globalCellAray[i].next = NULL;
		
		for(j = 0; j < GRID_NUM; j++){
			NodePointer localCell = (NodePointer) malloc(sizeof(Node)); 
			localCell -> cell = j;
			localCell -> next = NULL;
			if(j == 0){
				globalCellAray[i].next = localCell;				
			}else{
				tail -> next = localCell;
			}
			tail = localCell;
		}
	}
	
	return globalCellAray;
}

//從閒置格子紀錄中移除小格子編號 
void removeCellNum(NodePointer cellState){
	NodePointer localNode = NULL, temp = NULL;
	localNode = (cellState+globalCellNum) -> next;
	
	while(localNode != NULL){
		if(localNode -> cell == localCellNum){
			if(temp != NULL)
				temp -> next = localNode -> next;
			else
				(cellState+globalCellNum) -> next = localNode -> next;	
			free(localNode);
		}
		temp = localNode;
		localNode = localNode -> next;
	}
}

//檢查是否還有空閒小格子 
bool checkFreeCell(NodePointer localNodeList){
	int count = 0;
	while(localNodeList != NULL){
		localNodeList = localNodeList -> next;
		count++;
	}
	return count != 0;
}

//檢查是否到達結束遊戲的條件，沒有任何空閒小格子 
bool checkEndGame(NodePointer cellState){
	int i, count = 0;
	for(i = 0; i < GRID_NUM; i++){
		NodePointer localNode = (cellState+i)->next;		
		while(localNode != NULL){
			count ++;
			localNode = localNode -> next;	
		}
	}
	return count == 0;
}

//取得玩家落子位置
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr){
		
	char keyPress;
	Location* nextfocusPtr;
	Location temp;
	
	if(kbhit()) 
		keyPress = getch();
	
	// 如果玩家按下space就回傳目標框座標 
	if(keyPress == ' ' && field[focusPtr->row][focusPtr->col] == EMPTY){
		return focusPtr;
	}
		
	cleanFocus(field, *focusPtr);
	
	//如果目標框位置不符合大格子編號，就將目標框移到規定範圍，除非是自由模式 
	if(getGlobalCellNum(*focusPtr) != globalCellNum && globalCellNum != -1){
		nextfocusPtr = getLocByCell(globalCellNum, getLocalCellNum(*focusPtr));
		focusPtr -> row = nextfocusPtr -> row;
		focusPtr -> col = nextfocusPtr -> col;
	}
	
	temp.row = focusPtr -> row;
	temp.col = focusPtr -> col;
	
	//decide focus moving direction	
	switch(keyPress){
		case KEY_RIGHT:
			if(focusPtr->col < GRID_NUM - 1)
				temp.col = focusPtr->col + 1;
			break;			
		case KEY_LEFT:
			if(focusPtr->col > 0)
				temp.col = focusPtr->col - 1;
			break;		
		case KEY_UP:
			if(focusPtr->row > 0)
				temp.row = focusPtr->row - 1;
			break;				
		case KEY_DOWN:
			if(focusPtr->row < GRID_NUM - 1)
				temp.row = focusPtr->row + 1;
			break;				
	}
	
	//限制目標框只能在規定大格子範圍內移動，除非是自由模式 
	if(getGlobalCellNum(temp) == globalCellNum || globalCellNum == -1){
		focusPtr -> row = temp.row;
		focusPtr -> col = temp.col;
	}
	
	drawFocus(field, *focusPtr);
	//只有輸入大小寫的a, q 和 s，系統才有反應並將輸入結果拋到外層等待處理 
	if (keyPress == 'q' || keyPress == 'Q' || keyPress == 's' || keyPress == 'S' || keyPress == 'a' || keyPress == 'A'){
		key = keyPress;
	}
	return NULL;
}

//設定落子
void setChess(int field[][GRID_NUM], Location chessLoc){
	if(field[chessLoc.row][chessLoc.col] == EMPTY){
	
		field[chessLoc.row][chessLoc.col] = currentPlayer;
		if(currentPlayer == PLAY_ONE){
			drawCircle(chessLoc.row, chessLoc.col, PLAYONE_COLOR);
		}else{
			drawCross(chessLoc.row, chessLoc.col, PLAYTWO_COLOR);
		}
		stepCount++;
	}
}

//檢查是否有3子連線 
int countLinking(int field[][GRID_NUM], Location chessLoc){
	int i, count, total;
	int searchSize = 2;
	int linkingSize = 3;
	int row = chessLoc.row;
	int col = chessLoc.col;
	Location head, tail; 
	head = chessLoc;
	tail = chessLoc;
	int minRow = (globalCellNum / 3) * 3, maxRow = (globalCellNum / 3) * 3 + 2;
	int minCol = (globalCellNum % 3) * 3, maxCol = (globalCellNum % 3) * 3 + 2;
	
	total = 0;
	count = 1;
	// 垂直搜尋 
	for(i = 1; i <= searchSize ; i ++){
		if(row - i >= minRow && field[row-i][col] == currentPlayer){
			count ++;
			head.row = row - i;
			head.col = col;
		}
		else
			break;	
	}
	for(i = 1; i <= searchSize; i ++){
		if(row + i <= maxRow && field[row+i][col] == currentPlayer){
			count ++;
			tail.row = row + i;
			tail.col = col;
		}
		else
			break;
	}
	if(count == linkingSize)	
	{
		drawLink(head, tail);
		total ++;
	}
	
	// 水平搜尋
	head = chessLoc;
	tail = chessLoc;
	count = 1;
	for(i = 1; i <= searchSize; i ++){
		if(col - i >= minCol && field[row][col-i] == currentPlayer){
			count ++;
			head.row = row;
			head.col = col - i;			
		}
		else
			break;	
	}
	for(i = 1; i <= searchSize; i ++){
		if(col + i <= maxCol && field[row][col+i] == currentPlayer){
			count ++;
			tail.row = row;
			tail.col = col + i;			
		}
		else
			break;
	}
	if (count == linkingSize)	
	{
		drawLink(head, tail);
		total ++;
	}
	
	// 左上右下搜尋 
	count = 1;
	head = chessLoc;
	tail = chessLoc;	
	for(i = 1; i <= searchSize; i ++)
		if(row - i >= minRow && col - i >= minCol && field[row-i][col-i] == currentPlayer){
			count ++;
			head.row = row - i;
			head.col = col - i;			
		}
		else
			break;	
	for(i = 1; i <= searchSize; i ++)
		if(row + i <= maxRow && col + i <= maxCol && field[row+i][col+i] == currentPlayer){
			count ++;
			tail.row = row + i;
			tail.col = col + i;			
		}
		else
			break;
	if (count == linkingSize)	
	{
		drawLink(head, tail);
		total ++;
	}
	
	// 右上左下搜尋 
	count = 1;
	head = chessLoc;
	tail = chessLoc;	
	for(i = 1; i <= searchSize; i ++)
		if(row - i >= minRow && col + i <= maxCol && field[row-i][col+i] == currentPlayer){
			count ++;
			head.row = row - i;
			head.col = col + i;	
		}
		else
			break;	
	for(i = 1; i <= searchSize; i ++)
		if(row + i <= maxRow && col - i >= minCol && field[row+i][col-i] == currentPlayer){
			count ++;
			tail.row = row + i;
			tail.col = col - i;
		}
		else
			break;
	if (count == linkingSize)	
	{
		drawLink(head, tail);
		total ++;
	}	
		
	return total;		
}

//繪製遊戲區域，依據遊戲場矩陣設定繪製物件 
void drawChessboard(int field[][GRID_NUM]){
   	int row = 0, col = 0;
   	int startX = 0, startY = 0, endX = 0, endY = 0;
   	cleardevice(); //清理螢幕畫面 
   	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
	setcolor(YELLOW);
	for(row = 0; row <= GRID_NUM; row ++){
		setlinestyle(SOLID_LINE, 0, 1);
		if(row != 0 && row != GRID_NUM && row % 3 == 0)
			setlinestyle(SOLID_LINE, 0, 7);
		startX = LEFT_MARGINE + 0 * squareWidth + 0;
		startY =  TOP_MARGINE + row * squareHeight;
		endX = startX + GRID_NUM * squareWidth;
		line(startX, startY, endX, startY);
	}
	
	for(col = 0; col <=  GRID_NUM; col++){
		setlinestyle(SOLID_LINE, 0, 1);
		if(col != 0 && col != GRID_NUM && col % 3 == 0)
			setlinestyle(SOLID_LINE, 0, 7);
		startX = LEFT_MARGINE + col * squareWidth;
		startY =  TOP_MARGINE + 0 * squareHeight + 0;
		endY = startY + GRID_NUM * squareHeight;
		line(startX, startY, startX, endY);
	}
}

//繪製目標框 
void drawFocus(int field[][GRID_NUM], Location focus){
	Location* locPtr;
	int margin = 0;
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	int left = 0, right = 0, bottom = 0, top = 0;  	   
	   
	int row = focus.row, col = focus.col;
	left = LEFT_MARGINE + col * squareWidth + margin;
   	top =  TOP_MARGINE + row * squareHeight + margin;
	right = left + squareWidth - margin * 2;
   	bottom = top + squareHeight -margin * 2;
   	setcolor(MAGENTA);
	setlinestyle(SOLID_LINE, 0, 3); 
   	rectangle(left , top, right, bottom);	//設定繪製方塊的位置 
}

//清除目標框 
void cleanFocus(int field[][GRID_NUM], Location focus){
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	int margin = 0;
   	int row = focus.row, col = focus.col;
   	int left = 0, right = 0, bottom = 0, top = 0;
	left = LEFT_MARGINE + col * squareWidth + margin;
   	top =  TOP_MARGINE + row * squareHeight + margin;
	right = left + squareWidth - margin * 2;
   	bottom = top + squareHeight - margin * 2;
   	
   	setcolor(BLACK);
	setlinestyle(SOLID_LINE, 0, 3); 
   	rectangle(left , top, right, bottom);	//設定繪製方塊的位置 
   	
   	setcolor(YELLOW);
	setlinestyle(SOLID_LINE, 0, 1); 
   	rectangle(left , top, right, bottom);	//設定繪製方塊的位置 
}

//繪製圓圈 
void drawCircle(int row, int col, int color){
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int SquareWidth = SCREEN_WIDTH / GRID_NUM;
	int left = 0, right = 0, bottom = 0, top = 0;
	left = LEFT_MARGINE + col * SquareWidth;
   	top =  TOP_MARGINE + row * squareHeight;
	right = left + SquareWidth;
	bottom = top + squareHeight;
	int lr_center = (left + right)/2;
	int td_center = (top + bottom)/2;

	int r = 20;   	
   	setcolor(color);
   	setlinestyle(SOLID_LINE, 0, 5);
	circle(lr_center,td_center,r);
}

//繪製X 
void drawCross(int row, int col, int color){
	int crossX = 0, crossY = 0;
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	crossX = LEFT_MARGINE + col * squareWidth + squareWidth / 2;
   	crossY =  TOP_MARGINE + row * squareHeight + squareHeight / 2;
   	setlinestyle(SOLID_LINE, 0, 5); 
   	setcolor(color);
   	line(crossX -15, crossY - 15, crossX + 15, crossY + 15);
   	line(crossX + 15, crossY - 15, crossX - 15, crossY + 15);
}

//繪製連線 
void drawLink(Location src, Location dest){
	int srcX = 0, srcY = 0, destX = 0, destY = 0;
	int squareHeight = SCREEN_HEIGHT / GRID_NUM;
   	int squareWidth = SCREEN_WIDTH / GRID_NUM;
   	srcX = LEFT_MARGINE + src.col * squareWidth + squareWidth / 2;
   	srcY = TOP_MARGINE + src.row * squareHeight + squareHeight / 2;
	destX = LEFT_MARGINE + dest.col * squareWidth + squareWidth / 2;
   	destY = TOP_MARGINE + dest.row * squareHeight + squareHeight / 2;
   	setlinestyle(SOLID_LINE, 0, 2); 
   	setcolor(YELLOW);
   	line(srcX, srcY, destX, destY);
}

//遊戲結束訊息
void showGameWinMsg(){
   	//cleardevice(); //清理所有螢幕資料，如果希望只顯示訊息時，取消註解 
	int i = 0;
	char msg1[20] = "";
	if(play1Score == play2Score){
		strcat(msg1, "End In A Draw!!");
	}else if(play1Score > play2Score){
		strcat(msg1, "Player One Win!!");
	}else
		strcat(msg1, "Player Two Win!!");
			
	char msg2[40] = "press [q] to quit or [s] to restart!!";
   	for(; ; i++){
	   	setcolor(i%14);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 5);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);
	   	
	   	setcolor(WHITE);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 2);
	   	outtextxy(20, SCREEN_HEIGHT / 2 + 50, msg2);
	   	
	   	delay(200);
	   	
	   	setcolor(BLACK);
	   	settextstyle(TRIPLEX_FONT, HORIZ_DIR , 5);
	   	outtextxy(0, SCREEN_HEIGHT / 2, msg1);	   
		   	
	   	if(kbhit()) {
			key = getch();	
			if (key == 'q' || key == 'Q' || key == 's' || key == 'S') {
				return; 				
			}
		}
	}	
}

//顯示遊戲相關資訊
void showInfo(){
	totalTime += speed;
	char timeMsg[45] = " Time:";
	char modeMsg[20] = "";
	char optMsg1[50] = "press [q] to quit, [s] to restart or";
	char optMsg2[50] = "press [a] to change mode.";
	char optMsg3[50] = "press [space] to set chess.";
	char play1RoundMsg[20] = "PLAYER ONE ROUND";
	char play2RoundMsg[20] = "PLAYER TWO ROUND";
	char stepCountMsg[10] = "STEP:";
	
	char time[10];
	char step[10];
	char score1Msg[10];
	char score2Msg[10];
	int left, top;
	sprintf(time, "%5d", totalTime/1000);
	strcat(timeMsg, time);
	strcat(timeMsg, " sec.");
	
	sprintf(score1Msg, "%3d", play1Score);
	sprintf(score2Msg, "%3d", play2Score);
	sprintf(step, "%3d", stepCount);
	strcat(stepCountMsg, step);
	
	setcolor(WHITE); //設定文字顏色 
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1); //設定字型，水平或垂直和字型大小 
   	outtextxy(0, 0, timeMsg); //依據坐標輸出文字到螢幕
   	
   	if(currentPlayer == PLAY_ONE){
   		outtextxy(30, 20, play1RoundMsg);
		setfillstyle(SOLID_FILL,PLAYONE_COLOR);  	  
	}else{
		outtextxy(30, 20, play2RoundMsg); 
		setfillstyle(SOLID_FILL,PLAYTWO_COLOR);  
	}
	
	left = 5, top = 20;
   	bar(left , top, left + 20, top + 20);		
	   		
   	if(playMode == PLAYER){
   		strcat(modeMsg, "Player Mode      ");
   				
	}else if(playMode == AI_SINGLE){
   		strcat(modeMsg, "P1 player, P2 AI");    		
	}else
		strcat(modeMsg, "P1, P2 AI both   ");

   	setcolor(LIGHTMAGENTA);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(SCREEN_WIDTH - LEFT_MARGINE * 4, 0, modeMsg);
	
	setcolor(WHITE); //設定文字顏色
	left = LEFT_MARGINE * 2 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 3;
	top = 20;	      	
	outtextxy(left + 25, 20, stepCountMsg);	
	
	left = LEFT_MARGINE * 4 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 2;
	top = 20;
	setfillstyle(SOLID_FILL,PLAYONE_COLOR);
	bar(left , top, left + 20, top + 20);	      	
	outtextxy(left + 25, 20, score1Msg);
	
	left = LEFT_MARGINE * 4 + GRID_NUM * SCREEN_WIDTH / GRID_NUM / 2 + 100;
	setfillstyle(SOLID_FILL,PLAYTWO_COLOR);
	bar(left, top, left + 20, top + 20);	      	
	outtextxy(left + 25, 20, score2Msg);
   	
   	setcolor(LIGHTGREEN);
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1);
   	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM, optMsg1);
	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM + 20, optMsg2);	      	
	outtextxy(0, TOP_MARGINE + (GRID_NUM) * SCREEN_HEIGHT / GRID_NUM + 40, optMsg3);	      	
}

//印出棋盤狀態 
void printBoardState(int field[][GRID_NUM], NodePointer cellState){
	int row,col, i;
	
	for(row = 0; row < GRID_NUM; row++){
		if(row != 0 && row != GRID_NUM && row % 3 == 0)
			printf("-------------------\n");	
		for(col = 0; col < GRID_NUM; col++){
			if(col != 0 && col != GRID_NUM && col % 3 == 0)
				printf("|");
			printf("%d ", field[row][col]);
		}
		printf("\n");
	}
	printf("*******************\n");


	for(i = 0; i < GRID_NUM; i++){
		NodePointer localNode = NULL;
		printf("[%d]: ", (cellState+i)->cell);
		localNode = (cellState+i)->next;		
		while(localNode != NULL){
			if(localNode -> next == NULL)
				printf("%d", localNode -> cell);
			else
				printf("%d -> ", localNode -> cell);
			localNode = localNode -> next;
		}
		printf("\n");
	}
}

//電腦AI，請在此function實作AI功能 
Location* PLAY_AI(int field[][GRID_NUM],  Location *focusPtr, NodePointer cellState){
	cleanFocus(field, *focusPtr);
	
	
	return focusPtr;
}
