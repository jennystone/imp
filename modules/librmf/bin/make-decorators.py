#!/usr/bin/python
def get_string(type, name, const, per_frame=False):
    if per_frame:
        pfs="true"
    else:
        pfs="false"
    if const:
        return """(fh.get_has_key<%(type)sTraits>(cat, \"%(name)s\")?
                   fh.get_key<%(type)sTraits>(cat, \"%(name)s\"):%(type)sKey())"""%{ "name":name,
                                                                                     "type": type,
                                                                                     "pfs":pfs}
    else:
        return """get_key_always<%(type)sTraits>(fh, cat,
                               \"%(name)s\", %(pfs)s)"""%{ "name":name,
                                                  "type": type,
                                                  "pfs":pfs}

class Attribute:
    def __init__(self, type, nice_name, attribute_name):
        self.type=type
        self.nice_name=nice_name
        self.attribute_name=attribute_name
    def get_key_members(self):
        return self.type+"Key "+self.nice_name+"_;"
    def get_methods(self, const):
        ret=[]
        ret.append(self.type+" get_"+self.nice_name+"(int frame=0) const {")
        ret.append("  return nh_.get_value("+self.nice_name+"_, frame);")
        ret.append("}")
        if not const:
            ret.append("void set_"+self.nice_name+"("+self.type+" v, int frame=0) {")
            ret.append("   nh_.set_value("+self.nice_name+"_, v, frame);")
            ret.append("}")
        return "\n".join(ret)
    def get_key_arguments(self):
        return self.type+"Key "+self.nice_name
    def get_key_pass(self):
        return self.nice_name+"_"
    def get_key_saves(self):
        return self.nice_name+"_("+self.nice_name+")"
    def get_initialize(self, const):
        return self.nice_name+"_="+get_string(self.type, self.attribute_name, const)
    def get_check(self):
        return "nh.get_has_value("+self.nice_name+"_)"

class SingletonRangeAttribute:
    def __init__(self, type, nice_name, attribute_name_begin, attribute_name_end):
        self.type=type
        self.nice_name=nice_name
        self.attribute_name_begin=attribute_name_begin
        self.attribute_name_end=attribute_name_end
    def get_key_members(self):
        return "boost::array<"+self.type+"Key,2> "+self.nice_name+"_;"
    def get_methods(self, const):
        ret=[]
        ret.append(self.type+" get_"+self.nice_name+"(int frame=0) const {")
        ret.append("  return nh_.get_value("+self.nice_name+"_[0], frame);")
        ret.append("}")
        if not const:
            ret.append("void set_"+self.nice_name+"("+self.type+" v, int frame=0) {")
            ret.append("   nh_.set_value("+self.nice_name+"_[0], v, frame);")
            ret.append("   nh_.set_value("+self.nice_name+"_[0], v, frame);")
            ret.append("}")
        return "\n".join(ret)
    def get_key_arguments(self):
        return "boost::array<"+self.type+"Key, 2> "+self.nice_name
    def get_key_pass(self):
        return self.nice_name+"_"
    def get_key_saves(self):
        return self.nice_name+"_("+self.nice_name+")"
    def get_initialize(self, const):
        return self.nice_name+"_[0]="+get_string(self.type, self.attribute_name_begin, const)+\
            ";\n"+self.nice_name+"_[1]="+get_string(self.type, self.attribute_name_end, const)
    def get_check(self):
        return "nh.get_has_value("+self.nice_name+"_[0])"+\
            "\n  && nh.get_has_value("+self.nice_name+"_[1])"+\
            "\n  && nh.get_value("+self.nice_name+"_[0])==nh.get_value("+self.nice_name+"_[1])"


