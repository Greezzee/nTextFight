#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#define uint unsigned int
using namespace std;

int msStepTime = 500000;
const float PI = 3.1415926f;

struct attackPackage {
	int DMG;
	int successChange;
};
struct effect {
	int ID, duration = 0;
	int level;
};
struct skill {
	int ID;
};
struct stats
{
	int strenght, agility, accuracy, intelligence;
	int baseHP, baseDMG;
	int realHP, realDMG;
	int currentHP;
};
struct playerInfo {
	string name, spriteName;
	stats playerStats;
	sf::Vector2i startPos;
	vector<skill> skills;
	int id;
};

float sigmoid(float x) {
	return pow(1 + exp(-x), -1);
}

class player {
public:
	player(playerInfo input) {

		myID = input.id;

		myTexture.loadFromFile("./Sprites/Characters/" + input.spriteName + ".png");
		punchTexture.loadFromFile("./Sprites/Effects/std_punch.png");
		mySprite.setTexture(myTexture);
		punchSprite.setTexture(punchTexture);

		myStats = input.playerStats;
		myStats.realHP = myStats.baseHP;
		myStats.currentHP = myStats.realHP;
		myStats.realDMG = myStats.baseDMG;

		myName = input.name;
		position = input.startPos;

		effects.resize(0);

		isAlive = true;
		isActionDone = false;
	}

	void action(vector<player*> enemy, sf::RenderWindow *window, sf::Time thisStep, bool isNewStep) {
		
		if (isNewStep) {
			target = chooseTarget(enemy);
			myAttack.DMG = myStats.realDMG;
			myAttack.successChange = myStats.accuracy;
			isAttackSuccess = !target->testForMiss(myAttack.successChange);
		}

		if (isAttackSuccess) {
			punchSprite.setTextureRect(sf::IntRect(64 * (int)(thisStep.asMicroseconds() * 4 / msStepTime), 0, 64, 64));
			punchSprite.setPosition(target->getPosition().x - 16, target->getPosition().y);
			window->draw(punchSprite);
		}
		else {
			sf::Font font;
			font.loadFromFile("font.ttf");
			sf::Text evadeText("Evade!", font);
			int alpha = 255;
			int test = thisStep.asMicroseconds();
			if ((float)(msStepTime - test) / (float)msStepTime <= 0.25)
				alpha = (int)(((float)(msStepTime - test) / (float)msStepTime * 4) * 255.f);
			if ((float)(msStepTime - test) / (float)msStepTime >= 0.75)
				alpha = (int)(((1.f - (float)abs(test - msStepTime) / (float)msStepTime) * 4) * 255.f);
			evadeText.setFillColor(sf::Color(255, 255, 255, alpha));
			evadeText.setOutlineColor(sf::Color(255, 255, 255, alpha));
			evadeText.setPosition(target->getPosition().x - 36, target->getPosition().y + 24 - 16 * sin((float)thisStep.asMicroseconds() / (float)msStepTime * PI));
			window->draw(evadeText);
		}


		sf::RectangleShape aura(sf::Vector2f(36, 68));
		aura.setFillColor(sf::Color(0, 0, 0, 0));
		aura.setOutlineColor(sf::Color(255, 255, 255, 255));
		aura.setOutlineThickness(-2);
		aura.setPosition((float)spritePos.x - 2.f, (float)spritePos.y - 2.f);
		window->draw(aura);



		if (!isActionDone && thisStep.asMicroseconds() > msStepTime / 2 && isAttackSuccess) {
			cout << myName + "(" + to_string(myStats.currentHP) + ")" << " attack ";
			target->takeAttack(myAttack);
			isActionDone = true;
		}
		else if (!isActionDone && thisStep.asMicroseconds() > msStepTime / 2) {
			cout << myName + "(" + to_string(myStats.currentHP) + ")" << " attack ";
			cout << "but " << target->getName() << " evade" << endl;
			isActionDone = true;
		}
	}

	void update(sf::RenderWindow *window) {

		if (myTeam == "red") spritePos = sf::Vector2i(280 - 80 * (position.x + 1), 16 + 128 * position.y);
		if (myTeam == "blue") spritePos = sf::Vector2i(280 + 80 * (position.x + 1), 16 + 128 * position.y);
		mySprite.setPosition((float)spritePos.x, (float)spritePos.y);
		window->draw(mySprite);

		sf::RectangleShape healthBar(sf::Vector2f(64, 10));
		sf::RectangleShape activeHeathBar(sf::Vector2f((float)((int)(64 * ((float)myStats.currentHP / (float)myStats.realHP))), 10.f));
		healthBar.setPosition((float)spritePos.x - 16.f, (float)spritePos.y + 74.f);
		activeHeathBar.setPosition((float)spritePos.x - 16.f, (float)spritePos.y + 74.f);
		activeHeathBar.setFillColor(sf::Color::Red);
		window->draw(healthBar);
		window->draw(activeHeathBar);
	}

	void refresh() {
		isActionDone = false;
	}

	void takeAttack(attackPackage attack) {
		cout << "and " << myName + "(" + to_string(myStats.currentHP) + "->" + to_string(myStats.currentHP - attack.DMG) + ")" << " take " << attack.DMG << " damage" << endl;
		myStats.currentHP -= attack.DMG;
		if (myStats.currentHP <= 0) isAlive = false;
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
					myStats.currentHP += 5;
					if (myStats.currentHP > myStats.realHP) myStats.currentHP = myStats.realHP;
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

	void setTeam(string team) {
		myTeam = team;
	}
	bool getAlive() {
		return isAlive;
	}
	sf::Vector2f getPosition() {
		return mySprite.getPosition();
	}
	stats getStats() {
		return myStats;
	}
	string getName() {
		return myName;
	}
	bool testForMiss(int successChange) {
		float realChange = (float)(myStats.agility - successChange) / 25.f;
		realChange = sigmoid(realChange) * 100;
		if (rand() % 100 + 1 < realChange) return true;
		else return false;	
	}
private:
	int myID;
	bool isAlive, isActionDone, isAttackSuccess;
	stats myStats;
	attackPackage myAttack;
	string myTeam;
	string myName;
	list<effect> effects;
	vector<skill> mySkils;
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
playerInfo readPlayerFile(ifstream *file) {
	playerInfo out;
	*file >> out.spriteName;
	*file >> out.playerStats.baseHP >> out.playerStats.baseDMG;
	*file >> out.playerStats.strenght >> out.playerStats.agility >> out.playerStats.intelligence >> out.playerStats.accuracy;
	return out;
}


int main() {
	vector<player*> redTeam(0);
	vector<player*> blueTeam(0);

	ifstream battleFile;
	battleFile.open("data/battle");
	string input = "";
	for (int i = 0; i < 4; i++) for (int j = 0; j < 6; j++) {
		battleFile >> input;
		if (input == "-") continue;

		ifstream playerFile;
		playerFile.open("data/Players/" + input + ".txt");
		playerInfo info = readPlayerFile(&playerFile);
		info.name = input;
		info.id = i * 6 + j;
		if (j < 3) {
			info.startPos = sf::Vector2i(2 - j, i);
			player *a = new player(info);
			a->setTeam("red");
			redTeam.push_back(a);
		}
		else {
			info.startPos = sf::Vector2i(j - 3, i);
			player *a = new player(info);
			a->setTeam("blue");
			blueTeam.push_back(a);
		}
	}


	sf::Clock clock;
	sf::Time timer, stepTimer;

	int activePlayerID = 0;
	string activeTeam = "red";
	bool isNewStep = true;


	sf::RenderWindow window(sf::VideoMode(560, 512), "nText Fight 0.0.1");
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