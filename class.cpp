#include "class.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <random>

static std::mt19937 rng((std::random_device())());

std::string Account::generateCardNumber() {
  std::uniform_int_distribution<int> dist(0, 9);
  std::string num;
  for (int i = 0; i < 16; ++i) num += char('0' + dist(rng));
  return num;
}

void Account::addHistory(const std::string& type, const std::string& details, double amount) {
  history.push_back({ type, details, amount });
}

Account::Account()
  : login(""), password(""), cardNumber(generateCardNumber()), balance(0.0),
  savingsBalance(0.0), creditDebt(0.0), loggedIn(false) {
}

Account::Account(const std::string& login, const std::string& password, double initialBalance)
  : login(login), password(password), cardNumber(generateCardNumber()), balance(initialBalance),
  savingsBalance(0.0), creditDebt(0.0), loggedIn(false) {
}

const std::string& Account::getLogin() const { return login; }
const std::string& Account::getCardNumber() const { return cardNumber; }
double Account::getBalance() const { return balance; }
double Account::getSavingsBalance() const { return savingsBalance; }
double Account::getCreditDebt() const { return creditDebt; }
bool Account::isLoggedIn() const { return loggedIn; }
bool Account::checkPassword(const std::string& pass) const { return password == pass; }
void Account::setLoggedIn(bool value) { loggedIn = value; }

void Account::deposit(double amount) {
  if (amount <= 0) return;
  balance += amount;
  addHistory("Пополнение", "На карту", amount);
}

bool Account::withdraw(double amount) {
  if (amount <= 0 || balance < amount) return false;
  balance -= amount;
  addHistory("Списание", "С карты", amount);
  return true;
}

bool Account::pay(double amount) { return withdraw(amount); }
void Account::topUp(double amount) { deposit(amount); }

bool Account::transferTo(Account& other, double amount) {
  if (amount <= 0 || balance < amount) return false;
  balance -= amount;
  other.balance += amount;
  addHistory("Перевод", "На карту " + other.cardNumber, amount);
  other.addHistory("Перевод", "С карты " + cardNumber, amount);
  return true;
}

bool Account::createTransferRequest(const std::string& toCard, double amount, BankSystem& bank) {
  if (amount <= 0) return false;
  bank.addRequest(cardNumber, toCard, amount);
  addHistory("Запрос", "Попросил сумму у карты " + toCard, amount);
  return true;
}

void Account::printHistory() const {
  std::cout << "История операций для карты " << cardNumber << "\n";
  if (history.empty()) {
    std::cout << "Пусто\n";
    return;
  }
  for (size_t i = 0; i < history.size(); ++i) {
    std::cout << i + 1 << ". " << history[i].type << " | "
      << history[i].details << " | " << history[i].amount << " RUB\n";
  }
}

void Account::currencyToUSD(double rubles) const {
  std::cout << rubles << " RUB = " << rubles / 90.0 << " USD\n";
}

void Account::currencyToKZT(double rubles) const {
  std::cout << rubles << " RUB = " << rubles / 0.20 << " KZT\n";
}

void Account::savingsDeposit(double amount) {
  if (amount <= 0 || balance < amount) return;
  balance -= amount;
  savingsBalance += amount;
  addHistory("Накопительный счет", "Пополнение", amount);
}

bool Account::savingsWithdraw(double amount) {
  if (amount <= 0 || savingsBalance < amount) return false;
  savingsBalance -= amount;
  balance += amount;
  addHistory("Накопительный счет", "Вывод", amount);
  return true;
}

void Account::savingsGrow() {
  savingsBalance *= 1.005;
  addHistory("Накопительный счет", "Рост 0.5%", savingsBalance);
}

void Account::printSavings() const {
  std::cout << "Накопительный счет: " << savingsBalance << " RUB\n";
}

void Account::takeCredit(double amount) {
  if (amount <= 0) return;
  balance += amount;
  creditDebt += amount;
  addHistory("Кредит", "Выдан", amount);
}

bool Account::payCredit(double amount) {
  if (amount <= 0 || balance < amount || creditDebt <= 0) return false;
  double p = std::min(amount, creditDebt);
  balance -= p;
  creditDebt -= p;
  addHistory("Кредит", "Погашение", p);
  return true;
}

void Account::growCredit() {
  creditDebt *= 1.005;
  addHistory("Кредит", "Рост долга 0.5%", creditDebt);
}

void Account::printCredit() const {
  std::cout << "Долг по кредиту: " << creditDebt << " RUB\n";
}

BankSystem::BankSystem() {
  addDefaultAccounts();
}

Account* BankSystem::findByLogin(const std::string& login) {
  for (auto& a : accounts)
    if (a.getLogin() == login) return &a;
  return nullptr;
}

Account* BankSystem::findByCard(const std::string& card) {
  for (auto& a : accounts)
    if (a.getCardNumber() == card) return &a;
  return nullptr;
}

const Account* BankSystem::findByCard(const std::string& card) const {
  for (const auto& a : accounts)
    if (a.getCardNumber() == card) return &a;
  return nullptr;
}

bool BankSystem::registerAccount(const std::string& login, const std::string& password) {
  if (login.empty() || password.empty() || findByLogin(login)) return false;
  accounts.emplace_back(login, password, 0.0);
  return true;
}

bool BankSystem::login(const std::string& login, const std::string& password, Account*& outAccount) {
  Account* a = findByLogin(login);
  if (!a || !a->checkPassword(password)) return false;
  outAccount = a;
  a->setLoggedIn(true);
  return true;
}

void BankSystem::addDefaultAccounts() {
  accounts.emplace_back("demo1", "1111", 5000.0);
  accounts.emplace_back("demo2", "2222", 3000.0);
  accounts.emplace_back("demo3", "3333", 10000.0);
}

void BankSystem::addRequest(const std::string& fromCard, const std::string& toCard, double amount) {
  requests.push_back({ fromCard, toCard, amount });
}

void BankSystem::showRequestsFor(const std::string& card) const {
  bool any = false;
  for (size_t i = 0; i < requests.size(); ++i) {
    if (requests[i].toCard == card) {
      any = true;
      std::cout << i << ". Пользователь с картой " << requests[i].fromCard
        << " просит у вас " << requests[i].amount << " RUB\n";
    }
  }
  if (!any) std::cout << "Нет запросов\n";
}

bool BankSystem::acceptRequest(const std::string& card, int index) {
  if (index < 0 || index >= (int)requests.size() || requests[index].toCard != card) return false;

  Account* requester = findByCard(requests[index].fromCard);
  Account* owner = findByCard(requests[index].toCard);

  if (!requester || !owner || owner->getBalance() < requests[index].amount) return false;

  owner->transferTo(*requester, requests[index].amount);
  requests.erase(requests.begin() + index);
  return true;
}

bool BankSystem::rejectRequest(const std::string& card, int index) {
  if (index < 0 || index >= (int)requests.size() || requests[index].toCard != card) return false;
  requests.erase(requests.begin() + index);
  return true;
}

std::vector<Account>& BankSystem::getAccounts() {
  return accounts;
}