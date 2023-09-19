#include<string>
#include"skiplist.hpp"
#include"ToolFunctions_H.h"
#define TEST_DEBUG 1
using namespace dbase;


#include<deque>

std::deque<int> deq;

void cout_int(void* v) {
	std::cout << *(int*)v;
}
void cout_string(void* v) {
	std::cout << *(std::string*)v;
}
enum Day {
	mon,
	tu,
	wed
}day;
int main() {
	//为了输出value，需要传入回调函数，不然输出
	SkipList<int, int> head(10);
	//SkipList<int, std::string> head(10);
	head.initialize();
	//head.add_value(1, "df", 1);
	for (int i = 0; i < 30; i++) {
		head.add_value(i, i, 1);
	}
	/////////////////////////目前第二次出现level>0的数字插入时会出现死循环////////////////////////////////
	head.display_list(cout_int, cout_int);
	return 0;
}