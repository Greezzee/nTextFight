#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cmath>
#include <SFML/Graphics.hpp>
#define uint unsigned int
using namespace std;

int msStepTime = 500000;


struct attackPackage {
	int DMG;
	int successChange;
};
struct effect {
	int ID, duration = 0;
	int level;
};
struct skills {
	int ID, 
};
struct stats
{
	int strenght, agility, accuracy, intelligence;
};

float sigmoid(float x) {
	return pow(1 + exp(-x), -1);
}

class player {
public:
	player(string name, string team, string spriteName, sf::Vector2i startPos, int id) {

		myID = id;

		myTexture.loadFromFile("./Sprites/Characters/" + spriteName + ".png");
		punchTexture.loadFromFile("./Sprites/Effects/std_punch.png");
		mySprite.setTexture(myTexture);
		punchSprite.setTexture(punchTexture);

		baseHP = 100;
		baseDMG = 10;
		realHP = baseHP;
		currentHP = realHP;
		realDMG = baseDMG;
		myStats.accuracy = 100;
		myStats.agility = 0;
		myStats.intelligence = 50;
		myStats.strenght = 50;

		myTeam = team;
		myName = name;
		position = startPos;

		effects.resize(0);

		isAlive = true;
		isActionDone = false;

		if (myTeam == "red") spritePos = sf::Vector2i(256 - 64 * position.x, 16 + 128 * position.y);
		if (myTeam == "blue") spritePos = sf::Vector2i(256 + 64 * position.x, 16 + 128 * position.y);
	}

	void action(vector<player*> enemy, sf::RenderWindow *window, sf::Time thisStep, bool isNewStep) {
		
		if (isNewStep) target = chooseTarget(enemy);

		punchSprite.setTextureRect(sf::IntRect(64 * (int)(thisStep.asMicroseconds() * 4 / msStepTime), 0, 64, 64));
		punchSprite.setPosition(target->getPosition().x - 16, target->getPosition().y);
		window->draw(punchSprite);

		sf::RectangleShape aura(sf::Vector2f(36, 68));
		aura.setFillColor(sf::Color(0, 0, 0, 0));
		aura.setOutlineColor(sf::Color(255, 255, 255, 255));
		aura.setOutlineThickness(-2);
		aura.setPosition(spritePos.x - 2, spritePos.y - 2);
		window->draw(aura);



		if (!isActionDone && thisStep.asMicroseconds() > msStepTime / 2) {
			attackPackage myAttack;
			myAttack.DMG = realDMG;
			myAttack.successChange = myStats.accuracy;
			cout << myName + "(" + to_string(currentHP) + ")" << " attack ";
			target->takeAttack(myAttack);
			isActionDone = true;
		}
	}

	void update(sf::RenderWindow *window) {

		mySprite.setPosition(spritePos.x, spritePos.y);
		window->draw(mySprite);

		sf::RectangleShape healthBar(sf::Vector2f(64, 10));
		sf::RectangleShape activeHeathBar(sf::Vector2f((int)(64 * ((float)currentHP / (float)realHP)), 10));
		healthBar.setPosition(spritePos.x - 16, spritePos.y + 74);
		activeHeathBar.setPosition(spritePos.x - 16, spritePos.y + 74);
		activeHeathBar.setFillColor(sf::Color::Red);
		window->draw(healthBar);
		window->draw(activeHeathBar);
	}

	void refresh() {
		isActionDone = false;
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
	sf::Vector2f getPosition() {
		return mySprite.getPosition();
	}

private:
	int myID;
	int baseHP, baseDMG;
	int realHP, realDMG;
	int currentHP;
	bool isAlive, isActionDone;
	stats myStats;
	string myTeam;
	string myName;
	list<effect> effects;
	sf::Vector2i position;
	sf::Texture myTexture, punchTexture;
	sf::Sprite mySprite, punchSprite;
	sf::Vector2i spritePos;
	player* target;
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
	sf::Time timer, stepTimer;

	int activePlayerID = 0;
	string activeTeam = "red";
	bool isNewStep = true;
	while (window.isOpen()) {
		timer += clock.getElapsedTime();
		stepTimer += timer;
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(0, 100, 0));
		if (stepTimer.asMicroseconds() >= msStepTime) {
			stepTimer = stepTimer.Zero;
			isNewStep = true;
			if (activeTeam == "red") {
				redTeam[activePlayerID]->refresh();
				if (activePlayerID == redTeam.size() - 1) {
					activePlayerID = 0;
					activeTeam = "blue";
				}
				else activePlayerID++;
			}
			else if (activeTeam == "blue") {
				blueTeam[activePlayerID]->refresh();
				if (activePlayerID == blueTeam.size() - 1) {
					activePlayerID = 0;
					activeTeam = "red";
				}
				else activePlayerID++;
			}

		}

		for (uint i = 0; i < redTeam.size(); i++) redTeam[i]->update(&window);
		for (uint i = 0; i < blueTeam.size(); i++) blueTeam[i]->update(&window);

		if (activeTeam == "red") redTeam[activePlayerID]->action(blueTeam, &window, stepTimer, isNewStep);
		else if (activeTeam == "blue") blueTeam[activePlayerID]->action(redTeam, &window, stepTimer, isNewStep);

		window.display();
		timer = timer.Zero;
		isNewStep = false;
	}
	system("pause");
	return 0;
}