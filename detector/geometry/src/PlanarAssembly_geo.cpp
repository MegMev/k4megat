//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
//
//==========================================================================
#include <DD4hep/DetElement.h>
#include <DD4hep/Handle.h>
#include <DD4hep/Objects.h>
#include <DD4hep/Shapes.h>
#include <DD4hep/Volumes.h>
#include <DD4hep/detail/SegmentationsInterna.h>
#include <XML/XML.h>
#include <XML/XMLTags.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens) {
    // typedef pair<Assembly, double> AssemblyPair;
    typedef pair<Volume, double> VolumePair;

    xml_det_t x_det(e);
    string det_name = x_det.nameStr();
    // default fill material for assemble envelopes
    Material chamber_gas = description.material(x_det.materialStr());
    Assembly det_env(det_name);
    PlacedVolume pv;
    map<string, Volume> modules;
    map<string, Position> mod_dims;
    DetElement sdet(det_name, x_det.id());
    SensitiveDetector sd = sens;
    int n_sensitive = 0;

    /// Set visualization, limits and region (if present)
    det_env.setRegion(description, x_det.regionStr());
    det_env.setLimitSet(description, x_det.limitsStr());
    det_env.setVisAttributes(description, x_det.visStr());

    /// module loop
    for (xml_coll_t mi(x_det, _U(module)); mi; ++mi) {
        xml_comp_t x_mod = mi;
        string m_nam = x_mod.nameStr();
        double x = x_mod.x();
        double y = x_mod.y();
        xml_coll_t ci(x_mod, _U(component));
        double dd, half_z, total_thickness = 0.;
        for (ci.reset(); ci; ++ci) {
            total_thickness += xml_comp_t(ci).thickness();
        }
        half_z = total_thickness / 2;
        Volume mod_vol = Volume(m_nam, Box(x / 2, y / 2, half_z), chamber_gas);
        mod_vol.setVisAttributes(description.invisible());

        for (ci.reset(), dd = half_z; ci; ++ci) {
            xml_comp_t c = ci;
            string c_name = c.nameStr();
            double c_thick = c.thickness();
            Material c_mat = description.material(c.materialStr());
            Volume c_vol = Volume(c_name, Box(x / 2, y / 2, c_thick / 2), c_mat);
            c_vol.setVisAttributes(description.visAttributes(c.visStr()));

            // first component is on the top
            pv = mod_vol.placeVolume(c_vol, Position(0, 0, dd - c_thick / 2));

            if (c.isSensitive()) {
                c_vol.setSensitiveDetector(sens);
                n_sensitive++;
            }
            dd -= c_thick;
        }
        modules[m_nam] = mod_vol;
        mod_dims[m_nam] = {x, y, total_thickness};
    }

    ///
    if (n_sensitive) {
        sd.setType("calorimeter");
    }

    /// section loop
    for (xml_coll_t si(x_det, _U(section)); si; ++si) {
        xml_comp_t x_sec = si;
        string sec_name = x_sec.nameStr();
        // default layer gap
        double l_gap = x_sec.hasAttr(_U(gap)) ? x_sec.gap() : 0;
        vector<VolumePair> layers;
        Position sec_pos(0, 0, 0);
        RotationZYX sec_rot(0, 0, 0);
        if (x_sec.hasChild(_U(position))) {
            xml_dim_t dim = x_sec.position();
            sec_pos = {dim.x(), dim.y(), dim.z()};
        }
        if (x_sec.hasChild(_U(rotation))) {
            xml_dim_t dim = x_sec.rotation();
            sec_rot = {dim.z(), dim.y(), dim.x()};
        }

        /// layer loop
        int layer_id = 0;  // initial index
        for (xml_coll_t li(x_sec, _U(layer)); li; ++li) {
            xml_comp_t x_layer = li;
            string m_name = x_layer.moduleStr();
            int nrepeat = x_layer.hasAttr(_U(repeat)) ? x_layer.repeat() : 1;
            // override default layer gap if specified
            if (x_layer.hasAttr(_U(gap))) {
                l_gap = x_layer.gap();
            }

            //
            xml_dim_t x_row = x_layer.child(_U(row));
            int nrow = x_row.nmodules();
            double row_gap = x_row.hasAttr(_U(gap)) ? x_row.gap() : 0;
            xml_dim_t x_col = x_layer.child(_Unicode(column));
            int ncol = x_col.nmodules();
            double col_gap = x_col.hasAttr(_U(gap)) ? x_col.gap() : 0;

            double mod_x = mod_dims[m_name].X();
            double mod_y = mod_dims[m_name].Y();
            double mod_z = mod_dims[m_name].Z();
            double row_ll = (nrow - 1) * row_gap + nrow * mod_x;
            double col_ll = (ncol - 1) * col_gap + ncol * mod_y;

            /// repeat loop
            for (int r_id = 0; r_id < nrepeat; ++r_id, ++layer_id) {
                Volume l_env(_toString(layer_id, "layer%d"), Box(row_ll / 2, col_ll / 2, mod_z / 2),
                             chamber_gas);
                l_env.setVisAttributes(description.invisible());

                double col_dd = -col_ll / 2.;
                for (int col_id = 0; col_id < ncol; col_id++) {
                    double row_dd = -row_ll / 2.;

                    Volume row_env(_toString(col_id, "row%d"), Box(row_ll / 2, mod_y / 2, mod_z / 2),
                                   chamber_gas);
                    for (int row_id = 0; row_id < nrow; row_id++) {
                        pv = row_env.placeVolume(modules[m_name], Position(row_dd + mod_x / 2, 0, 0));
                        pv.addPhysVolID("column", row_id);
                        row_dd += mod_x + row_gap;
                    }
                    pv = l_env.placeVolume(row_env, Position(0, col_dd + mod_y / 2, 0));
                    pv.addPhysVolID("row", col_id);
                    col_dd += mod_y + col_gap;
                }
                layers.emplace_back(l_env, mod_z + l_gap);
            }
        }

        /// place the layers
        Assembly sec_env(sec_name);
        sec_env.setVisAttributes(description.invisible());
        double l_dd = 0;
        layer_id = 0;
        for (auto vol : layers) {
            pv = sec_env.placeVolume(vol.first, Position(0, 0, l_dd - vol.second / 2));
            pv.addPhysVolID("layer", layer_id);
            l_dd -= vol.second;
            layer_id++;
        }

        /// place the section
        pv = det_env.placeVolume(sec_env, Transform3D(sec_rot, sec_pos));
        pv.addPhysVolID("section", x_sec.id());
        DetElement sec_de(sdet, sec_name, x_sec.id());
        sec_de.setPlacement(pv);
    }

    /// place the detector in mother volume
    Position pos_det(0, 0, 0);
    RotationZYX rot_det(0, 0, 0);
    if (x_det.hasChild(_U(position))) {
        xml_dim_t dim = x_det.position();
        pos_det = {dim.x(), dim.y(), dim.y()};
    }
    if (x_det.hasChild(_U(rotation))) {
        xml_dim_t rot = x_det.rotation();
        rot_det = {rot.z(), rot.y(), rot.x()};
    };
    Volume mother = description.pickMotherVolume(sdet);
    pv = mother.placeVolume(det_env, Transform3D(rot_det, pos_det));
    if (x_det.hasAttr(_U(id))) {
        pv.addPhysVolID("system", x_det.id());
    }
    sdet.setPlacement(pv);
    return sdet;
}

DECLARE_DETELEMENT(Megat_PlanarAssembly, create_element)
