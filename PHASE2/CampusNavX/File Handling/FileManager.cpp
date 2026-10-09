#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
static std::vector<std::string> split(const std::string& line){std::vector<std::string> v;std::stringstream ss(line);std::string x;while(std::getline(ss,x,','))v.push_back(x);return v;}
static int number(const std::vector<std::string>& v,std::size_t i,int fallback=0){try{return i<v.size()?std::stoi(v[i]):fallback;}catch(...){return fallback;}}
static double decimal(const std::vector<std::string>& v,std::size_t i,double fallback=0){try{return i<v.size()?std::stod(v[i]):fallback;}catch(...){return fallback;}}
bool FileManager::loadLocations(Graph& g,const std::string& file){std::ifstream in(file);if(!in){std::cerr<<"Cannot open "<<file<<"\n";return false;}std::string line;std::getline(in,line);int count=0;while(std::getline(in,line)){if(line.empty())continue;auto v=split(line);if(v.size()<2)continue;int id=number(v,0);if(id<1)continue;g.addLocation(Location(id,v[1],v.size()>2?v[2]:"Unknown",number(v,3)));++count;}return count>0;}
bool FileManager::loadPathways(Graph& g,const std::string& file){std::ifstream in(file);if(!in){std::cerr<<"Cannot open "<<file<<"\n";return false;}std::string line;std::getline(in,line);int count=0;while(std::getline(in,line)){if(line.empty())continue;auto v=split(line);if(v.size()<11)continue;g.addEdge(number(v,0),number(v,2),decimal(v,4),decimal(v,5),number(v,10),number(v,7),number(v,8),number(v,9));++count;}return count>0;}
bool FileManager::loadIndoorLocations(std::vector<IndoorLocation>& out,const std::string& file){std::ifstream in(file);if(!in){std::cerr<<"Cannot open "<<file<<"\n";return false;}std::string line;std::getline(in,line);int maxId=0;std::vector<IndoorLocation> temp;while(std::getline(in,line)){if(line.empty())continue;auto v=split(line);if(v.size()<6)continue;IndoorLocation l(number(v,0),number(v,1),v[2],number(v,3),v[4],v[5]);if(l.id>0){temp.push_back(l);if(l.id>maxId)maxId=l.id;}}out.assign(static_cast<std::size_t>(maxId+1),IndoorLocation());for(const auto& l:temp)out[l.id]=l;return !temp.empty();}
bool FileManager::loadIndoorPathways(IndoorGraph& g,const std::string& file){std::ifstream in(file);if(!in){std::cerr<<"Cannot open "<<file<<"\n";return false;}std::string line;std::getline(in,line);int count=0;while(std::getline(in,line)){if(line.empty())continue;auto v=split(line);if(v.size()<8)continue;g.addEdge(number(v,0),number(v,1),decimal(v,2),decimal(v,3),number(v,4),number(v,5),number(v,6),number(v,7));++count;}return count>0;}


static std::string joinCsv(const std::vector<std::string>& values) {
    std::string out;
    for (std::size_t i=0;i<values.size();++i) { if(i) out += ','; out += values[i]; }
    return out;
}
static bool saveBlockedColumn(const std::string& file, const Graph* outdoor, const IndoorGraph* indoor) {
    std::ifstream in(file.c_str());
    if(!in) return false;
    std::vector<std::string> lines;
    std::string line;
    if(!std::getline(in,line)) return false;
    lines.push_back(line);
    while(std::getline(in,line)) {
        if(line.empty()) { lines.push_back(line); continue; }
        std::vector<std::string> v=split(line);
        if(outdoor && v.size()>=11) {
            int a=number(v,0), b=number(v,2);
            bool found=false, blocked=false;
            const std::vector<Edge>& edges=outdoor->getNeighbours(a);
            for(std::size_t i=0;i<edges.size();++i) if(edges[i].destination==b){found=true;blocked=edges[i].blocked!=0;break;}
            if(found) v[10]=blocked?"1":"0";
        } else if(indoor && v.size()>=8) {
            int a=number(v,0), b=number(v,1);
            bool found=false, blocked=false;
            const std::vector<IndoorEdge>& edges=indoor->getNeighbours(a);
            for(std::size_t i=0;i<edges.size();++i) if(edges[i].destination==b){found=true;blocked=edges[i].blocked!=0;break;}
            if(found) v[7]=blocked?"1":"0";
        }
        lines.push_back(joinCsv(v));
    }
    in.close();
    std::ofstream out(file.c_str(),std::ios::trunc);
    if(!out) return false;
    for(std::size_t i=0;i<lines.size();++i) out<<lines[i]<<'\n';
    return static_cast<bool>(out);
}
bool FileManager::savePathways(const Graph& g,const std::string& file){return saveBlockedColumn(file,&g,0);}
bool FileManager::saveIndoorPathways(const IndoorGraph& g,const std::string& file){return saveBlockedColumn(file,0,&g);}
