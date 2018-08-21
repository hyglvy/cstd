
#include <windows.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

void background(void)
{
  glClearColor(0.0,0.0,0.0,0.0);//���ñ�����ɫΪ��ɫ
}
#if 0

void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT);//buffer����Ϊ��ɫ��д
  
    glBegin(GL_TRIANGLES);//��ʼ��������
    glShadeModel(GL_SMOOTH);//����Ϊ�⻬����ģʽ
  
    glColor3f(1.0,0.0,0.0);//���õ�һ������Ϊ��ɫ
    glVertex2f(-1.0,-1.0);//���õ�һ�����������Ϊ��-1.0��-1.0��
  
    glColor3f(0.0,1.0,0.0);//���õڶ�������Ϊ��ɫ
    glVertex2f(0.0,-1.0);//���õڶ������������Ϊ��0.0��-1.0��
  
    glColor3f(0.0,0.0,1.0);//���õ���������Ϊ��ɫ
    glVertex2f(-0.5,1.0);//���õ��������������Ϊ��-0.5��1.0��
    glEnd();//�����ν���
  
    glFlush();//ǿ��OpenGL����������ʱ��������
}

void myReshape(GLsizei w,GLsizei h)
{
    glViewport(0,0,w,h);//�����ӿ�
  
    glMatrixMode(GL_PROJECTION);//ָ����ǰ����ΪGL_PROJECTION
    glLoadIdentity();//����ǰ�����û�Ϊ��λ��
    
    if(w <= h)
      gluOrtho2D(-1.0,1.5,-1.5,1.5*(GLfloat)h/(GLfloat)w);//�����ά����ͶӰ����
    else
      gluOrtho2D(-1.0,1.5*(GLfloat)w/(GLfloat)h,-1.5,1.5);
    glMatrixMode(GL_MODELVIEW);//ָ����ǰ����ΪGL_MODELVIEW
}

#else

void myDisplay(void) {}
void myReshape(GLsizei w,GLsizei h) {

}
int test_opengl()
{
  /*��ʼ��*/
  glutInit(0, 0);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowSize(400,400);
  glutInitWindowPosition(200,200);
  
  /*��������*/
  glutCreateWindow("Triangle");
  
  /*��������ʾ*/
  background();
  glutReshapeFunc(myReshape);
  glutDisplayFunc(myDisplay);
  
  glutMainLoop();
  return(0);
}
#endif
