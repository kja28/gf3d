#ifndef __FLYING_H__
#define __FLYING_H__

#include "entity.h"

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* flying_new(Vector3D position);


#endif
