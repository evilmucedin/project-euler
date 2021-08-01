#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Node {
	Node* parent_ = nullptr;
	string tag_;
	unordered_map<string, string> attrs_;
	unordered_map<string, Node*> children_;
};

struct Tree {
	Node* root_ = nullptr;
	Node* current_ = nullptr;
};

string readTag(const char* s, size_t& current) {
	string result;
	while (s[current] != ' ' && s[current] != '>' && s[current] != '.' && s[current] != '~') {
		result += s[current];
		++current;
	}
	return result;
}

string readAttr(const char* s, size_t& current) {
	while (s[current] == ' ') {
		++current;
	}
	string result;
	while (s[current] != ' ' && s[current] != '=' && s[current] != 0 && s[current] != '\n') {
		result += s[current];
		++current;
	}
	return result;
}

void skipEqualSign(const char* s, size_t& current) {
	while (s[current] != '=') {
		++current;
	}
	++current;
}

string readValue(const char* s, size_t& current) {
	while (s[current] != '\"') {
		++current;
	}
	++current;
	string result;
	while (s[current] != '\"') {
		result += s[current];
		++current;
	}
	if (s[current] == '\"') {
		++current;
	}
	return result;
}

int main() {
	freopen("input.txt", "r", stdin);

	int n;
	int q;
	scanf("%d%d", &n, &q);

	static const size_t MAX_LEN = 1 << 19;
	char buffer[MAX_LEN];
	fgets(buffer, MAX_LEN, stdin);

	Tree tree;
	tree.root_ = new Node();
	tree.current_ = tree.root_;

	for (size_t i = 0; i < n; ++i) {
		fgets(buffer, MAX_LEN, stdin);
		if (buffer[1] == '/') {
			tree.current_ = tree.current_->parent_;
		} else {
			size_t current = 1;
			auto tag = readTag(buffer, current);

			auto newNode = new Node();
			newNode->tag_ = tag;
			newNode->parent_ = tree.current_;
			tree.current_->children_.emplace(tag, newNode);
			tree.current_ = newNode;

			while (buffer[current] != '>') {
				auto attrName = readAttr(buffer, current);
				skipEqualSign(buffer, current);
				auto attrValue = readValue(buffer, current);
				tree.current_->attrs_.emplace(attrName, attrValue);
			}
		}
	}

	for (size_t i = 0; i < q; ++i) {
		fgets(buffer, MAX_LEN, stdin);
		tree.current_ = tree.root_;
		size_t current = 0;
		while (tree.current_ && buffer[current] != '~') {
			if (buffer[current] == '.') {
				++current;
			}
			auto tag = readTag(buffer, current);
			auto toTag = tree.current_->children_.find(tag);
			if (toTag != tree.current_->children_.end()) {
				tree.current_ = toTag->second;
			}
			else {
				tree.current_ = nullptr;
			}
		}
		if (buffer[current] == '~' && tree.current_) {
			++current;
			auto attrName = readAttr(buffer, current);
			auto toAttr = tree.current_->attrs_.find(attrName);
			if (toAttr != tree.current_->attrs_.end()) {
				cout << toAttr->second << endl;
			} else {
				cout << "Not Found!" << endl;
			}
		} else {
			cout << "Not Found!" << endl;
		}
	}

	return 0;
}