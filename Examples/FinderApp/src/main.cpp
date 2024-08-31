//
// Created by ianpo on 31/08/2024.
//

#include <Todo/Todo.hpp>

int main(int argc, char* argv[])
{
	Todo::Initialize(Todo::GetOptimalThreadCount());
	std::cout << "Hello World" << std::endl;
	Todo::Destroy();
	return 0;
}