#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <algorithm>
using namespace std;

int global_limit = 10;

// Forward declarations
class Library;
class Member;
class Transaction;

class Book {
public:
    string book_name;
    int id;
    string type; // "Regular", "E-book", "Journal"
    Book* next;
    bool is_available;
    
    Book() : book_name(""), id(0), next(nullptr), is_available(true), type("Regular") {}
    Book(string name, int id1, string book_type = "Regular") 
        : book_name(name), id(id1), next(nullptr), is_available(true), type(book_type) {}
};

class Member {
public:
    string name;
    int member_id;  // Changed from 'id' to 'member_id' for clarity
    int books_issued;
    Member* next;
    
    Member() : name(""), member_id(0), books_issued(0), next(nullptr) {}
    Member(string n, int id) : name(n), member_id(id), books_issued(0), next(nullptr) {}
};

class Librarian {
public:
    string name;
    int staff_id;
    
    Librarian(string n, int id) : name(n), staff_id(id) {}
    
    void add_book(Library& lib, string name, int id, string type);
    void remove_book(Library& lib, int book_id);
};

class Transaction {
public:
    int transaction_id;
    int member_id;
    int book_id;
    string date;
    string type; // "Issue" or "Return"
    
    Transaction(int tid, int mid, int bid, string d, string t) 
        : transaction_id(tid), member_id(mid), book_id(bid), date(d), type(t) {}
};

class Library {
public:
    Book* head;
    Member* member_head;
    queue<Book*> issuedbooks;
    stack<Transaction> recent_transactions;
    
    Library() : head(nullptr), member_head(nullptr) {}
    
    ~Library() {
        // Clean up books
        Book* current_book = head;
        while(current_book) {
            Book* next_book = current_book->next;
            delete current_book;
            current_book = next_book;
        }
        
        // Clean up members
        Member* current_member = member_head;
        while(current_member) {
            Member* next_member = current_member->next;
            delete current_member;
            current_member = next_member;
        }
    }
    
    void search_book(int id1) {
        try {
            int flag = 0;
            Book* temp = head;
            while(temp) {
                if(temp->id == id1) {
                    cout << "Book found: " << temp->book_name 
                         << " (Type: " << temp->type 
                         << ", Available: " << (temp->is_available ? "Yes" : "No") << ")\n";
                    flag = 1;
                    break;
                }
                temp = temp->next;
            }
            if(flag == 0) {
                throw("INVALID ID NOT FOUND!");
            }
        }
        catch(const char* msg) {
            cout << "EXCEPTION: " << msg << endl;
        }
    }
    
    void issue_book(string name, int member_id) {
        try {
            // Check member exists and hasn't exceeded limit
            Member* mem = member_head;
            bool member_found = false;
            while(mem) {
                if(mem->member_id == member_id) {
                    member_found = true;
                    if(mem->books_issued >= global_limit) {
                        throw("MAXIMUM ISSUE LIMIT REACHED!");
                    }
                    break;
                }
                mem = mem->next;
            }
            if(!member_found) {
                throw("INVALID MEMBER ID!");
            }
            
            Book* temp = head;
            Book* prev = nullptr;
            int flag = 0;
            while(temp) {
                if(temp->book_name == name && temp->is_available) {
                    flag = 1;
                    break;
                }
                prev = temp;
                temp = temp->next;
            }
            
            if(flag == 1) {
                if(temp == head) {
                    head = head->next;
                } else {
                    prev->next = temp->next;
                }
                
                issuedbooks.push(temp);
                temp->is_available = false;
                mem->books_issued++;
                
                // Record transaction
                recent_transactions.push(Transaction(
                    recent_transactions.size() + 1, 
                    member_id, 
                    temp->id, 
                    "2023-11-15", 
                    "Issue"
                ));
                
                cout << "Book issued successfully!\n";
            } else {
                throw("BOOK NOT AVAILABLE!");
            }
        }
        catch(const char* msg) {
            cout << "EXCEPTION: " << msg << endl;
        }
    }
    
    void return_book(string name, int id) {
        try {
            if(issuedbooks.empty()) {
                throw("NO BOOKS TO RETURN!");
            }
            
            Book* returned = issuedbooks.front();
            issuedbooks.pop();
            returned->is_available = true;
            
            // Find member to decrement their count
            Member* mem = member_head;
            while(mem) {
                if(mem->member_id == id) {
                    mem->books_issued--;
                    break;
                }
                mem = mem->next;
            }
            
            // Add book back to available list
            returned->next = head;
            head = returned;
            
            // Record transaction
            recent_transactions.push(Transaction(
                recent_transactions.size() + 1, 
                id, 
                returned->id, 
                "2023-11-15", 
                "Return"
            ));
            
            cout << "RETURN WAS A SUCCESS!\n";
        }
        catch(const char* msg) {
            cout << "EXCEPTION: " << msg << endl;
        }
    }
    
    void display_books() {
        Book* temp = head;
        cout << "Available Books:\n";
        while(temp) {
            if(temp->is_available) {
                cout << "- " << temp->book_name << " (ID: " << temp->id 
                     << ", Type: " << temp->type << ")\n";
            }
            temp = temp->next;
        }
    }
    
