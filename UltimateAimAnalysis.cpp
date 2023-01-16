/******************
*제목: 마우스 조준 정확성 분석 프로그램
*작성자: 2113 이지훈 
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
#define SET(x, y) gotoxy((displayWidth / 2) - X_CUSTOM + (x), (displayHeight / 2) - Y_CUSTOM + (y)) //커서위치 설정 
//#define RECORDING ".\\record\\" //기록파일 폴더
//#define ANALYSE ".\\analyse\\" //분석파일 폴더

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
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT); //마우스 입력 활성화
	cursorView();
	//mkdir(RECORDING); //기록파일 폴더생성
	//mkdir(ANALYSE); //분석파일 폴더생성
	menu();

	return 0;
}

void menu() {
	int selec = 0;
	int key = -1;

	do { //작업 선택 
		SET(0, 0);
		printf("%c", selec == 0 ? '>' : ' ');
		printf(" 1. 조준 정확성 기록\n");
		
		SET(0, 1);
		printf("%c", selec == 1 ? '>' : ' ');
		printf(" 2. 조준 정확성 분석\n");
		
		SET(0, 2);
		printf("%c", selec == 2 ? '>' : ' ');
		printf(" 3. 사용자 목록\n");
		
		SET(0, 3);
		printf("%c", selec == 3 ? '>' : ' ');
		printf(" 4. 사용자 삭제\n");
		
		SET(0, 4);
		printf("%c", selec == 4 ? '>' : ' ');
		printf(" 5. 종료\n");

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

		if (key == ENTER) { //작업 수행
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


void recordAim() { //조준 정확도 기록
	char name[NAME_LEN]; //사용자명
	char overlap_check[NAME_LEN]; //중복 확인용 임시 변수
	char file_name[NAME_LEN + TXT_LEN]; //사용자 파일명
	char text[50];
	char temp[50];
	bool file_already_being = false;
	
	do { //사용자명 입력
		fflush(stdin);
		SET(0, 6);
		printf("사용자명 입력(알파벳 대소문자와 숫자)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));
	
	SET(0, 7);
	
	FILE* list_check = fopen("userList.txt", "a");
	if (list_check == NULL) { //(없다면)userList.txt 생성 
		printf("오류가 발생했습니다!");
		Sleep(250);
		return;
	}
	fclose(list_check);

	list_check = fopen("userList.txt", "r");
	if (list_check == NULL) { 
		printf("오류가 발생했습니다!");
		Sleep(250);
		return;
	}

	while (!feof(list_check)) { //중복확인
		fgets(overlap_check, NAME_LEN, list_check);
		file_already_being = file_already_being || !strcmp(name, overlap_check);
	}
	fclose(list_check);

	FILE* list = fopen("userList.txt", "a");
	if (list == NULL) {
		printf("오류가 발생했습니다!");
		Sleep(250);
		return;
	}
	(file_already_being) ? NULL : fprintf(list, "%s", name); //(중복되지 않았다면) 사용자 기록
	(!(file_already_being) && strlen(name) >= 19) ? fprintf(list, "\n") : NULL; //(최대길이라면) 말단에 '\0'추가 
	fclose(list);

	sprintf(file_name, "%s", strtok(name, "\n")); //사용자 파일 이름 설정	
	strncat(file_name, ".txt", TXT_LEN);

	FILE* record = fopen(file_name, "w");
	if (record == NULL) { //조준 정확성 기록 
		printf("오류가 발생했습니다!");
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
		clearConsole(); //목표물 위치 설정
		target_x = rand() % displayWidth;
		target_y = rand() % displayHeight;
		Sleep(125); 

		gotoxy(target_x, target_y);
		GetCursorPos(&mouse); //마우스 위치 기록
		printf("●"); //목표물 출력

		detectClick(&clicked_x, &clicked_y, &react_rate); //클릭 감지
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		error_x_distance = target_x - clicked_x; //수평오차
		error_y_distance = target_y - clicked_y; //수직오차
		error_move_distance = sqrt(pow((target_x - mouse.x), 2) + pow((target_y - mouse.y), 2)) - sqrt(pow((clicked_x - mouse.x), 2) + pow((clicked_y - mouse.y), 2)); //직선거리 오차

		if (react_rate > TIME_LIMIT) { //너무 늦게 반응했다면
			clearConsole(); SET(-5, 0);
			printf("너무 늦었습니다!"); Sleep(1250);
			i--; continue;
		}

		clearConsole(); //기록
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

void detectClick(int* x, int* y, int* react_rate) { //클릭 감지
	INPUT_RECORD event; //상호작용 기록
	DWORD dword; //
	int start_time = clock();
	int clicked_time = -1;

	while (true) {
		do {
			ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &event, 1, &dword); //마우스 입력 감지
		} while (event.EventType != MOUSE_EVENT); //마우스가 입력될 때까지
		clicked_time = clock();

		//printf("%d %d %d\n", start_time, clicked_time, clicked_time - start_time);

		if (event.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { //좌클릭
			*x = event.Event.MouseEvent.dwMousePosition.X;
			*y = event.Event.MouseEvent.dwMousePosition.Y;
			*react_rate = clicked_time - start_time;
			return;
		}
		else if (event.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) { //우클릭
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

void analyseAim() { //조준 정확도 분석
	char name[NAME_LEN]; //사용자명
	char file_name[NAME_LEN + TXT_LEN]; //사용자 파일명 
	char analyse_file[NAME_LEN + ANALYSE_LEN]; //사용자 분석 파일명 
	char* temp;

	int react_rate = 0; //반응속도
	int error_x_distance = 0; //수평오차
	int error_y_distance = 0; //수직오차
	double error_distance = 0; //직선오차
	double error_move_distance = 0; //이동오차

	int intmp;
	double doutmp;
	int i = 0;
	
	do { //사용자명 입력
		fflush(stdin);
		SET(0, 6);
		printf("사용자명 입력(알파벳 대소문자와 숫자)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));

	sprintf(file_name, "%s", strtok(name, "\n")); //사용자 파일명 설정
	strncat(file_name, ".txt", TXT_LEN);
	
	FILE* aim = fopen(file_name, "r");
	if (aim == NULL) {		
		SET(0, 7);
		printf("%s(이)라는 사용자는 존재하지 않습니다\n\n", name);
		SET(0, 9);
	} else{

		do {
			sprintf(analyse_file, "%s", strtok(name, "\n")); //사용자 분석 파일명 설정
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
			printf("오류가 발생했습니다!");
			Sleep(250);
			return;
		}
	
		clearConsole();
	
		SET(-4, 0);
		printf("평균 반응속도: %.2lfms\n", (double)react_rate / i);
		fprintf(analyse, "평균 반응속도: %.2lfms\n", (double)react_rate / i);
	
		SET(-4, 1);
		printf("평균 수평거리 오차: %.2lf\n", (double)error_x_distance / i);
		fprintf(analyse, "평균 수평거리 오차: %.2lf\n", (double)error_x_distance / i);
	
		SET(-4, 2);
		printf("평균 수직거리 오차: %.2lf\n", (double)error_y_distance / i);
		fprintf(analyse, "평균 수직거리 오차: %.2lf\n", (double)error_y_distance / i);
	
		SET(-4, 3);
		printf("평균 직선거리 오차: %.2lf\n", error_distance / i);
		fprintf(analyse, "평균 직선거리 오차: %.2lf\n", error_distance / i);
	
		SET(-4, 4);
		printf(((double)react_rate / i < 750) ? ((error_move_distance / i) < PASSIVE) ? "\"조금 더 과감하게 움직여보는 것도 좋을 것 같네요\"" : (((error_move_distance / i) > ACTIVE) ? "\"조금 더 신중하게 움직여보는것도 좋을 것 같네요\"" : "\"상당히 정확한 조준실력을 갖고 계시군요!\"") : "\"조금 더 빠르게 반응할 필요가 있어보입니다\"");
		
		//printf("%d %lf %lf lf", react_rate, error_x_istance, error_y_distance, error_move_distance);
		fclose(analyse);

		SET(-4, 7);
	}

	printf("아무키나 눌러 시작화면으로 돌아가기(EnterKey 제외)");
	while (getch() == ENTER);

	return;
}

void userList() {
	char name[NAME_LEN]; //사용자명
	int i = -1;

	FILE* list = fopen("userList.txt", "r");
	if (list == NULL) {
		printf("오류가 발생했습니다!");
		Sleep(250);
		return;
	}
	
	clearConsole();
	
	SET(0, -2);
	printf("─────── 사용자 명단 ───────");
	while (!feof(list)) {
		SET(0, i);
		fgets(name, NAME_LEN, list);
		printf("%s", name);

		sprintf(name, "");
		i++;
	}
	SET(0, i);
	printf("───────────────────────────");
	
	fclose(list);

	SET(0, i + 2);
	printf("아무키나 눌러 시작화면으로 돌아가기(EnterKey 제외)");
	while (getch() == ENTER);
	
	return;
}

void deleteUser() { //사용자 삭제
	char name[NAME_LEN]; //사용자명 
	char file_name[NAME_LEN + TXT_LEN]; //사용자 파일명 
	char analyse_file[NAME_LEN + ANALYSE_LEN]; //사용자 분석 파일명 
	char check_name[NAME_LEN];
	char* temp;
	int i = 0;

	do { //사용자명 입력 
		fflush(stdin);
		SET(0, 6);
		printf("사용자명 입력(알파벳 대소문자와 숫자)> ");
		fgets(name, NAME_LEN, stdin);
	} while (!strcmp(name, "\n"));
	
	sprintf(file_name, "%s", strtok(name, "\n")); //사용자 파일명 설정 
	strncat(file_name, ".txt", TXT_LEN);
	
	FILE* check = fopen(file_name, "r");
	FILE* tmp;
	
	if(check == NULL){		
		SET(0, 7);
		printf("%s(이)라는 사용자는 존재하지 않습니다\n\n", name);
		SET(0, 9);
	} else {
		fclose(check);
		
		sprintf(analyse_file, "%s", strtok(name, "\n")); //사용자 분석 파일명 설정 
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
	
	printf("아무키나 눌러 시작화면으로 돌아가기(EnterKey 제외)");
	while (getch() == ENTER);

	return;
}

void clearConsole() {
	COORD zeropoint = {0, 0};
	DWORD temp; //좌측 최상단에서 우측 최하단까지 공백문자 출력 (== 화면 지우기) 
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', displayWidth * displayHeight, zeropoint, &temp);
}

void gotoxy(int x, int y) {
	COORD pos = {x, y}; //커서위치 이동 
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursorView() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false; //커서 숨김
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
