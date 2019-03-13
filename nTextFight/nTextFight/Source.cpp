#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>

#define uint unsigned int

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
	player(string name, string team, string spriteName, sf::Vector2i startPos, int id) {

		myID = id;

		myTexture.loadFromFile("./Sprites/Characters/" + spriteName + ".png");
		mySprite.setTexture(myTexture);

		baseHP = 100;
		baseDMG = 10;
		realHP = baseHP;
		currentHP = realHP;
		realDMG = baseDMG;
		myStats.accuracy = 50;
		myStats.agility = 50;
		myStats.intelligence = 50;
		myStats.strenght = 50;

		myTeam = team;
		myName = name;
		position = startPos;

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

	void update(sf::RenderWindow *window, string team, int ID) {
		sf::Vector2i spritePos;
		if (myTeam == "red") spritePos = sf::Vector2i(256 - 64 * position.x, 16 + 128 * position.y);
		if (myTeam == "blue") spritePos = sf::Vector2i(256 + 64 * position.x, 16 + 128 * position.y);
		mySprite.setPosition(spritePos.x, spritePos.y);
		if (team == myTeam && ID == myID) {
			sf::RectangleShape aura(sf::Vector2f(36, 68));
			aura.setPosition(spritePos.x - 2, spritePos.y - 2);
			window->draw(aura);
		}
		window->draw(mySprite);

		sf::RectangleShape healthBar(sf::Vector2f(64, 10));
		sf::RectangleShape activeHeathBar(sf::Vector2f((int)(64 * ((float)currentHP / (float)realHP)), 10));
		healthBar.setPosition(spritePos.x - 16, spritePos.y + 74);
		activeHeathBar.setPosition(spritePos.x - 16, spritePos.y + 74);
		activeHeathBar.setFillColor(sf::Color::Red);
		window->draw(healthBar);
		window->draw(activeHeathBar);
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
	int myID;
	int baseHP, baseDMG;
	int realHP, realDMG;
	int currentHP;
	bool isAlive;
	stats myStats;
	string myTeam;
	string myName;
	list<effect> effects;
	sf::Vector2i position;
	sf::Texture myTexture;
	sf::Sprite mySprite;

	player* chooseTarget(vector<player*> enemy) {
		for (uint i = 0; i < enemy.size(); i++) {

			//choose func//

			return enemy[i];
		}
		return enemy[0];
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
	for (uint i = 0; i < blueTeam.size(); i++) if (blueTeam[i]->getAlive()) isAnyBlueAlive = true;
	for (uint i = 0; i < redTeam.size(); i++) if (redTeam[i]->getAlive()) isAnyRedAlive = true;
	if (isAnyBlueAlive && !isAnyBlueAlive) return 1;
	if (!isAnyBlueAlive && isAnyBlueAlive) return 2;
	if (!isAnyBlueAlive && !isAnyBlueAlive) return -1;
	return 0;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(512, 512), "nText Fight 0.0.1");
	vector<player*> redTeam(2);
	vector<player*> blueTeam(2);
	for (uint i = 0; i < redTeam.size(); i++) {
		if (i == 0) {
			player *a = new player("lupa" + to_string(i), "red", "warrior", sf::Vector2i(1, 1 + i), i);
			redTeam[i] = a;
		}
		else {
			player *a = new player("lupa" + to_string(i), "red", "archer", sf::Vector2i(1, 1 + i), i);
			redTeam[i] = a;
		}
	}

	for (uint i = 0; i < blueTeam.size(); i++) {
		if (i == 1) {
			player *a = new player("pupa" + to_string(i), "blue", "warrior", sf::Vector2i(1, 1 + i), i);
			blueTeam[i] = a;
		}
		else {
			player *a = new player("pupa" + to_string(i), "blue", "mage", sf::Vector2i(1, 1 + i), i);
			blueTeam[i] = a;
		}
	}

	sf::Clock clock;
	sf::Time timer, stepTime;

	int activePlayerID = 0;
	string activeTeam = "red";
	bool newStep = true;

	while (window.isOpen()) {
		timer += clock.getElapsedTime();
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (testForAlive(redTeam, blueTeam) != 0) {

		}
		else if (timer.asMicroseconds() >= 500000) {

			if (activeTeam == "red" && newStep) {
				for (int i = 0; i < redTeam.size(); i++) redTeam[i]->activateEffects();
				newStep = false;
				activePlayerID = 0;
			}
			if (activeTeam == "blue" && newStep) {
				for (int i = 0; i < blueTeam.size(); i++) blueTeam[i]->activateEffects();
				newStep = false;
				activePlayerID = 0;
			}

			window.clear(sf::Color(0, 75, 0));
			for (uint i = 0; i < blueTeam.size(); i++) blueTeam[i]->update(&window, activeTeam, activePlayerID);
			for (uint i = 0; i < blueTeam.size(); i++) redTeam[i]->update(&window, activeTeam, activePlayerID);
			window.display();

			if (activeTeam == "red") {
			    redTeam[activePlayerID]->action(blueTeam);
				activePlayerID++;
				if (activePlayerID >= redTeam.size()) {
					activeTeam = "blue";
					newStep = true;
				}
			}
			else if (activeTeam == "blue") {
			    blueTeam[activePlayerID]->action(redTeam);
				activePlayerID++;
				if (activePlayerID >= blueTeam.size()) {
					activeTeam = "red";
					newStep = true;
				}
			}

			timer = timer.Zero;
		}

	}
	system("pause");
	return 0;
}