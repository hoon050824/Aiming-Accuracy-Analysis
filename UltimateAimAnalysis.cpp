/******************
*����: ���콺 ���� ��Ȯ�� �м� ���α׷�
*�ۼ���: 2113 ������ 
******************/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<conio.h>
#include<math.h>
//#include<direct.h> 

#define ARROW 224
#define UP 72
#define	DOWN 80
#define	LEFT 75
#define	RIGHT 77
#define ENTER 13
#define NAME_LEN 20
#define TXT_LEN (strlen(".txt"))
#define ANALYSE_LEN (strlen("_analyse.txt"))
#define RECORD_CNT 25
#define TIME_LIMIT 2500
#define PASSIVE -2.5
#define ACTIVE 2.5
#define X_CUSTOM 10
#define Y_CUSTOM 6
#define SET(x, y) gotoxy((displayWidth / 2) - X_CUSTOM + (x), (displayHeight / 2) - Y_CUSTOM + (y)) //Ŀ����ġ ���� 
//#define RECORDING ".\\record\\" //������� ����
//#define ANALYSE ".\\analyse\\" //�м����� ����

void menu();
void recordAim();
void detectClick(int* x, int* y, int* react_rate);
void analyseAim();
void userList();
void deleteUser();
void clearConsole();

void gotoxy(int x, int y);
void cursorView();

int displayWidth;
int displayHeight;

int main() {
	CONSOLE_SCREEN_BUFFER_INFO display;

	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &display);
	displayWidth = display.srWindow.Right - display.srWindow.Left + 1;
	displayHeight = display.srWindow.Bottom - display.srWindow.Top + 1;

	SetConsoleTitle(TEXT("Ultimate Aim Anaylsis"));
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT); //���콺 �Է� Ȱ��ȭ
	cursorView();
	//mkdir(RECORDING); //������� ��������
	//mkdir(ANALYSE); //�м����� ��������
	menu();

	return 0;
}

void menu() {
	int selec = 0;
	int key = -1;

	do { //�۾� ���� 
		SET(0, 0);
		printf("%c", selec == 0 ? '>' : ' ');
		printf(" 1. ���� ��Ȯ�� ���\n");
		
		SET(0, 1);
		printf("%c", selec == 1 ? '>' : ' ');
		printf(" 2. ���� ��Ȯ�� �м�\n");
		
		SET(0, 2);
		printf("%c", selec == 2 ? '>' : ' ');
		printf(" 3. ����� ���\n");
		
		SET(0, 3);
		printf("%c", selec == 3 ? '>' : ' ');
		printf(" 4. ����� ����\n");
		
		SET(0, 4);
		printf("%c", selec == 4 ? '>' : ' ');
		printf(" 5. ����\n");

		while (!_kbhit());
		key = _getch();

		if (key == ARROW) {
			switch (_getch()) {
			case UP:
			case LEFT: (selec > 0) ? selec-- : selec = 4; break;
			case DOWN:
			case RIGHT: (selec < 4) ? selec++ : selec = 0; break;
			}
		}

		if (key == ENTER) { //�۾� ����
			switch (selec) {
			case 0: recordAim();
				break;
			case 1: analyseAim();
				break;
			case 2: userList();
				break;
			case 3: deleteUser();
				break;
			case 4: 
			default: exit(0);
			}
		}

		clearConsole();
	} while (true);
}


