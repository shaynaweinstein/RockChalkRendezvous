#ifndef RCR_MENU_FILE_CLIENT
#define RCR_MENU_FILE_CLIENT
#include <ncurses.h>
#include "../shared/timeanddate.hpp"
#include "../shared/calendar.hpp"
#include "../shared/group.hpp"
#include "login.hpp"
#include "calendar_editor.cpp"
#include "client.hpp"
#include <string.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include "httplib.h"
#include <iostream>

enum MenuOption {
    InMenu,     // This includes being in the menu, viewing options to change to the state below
    ViewingCalendars,    // This includes viewing/editing a user's calendar
    ViewingGroups,      // This includes viewing group calendars and selecting a group calendar to view
    EditingGroups,      // This includes editing a group - join group, create group, leave group
};

static MenuOption MenuState;
static WINDOW* menu_window;
static const char *menu_choices[] = { "View Calendar", "View Group Calendars", "Edit Groups" };
void draw_edit_groups_window();
void draw_groups_create_window();
void draw_groups_join_window();
void run_edit_group_selection(int selection);
void get_current_user_groups();
static Group current_group;
static std::vector<Group> current_groups;
static std::vector<std::tuple<std::string, Calendar>> current_group_calendars;

void decode_group_calendars(std::string request_output);
Group draw_groups_list();
void draw_groups_leave_window();
MenuOption draw_menu_choice_window() {
    const int num_choices = sizeof(menu_choices) / sizeof(menu_choices[0]);
    int current_selection = 0;
    int ch;
    keypad(menu_window, TRUE); // Enable keyboard input for the menu_windowdow
    noecho();          // Don't echo the pressed keys to the menu_windowdow
    mvwprintw(menu_window, 0, 0, "Choose one:");
    wrefresh(menu_window);
    while (true) {
        for (int i = 0; i < num_choices; ++i) {
            if (i == current_selection) {
                wattron(menu_window, A_REVERSE);  // Highlight the selected choice
            }
            mvwprintw(menu_window, i + 1, 1, "%s", menu_choices[i]);
            if (i == current_selection) {
                wattroff(menu_window, A_REVERSE);
            }
        }
        wrefresh(menu_window);

        ch = wgetch(menu_window); // Get user input

        switch (ch) {
            case KEY_UP:
            case 'k': // Move selection up
                if (--current_selection < 0) {
                    current_selection = num_choices - 1;
                }
                break;
            case KEY_DOWN:
            case 'j': // Move selection down
                if (++current_selection >= num_choices) {
                    current_selection = 0;
                }
                break;
            case '\n': // User made a selection
                return static_cast<MenuOption>(current_selection + 1);
        }
    }
}

void draw_group_interactions_window() {
    current_groups.clear();
    get_current_user_groups();
    switch (MenuState) {
        case InMenu:
            draw_menu_choice_window();
            break;
        case ViewingCalendars: // this is invalid, it shouldn't run when the state is ViewingCalendars
            break;
            
        case ViewingGroups: // this will be where a user selects a group's calendar to view
            current_group = draw_groups_list();
            // decode_group_calendars(send_get_groups_request());
            transfer_to_group_calendar_view();
            // set active_group to request for get_groups[0]
            // set set group_calendars to get_groups fn.

            break;
        case EditingGroups:
            draw_edit_groups_window();
            // this is where I'll display editing menu choices to edit a users groups
            break;
    }
}

void decode_groups(std::string request_output) {
    wclear(menu_window);
    std::istringstream iss(request_output);
    std::string groups_to_decode;
    std::string test;
    int num_groups;
    iss >> num_groups;
    mvwprintw(menu_window, 3, 2, "READ LINE: %d" , num_groups);


    wrefresh(menu_window);
    napms(2000); // Wait for 2 seconds (for visibility in demonstration)
    wgetch(menu_window);
    // Fix: Call the decode_vector function with the correct arguments
}

