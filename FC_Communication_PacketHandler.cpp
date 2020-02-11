// 
// 
// 

#include <FC_Communication_PacketHandler.h>


FC_Communication_PacketHandler::FC_Communication_PacketHandler(Stream* serial, uint8_t bufSize)
	:FC_Communication_Base(serial, bufSize)
{
	conStab = 0;
	
	amtOfReceivedDataPackets = (sizeof(receivedPacketTypes) /* / sizeof(uint8_t) */)/2;
	
	if (amtOfReceivedDataPackets > 0)
		receivedDataPacketsList = new bool[amtOfReceivedDataPackets+1]; // +1 to use 1 to TYPE1 and so on...
}


FC_Communication_PacketHandler::~FC_Communication_PacketHandler()
{
	if (amtOfReceivedDataPackets > 0)
		delete [] receivedDataPacketsList;
}


/*
	SOME IMPORTANT INFO:
	
		- How to use checksums:
		- buffer[0] in dataPacket is reserved for the checksum value
		- if you are calculating the checksum, store it in the buffer[0]
		  (calculate it after packing data to dataPacket!)
		
		- buffer[1] is reserved for data type (ID) !
		- buffer[0] is not required for checksum if you do not use it
		- buffer[1] is not required for data type (ID) if you use only one data packet type
*/




void FC_Communication_PacketHandler::beforeReceiving()
{
	// Need to be reset before receiving. Used to calculate connection stability.
	atLeastOneFlag = false; // at least one packet was received. Needed to return true/false at the end
	
	// reset the list of received data packets
	for (int i=1; i<=amtOfReceivedDataPackets; i++)
		receivedDataPacketsList[i] = false;
}


bool FC_Communication_PacketHandler::receiveAndUnpackData()
{
	beforeReceiving();
		
	while (receiveData())
	{
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// CHANGE FOR OTHER PURPOSES FROM HERE
		
		
		
		
		// Check if this packet is the specific one (TYPE1)
		if (checkReceivedDataPacket(receivedPacketTypes.TYPE1_ID, receivedPacketTypes.TYPE1_SIZE, true))
		{
			// Unpack data by updating proper variables
			// [0] - checksum
			// [1] - ID
			data.received.var1 = dpReceived.buffer[2];
			data.received.liczba.byteArr()[0] = dpReceived.buffer[3];
			data.received.liczba.byteArr()[1] = dpReceived.buffer[4];
			// 4 bytes
			for(int i=0; i<4; i++)
				data.received.zmienna.byteArr()[i] = dpReceived.buffer[5+i];
			data.received.innaLiczba.byteArr()[0] = dpReceived.buffer[9];
			data.received.innaLiczba.byteArr()[1] = dpReceived.buffer[10];
		
			// PACKET SIZE SHOULD BE 11 !!! (10+1=11  counted from zero)
		}
	
	
		// Check if this packet is TYPE2
		/*
		else if (checkReceivedDataPacket(receivedPacketTypes.TYPE2_ID, receivedPacketTypes.TYPE2_SIZE, true))
		{
			// ....
			// unpack data from the buffer
			// ....
		}
		*/
		
		
		
		
		// TO HERE
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	}
	
	
	// Calculate the connection stability (edit only parameters)
	uint8_t sum = (uint8_t)pastComStatesArr[1] + pastComStatesArr[0] + atLeastOneFlag;
	// TUNE multipliers if needed (depending on the update frequency)
	conStab = sum<conStab ? 0.8*sum + 0.2*conStab  :  0.6*sum + 0.4*conStab; // slower increase than decrease
	// update historic values
	pastComStatesArr[1] = pastComStatesArr[0];
	pastComStatesArr[0] = atLeastOneFlag;
	
	
	if (atLeastOneFlag)
		return true;
	
	return false;
}


void FC_Communication_PacketHandler::packAndSendData(uint8_t packetID, uint8_t packetSize)
{
	dpToSend.size = (size_t)packetSize;
	dpToSend.buffer[1] = packetID;
	
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// CHANGE FOR OTHER PURPOSES FROM HERE
	
	
	
	// TYPE1
	if (packetID == sendPacketTypes.TYPE1_ID)
	{
		// 4 bytes
		for (int i=0; i<4; i++)
			dpToSend.buffer[2+i] = data.toSend.temp.byteArr()[i];
		dpToSend.buffer[6] = data.toSend.zmiennaDoWyslania.byteArr()[0];
		dpToSend.buffer[7] = data.toSend.zmiennaDoWyslania.byteArr()[1];
		dpToSend.buffer[8] = data.toSend.otherVar;
		
		dpToSend.buffer[0] = calcChecksum();
		
		sendData();
	}
	
	// TYPE2
	/*
	else if (packetID == sendPacketTypes.TYPE2_ID)
	{
		// .....
		
		dpToSend.buffer[0] = calcChecksum();
		
		sendData();
	}
	*/
	
	
	
	// TO HERE
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}




uint8_t FC_Communication_PacketHandler::connectionStability()
{
	// calculated while receiving
	return uint8_t(conStab + 0.5); // average
}


bool FC_Communication_PacketHandler::checkReceivedDataPacket(uint8_t packetID, uint8_t packetSize, bool checkChecksumFlag)
{
	uint8_t IDpos;
	IDpos = checkChecksumFlag==true ? 1 : 0;
	
	if (dpReceived.buffer[IDpos] == packetID && dpReceived.size == packetSize)
	{
		// if checkChecksumFlag == false or if checkChecksum==true if checkChecksum()==true
		if ( !checkChecksumFlag || checkChecksum() )
		{
			// check which packet was received
			for (uint8_t i=0; i<amtOfReceivedDataPackets; i++)
			{
				uint8_t* address = ((uint8_t*)(&receivedPacketTypes)) + (i*2); // ID is every 2nd place in the receivedPacketTypes structure
				if ( *address == packetID )
				{
					receivedDataPacketsList[i+1] = true;
					break;
				}
			}
			
			
			atLeastOneFlag = true;
			return true;
		}
	}
	
	return false;
}


// One of the receivedPacketType structure elements can be passed only !!!!!!
bool FC_Communication_PacketHandler::wasReceived(const uint8_t& packetID)
{
	uint8_t pos = (int)(&packetID) - (int)(&receivedPacketTypes) + 1; // number of checked type (TYPE1 = 1, TYPE2 = 2,... ONLY IF IN ORDER !!! )
	
	if (pos <= amtOfReceivedDataPackets)
		return receivedDataPacketsList[pos];
	return false;
}


