import IMP
import IMP.test
import IMP.atom
import IMP.domino
import sys
class PointAlignmentTests(IMP.test.TestCase):
    def setUp(self):
        IMP.test.TestCase.setUp(self)
        IMP.set_log_level(IMP.SILENT)#TERSE)#VERBOSE)
    def test_docking_solutions(self):
        """Test nested rigid bodies"""
        #load components
        mdl=IMP.Model()
        mhs=[]
        for i in range(4):
            fn ="small_protein.pdb"
            mhs.append(IMP.atom.read_pdb(self.get_input_file_name(fn),
                                         mdl,IMP.atom.CAlphaPDBSelector()))
            IMP.atom.setup_as_rigid_body(mhs[-1])
        rbs=IMP.core.RigidBodies(mhs)
        ts=[]
        bb= IMP.algebra.BoundingBox3D(
            IMP.algebra.Vector3D(-10.,-10.,-10.),
            IMP.algebra.Vector3D(10.,10.,10.))
        for i in range(5):
            ts.append(IMP.algebra.Transformation3D(
                IMP.algebra.get_random_rotation_3d(),
                IMP.algebra.get_random_vector_in(bb)))
        #set nesting
        for i in range(3):
            rbs[0].add_member(rbs[i+1])
        #set ev
        sev=IMP.atom.create_excluded_volume_restraint(mhs, 1)
        mdl.add_restraint(sev)
        mdl.evaluate(False)
        #set states
        pst=IMP.domino.ParticleStatesTable()
        for i in range(1):
            states=IMP.domino.NestedRigidBodyStates(ts)
            pst.set_particle_states(mhs[i+1],states)
        id_trans=[]
        id_trans.append(IMP.core.RigidBody(mhs[0]).get_reference_frame())
        pst.set_particle_states(mhs[0],IMP.domino.RigidBodyStates(id_trans))
        ds=IMP.domino.DominoSampler(mdl,pst)
        cg=ds.get_sample()
        print cg.get_number_of_configurations()

if __name__ == '__main__':
    IMP.test.main()