    void add_member(string name, int id) {
        Member* new_member = new Member(name, id);
        new_member->next = member_head;
        member_head = new_member;
        cout << "Member added successfully!\n";
    }
    
    // Template function for searching
    template<typename T>
    bool search_item(T* head, int id) {
        T* temp = head;
        while(temp) {
            if(temp->member_id == id) {  // Changed to access member_id
                return true;
            }
            temp = temp->next;
        }
        return false;
    }
    
    // Template function for sorting (using bubble sort)
    template<typename T>
    void sort_by_id(T* &head) {
        if(!head) return;
        
        bool swapped;
        T *ptr1, *lptr = nullptr;
        
        do {
            swapped = false;
            ptr1 = head;
            
            while(ptr1->next != lptr) {
                if(ptr1->member_id > ptr1->next->member_id) {  // Changed to member_id
                    swap(ptr1->member_id, ptr1->next->member_id);
                    swap(ptr1->name, ptr1->next->name);
                    swapped = true;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while(swapped);
    }
};

// Implement Librarian methods after Library definition
void Librarian::add_book(Library& lib, string name, int id, string type) {
    Book* new_book = new Book(name, id, type);
    new_book->next = lib.head;
    lib.head = new_book;
    cout << "Book added successfully!\n";
}

void Librarian::remove_book(Library& lib, int book_id) {
    Book* temp = lib.head;
    Book* prev = nullptr;
    
    while(temp) {
        if(temp->id == book_id) {
            if(prev) {
                prev->next = temp->next;
            } else {
                lib.head = temp->next;
            }
            delete temp;
            cout << "Book removed successfully!\n";
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    cout << "Book not found!\n";
}
// ... [Previous code remains exactly the same until main()]

int main() {
    Library lib;
    Librarian librarian("Admin", 1);
    int choice;
    
    // Sample initialization
    librarian.add_book(lib, "The C++ Programming Language", 1, "Regular");
    librarian.add_book(lib, "Effective Modern C++", 2, "E-book");
    librarian.add_book(lib, "Journal of Computer Science", 3, "Journal");
    lib.add_member("Alice Smith", 1001);
    lib.add_member("Bob Johnson", 1002);

    do { 
        cout << "\n===== Smart Library Management System =====" << endl;
        cout << "1. Librarian Menu" << endl;
        cout << "2. Member Services" << endl;
        cout << "3. View Available Books" << endl;
        cout << "4. Search Book" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1: { // Librarian Menu
                int librarianChoice;
                cout << "\n----- Librarian Menu -----" << endl;
                cout << "1. Add Book" << endl;
                cout << "2. Remove Book" << endl;
                cout << "3. Add Member" << endl;
                cout << "4. View All Books" << endl;
                cout << "5. Back to Main Menu" << endl;
                cout << "Enter choice: ";
                cin >> librarianChoice;
                
                switch(librarianChoice) {
                    case 1: {
                        string name, type;
                        int id;
                        cout << "Enter book name: ";
                        cin.ignore();
                        getline(cin, name);
                        cout << "Enter book ID: ";
                        cin >> id;
                        cout << "Enter book type (Regular/E-book/Journal): ";
                        cin >> type;
                        librarian.add_book(lib, name, id, type);
                        break;
                    }
                    case 2: {
                        int bookId;
                        cout << "Enter book ID to remove: ";
                        cin >> bookId;
                        librarian.remove_book(lib, bookId);
                        break;
                    }
                    case 3: {
                        string name;
                        int id;
                        cout << "Enter member name: ";
                        cin.ignore();
                        getline(cin, name);
                        cout << "Enter member ID: ";
                        cin >> id;
                        lib.add_member(name, id);
                        break;
                    }
                    case 4:
                        lib.display_books();
                        break;
                    case 5:
                        break;
                    default:
                        cout << "Invalid choice!" << endl;
                }
                break;
            }
            case 2: { // Member Services
                int memberId, memberChoice;
                cout << "Enter your member ID: ";
                cin >> memberId;
                
                if(!lib.search_item(lib.member_head, memberId)) {
                    cout << "Invalid member ID!" << endl;
                    break;
                }
                
                cout << "\n----- Member Services -----" << endl;
                cout << "1. Issue Book" << endl;
                cout << "2. Return Book" << endl;
                cout << "3. Back to Main Menu" << endl;
                cout << "Enter choice: ";
                cin >> memberChoice;
                
                switch(memberChoice) {
                    case 1: {
                        string bookName;
                        cout << "Enter book name to issue: ";
                        cin.ignore();
                        getline(cin, bookName);
                        lib.issue_book(bookName, memberId);
                        break;
                    }
                    case 2: {
                        string bookName;
                        cout << "Enter book name to return: ";
                        cin.ignore();
                        getline(cin, bookName);
                        lib.return_book(bookName, memberId);
                        break;
                    }
                    case 3:
                        break;
                    default:
                        cout << "Invalid choice!" << endl;
                }
                break;
            }
            case 3:
                lib.display_books();
                break;
            case 4: {
                int bookId;
                cout << "Enter book ID to search: ";
                cin >> bookId;
                lib.search_book(bookId);
                break;
            }
            case 5:
                cout << "Exiting system. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while(choice != 5);

    return 0;
}