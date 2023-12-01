#include "TLB.h"

void TLB::manageHistory(uint8_t page)
{
	std::stack<uint8_t> tempPageHistory;
	while (!pageHistory.empty())
	{
		uint8_t currentPage = pageHistory.top();
		pageHistory.pop();

		// Page needs to be put on top of stack, don't add back yet
		if (currentPage != page)
		{
			tempPageHistory.push(currentPage);
		}
	}

	// If we have 'TLB_MAX_ELEMENTS' or more elements by now, we have to remove last element from history and elements map
	if (tempPageHistory.size() >= TLB_MAX_ELEMENTS)
	{
		uint8_t removedPage = tempPageHistory.top();
		tempPageHistory.pop();

		elements.erase(removedPage); // This should probably be elsewhere ...
	}

	while (!tempPageHistory.empty())
	{
		uint8_t currentPage = tempPageHistory.top();
		tempPageHistory.pop();

		pageHistory.push(currentPage);
	}

	// Push last page
	pageHistory.push(page);
}

int TLB::getFrameFromPage(uint8_t page)
{
	if (elements.count(page) != 0)
	{
		manageHistory(page);
		return elements[page];
	}
	return -1;
}

void TLB::addElement(uint8_t page, int frame)
{
	if (getFrameFromPage(page) != -1)
	{
		manageHistory(page);
		return;
	}

	if (elements.size() <= TLB_MAX_ELEMENTS)
	{
		elements.insert({ page, frame });
		manageHistory(page);
	}
}