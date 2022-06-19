#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <algorithm>
#include <vector>

using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

struct Node{
    int row, col;
};

int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int alpha_beta(int depth, bool isMax, int alpha, int beta, Node node);
int evaluate(int isMax);
int check_board();

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void write_valid_spot(std::ofstream& fout) {
	int M = INT_MIN, x = -1, y = -1;
	
	for (int i=0;i<15;i++){
		for (int j=0;j<15;j++){
			if (board[x][y] == EMPTY){
                Node node;
                node.row = i; node.col = j;
				int temp = alpha_beta (2, true,INT_MIN, INT_MAX, node);
				if (M < temp){
					M = temp;
					x = i;
					y = j;
				}
			}
		}
	}
	if (x == -1 && y == -1){
		x = 15/2;
		y = 15/2;
	}
    fout << x << " " << y << std::endl;
    // Remember to flush the output to ensure the last action is written to file.
    fout.flush();
}

int check_board(){
    //檢查棋盤有沒有結束 玩家1獲勝回傳1 玩家2獲勝回傳2 沒結束回傳0
    //橫的檢查
    for (int i=0;i<15;i++) { //直排0~15
        for (int j=0;j<11;j++) { //橫排最多到10就好(10+4就是最後)
            if (board[i][j] != ' ') {
                if (board[i][j] == 1 && board[i][j+1] == 1 && board[i][j+2] == 1 &&
                    board[i][j+3] == 1 && board[i][j+4] == 1) {
                    return 1;
                }
                else if (board[i][j] == 2 && board[i][j+1] == 2 && board[i][j+2] == 2 &&
                    board[i][j+3] == 2 && board[i][j+4] == 2) {
                    return 2;
                }
            }
        }
    }
    
    //直的檢查
    for (int i=0;i<15;i++) {
        for (int j=0;j<11;j++) {
            if (board[j][i] != ' '){
                if (board[j][i] == 1 && board[j+1][i] == 1 && board[j+2][i] == 1 &&
                    board[j+3][i] == 1 && board[j+4][i] == 1) {
                    return 1;
                }
                else if (board[j][i] == 2 && board[j+1][i] == 2 && board[j+2][i] == 2 &&
                    board[j+3][i] == 2 && board[j+4][i] == 2) {
                    return 2;
                }
            }
        }
    }
    
    //左上右下(上半部)
    for (int j=0;j<11;j++) {
        int k = j;
        for (int i=0;i<=10-j;i++,k++) {
            if (board[i][k] != ' '){
                if (board[i][k] == 1 && board[i+1][k+1] == 1 && board[i+2][k+2] == 1 &&
                    board[i+3][k+3] == 1 && board[i+4][k+4] == 1) {
                    return 1;
                }
                else if (board[i][k] == 2 && board[i+1][k+1] == 2 && board[i+2][k+2] == 2 &&
                    board[i+3][k+3] == 2 && board[i+4][k+4] == 2) {
                    return 2;
                }
            }
        }
    }
    
    //左上右下(下半部)
    for (int i=1;i<=10;i++) {
        int k = i;
        for (int j=0;k<=10;k++,j++) {
            if (board[k][j] != ' '){
                if (board[k][j] == 1 && board[k+1][j+1] == 1 && board[k+2][j+2] == 1 &&
                    board[k+3][j+3] == 1 && board[k+4][j+4] == 1) {
                    return 1;
                }
                else if (board[k][j] == 2 && board[k+1][j+1] == 2 && board[k+2][j+2] == 2 &&
                    board[k+3][j+3] == 2 && board[k+4][j+4] == 2) {
                    return 2;
                }
            }
        }
    }
    
    //右上左下(上半部)
    for (int j=14;j>=5;j--) {
        int k = j;
        for (int i=0;k>=5;k--,i++) {
            if (board[i][k]!=' ') {
                if (board[i][k] == 1 && board[i+1][k-1] == 1 && board[i+2][k-2] == 1 &&
                    board[i+3][k-3] == 1 && board[i+4][k-4] == 1) {
                    return 1;
                }
                else if (board[i][k] == 2 && board[i+1][k-1] == 2 && board[i+2][k-2] == 2 &&
                    board[i+3][k-3] == 2 && board[i+4][k-4] == 2) {
                    return 2;
                }
            }
        }
    }
    
    //佑上左下(下半部)
    for (int i=1;i<=10;i++) {
        int k = i;
        for (int j=14;k<=10;k++,j--) {
            if (board[k][j]!=' ') {
                if (board[k][j] == 1 && board[k+1][j-1] == 1 && board[k+2][j-2] == 1 &&
                    board[k+3][j-3] == 1 && board[k+4][j-4] == 1) {
                    return 1;
                }
                else if (board[k][j] == 2 && board[k+1][j-1] == 2 && board[k+2][j-2] == 2 &&
                    board[k+3][j-3] == 2 && board[k+4][j-4] == 2) {
                    return 2;
                }
            }
        }
    }
    return 0;
}

