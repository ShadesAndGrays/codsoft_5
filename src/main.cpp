#include <algorithm>
#include <cstring>
#include <ctime>
#include <database.hpp>
#include <exception>
#include <memory>
#include <raylib.h>
#include <raygui.h>
#include <SearchPage.h>
#include <ReturnBook.h>
#include <string>
#include <util.h>
#include <vector>
//#include <iostream>

void displayBooks(GuiSearchPageState &sp, std::shared_ptr<DatabaseManger> db){
    sp.elements.clear();
    std::string filter;
    switch (sp.searchFilter) {
        case TITLE:
            filter = "title";
            break;
        case ISBN:
            filter = "isbn";
            break;
        case AUTHOR:
            filter = "author";
            break;
    }
    // Get all books if none are selected
    std::vector <schema::Book> Books =  std::string(sp.SearchText) == "" ? db->getBooks() : db->getBooks(filter,"%" + std::string(sp.SearchText) + "%",false) ;


    for ( auto &i : Books){
        GuiBookElementState x = {0};
        strncpy(x.title, i.title.c_str(),128);
        strncpy(x.author, i.author.c_str(),128);
        strcpy(x.isbn, i.isbn.c_str());
        x.copies = i.noOfCopies;
        sp.elements.push_back(x);
    }

}
// Re-render logs on Return Book page
void displayLogs(GuiReturnBookState &rb, std::shared_ptr<DatabaseManger> db){

    rb.logs.clear();
    rb.isReturning = false;
    int logNumber = -1;
    try {
        if (std::string(rb.SearchBoxText) != "") 
            logNumber = std::stoi(rb.SearchBoxText);
    }catch(std::exception e){
        rb.logs.clear();
        return;
    }
    std::vector<schema::Log> logs = logNumber == -1 ? db->getBorrowLog() : db->getBorrowLog(logNumber);
    int uniqueLog = -1;
    // Filter through and only add unique bookIds
    for(auto &log : logs){
        if (uniqueLog != log.borrowId){
            uniqueLog = log.borrowId;
            rb.logs.push_back(log);
        }

    }
}


struct PopUpState{
    bool on;
    char title [32];
    char msg[128];
};
void popup(std::string title, std::string msg, PopUpState* popUpState){

    strncpy(popUpState->title, title.c_str(), sizeof(char) * 32);
    strncpy(popUpState->msg, msg.c_str(), sizeof(char) * 128);
    popUpState->on = true;
}
void GuiPopUp(PopUpState* popUpState){

    if (!popUpState->on)
        return;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,60});
    popUpState->on = !GuiWindowBox(scaleDynamic((Rectangle){SIZE_X/4.0 , SIZE_Y/4.0 ,300,300}), popUpState->title);
    GuiLabel(scaleDynamic((Rectangle){SIZE_X/4.0 + 100, SIZE_Y/4.0 ,300,300}),popUpState->msg);
}
void refreshInfopage(GuiReturnBookState &rb, std::shared_ptr<DatabaseManger> db){
    rb.refreshInfo = false;
    memset(rb.InfoBoxText, 0, sizeof(rb.InfoBoxText));

    // Get all Borrow log and find all bookd relating to ISBN
    std::vector<schema::Log> logs= db->getBorrowLog(rb.selectedLog);
    std::vector<std::string> isbnOfBooks(logs.size());
    std::transform(logs.begin(),logs.end(),isbnOfBooks.begin(),[](schema::Log logs){return logs.isbn;});
    std::vector<schema::Book> books = db->getBooks(isbnOfBooks);

    std::string txt = "";
    for (auto i =0 ; i < books.size(); i++){
        txt += "Title: " + books[i].title;
        txt += "\nAuthor: " + books[i].author;
        txt += "\n\n";
    }

    auto timeDiffenrece = [](time_t startDate){
        time_t endDate = time(NULL);
        long timeLate = endDate - startDate;
        timeLate /= 60 * 60 * 24; // sec to Days 
        return timeLate; 
    };

    auto calculateFine = [&](time_t startDate){
        return std::to_string(timeDiffenrece(startDate) * 100);
    };
    txt += "\n\nDate Borrowed: "+ std::string(timeToHuman(logs[0].borrowDate)) +"\n";

    if (logs[0].hasReturned){
        txt += "Book has been returned";
    } else{
        int days = timeDiffenrece(logs[0].borrowDate);
        //Calculates log fine if one week has passed since borrowed
        if (days > 7){
            txt += std::string("Over Due By: ") + std::to_string(timeDiffenrece(logs[0].borrowDate)) +std::string("\nFine: N ") + calculateFine(logs[0].borrowDate);
            txt+= "\n100 naira per Over Due Day\n";
        } else{
            // Borrower has a few days left
            txt +=  std::to_string( 7 - timeDiffenrece(logs[0].borrowDate)) + " Days left till overdue";
        }
    }
    strncat(rb.InfoBoxText,txt.c_str(),500);

}

