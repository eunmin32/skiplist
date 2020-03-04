//
// Created by Yusuf Pisan on 4/26/18.
// Edited by Eunmin Lee on 11/17/2019
//

#include <climits>
#include <cstdlib>
#include <iostream>

#include "skiplist.h"

using namespace std;

ostream &operator<<(ostream &Out, const SkipList &SkipL) {
	for (int Level = SkipL.Depth - 1; Level >= 0; Level--) {
		Out << "Level: " << Level << " -- ";
		SkipList::SNode* Curr = SkipL.FrontGuards[Level];
		while (Curr != nullptr) {
			Out << Curr->Data << ", ";
			Curr = Curr->Next;
		}
		Out << "\n" ;
	}
  return Out;
}

SkipList::SNode::SNode(int Data) : Data{ Data }, Next{ nullptr },
Prev{ nullptr }, UpLevel{ nullptr }, DownLevel{nullptr}{}

SkipList::SkipList(int Depth) : Depth{ Depth }, FrontGuards{new SNode*[Depth]}, RearGuards{new SNode *[Depth] }{
	FrontGuards[0] = new SNode(INT_MIN);
	RearGuards[0] = new SNode(INT_MAX);
	connectToRight(FrontGuards[0], RearGuards[0]); //HOW DO I MAKE IT SHORT?

	for (int Level = 1; Level < Depth; Level++) {
		FrontGuards[Level] = new SNode(INT_MIN);
		RearGuards[Level] = new SNode(INT_MAX);
		connectToRight(FrontGuards[Level], RearGuards[Level]);
		connectToTop(FrontGuards[Level - 1], FrontGuards[Level]);
		connectToTop(RearGuards[Level - 1], RearGuards[Level]);
	}
}

void SkipList::connectToTop(SNode* bottom, SNode* top){
	bottom->UpLevel = top;
	top->DownLevel = bottom;
}

void SkipList::connectToRight(SNode* left, SNode* right){
	left->Next = right;
	right->Prev = left;
}


void SkipList::addBefore(SNode* NewNode, SNode* NextNode){
	connectToRight(NextNode->Prev, NewNode);
	connectToRight(NewNode, NextNode);
}

bool SkipList::alsoHigher() const {
	return rand() % 2 == 1;
}

bool SkipList::add(const int Data, int Level, bool coinFlip, SNode* BottomNode) {
	if (coinFlip == false || Level == Depth) { //Base: when coin didn't flip right or at the top level
		return true;
	}
	SNode* Curr = FrontGuards[Level]->Next; 
	while (Curr != RearGuards[Level] && Curr->Data <= Data) { //Stop going when Curr at the end
		if (Data == Curr->Data) {						   //or curr has the higher data
			return false; //Base: When Data is duplicated    
		}
		Curr = Curr->Next;
	}
	SNode* NewNode = new SNode(Data);
	addBefore(NewNode, Curr); // add 
	if (Level != 0) { //connect to bottom
		connectToTop(BottomNode, NewNode);
	}
	return add(Data, Level + 1, alsoHigher(), NewNode); //recursion
}

SkipList::SNode* SkipList::getTopPointerTo(const int Data) {
	SNode* Curr = FrontGuards[Depth - 1];
	while (Curr != nullptr) {
		while (Data >= Curr->Next->Data) {
			Curr = Curr->Next;
			if (Data == Curr->Data) 
				return Curr;
		}	
			Curr = Curr->DownLevel;
	}
	return nullptr;
}

bool SkipList::add(const int Data) {
	return add(Data, 0, true, nullptr);
}

SkipList::~SkipList() {
	SNode* Target;
	for (int Level = Depth - 1; Level >= 0; Level--) {
		Target = RearGuards[Level]->Prev;
		while (Target != FrontGuards[Level]) { 
			delete Target->Next;
			Target = Target->Prev;
		}
		delete Target->Next;
		delete Target;		
    }
	delete[] FrontGuards;
	delete[] RearGuards;
	cout << "SKIP LIST DELETED" << endl;
}

bool SkipList::remove(const int Data) { 
	SNode* Target = getTopPointerTo(Data);
	if (Target == nullptr) //Data is not contained 
		return false;
	connectToRight(Target->Prev, Target->Next); //disconnect horizontally
	while (Target->DownLevel != nullptr) {
		Target = Target->DownLevel;
		delete Target->UpLevel;
		connectToRight(Target->Prev, Target->Next); //disconnet horizontally 
	}	
	//delete very bottom element
	//delete very bottom element
	delete Target;
	Target = nullptr;
	return true;
}

bool SkipList::contains(const int Data) {
	return !(getTopPointerTo(Data) == nullptr); 
}



