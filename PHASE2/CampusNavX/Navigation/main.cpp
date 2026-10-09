#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include "Data Structures/Graph.h"
#include "Data Structures/IndoorGraph.h"
#include "File Handling/FileManager.h"
#include "algorithms/BFS.h"
#include "algorithms/DFS.h"
#include "algorithms/Dijkstra.h"
#include "navigation/Navigation.h"
#include "navigation/IndoorNavigation.h"
#include "navigation/EmergencyNavigation.h"

static std::string trim(const std::string& value) {
    std::size_t first=value.find_first_not_of(" \t\r\n");
    if(first==std::string::npos) return "";
    std::size_t last=value.find_last_not_of(" \t\r\n");
    return value.substr(first,last-first+1);
}
static std::string normalize(const std::string& value) {
    std::string out=trim(value);
    std::transform(out.begin(),out.end(),out.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    return out;
}
static std::string readLine(const std::string& prompt) {
    std::cout<<prompt;
    std::string value;
    std::getline(std::cin,value);
    return trim(value);
}
static int readChoice() {
    std::string line=readLine("Choose: ");
    std::stringstream ss(line); int value=-1;
    if(!(ss>>value)) return -1;
    return value;
}
static int findOutdoorId(const Graph& g,const std::string& name) {
    std::string target=normalize(name);
    for(int i=1;i<=g.getSize();++i) {
        const Location* l=g.getLocation(i);
        if(l && normalize(l->name)==target) return l->id;
    }
    return -1;
}
static int findIndoorId(const IndoorGraph& g,const std::string& name,bool& ambiguous) {
    std::string target=normalize(name); int match=-1; ambiguous=false;
    for(int i=1;i<=g.getSize();++i) {
        const IndoorLocation* l=g.getLocation(i); if(!l) continue;
        std::ostringstream full; full<<l->roomName<<" ["<<l->buildingName<<", floor "<<l->floor<<"]";
        std::ostringstream buildingRoom; buildingRoom<<l->buildingName<<" "<<l->roomName;
        if(normalize(l->roomName)==target || normalize(full.str())==target || normalize(buildingRoom.str())==target) {
            if(match!=-1 && match!=l->id) { ambiguous=true; return -1; }
            match=l->id;
        }
    }
    return match;
}
static void showOutdoor(const Graph& g) {
    std::cout<<"\nOutdoor locations (enter names, not IDs):\n";
    for(int i=1;i<=g.getSize();++i) { const Location* l=g.getLocation(i); if(l) std::cout<<" - "<<l->name<<" ("<<l->type<<")\n"; }
}
static void showIndoor(const IndoorGraph& g) {
    std::cout<<"\nIndoor locations (format: Room [Building, floor N]):\n";
    for(int i=1;i<=g.getSize();++i) { const IndoorLocation* l=g.getLocation(i); if(l) std::cout<<" - "<<l->roomName<<" ["<<l->buildingName<<", floor "<<l->floor<<"]\n"; }
}
static void rememberRoute(const std::string& label,const std::string& source,const std::string& destination,const RouteResult& r) {
    if(!r.found) return;
    std::ofstream out("data/route_history.txt",std::ios::app);
    if(out) out<<label<<" | "<<source<<" -> "<<destination<<" | "<<r.distance<<" m | "<<r.walkingTime<<" min\n";
}
static void showHistory() {
    std::ifstream in("data/route_history.txt");
    if(!in) { std::cout<<"No saved route history yet.\n"; return; }
    std::string line; std::cout<<"\nSaved route history:\n";
    while(std::getline(in,line)) std::cout<<" - "<<line<<"\n";
}
static void printOutdoorPath(const Graph& g,const std::vector<int>& path,const char* label) {
    if(path.empty()) { std::cout<<label<<": no accessible path found.\n"; return; }
    double distance=0,time=0;
    std::cout<<label<<": ";
    for(std::size_t i=0;i<path.size();++i) {
        if(i) std::cout<<" -> ";
        const Location* l=g.getLocation(path[i]); std::cout<<(l?l->name:"Unknown");
        if(i>0) {
            const std::vector<Edge>& edges=g.getNeighbours(path[i-1]);
            for(std::size_t j=0;j<edges.size();++j) if(edges[j].destination==path[i]) {distance+=edges[j].distance;time+=edges[j].walkingTime;break;}
        }
    }
    std::cout<<"\nDistance: "<<distance<<" m | Estimated time: "<<time<<" min\n";
}
static bool getOutdoorPair(const Graph& g,int& s,int& d,std::string& sourceName,std::string& destinationName) {
    sourceName=readLine("Enter source location name: ");
    destinationName=readLine("Enter destination location name: ");
    s=findOutdoorId(g,sourceName); d=findOutdoorId(g,destinationName);
    if(s<0) std::cout<<"Source location not found: "<<sourceName<<". Use option 1 to see names.\n";
    if(d<0) std::cout<<"Destination location not found: "<<destinationName<<". Use option 1 to see names.\n";
    return s>0 && d>0;
}
static bool getIndoorPair(const IndoorGraph& g,int& s,int& d,std::string& sourceName,std::string& destinationName) {
    showIndoor(g);
    sourceName=readLine("Enter indoor source name (room or full displayed name): ");
    destinationName=readLine("Enter indoor destination name (room or full displayed name): ");
    bool ambS=false,ambD=false;
    s=findIndoorId(g,sourceName,ambS); d=findIndoorId(g,destinationName,ambD);
    if(ambS) std::cout<<"Source room name is ambiguous. Enter its full displayed name including building and floor.\n";
    else if(s<0) std::cout<<"Indoor source not found. Use the displayed room names.\n";
    if(ambD) std::cout<<"Destination room name is ambiguous. Enter its full displayed name including building and floor.\n";
    else if(d<0) std::cout<<"Indoor destination not found. Use the displayed room names.\n";
    return s>0 && d>0;
}
int main() {
    Graph campus;
    IndoorGraph indoor;
    std::vector<IndoorLocation> indoorLocations;
    if(!FileManager::loadLocations(campus,"data/locations.csv")) { std::cout<<"Could not load data/locations.csv. Run the program from the project folder.\n"; return 1; }
    if(!FileManager::loadPathways(campus,"data/pathways.csv")) { std::cout<<"Could not load data/pathways.csv.\n"; return 1; }
    if(!FileManager::loadIndoorLocations(indoorLocations,"data/indoor_locations.csv")) { std::cout<<"Could not load indoor locations.\n"; return 1; }
    for(std::size_t i=1;i<indoorLocations.size();++i) if(indoorLocations[i].id>0) indoor.addLocation(indoorLocations[i]);
    if(!FileManager::loadIndoorPathways(indoor,"data/indoor_pathways.csv")) { std::cout<<"Could not load data/indoor_pathways.csv.\n"; return 1; }

    int choice=-1;
    do {
        std::cout<<"\n========== CampusNavX =========="
                 <<"\nAll routes use accessibility filtering: blocked paths and stairs-only links are avoided."
                 <<"\n1. List outdoor locations"
                 <<"\n2. BFS accessible route (fewest edges)"
                 <<"\n3. DFS accessible exploration route"
                 <<"\n4. Dijkstra shortest-distance accessible route"
                 <<"\n5. Compare BFS, DFS and Dijkstra"
                 <<"\n6. Indoor accessible route"
                 <<"\n7. Emergency: nearest accessible exit"
                 <<"\n8. Block outdoor pathway"
                 <<"\n9. Unblock outdoor pathway"
                 <<"\n10. Block indoor pathway"
                 <<"\n11. Unblock indoor pathway"
                 <<"\n12. List indoor locations"
                 <<"\n13. Show route history"
                 <<"\n0. Exit\n";
        choice=readChoice();
        if(choice==1) showOutdoor(campus);
        else if(choice>=2 && choice<=5) {
            int s=-1,d=-1; std::string sn,dn;
            if(!getOutdoorPair(campus,s,d,sn,dn)) continue;
            if(choice==2) printOutdoorPath(campus,BFS::findPath(campus,s,d,true),"BFS route");
            else if(choice==3) printOutdoorPath(campus,DFS::findPath(campus,s,d,true),"DFS route");
            else if(choice==4) { RouteResult r=Dijkstra::shortestPath(campus,s,d,true); Navigation::printOutdoorRoute(campus,r); rememberRoute("Outdoor Dijkstra",sn,dn,r); }
            else {
                std::cout<<"\nCandidate 1, BFS (fewest edges):\n"; printOutdoorPath(campus,BFS::findPath(campus,s,d,true),"BFS");
                std::cout<<"\nCandidate 2, DFS (exploration path):\n"; printOutdoorPath(campus,DFS::findPath(campus,s,d,true),"DFS");
                std::cout<<"\nDijkstra independently computes the shortest-distance accessible route:\n";
                RouteResult r=Dijkstra::shortestPath(campus,s,d,true); Navigation::printOutdoorRoute(campus,r); rememberRoute("Outdoor Dijkstra",sn,dn,r);
            }
        } else if(choice==6) {
            int s=-1,d=-1; std::string sn,dn;
            if(!getIndoorPair(indoor,s,d,sn,dn)) continue;
            RouteResult r=Dijkstra::shortestIndoorPath(indoor,s,d,true);
            IndoorNavigation::printIndoorRoute(indoor,r); rememberRoute("Indoor Dijkstra",sn,dn,r);
        } else if(choice==7) {
            std::string sn=readLine("Enter your current outdoor location name: "); int s=findOutdoorId(campus,sn);
            if(s<0) {std::cout<<"Location not found. Use option 1 to see names.\n";continue;}
            RouteResult r=EmergencyNavigation::nearestAccessibleExit(campus,s);
            if(r.found && !r.path.empty()) {const Location* ex=campus.getLocation(r.path.back());std::cout<<"Nearest accessible exit: "<<(ex?ex->name:"Unknown")<<"\n";Navigation::printOutdoorRoute(campus,r);rememberRoute("Emergency",sn,ex?ex->name:"exit",r);}
            else std::cout<<"No reachable accessible exit was found.\n";
        } else if(choice==8 || choice==9) {
            int s=-1,d=-1; std::string sn,dn;
            if(!getOutdoorPair(campus,s,d,sn,dn)) continue;
            bool ok=choice==8?campus.blockPath(s,d):campus.unblockPath(s,d);
            if(ok) {
                bool saved=FileManager::savePathways(campus,"data/pathways.csv");
                std::cout<<"Outdoor pathway "<<(choice==8?"blocked":"unblocked")<<". "<<(saved?"Change saved to CSV.\n":"Warning: change is only in memory because CSV save failed.\n");
            } else std::cout<<"No direct pathway exists between those locations.\n";
        } else if(choice==10 || choice==11) {
            int s=-1,d=-1; std::string sn,dn;
            if(!getIndoorPair(indoor,s,d,sn,dn)) continue;
            bool ok=choice==10?indoor.blockPath(s,d):indoor.unblockPath(s,d);
            if(ok) {
                bool saved=FileManager::saveIndoorPathways(indoor,"data/indoor_pathways.csv");
                std::cout<<"Indoor pathway "<<(choice==10?"blocked":"unblocked")<<". "<<(saved?"Change saved to CSV.\n":"Warning: change is only in memory because CSV save failed.\n");
            } else std::cout<<"No direct indoor pathway exists between those locations.\n";
        } else if(choice==12) showIndoor(indoor);
        else if(choice==13) showHistory();
        else if(choice!=0) std::cout<<"Invalid option. Enter a menu number from 0 to 13.\n";
    } while(choice!=0);
    std::cout<<"CampusNavX closed.\n";
    return 0;
}
