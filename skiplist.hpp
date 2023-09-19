#ifndef KEY_VALUE_SKIPLIST_BASE
#define KEY_VALUE_SKILLIST_BASE
#include<iostream>
#include<vector>
#include<utility>
#include<math.h>
#include"ToolFunctions_H.h"
#include"rwlock.h"

namespace dbase {
    enum AddType {
        AddNewNode = 0,
        ChangeNode,
        ChangeOrAdd
    };
    template<typename Key, typename Value>
    struct Node {
        template<typename Key, typename Value>
        friend class SkipList;
        //struct NodePtrs {
        //    Node* pre_;
        //    Node* next_;
        //    NodePtrs() {
        //        pre_ = NULL;
        //        next_ = NULL;
        //    }
        //};
        Node(int max_level):max_level_(max_level) {
            pre_.assign(max_level, nullptr);
            next_.assign(max_level, nullptr);
            //nodeptrs.assign(max_level, NodePtrs());
            cur_level_ = 0;
            //pre_ = NULL;
            //down_ = NULL;
            //next_ = NULL;
        }
        Node(int level, int max_level) :cur_level_(level), max_level_(max_level) {
            pre_.assign(max_level, nullptr);
            next_.assign(max_level, nullptr);
            //nodeptrs.assign(max_level, NodePtrs());
            //初始化没有传入max_level，没有初始化nodeptrs使得值写入时指向了空指针
        }
        Node(Key key, Value value, int level, int max_level) :cur_level_(level), value_(std::pair<Key, Value>(key, value)), max_level_(max_level) {
            pre_.assign(max_level, nullptr);
            next_.assign(max_level, nullptr);
            //nodeptrs.assign(max_level, NodePtrs());
            //初始化没有传入max_level，没有初始化nodeptrs使得值写入时指向了空指针
        }
        Node(const Node& a) {
            cur_level_ = a.cur_level_;
            value_ = a.value_;
            max_level_ = a.max_level_;
            pre_ = a.pre_;
            next_ = a.next_;
        }
        bool operator==(const Node& other) {
            return value_.first == other.value_.first;
        }
        bool operator!=(const Node& other) {
            return value_.first != other.value_.first;
        }
        std::vector<Node*> pre_;  //指向每一层的上一个
        std::vector<Node*> next_;  //指向每一层的下一个
        //用一个数组保存上下节点的做法太蠢了
        //std::vector<NodePtrs> nodeptrs; //用数组来保存上下节点的指针，一个节点的空间来实现逻辑上的多层节点
        //Node* down_;  //指向下一层   这个做法太费空间了
        int cur_level_;  //当前层
        std::pair<const Key, Value> value_;  //键值对
        int max_level_; //最大值
    };
    template<typename Key, typename Value>
    class SkipList {
    public:
        /////////////////////////////////////待验证///////////////////////////////////////////////////////
        class iterator {
        public:
            iterator() = default;
            iterator(Node<Key, Value>* node) : it_(node) {
                if (node != NULL) {
                    cur_level_ = node->cur_level_;
                }
            }
            //当我传入nullptr时调用的是下面这个函数，为什么不是上面这个
            //iterator(const Node<Key, Value>* node) :it_(node), cur_level_(node->cur_level_) {}
            iterator operator++(int) {
                Node<Key, Value>* temp = it_;
                it_ = it_->next_[cur_level_];
                return iterator(temp);
            }
            iterator operator--(int) {
                Node<Key, Value>* temp = it_;
                it_ = it_->pre_[cur_level_];
                return iterator(temp);
            }
            iterator& operator++() {
                it_ = ((it_->next_)[cur_level_]);
                return *this;
            }
            iterator& operator--() {
                it_ = it_->pre_[cur_level_];
                return *this;
            }
            std::pair<const Key, Value> operator*() {
                return it_->value_;
            }
            std::template pair<const Key, Value>* operator->() {
                return &it_->value_;
            }
            bool operator==(const iterator& other) {
                return it_ == other.it_;
            }
            bool operator!=(const iterator& other) {
                return it_ != other.it_;
            }
        private:
            Node<Key, Value>* it_ = NULL;
            int cur_level_;
        };  //迭代器
        /////////////////////////////////////待验证///////////////////////////////////////////////////////
    public:
        SkipList(int max_level);
        ~SkipList();
        int initialize();
        int size(int n = 0) const;
        int add_value(Key key, Value value, int flag);
        int erase_elem(Key key);
        int search_key(Key key) const;
        int search_value(Value value) const;
        Value get_value(Key key) const;
        void display_list(void (*foreach_echo_key)(void*), void (*foreach_echo_value)(void*))const;

