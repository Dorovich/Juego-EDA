#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Vido

#define SCAN_DIST 15

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
        bool found = false, danger = false;

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

                if (danger and act.dist > 1) found = true;
                else if (cell(act.pos).food) {
                    unitp.prio = PRIO_FOOD;
                    found = true;
                }
                else if (enemy_found(act.pos) and act.dist < 3) {
                    if (act.dist == 1) unitp.prio = PRIO_KILL;
                    if (act.dist == 2) unitp.prio = PRIO_WAIT;
                    found = true;
                }
                else if (zombie_found(act.pos) and act.dist < 3) {
                    aux = act;
                    unitp.prio = PRIO_VOID;
                    unitp.prio = PRIO_VOID;
                    danger = true;
                }
                else if (cell(act.pos).owner != me() and act.dist < aux.dist) {
                    aux = act;
                    unitp.prio = PRIO_VOID;
                }

                scanq.push( cellNode {act.pos+Up,    act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Down,  act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Left,  act.dist+1, act.auxd} );
                scanq.push( cellNode {act.pos+Right, act.dist+1, act.auxd} );
            }
        }

        unitp.dir = (found ? (danger ? aux.auxd : act.auxd) : aux.auxd);
        return unitp;
    }

    /*
     * Check if something is blocking a position, if it was already scanned or if it is out of bounds.
     */
    bool legal_node (const cellNode& cn, const CharMat& visited, int myid) {
        Cell cl = cell(cn.pos);
        if (not pos_ok(cn.pos)) return false;
        if (cl.type == Waste) return false;
        if (cl.id != -1 and (unit(cl.id).type == Dead or unit(cl.id).player == me())) return false;
        if (visited[cn.pos.i][cn.pos.j] == myid) return false;
        if (cn.dist == SCAN_DIST) return false;
        return true;
    }

    /*
     * Check if an enemy unit is in a position. pos must be legal (see legal_node).
     */
    bool enemy_found (const Pos& pos) {
        return (cell(pos).id != -1 and unit(cell(pos).id).player != me());
    }

    /*
     * Check if a zombie unit is in a position. pos must be legal (see legal_node).
     */
    bool zombie_found (const Pos& pos) {
        return (cell(pos).id != -1 and unit(cell(pos).id).player == -1);
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
