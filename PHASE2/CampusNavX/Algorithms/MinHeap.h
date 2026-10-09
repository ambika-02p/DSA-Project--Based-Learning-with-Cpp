#ifndef MINHEAP_H
#define MINHEAP_H
#include <vector>
struct HeapNode { int vertex; double distance; HeapNode(int v=0,double d=0):vertex(v),distance(d){} };
class MinHeap {
    std::vector<HeapNode> data;
    void siftUp(std::size_t i);
    void siftDown(std::size_t i);
public:
    bool empty() const;
    std::size_t size() const;
    void push(int vertex,double distance);
    HeapNode pop();
};
#endif
