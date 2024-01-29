#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Vido

#define SCAN_DIST 10

#define PRIO_KILL 2
#define PRIO_FOOD 1
#define PRIO_VOID 0
#define PRIO_WAIT -1

struct PLAYER_NAME : public Player {

    /**
      * Factory: returns a new instance of this class.
      * Do not modify this function.
      */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
      * Types and attributes for your player can be defined here.
      */
    struct uPrio { int id, prio; Dir dir; };
    struct cellNode { Pos pos; int dist; Dir auxd; };
    struct blockedPos { bool up, down, left, right; };
    typedef vector<uPrio> PrioVec;
    typedef queue<cellNode> NodeQue;
    typedef vector<unsigned char> CharLine;
    typedef vector<CharLine> CharMat;

    /**
      * Play method, invoked once per each round.
      */
    virtual void play () {
        vector<int> alive = alive_units(me());
        int nalive = alive.size();

        PrioVec idPrio (nalive);
        CharMat visited (board_rows(), CharLine (board_cols(), 0));

        for (int i = 0; i < nalive; ++i)
            idPrio[i] = scan(alive[i], visited);

        sort (idPrio.begin(), idPrio.end(), priocmp);

        for (int i = 0; i < nalive; ++i) 
            move(idPrio[i].id, idPrio[i].dir);
    }

    uPrio scan (int id, CharMat& visited) {
        uPrio unitp;
        NodeQue scanq;
        Pos root = unit(id).pos;
        cellNode act, aux = { root, SCAN_DIST };
        blockedPos bp = { false, false, false, false };
        bool found = false;

        unitp.id = visited[root.i][root.j] = id;

        scanq.push( cellNode {root+Up,    1, Up   } );
        scanq.push( cellNode {root+Down,  1, Down } );
        scanq.push( cellNode {root+Left,  1, Left } );
        scanq.push( cellNode {root+Right, 1, Right} );

        while (not found and not scanq.empty()) {
            act = scanq.front();
            scanq.pop();

            if (legal_node(act, visited, id)) {
                visited[act.pos.i][act.pos.j] = id;

                if (cell(act.pos).food) {
                    unitp.prio = PRIO_FOOD;
                    found = true;
                }
                else if (enemy_found(act.pos) and act.dist < 3) {
                    if (act.dist == 1) unitp.prio = PRIO_KILL;
                    if (act.dist == 2) unitp.prio = PRIO_WAIT;
                    found = true;
                }
                else if (cell(act.pos).owner != me() and act.dist < aux.dist) {
                    aux = act;
                    unitp.prio = PRIO_VOID;
                }

                scanq.push( cellNode {act.pos+Up,    act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Down,  act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Left,  act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Right, act.dist+1, act.auxd} );
            } else {
                if (act.auxd == Up) bp.up = true;
                if (act.auxd == Down) bp.down = true;
                if (act.auxd == Left) bp.left = true;
                if (act.auxd == Right) bp.right = true;
            }
        }

        unitp.dir = (found ? dir_from_root(act.pos, root, bp) : dir_from_root(aux.pos, root, bp));
        return unitp;
    }

    /*
     * Check if something is blocking a position, if it was already scanned or if it is out of bounds.
     */
    bool legal_node (const cellNode& cn, const CharMat& visited, int myid) {
        if (not pos_ok(cn.pos)) return false;
        if (cell(cn.pos).type == Waste) return false;
        if (cell(cn.pos).id != -1 and unit(cell(cn.pos).id).type == Dead) return false;
        if (visited[cn.pos.i][cn.pos.j] == myid) return false;
        if (cn.dist == SCAN_DIST) return false;
        return true;
    }

    /*
     * Check if an enemy unit is in a position. pos must be legal (see legal_node). pl is the player that owns the unit.
     */
    bool enemy_found (const Pos& pos) {
        return (cell(pos).id != -1 and unit(cell(pos).id).player != me());
    }

    /*
     * Returns relative direction of pos from root.
     */
    Dir dir_from_root (const Pos& pos, const Pos& root, const blockedPos& bp) {
        int dx = root.i - pos.i;
        int dy = root.j - pos.j;
        if (dx == 0) return (dy > 0 ? Left : Right);
        if (dy == 0) return (dx > 0 ? Up : Down);
        if (dx > 0 and dy > 0) return (bp.up ? Left : Up);
        if (dx > 0 and dy < 0) return (bp.up ? Right : Up);
        if (dx < 0 and dy > 0) return (bp.down ? Left : Down);
        /* if dx<0 and dy<0 */ return (bp.down ? Right : Down);
    }

    /**
     * Sort priority vector function.
     */
    static bool priocmp (const uPrio& A, const uPrio& B) { return A.prio > B.prio; }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
