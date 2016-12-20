//
//  Bullet.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 12/20/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Bullet_hpp
#define Bullet_hpp

#include <stdio.h>
#include "Entity.h"

class Bullet: public Entity {
public:
    Bullet(float wVal, float hVal, float s, bool st, EntityType eType, SpriteSheet *sheet);
    
    void movement(ShaderProgram *program, Entity *other, float elapsed);
    
    void xtileCollision(ReadTileMap rTM);
    
    
};

#endif /* Bullet_hpp */
