#include "PoolAllocator.h"
#include <cstdio>
#include <ostream>
#include <iostream>
#include <windows.h>
#include <chrono>


#define	MAX_ETHER_FRAME		0x5EA 

Allocator* EthPoolAllocator;

struct INTERMEDIATE_BUFFER
{
	LIST_ENTRY		m_qLink;
	ULONG			m_dwDeviceFlags;
	ULONG			m_Length;
	ULONG			m_Flags; // NDIS_PACKET flags
	ULONG			m_8021q; // 802.1q info
	ULONG			m_FilterID;
	ULONG			m_Reserved[4];
	UCHAR			m_IBuffer[MAX_ETHER_FRAME];

};

struct NDISRD_ETH_Packet
{
	INTERMEDIATE_BUFFER		Buffer;
};

struct ETH_REQUEST
{
	HANDLE				hAdapterHandle;
	NDISRD_ETH_Packet	EthPacket;
};

//constexpr auto COUNT = 0x2710;   //10'000
constexpr auto COUNT = 0x186A0;  //100'000

constexpr auto ethSize = sizeof(ETH_REQUEST);
constexpr auto maxSize = COUNT * ethSize;
int counter;

void* fillEth(const size_t ethSize)
{
	auto ethAddr = reinterpret_cast<ETH_REQUEST*>(EthPoolAllocator->Allocate(ethSize, 8));

	ethAddr->hAdapterHandle = reinterpret_cast<void*>(1220);
	NDISRD_ETH_Packet bfr{};
	INTERMEDIATE_BUFFER interMedBuffer{};
	interMedBuffer.m_dwDeviceFlags = counter * 0x100;
	interMedBuffer.m_Length = counter * 0x200;
	interMedBuffer.m_Flags = counter * 0x300;
	interMedBuffer.m_8021q = counter * 0x400;
	interMedBuffer.m_FilterID = counter * 0x500;
	interMedBuffer.m_Reserved[4] = {  0x600, };
	memset(interMedBuffer.m_IBuffer, 1, MAX_ETHER_FRAME);
	bfr.Buffer = interMedBuffer;
	ethAddr->EthPacket = bfr;
	++counter;
	return static_cast<void*>(ethAddr);
}

void fillEth(ETH_REQUEST& eth)
{
	eth.hAdapterHandle = reinterpret_cast<void*>(1220);
	NDISRD_ETH_Packet bfr{};
	INTERMEDIATE_BUFFER interMedBuffer{};
	interMedBuffer.m_dwDeviceFlags = counter * 0x100;
	interMedBuffer.m_Length = counter * 0x200;
	interMedBuffer.m_Flags = counter * 0x300;
	interMedBuffer.m_8021q = counter * 0x400;
	interMedBuffer.m_FilterID = counter * 0x500;
	interMedBuffer.m_Reserved[4] = { 0x600, };
	memset(interMedBuffer.m_IBuffer, 1, MAX_ETHER_FRAME);
	bfr.Buffer = interMedBuffer;
	eth.EthPacket = bfr;
	++counter;
}

void freeEth(void* ethAddr)
{
	EthPoolAllocator->Free(ethAddr);
}

int mainAllc()
{
	counter = 1;

	EthPoolAllocator = new PoolAllocator(maxSize, ethSize);
	EthPoolAllocator->Init();
	for(auto iop = 0 ; iop < 100 ; iop ++)
	{
		std::cout << "__________________ROUND: " << iop << "_________________________" << std::endl;
		void* addresses[COUNT];
		const auto fillStart = std::chrono::high_resolution_clock::now();
		for (auto& addresse : addresses)
			addresse = fillEth(ethSize);
		const auto fillEnd = std::chrono::high_resolution_clock::now();

		const auto iEth = static_cast<ETH_REQUEST*>(addresses[5000]);
		const auto iEth1 = static_cast<ETH_REQUEST*>(addresses[2200]);
		std::cout << "Length is:" << iEth->EthPacket.Buffer.m_Length << " Flags is:" << iEth->EthPacket.Buffer.m_Flags << std::endl;
		std::cout << "Length is:" << iEth1->EthPacket.Buffer.m_Length << " Flags is:" << iEth1->EthPacket.Buffer.m_Flags << std::endl;

		const auto freeStart = std::chrono::high_resolution_clock::now();
		for (auto& addresse : addresses)
			freeEth(addresse);
		const auto freeEnd = std::chrono::high_resolution_clock::now();

		auto fill_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(fillEnd - fillStart);
		auto free_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(freeEnd - freeStart);

		std::cout << "fill: " << fill_time_span.count() << " seconds." << std::endl;
		std::cout << "free: " << free_time_span.count() << " seconds." << std::endl;

		Sleep(2000);
	}

	delete EthPoolAllocator;
	return getchar();
}


int mainNew()
{
	ETH_REQUEST* addresses[COUNT]={};
	for (auto iop = 0; iop < 100; iop++)
	{
		std::cout << "__________________ROUND: " << iop << "_________________________" << std::endl;
		const auto fillStart = std::chrono::high_resolution_clock::now();
		for (auto i = 0; i < COUNT;i++)
		{
			auto eth = new ETH_REQUEST();
			fillEth(*eth);
			addresses[i] = eth;
		}
		const auto fillEnd = std::chrono::high_resolution_clock::now();

		const auto iEth = static_cast<ETH_REQUEST*>(addresses[5000]);
		const auto iEth1 = static_cast<ETH_REQUEST*>(addresses[2200]);
		std::cout << "Length is:" << iEth->EthPacket.Buffer.m_Length << " Flags is:" << iEth->EthPacket.Buffer.m_Flags << std::endl;
		std::cout << "Length is:" << iEth1->EthPacket.Buffer.m_Length << " Flags is:" << iEth1->EthPacket.Buffer.m_Flags << std::endl;

		const auto freeStart = std::chrono::high_resolution_clock::now();
		for (auto& addresse : addresses)
			delete(addresse);
		const auto freeEnd = std::chrono::high_resolution_clock::now();

		auto fill_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(fillEnd - fillStart);
		auto free_time_span = std::chrono::duration_cast<std::chrono::duration<double>>(freeEnd - freeStart);

		std::cout << "fill: " << fill_time_span.count() << " seconds." << std::endl;
		std::cout << "free: " << free_time_span.count() << " seconds." << std::endl;
		Sleep(2000);
	}

	return getchar();

}