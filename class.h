#pragma once
#include <string>
#include <vector>
#include <iostream>

class BankSystem;

struct Transaction {
  std::string type;
  std::string details;
  double amount;
};

class Account {
private:
  std::string login;
  std::string password;
  std::string cardNumber;
  double balance;
  double savingsBalance;
  double creditDebt;
  bool loggedIn;
  std::vector<Transaction> history;

  static std::string generateCardNumber();
  void addHistory(const std::string& type, const std::string& details, double amount);

public:
  Account();
  Account(const std::string& login, const std::string& password, double initialBalance = 0.0);

  const std::string& getLogin() const;
  const std::string& getCardNumber() const;
  double getBalance() const;
  double getSavingsBalance() const;
  double getCreditDebt() const;
  bool isLoggedIn() const;

  bool checkPassword(const std::string& pass) const;
  void setLoggedIn(bool value);

  void deposit(double amount);
  bool withdraw(double amount);
  bool pay(double amount);
  void topUp(double amount);

  bool transferTo(Account& other, double amount);
  bool createTransferRequest(const std::string& toCard, double amount, BankSystem& bank);

  void printHistory() const;

  void currencyToUSD(double rubles) const;
  void currencyToKZT(double rubles) const;

  void savingsDeposit(double amount);
  bool savingsWithdraw(double amount);
  void savingsGrow();
  void printSavings() const;

  void takeCredit(double amount);
  bool payCredit(double amount);
  void growCredit();
  void printCredit() const;
};

struct TransferRequest {
  std::string fromCard;
  std::string toCard;
  double amount;
};

class BankSystem {
private:
  std::vector<Account> accounts;
  std::vector<TransferRequest> requests;

public:
  BankSystem();

  Account* findByLogin(const std::string& login);
  Account* findByCard(const std::string& card);
  const Account* findByCard(const std::string& card) const;

  bool registerAccount(const std::string& login, const std::string& password);
  bool login(const std::string& login, const std::string& password, Account*& outAccount);
  void addDefaultAccounts();

  void addRequest(const std::string& fromCard, const std::string& toCard, double amount);
  void showRequestsFor(const std::string& card) const;
  bool acceptRequest(const std::string& card, int index);
  bool rejectRequest(const std::string& card, int index);

  std::vector<Account>& getAccounts();
};