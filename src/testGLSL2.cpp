// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "GLMaterial.h"
#include "GLLight.h"
#include "GLUtils.h"
#include "GLMaterial.h"
#include "Util.h"
#include "Imageloader.h"
#include "GLSL.h"
#include "Arcball.h"
#include "ObjLoader.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;
static const double BOX_SIZE=2.0;

using namespace std;
int width=1280, height=1024;
double eyeZ=2;
double eyeDistance=2;
double FOV = 45;
double frame=0.0;

std::string imageNameFile;
Arcball arcball;
ObjLoader obj;

glsl::glShaderManager *SM;
glsl::glShader *shader;
GLuint _textureId;
GLuint cubeMapTexturesId[6];

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 image->width, image->height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    return textureId;
}



void handleKeypress(unsigned char key, int x, int y)
{  switch (key)
    {  case 'q': //Escape key
        delete shader;
        exit(0);
        break;
    case '+':
        eyeDistance+=0.5;
        break;
    case '-':
        eyeDistance-=0.5;
        break;
    }
}

void initRendering()
{
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable (GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.0,0.0,0.0,1.0);

    GLfloat ambientLight[] = {1.0,1.0,1.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    GLfloat lightColor[] = {1.0f,1.0f,1.0f,1.0f};

    GLfloat lightPos[] = {200,100, -100.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_SPECULAR, standardSpecular);

    // Object loading
    //obj.load("data/objmodels/face.obj");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,eyeDistance);

    // Simple2D texture things
    //Image *im = loadBMP("data/textures/grace_1536SCR_10_LL.bmp");
    cout << "XXXX "<< imageNameFile << endl;
    Image *im = loadBMP(imageNameFile.c_str());
    //Image *im = loadBMP("data/textures/envmap.bmp");
    _textureId =loadTexture(im);
    //_textureId = generateCirclesTexture(1280,1280,150,5,80,true);//loadTexture(im);
    delete im;
}

void handleResize(int w, int h)
{
    arcball.setWidthHeight(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV, (float)w / (float)h, 0.1, 200.0);
}

void drawThings()
{
    glPushMatrix();
    glLoadIdentity();
    glTranslated(0,0,-5.5+eyeDistance);

    arcball.applyRotationMatrix();

    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, _textureId);


    shader->begin();
    //shader->setUniform1f("c",sin(frame)+1.0);

    GLUquadric *quad = gluNewQuadric();
    //gluQuadricDrawStyle(quad,GLU_POINT);
    gluDisk(quad,0.0,0.5,100,1000);
    gluDeleteQuadric(quad);

    //drawParaboloidConstantSurfaceElement(-0.5,0.5,-0.5,0.5,250,250);
    shader->end();
    glDisable(GL_TEXTURE_2D);
/*
    GLUquadric *quad2 = gluNewQuadric();
    gluQuadricDrawStyle(quad2,GLU_LINE);
    gluDisk(quad2,0.0,1.0,100,100);
    gluDeleteQuadric(quad2);
*/
/*
    glBegin(GL_LINE_STRIP);
    for (double theta=0; theta<2*M_PI; theta+=0.1)
    {
        glVertex3d(cos(theta),sin(theta),0);
    }
    glEnd();
*/
glPopMatrix();


    getGLerrors();

}

void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    frame+=0.01;
    arcball.applyRotationMatrix();

    glTranslatef(0.0f, 0.0f, eyeDistance);

    drawThings();
    glutSwapBuffers();
}
bool rightPressed=false;

void mouseFunc(int state, int button, int _x , int _y)
{  if ( button == GLUT_LEFT_BUTTON )
        arcball.startRotation(_x,_y);
    else
        arcball.stopRotation();

    if ( button == GLUT_RIGHT_BUTTON )
        rightPressed=true;
    else
        rightPressed=false;

    glutPostRedisplay();
}

void mouseDrag(int _x, int _y)
{
    arcball.updateRotation(_x,_y);
    glutPostRedisplay();
}


//Called every 15 milliseconds
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}

int main(int argc, char* argv[])
{
    if (argc<2)
    {
        cerr << "Usage: ./testGLSL2 shadername texture.bmp" << endl;
        cerr << "will load shadername.vert and shadername.frag" << endl;
        exit(0);
    }
    
    
    string shaderfile(argv[1]);
    imageNameFile = string(argv[2]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);

    glutCreateWindow("test GLSL 2");
    initRendering();

    // Load (and compile, link) from file
    string shadervert = shaderfile+".vert";
    string shaderfrag = shaderfile+".frag";

    SM = new glsl::glShaderManager();
    /*
    SM.setInputPrimitiveType(GL_TRIANGLES);
    SM.setOutputPrimitiveType(GL_TRIANGLE_STRIP);
    SM.setVerticesOut(3);
*/
    shader = SM->loadfromFile((char*)shadervert.c_str(), (char*)shaderfrag.c_str());
    delete SM;
    
    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutTimerFunc(15, update, 0);

    glutMainLoop();
    return 0;
}