void recordAim() { //���� ��Ȯ�� ���
	char name[NAME_LEN]; //����ڸ�
	char overlap_check[NAME_LEN]; //�ߺ� Ȯ�ο� �ӽ� ����
	char file_name[NAME_LEN + TXT_LEN]; //����� ���ϸ�
	char text[50];
	char temp[50];
	bool file_already_being = false;
	
	do { //����ڸ� �Է�
		fflush(stdin);
		SET(0, 6);
		printf("����ڸ� �Է�(���ĺ� ��ҹ��ڿ� ����)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));
	
	SET(0, 7);
	
	FILE* list_check = fopen("userList.txt", "a");
	if (list_check == NULL) { //(���ٸ�)userList.txt ���� 
		printf("������ �߻��߽��ϴ�!");
		Sleep(250);
		return;
	}
	fclose(list_check);

	list_check = fopen("userList.txt", "r");
	if (list_check == NULL) { 
		printf("������ �߻��߽��ϴ�!");
		Sleep(250);
		return;
	}

	while (!feof(list_check)) { //�ߺ�Ȯ��
		fgets(overlap_check, NAME_LEN, list_check);
		file_already_being = file_already_being || !strcmp(name, overlap_check);
	}
	fclose(list_check);

	FILE* list = fopen("userList.txt", "a");
	if (list == NULL) {
		printf("������ �߻��߽��ϴ�!");
		Sleep(250);
		return;
	}
	(file_already_being) ? NULL : fprintf(list, "%s", name); //(�ߺ����� �ʾҴٸ�) ����� ���
	(!(file_already_being) && strlen(name) >= 19) ? fprintf(list, "\n") : NULL; //(�ִ���̶��) ���ܿ� '\0'�߰� 
	fclose(list);

	sprintf(file_name, "%s", strtok(name, "\n")); //����� ���� �̸� ����	
	strncat(file_name, ".txt", TXT_LEN);

	FILE* record = fopen(file_name, "w");
	if (record == NULL) { //���� ��Ȯ�� ��� 
		printf("������ �߻��߽��ϴ�!");
		Sleep(250);
		return;
	}
	
	clearConsole();

	SET(-X_CUSTOM, -Y_CUSTOM);
	printf("3");
	Sleep(1000);

	SET(-X_CUSTOM, -Y_CUSTOM);
	printf("2");
	Sleep(1000);

	SET(-X_CUSTOM, -Y_CUSTOM);
	printf("1");
	Sleep(1000);

	srand(time(NULL));
	
	int target_x = -1;
	int target_y = -1;
	int clicked_x = -1;
	int clicked_y = -1;
	POINT mouse;
	int error_x_distance = -1;
	int error_y_distance = -1;
	double error_move_distance = -1;
	int react_rate = -1;

	for (int i = 0; i < RECORD_CNT; i++) {
		clearConsole(); //��ǥ�� ��ġ ����
		target_x = rand() % displayWidth;
		target_y = rand() % displayHeight;
		Sleep(125); 

		gotoxy(target_x, target_y);
		GetCursorPos(&mouse); //���콺 ��ġ ���
		printf("��"); //��ǥ�� ���

		detectClick(&clicked_x, &clicked_y, &react_rate); //Ŭ�� ����
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		error_x_distance = target_x - clicked_x; //�������
		error_y_distance = target_y - clicked_y; //��������
		error_move_distance = sqrt(pow((target_x - mouse.x), 2) + pow((target_y - mouse.y), 2)) - sqrt(pow((clicked_x - mouse.x), 2) + pow((clicked_y - mouse.y), 2)); //�����Ÿ� ����

		if (react_rate > TIME_LIMIT) { //�ʹ� �ʰ� �����ߴٸ�
			clearConsole(); SET(-5, 0);
			printf("�ʹ� �ʾ����ϴ�!"); Sleep(1250);
			i--; continue;
		}

		clearConsole(); //���
		sprintf(text, "%d", react_rate);
		sprintf(temp, " %d", error_x_distance);
		strcat(text, temp);
		sprintf(temp, " %d", error_y_distance);
		strcat(text, temp);
		sprintf(temp, " %.2lf", error_move_distance);
		strcat(text, temp);
		fprintf(record, "%s\n", text);
		//printf("%s", text);
	}

	fclose(record);
}

void detectClick(int* x, int* y, int* react_rate) { //Ŭ�� ����
	INPUT_RECORD event; //��ȣ�ۿ� ���
	DWORD dword; //
	int start_time = clock();
	int clicked_time = -1;

	while (true) {
		do {
			ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &event, 1, &dword); //���콺 �Է� ����
		} while (event.EventType != MOUSE_EVENT); //���콺�� �Էµ� ������
		clicked_time = clock();

		//printf("%d %d %d\n", start_time, clicked_time, clicked_time - start_time);

		if (event.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { //��Ŭ��
			*x = event.Event.MouseEvent.dwMousePosition.X;
			*y = event.Event.MouseEvent.dwMousePosition.Y;
			*react_rate = clicked_time - start_time;
			return;
		}
		else if (event.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) { //��Ŭ��
			*x = event.Event.MouseEvent.dwMousePosition.X;
			*y = event.Event.MouseEvent.dwMousePosition.Y;
			*react_rate = clicked_time - start_time;
			return;
		}
		else {
			continue;
		}
	}
}

void analyseAim() { //���� ��Ȯ�� �м�
	char name[NAME_LEN]; //����ڸ�
	char file_name[NAME_LEN + TXT_LEN]; //����� ���ϸ� 
	char analyse_file[NAME_LEN + ANALYSE_LEN]; //����� �м� ���ϸ� 
	char* temp;

	int react_rate = 0; //�����ӵ�
	int error_x_distance = 0; //�������
	int error_y_distance = 0; //��������
	double error_distance = 0; //��������
	double error_move_distance = 0; //�̵�����

	int intmp;
	double doutmp;
	int i = 0;
	
	do { //����ڸ� �Է�
		fflush(stdin);
		SET(0, 6);
		printf("����ڸ� �Է�(���ĺ� ��ҹ��ڿ� ����)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));

	sprintf(file_name, "%s", strtok(name, "\n")); //����� ���ϸ� ����
	strncat(file_name, ".txt", TXT_LEN);
	
	FILE* aim = fopen(file_name, "r");
	if (aim == NULL) {		
		SET(0, 7);
		printf("%s(��)��� ����ڴ� �������� �ʽ��ϴ�\n\n", name);
		SET(0, 9);
	} else{

		do {
			sprintf(analyse_file, "%s", strtok(name, "\n")); //����� �м� ���ϸ� ����
			strncat(analyse_file, "_analyse.txt", ANALYSE_LEN);
			
			fscanf(aim, "%d", &intmp);
			react_rate += intmp;
	
			fscanf(aim, "%d", &intmp);
			error_x_distance += fabs(intmp);
	
			fscanf(aim, "%d", &intmp);
			error_y_distance += fabs(intmp);
	
			fscanf(aim, "%lf", &doutmp);
			error_move_distance += doutmp;
	
			error_distance += sqrt(pow(error_x_distance, 2) + pow(error_y_distance, 2));
	
			i++;
		} while (!feof(aim));
	
		fclose(aim);
	
		FILE* analyse = fopen(analyse_file, "w");
		if (analyse == NULL) {
			printf("������ �߻��߽��ϴ�!");
			Sleep(250);
			return;
		}
	
		clearConsole();
	
		SET(-4, 0);
		printf("��� �����ӵ�: %.2lfms\n", (double)react_rate / i);
		fprintf(analyse, "��� �����ӵ�: %.2lfms\n", (double)react_rate / i);
	
		SET(-4, 1);
		printf("��� ����Ÿ� ����: %.2lf\n", (double)error_x_distance / i);
		fprintf(analyse, "��� ����Ÿ� ����: %.2lf\n", (double)error_x_distance / i);
	
		SET(-4, 2);
		printf("��� �����Ÿ� ����: %.2lf\n", (double)error_y_distance / i);
		fprintf(analyse, "��� �����Ÿ� ����: %.2lf\n", (double)error_y_distance / i);
	
		SET(-4, 3);
		printf("��� �����Ÿ� ����: %.2lf\n", error_distance / i);
		fprintf(analyse, "��� �����Ÿ� ����: %.2lf\n", error_distance / i);
	
		SET(-4, 4);
		printf(((double)react_rate / i < 750) ? ((error_move_distance / i) < PASSIVE) ? "\"���� �� �����ϰ� ���������� �͵� ���� �� ���׿�\"" : (((error_move_distance / i) > ACTIVE) ? "\"���� �� �����ϰ� ���������°͵� ���� �� ���׿�\"" : "\"����� ��Ȯ�� ���ؽǷ��� ���� ��ñ���!\"") : "\"���� �� ������ ������ �ʿ䰡 �־�Դϴ�\"");
		
		//printf("%d %lf %lf lf", react_rate, error_x_istance, error_y_distance, error_move_distance);
		fclose(analyse);

		SET(-4, 7);
	}

	printf("�ƹ�Ű�� ���� ����ȭ������ ���ư���(EnterKey ����)");
	while (getch() == ENTER);

	return;
}

void userList() {
	char name[NAME_LEN]; //����ڸ�
	int i = -1;

	FILE* list = fopen("userList.txt", "r");
	if (list == NULL) {
		printf("������ �߻��߽��ϴ�!");
		Sleep(250);
		return;
	}
	
	clearConsole();
	
	SET(0, -2);
	printf("�������������� ����� ��� ��������������");
	while (!feof(list)) {
		SET(0, i);
		fgets(name, NAME_LEN, list);
		printf("%s", name);

		sprintf(name, "");
		i++;
	}
	SET(0, i);
	printf("������������������������������������������������������");
	
	fclose(list);

	SET(0, i + 2);
	printf("�ƹ�Ű�� ���� ����ȭ������ ���ư���(EnterKey ����)");
	while (getch() == ENTER);
	
	return;
}

void deleteUser() { //����� ����
	char name[NAME_LEN]; //����ڸ� 
	char file_name[NAME_LEN + TXT_LEN]; //����� ���ϸ� 
	char analyse_file[NAME_LEN + ANALYSE_LEN]; //����� �м� ���ϸ� 
	char check_name[NAME_LEN];
	char* temp;
	int i = 0;

	do { //����ڸ� �Է� 
		fflush(stdin);
		SET(0, 6);
		printf("����ڸ� �Է�(���ĺ� ��ҹ��ڿ� ����)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));
	
	sprintf(file_name, "%s", strtok(name, "\n")); //����� ���ϸ� ���� 
	strncat(file_name, ".txt", TXT_LEN);
	
	FILE* check = fopen(file_name, "r");
	FILE* tmp;
	
	if(check == NULL){		
		SET(0, 7);
		printf("%s(��)��� ����ڴ� �������� �ʽ��ϴ�\n\n", name);
		SET(0, 9);
	} else {
		fclose(check);
		
		sprintf(analyse_file, "%s", strtok(name, "\n")); //����� �м� ���ϸ� ���� 
		strncat(analyse_file, "_analyse.txt", ANALYSE_LEN);
		
		check = fopen("userList.txt", "r");
		tmp = fopen("temp.txt", "w");
		
		(strlen(name) >= 19) ? NULL : strncat(name, "\n", 1);		
		
		while(!feof(check)){
			fgets(check_name, NAME_LEN, check); 
			(strcmp(check_name, name)) ? fprintf(tmp, "%s", check_name) : NULL;
			
			sprintf(check_name, "");
		}
	
		fclose(check);
		fclose(tmp);
		
		check = fopen("userList.txt", "w");
		tmp = fopen("temp.txt", "r");
		
		while(!feof(tmp)){
			fgets(check_name, NAME_LEN, tmp);
			
			(strcmp(check_name, "\n")) ? fprintf(check, "%s", check_name) : NULL;
			(strlen(check_name) >= 19) ? fprintf(check, "\n") : NULL;
			
			sprintf(check_name, "");
		}
	
		fclose(check);
		fclose(tmp);
		
		remove(file_name);
		remove(analyse_file);
		
		userList();
	}
	
	printf("�ƹ�Ű�� ���� ����ȭ������ ���ư���(EnterKey ����)");
	while (getch() == ENTER);

	return;
}

void clearConsole() {
	COORD zeropoint = {0, 0};
	DWORD temp; //���� �ֻ�ܿ��� ���� ���ϴܱ��� ���鹮�� ��� (== ȭ�� �����) 
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', displayWidth * displayHeight, zeropoint, &temp);
}

void gotoxy(int x, int y) {
	COORD pos = {x, y}; //Ŀ����ġ �̵� 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursorView() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false; //Ŀ�� ����
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
