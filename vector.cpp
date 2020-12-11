#include "iostream"
using namespace std;

template <typename T>
class node
{
public:
    T content;
    node* next;
};

template <typename T>
class CArrayIterator;

template <typename T>
class CArrayConstIterator;

template<typename T>
class CArray
{
public:
    typedef T Element;
    typedef CArrayIterator<T> Iterator;
    typedef CArrayConstIterator<T> ConstIterator;
    typedef node<T> node;
public:
    CArray();
    ~CArray(){};
    
    int capacity() const;
    int size() const;
    
    void recap(int capacity);
    
    Element &at(int index);
    Element at(int index) const;
    
    void append(Element element);
    void insert(int index, Element element);
    
    void copy(const CArray &rhs);
    bool compare(const CArray &rhs) const;
    
    Iterator begin(){return Iterator(_head);}
    Iterator end(){return Iterator(_tail->next);}
    
    ConstIterator begin()const {return ConstIterator(_head);}
    ConstIterator end()const {return ConstIterator(_tail->next);}
    
    
private:
    node* _head;
    node* _tail;
    void _check_capacity(int minimal)//!!!!!!!!!!!!!!!unused!!!!!
    {
        if(minimal>_capital)
            _capital=minimal;
    }
    int _capital;//_cap
    int _len;//_size
};

template <typename T>
CArray<T>::CArray()
{
    _head=NULL;
    _tail=NULL;
    _capital=0;
    _len=0;
}

/*
template <typename T>
CArray<T>::~CArray()
{
    _capital=0;
    _len=0;
    while(_head!=NULL)
    {
        node* p=_head;
        _head=_head->next;
        delete p;
    }
    
}
*/
template <typename T>
int CArray<T>::capacity() const
{
    return _capital;
}

template <typename T>
int CArray<T>::size() const
{
    return _len;
}

template <typename T>
void CArray<T>::recap(int newCap)
{
    _capital=newCap;
}

template <typename T>
T& CArray<T>::at(int index)
{
    node* t=_head;
    for(int i=0;i<index;i++)
        t=t->next;
    
    return t->content;
    
}

template <typename T>
T CArray<T>::at(int index) const
{
    node* t=_head;
    for(int i=0;i<index;i++)
        t=t->next;
    
    return t->content;
}

template <typename T>
void CArray<T>::append(Element element)
{
    node* p=new node;
    p->content=element;
    p->next=NULL;
    if(_head==NULL)
        _head=p;
    else
        _tail->next=p;
    _tail=p;
    
    _len++;
}

template <typename T>
void CArray<T>::insert(int index, Element element)
{
    node* p=new node;
    p->content=element;
    p->next=NULL;
    if(_head==NULL)
    {
        _head=p;
        _tail=p;
    }
    else
    {
        if(_len==index)
        {
            if(index==0)
            {
                _head=p;
                _tail=p;
            }
            else
                _tail->next=p;
            _tail=p;
        }
        else if(index==0)
        {
            p->next=_head;
            _head=p;
        }
        else
        {
            node* pre=_head;
            for(int i=1;i<index;i++)
                pre=pre->next;
            p->next=pre->next;
            pre->next=p;
        }
    }
    _len++;
}

template <typename T>
void CArray<T>::copy(const CArray<T> &rhs)
{
    //destroy
    _capital=rhs._capital;
    _len=rhs._len;
    
    while(_head!=NULL)
    {
        node* p=_head;
        _head=_head->next;
        delete p;
    }
    
    for(node* p=rhs._head;p!=NULL;p=p->next)
    {
        node* t=new node;
        t->content=p->content;
        t->next=NULL;
        if(_head==NULL)
            _head=t;
        else
            _tail->next=t;
        _tail=t;
    }
}

template <typename T>
bool CArray<T>::compare(const CArray<T> &rhs) const
{
    if(_len!=rhs._len)
        return false;
    else
    {
        int cnt=0;
        node* p1=_head;
        node* p2=rhs._head;
        for(int i=0;i<_len;i++)
        {
            if(p1->content!=p2->content)
                cnt++;
            p1=p1->next;
            p2=p2->next;
        }
        if(cnt==0)
            return true;
        else
            return false;
    }
}

