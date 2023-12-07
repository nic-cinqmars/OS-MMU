#include "TLB.h"

void TLB::manageHistory(uint8_t page)
{
	// Flip stack
	std::stack<uint8_t> tempPageHistory;
	while (!pageHistory.empty())
	{
		uint8_t currentPage = pageHistory.top();
		pageHistory.pop();

		// Page in parameter needs to be put on top of stack
		// If it is not the page in parameter, add it to the stack
		if (currentPage != page)
		{
			tempPageHistory.push(currentPage);
		}
	}

	// Flip stack back to correct direction
	while (!tempPageHistory.empty())
	{
		uint8_t currentPage = tempPageHistory.top();
		tempPageHistory.pop();

		pageHistory.push(currentPage);
	}

	// Push page in parameter to top of stack
	pageHistory.push(page);
}

void TLB::removeLastElement()
{
	// Flip stack
	std::stack<uint8_t> tempPageHistory;
	while (!pageHistory.empty())
	{
		uint8_t currentPage = pageHistory.top();
		pageHistory.pop();

		tempPageHistory.push(currentPage);
	}

	// Last element is now on top, remove it
	uint8_t removedPage = tempPageHistory.top();
	tempPageHistory.pop();

	// Remove it from elements
	elements.erase(removedPage);

	// Flip stack back to original direction
	while (!tempPageHistory.empty())
	{
		uint8_t currentPage = tempPageHistory.top();
		tempPageHistory.pop();

		pageHistory.push(currentPage);
	}
}

int TLB::getFrameFromPage(uint8_t page)
{
	if (elements.count(page) != 0)
	{
		// Place page back to top of stack since it has been referenced
		manageHistory(page);
		return elements[page];
	}
	return -1;
}

void TLB::addElement(uint8_t page, int frame)
{
	if (getFrameFromPage(page) != -1)
	{
		// If page already exists in our TLB
		//
		// Place page back to top of stack since it has been referenced
		manageHistory(page);
		return;
	}

	// If TLB is already full
	if (elements.size() == TLB_MAX_ELEMENTS)
	{
		removeLastElement();
	}

	elements.insert({ page, frame });
	pageHistory.push(page);
}