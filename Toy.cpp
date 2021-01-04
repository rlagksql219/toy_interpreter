#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>

#define BUFFER_SIZE 1024

using namespace std;

typedef struct TreeNode { /* 트리 노드 구조체 */
	string data;
	TreeNode* parent;
	TreeNode* left;
	TreeNode* right;
} Node;


void file_load(); /* 파일을 로드하여 결과값 계산 */
void interactive_mode(); /* 문장을 입력받아 결과값 계산 */

vector<string> make_token(char* read_line, int n); /* 계산할 문장을 숫자와 연산자로 나누어 토큰 형태로 저장 */

Node* make_tree(vector<string> expr); /* 토큰으로 수식트리 생성 */
void InitNode(Node* node); /* 트리 노드 초기화 */

int Evaluate_expressionTree(Node* node); /* 수식트리를 postorder 방식으로 순회하여 결과값 계산 */
int value(string op, int op1, int op2); /* TOY Language 연산자에 따른 계산 */

int use_array_error_check(char* read_line); /* 문법에 맞게 문장이 입력되었는지 확인 */
int use_vector_error_check(char* read_line, vector<string> expr); /* 문법에 맞게 문장이 입력되었는지 확인 */


int main()
{
	int num;	
	
	while (1) {

		printf("==================================\n");
		printf("1. File Load\n");
		printf("2. Interactive Mode\n");
		printf("3. Exit\n");
		printf("==================================\n");

		printf("메뉴를 선택하세요 >> ");
		scanf_s("%d", &num);

		if (num == 1) {
			file_load();
		}

		if (num == 2) {
			interactive_mode();
		}

		if (num == 3) {
			printf("\n프로그램을 종료합니다.\n");
			return 0;
		}
	}
}


/**
함 수 : file_load()
기 능 : 파일을 로드하여 내용을 출력하고 결과값 계산
*/
void file_load()
{

	char fname[200];
	FILE* fp;
	char read_line[BUFFER_SIZE] = {0};
	
	vector<string> expr;
	Node* root;
	
	vector<int> eval;


	printf("\n파일명을 입력하세요 >> ");
	scanf_s("%s", fname, sizeof(fname));

	fopen_s(&fp, fname, "r");

	if (fp == NULL)
	{
		perror("입력한 파일명이 폴더 내에 존재하지 않습니다");
		
		printf("\n파일명을 입력하세요 >> ");
		scanf_s("%s", fname, sizeof(fname));
		
		fopen_s(&fp, fname, "r");
	}

	printf("\n파일 내용은\n");
	printf("- - - - - - - - - - - - - - - - - \n");

	while (!feof(fp))
	{
		fgets(read_line, BUFFER_SIZE, fp);
		printf("%s", read_line);

		expr = make_token(read_line, sizeof(read_line));
		root = make_tree(expr);
		eval.push_back(Evaluate_expressionTree(root));

	}

	printf("\n- - - - - - - - - - - - - - - - - \n");
	printf("입니다.\n");
	printf("\n");

	for (int i = 0; i<eval.size(); i++) {
		printf("결과 : %d\n", eval[i]);
	}
	printf("\n");

	fclose(fp);
}


/**
함 수 : interactive_mode()
기 능 : 계산할 문장을 입력받아서 결과값 계산
*/
void interactive_mode()
{
	int exist_error = 0;

	char read_line[BUFFER_SIZE] = { 0 };

	vector<string> expr;
	Node* root;

	int eval = 0;


	printf("\n문장을 입력하세요.\n");
	printf("- - - - - - - - - - - - - - - - - \n");

	getchar(); //메뉴 고를 때, 입력했던 개행문자 읽기
	scanf_s("%[^\n]s", read_line, BUFFER_SIZE); //공백 포함해서 문자열 입력받기
	
	exist_error = use_array_error_check(read_line);
	if (exist_error == 1)
		return;

	expr = make_token(read_line, sizeof(read_line));

	exist_error = use_vector_error_check(read_line, expr);
	if (exist_error == 1)
		return;

	root = make_tree(expr);
	eval = Evaluate_expressionTree(root);

	printf("\n- - - - - - - - - - - - - - - - - \n");
	printf("결과 : %d\n", eval);
	printf("- - - - - - - - - - - - - - - - - \n");
}


/**
함 수 : make_token(char* read_line, int n)
기 능 : 계산할 문장을 저장한 배열에서 괄호를 제외하고, 숫자와 연산자로 나누어 토큰 형태로 저장
*/
vector<string> make_token(char* read_line, int n) 
{
	
	vector<string> expr;
	vector<char> tmp;
	char rm_bracket[BUFFER_SIZE] = { 0 };

	for (int i = 0; i < n; i++) {
		if (read_line[i] == '(')
			continue;
		if (read_line[i] == ')')
			continue;
		if (read_line[i] == '\0')
			break;

		tmp.push_back(read_line[i]);
	}


	for (int i = 0; i < tmp.size(); i++)
		rm_bracket[i] = tmp[i];

	char* context = NULL;
	char* token = strtok_s(rm_bracket, " ", &context);

	while (token != NULL) {
		expr.push_back(token);
		token = strtok_s(NULL, " ", &context);
	}

	return expr;
}


