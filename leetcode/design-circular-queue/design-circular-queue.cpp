#include "../header.h"

class MyCircularQueue {
public:
 MyCircularQueue(int k) : data_(k), front_(0), back_(0), size_(0) {}

 bool enQueue(int value) {
     if (size_ == data_.size()) {
        return false;
     }

     data_[back_++] = value;
     if (back_ == data_.size()) {
        back_ = 0;
     }
     ++size_;
     return true;
 }

 bool deQueue() {
     if (size_ == 0) {
         return false;
     }

     ++front_;
     if (front_ == data_.size()) {
         front_ = 0;
     }
     --size_;

     return true;
 }

 int Front() {
     if (size_ == 0) {
         return -1;
     }
     return data_[front_];
 }

 int Rear() {
     if (size_ == 0) {
         return -1;
     }
     return data_[(back_ + data_.size() - 1) % data_.size()];
 }

 bool isEmpty() {return size_ == 0;}

 bool isFull() { return size_ == data_.size(); }

private:
    vector<int> data_;
    size_t front_;
    size_t back_;
    size_t size_;
};

/**
 * Your MyCircularQueue object will be instantiated and called as such:
 * MyCircularQueue* obj = new MyCircularQueue(k);
 * bool param_1 = obj->enQueue(value);
 * bool param_2 = obj->deQueue();
 * int param_3 = obj->Front();
 * int param_4 = obj->Rear();
 * bool param_5 = obj->isEmpty();
 * bool param_6 = obj->isFull();
 */

int main() {
    Timer t("Subarray timer");

    MyCircularQueue myCircularQueue(3);
    cerr << myCircularQueue.enQueue(1) << endl;  // return True
    cerr << myCircularQueue.enQueue(2) << endl;  // return True
    cerr << myCircularQueue.enQueue(3) << endl;  // return True
    cerr << myCircularQueue.enQueue(4) << endl;  // return False
    cerr << myCircularQueue.Rear() << endl;      // return 3
    cerr << myCircularQueue.isFull() << endl;    // return True
    cerr << myCircularQueue.deQueue() << endl;   // return True
    cerr << myCircularQueue.enQueue(4) << endl;  // return True
    cerr << myCircularQueue.Rear() << endl;      // return 4

    return 0;
}
