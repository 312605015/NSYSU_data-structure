#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <time.h>
#include <math.h>

#define SCREEN_HEIGHT 500 //�]�w�C���������� 
#define SCREEN_WIDTH 500 //�]�w�C�������e��
#define GRID_NUM 9 //�]�w�C����}�C���l�ƶq 
#define LEFT_MARGINE 30 //�]�w����� 
#define TOP_MARGINE 45 //�]�w�W��� 
#define INIT_SPEED  80 //�]�w��l�t�� 
#define PLAYONE_COLOR  GREEN //�]�w���a�@�C��
#define PLAYTWO_COLOR  RED //�]�w���a�G�C��

//�ŧi�Ѥl���� 
enum Chess {
	EMPTY, //�ť� 
	PLAY_ONE, //���a�@ 
	PLAY_TWO //���a�G 
};

enum Mode {
	PLAYER, //���a�Ҧ� 
	AI_SINGLE, //���a�@, AI ���a�G 
	AI_BOTH //���a�@, ���a�G ���OAI 
};

//�ŧi�x�s�Ѥl��m���ܼ� 
struct Location{
	int row; 
	int col;
};

//�ŧi������l���X���`�I���c 
struct Node {
	int cell; // ��l���X
	struct Node *next;	//���V�U�@�Ӹ`�I 
};

//�w�q���V�`�I���c�������ܼ� 
typedef struct Node *NodePointer;

void openWindow(); //�}�Ҵ������� 
void closeGame(); //�B�z�C�������޿� 
void playGame(int field[][GRID_NUM]); //�C���i���޿� 
int getGlobalCellNum(Location loc); //�̾ڤU�l��m�ӧP�_�Ӧ�m���ݤj��l�s��  
int getLocalCellNum(Location loc); //�̾ڤU�l��m�ӧP�_�Ӧ�m���ݤp��l�s��  
Location* getLocByCell(int globalCellNum, int localCellNum); // �̾ڤj�p��l�s���ӭp��y��  
NodePointer createcellState();  // �����Ŷ���l
void removeCellNum(NodePointer cellState); //�q���m��l�����������p��l�s�� 
bool checkFreeCell(NodePointer cellState); //�ˬd�O�_�٦��Ŷ��p��l  
bool checkEndGame(NodePointer cellState); //�ˬd�O�_��F�����C��������A�S������Ŷ��p��l
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr); //���o���a���l��m 
void setChess(int field[][GRID_NUM], Location chessLoc); //�]�w���l 
int countLinking(int field[][GRID_NUM], Location chessLoc); //�P�_�O�_3�l�s�u 
void drawChessboard(int field[][GRID_NUM]); //ø�s�C���ϰ� 
void drawFocus(int field[][GRID_NUM], Location focus); //ø�s�ؼФQ�r 
void cleanFocus(int field[][GRID_NUM], Location focus);//�M���Q�r 
void drawCircle(int row, int col, int color); //ø�s���  
void drawCross(int row, int col, int color); //ø�sX 
void drawLink(Location src, Location dest); //ø�s�s�u 
void showGameWinMsg(); //�C�������T��
void showInfo(); //��ܹC��������T 
void printBoardState(int field[][GRID_NUM], NodePointer cellState); //�L�X�ѽL���A  
Location* PLAY_AI(int field[][GRID_NUM],  Location *focusPtr, NodePointer cellState);//��@�q�������޿� 

char key; //�ާ@���� 
int speed; //���ʳt�� 
int currentPlayer = PLAY_ONE; // ����O���a1 
int totalTime = 0; //�����C���ɶ� 
int playMode = PLAYER; //�]�w��l�C���Ҧ������a��ʤU�l 
int modeCount = 0; //�����]�w�C���Ҧ������� 
int globalCellNum; //�W�Ť��r�C���N�ѽL���Φ�9�Ӥj��l�A�q�ѽL�����W��k�U�̧ǬO0~8�A���w���a�o�^�X�i�H�U�l���d��A�p�G�O-1�h�ѽL�Ҧ���m���i�H�U�l 
int localCellNum; //�@�Ӥj��l�]�t9�Ӥp��l�A�p��l�s���@�ˬO�q���W��k�U��0~8 
int play1Score; //���a1�ֿn���s�u�� 
int play2Score; //���a2�ֿn���s�u��  
int stepCount; //�Ҧ����a�֭p���B�� 
      
int main( )
{  	
	openWindow(); //�}�Ҵ������� 
	while(TRUE){
		
		speed = INIT_SPEED; //��l�ƹC���t�� 
		srand((unsigned)time(NULL)); //�]�w�üƺؤl�A�o�̪��üƺؤl�O�̾ڹq�������ɶ��A�T�O�u�ɶ����C���C�����üƺؤl�����@�� 
		
	   	int field[GRID_NUM][GRID_NUM] = {0}; //��l�ƴѽL  
	   	
		playGame(field); //�i��C��
		if (key == 'q' || key == 'Q')
			closeGame(); //�p�G���a��J'q'���}�C��	
		else if (key == 's' || key == 'S')
			continue; //�p�G���a��J's' �~��C�� 		    
	}
}