/**
함 수 : make_tree(vector<string> expr)
기 능 : 숫자와 연산자로 나누어 저장된 토큰으로 수식트리 생성
*/
Node* make_tree(vector<string> expr) 
{

	Node* root = new Node;
	Node* cur;
	Node* insert;
	
	InitNode(root);

	root->data = expr.front();
	
	cur = root;
	
	for (int i = 1; i < expr.size(); i++) {

		/* 이전 토큰이 연산자인 경우 */
		if (expr[i - 1] == "MINUS" || expr[i - 1] == "IF") {
			if (cur->right != NULL) 
				cur = cur->right;


			if (cur->left == NULL) { 
				insert = new Node;
				InitNode(insert);

				cur->left = insert;
				insert->parent = cur;
				cur = cur->left;
				cur->data = expr[i];
			}
			else { 
				cur = cur->parent;
				insert = new Node;
				InitNode(insert);

				cur->right = insert;
				insert->parent = cur;
				cur = cur->right;
				cur->data = expr[i];
			}
		}

		/* 이전 토큰이 숫자인 경우 */
		else {
			cur = cur->parent;
			
			if (cur->right == NULL) {
				insert = new Node;
				InitNode(insert);

				cur->right = insert;
				insert->parent = cur;
				cur->right->data = expr[i];
			}
		}
	}

	while (cur->parent != NULL)
		cur = cur->parent;

	return cur;
}


/**
함 수 : InitNode(Node* node) 
기 능 : 트리의 노드 초기화
*/
void InitNode(Node* node) 
{
	node->data = '\0';
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
}


/**
함 수 : Evaluate_expressionTree(Node* node)
기 능 : 수식트리를 postorder 방식으로 순회하여 결과값 계산
*/
int Evaluate_expressionTree(Node* node)
{
	Node* left = node->left;
	Node* right = node->right;
	int answer;

	if (left == NULL && right == NULL) {
		return stoi(node->data);
	}

	int op1 = Evaluate_expressionTree(left);
	int op2 = Evaluate_expressionTree(right);
	string op = node->data;

	answer = value(op, op1, op2);
	return answer;
}


/**
함 수 : value(string op, int op1, int op2)
기 능 : TOY Language 연산자에 따른 계산
*/
int value(string op, int op1, int op2)
{

	if (op == "MINUS") {
		return op1 - op2;
	}


	if (op == "IF") {
		if (op1 > 0)
			return op2;

		else
			return 0;
	}
}



/**
함 수 : use_array_error_check(char* read_line)
기 능 : 문법에 맞게 문장이 입력되었는지 확인
*/
int use_array_error_check(char* read_line)
{
	/* "(", ")"가 잘못 사용 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (read_line[0] != '(') {
			printf("'('의 위치가 잘못되었습니다.\n");
			return 1;
		}

		if (read_line[i] == '\0') {
			if (read_line[i - 1] != ')') {
				printf("')'의 위치가 잘못되었습니다.\n");
				return 1;
			}
		}
	}
	
	
	/* 알파벳과 "-"를 제외한 각종 기호 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		char tmp = read_line[i];
		char a[100] = "-MINUSF() ";

		if (tmp == '\0')
			break;

		if (isdigit(tmp) == 0) { //숫자 제외
					
			if (strchr(a, tmp) == NULL) {
				printf("알파벳과 '-'를 제외한 각종 기호가 입력되었습니다.\n");
				return 1;
			}
		}
	}
	
	
	/* "-" 여러번 연속으로 입력 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		char tmp1 = read_line[i];
		char tmp2 = read_line[i + 1];

		if (tmp1 == '-' && tmp2 == '-') {
			printf("숫자가 입력되어야 할 자리에 '-'가 여러 번 연속으로 입력되었습니다.\n");
			return 1;
		}
	}
}


/**
함 수 : use_vector_error_check(char* read_line, vector<string> expr)
기 능 : 문법에 맞게 문장이 입력되었는지 확인
*/
int use_vector_error_check(char* read_line, vector<string> expr)
{
	double compare=0;

	/* 실수 */
	for (int i = 0; i < expr.size(); i++) {
		if (atoi(expr[i].c_str()) != 0) { //연산자 제외
			compare = stod(expr[i]);

			if ((compare - (int)compare) == 0) { //정수
			}

			else {
				printf("숫자가 들어갈 자리에 정수가 아닌 실수가 입력되었습니다.\n");
				return 1;
			}
		}
	}
	
	
	/* IF와 MINUS 이외에 다른 문자 */
	for (int i = 0; i < expr.size(); i++) {

		/* 연산자인 경우 */
		if (atoi(expr[i].c_str()) == 0) {
			if (expr[i] != "IF" && expr[i] != "MINUS") {
				printf("undefined\n");
				return 1;
			}
		}
	}
}