int evaluate(int isMax){
    //計算state value:1的34連再一起-2的4連再一起
    //p1直接算3個連再一起的 p2算4個
    int p1_value = 0, p2_value = 0;
    for (int i=0;i<15;i++) { //直排0~15
        for (int j=0;j<12;j++) { //橫排最多到11就好(11+2就是最後)
            if (board[i][j] != ' ') {
                if (board[i][j] == 1 && board[i][j+1] == 1 && board[i][j+2] == 1) {
                    p1_value += 1;
                }
                else if (board[i][j] == 2 && board[i][j+1] == 2 
                    && board[i][j+2] == 2 && board[i][j+3] == 2) {
                    p2_value += 1;
                }
            }
        }
        //少判斷一次3個的
        if (board[i][12] == 1 && board[i][13] == 1 && board[i][14] == 1) {
            p1_value += 1;
        }
    }
    
    //直的檢查
    for (int i=0;i<15;i++) {
        for (int j=0;j<12;j++) {
            if (board[j][i] != ' '){
                if (board[j][i] == 1 && board[j+1][i] == 1 && board[j+2][i] == 1) {
                    p1_value += 1;
                }
                else if (board[j][i] == 2 && board[j+1][i] == 2 
                    && board[j+2][i] == 2 && board[j+3][i] == 2) {
                    p2_value += 1;
                }
            }
        }
        if (board[12][i] == 1 && board[13][i] == 1 && board[14][i] == 1) {
            p1_value += 1;
        }
    }
    
    //左上右下(上半部)
    for (int j=0;j<13;j++) {
        int k = j;
        for (int i=0;i<=12-j;i++,k++) {
            if (board[i][k] != ' '){
                if (board[i][k] == 1 && board[i+1][k+1] == 1 && board[i+2][k+2] == 1) {
                    p1_value += 1;
                }
            }
        }
    }
    for (int j=0;j<12;j++) {
        int k = j;
        for (int i=0;i<=11-j;i++,k++) {
            if (board[i][k] != ' '){
                if (board[k][j] == 2 && board[k+1][j+1] == 2 && 
                    board[k+2][j+2] == 2 && board[k+3][j+3] == 2){
                    p2_value += 1;
                }
            }
        }
    }
    
    //左上右下(下半部)
    for (int i=1;i<=12;i++) {
        int k = i;
        for (int j=0;k<=12;k++,j++) {
            if (board[k][j] != ' '){
                if (board[k][j] == 1 && board[k+1][j+1] == 1 && board[k+2][j+2] == 1) {
                    p1_value += 1;
                }
            }
        }
    }
    for (int i=1;i<=11;i++) {
        int k = i;
        for (int j=0;k<=11;k++,j++) {
            if (board[k][j] != ' '){
                if (board[k][j] == 2 && board[k+1][j+1] == 2 && 
                    board[k+2][j+2] == 2 && board[k+3][j+3] == 2) {
                    p2_value += 1;
                }
            }
        }
    }
    
    //右上左下(上半部)
    for (int j=14;j>=3;j--) {
        int k = j;
        for (int i=0;k>=3;k--,i++) {
            if (board[i][k]!=' ') {
                if (board[i][k] == 1 && board[i+1][k-1] == 1 && board[i+2][k-2] == 1) {
                    p1_value += 1;
                }
            }
        }
    }
    for (int j=14;j>=4;j--) {
        int k = j;
        for (int i=0;k>=4;k--,i++) {
            if (board[i][k]!=' ') {
                if (board[i][k] == 2 && board[i+1][k-1] == 2 &&
                    board[i+2][k-2] == 2 &&board[i+3][k-3] == 2) {
                    p2_value += 1;
                }
            }
        }
    }
    
    //佑上左下(下半部)
    for (int i=1;i<=10;i++) {
        int k = i;
        for (int j=14;k<=10;k++,j--) {
            if (board[k][j]!=' ') {
                if (board[k][j] == 1 && board[k+1][j-1] == 1 && board[k+2][j-2] == 1) {
                    p1_value += 1;
                }
            }
        }
    }
    for (int i=1;i<=10;i++) {
        int k = i;
        for (int j=14;k<=10;k++,j--) {
            if (board[k][j]!=' ') {
                if (board[k][j] == 2 && board[k+1][j-1] == 2 &&
                    board[k+2][j-2] == 2 && board[k+3][j-3] == 2) {
                    p2_value += 1;
                }
            }
        }
    }
    if(isMax)
        return (p1_value-p2_value);
    else
        return (p2_value-p1_value);
}

int alpha_beta(int depth, bool isMax, int alpha, int beta, Node node){
    if(check_board() == 1) return INT_MAX; //玩家1贏了
    else if(check_board() == 2) return INT_MIN; //玩家2贏了

    if (depth == 0){
		int value = 0;
		value = evaluate(isMax);
		return value;
	}

    //找到所有可以放的點
    vector<Node> valid_node;
	for (int i=0;i<15;i++){
		for (int j=0;j<15;j++){
			if (board[i][j] == 0 ){
                Node valid;
                valid.row = i; valid.col = j;
				valid_node.push_back(valid);
			}
		}
	}

    //開始跑!
    int len = valid_node.size();
 
    if (isMax){ //玩家1當max
        int best_value = INT_MAX;
        for (int i=0;i<len;i++){
			int temp = alpha_beta(depth - 1, false, alpha, beta, valid_node[i]);
			if (best_value > temp){
				best_value = temp;
			}
			if (beta > best_value){
				beta = best_value;
			 }	
			 if (alpha >= beta){
				break;
			 }
		}
        return best_value;
    }
    else{ //玩家2當min
        int best_value = INT_MIN;
        for (int i=0;i<len;i++){
			int temp = alpha_beta(depth - 1, true, alpha, beta, valid_node[i]);
			if (best_value < temp){
				best_value = temp;
			}
			if (alpha < best_value){
				beta = best_value;
			}	
			if (alpha >= beta){
				break;
			}
		}
        return best_value;
    }
}


int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
