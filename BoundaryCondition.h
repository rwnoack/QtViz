#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include <string>
#include <sstream>

class BoundaryCondition
{
 public:
  BoundaryCondition(const std::string& name, const std::string& bc)
    : _name(name)
    , _bc(bc)
  {
    _label = _name + ";" + _bc;
  }

  BoundaryCondition(const std::string& name,
                    const std::string& bc,
                    const std::string& suggar_bc)
    : _name(name)
    , _bc(bc)
    , _suggar_bc(suggar_bc)
  {
    _label = _name + ";" + _bc;
    _label = _name + ";" + _bc + ";" + _suggar_bc;
  }

  ~BoundaryCondition() {}
  const std::string&     getName(void) const { return _name; }
  const std::string&       getBC(void) const { return _bc; }
  const std::string& getSuggarBC(void) const { return _suggar_bc; }

  virtual const std::string& getLabel(void) const { return _label; }
  void setLabel(std::string& lbl) { _label = lbl; }

 protected:
  std::string _label;
  std::string _name;
  std::string _bc;
  std::string _suggar_bc;
 private:
  BoundaryCondition();

};
class UnstrBoundaryCondition: public BoundaryCondition
{
 public:
 UnstrBoundaryCondition(int patch_id,const std::string& name, const std::string& bc)
    : BoundaryCondition(name, bc)
    , _patch_id(patch_id)
  {
    std::stringstream ss;
    ss << "Patch="<< _patch_id << ";" << _name << ";" << _bc;

    _label = ss.str();
  }
 UnstrBoundaryCondition(int patch_id,const std::string& name,
                        const std::string& bc,const std::string& suggar_bc)
   : BoundaryCondition(name, bc,suggar_bc)
    , _patch_id(patch_id)
  {
    std::stringstream ss;
    ss << "Patch="<< _patch_id << ";" << _name << ";" << _bc;
    ss << ";" << _suggar_bc;

    _label = ss.str();
  }

  ~UnstrBoundaryCondition();
  int getPatchID(void) const { return _patch_id; }
 private:
  int _patch_id;

  UnstrBoundaryCondition();
};
class StrBoundaryCondition: public BoundaryCondition
{
 public:
 StrBoundaryCondition(int imin,int jmin,int kmin,
                      int imax,int jmax,int kmax,
                      std::string& name, std::string& bc)
    : BoundaryCondition(name, bc)
    , _imin(imin),_jmin(jmin),_kmin(kmin)
    , _imax(imax),_jmax(jmax),_kmax(kmax)
  {
    std::stringstream ss;
    ss << "("
       << _imin << "," << _imax << ":"
       << _jmin << "," << _jmax << ":"
       << _kmin << "," << _kmax << ")"
       << ";" << _name << ";" << _bc;

    _label = ss.str();
  }


  ~StrBoundaryCondition();
  void getRegion(int& imin,int& jmin,int& kmin,
                 int& imax,int& jmax,int& kmax) const {
    imin = _imin;    jmin = _jmin;    kmin = _kmin;
    imax = _imax;    jmax = _jmax;    kmax = _kmax;
  }
 private:
  int _imin,_jmin,_kmin, _imax,_jmax,_kmax;
  StrBoundaryCondition();
};

#endif /*ndef BOUNDARYCONDITION_H*/
