//
//  B2Draw_OpenGL.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 3/5/18.
//

#include "B2Draw_OpenGL.hpp"

void B2Draw_OpenGL::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    
    glColor4f(color.r, color.g, color.b, 0.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < vertexCount; i++) {
        b2Vec2 v = vertices[i];
        glVertex2f(v.x, v.y);
    }
    glEnd();
}

void B2Draw_OpenGL::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    //set up vertex array
    GLfloat glverts[8]; //allow for polygons up to 4 vertices
    int floatCount = vertexCount * 2;
    
    //fill in vertex positions as directed by Box2D
    for (int i = 0; i < vertexCount; i++) {
        if(vertices[i].x > maxX || vertices[i].x < minX)
            return; //culling
        
        glverts[i*2]   = vertices[i].x;
        glverts[i*2+1] = vertices[i].y;
    }
    
    /* POSITIONS */
    GLuint DrawBufferObject;
    CHECKED_GL_CALL( glGenBuffers(1, &DrawBufferObject) );
    glBindBuffer(GL_ARRAY_BUFFER, DrawBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*floatCount, glverts, GL_STATIC_DRAW);
    
    /* INDICES */
    unsigned short idx[] = {0, 1, 2, 3};
    GLuint IndexBufferObject;
    CHECKED_GL_CALL( glGenBuffers(1, &IndexBufferObject) );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);
    //glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    
    CHECKED_GL_CALL( glDisableVertexAttribArray(0) );
}

void B2Draw_OpenGL::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
    const float32 k_segments = 16.0f;
    const int vertexCount = 16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    
    CHECKED_GL_CALL( glColor4f(color.r, color.g, color.b, 1) );
    CHECKED_GL_CALL( glBegin(GL_LINE_LOOP) );
    GLfloat glVertices[vertexCount * 2];
    for (int32 i = 0; i < k_segments; ++i) {
        b2Vec2 v = center + radius * b2Vec2(cos(theta), sin(theta));
        CHECKED_GL_CALL( glVertex3f(v.x, v.y, 0.0f) );
        theta += k_increment;
    }
    CHECKED_GL_CALL( glEnd() );
}

void B2Draw_OpenGL::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
    const float32 k_segments = 16.0f;
    const int vertexCount = 16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    
    glColor4f(color.r, color.g, color.b, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    GLfloat glVertices[vertexCount * 2];
    for (int32 i = 0; i < k_segments; ++i) {
        b2Vec2 v = center + radius * b2Vec2(cos(theta), sin(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
    
    DrawSegment(center, center + radius*axis, color);
}

void B2Draw_OpenGL::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    //set up vertex array
    int floatCount = 4;
    GLfloat glverts[4]; //allow for polygons up to 4 vertices
    
    glverts[0] = p1.x;
    glverts[1] = p1.y;
    glverts[2] = p2.x;
    glverts[3] = p2.y;
    
    /* POSITIONS */
    GLuint DrawBufferObject;
    CHECKED_GL_CALL( glGenBuffers(1, &DrawBufferObject) );
    glBindBuffer(GL_ARRAY_BUFFER, DrawBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)*floatCount, glverts, GL_STATIC_DRAW);
    
    /* INDICES */
    unsigned short idx[] = {0, 1};
    GLuint IndexBufferObject;
    CHECKED_GL_CALL( glGenBuffers(1, &IndexBufferObject) );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // draw!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferObject);
    //glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 2);
    
    CHECKED_GL_CALL( glDisableVertexAttribArray(0) );
}

void B2Draw_OpenGL::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
    glColor4f(color.r, color.g, color.b, 1);
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    glVertex2f(p.x, p.y);
    glEnd();
}

void B2Draw_OpenGL::DrawTransform(const b2Transform& xf) {
    
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;
    
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    DrawSegment(p1, p2, b2Color(1, 0, 0));
    
    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    DrawSegment(p1, p2, b2Color(0, 1, 0));
}
