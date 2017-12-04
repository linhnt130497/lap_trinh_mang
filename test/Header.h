#pragma once
#include <conio.h>
#include<stdio.h>
#include<iostream>

typedef struct {
	int a;
	int b;
	int c;
} Node;

class List{
private:
	Node newNode;
public:
	void setNewNode(Node newNode);
	Node getNewNode();
};