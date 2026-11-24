
#ifndef ROUTE_HISTORY_H
#define ROUTE_HISTORY_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class RouteHistory {
private:
    vector<string> history;
    string filename;

public:
    RouteHistory(string file = "data/route_history.txt") {
        filename = file;
        loadHistory();
    }

    // Load previous history from the file
    void loadHistory() {
        history.clear();

        ifstream file(filename.c_str());

        if (!file.is_open()) {
            return;
        }

        string line;

        while (getline(file, line)) {
            if (!line.empty()) {
                history.push_back(line);
            }
        }

        file.close();
    }

    // Add a route and save it
    void addRoute(string start, string destination,
                  string routeDetails) {
        string entry = "From: " + start +
                       " | To: " + destination +
                       " | Route: " + routeDetails;

        history.push_back(entry);
        saveHistory();
    }

    // Save all history entries
    bool saveHistory() {
        ofstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Could not save route history.\n";
            return false;
        }

        for (int i = 0; i < (int)history.size(); i++) {
            file << history[i] << "\n";
        }

        file.close();
        return true;
    }

    // Display history, newest entry first
    void showHistory() {
        if (history.empty()) {
            cout << "\nNo route history available.\n";
            return;
        }

        cout << "\n========== ROUTE HISTORY ==========\n";

        for (int i = (int)history.size() - 1; i >= 0; i--) {
            cout << history.size() - i << ". "
                 << history[i] << "\n";
        }
    }

    // Clear all saved history
    void clearHistory() {
        history.clear();
        saveHistory();
        cout << "Route history cleared.\n";
    }

    int getCount() {
        return (int)history.size();
    }
};

#endif
