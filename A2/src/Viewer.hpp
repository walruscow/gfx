#pragma once

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMatrix4x4>
#include <QtGlobal>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "Model.hpp"
#include "ViewPoint.hpp"

class Viewer : public QGLWidget {

  Q_OBJECT

 public:
  Viewer(const QGLFormat& format, QWidget *parent = 0);
  virtual ~Viewer();

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  // Set the parameters of the current perspective projection using
  // the semantics of gluPerspective().
  void set_perspective(double fov, double aspect, double near, double far);

  // Restore all the transforms and perspective parameters to their
  // original state. Set the viewport to its initial size.
  void reset_view();

  enum class Mode {
    VIEW_ROTATE, VIEW_TRANSLATE, VIEW_PERSPECTIVE,
    MODEL_ROTATE, MODEL_TRANSLATE, MODEL_SCALE,
    VIEWPORT,
  };

 protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);

  // Draw a line -- call draw_init first!
  void draw_line(const QVector2D& p1, const QVector2D& p2) ;

  // Set the current colour
  void set_colour(const QColor& col);

  // Call this before you begin drawing. Width and height are the width
  // and height of the GL window.
  void draw_init();

 private:
  void scale(Model& model, int dx, bool L, bool M, bool R);
  void rotate(Movable& obj, int dx, bool L, bool M, bool R);
  void translate(Movable& obj, int dx, bool L, bool M, bool R);
  void changePerspective(int dx, bool L, bool M, bool R);

  static const double SCALE_FACTOR;
  static const double ROTATE_FACTOR;
  static const double TRANSLATE_FACTOR;
  static const double FOV_FACTOR;

  QOpenGLBuffer mVertexBufferObject;
  QOpenGLVertexArrayObject mVertexArrayObject;
  QGLShaderProgram mProgram;
  int mColorLocation;

  QTimer* refreshTimer;
  Mode mode = Mode::MODEL_ROTATE;
  int lastMouseX;

  Model boxModel = Model("Box", {
      // "Front"
      {{1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}},
      {{-1.0, 1.0, 1.0}, {-1.0, -1.0, 1.0}},
      {{-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}},
      {{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}},

      // "Back"
      {{1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}},
      {{-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0}},
      {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}},
      {{1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}},

      // Middle lines
      {{1.0, 1.0, 1.0}, {1.0, 1.0, -1.0}},
      {{-1.0, 1.0, 1.0}, {-1.0, 1.0, -1.0}},
      {{-1.0, -1.0, 1.0}, {-1.0, -1.0, -1.0}},
      {{1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}},
  });

  Model boxGnomon = Model("BoxGnomon", {
      {{0, 0, 0}, {1, 0, 0}},
      {{0, 0, 0}, {0, 1, 0}},
      {{0, 0, 0}, {0, 0, 1}}
  });

  Model worldGnomon = Model("WorldGnomon", {
      {{0, 0, 0}, {1, 0, 0}},
      {{0, 0, 0}, {0, 1, 0}},
      {{0, 0, 0}, {0, 0, 1}}
  });

  ViewPoint viewPoint;
};
