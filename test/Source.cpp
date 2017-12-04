#include"Header.h"

void List::setNewNode(Node newNodeA) {
	this->newNode = newNodeA;
}

// cái này là return 1 bản copy của newNode
// ko phải return newNode
// chú return thế này thì không thể thay đổi object bên trong được
// hoặc return còn trỏ Node*
// hoặc return tham chiếu Node&
Node List::getNewNode() {
	return newNode;
}

int main() {

	List *oldNode = new List();

	Node node = oldNode->getNewNode();
	node.a = 3;
	// giờ giả sử em muốn chỉnh sủa chính cái node trong list đó chứ ko phải tạo ra một node mới như anh
	// thì phải làm sao ạ
	// thì chú phải chui vào trong lớp để sửa

	// ok
	int x = oldNode->getNewNode().a;

	// ở đây có khái niệm lvalue với rvalue: chỗ này chưa hiểu ạ, chỗ trên em hiểu rồi ạ ?

	// 2 khái niệm này khá mới từ C++ 11
	// đại loại rvalue là những thứ chỉ nằm được ở bên phải
	// lvalue là những cái vừa nằm bên phải, vừa nằm bên trái dấu bằng
	// trước c++ 11 thì đơn giản là lỗi, anh cũng ko biết giải thích thế nào

	// lvalue với rvalue nó liên quan đến move semantic
	// nó rất hay nhưng phần lớn được ẩn đi
	// chú đọc thêm về nó cũng tốt mà không thì cứ sử dụng những cái có sẵn như std::list, std::vector, std::string
	// nói chung là những cái std đều được implement move semantic rồi
	// nó tăng hiệu năng đáng kể của c++ 11 so với các bản trước đó
	// ok anh ! em sẻ đọc thêm ạ !! thanks anh ạ, hôm nay lên công ty máy anh vẫn chưa tắt :(

	// nó báo là cái cục kia phải là modifiable lvalue mới dùng được
	// nhưng thằng kia là rvalue, nó chỉ được nằm ở bên phải dấu bằng
	oldNode->getNewNode().a = 3;
}