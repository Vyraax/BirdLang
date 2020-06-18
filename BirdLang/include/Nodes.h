#pragma once

#include "Token.h"

class Node {
public:
	Node(Token* token, Node* left = nullptr, Node* right = nullptr) : 
		token(token),
		left(left),
		right(right)
	{}

	Token* token;
	Node* left;
	Node* right;

	inline friend std::ostream& operator << (std::ostream& stream, Node* node) {
		return stream << "Node, Token:" << node->token;
	}
};

class NumericNode : public Node {
public:
	NumericNode(Token* token) :
		Node(token) {}

	inline friend std::ostream& operator << (std::ostream& stream, NumericNode* node) {
		return stream << "NumericNode, Token:" << node->token;
	}
};

class BinaryOperationNode : public Node {
public:
	BinaryOperationNode(Node* left, Token* token, Node* right) :
		Node(token, left, right) {}

	inline friend std::ostream& operator << (std::ostream& stream, BinaryOperationNode* node) {
		return stream << "BinaryOperationNode, Token:" << node->token;
	}
};

class UnaryOperationNode : public Node {
public:
	UnaryOperationNode(Node* node, Token* token) :
		Node(token, nullptr, node),
		node(node) {}

	inline friend std::ostream& operator << (std::ostream& stream, UnaryOperationNode* node) {
		return stream << "UnaryOperationNode, Token:" << node->token;
	}

	Node* node;
};