class Attributes:
    def __init__(self, type, ptype, nice_name, attribute_names, pf=False):
        self.type=type
        self.nice_name=nice_name
        self.ptype=ptype
        self.attribute_names=attribute_names
        self.per_frame=pf
    def get_key_members(self):
        return self.type+"Keys "+self.nice_name+"_;"
    def get_methods(self, const):
        ret=[]
        ret.append("""%(ptype)s get_%(name)s(int frame=0) const {
       %(ptype)s ret;
       for (unsigned int i=0; i< %(len)s; ++i) {
          ret.push_back(nh_.get_value(%(key)s[i], frame));
       }
       return ret;
    }"""%{"type":self.type,
          "ptype":self.ptype,
          "name":self.nice_name,
          "len":len(self.attribute_names),
          "key":self.nice_name+"_"})
        if not const:
            ret.append("""void set_%(name)s(const %(ptype)s &v, int frame=0) {
         for (unsigned int i=0; i< %(len)s; ++i) {
            nh_.set_value(%(key)s[i], v[i], frame);
         }
      }"""%{"type":self.type,
            "ptype":self.ptype,
            "name":self.nice_name,
            "len":len(self.attribute_names),
            "key":self.nice_name+"_"})
        return "\n".join(ret)
    def get_key_arguments(self):
        return self.type+"Keys "+self.nice_name
    def get_key_pass(self):
        return self.nice_name+"_"
    def get_key_saves(self):
        return self.nice_name+"_("+self.nice_name+")"
    def get_initialize(self, const):
        ret=[]
        for n in self.attribute_names:
            ret.append("""      %(name)s_.push_back(%(get)s);"""%{"type":self.type,
                                                            "name":self.nice_name,
                                                            "len":len(self.attribute_names),
                                                            "key":self.nice_name+"_",
                                                            "get":get_string(self.type, n, const, self.per_frame)
                                                            })
        return "\n".join(ret)
    def get_check(self):
        return "nh.get_has_value("+self.nice_name+"_[0])"


class DecoratorCategory:
    def __init__(self, category, arity,
                 attributes):
        self.category=category
        self.arity=arity
        self.attributes=attributes

class Decorator:
    def __init__(self, name, description,
                 decorator_categories,
                 check_function):
        self.name=name
        self.description=description
        self.categories= decorator_categories
        self.check_function= check_function
    def _get_key_members(self):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_key_members())
        return "\n".join(ret)
    def _get_methods(self, const):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_methods(const))
        return "\n".join(ret)
    def _get_key_arguments(self):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_key_arguments())
        return ",\n".join(ret)
    def _get_key_pass(self):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_key_pass())
        return ",\n".join(ret)
    def _get_key_saves(self):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_key_saves())
        return ",\n".join(ret)
    def _get_checks(self):
        ret=[]
        for cd in self.categories:
            for a in cd.attributes:
                ret.append(a.get_check())
        return "\n    && ".join(ret)
    def _get_initialize(self, const):
        ret=[]
        for cd in self.categories:
            # make handle missing later
            lhs="{\n  CategoryD<"+str(cd.arity)\
                         +"> cat="
            if const:
                rhs="fh.get_category<"+str(cd.arity)+">(\""\
                    +cd.category+"\");"
            else:
                rhs="get_category_always<"+str(cd.arity)+">(fh, \""\
                    +cd.category+"\");"
            ret.append(lhs+rhs)
            for a in cd.attributes:
                ret.append(a.get_initialize(const)+";")
            ret.append("}")
        return "\n".join(ret)
    def get(self):
        ret=[]
        classstr="""/** %(description)s

       \see %(name)s%(NOTCONST)s
       \see %(name)s%(CONST)sFactory
     */
    class %(name)s%(CONST)s {
    Node%(CONST)sHandle nh_;
    friend class %(name)s%(CONST)sFactory;
    private:
    %(key_members)s
    %(name)s%(CONST)s(Node%(CONST)sHandle nh,
                  %(key_arguments)s): nh_(nh),%(key_saves)s {
    %(check)s;
    }
    public:
    %(methods)s
    IMP_RMF_SHOWABLE(Const%(name)s, "%(name)s%(CONST)s " << nh_.get_name());
    ~%(name)s%(CONST)s() {
    %(check)s;
    }
    };

    typedef vector<%(name)s%(CONST)s> %(name)s%(CONST)ss;
    """
        ret.append(classstr%{"description":self.description,
                             "name":self.name,
                             "key_members": self._get_key_members(),
                             "methods": self._get_methods(True),
                             "key_arguments": self._get_key_arguments(),
                             "key_saves": self._get_key_saves(),
                             "CONST":"Const", "NOTCONST":"",
                             "check":self.check_function})
        ret.append(classstr%{"description":self.description,
                             "name":self.name,
                             "key_members": self._get_key_members(),
                             "methods": self._get_methods(False),
                             "key_arguments": self._get_key_arguments(),
                             "key_saves": self._get_key_saves(),
                             "CONST":"", "NOTCONST":"Const",
                             "check":self.check_function})
        factstr="""/** Create decorators of type %(name)s.

       \see %(name)s%(CONST)s
       \see %(name)s%(NOTCONST)sFactory
    */
    class %(name)s%(CONST)sFactory {
    private:
    %(key_members)s
    public:
    IMP_RMF_SHOWABLE(%(name)s%(CONST)sFactory, "Factory");
    %(name)s%(CONST)sFactory(File%(CONST)sHandle fh){
    %(initialize)s;
    }
    %(name)s%(CONST)s get(Node%(CONST)sHandle nh) const {
      return %(name)s%(CONST)s(nh, %(key_pass)s);
    }
    bool get_is(Node%(CONST)sHandle nh) const {
      return %(checks)s;
    }
    };

    typedef vector<%(name)s%(CONST)sFactory> %(name)s%(CONST)sFactories;
    """
        ret.append(factstr%{"name":self.name,
                             "key_members": self._get_key_members(),
                             "key_pass": self._get_key_pass(),
                             "CONST":"", "NOTCONST":"Const",
                            "initialize": self._get_initialize(False),
                            "checks":self._get_checks()});
        ret.append(factstr%{"name":self.name,
                             "key_members": self._get_key_members(),
                             "key_pass": self._get_key_pass(),
                             "CONST":"Const", "NOTCONST":"",
                            "initialize": self._get_initialize(True),
                            "checks":self._get_checks()});
        return "\n".join(ret)


