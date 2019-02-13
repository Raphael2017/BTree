 This is a simple implementation of a B+ tree
 
 The implemented interfaces include:
 
    bool storage::btree::insert(int key, void* record)
 
    bool storage::btree::find(int key, void*& out_record)
 
    bool storage::btree::erase(int key, void*& out_record)
    
    void storage::btree::clear()
    
    size_t storage::btree::size()
    
 Performance test of 1000w random records:
 
    BTREE_N 2
        btree::insert   ~23s
        btree::find     ~23s
        btree::erase    ~29s
        
    BTREE_N 10
        btree::insert   ~15s
        btree::find     ~13s
        btree::erase    ~18s
         
    BTREE_N 50
        btree::insert   ~10s
        btree::find     ~9s
        btree::erase    ~16s  
         
    BTREE_N 100
        btree::insert   ~10s
        btree::find     ~8s
        btree::erase    ~15s
        
    BTREE_N 200
        btree::insert   ~10s
        btree::find     ~8s
        btree::erase    ~15s   
 
    BTREE_N 250
        btree::insert   ~11s
        btree::find     ~8s
        btree::erase    ~16s 
        
    BTREE_N 300
        btree::insert   ~13s
        btree::find     ~7s
        btree::erase    ~18s 
        
    BTREE_N 500
        btree::insert   ~15s
        btree::find     ~8s
        btree::erase    ~21s 
    
 
 
 
 