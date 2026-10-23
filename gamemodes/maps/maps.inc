// Every map file is included here and loaded once on startup. Objects go through the streamer,
// so only those near a player are actually sent to them.
#include "maps/airport.inc"

stock Maps_Init()
{
    Map_Airport();
}
