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
    return (bp.down ? Right : Down);
}