void get_current_user_group_calendars(){
    std::string request_output = send_get_group_calendars_request(current_group.id);
    mvwprintw(menu_window, 5, 2, request_output.c_str());
    std::istringstream iss(request_output);
    std::string groups_to_decode;
    // decode_vector<std::tuple<std::string, Calendar>>(iss, current_group_calendars, &Group::decode_static);
    wrefresh(menu_window);
}

void get_current_user_groups() {
    std::string request_output = send_get_groups_request();
    std::istringstream iss(request_output);
    std::string groups_to_decode;
    decode_vector<Group>(iss, current_groups, Group::decode_static);
    wrefresh(menu_window);
}
void draw_edit_groups_window() {
    static const char *editing_menu_choices[] = { "Create Group", "Join Group", "Leave Group", "Rename Group", "View Groups" };
    const int num_choices = sizeof(editing_menu_choices) / sizeof(editing_menu_choices[0]);
    int current_selection = 0;
    int ch;
    keypad(menu_window, TRUE); // Enable keyboard input for the menu_windowdow
    noecho();          // Don't echo the pressed keys to the menu_windowdow
    mvwprintw(menu_window, 0, 0, "Choose one:");
    wrefresh(menu_window);
    // int character;
    bool not_chosen = true;
    while (not_chosen ) {
        // character = wgetch(interact_window);

        for (int i = 0; i < num_choices; ++i) {
            if (i == current_selection) {
                wattron(menu_window, A_REVERSE);  // Highlight the selected choice
            }
            mvwprintw(menu_window, i + 1, 1, "%s", editing_menu_choices[i]);
            if (i == current_selection) {
                wattroff(menu_window, A_REVERSE);
            }
        }
        wrefresh(menu_window);

        ch = wgetch(menu_window); // Get user input

        switch (ch) {
            case KEY_UP:
            case 'k': // Move selection up
                if (--current_selection < 0) {
                    current_selection = num_choices - 1;
                }
                break;
            case KEY_DOWN:
            case 'j': // Move selection down
                if (++current_selection >= num_choices) {
                    current_selection = 0;
                }
                break;
            case '\n': // User made a selection
                not_chosen = false;
                break;
        }
    }
    wclear(menu_window);
    run_edit_group_selection(current_selection);
    
    wrefresh(menu_window);

    napms(3000);
    wclear(menu_window);

}

void run_edit_group_selection(int current_selection){

    switch (current_selection){
        case 0:
            draw_groups_create_window();
            break;
        case 1:
            draw_groups_join_window();
            break;
        case 2:
            draw_groups_leave_window();
            break;
        case 3:
            mvwprintw(menu_window, 1, 1, "RENAME GROUP");
            break;
        case 4:

            mvwprintw(menu_window, 1, 1, "VIEW GROUP");
            break;
    }
}
Group draw_groups_list() {
    std::vector<usize> group_ids;
    std::vector<std::string> group_names;

    int num_groups = current_groups.size();
    int current_selection = 0;
    int ch;
    keypad(menu_window, TRUE); // Enable keyboard input for the menu_windowdow
    noecho();          // Don't echo the pressed keys to the menu_windowdow
    wrefresh(menu_window);

    bool not_chosen = true;
    while (not_chosen ) {
        // character = wgetch(interact_window);

        for (int i = 0; i < num_groups; ++i) {
            if (i == current_selection) {
                wattron(menu_window, A_REVERSE);  // Highlight the selected choice
            }
            // Safeguard against potential out-of-bounds or corrupted strings - cgpt
            // std::string safe_display_name = (i < group_names.size()) ? group_names[i] : "Invalid Group";
            mvwprintw(menu_window, i + 1, 1, "%s", current_groups[i].name.c_str());
            if (i == current_selection) {
                wattroff(menu_window, A_REVERSE);
            }
        }
        wrefresh(menu_window);

        ch = wgetch(menu_window); // Get user input

        switch (ch) {
            case KEY_UP:
            case 'k': // Move selection up
                if (--current_selection < 0) {
                    current_selection = num_groups - 1;
                }
                break;
            case KEY_DOWN:
            case 'j': // Move selection down
                if (++current_selection >= num_groups) {
                    current_selection = 0;
                }
                break;
            case '\n': // User made a selection
                not_chosen = false;
                break;
        }
    }
    wclear(menu_window);
    return current_groups[current_selection];
}
// TODO - create catchall function that iterates through groups and can return whatever is needed dependent on selection
// CreateGroup(user, group_name)
void draw_groups_create_window() {
    char group_name[50];
    echo();
    mvwprintw(menu_window, 1, 1, "Enter group name:");
    mvwgetnstr(menu_window, 2, 5, group_name, 50);
    std::string group_name_string = group_name;
    noecho();
    mvwprintw(menu_window, 3, 1, "User: %s", username);
    mvwprintw(menu_window, 4, 1, "Group Name: %s", group_name);
    Status created_group =  send_create_group_request(group_name_string);
    if (created_group == Failure) {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "FAILED TO CREATE GROUP");
    } else {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "SUCCESSFULLY CREATED GROUP");
        
    }
    wrefresh(menu_window);
    napms(2000);
    MenuState = MenuOption::InMenu;
}

