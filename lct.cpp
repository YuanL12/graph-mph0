#include <bits/stdc++.h>
using namespace std;

// Link-Cut Tree node, storing "edge‐weight to parent" in `value`.
struct Node {
    Node *left = nullptr, *right = nullptr, *parent = nullptr;
    bool reversed = false;

    int value;       // "edge‐weight to this node's parent in the represented tree"
    int subtreeMin;  // min over {value, left->subtreeMin, right->subtreeMin}
    int edgeIdx;     // store an index if this node is an edge, or -1 if a vertex

    Node(int v = INT_MAX, int idx = -1)
      : value(v), subtreeMin(v), edgeIdx(idx) {}
};

bool isRoot(Node* x) {
    return !x->parent 
        || (x->parent->left != x && x->parent->right != x);
}

// Push down the lazy tag to the children
void push(Node* x) {
    if (!x || !x->reversed) return;
    swap(x->left, x->right);
    if (x->left)  x->left->reversed  ^= true;
    if (x->right) x->right->reversed ^= true;
    x->reversed = false;
}

void pull(Node* x) {
    x->subtreeMin = x->value;
    if (x->left)  x->subtreeMin = min(x->subtreeMin, x->left->subtreeMin);
    if (x->right) x->subtreeMin = min(x->subtreeMin, x->right->subtreeMin);
}

void rotate(Node* x) {
    Node* p = x->parent;
    Node* g = p->parent;
    push(p); 
    push(x);

    bool xIsRight = (p->right == x);
    if (xIsRight) {
        p->right = x->left;
        if (x->left) x->left->parent = p;
        x->left = p;
    } else {
        p->left = x->right;
        if (x->right) x->right->parent = p;
        x->right = p;
    }

    p->parent = x;
    x->parent = g;
    if (g) {
        if (g->left == p)  g->left  = x; 
        else if (g->right == p) g->right = x;
    }
    pull(p);
    pull(x);
}

// reverse all ancestor nodes of x 
void propagateDown(Node* x) {
    if (!isRoot(x)) propagateDown(x->parent);
    push(x);
}

void splay(Node* x) {
    propagateDown(x);
    while (!isRoot(x)) {
        Node* p = x->parent;
        Node* g = p->parent;
        if (isRoot(p)) {
            rotate(x);  // Zig
        } else {
            bool xIsRight = (p->right == x);
            bool pIsRight = (g->right == p);
            if (xIsRight == pIsRight) {
                // Zig-Zig
                rotate(p);
                rotate(x);
            } else {
                // Zig-Zag
                rotate(x);
                rotate(x);
            }
        }
    }
    pull(x);
}

void access(Node* x) {
    Node* last = nullptr;
    while (x) {
        splay(x);
        x->right = last;
        pull(x);
        last = x;
        x = x->parent;   // climb up the represented tree
    }
}

// Find the root of u in the represented tree
Node* findRoot(Node* u) {
    access(u); splay(u); // u is now the root of its auxiliary tree
    while (u->left) {
        push(u);
        u = u->left;
    }
    splay(u); // make sure 'u' is splay‐root
    return u; // now u is also the represented‐tree root of u's component
}

// Returns true if u and v are in the same LCT‐tree (i.e. same connected component).
bool connected(Node* u, Node* v) {
    // A standard trick: expose each node, then see if their splay‐roots coincide.
    return findRoot(u) == findRoot(v);
}

void makeRoot(Node* u) {
    access(u); splay(u);
    u->reversed ^= true;
    push(u);
}

/*
    make x-...-...-y path a disjoint preferred path (aux tree), 
    make y as the aux root, 
    make x as the rep root
*/
void split(Node* x, Node* y){
    makeRoot(x);
    access(y); splay(y);
}

// Link by setting x->parent = y
inline bool link(Node* x, Node* y){
    makeRoot(x);
    if(findRoot(y)==x)return 0;
    x->parent = y;
    return 1;
}

inline bool cut(Node* x, Node* y){
    split(x, y); // now x is the root and y is the aux root
    if (y->left != x || x->parent != y) throw std::runtime_error("cannot cut because y->left != x or x->parent != y");
    y->left = nullptr;
    x->parent = nullptr;
    pull(y);
    return 1;
}

void linkEdge(int idx, Node* u, Node* v, int weight, Node* edgeNode[]) {
    // idx = index of this edge in E, u and v are endpoint‐vertices.
    // 1) Create or reinitialize the edge‐node, storing its weight & index.
    Node* e = edgeNode[idx];
    e->value = weight;
    e->edgeIdx = idx;
    pull(e);

    // 2) Link them together by light edges
    makeRoot(u);
    u->parent = e; 
    e->parent = v;
}

void cutEdge(Node* e) {
    if (e->edgeIdx == -1) throw std::runtime_error("cannot cutEdge because the input Node is not an edge node");

    // Remove edge‐node e from its two endpoint subtrees.
    access(e);
    splay(e);
    // break left and right relations of e if necessary
    if (e->left) {
        e->left->parent = nullptr;
        e->left = nullptr;
    }
    if (e->right) {
        e->right->parent = nullptr;
        e->right = nullptr;
    }
    e->parent = nullptr;
    pull(e);
    e->value = INT_MAX;   // optional: "erase" its weight
    e->edgeIdx = -1;      // optional: mark it no longer represents a real edge
}


