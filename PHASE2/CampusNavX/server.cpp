#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <cmath>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close
#endif
#include "Data Structures/Graph.h"
#include "Data Structures/IndoorGraph.h"
#include "File Handling/FileManager.h"
#include "algorithms/BFS.h"
#include "algorithms/DFS.h"
#include "algorithms/Dijkstra.h"
#include "navigation/EmergencyNavigation.h"

static std::string trim(const std::string& s){std::size_t a=s.find_first_not_of(" \t\r\n");if(a==std::string::npos)return "";return s.substr(a,s.find_last_not_of(" \t\r\n")-a+1);}
static std::string lower(std::string s){std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});return s;}
static std::string jsonEscape(const std::string& s){std::ostringstream o;for(unsigned char c:s){switch(c){case '"':o<<"\\\"";break;case '\\':o<<"\\\\";break;case '\n':o<<"\\n";break;case '\r':o<<"\\r";break;case '\t':o<<"\\t";break;default:if(c>=32)o<<c;}}return o.str();}
static std::string indoorLabel(const IndoorLocation* l){if(!l)return "Unknown";std::ostringstream o;o<<l->roomName<<" ["<<l->buildingName<<", floor "<<l->floor<<"]";return o.str();}
static int outdoorId(const Graph& g,const std::string& name){for(int i=1;i<=g.getSize();++i){const Location* l=g.getLocation(i);if(l&&lower(trim(l->name))==lower(trim(name)))return l->id;}return -1;}
static int indoorId(const IndoorGraph& g,const std::string& name){int found=-1;for(int i=1;i<=g.getSize();++i){const IndoorLocation* l=g.getLocation(i);if(!l)continue;if(lower(trim(l->roomName))==lower(trim(name))||lower(trim(indoorLabel(l)))==lower(trim(name))||lower(trim(l->buildingName+" "+l->roomName))==lower(trim(name))){if(found!=-1)return -1;found=l->id;}}return found;}
static std::string indoorJson(const IndoorLocation* l){if(!l)return "{}";std::ostringstream o;o<<"{\"id\":"<<l->id<<",\"name\":\""<<jsonEscape(indoorLabel(l))<<"\",\"roomName\":\""<<jsonEscape(l->roomName)<<"\",\"buildingName\":\""<<jsonEscape(l->buildingName)<<"\",\"floor\":"<<l->floor<<",\"type\":\""<<jsonEscape(l->type)<<"\"}";return o.str();}
static std::string locationsJson(const Graph& g,const IndoorGraph& ig){std::ostringstream o;o<<"{\"outdoor\":[";bool first=true;for(int i=1;i<=g.getSize();++i){const Location* l=g.getLocation(i);if(!l)continue;if(!first)o<<',';first=false;o<<"{\"id\":"<<l->id<<",\"name\":\""<<jsonEscape(l->name)<<"\",\"type\":\""<<jsonEscape(l->type)<<"\"}";}o<<"],\"indoor\":[";first=true;for(int i=1;i<=ig.getSize();++i){const IndoorLocation* l=ig.getLocation(i);if(!l)continue;if(!first)o<<',';first=false;o<<indoorJson(l);}o<<"]}";return o.str();}
static std::string decode(const std::string& s){std::string o;for(std::size_t i=0;i<s.size();++i){if(s[i]=='+')o+=' ';else if(s[i]=='%'&&i+2<s.size()){char h[3]={s[i+1],s[i+2],0};o+=static_cast<char>(std::strtol(h,nullptr,16));i+=2;}else o+=s[i];}return o;}
static std::map<std::string,std::string> parsePairs(const std::string& s,char sep){std::map<std::string,std::string> m;std::istringstream in(s);std::string item;while(std::getline(in,item,sep)){std::size_t p=item.find('=');if(p!=std::string::npos)m[decode(item.substr(0,p))]=decode(item.substr(p+1));}return m;}
static std::string jsonField(const std::string& body,const std::string& key){std::string token="\""+key+"\"";std::size_t p=body.find(token);if(p==std::string::npos)return "";p=body.find(':',p+token.size());if(p==std::string::npos)return "";++p;while(p<body.size()&&std::isspace(static_cast<unsigned char>(body[p])))++p;if(p>=body.size()||body[p]!='"')return "";++p;std::string out;while(p<body.size()){char c=body[p++];if(c=='"')break;if(c=='\\'&&p<body.size()){char n=body[p++];if(n=='n')out+='\n';else out+=n;}else out+=c;}return out;}
static void saveHistory(const std::string&,const std::string&,const std::string&,double,double);
static void savePathHistory(const std::string& algorithm,const std::string& source,const std::string& destination,const std::vector<int>& path,const Graph& g){if(path.empty())return;double dist=0,mins=0;for(std::size_t i=1;i<path.size();++i){for(const auto& e:g.getNeighbours(path[i-1]))if(e.destination==path[i]){dist+=e.distance;mins+=e.walkingTime;break;}}saveHistory(algorithm,source,destination,dist,mins);}
static std::string routeResultJson(const std::string& algorithm,const std::vector<int>& path,const Graph& g){if(path.empty())return "{\"found\":false,\"algorithm\":\""+jsonEscape(algorithm)+"\",\"message\":\"No accessible route exists between these locations.\"}";double distance=0,time=0;std::ostringstream names;for(std::size_t i=0;i<path.size();++i){const Location* l=g.getLocation(path[i]);if(i)names<<',';names<<'"'<<jsonEscape(l?l->name:"Unknown")<<'"';if(i){const auto& es=g.getNeighbours(path[i-1]);for(const auto& e:es)if(e.destination==path[i]){distance+=e.distance;time+=e.walkingTime;break;}}}std::ostringstream o;o<<"{\"found\":true,\"algorithm\":\""<<jsonEscape(algorithm)<<"\",\"distance\":"<<distance<<",\"walkingTime\":"<<time<<",\"path\":["<<names.str()<<"]}";return o.str();}
static std::string routeResultJson(const std::string& algorithm,const RouteResult& r,const Graph& g){if(!r.found||r.path.empty())return "{\"found\":false,\"algorithm\":\""+jsonEscape(algorithm)+"\",\"message\":\"No accessible route exists between these locations.\"}";std::ostringstream names;for(std::size_t i=0;i<r.path.size();++i){const Location* l=g.getLocation(r.path[i]);if(i)names<<',';names<<'\"'<<jsonEscape(l?l->name:"Unknown")<<'\"';}std::ostringstream o;o<<"{\"found\":true,\"algorithm\":\""<<jsonEscape(algorithm)<<"\",\"distance\":"<<r.distance<<",\"walkingTime\":"<<r.walkingTime<<",\"path\":["<<names.str()<<"]}";return o.str();}
static std::string routeResultJson(const std::string& algorithm,const RouteResult& r,const IndoorGraph& g){if(!r.found||r.path.empty())return "{\"found\":false,\"algorithm\":\""+jsonEscape(algorithm)+"\",\"message\":\"No accessible route exists. Check the locations and blocked pathways.\"}";std::ostringstream names;for(std::size_t i=0;i<r.path.size();++i){if(i)names<<',';names<<'"'<<jsonEscape(indoorLabel(g.getLocation(r.path[i])))<<'"';}std::ostringstream o;o<<"{\"found\":true,\"algorithm\":\""<<jsonEscape(algorithm)<<"\",\"distance\":"<<r.distance<<",\"walkingTime\":"<<r.walkingTime<<",\"path\":["<<names.str()<<"]}";return o.str();}
static void saveHistory(const std::string& algorithm,const std::string& source,const std::string& destination,double dist,double mins){std::ofstream f("data/route_history.txt",std::ios::app);if(f)f<<algorithm<<" | "<<source<<" -> "<<destination<<" | "<<dist<<" m | "<<mins<<" min\n";}
static std::string handleApi(const std::string& method,const std::string& path,const std::string& body,Graph& g,IndoorGraph& ig){std::size_t q=path.find('?');std::string route=path.substr(0,q);auto query=parsePairs(q==std::string::npos?"":path.substr(q+1),'&');
 if(method=="GET"&&route=="/api/locations")return locationsJson(g,ig);
 if(method=="GET"&&route=="/api/history"){std::ifstream f("data/route_history.txt");std::ostringstream o;o<<"{\"items\":[";std::string line;bool first=true;while(std::getline(f,line)){if(!first)o<<',';first=false;o<<'"'<<jsonEscape(line)<<'"';}o<<"]}";return o.str();}
 if(method=="GET"&&route=="/api/route"){
  std::string mode=query["mode"],algo=query["algorithm"],sn=query["source"],dn=query["destination"];
  if(mode=="emergency"){int s=outdoorId(g,sn);if(s<1)return "{\"found\":false,\"message\":\"Starting location not found. Choose a campus location.\"}";RouteResult r=EmergencyNavigation::nearestAccessibleExit(g,s);std::string out=routeResultJson("Nearest accessible emergency exit",r,g);if(r.found&&!r.path.empty()){const Location* exitLocation=g.getLocation(r.path.back());saveHistory("Emergency",sn,exitLocation?exitLocation->name:"exit",r.distance,r.walkingTime);}return out;}
  if(mode=="indoor"){int s=indoorId(ig,sn),d=indoorId(ig,dn);if(s<1||d<1)return "{\"found\":false,\"message\":\"Indoor location not found or room name is ambiguous. Select the full displayed room name.\"}";RouteResult r=Dijkstra::shortestIndoorPath(ig,s,d,true);std::string out=routeResultJson("Dijkstra",r,ig);if(r.found)saveHistory("Indoor Dijkstra",sn,dn,r.distance,r.walkingTime);return out;}
  int s=outdoorId(g,sn),d=outdoorId(g,dn);if(s<1||d<1)return "{\"found\":false,\"message\":\"Outdoor location not found. Choose a location from the list.\"}";
  if(algo=="compare"){std::string b=routeResultJson("BFS",BFS::findPath(g,s,d,true),g),f=routeResultJson("DFS",DFS::findPath(g,s,d,true),g);RouteResult dr=Dijkstra::shortestPath(g,s,d,true);std::string dj=routeResultJson("Dijkstra",dr,g);std::ostringstream o;o<<"{\"found\":"<<(dr.found?"true":"false")<<",\"comparison\":true,\"routes\":["<<b<<','<<f<<','<<dj<<"]}";if(dr.found)saveHistory("Outdoor Dijkstra",sn,dn,dr.distance,dr.walkingTime);return o.str();}
  std::string out;if(algo=="bfs"){std::vector<int> p=BFS::findPath(g,s,d,true);out=routeResultJson("BFS",p,g);savePathHistory("Outdoor BFS",sn,dn,p,g);}else if(algo=="dfs"){std::vector<int> p=DFS::findPath(g,s,d,true);out=routeResultJson("DFS",p,g);savePathHistory("Outdoor DFS",sn,dn,p,g);}else{RouteResult r=Dijkstra::shortestPath(g,s,d,true);out=routeResultJson("Dijkstra",r,g);if(r.found)saveHistory("Outdoor Dijkstra",sn,dn,r.distance,r.walkingTime);return out;}
  return out;
 }
 if(method=="POST"&&route=="/api/pathway"){
  std::string scope=jsonField(body,"scope"),sn=jsonField(body,"source"),dn=jsonField(body,"destination"),action=jsonField(body,"action");bool ok=false,saved=false;
  if(scope=="indoor"){int s=indoorId(ig,sn),d=indoorId(ig,dn);if(s<1||d<1)return "{\"error\":\"Could not identify both indoor locations. Choose the full room labels.\"}";ok=action=="block"?ig.blockPath(s,d):ig.unblockPath(s,d);if(ok)saved=FileManager::saveIndoorPathways(ig,"data/indoor_pathways.csv");}
  else {int s=outdoorId(g,sn),d=outdoorId(g,dn);if(s<1||d<1)return "{\"error\":\"Could not identify both outdoor locations.\"}";ok=action=="block"?g.blockPath(s,d):g.unblockPath(s,d);if(ok)saved=FileManager::savePathways(g,"data/pathways.csv");}
  if(!ok)return "{\"error\":\"No direct pathway exists between those locations.\"}";if(!saved)return "{\"error\":\"Pathway changed in memory, but saving the CSV failed. Check folder permissions.\"}";return "{\"message\":\"Pathway "+std::string(action=="block"?"blocked":"unblocked")+" and saved successfully.\"}";
 }
 return "{\"error\":\"Unknown API endpoint.\"}";
}
static std::string mimeType(const std::string& p){if(p.size()>=5&&p.substr(p.size()-5)==".html")return "text/html; charset=utf-8";if(p.size()>=4&&p.substr(p.size()-4)==".css")return "text/css; charset=utf-8";if(p.size()>=3&&p.substr(p.size()-3)==".js")return "application/javascript; charset=utf-8";return "text/plain; charset=utf-8";}
static std::string readFile(const std::string& p){std::ifstream f(p.c_str(),std::ios::binary);if(!f)return "";std::ostringstream o;o<<f.rdbuf();return o.str();}
static void sendResponse(socket_t client,int status,const std::string& type,const std::string& body){std::ostringstream h;h<<"HTTP/1.1 "<<status<<(status==200?" OK":" Not Found")<<"\r\nContent-Type: "<<type<<"\r\nContent-Length: "<<body.size()<<"\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Content-Type\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nConnection: close\r\n\r\n";std::string all=h.str()+body;std::size_t sent=0;while(sent<all.size()){int n=send(client,all.data()+sent,static_cast<int>(all.size()-sent),0);if(n<=0)break;sent+=static_cast<std::size_t>(n);}}
int main(){
 Graph campus;IndoorGraph indoor;std::vector<IndoorLocation> indoorLocations;
 if(!FileManager::loadLocations(campus,"data/locations.csv")||!FileManager::loadPathways(campus,"data/pathways.csv")||!FileManager::loadIndoorLocations(indoorLocations,"data/indoor_locations.csv")){std::cerr<<"Could not load project CSV files. Run run_gui.bat from the CampusNavX_Backend folder.\n";return 1;}
 for(std::size_t i=1;i<indoorLocations.size();++i)if(indoorLocations[i].id>0)indoor.addLocation(indoorLocations[i]);
 if(!FileManager::loadIndoorPathways(indoor,"data/indoor_pathways.csv")){std::cerr<<"Could not load indoor pathways.\n";return 1;}
#ifdef _WIN32
 WSADATA wsa;if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){std::cerr<<"Winsock startup failed.\n";return 1;}
