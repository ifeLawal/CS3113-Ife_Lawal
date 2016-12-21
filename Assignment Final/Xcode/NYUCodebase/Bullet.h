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
    bool reset;
    
    Bullet(float wVal, float hVal, float s, bool st, EntityType eType, SpriteSheet *sheet);
    
    void movement(ShaderProgram *program, Entity *other, float elapsed);
    
    void movement(ShaderProgram *program, ReadTileMap rTM, Entity *other, int &score, float elapsed);
    
    bool xtileCollision(ReadTileMap rTM);

    void checkReset();
    
};

#endif /* Bullet_hpp */
