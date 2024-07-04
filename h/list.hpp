
#ifndef _list_
#define _list_

template<typename T>
class List {
private:
    struct Node {
        T* data;
        Node* next;

        Node( T* data, Node* next ) : data(data), next(next) {}
    };

    Node *head, *tail;

public:
    List() : head(nullptr), tail(nullptr) {}

    List(const List<T>&) = delete;
    List<T> &operator=(const List<T>&) = delete;

    void addFirst( T* t ) {
        Node *newNode = new Node( t, head );
        head = newNode;
        if ( tail == nullptr ) {
            tail = newNode;
        }
    }

    void addLast( T* t ) {
        Node *newNode = new Node( t, nullptr );

        if ( tail != nullptr ) {
            tail = tail->next = newNode;
        }
        else {
            head = tail = newNode;
        }
    }

    T* getFirst() {
        if ( head == nullptr ) {
            return nullptr;
        }

        Node *node = head;
        head = head->next;

        if ( head == nullptr ) {
            tail = nullptr;
        }

        T* ret = node->data;
        delete node;
        return ret;
    }

    T* getLast() {
        if ( head == nullptr ) {
            return nullptr;
        }

        Node* prev = nullptr;
        for ( Node* cur = head; cur != nullptr && cur != tail; cur = cur->next ) {
            prev = cur;
        }

        Node* node = tail;
        if ( prev != nullptr ) {
            prev->next = nullptr;
        }
        else {
            head = nullptr;
        }
        tail = prev;

        T* ret = node->data;
        delete node;
        return ret;
    }

    T* peekFirst() {
        if ( head != nullptr ) {
            return head->data;
        }
        return nullptr;
    }

    T* peekLast() {
        if ( tail != nullptr ) {
            return tail->data;
        }
        return nullptr;
    }
};

#endif