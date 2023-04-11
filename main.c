// make so program will work even with the 222692IACB stuff 

#include <stdio.h>

#include <ncurses.h>

#include <stdlib.h>
#include <string.h>

//file process 
#define TRANSACTION_ID_LEN 7
#define DATE_LEN 9
#define ARGS_FILE_ACCOUNT argv[1]
#define ARGS_FILE_TRANSACTIONS argv[2]
#define BUF_LEN 1000

//ncurses 



typedef struct 
{
	char transactionId[TRANSACTION_ID_LEN]; 
	int accountNo; 
	char date[DATE_LEN]; 
	char *balanceChange; 
	char *description; 
	
}transaction;

typedef struct 
{
	int accountNo; 
	char *name; 
	char *accountBalance;
	transaction *transactions; 
	
}account;

//fileprocess
account * ReadAccountData(int *pLineCount, char *fileName);
void SafeFreeAccounts(account *accounts, int lineCnt);
void SafeFreeTransactions(transaction *transactions, int lineCnt); 
int FindAccountForTransaction(int accountNo, account *accounts, int accountCount); 
transaction * ReadTransactionData(int *pLineCount, char *fileName); 
int * MatchAccountsToTransactions(transaction *transactions, account *accounts,
			int accountCount, int transactionCount); 
void ResetArray(int *arr, int arraySize);
 

//ncurses 
void PrintAccounts(account *accounts, int accountCnt, int transactionCnt, int *pTranPerAccount); 
void PrintAccountTransctions(account accounts, int accountCnt, int accountTranCnt); 

//to without arguments hardcode the file names to the program 
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr, "usage %s file\n", argv[0]);
		return (EXIT_FAILURE);
	}
	
	int accountCount = 0; 
	int transactionCount = 0; 
	
	account *accounts = ReadAccountData(&accountCount, ARGS_FILE_ACCOUNT);
	
	transaction *transactions = ReadTransactionData(&transactionCount, ARGS_FILE_TRANSACTIONS); 
	
	int *pTranPerAccount = MatchAccountsToTransactions(transactions, accounts, accountCount, transactionCount); 
	
	initscr(); 
	
	noecho(); 
	
	start_color(); 
	
	keypad(stdscr, TRUE); 
	
	PrintAccounts(accounts, accountCount, transactionCount, pTranPerAccount); 
	
	endwin(); 
	
	SafeFreeTransactions(transactions, transactionCount); 
	SafeFreeAccounts(accounts, accountCount); 
	free(pTranPerAccount); 	
	
	return 0;
}

void PrintAccountTransctions(account accounts, int accountCnt, int accountTranCnt)
{
	curs_set(0); 
	
	//~ int row, col; 
	
	//~ getmaxyx(stdscr, row, col); 
	
	int selected = 0; 
	int exit = 0; 
	int ch; 
	
	while (1)
	{
		mvprintw( 7, 10, "TRANSACTION");
		mvprintw( 8, 2, "============================");
		
		attron(A_BOLD | A_DIM);
		
		mvprintw( 9, 2, "TRANSACTION ID: %s", accounts.transactions[selected].transactionId); 
		mvprintw( 10, 2, "BALANCE CHANGE: %s", accounts.transactions[selected].balanceChange); 
		mvprintw( 11, 2, "TRANSACTION DATE: %s", accounts.transactions[selected].date); 
		mvprintw( 12, 2, "DESCRIPTION: %s", accounts.transactions[selected].description); 
		 
		attroff(A_BOLD | A_DIM);
		
		ch = getch(); 
		
		switch (ch) 
        { 
            case KEY_UP:
                if (selected > 0) 
                {
                    selected--;
                }
                
                break;
            case KEY_DOWN:
                if (selected < accountTranCnt - 1) 
                {
                    selected++;
                } 
                
                break;
            case '\n':
                
                exit = 1; 
                
                break;
        }
        
        refresh();
        
        if (exit == 1)
		{
			break; 
		} 
	}
} 

void PrintAccounts(account *accounts, int accountCnt, int transactionCnt, int *pTranPerAccount)
{
	curs_set(0); 
	
	//~ int row, col; 
	
	//~ getmaxyx(stdscr, row, col); 
	
	int selected = 0; 
	int exit = 0; 
	int ch; 
	
	while (1)
	{
		clear();
		
		mvprintw( 1, 10, "ACCOUNT");
		mvprintw( 2, 2, "============================");
		
		attron(A_BOLD | A_DIM);
		
		mvprintw( 3, 2, "ACCOUNT NUMBER: %d", (accounts + selected)->accountNo); 
		mvprintw( 4, 2, "ACCOUNT NAME: %s", (accounts + selected)->name); 
		mvprintw( 5, 2, "ACCOUNT BALANCE: %s EUR", (accounts + selected)->accountBalance); 
		 
		attroff(A_BOLD | A_DIM);
		
		ch = getch(); 
		
		switch (ch) 
        {		
            case '\n':
            
				PrintAccountTransctions(*(accounts + selected), accountCnt, *(pTranPerAccount + selected)); 
				
				break; 
            case KEY_UP:
                if (selected > 0) 
                {
                    selected--;
                }
                
                break;
            case KEY_DOWN:
                if (selected < accountCnt - 1) 
                {
                    selected++;
                } 
                
                break;
            case ' ':
                
                exit = 1; 
                
                break;
        }
        
        refresh(); 
        
        if (exit == 1)
		{
			break; 
		}
	}
}

