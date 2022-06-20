#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <algorithm>
#include <vector>
#include <bitset>

using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

struct Node{
    int row, col;
};

//從main偷來的point定義
struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

//8個方向(跑子的周遭情況)
int direction[8][2] = {
    {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {-1,1}, {1,-1}, {-1,-1}
};

int player;
const int SIZE = 15;

//用bitset建造遊戲面板
typedef bitset<15> Row; //一行15個
typedef array<Row, 15> Board_min; //一個board
typedef array<Board_min, 3> Board; //記錄三種點(三維陣列!)
//[哪種點012][x][y]

//一個class紀錄遊戲的狀態(有沒有結束等等)
class State{
private:
    Board board;
    int player;
    void get_legal_move(); //finish

public:
    std::vector<Point> legal_move; //記錄所有的合法移動
    State(){};
    State(Board board, int player); //finish
    int evaluate(); //finish
    State* next_state(Point move); //finish
};

//constructor
State::State(Board board, int player): board(board), player(player){
    this->get_legal_move();
};

//把所有可以下的點存起來
std::vector<Point> all_move;
void get_all_move(){
  for(int i=0;i<15;i++)
    for(int j=0;j<15;j++)
      all_move.push_back(Point(i,j));
}

//看看有沒有五個連載一起(用bitwise確認!)
int check_5cnt(Board_min board){
    for(int i=0;i<15-4;i++){
        //因為有5顆所以檢查到15-4
        //橫排的連續5個都沒碰到0就是有連著
        //.any() 測試是不是0或空 至少一個1就回傳1
        if((board[i] & board[i+1] & board[i+2] & board[i+3] & board[i+4]).any())
            return 1;
        //斜的把他們推到同一直排之後比
        if((board[i] & (board[i+1]>>1) & (board[i+2]>>2) & (board[i+3]>>3) & (board[i+4]>>4)).any())
            return 1;
        if((board[i] & (board[i+1]<<1) & (board[i+2]<<2) & (board[i+3]<<3) & (board[i+4]<<4)).any())
            return 1;
        //直的直接用bitwise比就好
        for(int j=0;j<15;j++){
            if(((board[j]>>i)&=0b11111) == 0b11111){
                return 1;
            }
        }
    }
    return 0;
}

//檢查是不是活四(看看兩端期中一邊是不是空的)
int check_4alive(Board_min board, Board_min empty){
    for(int i=0;i<15-4;i++){
        //橫的左邊右邊是不是空的
        if((empty[i] & board[i+1] & board[i+2] & board[i+3] & board[i+4]).any())
            return 1;
        if((board[i] & board[i+1] & board[i+2] & board[i+3] & empty[i+4]).any())
            return 1;

        //斜的對齊之後判斷
        if((empty[i] & (board[i+1]>>1) & (board[i+2]>>2) & (board[i+3]>>3) & (board[i+4]>>4)).any())
            return 1;
        if((board[i] & (board[i+1]>>1) & (board[i+2]>>2) & (board[i+3]>>3) & (empty[i+4]>>4)).any())
            return 1;
        if((empty[i] & (board[i+1]<<1) & (board[i+2]<<2) & (board[i+3]<<3) & (board[i+4]<<4)).any())
            return 1;
        if((board[i] & (board[i+1]<<1) & (board[i+2]<<2) & (board[i+3]<<3) & (empty[i+4]<<4)).any())
            return 1;

        //直的用bitwise比較
        for(int j=0;j<15; j++){
            if(((board[j]>>i)&=0b11110) == 0b11110 && ((empty[j]>>i)&=0b00001) == 0b00001)
                return 1;
            if(((board[j]>>i)&=0b01111) == 0b01111 && ((empty[j]>>i)&=0b10000) == 0b10000)
                return 1;
        }
    }
    return 0;
}

//計算四個連載一起
int count_4cnt(Board_min board, Board_min empty){
    int count = 0;

    for(int i=0; i<SIZE-4; i+=1){
        //count計算有幾個1
        //橫的(左右一端空白)
        count += (empty[i] & board[i+1] & board[i+2] & board[i+3] & board[i+4]).count();
        count += (board[i] & board[i+1] & board[i+2] & board[i+3] & empty[i+4]).count();
        //斜的
        count += (empty[i] & (board[i+1]>>1) & (board[i+2]>>2) & (board[i+3]>>3) & (board[i+4]>>4)).count();
        count += (board[i] & (board[i+1]>>1) & (board[i+2]>>2) & (board[i+3]>>3) & (empty[i+4]>>4)).count();
        count += (empty[i] & (board[i+1]<<1) & (board[i+2]<<2) & (board[i+3]<<3) & (board[i+4]<<4)).count();
        count += (board[i] & (board[i+1]<<1) & (board[i+2]<<2) & (board[i+3]<<3) & (empty[i+4]<<4)).count();
        //直的
        for(int j=0;j<15;j++){
            count += (((board[j]>>i)&=0b11110) == 0b11110 && ((empty[j]>>i)&=0b00001) == 0b00001);
            count += (((board[j]>>i)&=0b01111) == 0b01111 && ((empty[j]>>i)&=0b10000) == 0b10000);
        }
        if(count>2) //已經超過了就提早return省時間
            return count;
    }
    return count;
}

//檢查3個連載一起的情況
int count_3cnt(Board_min board, Board_min empty){
    int count = 0;
    for(int i=0;i<15-2;i++){

        //橫的
        for(int j=0;j<15;j++){
            //預設是錯的
            bool one_empty = false, double_empty = false;
            bool target = (((board[j]>>i)&=0b111) == 0b111); //知道target是哪個子
            if(i>0 && i<15-3)
                double_empty = empty[j][15-i] && empty[j][15-i-4];
            if(i>1)
                one_empty |= empty[j][15-i] && empty[j][15-i+1];
            if(i<15-4)
                one_empty |= empty[j][15-i-4] && empty[j][15-i-5];
            count += (one_empty & target);
            count += (double_empty & target);
        }

        Row one_empty; //其中一邊可以連成5顆
        Row double_empty; //記錄雙活3
        Row target; //連起來的子

        //直的部分
        one_empty = Row(0);
        double_empty = Row(0);
        target = board[i] & board[i+1] & board[i+2];
        if(i>0 && i<15-3) //雙活3
            double_empty = empty[i-1] & empty[i+3];
        if(i>1) //左邊兩顆是不是空的
            one_empty |= empty[i-1] & empty[i-2];
        if(i<15-4) //右邊兩顆是不是空的
            one_empty |= empty[i+3] & empty[i+4];
        //嘉進分數裡面
        count += (one_empty & target).count();
        count += (double_empty & target).count();

        //左上右下(邏輯同上)
        one_empty = Row(0);
        double_empty = Row(0);
        target = board[i] & (board[i+1]>>1) & (board[i+2]>>2); //記得對其
        if(i>0 && i<15-3)
            double_empty = (empty[i-1]<<1) & (empty[i+3]>>3);
        if(i>1)
            one_empty |= (empty[i-1]<<1) & (empty[i-2]<<2);
        if(i<15-4)
            one_empty |= (empty[i+3]>>3) & (empty[i+4]>>4);
        count += (one_empty & target).count();
        count += (double_empty & target).count();

        //右上左下
        one_empty = Row(0);
        double_empty = Row(0);
        target = board[i] & (board[i+1]<<1) & (board[i+2]<<2); //記得對其
        if(i>0 && i<15-3)
            double_empty = (empty[i-1]>>1) & (empty[i+3]<<3);
        if(i>1)
            one_empty |= (empty[i-1]>>1) & (empty[i-2]>>2);
        if(i<15-4)
            one_empty |= (empty[i+3]<<3) & (empty[i+4]<<4);
        count += (one_empty & target).count();
        count += (double_empty & target).count();
    }
    return count;
}

//計算state value的函式
int State::evaluate(){
    Board_min empty = board[0];
    Board_min me = board[this->player];
    Board_min he = board[3-this->player]; //看我是1或2 對手是2或1
    if(check_5cnt(me) || check_4alive(me, empty)) //連續五個或是有死||活四就贏了
        return INT_MAX;
    if(count_4cnt(he, empty)>1) //對手會贏的狀況(超過兩個死||活四)
        return INT_MIN;
    return count_3cnt(me, empty)-count_3cnt(he, empty);
}

//得到所有可以走的步驟
void State::get_legal_move(){
    std::vector<Point> moves; //蒐集所有可以動的點
    Board_min point;
    bool initial = true;
    //只要跑周遭的點就好 不用遍歷整個棋盤
    for(auto pt: all_move){
        if(board[0][pt.x][pt.y]==0){
            initial = false;
            for(auto pt_try: direction){ //跑附近8個方向的點
                int x = pt.x+pt_try[0];
                int y = pt.y+pt_try[1];
                if(x<0 || y<0 || x>=15 || y>=15 || point[x][y] || board[0][x][y]==0)
                    continue; //不合法的或是空的就跳過
                moves.push_back(Point(x, y));
                point[x][y] = 1;
            }
        }
    }
    //一開始要下中間!
    if(moves.empty() && initial)
        moves.push_back(Point(15/2, 15/2));
    legal_move = moves;
}

//根據可以走的步驟得到下一個狀態
State* State::next_state(Point move){
    //建立新的狀態和棋盤 玩家換人當!
    //建立一個新的棋盤來更新新的狀態
    Board new_board = this->board;
    new_board[this->player][move.x][move.y] = 1;
    new_board[0][move.x][move.y] = 0;
    //建立一個新的狀態
    State *next = new State();
    next->board = new_board; //棋盤更新
    next->player = 3-player; //換人下
    
    //找到所有下一部可以下的
    Board_min point;
    std::vector<Point> moves;
    for(Point way:legal_move){
        if(way!=move){
            moves.push_back(way);
            point[way.x][way.y] = 1;
        }
    }
    //跑8個方向
    for(auto p_try: direction){
        int x = move.x+p_try[0];
        int y = move.y+p_try[1];
        if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || board[0][x][y]==0)
            continue; //不合法的或是空的就跳過
        moves.push_back(Point(x, y));
        point[x][y] = 1;
    }
    next->legal_move = moves;
    return next;
}


class alpha_beta{
    public:
    void write_valid_spot(std::ofstream&);
    int mmalpha_beta(int, int, int, int, Node);
    int evaluate(int);
    int check_board();
};

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void alpha_beta::write_valid_spot(std::ofstream& fout) {
	int M = INT_MIN, x = -1, y = -1;
	
	for (int i=0;i<15;i++){
		for (int j=0;j<15;j++){
			if (board[i][j] == EMPTY){
                Node node;
                node.row = i; node.col = j;
				int temp = mmalpha_beta (1, 1, INT_MIN, INT_MAX, node);
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

int alpha_beta::mmalpha_beta(int depth, int isMax, int alpha, int beta, Node node){
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
 
    if (isMax == 1){ //玩家1當max
        int best_value = INT_MAX;
        for (int i=0;i<len;i++){
			int temp = mmalpha_beta(depth - 1, 2, alpha, beta, valid_node[i]);
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
    else if(isMax == 2){ //玩家2當min
        int best_value = INT_MIN;
        for (int i=0;i<len;i++){
			int temp = mmalpha_beta(depth - 1, 1, alpha, beta, valid_node[i]);
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
    alpha_beta player;
    player.write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
