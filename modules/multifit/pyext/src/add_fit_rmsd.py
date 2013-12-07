#!/usr/bin/env python

__doc__ = "Add RMSD to reference to each fitting file."

import IMP.multifit
from IMP import OptionParser


def parse_args():
    usage =  """%prog [options] <asmb.input> <proteomics> <mapping> <align param>

Given a set of local fits (e.g. generated by fit_fft), the RMSD between each
subunit and a reference orientation is calculated and added to each fitting
file, in the final "RMSD to reference" column. (The original fitting file is
not modified; a new fitting file is created with a '.RMSD' extension.)

Note that the assembly input file must contain a reference PDB filename for
each subunit (in the rightmost column).
"""
    parser = OptionParser(usage)
    parser.add_option("-d", action="store_true", dest="use_dock",
                      help="if set the docking transformation is used (and not the fitting transformation)")
    (options, args) = parser.parse_args()
    if len(args) != 4:
        parser.error("incorrect number of arguments")
    return [options, args]


def run(asmb_fn, proteomics_fn, mapping_fn, params_fn, dock_trans):
    asmb = IMP.multifit.read_settings(asmb_fn)

    prot_data = IMP.multifit.read_proteomics_data(proteomics_fn)
    print "=========3"
    mapping_data = IMP.multifit.read_protein_anchors_mapping(
        prot_data, mapping_fn)
    alignment_params = IMP.multifit.AlignmentParams(params_fn)
    align = IMP.multifit.ProteomicsEMAlignmentAtomic(
        mapping_data, asmb, alignment_params)
    ensmb = IMP.multifit.Ensemble(asmb, mapping_data)
    print "=========6"
    # load all proteomics restraints
    mdl = align.get_model()
    mhs = align.get_molecules()
    gs = []
    for i, mh in enumerate(mhs):
        fits_fn = asmb.get_component_header(i).get_transformations_fn()
        fits = IMP.multifit.read_fitting_solutions(fits_fn)
        print "calculating rmsd for", len(fits), "fits of protein", mh.get_name()
        xyz = IMP.core.XYZs(IMP.core.get_leaves(mh))
        mh_ref = IMP.atom.read_pdb(
            asmb.get_component_header(i).get_reference_fn(),
            mdl)
        rb = IMP.atom.setup_as_rigid_body(mh_ref)
        xyz_ref = IMP.core.XYZs(IMP.core.get_leaves(mh_ref))
        new_fits = []
        for i, rec in enumerate(fits):
            fit_t = rec.get_fit_transformation()
            if dock_trans:
                fit_t = rec.get_dock_transformation()
            IMP.core.transform(rb, fit_t)
            rmsd = IMP.atom.get_rmsd(xyz, xyz_ref)
            rec.set_rmsd_to_reference(rmsd)
            new_fits.append(rec)
            IMP.core.transform(rb, fit_t.get_inverse())
        IMP.multifit.write_fitting_solutions(fits_fn + ".RMSD", new_fits)


def main():
    options, args = parse_args()
    run(args[0], args[1], args[2], args[3], options.use_dock)

if __name__ == "__main__":
    main()
