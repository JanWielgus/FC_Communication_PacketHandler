/*
    Name:       FC_Communication_PacketHandler_example.ino
    Created:	30/05/2019 23:10:11
    Author:     Jan Wielgus
	
	NOTE:
	This code is an example class. For further use there should be made individual class for each purpose
	eg. one for communication and for debugging, each with proper name and each inherit form FC_Communication_Base class.
*/

#include "FC_Communication_PacketHandler.h"

const uint8_t MAX_SEND_SIZE = 30;

FC_Communication_PacketHandler com(&Serial1, MAX_SEND_SIZE);



void setup()
{
	Serial1.begin(9600);
}



void loop()
{
	//
	// HOW TO USE THIS CLASS
	//
	
	
	// Access the data by using this:
	com.receiveAndUnpackData();
	int odebrana = com.received.var1;
	// you can access other data
	
	
	// Pack data here:
	com.toSend.zmiennaDoWyslania = 5;
	// pack all variables in the data packet
	com.packAndSendData(com.sendPacketTypes.TYPE1_ID, com.sendPacketTypes.TYPE1_SIZE);
	// you can repeat this for every data packet
	
	
	// Check conneciton stability:
	int conStability = com.connectionStability(); // int values from 0 to 3 (3 best)
	
	
	delay(50);
}
