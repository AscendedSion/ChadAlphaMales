#pragma once

#include "../Menu/Menu.h"
#include "../ConfigManager/ConfigManager.h"
#include "../Vars.h"
#include <set>

// I want a playerlist that will show
// Name - Kick - Checkbox with ignore - Profile? [x]
// File will consist of pi.friendsID on each newline [x]
// If not checkbox the friendsID is in the file - they are ignored [x]
// If checkbox && friendsID is in file - remove it from file [x]\
// Kick = callvote kick pi.userID
// Profile = g_SteamInterfaces.Friends015->ActivateGameOverlayToUser("steamid", CSteamID((UINT64)(0x0110000100000000ULL + pi.friendsID)));

struct plistPlayer
{
	int index;
	PlayerInfo_t info;
	int teamNum;
};

class CPlayerlist {
private:
	//The location of all ignored users
	std::wstring ignorePath = (g_CFG.m_sConfigPath + _(L"\\CAMCore\\IGNORE.poop"));
	//A set of friendsIDs
	std::set<uint32_t> ignoredPlayers;
public:
	//Saves the file
	void SaveSet() {
		std::wofstream IgnoreWrite(ignorePath, std::ios::out);
		if (IgnoreWrite.is_open()) {
			for (uint32_t y : ignoredPlayers) {
				IgnoreWrite << y << "\n";
			}
		}
		IgnoreWrite.close();
	}

	// Run this when injecting
	void GetIgnoredPlayers() {
		//Get
		std::wifstream IgnoreRead(ignorePath);

		uint32_t x;
		while (IgnoreRead >> x)
		{
			ignoredPlayers.insert(x);
		}
	}

	// Pass in the users friendsID
	void AddIgnore(uint32_t friendsID) {
		ignoredPlayers.insert(friendsID);
		SaveSet();
	}

	void RemoveIgnore(uint32_t friendsID) {
		ignoredPlayers.erase(friendsID);
		SaveSet();
	}
public:
	// Use this for checkbox?
	bool IsIgnored(uint32_t friendsID) {
		return ignoredPlayers.find(friendsID) != ignoredPlayers.end();
	}
	void GetPlayers();
	//A vector of the plistPlayer struct (honestly not sure why i used std::set for ignoredPlayers but oh well)
	std::vector<plistPlayer> players;
};

inline CPlayerlist g_Playerlist;