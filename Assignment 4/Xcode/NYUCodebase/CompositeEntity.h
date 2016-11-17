//
//  Composite Entity.hpp
//  NYUCodebase
//
//  Created by Ifeoluwa Lawal on 11/2/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Composite_Entity_h
#define Composite_Entity_h

#include <stdio.h>
#include <vector>
#include "Entity.h"

class CompositeEntity : public Entity {
public:
    Entity baseEntity, subEntity;
    
    CompositeEntity();
    CompositeEntity(Entity base, Entity sub);
    
    void addEntity(Entity newE, std::string eName);
    
    void updateYPosition(Entity e, float elapsed);
    
    void updateXPosition(Entity e, float elapsed);
    
    void movement(ShaderProgram *program, Entity *other, float elapsed);
    
    bool isColliding(Entity *other);
    
    void setMatrices(ShaderProgram* program);
        
    void drawSprite(ShaderProgram* program);
};



#endif /* Composite_Entity_hpp */
