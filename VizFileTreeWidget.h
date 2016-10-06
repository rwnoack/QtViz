#ifndef VIZFILETREEWIDGET_H
#define VIZFILETREEWIDGET_H

#include <QtGui>

class VizFile;
class grObj;

class VizFileTreeWidgetItem;
class VizFileTreeWidget:public QTreeWidget
{
  public:

  VizFileTreeWidget()
    :QTreeWidget()
  {
  }

  ~VizFileTreeWidget(){};
    
};

class VizFileTreeWidgetItem: public QTreeWidgetItem
{
public:
  VizFileTreeWidgetItem(VizFileTreeWidget* parent,
                        const QStringList& strings,int type)
    :QTreeWidgetItem(parent,strings,type)
    , _file(nullptr)
    , _obj(nullptr)
  {
  }
  VizFileTreeWidgetItem(VizFileTreeWidgetItem* parent,
                        const QStringList& strings,int type)
    :QTreeWidgetItem(parent,strings,type)
    , _file(nullptr)
    , _obj(nullptr)
  {
  }
  void set(VizFile* file) { _file = file; }
  void set(grObj* obj) { _obj = obj; }
  VizFile* getVizFile(void) { return _file; }
  grObj*   getObj(void)     { return _obj; }
  void addObj(grObj* obj,const char* const prefix,const char* const lbl,
              VizFileTreeWidgetItem* parent);


private:
  VizFile* _file;
  grObj*   _obj;
  VizFileTreeWidgetItem();
};


#endif /*ndef VIZFILETREEWIDGET_H */