void SafeFreeTransactions(transaction *transactions, int lineCnt)
{
	for (int i = 0; i < lineCnt; i++)
	{
		free((transactions + i)->balanceChange);
		free((transactions + i)->description);
	}
	
	free(transactions);
}

transaction * ReadTransactionData(int *pLineCount, char *fileName)
{
	// Current line counter
    int count = 0;

    // Main pointer for the allocated array
    transaction *pArr = NULL;
    // Secondary temporary pointer for the allocated array
    transaction *pTemp = NULL; 
    
    transaction tranBuf;
    
	char balanceChangeBuf[BUF_LEN];
	char descriptionBuf[BUF_LEN];
	
	FILE *finput = fopen(fileName, "r"); 

	if (finput == NULL)
	{
		perror("Couldnt open a input fail\n");
		exit(EXIT_FAILURE);
	}
	
	while (fscanf(finput, "%s %d %s %s %s", tranBuf.transactionId, &tranBuf.accountNo, 
				tranBuf.date, balanceChangeBuf, descriptionBuf) == 5)
	{
		printf("%s %d %s %s %s\n", tranBuf.transactionId, tranBuf.accountNo, 
				tranBuf.date, balanceChangeBuf, descriptionBuf); 
		
		pTemp = (transaction *)realloc(pArr, sizeof(transaction) * (unsigned)(count + 1));
		
		if (pTemp == NULL)
		{
			break; 
		}
		
		*(pTemp + count) = tranBuf;
		
		(pTemp + count)->balanceChange = strdup(balanceChangeBuf);
		(pTemp + count)->description = strdup(descriptionBuf);
		
		pArr = pTemp; 
		
		count++;
	}

	//~ printf("tere1"); 
	
	fclose(finput);
	
    // Store the number of lines trough the pointer
    *pLineCount = count;
    
    return pArr; 
}

int FindAccountForTransaction(int accountNo, account *accounts, int accountCount)
{
	int flag;
	
	for (int i = 0; i < accountCount; i++)
	{
		if (accountNo == (accounts + i)->accountNo)
		{
			flag = i;
		}
	} 
	
	return flag; 
}

int * MatchAccountsToTransactions(transaction *transactions, account *accounts,
			int accountCount, int transactionCount)
{
	int data[transactionCount + 1];
	int *numOfTransPerAccount = (int *)malloc(accountCount * sizeof(int));
	
	ResetArray(data, transactionCount + 1);
	
	int counter = 0; 
	
	for (int i = 0; i < accountCount; i++)
	{
		for (int j = 0; j < transactionCount; j++)
		{
			if ((accounts + i)->accountNo == (transactions + j)->accountNo)
			{
				data[counter] = j;
				counter++;  
				data[transactionCount + 1]++; 
			}
		}
		
		(accounts + i)->transactions = (transaction *)malloc(data[transactionCount + 1]
				* sizeof(transaction)); 

		if ((accounts + i)->transactions == NULL)
		{
			exit(EXIT_FAILURE); 
		}
		
		printf("\n"); 
		
		printf("%d  ::: ", (accounts + i)->accountNo);
		printf("%d  ::: ", data[transactionCount + 1]);
	
		for (int k = 0; k < data[transactionCount + 1]; k++)
		{
			printf("%d", data[k]);
			
			(accounts + i)->transactions[k] = *(transactions + data[k]);
		}
		
		printf("\n"); 
		
		*(numOfTransPerAccount + i) = data[transactionCount + 1]; 
		
		ResetArray(data, transactionCount + 2); 
		counter = 0;  
	}
	
	return numOfTransPerAccount; 
}

void ResetArray(int *arr, int arraySize)
{
	for (int i = 0; i < arraySize; i++)
	{
		*(arr + i) = 0; 
	}
}

void SafeFreeAccounts(account *accounts, int lineCnt)
{
	for (int i = 0; i < lineCnt; i++)
	{
		free((accounts + i)->name);
		free((accounts + i)->accountBalance);
		//~ free((accounts + i)->transactions);
	}
	
	free(accounts);
}


account * ReadAccountData(int *pLineCount, char *fileName)
{
    // Current line counter
    int count = 0;

    // Main pointer for the allocated array
    account *pArr = NULL;
    // Secondary temporary pointer for the allocated array
    account *pTemp = NULL; 
    
    account accBuf; 
	char nameBuf[BUF_LEN];
	char accountBalanceBuf[BUF_LEN];
	
	FILE *finput = fopen(fileName, "r"); 

	if (finput == NULL)
	{
		perror("Couldnt open a input fail\n");
		return 0;
	}
	
	while (fscanf(finput, "%d %s %s", &accBuf.accountNo, nameBuf, accountBalanceBuf) == 3)
	{
		pTemp = (account *)realloc(pArr, sizeof(account) * (unsigned)(count + 1));
		
		if (pTemp == NULL)
		{
			break; 
		}
		
		*(pTemp + count) = accBuf;
		
		(pTemp + count)->name = strdup(nameBuf);
		(pTemp + count)->accountBalance = strdup(accountBalanceBuf);
		
		pArr = pTemp; 
		
		printf("%d %s %s\n", (pArr+count)->accountNo, (pArr+count)->name, (pArr+count)->accountBalance); 
		
		count++; 	
	}

	
	fclose(finput);
	
    // Store the number of lines trough the pointer
    *pLineCount= count;
 
    // Return the pointer to the data
    return pArr;
}
