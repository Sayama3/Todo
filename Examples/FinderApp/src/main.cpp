//
// Created by ianpo on 31/08/2024.
//


#include <Todo/Todo.hpp>

#include <iostream>


int main(int argc, char* argv[])
{
	Todo::Initialize(Todo::GetOptimalThreadCount());
	// Do things.
	Todo::Destroy();
	return 0;
}