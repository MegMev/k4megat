from __future__ import absolute_import, unicode_literals

# activate library loading
import cppyy.gbl as gbl
gbl.LoadMegat

# 
current_ns = __import__(__name__)

# helpers for importing items from megat::
def import_item(nam):
    """Import signatures from megat ns"""
    attr = getattr(gbl.megat, nam)
    setattr(current_ns, nam, attr)
    return attr

def import_ns_item(ns, nam):
    """Import signatures from a specific namespace"""
    scope = getattr(gbl.megat, ns)
    attr = getattr(scope, nam)
    setattr(current_ns, nam, attr)
    return attr

# import each sub namespace
core = gbl.megat
simcalo = import_item('SimCalo')

# import utility
import_ns_item('utility', 'loadGeometry')
import_ns_item('utility', 'IdConverter')