//�}�Ҵ�������
void openWindow(){
	initwindow(SCREEN_WIDTH + LEFT_MARGINE * 3, SCREEN_HEIGHT + TOP_MARGINE * 3, "Backgammon");
}

//�B�z�C�������޿� 
void closeGame(){
	exit(0);
}

//�C���i���޿�
void playGame(int field[][GRID_NUM]){
	
	drawChessboard(field); //ø�s�W�Ť��r�C�����ѽL
	//��l�ƹC���|�ϥΨ쪺�����ܼ� 
	Location focus = {0, 0};
	currentPlayer = PLAY_ONE;
	Location* chessLocPtr;
	globalCellNum = 0;
	localCellNum = 0;
	play1Score = 0;
	play2Score = 0;
	stepCount = 0;
	
	
	key = 0;
	NodePointer cellState = createcellState(); //��l�Ƭ����Ŷ���l����Ƶ��c�ܼ� 
	printBoardState(field, cellState);
	
	while(true){
		showInfo(); //��ܮɶ��M���Ƹ�T
		
		//�P�_�ثe�C���Ҧ��A�̾ڼҦ��ӳ]�w�����޿�A���a�Ҧ��N���a������ʳz�L�ؽL�ӳ]�w�U�l��m
		//AI�Ҧ��h�O�ѵ{���ӨM�w�U�l��m 
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
		
		//�p�G�u�O���ʦ�m��СA�h�|pass�A�p�G�ϥΪ̫��Uspace�M�w�U�l��m�A��AI�{���M�w�U�l��m�A�h�|�i��U�����p�� 
		if(chessLocPtr != NULL){
			setChess(field, *chessLocPtr); //�b�ѽL�Wø�s�Ӫ��a�Ѥl�A�ðO���b�ѽL�x�}�W
			
			//�p�G�O�ۥѤU�l���p�A�p��U�l��m�ҥN���j��l 
			if(globalCellNum == -1)
				globalCellNum = getGlobalCellNum(*chessLocPtr);
				
			//�ѤU�l���y�Эp��ҥN���p��l�s��	
			localCellNum = getLocalCellNum(*chessLocPtr);
			removeCellNum(cellState); //�����Ӧ�m���Ŷ����A 
			
			//�p��U�l��A�O�_������3�l�s�u�A�p��s�u���ƶq�A�ò֥[��Ӫ��a�����ƤW 
			if(currentPlayer == PLAY_ONE){
				play1Score += countLinking(field, *chessLocPtr);
				currentPlayer = PLAY_TWO;
			}else{
				play2Score += countLinking(field, *chessLocPtr);
				currentPlayer = PLAY_ONE;
			}			

			//�̾ڶW�Ť��r�C���W�h�A�o�@�B�i�H�U�l���j��l�O�ѡA�W�@�B���ҤU���p��l�M�w�A 
			//�o�@�B�ҤU���p��l�A�M�w���U�@�B�i�H�U���j��l�A
			//�o�̨̾ڥثe�o�@�B�U�l���p��l�s���A�ӭ���U�@�B�i�H�U���j��l�O���� 
			globalCellNum = localCellNum;
			
			//�P�_�U�@�B���j��l�O�_�٦��i�H�U�l���Ŷ��p��l�A�p�G�S���N�}��ۥѼҦ��A�U�@�⪺���a�i�H�ۥѤU�b���@�Ӧ�m 
			NodePointer localNodeList = (cellState + globalCellNum) -> next;
			if(checkFreeCell(localNodeList) == false)
				globalCellNum = -1;	
			
			//�L�X�ѽL���A�A�Ψ�debug�ϥ� 
			printBoardState(field, cellState);
			
			//�P�_�O�_�C�������A�p�G�S���Ѿl����i�H�U�l����m�A�C���N���� 
			if(checkEndGame(cellState)){
				showGameWinMsg(); //��ܹC�������T���A�õ��ݪ��a��J�ﶵ
				return;				
			}
		}
		//�u����J�j�p�g��a, q �M s�A�t�Τ~�������ñN��J���G�ߨ�~�h���ݳB�z 
		if (key == 'q' || key == 'Q' || key == 's' || key == 'S')
			return;				
		else if (key == 'a' || key == 'A'){ 
			//�M�w�O�_���ܼҦ� �A�D�n�����a�Ҧ��M�q���ޱ���AI�Ҧ� 
			modeCount++;
			playMode = modeCount % 3;
			key = 0;
		}
	
		delay(speed); //�M�w�C�^�X�U�ѳt�� 			
	}		
}