int main() {
    std::shared_ptr<DatabaseManger> db = DatabaseManger::getInstance();

    InitWindow(SIZE_X,SIZE_X,"BookKeeper");

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    GuiSearchPageState sp = InitGuiSearchPage(); 
    GuiReturnBookState rb = InitGuiReturnBook();

    PopUpState ps {0};

    const char* pages[2] = {"SEARCH","RETURN"};
    int currentPage = 0;

    displayBooks(sp,db);
    displayLogs(rb,db);

    while(!WindowShouldClose()){
        // My poor attempt as scaling the fonts
        GuiSetStyle(DEFAULT,TEXT_SIZE, (float)GetScreenWidth()/SIZE_X * 9);
        GuiSetStyle(DEFAULT,TEXT_SPACING, (float)GetScreenWidth()/SIZE_X * 2);
        GuiSetStyle(DEFAULT,TEXT_LINE_SPACING, (float)GetScreenWidth()/SIZE_X * 11);

        if (sp.Exit){
            break;
        }
        if (rb.Exit){
            break;
        }
        /*  ####### SEARCH PAGE ###### */
        if(sp.SearchButtonPressed){
            displayBooks(sp,db);
            if (sp.elements.size() == 0)
                popup("Not Found", "The Book was not found", &ps);
        }
        if (sp.checkOutBtn){
            auto books = std::vector<std::string> (sp.checkingOut.begin(),sp.checkingOut.end());
            db->borrowBook(books);
            sp.checkingOut.clear();
            sp.checkOutBtn = false;
            displayLogs(rb,db);
            displayBooks(sp,db);
            char  noOfBooks[30];
            strcat(noOfBooks,  std::to_string(books.size()).c_str());
            strcat(noOfBooks, " borrowed book(s)" );
            popup("Borrow Book", noOfBooks, &ps);
        }

        /*  ####### END SEARCH PAGE ###### */

        /*  ####### REUTRN PAGE ###### */

        if(rb.returnBookPressed){
            schema::Log log = rb.logs[rb.selectedLog];
            db->returnBook(log.borrowId);
            refreshInfopage(rb,db);

            displayLogs(rb,db);
            displayBooks(sp,db);
        }

        if (rb.SearchLogBtnPressed){
            displayLogs(rb,db);
            if (rb.logs.size() == 0)
                popup("Not Found", "The Log was not found", &ps);
        }

        if (rb.refreshInfo){
            refreshInfopage(rb,db);
        }

        /*  ####### END REUTRN PAGE ###### */

        // Main drawing loop
        BeginDrawing();
        ClearBackground(RAYWHITE);
        //Selective Rendering
        if(currentPage == 0){
            GuiSearchPage(&sp);
        } else if(currentPage == 1){
            GuiReturnBook(&rb);
        }
        GuiPopUp(&ps); // little popup mechanic
        GuiTabBar(scaleDynamic((Rectangle){0,0,15,15}),pages ,2,&currentPage);
        EndDrawing();
    }
}
