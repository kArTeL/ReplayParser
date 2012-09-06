#include <stdio.h>
#include <BWAPI.h>
#include <BWAPI/Client.h>
#include <BWTA.h>
#include <windows.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

using namespace BWAPI;

void reconnect()
{
	while(!BWAPIClient.connect())
	{
		Sleep(1000);
	}
}

void appendToFile(std::string in) {
	std::ofstream myfile;
	myfile.open ("output.txt", std::ios::out | std::ios::app); // append
	myfile << in;
	myfile.close();
}

void logToCsv(Event *e) {
	int seconds=Broodwar->getFrameCount()/24;

	// A unit was just made
	if( (e->getType() == EventType::UnitMorph || e->getType() == EventType::UnitCreate) ) {
		// Unit is a building
		if (e->getUnit()->getType().isBuilding())
		{
			/*
			 * Time (in seconds),
			 * Player ID,
			 * Player race,
			 * Player unitscore,
			 * Building name,
			 * Building position x,
			 * Building position y
			 */
			std::stringstream s;
			s	<< seconds << "," 
				<< e->getUnit()->getPlayer()->getID() << ","
				//<< e->getUnit()->getPlayer()->getName() << ","
				<< "\"" << e->getUnit()->getPlayer()->getRace().c_str() <<  "\"" << ","
				<< e->getUnit()->getPlayer()->getUnitScore() << ","
				<< "\"" << e->getUnit()->getType().getName().c_str() << "\"" << ","
				<< e->getUnit()->getPosition().x() << "," << e->getUnit()->getPosition().y()
				<< std::endl;
			appendToFile(s.str());
		}
	}
}

void getWorkerCount(Event *e) {
	
}

int main(int argc, const char* argv[])
{
	BWAPI::BWAPI_init();
	printf("Connecting...");
	reconnect();
	
	while(true)
	{
		printf("waiting to enter match\n");
		while (!Broodwar->isInGame())
		{
			BWAPI::BWAPI_init();
			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				printf("Reconnecting...\n");
				reconnect();
			}
		}
		printf("starting match!");
		Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
		// Enable some cheat flags
		//Broodwar->enableFlag(Flag::UserInput);
		//Broodwar->enableFlag(Flag::CompleteMapInformation);
		Broodwar->setLocalSpeed(0);
		Broodwar->setGUI(false);

		if (Broodwar->isReplay())
		{
			Broodwar->printf("The following players are in this replay:");
			for(std::set<Player*>::iterator p=Broodwar->getPlayers().begin();p!=Broodwar->getPlayers().end();p++)
			{
				if (!(*p)->getUnits().empty() && !(*p)->isNeutral())
				{
					Broodwar->printf("%s, playing as a %s",(*p)->getName().c_str(),(*p)->getRace().getName().c_str());
				}
			}
		}

		
		
		while(Broodwar->isInGame())
		{
			
			for(std::list<Event>::iterator e=Broodwar->getEvents().begin();e!=Broodwar->getEvents().end();e++)
			{
				// Ignore some events
				if( e->getType() != EventType::MatchFrame && e->getType() != EventType::None && Broodwar->getFrameCount()>1) {
					logToCsv(&*e);
				}
				//logAllEvents(&*e);
				//handleEvent(&*e);
			}

			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				printf("Reconnecting...\n");
				reconnect();
			}
		}
		appendToFile("Game ended\n\n");
		

	}
	system("pause");
	return 0;
}