//�̾ڤU�l��m�ӧP�_�Ӧ�m���ݤj��l�s�� 
int getGlobalCellNum(Location loc){
	return (loc.row / 3) * 3 + loc.col / 3;
}

//�̾ڤU�l��m�ӧP�_�Ӧ�m���ݤp��l�s�� 
int getLocalCellNum(Location loc){
	return loc.row % 3 * 3 + loc.col % 3;
}

//�̾ڤj�p��l�s���ӭp��y�� 
Location* getLocByCell(int globalCellNum, int localCellNum){
	Location* locPtr;
	locPtr = (Location*)malloc(sizeof(Location));
	locPtr -> row =  (globalCellNum / 3) * 3 + localCellNum / 3;
	locPtr -> col = globalCellNum % 3 * 3 + localCellNum % 3;
	return locPtr;
}

// �����Ŷ���l�A�O�Ѥ@���}�C�ӥN��j��l�A
// �C�@�Ӱ}�C���������@�Ӥj��l�ҥ]�t���Ŷ����A���p��l�A
// �p��l�O���쵲��C���� 
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

//�q���m��l�����������p��l�s�� 
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

//�ˬd�O�_�٦��Ŷ��p��l 
bool checkFreeCell(NodePointer localNodeList){
	int count = 0;
	while(localNodeList != NULL){
		localNodeList = localNodeList -> next;
		count++;
	}
	return count != 0;
}

//�ˬd�O�_��F�����C��������A�S������Ŷ��p��l 
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

//���o���a���l��m
Location* getChessDown(int field[][GRID_NUM], Location *focusPtr){
		
	char keyPress;
	Location* nextfocusPtr;
	Location temp;
	
	if(kbhit()) 
		keyPress = getch();
	
	// �p�G���a���Uspace�N�^�ǥؼЮخy�� 
	if(keyPress == ' ' && field[focusPtr->row][focusPtr->col] == EMPTY){
		return focusPtr;
	}
		
	cleanFocus(field, *focusPtr);
	
	//�p�G�ؼЮئ�m���ŦX�j��l�s���A�N�N�ؼЮز���W�w�d��A���D�O�ۥѼҦ� 
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
	
	//����ؼЮإu��b�W�w�j��l�d�򤺲��ʡA���D�O�ۥѼҦ� 
	if(getGlobalCellNum(temp) == globalCellNum || globalCellNum == -1){
		focusPtr -> row = temp.row;
		focusPtr -> col = temp.col;
	}
	
	drawFocus(field, *focusPtr);
	//�u����J�j�p�g��a, q �M s�A�t�Τ~�������ñN��J���G�ߨ�~�h���ݳB�z 
	if (keyPress == 'q' || keyPress == 'Q' || keyPress == 's' || keyPress == 'S' || keyPress == 'a' || keyPress == 'A'){
		key = keyPress;
	}
	return NULL;
}

//�]�w���l
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

//�ˬd�O�_��3�l�s�u 
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
	// �����j�M 
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
	
	// �����j�M
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
	
	// ���W�k�U�j�M 
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
	
	// �k�W���U�j�M 
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

//ø�s�C���ϰ�A�̾ڹC�����x�}�]�wø�s���� 
void drawChessboard(int field[][GRID_NUM]){
   	int row = 0, col = 0;
   	int startX = 0, startY = 0, endX = 0, endY = 0;
   	cleardevice(); //�M�z�ù��e�� 
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

//ø�s�ؼЮ� 
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
   	rectangle(left , top, right, bottom);	//�]�wø�s�������m 
}

//�M���ؼЮ� 
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
   	rectangle(left , top, right, bottom);	//�]�wø�s�������m 
   	
   	setcolor(YELLOW);
	setlinestyle(SOLID_LINE, 0, 1); 
   	rectangle(left , top, right, bottom);	//�]�wø�s�������m 
}

//ø�s��� 
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

//ø�sX 
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

//ø�s�s�u 
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

//�C�������T��
void showGameWinMsg(){
   	//cleardevice(); //�M�z�Ҧ��ù���ơA�p�G�Ʊ�u��ܰT���ɡA�������� 
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

//��ܹC��������T
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
	
	setcolor(WHITE); //�]�w��r�C�� 
   	settextstyle(COMPLEX_FONT, HORIZ_DIR , 1); //�]�w�r���A�����Ϋ����M�r���j�p 
   	outtextxy(0, 0, timeMsg); //�̾ڧ��п�X��r��ù�
   	
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
	
	setcolor(WHITE); //�]�w��r�C��
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

//�L�X�ѽL���A 
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

//�q��AI�A�Цb��function��@AI�\�� 
Location* PLAY_AI(int field[][GRID_NUM],  Location *focusPtr, NodePointer cellState){
	cleanFocus(field, *focusPtr);
	
	
	return focusPtr;
}
