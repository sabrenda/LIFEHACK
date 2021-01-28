#include        <windows.h>
#include        <gl\gl.h>

void    drawBox ( GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2 )
{
        glBegin ( GL_POLYGON );         // front face
          glNormal3f ( 0.0, 0.0, 1.0 );
          glVertex3f ( x1, y1, z2 );
          glVertex3f ( x2, y1, z2 );
          glVertex3f ( x2, y2, z2 );
          glVertex3f ( x1, y2, z2 );
        glEnd ();

        glBegin ( GL_POLYGON );         // back face
          glNormal3f ( 0.0, 0.0, -1.0 );
          glVertex3f ( x2, y1, z1 );
          glVertex3f ( x1, y1, z1 );
          glVertex3f ( x1, y2, z1 );
          glVertex3f ( x2, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // left face
          glNormal3f ( -1.0, 0.0, 0.0 );
          glVertex3f ( x1, y1, z1 );
          glVertex3f ( x1, y1, z2 );
          glVertex3f ( x1, y2, z2 );
          glVertex3f ( x1, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // right face
          glNormal3f ( 1.0, 0.0, 0.0 );
          glVertex3f ( x2, y1, z2 );
          glVertex3f ( x2, y1, z1 );
          glVertex3f ( x2, y2, z1 );
          glVertex3f ( x2, y2, z2 );
        glEnd ();

        glBegin ( GL_POLYGON );         // top face
          glNormal3f ( 0.0, 1.0, 0.0 );
          glVertex3f ( x1, y2, z2 );
          glVertex3f ( x2, y2, z2 );
          glVertex3f ( x2, y2, z1 );
          glVertex3f ( x1, y2, z1 );
        glEnd ();

        glBegin ( GL_POLYGON );         // bottom face
          glNormal3f ( 0.0, -1.0, 0.0 );
          glVertex3f ( x2, y1, z2 );
          glVertex3f ( x1, y1, z2 );
          glVertex3f ( x1, y1, z1 );
          glVertex3f ( x2, y1, z1 );
        glEnd ();
}
