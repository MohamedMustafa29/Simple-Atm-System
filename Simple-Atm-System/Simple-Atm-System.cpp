#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

const string ClientsFileName = "Clients.txt";

struct sClient
{
    string AccountNumber;
    string Name;
    string Pincode;
    double Balance;
    string Phone;
    bool MarkForDelete = false;
};

// ------------------ Utility ------------------

vector<string> SplitString(string str, string Delim = "#//#")
{
    vector<string> vWord;
    size_t Pos = 0;
    string Word;
    while ((Pos = str.find(Delim)) != string::npos)
    {
        Word = str.substr(0, Pos);
        if (Word != "") vWord.push_back(Word);
        str.erase(0, Pos + Delim.length());
    }
    if (str != "") vWord.push_back(str);
    return vWord;
}

sClient ConvertLineToRecord(string Line, string Delim = "#//#")
{
    sClient Client;
    vector<string> vWord = SplitString(Line, Delim);
    Client.AccountNumber = vWord[0];
    Client.Name = vWord[1];
    Client.Pincode = vWord[2];
    Client.Balance = stod(vWord[3]);
    Client.Phone = vWord[4];
    return Client;
}

string ConvertRecordToLine(sClient Client, string Delim = "#//#")
{
    return Client.AccountNumber + Delim + Client.Name + Delim + Client.Pincode + Delim +
        to_string(Client.Balance) + Delim + Client.Phone;
}

// ------------------ File & Data ------------------

vector<sClient> LoadClientsDataFromFile()
{
    vector<sClient> vClients;
    fstream MyFile(ClientsFileName, ios::in);
    if (MyFile.is_open())
    {
        string Line;
        while (getline(MyFile, Line))
        {
            if (Line != "") vClients.push_back(ConvertLineToRecord(Line));
        }
        MyFile.close();
    }
    return vClients;
}

void SaveClientsDataToFile(const vector<sClient>& vClients)
{
    fstream MyFile(ClientsFileName, ios::out);
    if (MyFile.is_open())
    {
        for (const sClient& Client : vClients)
        {
            if (!Client.MarkForDelete)
                MyFile << ConvertRecordToLine(Client) << endl;
        }
        MyFile.close();
    }
}

void UpdateClientBalance(string AccountNumber, double NewBalance, vector<sClient>& vClients)
{
    for (sClient& C : vClients)
    {
        if (C.AccountNumber == AccountNumber)
        {
            C.Balance = NewBalance;
            break;
        }
    }
    SaveClientsDataToFile(vClients);
}

// ------------------ Input ------------------

double ReadAmount()
{
    double Amount;
    cout << "\nEnter amount: ";
    cin >> Amount;
    while (cin.fail() || Amount <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid amount. Enter a positive number: ";
        cin >> Amount;
    }
    return Amount;
}

// ------------------ ATM Operations ------------------

void DepositPage(sClient& CurrentClient, vector<sClient>& vClients)
{
    double Amount = ReadAmount();
    char Answer;
    cout << "\nConfirm deposit? (y/n): "; cin >> Answer;

    if (toupper(Answer) == 'Y')
    {
        CurrentClient.Balance += Amount;
        UpdateClientBalance(CurrentClient.AccountNumber, CurrentClient.Balance, vClients);
        cout << "\n Done. New Balance: " << CurrentClient.Balance << endl;
    }
}

void WithdrawPage(sClient& CurrentClient, vector<sClient>& vClients)
{
    double Amount = ReadAmount();
    if (Amount > CurrentClient.Balance) {
        cout << "\nInsufficient balance! Your balance is: " << CurrentClient.Balance << endl;
        return;
    }

    char Answer;
    cout << "\nConfirm withdraw? (y/n): "; cin >> Answer;

    if (toupper(Answer) == 'Y')
    {
        CurrentClient.Balance -= Amount;
        UpdateClientBalance(CurrentClient.AccountNumber, CurrentClient.Balance, vClients);
        cout << "\nDone. New Balance: " << CurrentClient.Balance << endl;
    }
}

void QuickWithdrawPage(sClient& CurrentClient, vector<sClient>& vClients)
{
    short Choices[] = { 0, 50, 100, 200, 500, 1000 };
    cout << "\n[1] 50  [2] 100\n[3] 200 [4] 500\n[5] 1000 [6] Back\n";

    int choice; cout << "Enter choice: "; cin >> choice;
    if (choice < 1 || choice > 5) return;

    if (Choices[choice] > CurrentClient.Balance) {
        cout << "\nNot enough balance!\n";
    }
    else {
        CurrentClient.Balance -= Choices[choice];
        UpdateClientBalance(CurrentClient.AccountNumber, CurrentClient.Balance, vClients);
        cout << "\nDone. New Balance: " << CurrentClient.Balance << endl;
    }
}

// ------------------ UI ------------------

void ShowMenu()
{
    system("cls");
    cout << "====================================\n";
    cout << "\t   ATM MAIN MENU\n";
    cout << "====================================\n";
    cout << "  [1] Quick Withdraw\n";
    cout << "  [2] Normal Withdraw\n";
    cout << "  [3] Deposit\n";
    cout << "  [4] Check Balance\n";
    cout << "  [5] Logout\n";
    cout << "====================================\n";
}

bool Login(vector<sClient>& vClients, sClient& LoggedClient)
{
    string AccNum, Pin;
    cout << "Enter Account Number: "; cin >> AccNum;
    cout << "Enter Pincode: "; cin >> Pin;

    for (sClient& C : vClients) {
        if (C.AccountNumber == AccNum && C.Pincode == Pin) {
            LoggedClient = C;
            return true;
        }
    }
    return false;
}

void ATMSystem()
{
    vector<sClient> vClients = LoadClientsDataFromFile();
    sClient CurrentClient;


    while (!Login(vClients, CurrentClient)) {
        cout << "\nInvalid Account/Pin. Try again.\n\n";
    }

    int choice = 0;
    while (choice != 5)
    {
        ShowMenu();
        cout << "Choose [1-5]: "; cin >> choice;
        switch (choice)
        {
        case 1: QuickWithdrawPage(CurrentClient, vClients);
            system("pause");
            break;
        case 2: WithdrawPage(CurrentClient, vClients);
            system("pause");
            break;
        case 3: DepositPage(CurrentClient, vClients); 
            system("pause");
            break;
        case 4:
            cout << "\nYour Balance is: " << CurrentClient.Balance << endl;
            system("pause"); break;
        case 5: break;
        }
    }
}

int main()
{
    ATMSystem();
    return 0;
}