colored= Decorator("Colored", "These particles has associated color information.",
                   [DecoratorCategory("shape", 1, [Attributes("Float", "Floats",
                                                              "rgb_color", ["rgb color red",
                                                                            "rgb color green",
                                                                            "rgb color blue"])])],
                   "")
particle= Decorator("Particle", "These particles has associated coordinates and radius information.",
                   [DecoratorCategory("physics", 1, [Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"], True),
                                                     Attribute("Float", "radius", "radius"),
                                                     Attribute("Float", "mass", "mass")])],
                   "")
pparticle= Decorator("RigidParticle", "These particles has associated coordinates and radius information.",
                   [DecoratorCategory("physics", 1, [Attributes("Float", "Floats",
                                                                "orientation", ["orientation r",
                                                                                "orientation i",
                                                                                "orientation j",
                                                                                "orientation k"], True),
                                                     Attributes("Float", "Floats",
                                                                "coordinates", ["cartesian x",
                                                                                "cartesian y",
                                                                                "cartesian z"], True),
                                                     Attribute("Float", "radius", "radius"),
                                                     Attribute("Float", "mass", "mass")])],
                   "")

score= Decorator("Score", "Associate a score with some set of particles.",
                   [DecoratorCategory("feature", 1, [Attribute("NodeIDs",
                                                               "representation",
                                                               "representation"),
                                                     Attribute("Float", "score", "score")])],
                   "")

ball= Decorator("Ball", "A geometric ball.",
                   [DecoratorCategory("shape", 1, [Attributes("Float", "Floats",
                                                              "coordinates", ["cartesian x",
                                                                              "cartesian y",
                                                                              "cartesian z"]),
                                                   Attribute("Float", "radius", "radius")])],
                   "")
cylinder= Decorator("Cylinder", "A geometric cylinder.",
                   [DecoratorCategory("shape", 1, [Attributes("Floats", "FloatsList",
                                                              "coordinates", ["cartesian xs",
                                                                              "cartesian ys",
                                                                              "cartesian zs"]),
                                                   Attribute("Float", "radius", "radius")])],
                   "")

journal= Decorator("JournalArticle", "Information regarding a publication.",
                   [DecoratorCategory("publication", 1, [Attribute("String", "title", "title"),
                                                         Attribute("String", "journal", "journal"),
                                                         Attribute("String", "pubmed_id", "pubmed id"),
                                                         Attribute("Int", "year", "year"),
                                                         Attribute("Strings", "authors", "authors"),])],
                   "")

residue= Decorator("Residue", "Information regarding a residue.",
                   [DecoratorCategory("sequence", 1, [SingletonRangeAttribute("Int", "index", "first residue index", "last residue index"),
                                                         Attribute("String", "type", "residue type")])],
                   "")

print """/**
 *  \file RMF/decorators.h
 *  \brief Helper functions for manipulating RMF files.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPLIBRMF_DECORATORS_H
#define IMPLIBRMF_DECORATORS_H

#include "RMF_config.h"
#include "infrastructure_macros.h"
#include "NodeHandle.h"
#include "FileHandle.h"
namespace RMF {
"""
print colored.get()
print particle.get()
print pparticle.get()
print score.get()
print ball.get()
print cylinder.get()
print journal.get()
print residue.get()
print """} /* namespace RMF */

#endif /* IMPLIBRMF_DECORATORS_H */"""
