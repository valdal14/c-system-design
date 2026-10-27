#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

// declaration for the function pointer
struct BankAccount;

typedef void (*BankOperation)(struct BankAccount *self, uint64_t amount);
typedef void (*BalanceCheck)(struct BankAccount *self);

typedef struct BankAccount
{
    BankOperation deposit;
    BankOperation withdraw;
    BalanceCheck check_balance;
    int64_t funds;
} BankAccount;

/**
 * @brief Adds the given amount to the BankAccount founds
 *
 * @param self struct BankAccount pointer
 * @param amount uint64_t
 * @return void
 */
void deposit(struct BankAccount *self, uint64_t amount);

/**
 * @brief Subtracts the given amount to the BankAccount founds
 *
 * @param self struct BankAccount pointer
 * @param amount uint64_t
 * @return void
 */
void withdraw(struct BankAccount *self, uint64_t amount);

/**
 * @brief Prints out the BankAccount founds
 *
 * @param self struct BankAccount pointer
 * @return void
 */
void check_balance(struct BankAccount *self);

/**
 * @brief Allocs a new BankAccount
 * @return void
 */
BankAccount *open(void);

/**
 * @brief Free the  BankAccount
 *
 * @param account struct BankAccount pointer
 * @return void
 */
void close_account(BankAccount *account);

int main(void)
{
    BankAccount *account = open();
    
    account->deposit(account, 300);
    assert(account->funds == 300);

    account->deposit(account, 400);
    assert(account->funds == 700);

    account->withdraw(account, 500);
    assert(account->funds == 200);

    account->check_balance(account);

    close_account(account);

    return 0;
}

void deposit(struct BankAccount *self, uint64_t amount) {
    if (amount == 0 || self == NULL) return;
    printf("Depositing amount: %llu\n", amount);
    self->funds += (int64_t)amount;
}

void withdraw(struct BankAccount *self, uint64_t amount) {
    if ((int64_t)amount > self->funds || amount == 0) return;
    self->funds -= amount;
}

void check_balance(struct BankAccount *self) {
    if (self == NULL) return;
    printf("Current Balance: %lld\n", self->funds);
}

BankAccount *open(void) {
    
    BankAccount *new_account = (BankAccount *)calloc(1, sizeof(BankAccount));
    
    if (new_account == NULL)
    {
        fprintf(stderr, "There was a problem opening the new account\n");
        exit(EXIT_FAILURE);
    }
    
    new_account->deposit = deposit;
    new_account->withdraw = withdraw;
    new_account->check_balance = check_balance;

    return new_account;
}

void close_account(BankAccount *account) {
    if (account == NULL) return;
    free(account);
}
