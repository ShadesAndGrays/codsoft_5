/*******************************************************************************************
*
*   ReturnBook v1.0.0 - 
*
*   MODULE USAGE:
*       #define GUI_RETURNBOOK_IMPLEMENTATION
*       #include "gui_ReturnBook.h"
*
*       INIT: GuiReturnBookState state = InitGuiReturnBook();
*       DRAW: GuiReturnBook(&state);
*
*
**********************************************************************************************/

#include "database.hpp"
#include "raylib.h"
#include "util.h"
#include <string>
#include <vector>

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_RETURNBOOK_H
#define GUI_RETURNBOOK_H

typedef struct {
    bool Exit;
    bool SearchBoxEditMode;
    char SearchBoxText[128];
    bool SearchLogBtnPressed;
    Rectangle ScrollPanel005ScrollView;
    Vector2 ScrollPanel005ScrollOffset;
    Vector2 ScrollPanel005BoundsOffset;
    bool InfoBoxEditMode;
    char InfoBoxText[500];
    bool returnBookPressed;
    bool LabelButton010Pressed;
    bool Button011Pressed;

    // Custom state variables 
    std::vector<schema::Log> logs; 
    bool isReturning;
    bool refreshInfo;
    int selectedLog;


} GuiReturnBookState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiReturnBookState InitGuiReturnBook(void);
void GuiReturnBook(GuiReturnBookState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_RETURNBOOK_H

/***********************************************************************************
*
*   GUI_RETURNBOOK IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_RETURNBOOK_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiReturnBookState InitGuiReturnBook(void)
{
    GuiReturnBookState state = { 0 };
    state.Exit = false;
    state.SearchBoxEditMode = false;
    strcpy(state.SearchBoxText, "");
    state.SearchLogBtnPressed = false;
    state.ScrollPanel005ScrollView = (Rectangle){ 0, 0, 0, 0 };
    state.ScrollPanel005ScrollOffset = (Vector2){ 0, 0 };
    state.ScrollPanel005BoundsOffset = (Vector2){ 0, 0 };
    state.InfoBoxEditMode = false;
    strcpy(state.InfoBoxText, "");
    state.returnBookPressed = false;
    state.LabelButton010Pressed = false;
    state.Button011Pressed = false;

    state.selectedLog = -1;
    state.logs = {};
    state.isReturning = false;
    state.refreshInfo = false;

    for (auto i = 0; i < 20; i++)
        state.logs.push_back(schema::Log{0});

    return state;
}

void GuiReturnBook(GuiReturnBookState *state)
{
    GuiPanel(scaleDynamic((Rectangle){ 0, 0, 696, 472 }), NULL);
    if (GuiTextBox(scaleDynamic((Rectangle){ 72, 72, 456, 24 }), state->SearchBoxText, 128, state->SearchBoxEditMode)) state->SearchBoxEditMode = !state->SearchBoxEditMode;
    state->SearchLogBtnPressed = GuiButton(scaleDynamic((Rectangle){ 544, 72, 56, 24 }), "Search"); 

    Rectangle scrollArea = scaleDynamic((Rectangle){ 72, 120, 280 - state->ScrollPanel005BoundsOffset.x, 280 - state->ScrollPanel005BoundsOffset.y });
    Rectangle scrollView = scaleDynamic(state->ScrollPanel005ScrollView);
    GuiScrollPanel(scrollArea, NULL, scaleDynamic((Rectangle){ 72, 120, 280, 280 + (float)state->logs.size() * 20 }), &state->ScrollPanel005ScrollOffset,&scrollView);

    GuiLabel(scaleDynamic((Rectangle){ 104, 24, 456, 32 }), "Return Book");
    state->Exit = GuiButton(scaleDynamic((Rectangle){ 24, 24, 32, 24 }), "X"); 
    
    // Info of log
    auto r1 = scaleDynamic((Rectangle){ 367, 151, 234, 250 });
    BeginScissorMode( r1.x,r1.y,r1.width,r1.height);
    auto sizeStyle = GuiGetStyle(DEFAULT,TEXT_SIZE);
    auto alignmentStyle = GuiGetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL);
    GuiSetStyle(DEFAULT,TEXT_SIZE, (float)GetScreenWidth()/SIZE_X * 7);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
    GuiTextBox(scaleDynamic((Rectangle){ 368, 152, 232, 248 }), state->InfoBoxText, 200, false);
    GuiSetStyle(DEFAULT,TEXT_SIZE, sizeStyle);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL, alignmentStyle);
    EndScissorMode();

    GuiLabel(scaleDynamic((Rectangle){ 368, 120, 232, 24 }), "Info:");

    if (state->isReturning)
        state->returnBookPressed = GuiButton(scaleDynamic((Rectangle){ 480, 424, 120, 24 }), "Return"); 
    else
        GuiLabel(scaleDynamic((Rectangle){ 480, 424, 120, 24 }), "Return");

    BeginScissorMode(scrollArea.x,scrollArea.y,scrollArea.width,scrollArea.height);

    state->refreshInfo = false;
    for (auto i  = 0; i< state->logs.size(); i++){

        const float offset =state->ScrollPanel005ScrollOffset.y + i* 50;
        GuiPanel(scaleDynamic((Rectangle){ 80, (float)(128 +  offset), 256, 48 }), NULL);
        char borrowID[30] = "BorrowID: ";
        strcat(borrowID, std::to_string(state->logs[i].borrowId).c_str());
        GuiLabel(scaleDynamic((Rectangle){ 88, (float)(136 +  offset), 120, 24 }), borrowID);

        if(GuiButton(scaleDynamic((Rectangle){ 308, (float)(136 +  offset) , 24, 24 }), "-")) {
            state->isReturning = !state->logs[i].hasReturned;
            state->selectedLog = state->logs[i].borrowId;
            state->refreshInfo = true;
        }
    }
    EndScissorMode();
}

#endif // GUI_RETURNBOOK_IMPLEMENTATION
