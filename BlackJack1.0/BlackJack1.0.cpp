
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
using namespace std;

class Card {
public:
	enum rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };
	enum suit { CLUBS, DIAMONDS, HEARTS, SPADES };

	friend ostream& operator<<(ostream& os, const Card& aCard);
	Card(rank r = ACE, suit s = SPADES, bool ifu = true);
	int GetValue() const;
	void Flip();
private:
	rank m_rank;
	suit m_suit;
	bool m_IsFaceUp;
};

Card::Card(rank r, suit s, bool ifu) :m_rank(r), m_suit(s), m_IsFaceUp(ifu) {

}
int Card::GetValue() const {
	int value = 0;
	if (m_IsFaceUp) {
		value = m_rank;
		if (value > 10)
			value = 10;
		return value;
	}
}
void Card::Flip() {
	m_IsFaceUp = !(m_IsFaceUp);
}

class Hand {
public:
	Hand();
	virtual ~Hand();
	void Add(Card* pCard);
	void Clear();
	int GetTotal() const;
protected:
	vector<Card*>m_cards;
};
Hand::Hand() {
	m_cards.reserve(7);
}
Hand::~Hand() {
	Clear();
}
void Hand::Add(Card* pCard)
{
	m_cards.push_back(pCard);
}
void Hand::Clear() {
	vector<Card*>::iterator iter = m_cards.begin();
	for (iter = m_cards.begin(); iter != m_cards.end(); ++iter) {
		delete* iter;
		*iter = 0;
	}
	m_cards.clear();
}
int Hand::GetTotal() const {
	if (m_cards.empty()) {
		return 0;
	}
	if (m_cards[0]->GetValue() == 0) {
		return 0;
	}
	int total = 0;
	vector<Card*>::const_iterator iter;
	for (iter = m_cards.begin(); iter != m_cards.end(); ++iter) {
		total += (*iter)->GetValue();
	}
	bool containsAce = false;
	for (iter = m_cards.begin(); iter != m_cards.end(); ++iter) {
		if ((*iter)->GetValue() == Card::ACE) {
			containsAce = true;
		}
	}
	if (containsAce && total <= 11) {
		total += 10;
	}
	return total;
}
class GenericPlayer : public Hand {
	friend ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);
public:
	GenericPlayer(const string& name = " ");
	virtual ~GenericPlayer();
	virtual bool IsHitting() const = 0;
	bool IsBusted() const;
	void Bust() const;
protected:
	string m_name;

};
GenericPlayer::GenericPlayer(const string& name) : m_name(name) {
}
GenericPlayer::~GenericPlayer() {
}
bool GenericPlayer::IsBusted() const {
	return (GetTotal() > 21);
}
void GenericPlayer::Bust() const {
	cout << m_name << " busts.\n";
}
class Player : public GenericPlayer {

public:
	Player(const string& name = " ");
	virtual ~Player();
	virtual bool IsHitting() const;
	void Win() const;
	void Lose() const;
	void Push() const;

};

Player::Player(const string& name) :
	GenericPlayer(name)
{}
Player::~Player()
{}
bool Player::IsHitting() const {
	cout << m_name << ", do u want a hit? (Y/N): ";
	char response;
	cin >> response;
	return (response == 'y' || response == 'Y');
}
void Player::Win() const {
	cout << m_name << "wins.\n";
}
void Player::Lose() const {
	cout << m_name << "loses.\n";
}
void Player::Push() const {
	cout << m_name << "pushes.\n";
}

class House : public GenericPlayer {
public:
	House(const string& name = "House");
	virtual ~House();
	//Проверка будет ли игрок брать карты
	virtual bool IsHitting()const;
	//переворачивает первую карту
	void FlipFirstCard();
};

House::House(const string& name) :
	GenericPlayer(name) {
}
House::~House() {
}
bool House::IsHitting()const {
	return (GetTotal() <= 16);
}
void House::FlipFirstCard() {
	if (!(m_cards.empty()))
		m_cards[0]->Flip();

	else
		cout << "No card to flip!\n";
}

