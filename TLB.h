#pragma once
#include <stack>
#include <map>
#include <stdint.h>

class TLB
{
private:
	const int TLB_MAX_ELEMENTS;
	std::stack<uint8_t> pageHistory;
	std::map<uint8_t, int> elements;

	void manageHistory(uint8_t page);
	void removeLastElement();

public:
	TLB(int tlbSize) : TLB_MAX_ELEMENTS(tlbSize) {};

	int getFrameFromPage(uint8_t page);

	void addElement(uint8_t page, int frame);
};