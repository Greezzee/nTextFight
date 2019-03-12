#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cmath>
using namespace std;

struct stats
{
	int strenght, agility, accuracy, intelligence;
};
struct attackPackage {
	int DMG;
	int successChange;
};
struct effect {
	int ID, duration = 0;
	int level;
};

float sigmoid(float x) {
	return pow(1 + exp(-x), -1);
}

class player {
public:
	player(string name, string team) {
		baseHP = 100.f;
		baseDMG = 10.f;
		realHP = baseHP;
		currentHP = realHP;
		realDMG = baseDMG;
		myStats.accuracy = 50;
		myStats.agility = 50;
		myStats.intelligence = 50;
		myStats.strenght = 50;

		myTeam = team;
		myName = name;

		effects.resize(0);
		effect e;
		e.ID = 0;
		e.level = 1;
		effects.push_back(e);


		isAlive = true;
	}

	void action(vector<player*> enemy) {
		auto target = chooseTarget(enemy);
		attackPackage myAttack;
		myAttack.DMG = realDMG;
		myAttack.successChange = myStats.accuracy;

		cout << myName + "(" + to_string(currentHP) + ")" << " attack ";

		target->takeAttack(myAttack);
	}
	void takeAttack(attackPackage attack) {
		if (!testForMiss(attack.successChange)) {
			cout << "and " << myName + "(" + to_string(currentHP) + "->" + to_string(currentHP - attack.DMG) + ")" << " take " << attack.DMG << " damage" << endl;
			currentHP -= attack.DMG;
			if (currentHP <= 0) isAlive = false;
		}
	}
	void activateEffects() {
		for (auto i = effects.begin(); i != effects.end(); i++) {
			switch (i->ID)
			{
			case 0:
				switch (i->level)
				{
				case 1:
					if (i->duration == 0) i->duration = 3;
					currentHP += 5;
					if (currentHP > realHP) currentHP = realHP;
					cout << myName + " regenerate 5 HP" << endl;
					break;
				}
				i->duration--;
				break;

			}
		}
		list<effect> newEffects;
		for (auto i = effects.begin(); i != effects.end(); i++) if (i->duration != 0) newEffects.push_back(*i);
		effects = newEffects;
	}

	bool getAlive() {
		return isAlive;
	}

private:
	int baseHP, baseDMG;
	int realHP, realDMG;
	int currentHP;
	bool isAlive;
	stats myStats;
	string myTeam;
	string myName;
	list<effect> effects;

	player* chooseTarget(vector<player*> enemy) {
		for (int i = 0; i < enemy.size(); i++) {

			//choose func//

			return enemy[i];
		}
	}
	bool testForMiss(int successChange) {
		float realChange = (float)(myStats.agility - successChange) / 50.f;
		realChange = sigmoid(realChange) * 100;
		if (rand() % 100 + 1 < realChange) {
			cout << "but " << myName << " evade" << endl;
			return true;
		}
		else {
			return false;
		}
	}
};

int testForAlive(vector<player*> redTeam, vector<player*> blueTeam) {
	bool isAnyBlueAlive = false, isAnyRedAlive = false;
	for (int i = 0; i < blueTeam.size(); i++) if (blueTeam[i]->getAlive()) isAnyBlueAlive = true;
	for (int i = 0; i < redTeam.size(); i++) if (redTeam[i]->getAlive()) isAnyRedAlive = true;
	if (isAnyBlueAlive && !isAnyBlueAlive) return 1;
	if (!isAnyBlueAlive && isAnyBlueAlive) return 2;
	if (!isAnyBlueAlive && !isAnyBlueAlive) return -1;
	return 0;
}

int main() {
	vector<player*> redTeam(1);
	vector<player*> blueTeam(1);
	for (int i = 0; i < redTeam.size(); i++) {
		player *a = new player("lupa", "red");
		redTeam[i] = a;
	}

	for (int i = 0; i < blueTeam.size(); i++) {
		player *a = new player("pupa", "blue");
		blueTeam[i] = a;
	}

	while (testForAlive(redTeam, blueTeam) == 0) {
		for (int i = 0; i < redTeam.size(); i++) redTeam[i]->activateEffects();
		if (testForAlive(redTeam, blueTeam) != 0) break;
		for (int i = 0; i < redTeam.size(); i++) redTeam[i]->action(blueTeam);
		if (testForAlive(redTeam, blueTeam) != 0) break;
		for (int i = 0; i < blueTeam.size(); i++) blueTeam[i]->activateEffects();
		if (testForAlive(redTeam, blueTeam) != 0) break;
		for (int i = 0; i < blueTeam.size(); i++) blueTeam[i]->action(redTeam);
	}

	system("pause");
	return 0;
}