    protected:
        int stream_from_file(const char* path, SkipList* in);
        int stream_to_file(const char* path, SkipList* out);
        Node<Key, Value>* create_node(int max_level);
        Node<Key, Value>* create_node(int level, int max_level);
        Node<Key, Value>* create_node(Key key, Value value, int level, int max_level);
        int get_random_level() const;
        //int get_add_location(Key key, int level, std::vector<Key>& out);
        int delete_value();
        iterator begin(int a) const{ 
            return iterator(level_head_node_[a]); }
        iterator end() const{ return iterator(nullptr); }
    private:
        std::vector<Node<Key, Value>*> level_head_node_;  //每层头节点[0,max_level_)
        int cur_level_;
        std::vector<int> per_level_len_;  //记录每一层的宽度
        //Node<Key, Value>* head_node_;  //要插入的头节点
        int max_level_;  //最大层的上限
        RwLock rwlock;  //读写锁
    };

    template<typename Key, typename Value>
    SkipList<Key, Value>::SkipList(int max_level) : max_level_(max_level), cur_level_(0) {
        level_head_node_.clear();
    }
    template<typename Key, typename Value>
    SkipList<Key, Value>::~SkipList() {
        rwlock.rwlock_destroy();
        //for (int i = 0; i < level_head_node_.size(); i++) {
        //    //while (level_head_node_[i] != NULL) {
        //    //    Node<Key, Value>* temp = level_head_node_[i]->next_;
        //    //    level_head_node_[i]->head_ = NULL;
        //    //    level_head_node_[i]->next_ = NULL;
        //    //    level_head_node_[i]->down_ = NULL;
        //    //    delete level_head_node_[i];
        //    //    level_head_node_[i] = temp;
        //    //}
        //}
        DEBUG_PRINT_LINE("skiplist exit");
    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::initialize() {
        srand((unsigned)time(NULL));
        rwlock.rwlock_init();
        for (int i = 0; i < max_level_; i++) {
            //初始化没有把每一层的头节点的当前层初始化，使得后面用迭代器通过头节点遍历时，每次都访问的第一层为NULL
            Node<Key, Value>* it = create_node(i, max_level_);
            if (it == NULL) {
                ERROR_PRINT_LINE("Create node fail!");
                return -1;
            }
            //初始化时插入头节点
            level_head_node_.push_back(it);
            //level_head_node_[i]->nodeptrs[i].next_ = NULL;
            //level_head_node_[i]->nodeptrs[i].pre_ = NULL;
            //per_level_len_.push_back(0);
        }
        per_level_len_.assign(max_level_, 0);
        //head_node_ = level_head_node_[0];
        DEBUG_PRINT_LINE("Skiplist initialize success!");
        return 0;
    }

    template<typename Key, typename Value>
    int SkipList<Key, Value>::size(int n) const {
        return per_level_len_[n];
    }
    /////////////////////////////////////////////////////////////////////////////////
    //template<typename Key, typename Value>
    //int SkipList<Key, Value>::add_value(Key key, Value value, int flag) {
    //    int level = get_random_level();

    //    //Node* cur_ptr = head_node_[level];

    //    std::vector<Key> out;
    //    get_add_location(key, level, out);
    //    //for (int i = level; i > -1; i--) {
    //    //    //找到插入位置
    //    //    while (cur_ptr->next_ != NULL && cur_ptr->next_->value_.first < key) {
    //    //        cur_ptr = cur_ptr->next;
    //    //    }
    //    //    //创建节点
    //    //    Node<Key, Value>* new_node = create_node();
    //    //    if (new_node == NULL) {
    //    //        ERROR_PRINT_LINE("create node fail!");
    //    //        return -1;
    //    //    }
    //    //    new_node->cur_level_ = level;
    //    //    //插入
    //    //    std::pair<Key, Value> val(key, value);
    //    //    new_node->value_ = val;
    //    //    new_node->next_ = cur_ptr->next_;
    //    //}
    //    /////////////////////////////////////////////////////////////////////////////
    //    return 0;
    //}
    //是否可以遍历时找到要插入的节点位置时对前后节点上锁
    template<typename Key, typename Value>
    int SkipList<Key, Value>::add_value(Key key, Value value, int flag) {
        int level = get_random_level(); 

        //DEBUG_PRINT_LINE("Key is %d, Value is %d, level is %d", key, value, level);
        //DEBUG_PRINT_LINE("Key is %d, Value is %s, level is %d", key, value.c_str(), level);

        //Node<Key, Value> new_node(key, value, level);
        Node<Key, Value> *new_node = create_node(key, value, level, max_level_);
        if (level > cur_level_) {
            cur_level_ = level;
        }

        for (int i = level; i > -1; --i) {
            //for (iterator it = level_head_node_[i]; it != end(); it++) {
            //    ////////////////////////////////遍历level>0时报错/////////////level_head_node_[i]没有初始化？////////////////////
            //    /////////////////////////////////已解决，迭代器等于判断有问题//////////////////////////////////////////////////////
            //}
            Node<Key, Value>* p = level_head_node_[i];
            
            //找到每层插入的位置
            while (p->next_[i] != nullptr && p->next_[i]->value_.first < key)p = p->next_[i];
            if (flag == AddNewNode && p->next_[i]->value_.first == key) {
                ERROR_PRINT_LINE("This Key already exists!");
                return -1;
            }
            //插入
            if(p->next_[i] != nullptr)
                p->next_[i]->pre_[i] = new_node;
            new_node->next_[i] = p->next_[i];
            new_node->pre_[i] = p;
            p->next_[i] = new_node;
        }
        return 0;
    }

    template<typename Key, typename Value>
    int SkipList<Key, Value>::erase_elem(Key key) {

    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::search_key(Key key) const {

    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::search_value(Value value) const {

    }
    template<typename Key, typename Value>
    Value SkipList<Key, Value>::get_value(Key key) const {

    }
    template<typename Key, typename Value>
    void SkipList<Key, Value>::display_list(void (*foreach_echo_key)(void*), void (*foreach_echo_value)(void*)) const {
        for (int i = cur_level_; i > -1; --i) {
            printf("\033[0;31;40m[level: %d]\033[0m", i);
            auto it = begin(i);

            for (++it; it != end(); ++it) {
                printf("(");
                auto k = it->first;
                auto v = it->second;
                foreach_echo_key( (void*) & k);
                printf(", ");
                foreach_echo_value((void*)&v);
                printf(") ");
            }
            printf("\n");
        }
        printf("\n");
    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::delete_value() {

    }

    template<typename Key, typename Value>
    int SkipList<Key, Value>::stream_from_file(const char* path, SkipList* in) {

    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::stream_to_file(const char* path, SkipList* out) {

    }
    template<typename Key, typename Value>
    Node<Key, Value>* SkipList<Key, Value>::create_node(int max_level) {
        return new Node<Key, Value>(max_level);
    }    
    template<typename Key, typename Value>
    Node<Key, Value>* SkipList<Key, Value>::create_node(int level, int max_level) {
        return new Node<Key, Value>(level, max_level);
    }
    template<typename Key, typename Value>
    Node<Key, Value>* SkipList<Key, Value>::create_node(Key key, Value value, int level, int max_level) {
        return new Node<Key, Value>(key, value, level, max_level);
    }
    template<typename Key, typename Value>
    int SkipList<Key, Value>::get_random_level() const {
        int cur_level = 0;
        while ((rand() % 2) < 1 && cur_level + 1 < max_level_)++cur_level;
        //while ((rand() % 2) < 1 && ++cur_level < max_level_);
        //DEBUG_PRINT_LINE("%d", cur_level);
        return cur_level;
    }
}



#endif