template <typename T>
class CArrayIterator
{
    typedef node<T> node;
public:
    CArrayIterator() {}
    CArrayIterator(node *current):_current(current){};
    CArrayIterator(const CArrayIterator &rhs):_current(rhs._current){};
    ~CArrayIterator() {}
    
public:
    //实现所需要的操作符重载函数
    CArrayIterator& operator++()
    {
        _current=_current->next;
        return *this;
    }
    const CArrayIterator operator++(int)
    {
        CArrayIterator temp=*this;
        ++(*this);
        return temp;
    }
    
    bool operator==(CArrayIterator rhs){return _current==rhs._current;}//???
    bool operator!=(CArrayIterator rhs){return !(*this==rhs);}
    
    T& operator*()const{return _current->content;}
    T* operator->()const{return &_current->content;}
    
protected:
    node *_current;
    friend class CArray<T>;
};


template <class T>
class CArrayConstIterator
{
    typedef node<T> node;
public:
    CArrayConstIterator() {}
    CArrayConstIterator(node *current):_current(current){};
    CArrayConstIterator(const CArrayConstIterator &rhs):_current(rhs._current){};
    ~CArrayConstIterator() {}
    
public:
    //实现所需要的操作符重载函数
    CArrayConstIterator& operator++()
    {
        _current=_current->next;
        return *this;
    }
    
    const CArrayConstIterator operator++(int)
    {
        CArrayConstIterator temp=*this;
        ++(*this);
        return temp;
    }
    
    bool operator==(CArrayConstIterator rhs){return _current==rhs._current;}
    bool operator!=(CArrayConstIterator rhs){return !(*this==rhs);}
    T& operator*()const{return _current->content;}
    T* operator->()const{return _current->content;}
    
protected:
    node *_current;
    friend class CArray<T>;
};



template <typename T>
class Obj
{
public:
    Obj(){};
    Obj(int val) : data(val){};
    ~Obj(){};
    
public:
    T data;
    
private:
    template <typename Tp>
    friend std::ostream &operator<<(std::ostream &os, const Obj<Tp> &obj);
};

template <typename Tp>
std::ostream &operator<<(std::ostream &os, const Obj<Tp> &obj)
{
    os << obj.data;
    return os;
}

int main(int argc, char const *argv[])
{
    CArray<int> array;
    // 不再需要initial，但应该有正确的初始化
    // array_initial(array);
    array.recap(10);
    //assert(array.capacity() == 10);
    //////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < 20; ++i)
    {
        array.append(i);
    }
    //assert(array.size() == 20);

    
    
    //////////////////////////////////////////////////////////////////////////
    CArray<int> array2, array3;
    // array_initial(array2);
    // array_initial(array3);
    array2.copy(array);
    assert(array.compare(array2) == true);
    array3.copy(array);
    assert(array.compare(array3) == true);
    //////////////////////////////////////////////////////////////////////////
    array2.insert(2, 3);
    assert(array.compare(array2) == false);
    //////////////////////////////////////////////////////////////////////////
    array3.at(2) = 5;
    assert(array.compare(array3) == false);
    //////////////////////////////////////////////////////////////////////////
    //array:0,....,19
    //array2: 0,2,1,...19
    //array3: 0,5,2,...19
    for (CArray<int>::Iterator it = array.begin(); it != array.end();++it)
    {
        std::cout << *it << " ";
        (*it) = (*it) + 10;
    }
    
    //array: 10,11,...,29
    
    std::cout << std::endl;
    for (CArray<int>::Iterator it = array.begin(); it != array.end();)
    {
        std::cout << *(it++) << " ";
    }
    std::cout << std::endl;
    
    
    
    const CArray<int> tmp(array);
    
    
     for (CArray<int>::ConstIterator it = tmp.begin(); it != tmp.end(); ++it)
     {
     std::cout << *it << " ";
     }
     std::cout << std::endl;
    
     CArray<Obj<int>> obj_arr;
     for (CArray<int>::ConstIterator it = tmp.begin(); it != tmp.end();)
     {
     obj_arr.append(Obj<int>(*it));
     std::cout << *(it++) << " ";
     }
     std::cout << std::endl;
     
     for (CArray<Obj<int>>::Iterator it = obj_arr.begin(); it != obj_arr.end(); ++it)
     {
     std::cout << (*it) << " ";
     }
     std::cout << std::endl;
     for (CArray<Obj<int>>::Iterator it = obj_arr.begin(); it != obj_arr.end(); ++it)
     {
     std::cout << it->data << " ";
     }
     std::cout << std::endl;
    cout<<"finish"<<endl;
    
    return 0;
}