// JoinGroup(user, group_id)
void draw_groups_join_window() {
    Group selected_group = draw_groups_list();

    usize selected_group_id = selected_group.id;
    std::string selected_group_name = selected_group.name;
    wclear(menu_window);
    mvwprintw(menu_window, 4, 1, "Group Name: %s", selected_group_name.c_str());
    mvwprintw(menu_window, 5, 1, "Group ID: %lu", selected_group_id);

    send_join_group_request(selected_group_id);
    Status joined_group =  send_join_group_request(selected_group_id);
    if (joined_group == Failure) {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "FAILED TO JOIN GROUP");
    } else {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "SUCCESSFULLY JOINED GROUP");
        
    }
    wrefresh(menu_window);
    napms(2000);
    MenuState = MenuOption::InMenu;
}
// LeaveGroup(user, group_id)
void draw_groups_leave_window() {
    Group selected_group = draw_groups_list();
    usize selected_group_id = selected_group.id;
    std::string selected_group_name = selected_group.name;
    wclear(menu_window);
    mvwprintw(menu_window, 4, 1, "Group Name: %s", selected_group_name.c_str());
    mvwprintw(menu_window, 5, 1, "Group ID: %lu", selected_group_id);

    send_leave_group_request(selected_group_id);
    Status left_group =  send_leave_group_request(selected_group_id);
    if (left_group == Failure) {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "FAILED TO leave GROUP");
    } else {
        wclear(menu_window);
        mvwprintw(menu_window, 1, 1, "SUCCESSFULLY LEFT GROUP");
        
    }
    wrefresh(menu_window);
    napms(2000);
    MenuState = MenuOption::InMenu;
}
// GetGroups(user) -> get encoded groups
void draw_groups_get_window() {
    Group selected_group = draw_groups_list();
    usize selected_group_id = selected_group.id;
    std::string selected_group_name = selected_group.name;
    wclear(menu_window);
    mvwprintw(menu_window, 4, 1, "Group Name: %s", selected_group_name.c_str());
    mvwprintw(menu_window, 5, 1, "Group ID: %lu", selected_group_id);

    // THIS WHERE USERS WILL CHOOSE WHICH GROUP CALENDAR TO VIEW
}



void update_menu_screen() {
    switch (MenuState) {
        case InMenu:
            MenuState = draw_menu_choice_window();
            break;
        case ViewingCalendars:
            wclear(menu_window);
            transfer_to_calendar_editor();
            MenuState = MenuOption::InMenu;
            break;
        case ViewingGroups: 
        case EditingGroups:
            wclear(menu_window);
            draw_group_interactions_window();
            MenuState = MenuOption::InMenu;
            break;
    }
}

#endif