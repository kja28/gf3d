#ifndef __SNIPER_H__
#define __SNIPER_H__

#include "entity.h"

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* sniper_new(Vector3D position);


#endif
