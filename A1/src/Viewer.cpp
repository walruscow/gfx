// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>
// #include <GL/gl.h>
#include <GL/glu.h>


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
  : QGLWidget(format, parent)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
  , mVertexArrayObject(this)
#else
  , mVertexBufferObject(QGLBuffer::VertexBuffer)
#endif
{
  mTimer = new QTimer(this);
  connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
  mTimer->start(1000/30);
}

Viewer::~Viewer() {

}

void Viewer::setMode(DrawMode mode) {
  switch (mode) {
    case DrawMode::WIRE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case DrawMode::FACE:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
  }
}

QSize Viewer::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
  return QSize(300, 600);
}

void Viewer::initializeGL() {
  QGLFormat glFormat = QGLWidget::format();
  if (!glFormat.sampleBuffers()) {
    std::cerr << "Could not enable sample buffers." << std::endl;
    return;
  }

  glClearColor(0.7, 0.7, 1.0, 0.0);

  if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
    std::cerr << "Cannot load vertex shader." << std::endl;
    return;
  }

  if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
    std::cerr << "Cannot load fragment shader." << std::endl;
    return;
  }

  if (!mProgram.link()) {
    std::cerr << "Cannot link shaders." << std::endl;
    return;
  }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  mVertexArrayObject.create();
  mVertexArrayObject.bind();

  mVertexBufferObject.create();
  mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else
  /*
   * if qt version is less than 5.1, use the following commented code
   * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
   * QOpenGLBuffer.
   */
  uint vao;

  typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
  typedef void (APIENTRY *_glBindVertexArray) (GLuint);

  _glGenVertexArrays glGenVertexArrays;
  _glBindVertexArray glBindVertexArray;

  glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()
    ->getProcAddress("glGenVertexArrays");
  glBindVertexArray = (_glBindVertexArray) QGLWidget::context()
    ->getProcAddress("glBindVertexArray");

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  mVertexBufferObject.create();
  mVertexBufferObject.setUsagePattern(QGLBuffer::StaticDraw);
#endif

  if (!mVertexBufferObject.bind()) {
    std::cerr << "could not bind vertex buffer to the context." << std::endl;
    return;
  }

  mVertexBufferObject.allocate(cubeCoords, sizeof(cubeCoords));

  mProgram.bind();

  mProgram.enableAttributeArray("vert");
  mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

  // mPerspMatrixLocation = mProgram.uniformLocation("cameraMatrix");
  mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
}

void Viewer::paintGL() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
  mVertexArrayObject.bind();
#endif

  QMatrix4x4 wellTransform;
  // Top left
  wellTransform.translate(-6, 11, 0);

  auto cameraMatrix = getCameraMatrix();
  // 20 deep and 10 across is 21 down, 11 right, 20 up
  for (int i = 0; i < 21; ++i) {
    wellTransform.translate(0, -1, 0);
    mProgram.setUniformValue(mMvpMatrixLocation, cameraMatrix * wellTransform);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
  }
  for (int i = 0; i < 11; ++i) {
    wellTransform.translate(1, 0, 0);
    mProgram.setUniformValue(mMvpMatrixLocation, cameraMatrix * wellTransform);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
  }
  for (int i = 0; i < 20; ++i) {
    wellTransform.translate(0, 1, 0);
    mProgram.setUniformValue(mMvpMatrixLocation, cameraMatrix * wellTransform);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
  }
}

void Viewer::resizeGL(int width, int height) {
  if (height == 0) {
    height = 1;
  }

  mPerspMatrix.setToIdentity();
  mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

  glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent (QMouseEvent* event) {
  if (event->modifiers() & Qt::ShiftModifier) {
    scaling = true;
  }
  lastMouseX = event->x();
}

void Viewer::mouseReleaseEvent (QMouseEvent* event) {
  (void) event;
  scaling = false;
}

void Viewer::mouseMoveEvent (QMouseEvent* event) {
  int x = event->x();
  int dx = x - lastMouseX;

  if (scaling) {
    float scaleAmount = pow(scaleFactor, dx);
    scale *= scaleAmount;
    if (scale > MAX_SCALE) {
      scale = MAX_SCALE;
      scaleAmount = MAX_SCALE / scale;
    }
    else if (scale < MIN_SCALE) {
      scale = MIN_SCALE;
      scaleAmount = MIN_SCALE / scale;
    }
    scaleWorld(scaleAmount, scaleAmount, scaleAmount);
  }
  else {
    auto buttons = event->buttons();
    // 1 if appropriate button is held, else 0
    float xCoord = !!(buttons & Qt::LeftButton);
    float yCoord = !!(buttons & Qt::MidButton);
    float zCoord = !!(buttons & Qt::RightButton);
    rotateWorld(rotateFactor * dx, xCoord, yCoord, zCoord);
  }

  lastMouseX = x;
  lastDx = dx;
}

QMatrix4x4 Viewer::getCameraMatrix() {
  QMatrix4x4 vMatrix;

  QMatrix4x4 cameraTransformation;
  QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
  QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

  vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

  return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
  mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
  mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
  mTransformMatrix.scale(x, y, z);
}

void Viewer::resetView() {
  scale = 1.0f;
  mTransformMatrix.setToIdentity();
}
