#include "MinHeap.h"
#include <stdexcept>
void MinHeap::siftUp(std::size_t i){while(i>0){std::size_t p=(i-1)/2;if(data[p].distance<=data[i].distance)break;std::swap(data[p],data[i]);i=p;}}
void MinHeap::siftDown(std::size_t i){for(;;){std::size_t l=2*i+1,r=l+1,m=i;if(l<data.size()&&data[l].distance<data[m].distance)m=l;if(r<data.size()&&data[r].distance<data[m].distance)m=r;if(m==i)break;std::swap(data[i],data[m]);i=m;}}
bool MinHeap::empty()const{return data.empty();}std::size_t MinHeap::size()const{return data.size();}
void MinHeap::push(int v,double d){data.emplace_back(v,d);siftUp(data.size()-1);}
HeapNode MinHeap::pop(){if(data.empty())throw std::runtime_error("MinHeap is empty");HeapNode out=data.front();data.front()=data.back();data.pop_back();if(!data.empty())siftDown(0);return out;}