class Deck :public Hand {
public:
	Deck();
	virtual ~Deck();
	void Populate();
	void Shuffle();
	void Deal(Hand& aHand);
	void AdditionalCards(GenericPlayer& aGenericPlayer);
};
Deck::Deck() {
	m_cards.reserve(52);
	Populate();
}
Deck::~Deck() {
}
void Deck::Populate() {
	Clear();
	for (int s = Card::CLUBS; s <= Card::SPADES; ++s) {
		for (int r = Card::ACE; r <= Card::KING; ++r) {
			Add(new Card(static_cast<Card::rank>(r), static_cast<Card::suit>(s)));
		}
	}
}
void Deck::Shuffle() {
	//random_shuffle(m_cards.begin(); m_cards.end());
}
void Deck::Deal(Hand& aHand) {
	if (!m_cards.empty()) {
		aHand.Add(m_cards.back());
		m_cards.pop_back();
	}
	else {
		cout << "Out of cards. Unable to deal.";
	}
}
void Deck::AdditionalCards(GenericPlayer& aGenericPlayer) {
	cout << endl;
	while (!(aGenericPlayer.IsBusted()) && aGenericPlayer.IsHitting()) {
		Deal(aGenericPlayer);
		cout << aGenericPlayer << endl;
		if (aGenericPlayer.IsBusted()) {
			aGenericPlayer.Bust();
		}
	}
}
class Game {
public:
	Game(const vector<string>& names);
	~Game();
	void Play();
private:
	Deck m_deck;
	House m_house;
	vector<Player> m_players;
};
Game::Game(const vector<string>& names) {
	vector<string>::const_iterator pName;
	for (pName = names.begin(); pName != names.end(); ++pName) {
		m_players.push_back(Player(*pName));
	}
	srand(static_cast<unsigned int>(time(0)));
	m_deck.Populate();
	m_deck.Shuffle();
}
Game::~Game() {
}
void Game::Play() {
	vector<Player>::iterator pPlayer;
	for (int i = 0; i < 2; ++i) {
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
			m_deck.Deal(*pPlayer);
		}
		m_deck.Deal(m_house);
	}
	m_house.FlipFirstCard();
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
		cout << *pPlayer << endl;
	}
	cout << m_house << endl;
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
		m_deck.AdditionalCards(*pPlayer);
	}
	m_house.FlipFirstCard();
	cout << endl << m_house;
	m_deck.AdditionalCards(m_house);
	if (m_house.IsBusted()) {
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
			if (!(pPlayer->IsBusted())) {
				pPlayer->Win();
			}
		}
	}
	else {
		for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
			if (!(pPlayer->IsBusted())) {
				if (pPlayer->GetTotal() > m_house.GetTotal()) {
					pPlayer->Win();
				}
				else if (pPlayer->GetTotal() < m_house.GetTotal()) {
					pPlayer->Lose();
				}
				else {
					pPlayer->Push();
				}
			}
		}
	}
	for (pPlayer = m_players.begin(); pPlayer != m_players.end(); ++pPlayer) {
		pPlayer->Clear();
	}
	m_house.Clear();
}

ostream& operator<<(ostream& os, const Card& aCard);
ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

ostream& operator<<(ostream& os, const Card& aCard) {
	const string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
	const string SUITS[] = { "c", "d", "h", "s" };
	if (aCard.m_IsFaceUp)
		os << RANKS[aCard.m_rank] << SUITS[aCard.m_suit];
	else
		os << "XX";
	return os;
}

ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer) {
	os << aGenericPlayer.m_name << ":\t";
	vector <Card*>::const_iterator pCard;
	if (!aGenericPlayer.m_cards.empty()) {
		for (pCard = aGenericPlayer.m_cards.begin();
			pCard != aGenericPlayer.m_cards.end();
			++pCard) {
			os << *(*pCard) << "\t";
		}
		if (aGenericPlayer.GetTotal() != 0) {
			cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	}
	else
		os << "<empty>";
	return os;

}

int main()
{
	cout << "\t\tWelcome to BlackJack!\n\n";
	int numPlayers = 0;
	while (numPlayers < 1 || numPlayers > 7) {
		cout << "How many players? (1-7): ";
		cin >> numPlayers;
	}
	vector<string> names;
	string name;
	for (int i = 0; i < numPlayers; ++i) {
		cout << "Enter player name: ";
		cin >> name;
		names.push_back(name);
	}
	cout << endl;
	Game aGame(names);
	char again = 'y';
	while (again != 'n' && again != 'N') {
		aGame.Play();
		cout << "\nDo you want to play again? (Y\N): ";
		cin >> again;
	}
	return 0;
}
