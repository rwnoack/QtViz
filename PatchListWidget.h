#ifndef PATCHLISTWIDGET_H
#define PATCHLISTWIDGET_H

#include <QtGui>


class PatchListWidget:public QListWidget
{
  public:

  PatchListWidget(QWidget* parent)
    :QListWidget(parent)
  {
  }

  ~PatchListWidget(){};
    
};

class PatchListWidgetItem: public QListWidgetItem
{
public:
  PatchListWidgetItem(const QString & text, PatchListWidget* parent,int type)
    :QListWidgetItem(text,parent,type)
    , _patch_id(0)
  {
  }
  void set(int patch_id) { _patch_id = patch_id; }
  int  get(void) const { return _patch_id; }

private:
  PatchListWidgetItem();
  int _patch_id;
};


#endif /*ndef PATCHLISTWIDGET_H */