#endif
 socket_t server=socket(AF_INET,SOCK_STREAM,0);if(server==INVALID_SOCKET){std::cerr<<"Could not create local server socket.\n";return 1;}int yes=1;
#ifdef _WIN32
 setsockopt(server,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&yes),sizeof(yes));
#else
 setsockopt(server,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
#endif
 sockaddr_in addr;std::memset(&addr,0,sizeof(addr));addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);addr.sin_port=htons(8765);
 if(bind(server,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))==SOCKET_ERROR||listen(server,10)==SOCKET_ERROR){std::cerr<<"Could not start server on 127.0.0.1:8765. Close any other app using this port and retry.\n";CLOSESOCKET(server);return 1;}
 std::cout<<"CampusNavX GUI server is running at http://127.0.0.1:8765\nKeep this window open while using the GUI. Press Ctrl+C to stop.\n";
 while(true){sockaddr_in clientAddr;
#ifdef _WIN32
 int clientLen=sizeof(clientAddr);
#else
 socklen_t clientLen=sizeof(clientAddr);
#endif
 socket_t client=accept(server,reinterpret_cast<sockaddr*>(&clientAddr),&clientLen);if(client==INVALID_SOCKET)continue;std::string req;char buf[4096];int n=0;std::size_t headerEnd=std::string::npos;while((n=recv(client,buf,sizeof(buf),0))>0){req.append(buf,n);headerEnd=req.find("\r\n\r\n");if(headerEnd!=std::string::npos){std::size_t clp=req.find("Content-Length:");std::size_t contentLength=0;if(clp!=std::string::npos&&clp<headerEnd){std::size_t end=req.find("\r\n",clp);contentLength=static_cast<std::size_t>(std::atoi(req.substr(clp+15,end-(clp+15)).c_str()));}if(req.size()>=headerEnd+4+contentLength)break;}}
  std::size_t lineEnd=req.find("\r\n");std::istringstream first(req.substr(0,lineEnd));std::string method,path,version;first>>method>>path>>version;std::string body=headerEnd==std::string::npos?"":req.substr(headerEnd+4);if(method=="OPTIONS"){sendResponse(client,200,"text/plain","ok");CLOSESOCKET(client);continue;}
  if(path.find("/api/")==0){std::string result=handleApi(method,path,body,campus,indoor);sendResponse(client,200,"application/json; charset=utf-8",result);}
  else {if(path=="/")path="/index.html";std::string file=readFile("frontend"+path);if(file.empty())sendResponse(client,404,"text/plain; charset=utf-8","File not found. Run this server from the CampusNavX_Backend project folder.");else sendResponse(client,200,mimeType(path),file);}
  CLOSESOCKET(client);
 }
 CLOSESOCKET(server);
#ifdef _WIN32
 WSACleanup();
#endif
 return 0;
}
