#pragma once
#ifndef __TRANSFORMHOLDER_H__
#define __TRANSFORMHOLDER_H__

#include "quaternion.h"
#include "vec3.h"

struct TransformHolder
{
    Quaternion rotation;
    Vec3 position;
    Vec3 scale;
};

#endif