int minEdgeWeight(Node* u, Node* v) {
    if (!connected(u,v)) return -1;   // or some sentinel meaning "no path." 

    makeRoot(u);
    access(v); splay(v);
    return v->subtreeMin;  // guaranteed to be < INT_MAX because there's at least one edge
}


int findMinEdgeIndex(Node* u, Node* v) {
    if (!connected(u,v)) return -1;

    // If you also want to know WHICH edge has that minimum weight,
    // you can descend the splay rooted at v to locate any node x
    // with x->subtreeMin == v->subtreeMin.  When found, x->edgeIdx
    // is the index of that original edge.
    makeRoot(u);
    access(v);
    splay(v);
    int target = v->subtreeMin;
    if (target == INT_MAX) return -1; // no edge on that path

    // Descend the splay to find some node whose value == target:
    Node* cur = v;
    while (true) {
        push(cur); // ensure children are correct
        if (cur->value == target && cur->edgeIdx >= 0) {
            return cur->edgeIdx;
        }
        if (cur->left && cur->left->subtreeMin == target) {
            cur = cur->left;
        } else if (cur->right && cur->right->subtreeMin == target) {
            cur = cur->right;
        } else {
            // We must be at the node itself (cur->value == target).
            return cur->edgeIdx;
        }
    }
}

// ——— Example usage ———

int main() {
    int N = 5; // number of original vertices
    int M = 5; // number of original edges
    // Suppose the edges are (0–1, w=3), (1–2, w=5), (1–3, w=2), (3–4, w=7)
    vector<array<int,3>> edges = {
      {0,1,3},
      {1,2,5},
      {1,3,2},
      {3,4,7},
      {0,4,4}
    };

    // 1) Create N "vertex‐nodes" and M "edge‐nodes":
    vector<Node*> vertexNode(N), edgeNode(M);
    for(int i = 0; i < N; i++) {
      vertexNode[i] = new Node(INT_MAX, -1);
    }
    for(int i = 0; i < M; i++) {
      edgeNode[i] = new Node(INT_MAX, -1);
    }

    // 2) Link the first 4 edges into the LCT
    for(int i = 0; i < 4; i++){
      int u = edges[i][0], v = edges[i][1], weight = edges[i][2];
      linkEdge(i, vertexNode[u], vertexNode[v], weight, edgeNode.data());
    }

    // 3) Query: minEdge(2,4) = should be min{ w(1–2)=3, w(1–3)=2, w(3–4)=7 } = 2
    int ans = minEdgeWeight(vertexNode[2], vertexNode[4]);
    cout << "minEdgeWeight on the path from 2 to 4: " << ans << "\n"; // should print "2"

    // 4) Find which edge is that "2"
    int eidx = findMinEdgeIndex(vertexNode[2], vertexNode[4]);
    cout << "that edge is index = " 
         << eidx 
         << " -> endpoints (" 
         << edges[eidx][0] << "," << edges[eidx][1] << ")\n";
    // should see "that edge is index = 2 -> endpoints (1,3)"

    // 5) Now remove that minimum edge from the tree:
    cutEdge(edgeNode[eidx]);

    // The represented tree is now disconnected.  If we ask minEdge(2,4) again,
    // the path no longer exists; v->subtreeMin will end up INT_MAX.
    ans = minEdgeWeight(vertexNode[2], vertexNode[4]);
    if (ans == -1) 
      cout << "2 and 4 are now disconnected.\n";

    // Now try to add another edge (0,4,4)
    linkEdge(4, vertexNode[0], vertexNode[4], 4, edgeNode.data());
    ans = minEdgeWeight(vertexNode[1], vertexNode[3]);
    cout << "minEdgeWeight on the path from 1 to 3: " << ans << "\n"; // should print "3"

    eidx = findMinEdgeIndex(vertexNode[1], vertexNode[3]);
    cout << "that edge is index = " 
         << eidx 
         << " -> endpoints (" 
         << edges[eidx][0] << "," << edges[eidx][1] << ")\n";
    // should see "that edge is index = 0 -> endpoints (0,1)"

    // now remove that minimum edge from the tree
    cutEdge(edgeNode[eidx]);

    // The represented tree is now disconnected.
    ans = minEdgeWeight(vertexNode[1], vertexNode[3]);
    if (ans == -1) 
      cout << "1 and 3 are now disconnected.\n";
    else
      cout << "Wrong: minEdgeWeight on the path from 1 to 3: " << ans << "\n"; // should print "3"

    ans = minEdgeWeight(vertexNode[1], vertexNode[4]);
    if (ans == -1) 
      cout << "1 and 4 are now disconnected.\n";
    else
      cout << "Wrong: minEdgeWeight on the path from 1 to 4: " << ans << "\n"; // should print "4"
    return 0;
}
