#ifndef REGIONINPUT_H
#define REGIONINPUT_H

#include <iostream>
#include <QtGui>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QSlider>

#include "IntTypes.h"


typedef enum { BLK_FACE_IMIN,
               BLK_FACE_IMAX,
               BLK_FACE_JMIN,
               BLK_FACE_JMAX,
               BLK_FACE_KMIN,
               BLK_FACE_KMAX} BlkFaceIndex;

class StrVizMenu;
class QtViz;

class RegionInput: public QObject
{
  Q_OBJECT

public:
  RegionInput(BlkFaceIndex face, StrVizMenu* viz_menu,QtViz* qtviz);

  QLayout* getLayout(void) { return _h_layout; }

  int  get(void) const { return _value; }
  void set(int v);

  void setRange(int n) {
    _value_SpinBox->setRange(1,n);
    _slider->setRange(1,n);
  }
  void setValue(int val,bool setting_slave=false) {
    std::cout << "RegionInput setValue " << val << std::endl;
    _value = val;
    _value_SpinBox->setValue(val);
    _slider->setValue(val);
    if (nullptr != _slider_slave && ! setting_slave) {
      _slider_slave->setValue(val,true);
    }

  }
  void slaveWith(RegionInput* othr, int const_dir);
  void slaveRelease(void);

public slots:
    void action_setValue(int);
private:
  int _value;
  BlkFaceIndex _face;

  StrVizMenu* _viz_menu;
  QtViz* _qtviz;

  QPushButton* _btn;
  QSpinBox*    _value_SpinBox;
  QHBoxLayout* _h_layout;
  QSlider*     _slider;
  RegionInput* _slider_slave;

  RegionInput();
};
#endif /*ndef REGIONINPUT_H*/
