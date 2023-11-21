#include <bitset>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include "TLB.h"

const std::string BASE_PATH = "C:\\Users\\nicci\\OneDrive\\Documents\\Uni\\Session 3\\Systèmes d'exploitation\\Lab3\\";
const int PAGE_SIZE = 256;

// 256 frames of 256 bytes (chars)
char physicalMemory[256][256] = { 0 };
// 256 pages, [x][0] indicates the frame number, [x][1] indicates if the page is present in physical memory
int pageTable[256][2] = { 0 };

TLB tlb(16);

int getFrameFromPageCalled = 0;
int tlbMissed = 0;

uint16_t createMask(uint16_t a, uint16_t b)
{
	uint16_t r = 0;
	for (uint16_t i = a; i <= b; i++)
	{
		r |= (1 << i);
	}
	return r;
}

std::vector<uint16_t> getLogicalAddresses()
{
	std::vector<uint16_t> logicalAddresses;

	std::ifstream file(BASE_PATH + "addresses.txt");
	std::string address;

	while (std::getline(file, address))
	{
		logicalAddresses.push_back(std::stoul(address));
	}

	return logicalAddresses;
}

void updatePageTable(uint8_t page, int frame)
{
	pageTable[page][0] = frame;
	pageTable[page][1] = 1;
}

int getFrameFromPage(uint8_t page)
{
	getFrameFromPageCalled++;

	int tlbFrame = tlb.getFrameFromPage(page);
	if (tlbFrame != -1)
		return tlbFrame;
	else
		tlbMissed++;

	if (pageTable[page][1] != 1)
	{
		// Should never happen at this point, because physical memory is the same size as virtual memory
		std::cout << "Page fault on page [" << (int)page << "].\n";															
		// If it could happen, we would replace a page and load the page here
	}
	else
		return pageTable[page][0];
}

void loadPageIntoMemoryFrame(uint8_t page, int frame)
{
	std::ifstream file(BASE_PATH + "simuleDisque.bin");
	file.seekg(PAGE_SIZE * page);
	file.read(physicalMemory[frame], PAGE_SIZE);

	updatePageTable(page, frame);
	tlb.addElement(page, frame);
}

int getValueAtAddress(uint8_t page, uint8_t offset)
{
	int frame = getFrameFromPage(page);

	return (int)(physicalMemory[frame][offset]);
}

int main()
{
	std::ofstream resultsFile(BASE_PATH + "results.txt", std::ios_base::trunc);
	uint16_t pageMask = createMask(8, 15);
	uint16_t offsetMask = createMask(0, 7);

	std::vector<uint16_t> logicalAddresses = getLogicalAddresses();

	int filledFramesCount = 0;
	for (uint16_t address : logicalAddresses)
	{
		uint8_t page = (pageMask & address) >> 8;
		uint8_t offset = offsetMask & address;

		if (pageTable[page][1] != 1)
		{
			//std::cout << "Page fault on page [" << (int)page << "]. Loading page into memory.\n";

			if (filledFramesCount < 256)
			{
				loadPageIntoMemoryFrame(page, filledFramesCount);
				filledFramesCount++;
			}
			else
			{
				// Should never happen
				std::cout << "Unable to load page, memory full!\n";
			}
		}

		int value = getValueAtAddress(page, offset);
		std::bitset<8> valueBinary(value);

		uint8_t frame = getFrameFromPage(page);
		uint16_t physicalAddress = (frame << 8) | offset;

		std::string virtualAddressText = "Virtual: " + std::to_string(address);
		std::string physicalAddressText = "Physical: " + std::to_string(physicalAddress);
		std::string valueDText = "Value (D): " + std::to_string(value);
		std::string valueBText = "Value (B): " + valueBinary.to_string();

		std::stringstream ss;

		ss << std::setw(20) << std::left << virtualAddressText
			<< std::setw(20) << std::left << physicalAddressText
			<< std::setw(20) << std::left << valueDText
			<< std::setw(20) << std::left << valueBText << "\n";

		std::cout << ss.str();
		resultsFile << ss.str();
	}

	float tlbSuccesRate = (float)(getFrameFromPageCalled - tlbMissed) / getFrameFromPageCalled * 100;

	std::cout << "\n\n" << "TLB Succes rate : " << tlbSuccesRate;

	return 0;
}